import ts from 'typescript';
import {
    SyntaxKind,
    TypeAliasDeclaration,
    Program,
    TypeNode
} from 'typescript';
import * as fs from 'fs';
import * as path from 'path';
import { ArrayTypeNode } from 'ts-simple-ast';

interface TsParameter {
    name: string;
    type: TsType;
}

interface TsType {
    ns?: string[];
    name: string;
    isArray: boolean;
    arrayType: TsType;
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

function isArrayType(node: WithType) {
    return node.type.kind == SyntaxKind.ArrayType;
}

function getTypeName(node: WithType) {
    return node.type.kind == SyntaxKind.ArrayType
        ? undefined!
        : node.type.getText();
}

function buildTsNodeFromTypeAliasDeclaration(node: TypeAliasDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: [],
        name: node.name.getText(),
        kind: node.kind,
        type: isArrayType(node)
            ? buildArrayType(node.type as any, checker, tsNodes)
            : buildType(node.type as any, checker, tsNodes),
        returnType: undefined!,
        parameters: [],
    });
}

function buildArrayType(node: ArrayTypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    // TODO: this property `elementType` is not defined on typescript.d.ts. 
    // Maybe I'm using the wrong typescript version
    const withType: WithType = { type: (node as any).elementType } as any;

    return {
        ns: [],
        name: getTypeName(withType),
        isArray: true,
        arrayType: isArrayType(withType)
            ? buildArrayType(withType.type as any, checker, tsNodes)
            : buildType(withType.type as any, checker, tsNodes)
    };
}

function buildType(node: TypeNode, checker: ts.TypeChecker, tsNodes: TsNode[]): TsType {
    return {
        ns: [],
        name: node.getText(),
        isArray: false,
        arrayType: undefined!
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
