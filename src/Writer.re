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
  | TypeKind.Array =>
    state
    ->write("Js.Array.t(")
    ->writeType(tsType->TsType.getArrayType, types)
    ->write(")")

  | TypeKind.Tuple =>
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

  | _ =>
    switch (tsType->TsType.getName) {
    | "string" => state->write("string")
    | "boolean" => state->write("bool")
    | "number" => state->write("float")
    | "any" => state->write("'any")
    | "void"
    | "never" => state->write("unit")
    | "symbol" => state->write("Js.Types.symbol")
    | "null" => state->write("Js.Types.null_val")
    | "undefined" => state->write("Js.Types.undefined_val")
    | "object" => state->write("Js.Types.obj_val")
    | "bigint" => state->write("Int64.t")
    | _ =>
      switch (
        types
        |> Array.to_list
        |> List.filter((tp: TsNode.t) =>
             tp->TsNode.getName == tsType->TsType.getName
           )
      ) {
      | [] => state->write("t_TODO")
      | [h, ..._] => state->writeReasonType(h)
      }
    }
  };
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

let writeModuleNameFrom = (state: writerState, typ: TsNode.t) =>
  state->write(Utils.capitalize(Utils.normalizeName(typ->TsNode.getName)));

let writeModuleName = (state: writerState, ns: array(string)) =>
  state->write(
    Utils.capitalize(Utils.normalizeName(ns[(ns |> Array.length) - 1])),
  );

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