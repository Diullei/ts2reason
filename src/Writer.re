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

let write = (state: writerState, text: string): writerState => {
  ...state,
  code: state.code ++ text,
};

let increaseIndent = (state: writerState) => {
  ...state,
  currentIdentation: state.currentIdentation + 1,
};

let decreaseIndent = (state: writerState) => {
  ...state,
  currentIdentation: state.currentIdentation - 1,
};

let getCode = (state: writerState) => state.code;

let writeComment = (state: writerState, text: string) =>
  state->write({j|/* $text */|j});

let writeNewLine = (state: writerState) =>
  state->write(state.nl)
  |> (state => Utils.makeIndent(state.currentIdentation * 2) |> state->write);

let writeRawJs = (state: writerState, text: string) =>
  state->write({j|[%bs.raw {| $text |}]|j});

let writeReasonType = (state: writerState, typ: TsNode.t) => {
  let namespaceSplit =
    switch (typ->TsNode.getNs) {
    | [||] => ""
    | _ => "_"
    };

  state
  ->write("t_")
  ->write(typ->TsNode.getNs |> Utils.createNameSpaceName)
  ->write(namespaceSplit)
  ->write(typ->TsNode.getName);
};

let writeIf =
    (state: writerState, condition: bool, thenText: string, elseText: string) =>
  if (condition) {
    state->write(thenText);
  } else {
    state->write(elseText);
  };

let rec writeType =
        (state: writerState, tsType: TsType.t, types: array(TsNode.t)) => {
  switch (tsType->TsType.getTypeKind) {
  | TypeKind.Array => state->writeArrayType(tsType, types)
  | TypeKind.Tuple => state->writeTupleType(tsType, types)
  | _ =>
    switch (state->writePredefinedType(tsType)) {
    | Some(state) => state
    | None => state->writeReferenceType(tsType, types)
    }
  };
}
and writeArrayType =
    (state: writerState, tsType: TsType.t, types: array(TsNode.t)) =>
  state
  ->write("Js.Array.t(")
  ->writeType(tsType->TsType.getArrayType, types)
  ->write(")")

and writeTupleType =
    (state: writerState, tsType: TsType.t, types: array(TsNode.t)) =>
  state
  ->write("(")
  ->(
      state =>
        tsType->TsType.getTupleTypes
        |> Array.fold_left(
             ((state, i), typ) =>
               (
                 state->writeIf(i == 0, "", ", ")->writeType(typ, types),
                 i + 1,
               ),
             (state, 0),
           )
    )
  |> (((s, _)) => s->write(")"))

and writePredefinedType =
    (state: writerState, tsType: TsType.t): option(writerState) =>
  switch (tsType->TsType.getName) {
  | "string" => Some(state->write("string"))
  | "boolean" => Some(state->write("bool"))
  | "number" => Some(state->write("float"))
  | "any" => Some(state->write("'any"))
  | "unknown" => Some(state->write("'unknown"))
  | "void"
  | "never" => Some(state->write("unit"))
  | "symbol" => Some(state->write("Js.Types.symbol"))
  | "null" => Some(state->write("Js.Types.null_val"))
  | "undefined" => Some(state->write("Js.Types.undefined_val"))
  | "object" => Some(state->write("Js.Types.obj_val"))
  | "bigint" => Some(state->write("Int64.t"))
  | _ => None
  }

and writeReferenceType =
    (state: writerState, tsType: TsType.t, types: array(TsNode.t)) =>
  switch (
    types
    |> Array.to_list
    |> List.filter((tp: TsNode.t) =>
         tp->TsNode.getName == tsType->TsType.getName
       )
  ) {
  | [] => state->write("t_TODO")
  | [h, ..._] => state->writeReasonType(h)
  };

let writeModuleNameFrom = (state: writerState, typ: TsNode.t) =>
  state->write(Utils.capitalize(Utils.normalizeName(typ->TsNode.getName)));

let writeModuleName = (state: writerState, ns: array(string)) =>
  state->write(
    Utils.capitalize(Utils.normalizeName(ns[(ns |> Array.length) - 1])),
  );

let writeBeginModuleFromNs = (state: writerState, ns: array(string)) =>
  state
  ->writeNewLine
  ->write("module ")
  ->writeModuleName(ns)
  ->write(" = {")
  ->increaseIndent;

let writeBeginModuleFromType = (state: writerState, typ: TsNode.t) =>
  state
  ->writeNewLine
  ->write("module ")
  ->writeModuleNameFrom(typ)
  ->write(" = {")
  ->increaseIndent;

let writeEndModule = (state: writerState) =>
  state->decreaseIndent->writeNewLine->write("}");

let writeAbstractTypeDeclaration = (state: writerState, typ: TsNode.t) =>
  state->write("type t = ")->writeReasonType(typ)->write(";");

let createTypeAssignDeclaration =
    (writer: writerState, node: TsNode.t, typeNames: list(string)) => {
  let name = "t_" ++ Utils.capitalize(node->TsNode.getName);
  (
    writer->write("type t = ")->write(name)->write(";"),
    [name, ...typeNames],
  );
};

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

let createLiteralTypeBlock =
    (
      writer: writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      _names: list(string),
      typeNames: list(string),
    ) => {
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
};

let createLiteralType =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      writer: writerState,
      typeNames: list(string),
    ) => {
  let writer = writer->writeBeginModuleFromType(node)->writeNewLine;
  let (writer, typeNames) =
    writer->createLiteralTypeBlock(node, types, names, typeNames);
  let writer = writer->writeEndModule->writeNewLine;
  (writer, typeNames);
};

