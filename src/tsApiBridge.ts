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
    MethodSignature,
    PropertySignature,
    ParameterDeclaration,
    InterfaceDeclaration,
    ClassDeclaration,
    PropertyDeclaration,
    MethodDeclaration,
    FunctionTypeNode
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
    optional?: boolean;
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
    isGeneric?: boolean;
}

interface TsTypeParameter {
    name: string;
}

interface TsNode {
    ns?: string[];
    name?: string;
    kind: SyntaxKind;
    type: TsType;
    parameters: TsParameter[];
    isConst?: boolean;
    enumMembers?: TsEnumMember[];
    optional?: boolean;
}

type WithType = { type: TypeNode, name?: any; };

function normalizeNamespace(ns: string[]) {
    return ns.length == 0 ? [] : ns.slice(0, ns.length - 1);
}

function buildType(ns: string[], node: WithType, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsType {
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
            return buildType(ns, node.type as any, checker, tsNodes, typeParameters);

        case SyntaxKind.ArrayType:
            return buildArrayType(ns, node.type as any, checker, tsNodes, typeParameters);

        case SyntaxKind.TupleType:
            return buildTupleType(ns, node.type as any, checker, tsNodes, typeParameters);

        case SyntaxKind.TypeLiteral:
            return buildTypeLiteral(ns, node.type as any, checker, tsNodes, typeParameters);

        default:
            return buildRegularType(ns, node.type as any, checker, tsNodes, typeParameters);
    }
}

function buildTupleType(ns: string[], node: TupleTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.Tuple,
        tupleTypes: node.elementTypes.map(et => buildType(ns, { type: et }, checker, tsNodes, typeParameters))
    };
}

function buildArrayType(ns: string[], node: ArrayTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.Array,
        arrayType: buildType(ns, { type: node.elementType }, checker, tsNodes, typeParameters),
    };
}

function buildParameter(p: ParameterDeclaration, ns: string[], checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]) {
    return {
        name: p.name.getText(),
        type: buildType(ns, p as any, checker, tsNodes, typeParameters),
        optional: p.questionToken != null,
    };
}

function buildMember(ns: string[], node: any, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsNode {
    switch (node.kind) {
        case SyntaxKind.PropertySignature:
            const propSign = node as PropertySignature;
            // Skiping that cases for now.
            if (node.name!.getText().indexOf('[') == 0
                || node.name!.getText().indexOf('\'') == 0
                || node.name!.getText().indexOf('\"') == 0) {
                return null!;
            }

            if (node.type && node.type.kind == SyntaxKind.FunctionType) {
                const fnType = node.type as FunctionTypeNode;
                const fnTypeParameters: TsTypeParameter[] = !fnType.typeParameters ? [] : fnType.typeParameters.map(tp => ({ name: tp.name.getText() }))
                return {
                    ns: normalizeNamespace(ns),
                    name: node.name!.getText(),
                    kind: SyntaxKind.MethodSignature,
                    type: buildType(ns, fnType as any, checker, tsNodes, typeParameters.concat(fnTypeParameters)),
                    parameters: fnType
                        .parameters
                        .map(p => buildParameter(p, ns, checker, tsNodes, typeParameters.concat(fnTypeParameters))),
                };
            }

            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node as any, checker, tsNodes, typeParameters),
                optional: propSign.questionToken != null,
                parameters: [],
            };

        case SyntaxKind.MethodSignature:
            const methodSign = node as MethodSignature;
            // Skiping that cases for now.
            if (node.name!.getText().indexOf('[') == 0
                || node.name!.getText().indexOf('\'') == 0
                || node.name!.getText().indexOf('\"') == 0) {
                return null!;
            }

            const methodSignTypeParameters: TsTypeParameter[] = !methodSign.typeParameters ? [] : methodSign.typeParameters.map(tp => ({ name: tp.name.getText() }))
            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node as any, checker, tsNodes, typeParameters.concat(methodSignTypeParameters)),
                parameters: methodSign
                    .parameters
                    .map(p => buildParameter(p, ns, checker, tsNodes, typeParameters.concat(methodSignTypeParameters))),
            };

        case SyntaxKind.PropertyDeclaration:
            const propDecl = node as PropertyDeclaration;
            // Skiping that cases for now.
            if (node.name!.getText().indexOf('[') == 0
                || node.name!.getText().indexOf('\'') == 0
                || node.name!.getText().indexOf('\"') == 0) {
                return null!;
            }

            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node, checker, tsNodes, []),
                optional: propDecl.questionToken != null,
                parameters: [],
            };

        case SyntaxKind.MethodDeclaration:
            const methodDecl = node as MethodDeclaration;
            const methodDeclTypeParameters: TsTypeParameter[] = !methodDecl.typeParameters ? [] : methodDecl.typeParameters.map(tp => ({ name: tp.name.getText() }))

            // Skiping that cases for now.
            if (node.name!.getText().indexOf('[') == 0
                || node.name!.getText().indexOf('\'') == 0
                || node.name!.getText().indexOf('\"') == 0) {
                return null!;
            }

            return {
                ns: normalizeNamespace(ns),
                name: node.name!.getText(),
                kind: node.kind,
                type: buildType(ns, node, checker, tsNodes, typeParameters.concat(methodDeclTypeParameters)),
                parameters: methodDecl
                    .parameters
                    .map(p => buildParameter(p, ns, checker, tsNodes, typeParameters.concat(methodDeclTypeParameters))),
            };
    }
    return null!;
}

