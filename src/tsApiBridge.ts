import ts, { TupleTypeNode, ArrayTypeNode } from 'typescript';
import {
    SyntaxKind,
    TypeAliasDeclaration,
    Program,
    TypeNode
} from 'typescript';
import * as fs from 'fs';
import * as path from 'path';

enum TypeKind {
    Regular = 0,
    Array = 1,
    Tuple = 2,
    Union = 3,
    Intersection = 4,
}

interface TsParameter {
    name: string;
    type: TsType;
}

interface TsType {
    ns?: string[];
    name: string;
    typeKind: TypeKind;
    arrayType: TsType;
    tupleTypes: TsType[];
}

interface TsNode {
    ns: string[];
    name: string;
    kind: SyntaxKind;
    type: TsType;
    returnType: TsType;
    parameters: TsParameter[];
}

type WithType = { type: TypeNode };

function buildTsNodeFromTypeAliasDeclaration(node: TypeAliasDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: [],
        name: node.name.getText(),
        kind: node.kind,
        type: buildType(node as any, checker, tsNodes),
        returnType: undefined!,
        parameters: [],
    });
}

function buildType(node: WithType, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    switch (node.type.kind) {
        case SyntaxKind.ArrayType:
            return buildArrayType(node.type as any, checker, tsNodes);

        case SyntaxKind.TupleType:
            return buildTupleType(node.type as any, checker, tsNodes);

        default:
            return buildRegularType(node.type as any, checker, tsNodes);
    }
}

function buildTupleType(node: TupleTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: [],
        name: undefined!,
        typeKind: TypeKind.Tuple,
        arrayType: undefined!,
        tupleTypes: node.elementTypes.map(et => buildType({ type: et }, checker, tsNodes))
    };
}

function buildArrayType(node: ArrayTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: [],
        name: undefined!,
        typeKind: TypeKind.Array,
        // TODO: this property `elementType` is not defined on typescript.d.ts. 
        // Maybe I'm using the wrong typescript version
        arrayType: buildType({ type: node.elementType }, checker, tsNodes),
        tupleTypes: undefined!,
    };
}

function buildRegularType(node: TypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: [],
        name: node.getText(),
        typeKind: TypeKind.Regular,
        arrayType: undefined!,
        tupleTypes: undefined!,
    };
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

    function visit(node: ts.Node) {
        switch (node.kind) {
            case SyntaxKind.TypeAliasDeclaration:
                buildTsNodeFromTypeAliasDeclaration(node as TypeAliasDeclaration, checker, tsNodes);
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
