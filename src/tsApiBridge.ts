import * as ts from 'typescript';
import {
    SyntaxKind,
    TypeAliasDeclaration,
    CompilerOptions,
    Program
} from 'typescript';
import * as fs from 'fs';
import * as path from 'path';

type TypeKind = TypeAliasDeclaration;

interface TsNode {
    ns?: string[];
    id?: string;
    kind: SyntaxKind;
    node: TypeKind;
}

function buildtsNodeFromTypeAliasDeclaration(node: TypeAliasDeclaration, checker: ts.TypeChecker, tsNodes: TsNode[]) {
    tsNodes.push({
        ns: [],
        id: node.name.getText(),
        kind: node.kind,
        node
    });
}

function extractTypes(program: Program): TsNode[] {
    let checker = program.getTypeChecker();
    let tsNodes: TsNode[] = [];

    for (const sourceFile of program.getSourceFiles()) {
        if (sourceFile.isDeclarationFile) {
            ts.forEachChild(sourceFile, visit);
        }
    }

    return tsNodes;

    function visit(node: ts.Node) {
        switch (node.kind) {
            case SyntaxKind.TypeAliasDeclaration:
                buildtsNodeFromTypeAliasDeclaration(node as TypeAliasDeclaration, checker, tsNodes);
                break;
        }
    }
}

export function extractTypesFromFile(file: string) {
    let program = ts.createProgram([file], {
        target: ts.ScriptTarget.ES5,
        module: ts.ModuleKind.CommonJS
    });
    return extractTypes(program);
}

export function extractTypesFromCode(code: string) {
    const libSource = fs.readFileSync(path.join(path.dirname(require.resolve('typescript')), 'lib.d.ts')).toString();

    const compilerHost = {
        getSourceFile: function (filename: string, languageVersion: any) {
            if (filename === "file.ts") {
                return ts.createSourceFile(filename, code, ts.ScriptTarget.ES5);
            }
            else if (filename === "lib.d.ts") {
                return ts.createSourceFile(filename, libSource, ts.ScriptTarget.ES5);
            }
            return undefined;
        },
        writeFile: function (name: string, text: string, writeByteOrderMark: any) { },
        getDefaultLibFileName: function () { return "lib.d.ts"; },
        useCaseSensitiveFileNames: function () { return false; },
        getCanonicalFileName: function (filename: string) { return filename; },
        getCurrentDirectory: function () { return ""; },
        getNewLine: function () { return "\n"; }
    };

    let program = ts.createProgram(["file.ts"], {
        target: ts.ScriptTarget.ES5,
        module: ts.ModuleKind.CommonJS
    }, compilerHost as any);

    return extractTypes(program);
}
