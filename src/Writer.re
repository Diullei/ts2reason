open TsApi;

type writerState = {
  nl: string,
  code: string,
  currentIdentation: int,
};

let make = (~nl: string, ~code: string, ~currentIdentation: int) => {
  nl,
  code,
  currentIdentation,
};

let write = (writer: writerState, text: string): writerState => {
  ...writer,
  code: writer.code ++ text,
};

let increaseIndent = (writer: writerState) => {
  ...writer,
  currentIdentation: writer.currentIdentation + 1,
};

let decreaseIndent = (writer: writerState) => {
  ...writer,
  currentIdentation: writer.currentIdentation - 1,
};

let getCode = (writer: writerState) => writer.code;

let hasContent = (writer: writerState) =>
  writer->getCode |> String.length > 0;

let setupWriterAs = (writer: writerState) =>
  make(~nl=writer.nl, ~code="", ~currentIdentation=writer.currentIdentation);

let writeComment = (writer: writerState, text: string) =>
  writer->write({j|/* $text */|j});

let writeNewLine = (writer: writerState) =>
  writer->write(writer.nl)
  |> (state => Utils.makeIndent(state.currentIdentation * 2) |> state->write);

let writeRawJs = (writer: writerState, text: string) =>
  writer->write({j|[%bs.raw {| $text |}]|j});

let writeReasonType = (writer: writerState, typ: TsNode.t) => {
  let namespaceSplit =
    switch (typ->TsNode.getNs) {
    | [||] => ""
    | _ => "_"
    };

  writer
  ->write("t_")
  ->write(typ->TsNode.getNs |> Utils.createNamespaceName)
  ->write(namespaceSplit)
  ->write(typ->TsNode.getName);
};

let writeIf =
    (writer: writerState, condition: bool, thenText: string, elseText: string) =>
  if (condition) {
    writer->write(thenText);
  } else {
    writer->write(elseText);
  };

let rec buildType =
        (
          currentWriter: writerState,
          typeLabel: string,
          tsType: TsType.t,
          tsNodes: array(TsNode.t),
          disambiguate: list(string),
          complementWriter: writerState,
        )
        : (string, list(string), writerState) => {
  let writer = setupWriterAs(currentWriter);

  switch (tsType->TsType.getTypeKind) {
  | TypeKind.Array =>
    writer->writeArrayType(
      typeLabel,
      tsType,
      tsNodes,
      disambiguate,
      complementWriter,
    )

  | TypeKind.Tuple =>
    writer->writeTupleType(
      typeLabel,
      tsType,
      tsNodes,
      disambiguate,
      complementWriter,
    )

  | TypeKind.TypeLiteral =>
    let tName = Utils.capitalize(Utils.normalizeName(typeLabel));
    let complement = setupWriterAs(writer);
    let (writer, _, normalizedName, disambiguate) =
      createLiteralType(
        tsType->TsType.getNs,
        tName,
        tsType,
        tsNodes,
        disambiguate,
        complement,
        [],
        false,
      );
    (normalizedName ++ ".t", disambiguate, writer);
  | _ =>
    switch (writer->writePredefinedType(tsType)) {
    | Some(state) => (state->getCode, disambiguate, complementWriter)
    | None => (
        writer->writeReferenceType(tsType, tsNodes)->getCode,
        disambiguate,
        complementWriter,
      )
    }
  };
}
and writeArrayType =
    (
      currentWriter: writerState,
      typeLabel: string,
      tsType: TsType.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      complementWriter: writerState,
    ) => {
  let (typeStr, disambiguate, complementWriter) =
    buildType(
      currentWriter,
      typeLabel,
      tsType->TsType.getArrayType,
      tsNodes,
      disambiguate,
      complementWriter,
    );
  ({j|Js.Array.t($typeStr)|j}, disambiguate, complementWriter);
}

and writeTupleType =
    (
      currentWriter: writerState,
      typeLabel: string,
      tsType: TsType.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      complementWriter: writerState,
    ) =>
  currentWriter
  ->write("(")
  ->(
      state =>
        tsType->TsType.getTupleTypes
        |> Array.fold_left(
             ((state, i, disambiguate, complement), typ) => {
               let (typeStr, disambiguate, complement) =
                 state->buildType(
                   typeLabel,
                   typ,
                   tsNodes,
                   disambiguate,
                   complement,
                 );
               (
                 state->writeIf(i == 0, "", ", ")->write(typeStr),
                 i + 1,
                 disambiguate,
                 complement,
               );
             },
             (state, 0, disambiguate, complementWriter),
           )
    )
  |> (
    ((s, _, disambiguate, complement)) => (
      s->write(")")->getCode,
      disambiguate,
      complement,
    )
  )

and writePredefinedType =
    (writer: writerState, tsType: TsType.t): option(writerState) =>
  switch (tsType->TsType.getName) {
  | "string" => Some(writer->write("string"))
  | "boolean" => Some(writer->write("bool"))
  | "number" => Some(writer->write("float"))
  | "any" => Some(writer->write("'any"))
  | "unknown" => Some(writer->write("'unknown"))
  | "void"
  | "never" => Some(writer->write("unit"))
  | "symbol" => Some(writer->write("Js.Types.symbol"))
  | "null" => Some(writer->write("Js.Types.null_val"))
  | "undefined" => Some(writer->write("Js.Types.undefined_val"))
  | "object" => Some(writer->write("Js.Types.obj_val"))
  | "bigint" => Some(writer->write("Int64.t"))
  | _ => None
  }

