let reservedWords = [
  "sig",
  "module",
  "begin",
  "end",
  "object",
  "switch",
  "to",
  "then",
  "type",
  "as",
];

let rec range = (start: int, end_: int) =>
  if (start >= end_) {
    [];
  } else {
    [start, ...range(start + 1, end_)];
  };

let rec uniq = x => {
  let rec uniq_help = (l, n) =>
    switch (l) {
    | [] => []
    | [h, ...t] =>
      if (n == h) {
        uniq_help(t, n);
      } else {
        [h, ...uniq_help(t, n)];
      }
    };
  switch (x) {
  | [] => []
  | [h, ...t] => [h, ...uniq_help(uniq(t), h)]
  };
};

let capitalize = (input: string) => input->String.capitalize;

let lowerFirst = (input: string) => input->String.uncapitalize;

let normalizeName = (name: string) =>
  name
  |> Js.String.replaceByRe([%re "/[\$|\.|\-]/g"], "_")
  |> Js.String.replaceByRe([%re "/[\"|']/g"], "");

let fixIfItsAReservedWork = (id: string) =>
  switch (
    reservedWords |> List.filter(kw => kw == id->lowerFirst) |> List.length
  ) {
  | 0 => id
  | _ => {j|$(id)_|j}
  };

let toUniqueName = (candidateName: string, usedNames: list(string)) => {
  let name = candidateName->fixIfItsAReservedWork;
  let occurrence =
    usedNames
    |> List.filter(n => n == candidateName->fixIfItsAReservedWork)
    |> List.length;

  switch (occurrence) {
  | 0 => (name, [name, ...usedNames])
  | _ => (name ++ string_of_int(occurrence), [name, ...usedNames])
  };
};

let makeIndent =
  fun
  | 0 => ""
  | size => ' ' |> String.make(size);

let createNameSpaceName = (ns: array(string)) =>
  ns |> Array.to_list |> String.concat("_") |> normalizeName;