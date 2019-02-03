import {
    SyntaxKind,
    TypeAliasDeclaration,
    Program,
    TypeNode,
    TupleTypeNode,
    ArrayTypeNode,
    VariableDeclaration,
    FunctionDeclaration,
    EnumDeclaration,
    TypeLiteralNode,
    MethodSignature
} from 'typescript';
import ts from 'typescript';
import * as fs from 'fs';
import * as path from 'path';

enum TypeKind {
    Regular = 0,
    Array = 1,
    Tuple = 2,
    Union = 3,
    Intersection = 4,
    TypeLiteral = 5,
}

interface TsParameter {
    name: string;
    type: TsType;
}

interface TsEnumMember {
    name: string;
}

interface TsType {
    ns?: string[];
    name?: string;
    typeKind: TypeKind;
    arrayType?: TsType;
    tupleTypes?: TsType[];
    members?: TsNode[];
}

interface TsNode {
    ns?: string[];
    name?: string;
    kind: SyntaxKind;
    type: TsType;
    parameters: TsParameter[];
    isConst?: boolean;
    enumMembers?: TsEnumMember[];
}

type WithType = { type: TypeNode, name?: any; };

function normalizeNamespace(ns: string[]) {
    return ns.length == 0 ? [] : ns.slice(0, ns.length - 1);
}

function buildType(ns: string[], node: WithType, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    ns = ns.concat(node.name ? [node.name.getText()] : []);

    if (!node.type) {
        return {
            ns: normalizeNamespace(ns),
            name: 'any',
            typeKind: TypeKind.Regular,
        };
    }

    switch (node.type.kind) {
        case SyntaxKind.ParenthesizedType:
            return buildType(ns, node.type as any, checker, tsNodes);

        case SyntaxKind.ArrayType:
            return buildArrayType(ns, node.type as any, checker, tsNodes);

        case SyntaxKind.TupleType:
            return buildTupleType(ns, node.type as any, checker, tsNodes);

        case SyntaxKind.TypeLiteral:
            return buildTypeLiteral(ns, node.type as any, checker, tsNodes);

        default:
            return buildRegularType(ns, node.type as any, checker, tsNodes);
    }
}

function buildTupleType(ns: string[], node: TupleTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.Tuple,
        tupleTypes: node.elementTypes.map(et => buildType(ns, { type: et }, checker, tsNodes))
    };
}

function buildArrayType(ns: string[], node: ArrayTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.Array,
        arrayType: buildType(ns, { type: node.elementType }, checker, tsNodes),
    };
}

function buildMember(ns: string[], node: ts.TypeElement, checker: ts.TypeChecker, tsNodes: TsNode[]): TsNode {
    switch (node.kind) {
        case SyntaxKind.PropertySignature:
            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node as any, checker, tsNodes),
                parameters: [],
            };

        case SyntaxKind.MethodSignature:
            const methodSign = node as MethodSignature;
            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node as any, checker, tsNodes),
                parameters: methodSign.parameters.map(p => ({ name: p.name.getText(), type: buildType(ns, p as any, checker, tsNodes) })),
            };
    }
    return null!;
}

function buildTypeLiteral(ns: string[], node: TypeLiteralNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.TypeLiteral,
        members: node.members.map(m => buildMember(ns, m, checker, tsNodes)).filter(m => m != null)
    };
}

function buildRegularType(ns: string[], node: TypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        name: node.getText(),
        typeKind: TypeKind.Regular,
    };
}

function buildTsNodeFromTypeAliasDeclaration(ns: string[], node: TypeAliasDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name.getText(),
        kind: node.kind,
        type: buildType(ns, node as any, checker, tsNodes),
        parameters: [],
    });
}

function buildTsNodeFromVariableDeclaration(ns: string[], node: VariableDeclaration, isConst: boolean, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name.getText(),
        kind: node.kind,
        type: buildType(ns, node as any, checker, tsNodes),
        parameters: [],
        isConst,
    });
}

function buildTsNodeFromFunctionDeclaration(ns: string[], node: FunctionDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name!.getText(),
        kind: node.kind,
        type: buildType(ns, node as any, checker, tsNodes),
        parameters: node.parameters.map(p => ({ name: p.name.getText(), type: buildType(ns, p as any, checker, tsNodes) })),
    });
}

function buildTsNodeFromEnumDeclaration(ns: string[], node: EnumDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name!.getText(),
        kind: node.kind,
        type: null!,
        parameters: [],
        enumMembers: node.members.map(p => ({ name: p.name.getText() })),
    });
}

function extractTypes(program: Program, filename: string): TsNode[] {
    let checker = program.getTypeChecker();
    let tsNodes: TsNode[] = [];

    for (const sourceFile of program.getSourceFiles()) {
        if (sourceFile.fileName == filename) {
            ts.forEachChild(sourceFile, visit);
        }
    }

    return tsNodes;

    let isConst = false;
    function visit(node: ts.Node) {
        switch (node.kind) {
            case SyntaxKind.TypeAliasDeclaration:
                buildTsNodeFromTypeAliasDeclaration([], node as TypeAliasDeclaration, checker, tsNodes);
                break;

            case SyntaxKind.VariableDeclarationList:
                if (node.getChildAt(0).kind === SyntaxKind.ConstKeyword) {
                    isConst = true;
                    ts.forEachChild(node, visit);
                    isConst = false;
                }
                else {
                    ts.forEachChild(node, visit);
                }
                break;

            case SyntaxKind.VariableDeclaration:
                buildTsNodeFromVariableDeclaration([], node as VariableDeclaration, isConst, checker, tsNodes);
                break;

            case SyntaxKind.FunctionDeclaration:
                if ((node as FunctionDeclaration).name) {
                    buildTsNodeFromFunctionDeclaration([], node as FunctionDeclaration, checker, tsNodes);
                }
                else {
                    ts.forEachChild(node, visit);
                }
                break;

            case SyntaxKind.EnumDeclaration:
                buildTsNodeFromEnumDeclaration([], node as EnumDeclaration, checker, tsNodes);
                break;

            default:
                ts.forEachChild(node, visit);
                break;
        }
    }
}

function loadLib() {
    return fs.readFileSync(path.join(path.dirname(require.resolve('typescript')), 'lib.d.ts')).toString();
}

function createCompilerHost(sourceCodeName: string, code: string): ts.CompilerHost {
    return {
        getSourceFile: function (filename: string) {
            if (filename === sourceCodeName) {
                return ts.createSourceFile(filename, code, ts.ScriptTarget.ES5);
            }
            else if (filename === "lib.d.ts") {
                return ts.createSourceFile(filename, loadLib(), ts.ScriptTarget.ES5);
            }
            return undefined;
        },
        getDefaultLibFileName: function () { return "lib.d.ts"; },
        useCaseSensitiveFileNames: function () { return false; },
        getCanonicalFileName: function (filename: string) { return filename; },
        getCurrentDirectory: function () { return ""; },
        getNewLine: function () { return "\n"; }
    } as any;
}

export function extractTypesFromCode(code: string) {
    const sourceCodeName = "file.d.ts";

    let program = ts.createProgram([sourceCodeName], {
        target: ts.ScriptTarget.ES5,
        module: ts.ModuleKind.CommonJS
    }, createCompilerHost(sourceCodeName, code));

    return extractTypes(program, sourceCodeName);
}

export function extractTypesFromFile(file: string) {
    let program = ts.createProgram([file], {
        target: ts.ScriptTarget.ES5,
        module: ts.ModuleKind.CommonJS
    });
    return extractTypes(program, file);
}