and writeReferenceType =
    (writer: writerState, tsType: TsType.t, tsNodes: array(TsNode.t)) =>
  switch (
    tsNodes
    |> Array.to_list
    |> List.filter((tp: TsNode.t) =>
         tp->TsNode.getName == tsType->TsType.getName
       )
  ) {
  | [] => writer->write("t_TODO")
  | [h, ..._] => writer->writeReasonType(h)
  }

and writeAbstractTypeDeclaration = (writer: writerState, typ: TsNode.t) =>
  writer->write("type t = ")->writeReasonType(typ)->write(";")

and createGetName = (node: TsNode.t, names: list(string)) =>
  ("get" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names)

and createSetName = (node: TsNode.t, names: list(string)) =>
  ("set" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names)

and createUniqueName = (name: string, names: list(string)) =>
  name->Utils.lowerFirst->Utils.toUniqueName(names)

and createMemberGetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
    ) => {
  let (name, disambiguate) = node->createGetName(disambiguate);
  let (typeStr, disambiguate, complementWriter) =
    writer->buildType(
      node->TsNode.getName,
      node->TsNode.getType,
      tsNodes,
      disambiguate,
      setupWriterAs(writer),
    );
  let writer =
    writer->writeIf(
      complementWriter->hasContent,
      complementWriter->writeNewLine->getCode,
      "",
    );
  (
    writer
    ->write({j|[@bs.get] external $name: (t) => $typeStr = "|j})
    ->write(node->TsNode.getName)
    ->write("\";"),
    typeStr,
    disambiguate,
  );
}

and createMemberSetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      disambiguate: list(string),
      typeStr: string,
    ) => {
  let (name, disambiguate) = node->createSetName(disambiguate);
  (
    writer
    ->write({j|[@bs.send] external $name: (t, $typeStr) => $typeStr = "|j})
    ->write(node->TsNode.getName)
    ->write("\";"),
    disambiguate,
  );
}

and createGetSetFunction =
    (
      writer: writerState,
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
    ) => {
  let (writer, typeStr, disambiguate) =
    writer->createMemberGetFunction(node, tsNodes, disambiguate);
  let writer = writer->writeNewLine;
  let (writer, disambiguate) =
    writer->createMemberSetFunction(node, disambiguate, typeStr);
  (writer, disambiguate);
}

and createLiteralType =
    (
      ns: array(string),
      typeLabel: string,
      tsType: TsType.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      writer: writerState,
      typeNamesToPutInTheHead: list(string),
      addHeaderType: bool,
    ) => {
  let (normalizedName, disambiguate) =
    Utils.createModuleName(ns, typeLabel, disambiguate);

  let writer =
    writer
    ->write("module ")
    ->write(normalizedName)
    ->write(" = {")
    ->increaseIndent
    ->writeNewLine;

  let (writer, typeNamesToPutInTheHead) =
    if (addHeaderType) {
      let name = "t_" ++ normalizedName;
      (
        writer->write("type t = ")->write(name)->write(";"),
        [name, ...typeNamesToPutInTheHead],
      );
    } else {
      (writer->write("type t;"), typeNamesToPutInTheHead);
    };

  let (writer, disambiguate) = (
    writer->(
              writer =>
                tsType->TsType.getMembers
                |> Array.fold_left(
                     ((writer, disambiguate), member) =>
                       writer
                       ->writeNewLine
                       ->writeNewLine
                       ->createGetSetFunction(member, tsNodes, disambiguate),
                     (writer, disambiguate),
                   )
                |> (((s, _)) => s)
            ),
    typeNamesToPutInTheHead,
  );

  let writer = writer->decreaseIndent->writeNewLine->write("}")->writeNewLine;
  (writer, typeNamesToPutInTheHead, normalizedName, disambiguate);
};

let writeTypeArgumentsToFunctionDecl =
    (
      writer: writerState,
      pars: array(TsParameter.t),
      tsNodes: array(TsNode.t),
      complementWriter: writerState,
      disambiguate: list(string),
    )
    : (writerState, list(string), writerState) => {
  let state = writer->write("(");
  pars
  |> Array.fold_left(
       ((state, i, disambiguate, complementWriter), par) => {
         let (typeStr, disambiguate, complementWriter) =
           state->buildType(
             par->TsParameter.getName,
             par->TsParameter.getType,
             tsNodes,
             disambiguate,
             complementWriter,
           );
         (
           state->writeIf(i == 0, {j|$typeStr|j}, {j|, $typeStr|j}),
           i + 1,
           disambiguate,
           complementWriter,
         );
       },
       (state, 0, disambiguate, complementWriter),
     )
  |> (
    ((s, _, disambiguate, complement)) => (
      s->write(")"),
      disambiguate,
      complement,
    )
  );
};