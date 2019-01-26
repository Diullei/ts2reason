import * as ts from "typescript";
import {
    SyntaxKind,
    TypeAliasDeclaration,
    CompilerOptions
} from 'typescript';

type TypeKind = TypeAliasDeclaration;

interface TsNode {
    ns?: string[];
    id?: string;
    kind: SyntaxKind;
    node: TypeKind;
}

function buildtsNodeFromTypeAliasDeclaration(node: TypeAliasDeclaration, tsNodes: TsNode[]) {
    // ...
}

function extractTypes(fileNames: string[], options: CompilerOptions): TsNode[] {
    let program = ts.createProgram(fileNames, options);
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
                buildtsNodeFromTypeAliasDeclaration(node as TypeAliasDeclaration, tsNodes);
                break;
        }
    }
}

export function getTypes(file: string) {
    return extractTypes([file], {
        target: ts.ScriptTarget.ES5,
        module: ts.ModuleKind.CommonJS
    });
}