let writeParameterName =
    (state: writerState, name: string, startWithUnderline: bool) =>
  state->writeIf(
    startWithUnderline,
    "_" ++ name,
    name->Utils.fixIfItsAReservedWork,
  );

let writeParameter =
    (state: writerState, par: TsParameter.t, types: array(TsNode.t)) =>
  state
  ->writeParameterName(par->TsParameter.getName, true)
  ->write(": ")
  ->writeType(par->TsParameter.getType, types);

let writeArgumentsToMethodDecl =
    (state: writerState, pars: array(TsParameter.t), types: array(TsNode.t)) => {
  let state = state->write("(_inst: t");
  (
    pars
    |> Array.fold_left(
         (state, par) => state->write(", ")->writeParameter(par, types),
         state,
       )
  )
  ->write(")");
};

let writeArgumentsToFunctionDecl =
    (state: writerState, pars: array(TsParameter.t), types: array(TsNode.t)) => {
  let state = state->write("(");
  pars
  |> Array.fold_left(
       ((state, i), par) =>
         (
           state->writeIf(i == 0, "", ", ")->writeParameter(par, types),
           i + 1,
         ),
       (state, 0),
     )
  |> (((s, _)) => s->write(")"));
};

let writeTypeArgumentsToFunctionDecl =
    (state: writerState, pars: array(TsParameter.t), types: array(TsNode.t)) => {
  let state = state->write("(");
  pars
  |> Array.fold_left(
       ((state, i), par) =>
         (
           state
           ->writeIf(i == 0, "", ", ")
           ->writeType(par->TsParameter.getType, types),
           i + 1,
         ),
       (state, 0),
     )
  |> (((s, _)) => s->write(")"));
};

let writeArgumentsToFunctionCall =
    (state: writerState, pars: array(TsParameter.t)) => {
  let state = state->write("(");
  pars
  |> Array.fold_left(
       ((state, i), par) =>
         (
           state
           ->writeIf(i == 0, "", ", ")
           ->writeParameterName(par->TsParameter.getName, true),
           i + 1,
         ),
       (state, 0),
     )
  |> (((s, _)) => s->write(")"));
};

let writeGetPropertyDecl =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) =
    ("get" ++ Utils.capitalize(typ->TsNode.getName))
    ->Utils.toUniqueName(names);

  let state =
    state
    ->write("let ")
    ->write(name)
    ->write(" = (_inst: t): ")
    ->writeType(typ->TsNode.getType, types)
    ->write(" => [%bs.raw {| _inst.")
    ->write(typ->TsNode.getName)
    ->write(" |}];");

  (state, names);
};

let writeSetPropertyDecl =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) =
    ("set" ++ Utils.capitalize(typ->TsNode.getName))
    ->Utils.toUniqueName(names);

  let state =
    state
    ->write("let ")
    ->write(name)
    ->write(" = (_inst: t, _value: ")
    ->writeType(typ->TsNode.getType, types)
    ->write("): unit => [%bs.raw {| _inst.")
    ->write(typ->TsNode.getName)
    ->write(" = _value |}];");

  (state, names);
};

let writePropertyDecls =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (state, names) = state->writeGetPropertyDecl(typ, types, names);
  let state = state->writeNewLine;

  let (state, names) = state->writeSetPropertyDecl(typ, types, names);
  let state = state->writeNewLine;

  (state, names);
};

let writeMethodDecl =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) =
    Utils.lowerFirst(typ->TsNode.getName)->Utils.toUniqueName(names);

  let state =
    state
    ->write("let ")
    ->write(name)
    ->write(" = ")
    ->writeArgumentsToMethodDecl(typ->TsNode.getParameters, types)
    ->write(": ")
    ->writeType(typ->TsNode.getType, types)
    ->write(" => [%bs.raw {| _inst.")
    ->write(typ->TsNode.getName)
    ->writeArgumentsToFunctionCall(typ->TsNode.getParameters)
    ->write(" |}];");

  (state, names);
};

/* this function will be removed, see Main.re for the correct implementation */
let writeFunctionDecl =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      ns: array(string),
      names: list(string),
    ) => {
  let (name, names) =
    Utils.lowerFirst(typ->TsNode.getName)->Utils.toUniqueName(names);

  let state =
    state
    ->write("[@bs.module \"")
    ->write(Utils.normalizeName(ns |> Array.to_list |> String.concat(".")))
    ->write("\"] external ")
    ->write(name)
    ->write(": ")
    ->writeArgumentsToFunctionDecl(typ->TsNode.getParameters, types)
    ->write(" => ")
    ->writeType(typ->TsNode.getType, types)
    ->write(" = \"")
    ->write(typ->TsNode.getName)
    ->write("\"");

  (state, names);
};

/* this function will be removed, see Main.re for the correct implementation */
let writeVariableDecl =
    (
      state: writerState,
      typ: TsNode.t,
      types: array(TsNode.t),
      ns: array(string),
      names: list(string),
    ) => {
  let (name, names) =
    Utils.lowerFirst(typ->TsNode.getName)->Utils.toUniqueName(names);

  let state =
    state
    ->write("[@bs.module \"")
    ->write(Utils.normalizeName(ns |> Array.to_list |> String.concat(".")))
    ->write("\"] external ")
    ->write(name)
    ->write(": ")
    ->writeType(typ->TsNode.getType, types)
    ->write(" = \"")
    ->write(typ->TsNode.getName)
    ->write("\"");

  (state, names);
};