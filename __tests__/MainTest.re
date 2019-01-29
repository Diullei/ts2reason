open Jest;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Type alias declaration binding a predefined type :: string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type Typ = string")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module Typ = {
  type t = string;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: boolean", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyBoolType = boolean")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyBoolType = {
  type t = bool;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: any", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type myObjType = any")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyObjType = {
  type t = 'any;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: void", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type myObjType = void")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyObjType = {
  type t = unit;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: number", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = number;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = float;
}
")
    )
  )
);

describe(
  "Type alias declaration binding a predefined type :: multline declaration",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "
type myBoolType = boolean;
type MyType = number;
",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "
module MyBoolType = {
  type t = bool;
}

module MyType = {
  type t = float;
}
",
         )
    )
  )
);

describe("Type alias declaration binding a predefined type :: null", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = null;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Js.Types.null_val;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: undefined", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = undefined;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Js.Types.undefined_val;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: object", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = object;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Js.Types.obj_val;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: symbol", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = symbol;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Js.Types.symbol;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: bigint", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = bigint;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Int64.t;
}
")
    )
  )
);

describe("Type alias declaration binding a predefined type :: void", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = void;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = unit;
}
")
    )
  )
);

describe("Type alias declaration binding an array type :: string[]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = string[];")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = Js.Array.t(string);
}
")
    )
  )
);

describe("Type alias declaration binding an array type :: string[][]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = string[][];")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "
module MyType = {
  type t = Js.Array.t(Js.Array.t(string));
}
",
         )
    )
  )
);

describe("Type alias declaration binding an array type :: number[][][]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("type MyType = number[][][];")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "
module MyType = {
  type t = Js.Array.t(Js.Array.t(Js.Array.t(float)));
}
",
         )
    )
  )
);

describe(
  "Type alias declaration binding an array type :: [string, number, boolean]",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "type MyType = [string, number, boolean];",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("
module MyType = {
  type t = (string, float, bool);
}
")
    )
  )
);

describe(
  "Type alias declaration binding an array type :: [string, number[], boolean]",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "type MyType = [string, number[], boolean];",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "
module MyType = {
  type t = (string, Js.Array.t(float), bool);
}
",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("declare var a: number;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: float = \"a\";
let setA = (_value: float): float => [%bs.raw {| a = _value |}];
",
         )
    )
  )
);

describe("Variable declaration with no type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("declare var value;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external value: 'any = \"value\";
let setValue = (_value: 'any): 'any => [%bs.raw {| value = _value |}];
",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: (let) string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("declare let a: number;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: float = \"a\";
let setA = (_value: float): float => [%bs.raw {| a = _value |}];
",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: (const) string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("declare const a: number;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("[@bs.val] external a: float = \"a\";
")
    )
  )
);

describe("Variable declaration list - simple type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason("declare var a: boolean, b, c: number;")
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: bool = \"a\";
let setA = (_value: bool): bool => [%bs.raw {| a = _value |}];
[@bs.val] external b: 'any = \"b\";
let setB = (_value: 'any): 'any => [%bs.raw {| b = _value |}];
[@bs.val] external c: float = \"c\";
let setC = (_value: float): float => [%bs.raw {| c = _value |}];
",
         )
    )
  )
);

describe("Function declaration - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "declare function greet(greeting: string): void;",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual("[@bs.send] external greet: (string) => unit = \"greet\";
")
    )
  )
);

describe("Function declaration - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "declare function myFunc(arg1: string, arg2: boolean): number;",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.send] external myFunc: (string, bool) => float = \"myFunc\";
",
         )
    )
  )
);

describe("Function declaration - 003", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "declare function myFunc(arg1: string, arg2: boolean);",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.send] external myFunc: (string, bool) => 'any = \"myFunc\";
",
         )
    )
  )
);

describe("Enum declaration - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        (
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          |> Main.convertCodeToReason(
               "declare enum EnumTyp { Val1, Val2, Val3, };",
             )
        )
        ->Writer.getCode
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "
module EnumTyp = {
  [@bs.deriving jsConverter]
  type t =
    | [@bs.as 0] Val1
    | [@bs.as 1] Val2
    | [@bs.as 2] Val3;

  let name = (v: t) =>
    switch (v) {
    | Val1 => \"Val1\"
    | Val2 => \"Val2\"
    | Val3 => \"Val3\"
    };
}
",
         )
    )
  )
);