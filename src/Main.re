open TsApi;
open Ts;
open Writer;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertTypeAliasDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let (writer, typeNames) =
    switch (node->TsNode.getType->TsType.getTypeKind) {
    | TypeKind.TypeLiteral =>
      node->createLiteralType(types, names, writer, typeNames)

    | _ => (
        writer
        ->writeBeginModuleFromType(node)
        ->writeNewLine
        ->write("type t = ")
        ->writeType(node->TsNode.getType, types)
        ->write(";")
        ->writeEndModule
        ->writeNewLine,
        typeNames,
      )
    };

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