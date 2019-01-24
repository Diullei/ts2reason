let rec range = (start: int, end_: int) =>
  if (start >= end_) {
    [];
  } else {
    [start, ...range(start + 1, end_)];
  };

let repeat = (pattern: string, amount: int) =>
  range(0, amount - 1)
  |> List.map(_ => pattern)
  |> List.fold_left((++), pattern);

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
      | 0 => repeat(" ", state.currentIdentation) |> state->write
      | _ => state
      }
  );

let writeRawJs = (state: writerState, text: string) =>
  state->write({j|[%bs.raw {| $text |}]|j});
