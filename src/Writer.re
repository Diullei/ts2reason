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

let writeComment = (state: writerState, text: string) =>
  state->write({j|/* $text */|j});

let writeNewLine = (state: writerState) =>
  state->write({j|$state.nl|j})
  |> (
    state =>
      switch (state.currentIdentation) {
      | 0 => ' ' |> String.make(state.currentIdentation) |> state->write
      | _ => state
      }
  );

let writeRawJs = (state: writerState, text: string) =>
  state->write({j|[%bs.raw {| $text |}]|j});
