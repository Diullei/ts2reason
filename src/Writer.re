open Types;

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
    switch (typ.ns) {
    | [||] => ""
    | _ => "_"
    };

  state
  ->write("t_")
  ->write(typ.ns |> Utils.createNameSpaceName)
  ->write(namespaceSplit)
  ->write(typ.id);
};

let writeType =
    (state: writerState, tsType: TsType.t, types: array(TsNode.t)) =>
  switch (tsType->TsType.getText) {
  | "string" => state->write("string")
  | "boolean" => state->write("bool")
  | "number" => state->write("float")
  | _ =>
    switch (
      types
      |> Array.to_list
      |> List.filter((tp: TsNode.t) => tp.id == tsType->TsType.getText)
    ) {
    | [] => state->write("t_TODO")
    | [h, ..._] => state->writeReasonType(h)
    }
  };

let writeIf =
    (state: writerState, condition: bool, thenText: string, elseText: string) =>
  if (condition) {
    state->write(thenText);
  } else {
    state->write(elseText);
  };

let writeParameterName =
    (state: writerState, name: string, startWithUnderline: bool) =>
  state->writeIf(
    startWithUnderline,
    "_" ++ name,
    name->Utils.fixIfItsAReservedWork,
  );

let writeParameter =
    (state: writerState, par: TsParDecl.t, types: array(TsNode.t)) =>
  state
  ->writeParameterName(par->TsParDecl.getName, true)
  ->write(": ")
  ->writeType(par->TsParDecl.getType, types);

let writeArgumentsToMethodDecl =
    (state: writerState, pars: array(TsParDecl.t), types: array(TsNode.t)) => {
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
    (state: writerState, pars: array(TsParDecl.t), types: array(TsNode.t)) => {
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

let writeModuleNameFrom = (state: writerState, typ: TsNode.t) =>
  state->write(Utils.capitalize(Utils.normalizeName(typ.id)));

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
    ("get" ++ Utils.capitalize(typ.id))->Utils.toUniqueName(names);

  let state =
    state
    ->write("let ")
    ->write(name)
    ->write(" = (_inst: t): ")
    ->writeType(typ.node->TypeKind.getType, types)
    ->write(" => [%bs.raw {| _inst.")
    ->write(typ.id)
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
    ("set" ++ Utils.capitalize(typ.id))->Utils.toUniqueName(names);

  let state =
    state
    ->write("let ")
    ->write(name)
    ->write(" = (_inst: t, _value: ")
    ->writeType(typ.node->TypeKind.getType, types)
    ->write("): unit => [%bs.raw {| _inst.")
    ->write(typ.id)
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