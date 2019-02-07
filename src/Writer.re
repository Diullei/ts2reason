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

let returnFirstIfDifferent = (first: string, second: string) =>
  if (first == second) {
    "";
  } else {
    first;
  };

let rec buildType =
        (
          currentWriter: writerState,
          typeLabel: string,
          tsType: TsType.t,
          tsNodes: array(TsNode.t),
          disambiguate: list(string),
          complementWriter: writerState,
          indexAny: int,
        )
        : (string, list(string), writerState, int) => {
  let writer = setupWriterAs(currentWriter);

  switch (tsType->TsType.getTypeKind) {
  | TypeKind.Array =>
    writer->writeArrayType(
      typeLabel,
      tsType,
      tsNodes,
      disambiguate,
      complementWriter,
      indexAny,
    )

  | TypeKind.Tuple =>
    writer->writeTupleType(
      typeLabel,
      tsType,
      tsNodes,
      disambiguate,
      complementWriter,
      indexAny,
    )

  | TypeKind.TypeLiteral =>
    if (tsType->TsType.getMembers |> Array.length == 0) {
      ("Js.Types.obj_val", disambiguate, writer, indexAny);
    } else {
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
          false,
        );
      (normalizedName ++ ".t", disambiguate, writer, indexAny);
    }
  | _ =>
    switch (writer->writePredefinedType(tsType, indexAny)) {
    | Some((state, indexAny)) => (
        state->getCode,
        disambiguate,
        complementWriter,
        indexAny,
      )
    | None => (
        writer->writeReferenceType(tsType, tsNodes)->getCode,
        disambiguate,
        complementWriter,
        indexAny,
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
      indexAny: int,
    ) => {
  let (typeStr, disambiguate, complementWriter, indexAny) =
    buildType(
      currentWriter,
      typeLabel,
      tsType->TsType.getArrayType,
      tsNodes,
      disambiguate,
      complementWriter,
      indexAny,
    );
  ({j|Js.Array.t($typeStr)|j}, disambiguate, complementWriter, indexAny);
}

and writeTupleType =
    (
      currentWriter: writerState,
      typeLabel: string,
      tsType: TsType.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      complementWriter: writerState,
      indexAny: int,
    ) =>
  currentWriter
  ->write("(")
  ->(
      state =>
        tsType->TsType.getTupleTypes
        |> Array.fold_left(
             ((state, i, disambiguate, complement, indexAny), typ) => {
               let (typeStr, disambiguate, complement, indexAny) =
                 state->buildType(
                   typeLabel,
                   typ,
                   tsNodes,
                   disambiguate,
                   complement,
                   indexAny,
                 );
               (
                 state->writeIf(i == 0, "", ", ")->write(typeStr),
                 i + 1,
                 disambiguate,
                 complement,
                 indexAny,
               );
             },
             (state, 0, disambiguate, complementWriter, indexAny),
           )
    )
  |> (
    ((s, _, disambiguate, complement, indexAny)) => (
      s->write(")")->getCode,
      disambiguate,
      complement,
      indexAny,
    )
  )

and writePredefinedType =
    (writer: writerState, tsType: TsType.t, indexAny: int)
    : option((writerState, int)) =>
  switch (tsType->TsType.getName) {
  | "any" => Some((writer->write({j|'any$indexAny|j}), indexAny + 1))
  | "unknown" => Some((writer->write({j|'unknown$indexAny|j}), indexAny + 1))
  | "string" => Some((writer->write("string"), indexAny))
  | "boolean" => Some((writer->write("bool"), indexAny))
  | "number" => Some((writer->write("float"), indexAny))
  | "void"
  | "never" => Some((writer->write("unit"), indexAny))
  | "symbol" => Some((writer->write("Js.Types.symbol"), indexAny))
  | "null" => Some((writer->write("Js.Types.null_val"), indexAny))
  | "undefined" => Some((writer->write("Js.Types.undefined_val"), indexAny))
  | "object" => Some((writer->write("Js.Types.obj_val"), indexAny))
  | "bigint" => Some((writer->write("Int64.t"), indexAny))
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
  let (typeStr, disambiguate, complementWriter, indexAny) =
    writer->buildType(
      node->TsNode.getName,
      node->TsNode.getType,
      tsNodes,
      disambiguate,
      setupWriterAs(writer),
      0,
    );
  let writer =
    writer->writeIf(
      complementWriter->hasContent,
      complementWriter->writeNewLine->getCode,
      "",
    );
  (
    writer
    ->writeIf(
        node->TsNode.isOptional,
        {j|[@bs.get] external $name: (t) => option($typeStr) = "|j},
        {j|[@bs.get] external $name: (t) => $typeStr = "|j},
      )
    ->write(returnFirstIfDifferent(node->TsNode.getName, name))
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
    ->write(returnFirstIfDifferent(node->TsNode.getName, name))
    ->write("\";"),
    disambiguate,
  );
}

and writeGetSetFunction =
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
      isClassOrInterface: bool,
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
      let headName =
        if (tsType->TsType.getMembers |> Array.length > 0 || isClassOrInterface) {
          {j|t_$normalizedName|j};
        } else {
          {j|t_$normalizedName = Js.Types.obj_val|j};
        };
      (
        writer
        ->write("type t = ")
        ->write({j|t_$normalizedName|j})
        ->write(";"),
        [headName, ...typeNamesToPutInTheHead],
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
                       switch (member->TsNode.getKind) {
                       | Ts.SyntaxKind.PropertySignature
                       | Ts.SyntaxKind.PropertyDeclaration =>
                         writer
                         ->writeNewLine
                         ->writeNewLine
                         ->writeGetSetFunction(member, tsNodes, disambiguate)

                       | Ts.SyntaxKind.MethodSignature
                       | Ts.SyntaxKind.MethodDeclaration =>
                         writer
                         ->writeNewLine
                         ->writeNewLine
                         ->writeFunctionDeclaration(
                             member,
                             tsNodes,
                             disambiguate,
                             true,
                           )

                       | _ => (writer, disambiguate)
                       },
                     (writer, disambiguate),
                   )
                |> (((s, _)) => s)
            ),
    typeNamesToPutInTheHead,
  );

  let writer = writer->decreaseIndent->writeNewLine->write("}")->writeNewLine;
  (writer, typeNamesToPutInTheHead, normalizedName, disambiguate);
}
and writeFunctionDeclaration =
    (
      writer: writerState,
      node: TsNode.t,
      tsNodes: array(TsNode.t),
      disambiguate: list(string),
      isMember: bool,
    ) => {
  let (name, disambiguate) =
    node->TsNode.getName->createUniqueName(disambiguate);
  let (parsWriter, disambiguate, complementWriter, indexAny) =
    writeTypeArgumentsToFunctionDecl(
      setupWriterAs(writer),
      node->TsNode.getParameters,
      tsNodes,
      setupWriterAs(writer),
      disambiguate,
      isMember,
      0,
    );
  let (typeStr, disambiguate, complementWriter, _indexAny) =
    writer->buildType(
      node->TsNode.getName,
      node->TsNode.getType,
      tsNodes,
      disambiguate,
      complementWriter,
      indexAny,
    );

  let writer =
    writer
    ->writeIf(complementWriter->hasContent, complementWriter->getCode, "")
    ->writeIf(
        node->TsNode.getParameters |> Array.length == 0 && !isMember,
        "[@bs.val] external ",
        "[@bs.send] external ",
      )
    ->write(name)
    ->write(": ")
    ->write(parsWriter->getCode)
    ->write({j| => $typeStr = "|j})
    ->write(returnFirstIfDifferent(node->TsNode.getName, name))
    ->write("\";")
    ->writeNewLine;

  (writer, disambiguate);
}
and writeTypeArgumentsToFunctionDecl =
    (
      writer: writerState,
      pars: array(TsParameter.t),
      tsNodes: array(TsNode.t),
      complementWriter: writerState,
      disambiguate: list(string),
      isMember: bool,
      indexAny: int,
    )
    : (writerState, list(string), writerState, int) => {
  let state =
    writer->writeIf(
      isMember,
      "(t",
      if (pars |> Array.length == 0) {
        "(unit";
      } else {
        "(";
      },
    );
  pars
  |> Array.fold_left(
       ((state, i, disambiguate, complementWriter, indexAny), par) => {
         let (typeStr, disambiguate, complementWriter, indexAny) =
           state->buildType(
             par->TsParameter.getName,
             par->TsParameter.getType,
             tsNodes,
             disambiguate,
             complementWriter,
             indexAny,
           );
         let (parameterName, disambiguate) =
           par->TsParameter.getName->createUniqueName(disambiguate);
         (
           if (par->TsParameter.isOptional) {
             state->writeIf(
               i == 0,
               {j|~$parameterName: $typeStr=?|j},
               {j|, ~$parameterName: $typeStr=?|j},
             );
           } else {
             state->writeIf(i == 0, {j|$typeStr|j}, {j|, $typeStr|j});
           },
           i + 1,
           disambiguate,
           complementWriter,
           indexAny,
         );
       },
       (
         state,
         if (isMember) {1} else {0},
         disambiguate,
         complementWriter,
         indexAny,
       ),
     )
  |> (
    ((s, _, disambiguate, complement, indexAny)) => (
      s->write(")"),
      disambiguate,
      complement,
      indexAny,
    )
  );
};