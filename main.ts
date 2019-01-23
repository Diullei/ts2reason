import {
    Project,
    InterfaceDeclaration,
    ClassDeclaration,
    EnumDeclaration,
    PropertySignature,
    PropertyDeclaration,
    MethodSignature,
    MethodDeclaration,
    VariableDeclaration,
    FunctionDeclaration,
    SyntaxKind,
    NamespaceDeclaration,
    SourceFile,
    Type,
    ParameterDeclaration
} from 'ts-simple-ast';

import * as os from 'os';

const _uniq = <T>(value: T, i: number, self: T[]) => self.indexOf(value) === i;
const uniq = <T>(arr: T[]) => arr.filter(_uniq).sort();

const capitalize = (text: string) => text ? `${text[0].toUpperCase()}${text.substr(1)}` : text;

const lowerCap = (text: string) => text ? `${text[0].toLowerCase()}${text.substr(1)}` : text;

const normalizeName = (text: string) => text
    .replace(/[\$|\.|\-]/g, '_')
    .replace(/["|']/g, '');

const globalNames: string[] = [];
const getAsUniqueName = (name: string): string => {
    name = lowerCap(name);
    name = [
        'sig',
        'module',
        'begin',
        'end',
        'object',
        'switch',
        'to',
        'then',
        'type',
        'as',
    ].filter(kw => name == kw).length > 0 ? `${name}_` : name;

    globalNames.push(name);

    const amount = globalNames.filter(mn => mn == name).length;
    if (amount > 1) {
        return name + amount;
    }
    return name;
};

type TypeKind = InterfaceDeclaration
    | ClassDeclaration
    | EnumDeclaration
    | PropertySignature
    | PropertyDeclaration
    | MethodSignature
    | MethodDeclaration
    | VariableDeclaration
    | FunctionDeclaration;

type TsNode = {
    ns: string[];
    id: string;
    kind: SyntaxKind;
    node: TypeKind;
};

class Writer {
    private _code = '';
    private _currentIdentation = 0;

    constructor(private _newLine: string, private _indentSize: number) { }

    write(s: string): void {
        this._code += s;
    }

    writeComment(s: string): void {
        this.write(`/* ${s} */`);
    }

    writeNewLine(): void {
        this.write(`${
            this._newLine}${
            Array(this._currentIdentation == 0
                ? 0
                : (this._currentIdentation * this._indentSize + 1)).join(' ')}`);
    }

    writeRawJs(s: string): void {
        this.write(`[%bs.raw {| ${s} |}]`);
    }

    writeReType(typ: TsNode): void {
        this.write(`t_${normalizeName(`${typ.ns}_${typ.id}`)}`);
    }

    writeType(nodeType: Type, types: TsNode[]): void {
        const typ = nodeType.getText();

        switch (typ) {
            case 'string':
                this.write('string');
                return;

            case 'boolean':
                this.write('bool');
                return;

            // TODO: how to define if it's supposed to be an int or float?
            case 'number':
                this.write('int');
                return;

            case 'any':
                this.write('\'a');
                return;
        }

        this.write('t_TODO');
    }

    writeArgumentsToMethodDecl(pars: ParameterDeclaration[], types: TsNode[]): void {
        this.write(`(_inst: t`);
        if (pars.length > 0) {
            pars.forEach(p => {
                this.write(`, _${p.getName()}: `);
                this.writeType(p.getType(), types);
            });
        }
        this.write(`)`);
    }

    writeArgumentsToFunctionDecl(pars: ParameterDeclaration[], types: TsNode[]): void {
        this.write(`(`);
        if (pars.length > 0) {
            pars.forEach((p, i) => {
                if (i > 0) {
                    this.write(`, `);
                }
                this.write(`~${p.getName()}: `);
                this.writeType(p.getType(), types);
            });
        }
        this.write(`)`);
    }

    writeArgumentsToFunctionCall(pars: ParameterDeclaration[]): void {
        this.write(`(`);
        if (pars.length > 0) {
            pars.forEach((p, i) => {
                if (i > 0) {
                    this.write(`, `);
                }
                this.write(`_${p.getName()}`);
            });
        }
        this.write(`)`);
    }

    writeModuleNameFrom(typ: TsNode): void {
        this.write(capitalize(normalizeName(typ.id)));
    }

    writeModuleName(ns: string[]): void {
        this.write(capitalize(normalizeName(ns.join('.'))));
    }

    writeGetPropertyDecl(typ: TsNode, types: TsNode[]): void {
        this.write(`let ${getAsUniqueName(`get${capitalize(typ.id)}`)} = (_inst: t): `);
        this.writeType(typ.node.getType(), types);
        this.write(` => [%bs.raw {| _inst.${typ.id} |}];`);
    }

    writeSetPropertyDecl(typ: TsNode, types: TsNode[]): void {
        this.write(`let ${getAsUniqueName(`set${capitalize(typ.id)}`)} = (_inst: t, _value: `);
        this.writeType(typ.node.getType(), types);
        this.write(`): `);
        this.writeType(typ.node.getType(), types);
        this.write(` => [%bs.raw {| _inst.${typ.id} = _value |}];`);
    }

    writePropertyDecls(typ: TsNode, types: TsNode[]): void {
        this.writeGetPropertyDecl(typ, types);
        this.writeNewLine();
        this.writeSetPropertyDecl(typ, types);
        this.writeNewLine();
    }

    writeMethodDecl(typ: TsNode, types: TsNode[]): void {
        this.write(`let ${getAsUniqueName(`${lowerCap(typ.id)}`)} = `);
        this.writeArgumentsToMethodDecl((typ.node as MethodSignature).getParameters(), types);
        this.write(`: `);
        this.writeType((typ.node as MethodSignature).getReturnType(), types);
        this.write(` => [%bs.raw {| _inst.${typ.id}`);
        this.writeArgumentsToFunctionCall((typ.node as MethodSignature).getParameters());
        this.write(` |}];`);
    }

    writeFunctionDecl(typ: TsNode, types: TsNode[], ns: string[]): void {
        this.write(`[@bs.module "${normalizeName(ns.join('.'))}"] external ${getAsUniqueName(`${lowerCap(typ.id)}`)}: `);
        this.writeArgumentsToFunctionDecl((typ.node as MethodSignature).getParameters(), types);
        this.write(` => `);
        this.writeType((typ.node as MethodSignature).getReturnType(), types);
        this.write(` = "${typ.id}";`);
    }

    writeVariableDecl(typ: TsNode, types: TsNode[], ns: string[]): void {
        this.write(`[@bs.module "${normalizeName(ns.join('.'))}"] external ${getAsUniqueName(`${lowerCap(typ.id)}`)}: `);
        this.writeType(typ.node.getType(), types);
        this.write(` = "${typ.id}";`);
    }

    writeBeginModuleFromNs(ns: string[]): void {
        this.writeNewLine();
        this.write(`module `);
        this.writeModuleName(ns);
        this.write(` = {`);
        this.increaseIndent();
    }

    writeBeginModuleFromType(typ: TsNode): void {
        this.writeNewLine();
        this.write(`module `);
        this.writeModuleNameFrom(typ);
        this.write(` = {`);
        this.increaseIndent();
    }

    writeEndModule(): void {
        this.decreaseIndent();
        this.writeNewLine();
        this.write(`}`);
    }

    writeAbstractTypeDeclaration(typ: TsNode): void {
        this.write('type t = ');
        this.writeReType(typ);
        this.write(';');
    }

    increaseIndent(): void {
        this._currentIdentation++;
    }

    decreaseIndent(): void {
        this._currentIdentation--;
    }

    getText(): string {
        return this._code;
    }
}

const tsProject = new Project();

const addType = (types: TsNode[], nsName: string[], node: TypeKind) => {
    return types.slice().concat([{
        ns: nsName,
        id: node.getName()!,
        kind: node.getKind(),
        node: node,
    }]);
}

const exptractTsTypes = (types: TsNode[], nsDecl: NamespaceDeclaration | SourceFile, prevNs: string[]) => {
    const nsName = nsDecl instanceof SourceFile ? [] : [nsDecl.getName()];
    const ns = prevNs.concat(nsName);

    nsDecl.getVariableDeclarations()
        .forEach(varDcl => types = addType(types, ns, varDcl));

    nsDecl.getFunctions()
        .forEach(funDecl => types = addType(types, ns, funDecl));

    nsDecl.getInterfaces()
        .forEach(interf => {
            types = addType(types, ns, interf);
            interf.getProperties().forEach(prop => types = addType(types, ns.concat([interf.getName()]), prop));
            interf.getMethods().forEach(method => types = addType(types, ns.concat([interf.getName()]), method));
        });

    nsDecl.getClasses()
        .forEach(clazz => {
            types = addType(types, ns, clazz);
            clazz.getProperties().forEach(prop => types = addType(types, ns.concat([clazz.getName()!]), prop));
            clazz.getMethods().forEach(method => types = addType(types, ns.concat([clazz.getName()!]), method));
        });

    nsDecl.getEnums().map(en => types = addType(types, ns, en));
    nsDecl.getNamespaces().forEach(subNs => types = exptractTsTypes(types, subNs, ns));

    return types;
}

const startsWithNs = (typNs: string[], ns: string[]) => ns.join('.').indexOf(typNs.join('.')) === 0

function writeVars(vars: TsNode[], declars: TsNode[], writer: Writer, ns: string[]) {
    vars.forEach(f => {
        writer.writeNewLine();
        writer.writeVariableDecl(f, declars, ns);
    });
};

function writeFuncs(funcs: TsNode[], declars: TsNode[], writer: Writer, ns: string[]) {
    funcs.forEach(f => {
        writer.writeNewLine();
        writer.writeFunctionDecl(f, declars, ns);
    });
};

function writeProps(props: TsNode[], declars: TsNode[], writer: Writer) {
    props.forEach(p => {
        writer.writeNewLine();
        writer.writeNewLine();
        writer.writePropertyDecls(p, declars);
    });
};

function writeMethods(methods: TsNode[], declars: TsNode[], writer: Writer) {
    methods.forEach(m => {
        writer.writeNewLine();
        writer.writeMethodDecl(m, declars);
    });
};

function writeModules(ns: string[], declars: TsNode[], writer: Writer) {
    const vars = declars
        .filter(t => t.kind === SyntaxKind.VariableDeclaration
            && startsWithNs(t.ns, ns));

    writeVars(vars, declars, writer, ns);

    const funcs = declars
        .filter(t => t.kind === SyntaxKind.FunctionDeclaration
            && startsWithNs(t.ns, ns));

    writeFuncs(funcs, declars, writer, ns);

    const moduleTypes = declars
        .filter(t => startsWithNs(t.ns, ns)
            && (t.kind === SyntaxKind.InterfaceDeclaration || t.kind === SyntaxKind.ClassDeclaration));

    moduleTypes.forEach(typ => {
        writer.writeNewLine();
        writer.writeBeginModuleFromType(typ);
        writer.writeNewLine();

        writer.writeAbstractTypeDeclaration(typ);

        const props = declars
            .filter(t => t.kind === SyntaxKind.PropertySignature
                && startsWithNs(t.ns, ns.concat([typ.id])));

        writeProps(props, declars, writer);

        const methods = declars
            .filter(t => t.kind === SyntaxKind.MethodSignature
                && startsWithNs(t.ns, ns.concat([typ.id])));

        writeMethods(methods, declars, writer);

        writer.writeEndModule()
    });
}

const convertToReason = (tsFile: string): string => {
    tsProject.addExistingSourceFiles(tsFile);
    const sourceFile = tsProject.getSourceFileOrThrow(tsFile);

    let declars: TsNode[] = [];
    declars = exptractTsTypes(declars, sourceFile, []);

    const writer = new Writer(os.EOL, 2);

    writer.writeComment(`** This file has been automatically generated by ts2reason **`);
    writer.writeNewLine();
    writer.writeNewLine();

    writer.write(`type t_TODO;`);
    writer.writeNewLine();

    const types = declars
        .filter(
            decl => decl.kind === SyntaxKind.InterfaceDeclaration
                || decl.kind === SyntaxKind.ClassDeclaration);

    types.forEach(typ => {
        writer.writeAbstractTypeDeclaration(typ);
        writer.writeNewLine();
    });

    const nsList = uniq(declars
        .filter(t =>
            (
                t.kind === SyntaxKind.ClassDeclaration
                || t.kind === SyntaxKind.InterfaceDeclaration
                || t.kind === SyntaxKind.VariableDeclaration
                || t.kind === SyntaxKind.MethodDeclaration
                || t.kind === SyntaxKind.FunctionDeclaration
            )
            && t.ns.length == 1
        ).map(t => t.ns));

    nsList.forEach(ns => {
        writer.writeBeginModuleFromNs(ns);
        writeModules(ns, declars.filter(dcl => dcl.ns.length >= 1), writer);
        writer.writeEndModule();
        writer.writeNewLine();
    });

    return writer.getText();
};

const typs = convertToReason('example/input.d.ts');
console.log(typs);
