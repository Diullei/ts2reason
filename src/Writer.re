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
  |> (
    state =>
      switch (state.currentIdentation) {
      | 0 => ' ' |> String.make(state.currentIdentation) |> state->write
      | _ => state
      }
  );

let writeRawJs = (state: writerState, text: string) =>
  state->write({j|[%bs.raw {| $text |}]|j});

let writeReasonType = (state: writerState, typ: Types.TsNode.t) =>
  state
  ->write("t_")
  ->write(
      typ.ns |> Array.to_list |> String.concat("_") |> Utils.normalizeName,
    )
  ->write(
      switch (typ.ns) {
      | [||] => ""
      | _ => "_"
      },
    )
  ->write(typ.id);

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