function buildTypeLiteral(ns: string[], node: TypeLiteralNode, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        typeKind: TypeKind.TypeLiteral,
        members: node.members.map(m => buildMember(ns, m, checker, tsNodes, typeParameters)).filter(m => m != null)
    };
}

function buildRegularType(ns: string[], node: TypeNode, checker: ts.TypeChecker, tsNodes: TsNode[], typeParameters: TsTypeParameter[]): TsType {
    return {
        ns: normalizeNamespace(ns),
        name: node.getText(),
        typeKind: TypeKind.Regular,
        isGeneric: typeParameters.filter(tp => tp.name == node.getText()).length > 0
    };
}

function buildTsNodeFromTypeAliasDeclaration(ns: string[], node: TypeAliasDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    const typeParameters: TsTypeParameter[] = !node.typeParameters ? [] : node.typeParameters.map(tp => ({ name: tp.name.getText() }));
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name.getText(),
        kind: node.kind,
        type: buildType(ns, node, checker, tsNodes, typeParameters),
        parameters: [],
    });
}

function buildTsNodeFromVariableDeclaration(ns: string[], node: VariableDeclaration, isConst: boolean, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name.getText(),
        kind: node.kind,
        type: buildType(ns, node as any, checker, tsNodes, []),
        parameters: [],
        isConst,
    });
}

function buildTsNodeFromFunctionDeclaration(ns: string[], node: FunctionDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    const typeParameters: TsTypeParameter[] = !node.typeParameters ? [] : node.typeParameters.map(tp => ({ name: tp.name.getText() }));
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name!.getText(),
        kind: node.kind,
        type: buildType(ns, node as any, checker, tsNodes, typeParameters),
        parameters: node
            .parameters
            .map(p => buildParameter(p, ns, checker, tsNodes, typeParameters)),
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

function buildTsNodeFromInterfaceDeclaration(ns: string[], node: InterfaceDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    const typeParameters: TsTypeParameter[] = !node.typeParameters ? [] : node.typeParameters.map(tp => ({ name: tp.name.getText() }));
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name!.getText(),
        kind: node.kind,
        type: {
            ns: normalizeNamespace(ns),
            typeKind: TypeKind.TypeLiteral,
            members: node.members.map(m => buildMember(ns, m, checker, tsNodes, typeParameters)).filter(m => m != null)
        },
        parameters: [],
    });
}

function buildTsNodeFromClassDeclaration(ns: string[], node: ClassDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    const typeParameters: TsTypeParameter[] = !node.typeParameters ? [] : node.typeParameters.map(tp => ({ name: tp.name.getText() }));
    tsNodes.push({
        ns: normalizeNamespace(ns),
        name: node.name!.getText(),
        kind: node.kind,
        type: {
            ns: normalizeNamespace(ns),
            typeKind: TypeKind.TypeLiteral,
            members: node.members.map(m => buildMember(ns, m, checker, tsNodes, typeParameters)).filter(m => m != null)
        },
        parameters: [],
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

            case SyntaxKind.InterfaceDeclaration:
                buildTsNodeFromInterfaceDeclaration([], node as InterfaceDeclaration, checker, tsNodes);
                break;

            case SyntaxKind.ClassDeclaration:
                buildTsNodeFromClassDeclaration([], node as ClassDeclaration, checker, tsNodes);
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
