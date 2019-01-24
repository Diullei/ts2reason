let reservedWorks = [
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

let toUniqueName = (candidateName: string, usedNames: list(string)) => {
  let name =
    switch (
      reservedWorks
      |> List.filter(kw => kw == candidateName->lowerFirst)
      |> List.length
    ) {
    | 0 => candidateName
    | _ => {j|$(candidateName)_|j}
    };

  let occurrence = usedNames |> List.filter(n => n == name) |> List.length;

  switch (occurrence) {
  | 0 => (name, [name, ...usedNames])
  | _ => (name ++ string_of_int(occurrence), [name, ...usedNames])
  };
};