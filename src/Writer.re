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

let getCode = (state: writerState) => state.code;

let write = (state: writerState, text: string): writerState => {
  ...state,
  code: state.code ++ text,
};

let writeComment = (state: writerState, text: string) =>
  state->write({j|/* $text */|j});

let writeNewLine = (state: writerState) =>
  state->write(state.nl)
  |> (state => Utils.makeIndent(state.currentIdentation) |> state->write);

let writeRawJs = (state: writerState, text: string) =>
  state->write({j|[%bs.raw {| $text |}]|j});

let writeReasonType = (state: writerState, typ: Types.TsNode.t) => {
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
    (
      state: writerState,
      tsType: Types.TsType.t,
      types: array(Types.TsNode.t),
    ) =>
  switch (tsType->Types.TsType.getText) {
  | "string" => state->write("string")
  | "boolean" => state->write("bool")
  | "number" => state->write("float")
  | _ =>
    switch (
      types
      |> Array.to_list
      |> List.filter((tp: Types.TsNode.t) =>
           tp.id == tsType->Types.TsType.getText
         )
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
    (state: writerState, par: Types.TsParDecl.t, types: array(Types.TsNode.t)) =>
  state
  ->writeParameterName(par->Types.TsParDecl.getName, true)
  ->write(": ")
  ->writeType(par->Types.TsParDecl.getType, types);

let writeArgumentsToMethodDecl =
    (
      state: writerState,
      pars: array(Types.TsParDecl.t),
      types: array(Types.TsNode.t),
    ) => {
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
    (
      state: writerState,
      pars: array(Types.TsParDecl.t),
      types: array(Types.TsNode.t),
    ) => {
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