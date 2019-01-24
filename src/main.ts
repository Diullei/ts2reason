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
    ParameterDeclaration,
    TypeAliasDeclaration
} from 'ts-simple-ast';

import * as os from 'os';

const _uniq = <T>(value: T, i: number, self: T[]) => self.indexOf(value) === i;
const uniq = <T>(arr: T[]) => arr.filter(_uniq).sort();

const capitalize = (text: string) => text ? `${text[0].toUpperCase()}${text.substr(1)}` : text;

const lowerCap = (text: string) => text ? `${text[0].toLowerCase()}${text.substr(1)}` : text;

const normalizeName = (text: string) => text
    .replace(/[\$|\.|\-]/g, '_')
    .replace(/["|']/g, '');

const getAsUniqueName = (globalNames: string[], name: string): string => {
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

type TypeKind = NamespaceDeclaration
    | InterfaceDeclaration
    | ClassDeclaration
    | EnumDeclaration
    | PropertySignature
    | PropertyDeclaration
    | MethodSignature
    | MethodDeclaration
    | VariableDeclaration
    | FunctionDeclaration
    | TypeAliasDeclaration;

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
        this.write(`t_${normalizeName(`${(typ.ns.length > 0 ? typ.ns.join('_') + '_' : '')}${typ.id}`)}`);
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
                this.write('float');
                return;
        }

        // TODO: the code bellow need to be replaced to something more stable
        // it needs to have a better way to identify the type reference
        const tps = types.filter(tp => tp.id == typ);
        if (tps.length > 0) {
            this.writeReType(tps[0]);
            return;
        }

        this.write('t_TODO');
    }

    writeArgumentsToMethodDecl(pars: ParameterDeclaration[], types: TsNode[]): void {
        this.write(`t`);
        if (pars.length > 0) {
            pars.forEach(p => {
                this.write(` => `);
                this.writeType(p.getType(), types);
            });
        }
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

    writeTypeCtor(typ: TsNode, props: TsNode[], types: TsNode[]): void {
        this.write(`let make: `);

        this.write(`(`);
        if (props.length > 0) {
            props.forEach((p, i) => {
                if (i > 0) {
                    this.write(`, `);
                }
                this.write(`~${p.id}: `);
                this.writeType(p.node.getType(), types);
            });
        }
        this.write(`) => t = `);

        this.write(`[%bs.raw {| `);

        this.increaseIndent();
        this.writeNewLine();

        this.write(`function (`);

        this.increaseIndent();
        this.writeNewLine();

        if (props.length > 0) {
            props.forEach((p, i) => {
                if (i > 0) {
                    this.write(`, `);
                    this.writeNewLine();
                }
                this.write(`${p.id}`);
            });
        }

        this.decreaseIndent();
        this.writeNewLine();

        this.write(`) {`);

        this.increaseIndent();
        this.writeNewLine();

        this.write(`return {`);

        this.increaseIndent();
        this.writeNewLine();

        if (props.length > 0) {
            props.forEach((p, i) => {
                if (i > 0) {
                    this.write(`, `);
                    this.writeNewLine();
                }
                this.write(`${p.id}: ${p.id}`);
            });
        }

        this.decreaseIndent();
        this.writeNewLine();

        this.write(`}`);

        this.decreaseIndent();
        this.writeNewLine();

        this.write(`}`);

        this.decreaseIndent();
        this.writeNewLine();

        this.write(`|}]`);
    }

    writeModuleNameFrom(typ: TsNode): void {
        this.write(capitalize(normalizeName(typ.id)));
    }

    writeModuleName(ns: string[]): void {
        this.write(capitalize(normalizeName(ns[ns.length - 1])));
    }

    writeGetPropertyDecl(typ: TsNode, types: TsNode[], globalNames: string[]): void {
        this.write(`[@bs.get] external ${getAsUniqueName(globalNames, `get${capitalize(typ.id)}`)}: t => `);
        this.writeType(typ.node.getType(), types);
        this.write(` = "${typ.id}";`);
    }

    writeSetPropertyDecl(typ: TsNode, types: TsNode[], globalNames: string[]): void {
        this.write(`[@bs.send] external ${getAsUniqueName(globalNames, `set${capitalize(typ.id)}`)}: t => `);
        this.writeType(typ.node.getType(), types);
        this.write(` => unit = "${typ.id}";`);
    }

    writePropertyDecls(typ: TsNode, types: TsNode[], globalNames: string[]): void {
        this.writeGetPropertyDecl(typ, types, globalNames);
        this.writeNewLine();
        this.writeSetPropertyDecl(typ, types, globalNames);
        this.writeNewLine();
    }

    writeMethodDecl(typ: TsNode, types: TsNode[], globalNames: string[]): void {
        this.write(`[@bs.send.pipe: t] external ${getAsUniqueName(globalNames, `${lowerCap(typ.id)}`)}: `);
        this.writeArgumentsToMethodDecl((typ.node as MethodSignature).getParameters(), types);
        this.write(` => `);
        this.writeType((typ.node as MethodSignature).getReturnType(), types);
        this.write(` = "${typ.id}";`);
    }

    writeFunctionDecl(typ: TsNode, types: TsNode[], ns: string[], globalNames: string[]): void {
        this.write(`[@bs.module "${ns.map(n => normalizeName(n)).join('/')}"] external ${getAsUniqueName(globalNames, `${lowerCap(typ.id)}`)}: `);
        this.writeArgumentsToFunctionDecl((typ.node as MethodSignature).getParameters(), types);
        this.write(` => `);
        this.writeType((typ.node as MethodSignature).getReturnType(), types);
        this.write(` = "${typ.id}";`);
    }

    writeVariableDecl(typ: TsNode, types: TsNode[], ns: string[], globalNames: string[]): void {
        this.write(`[@bs.module "${ns.map(n => normalizeName(n)).join('/')}"] external ${getAsUniqueName(globalNames, `${lowerCap(typ.id)}`)}: `);
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

    if (nsDecl instanceof NamespaceDeclaration) {
        types = addType(types, ns, nsDecl);
    }

    nsDecl.getVariableDeclarations()
        .forEach(varDcl => types = addType(types, ns, varDcl));

    nsDecl.getFunctions()
        .forEach(funDecl => types = addType(types, ns, funDecl));

    nsDecl.getTypeAliases()
        .forEach(talias => types = addType(types, ns, talias));

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

const startsWithNs = (ns1: string[], ns2: string[]) => ns2.join('.').indexOf(ns1.join('.')) === 0

const compareNs = (typNs: string[], ns: string[]) => JSON.stringify(typNs) === JSON.stringify(ns);

function writeVars(vars: TsNode[], declars: TsNode[], writer: Writer, ns: string[], globalNames: string[]) {
    vars.forEach(f => {
        writer.writeNewLine();
        writer.writeVariableDecl(f, declars, ns, globalNames);
    });
};

function writeFuncs(funcs: TsNode[], declars: TsNode[], writer: Writer, ns: string[], globalNames: string[]) {
    funcs.forEach(f => {
        writer.writeNewLine();
        writer.writeFunctionDecl(f, declars, ns, globalNames);
    });
};

function writeProps(props: TsNode[], declars: TsNode[], writer: Writer, globalNames: string[]) {

    props.forEach(p => {
        writer.writeNewLine();
        writer.writePropertyDecls(p, declars, globalNames);
    });
};

function writeMethods(methods: TsNode[], declars: TsNode[], writer: Writer, globalNames: string[]) {
    methods.forEach(m => {
        writer.writeNewLine();
        writer.writeMethodDecl(m, declars, globalNames);
    });
};

function writeModules(ns: string[], declars: TsNode[], writer: Writer, globalNames: string[]) {
    const vars = declars
        .filter(t => t.kind === SyntaxKind.VariableDeclaration
            && compareNs(t.ns, ns));

    writeVars(vars, declars, writer, ns, globalNames);

    const funcs = declars
        .filter(t => t.kind === SyntaxKind.FunctionDeclaration
            && compareNs(t.ns, ns));

    writeFuncs(funcs, declars, writer, ns, globalNames);

    const moduleTypes = declars
        .filter(t => compareNs(t.ns, ns)
            && (
                t.kind === SyntaxKind.InterfaceDeclaration
                || t.kind === SyntaxKind.ClassDeclaration
            )
        );

    moduleTypes.forEach(typ => {
        const moduleGlobalNames: string[] = [];

        writer.writeNewLine();
        writer.writeBeginModuleFromType(typ);
        writer.writeNewLine();

        writer.writeAbstractTypeDeclaration(typ);
        writer.writeNewLine();

        const props = declars
            .filter(t => t.kind === SyntaxKind.PropertySignature
                && compareNs(t.ns, ns.concat([typ.id])));

        writeProps(props, declars, writer, moduleGlobalNames);

        const methods = declars
            .filter(t => t.kind === SyntaxKind.MethodSignature
                && compareNs(t.ns, ns.concat([typ.id])));

        writeMethods(methods, declars, writer, moduleGlobalNames);

        // NOTE: in case of an object with just properties we can create a constructor
        if (methods.length == 0) {
            writer.writeNewLine();
            writer.writeNewLine();

            writer.writeTypeCtor(typ, props, declars);
        }

        writer.writeEndModule();
    });

    let nss = uniq(
        declars
            .filter(t => checkValidSyntaxKind(t) && t.ns.length == ns.length + 1 && startsWithNs(ns, t.ns))
            .map(t => t.ns));

    nss.forEach(ns => {
        writer.writeNewLine();

        writer.writeBeginModuleFromNs(ns);
        writeModules(ns, declars, writer, []);
        writer.writeEndModule();
    });
}

const checkValidSyntaxKind = (t: TsNode) => (
    t.kind === SyntaxKind.ClassDeclaration
    || t.kind === SyntaxKind.InterfaceDeclaration
    || t.kind === SyntaxKind.VariableDeclaration
    || t.kind === SyntaxKind.MethodDeclaration
    || t.kind === SyntaxKind.FunctionDeclaration
    || t.kind === SyntaxKind.TypeAliasDeclaration
);

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
                || decl.kind === SyntaxKind.ClassDeclaration
                || decl.kind === SyntaxKind.TypeAliasDeclaration);


    types.forEach((typ, i) => {
        if (i > 0) {
            writer.writeNewLine();
        }
        writer.write('type ');
        writer.writeReType(typ);
        writer.write(';');
    });

    [[]].forEach(ns => {
        writeModules(ns, declars, writer, []);
        writer.writeNewLine();
    });

    return writer.getText();
};

const typs = convertToReason(process.argv[2]);
console.log(typs);
