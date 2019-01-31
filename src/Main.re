open TsApi;
open Ts;
open Writer;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let createTypeAssignDeclaration =
    (writer: writerState, node: TsNode.t, typeNames: list(string)) => {
  let name = "t_" ++ Utils.capitalize(node->TsNode.getName);
  (
    writer->write("type t = ")->write(name)->write(";"),
    [name, ...typeNames],
  );
};

let createTypeDeclaration =
    (writer: writerState, node: TsNode.t, types: array(TsNode.t)) =>
  writer
  ->write("type t = ")
  ->writeType(node->TsNode.getType, types)
  ->write(";");

let createGetName = (node: TsNode.t, names: list(string)) =>
  ("get" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names);

let createSetName = (node: TsNode.t, names: list(string)) =>
  ("set" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names);

let createUniqueName = (name: string, names: list(string)) =>
  name->Utils.lowerFirst->Utils.toUniqueName(names);

let createMemberGetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) = node->createGetName(names);

  (
    writer
    ->write({j|[@bs.get] external $name: (t) => |j})
    ->writeType(node->TsNode.getType, types)
    ->write(" = \"")
    ->write(node->TsNode.getName)
    ->write("\";"),
    names,
  );
};

let createMemberSetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) = node->createSetName(names);

  (
    writer
    ->write({j|[@bs.send] external $name: (t, |j})
    ->writeType(node->TsNode.getType, types)
    ->write(") => ")
    ->writeType(node->TsNode.getType, types)
    ->write(" = \"")
    ->write(node->TsNode.getName)
    ->write("\";"),
    names,
  );
};

let createGetSetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (writer, names) = writer->createMemberGetFunction(node, types, names);
  let writer = writer->writeNewLine;
  let (writer, names) = writer->createMemberSetFunction(node, types, names);
  (writer, names);
};

let convertTypeAliasDeclarationType =
    (
      writer: writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      _names: list(string),
      typeNames: list(string),
    ) =>
  switch (node->TsNode.getType->TsType.getTypeKind) {
  | TypeKind.TypeLiteral =>
    let (writer, typeNames) =
      writer->createTypeAssignDeclaration(node, typeNames);
    (
      writer->(
                writer =>
                  node->TsNode.getType->TsType.getMembers
                  |> Array.fold_left(
                       ((writer, memberNames), member) =>
                         writer
                         ->writeNewLine
                         ->writeNewLine
                         ->createGetSetFunction(member, types, memberNames),
                       (writer, []),
                     )
                  |> (((s, _)) => s)
              ),
      typeNames,
    );
  | _ => (writer->createTypeDeclaration(node, types), typeNames)
  };

let convertTypeAliasDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let writer = writer->writeBeginModuleFromType(node)->writeNewLine;
  let (writer, typeNames) =
    writer->convertTypeAliasDeclarationType(node, types, names, typeNames);
  let writer = writer->writeEndModule->writeNewLine;
  (writer, typeNames, names);
};

let convertVariableDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let (valName, names) = node->TsNode.getName->createUniqueName(names);
  let (setName, names) = node->createSetName(names);
  (
    writer
    ->write("[@bs.val] external ")
    ->write(valName)
    ->write(": ")
    ->writeType(node->TsNode.getType, types)
    ->write(" = \"")
    ->write(node->TsNode.getName)
    ->write("\";")
    ->writeNewLine
    |> (
      writer =>
        /* the set function will be generated if the
           declaration is not a 'const' declaration */
        if (!node->TsNode.isConst) {
          writer
          ->write("let ")
          ->write(setName)
          ->write(" = (_value: ")
          ->writeType(node->TsNode.getType, types)
          ->write("): ")
          ->writeType(node->TsNode.getType, types)
          ->write(" => [%bs.raw {| ")
          ->write(node->TsNode.getName)
          ->write(" = _value |}];")
          ->writeNewLine;
        } else {
          writer;
        }
    ),
    typeNames,
    names,
  );
};

let convertFunctionDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let (name, names) = node->TsNode.getName->createUniqueName(names);
  let writer =
    writer
    ->write("[@bs.send] external ")
    ->write(name)
    ->write(": ")
    ->writeTypeArgumentsToFunctionDecl(node->TsNode.getParameters, types)
    ->write(" => ")
    ->writeType(node->TsNode.getType, types)
    ->write(" = \"")
    ->write(node->TsNode.getName)
    ->write("\";")
    ->writeNewLine;

  (writer, typeNames, names);
};

let convertEnumDeclaration =
    (
      node: TsNode.t,
      _types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let writer = writer->writeBeginModuleFromType(node)->writeNewLine;

  let (writer, typeNames) =
    writer->createTypeAssignDeclaration(node, typeNames);

  (
    writer
    ->writeNewLine
    ->(
        writer =>
          node->TsNode.getEnumMembers
          |> Array.fold_left(
               ((writer, keyNames), enum) => {
                 let (name, keyNames) =
                   enum->TsEnumMember.getName->createUniqueName(keyNames);
                 (
                   writer
                   ->writeNewLine
                   ->write("let ")
                   ->write(name)
                   ->write(": t = [%bs.raw {| ")
                   ->write(Utils.capitalize(node->TsNode.getName))
                   ->write(".")
                   ->write(enum->TsEnumMember.getName)
                   ->write(" |}];"),
                   keyNames,
                 );
               },
               (writer, ["name_", "fromName_"]),
             )
          |> (((s, _)) => s)
      )
    /* Enum helper functions. We are not writing as a continous
       string because we need to keep the indentations */
    ->writeNewLine
    ->writeNewLine
    ->write("let name_ = (_key: t): option(string) =>")
    ->increaseIndent
    ->writeNewLine
    ->write("switch ([%bs.raw {| EnumTyp[_key] |}]) {")
    ->writeNewLine
    ->write("| Some(v) => Some(v)")
    ->writeNewLine
    ->write("| _ => None")
    ->writeNewLine
    ->write("};")
    ->decreaseIndent
    ->writeNewLine
    ->writeNewLine
    ->write("let fromName_ = (_name: string): option(t) =>")
    ->increaseIndent
    ->writeNewLine
    ->write("switch ([%bs.raw {| EnumTyp[_name] |}]) {")
    ->writeNewLine
    ->write("| Some(v) => Some(v)")
    ->writeNewLine
    ->write("| _ => None")
    ->writeNewLine
    ->write("};")
    ->decreaseIndent
    ->writeEndModule
    ->writeNewLine,
    typeNames,
    names,
  );
};

let convertCodeToReason = (code: string, writer: writerState) => {
  let types = TsApi.extractTypesFromCode(code);
  let (writer, typeNames) =
    types
    |> Array.fold_left(
         ((writer, typeNames, names), node) =>
           switch (node->TsNode.getKind) {
           | SyntaxKind.TypeAliasDeclaration =>
             node->convertTypeAliasDeclaration(
               types,
               names,
               writer,
               typeNames,
             )

           | SyntaxKind.VariableDeclaration =>
             node->convertVariableDeclaration(types, names, writer, typeNames)

           | SyntaxKind.FunctionDeclaration =>
             node->convertFunctionDeclaration(types, names, writer, typeNames)

           | SyntaxKind.EnumDeclaration =>
             node->convertEnumDeclaration(types, names, writer, typeNames)

           | _ => (
               writer->write("/* ** !INVALID NODE! ** */"),
               typeNames,
               names,
             )
           },
         (writer, [], []),
       )
    |> (((writer, typeNames, _)) => (writer, typeNames));

  let headerWriter =
    make(
      ~nl=writer.nl,
      ~code="",
      ~currentIdentation=writer.currentIdentation,
    );

  let headerWriter =
    typeNames
    |> List.fold_left(
         (headerWriter, typeName) =>
           headerWriter->write({j|type $typeName;|j})->writeNewLine,
         headerWriter,
       );
  headerWriter->getCode ++ writer->getCode;
};