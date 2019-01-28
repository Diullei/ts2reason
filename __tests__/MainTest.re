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
