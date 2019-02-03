open TsApi;
open Ts;
open Writer;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertTypeAliasDeclaration =
    (
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      writer: writerState,
      typeNamesToPutInTheHead: list(string),
    ) => {
  let (writer, typeNamesToPutInTheHead, disambiguate) =
    switch (node->TsNode.getType->TsType.getTypeKind) {
    | TypeKind.TypeLiteral =>
      createLiteralType(
        node->TsNode.getNs,
        node->TsNode.getName,
        node->TsNode.getType,
        tsNodes,
        disambiguate,
        writer,
        typeNamesToPutInTheHead,
        true,
      )
      |> (
        ((writer, typeNamesToPutInTheHead, _, disambiguate)) => (
          writer,
          typeNamesToPutInTheHead,
          disambiguate,
        )
      )

    | _ =>
      let (typeStr, disambiguate, complementWriter) =
        writer->buildType(
          node->TsNode.getName,
          node->TsNode.getType,
          tsNodes,
          disambiguate,
          setupWriterAs(writer),
        );

      let (normalizedName, disambiguate) =
        Utils.createModuleName(
          node->TsNode.getNs,
          node->TsNode.getName,
          disambiguate,
        );

      (
        writer
        ->writeIf(complementWriter->hasContent, complementWriter->getCode, "")
        ->write("module ")
        ->write(normalizedName)
        ->write(" = {")
        ->increaseIndent
        ->writeNewLine
        ->write({j|type t = $typeStr;|j})
        ->decreaseIndent
        ->writeNewLine
        ->write("}")
        ->writeNewLine,
        typeNamesToPutInTheHead,
        disambiguate,
      );
    };

  (writer, typeNamesToPutInTheHead, disambiguate);
};

let convertVariableDeclaration =
    (
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      writer: writerState,
      typeNamesToPutInTheHead: list(string),
    ) => {
  let (valName, disambiguate) =
    node->TsNode.getName->createUniqueName(disambiguate);
  let (setName, disambiguate) = node->createSetName(disambiguate);
  let (typeStr, disambiguate, complementWriter) =
    writer->buildType(
      node->TsNode.getName,
      node->TsNode.getType,
      tsNodes,
      disambiguate,
      setupWriterAs(writer),
    );

  (
    writer
    ->writeIf(
        complementWriter->hasContent,
        complementWriter->writeNewLine->getCode,
        "",
      )
    ->write("[@bs.val] external ")
    ->write(valName)
    ->write({j|: $typeStr = "|j})
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
          ->write({j| = (_value: $typeStr): $typeStr => [%bs.raw {| |j})
          ->write(node->TsNode.getName)
          ->write(" = _value |}];")
          ->writeNewLine;
        } else {
          writer;
        }
    ),
    typeNamesToPutInTheHead,
    disambiguate,
  );
};

let convertFunctionDeclaration =
    (
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      writer: writerState,
      typeNamesToPutInTheHead: list(string),
    ) => {
  let (writer, disambiguate) =
    writer->writeFunctionDeclaration(node, tsNodes, disambiguate);
  (writer, typeNamesToPutInTheHead, disambiguate);
};

let convertEnumDeclaration =
    (
      node: TsNode.t,
      disambiguate: list(string),
      writer: writerState,
      typeNamesToPutInTheHead: list(string),
    ) => {
  let (normalizedName, disambiguate) =
    Utils.createModuleName(
      node->TsNode.getNs,
      node->TsNode.getName,
      disambiguate,
    );

  let writer =
    writer
    ->write("module ")
    ->write(normalizedName)
    ->write(" = {")
    ->increaseIndent
    ->writeNewLine;

  let name = "t_" ++ Utils.capitalize(node->TsNode.getName);
  let (writer, typeNamesToPutInTheHead) = (
    writer->write("type t = ")->write(name)->write(";"),
    [name, ...typeNamesToPutInTheHead],
  );

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
    ->decreaseIndent
    ->writeNewLine
    ->write("}")
    ->writeNewLine,
    typeNamesToPutInTheHead,
    disambiguate,
  );
};

let convertCodeToReason = (code: string, writer: writerState) => {
  let tsNodes = TsApi.extractTypesFromCode(code);
  let (writer, typeNamesToPutInTheHead) =
    tsNodes
    |> Array.fold_left(
         ((writer, typeNamesToPutInTheHead, disambiguate), node) => {
           let (writer, typeNamesToPutInTheHead, disambiguate) =
             switch (node->TsNode.getKind) {
             | SyntaxKind.TypeAliasDeclaration =>
               node->convertTypeAliasDeclaration(
                 tsNodes,
                 disambiguate,
                 writer,
                 typeNamesToPutInTheHead,
               )

             | SyntaxKind.VariableDeclaration =>
               node->convertVariableDeclaration(
                 tsNodes,
                 disambiguate,
                 writer,
                 typeNamesToPutInTheHead,
               )

             | SyntaxKind.FunctionDeclaration =>
               node->convertFunctionDeclaration(
                 tsNodes,
                 disambiguate,
                 writer,
                 typeNamesToPutInTheHead,
               )

             | SyntaxKind.EnumDeclaration =>
               node->convertEnumDeclaration(
                 disambiguate,
                 writer,
                 typeNamesToPutInTheHead,
               )

             | _ => (
                 writer->write("/* ** !INVALID NODE! ** */"),
                 typeNamesToPutInTheHead,
                 disambiguate,
               )
             };
           (writer->writeNewLine, typeNamesToPutInTheHead, disambiguate);
         },
         (writer, [], []),
       )
    |> (
      ((writer, typeNamesToPutInTheHead, _)) => (
        writer,
        typeNamesToPutInTheHead,
      )
    );

  let headerWriter = setupWriterAs(writer);

  let headerWriter =
    typeNamesToPutInTheHead
    |> List.fold_left(
         (headerWriter, typeName) =>
           headerWriter->write({j|type $typeName;|j})->writeNewLine,
         headerWriter,
       );

  (
    if (headerWriter->hasContent) {
      headerWriter->writeNewLine->getCode;
    } else {
      "";
    }
  )
  ++ writer->getCode;
};