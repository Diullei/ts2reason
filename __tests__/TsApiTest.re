open Jest;
open TsApi;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe(
  "TsApi :: Type alias declaration binding a predefined type :: type Typ = string",
  () => {
    let node = extractTypesFromCode("type Typ = string")[0];

    Expect.(
      test("extractTypesFromCode id", () =>
        expect(node->TsNode.getId) |> toEqual("Typ")
      )
    );

    Expect.(
      test("extractTypesFromCode kind", () =>
        expect(node->TsNode.getKind)
        |> toEqual(SyntaxKind.TypeAliasDeclaration)
      )
    );

    Expect.(
      test("extractTypesFromCode node->type", () =>
        expect(node->TsNode.getNode->TypeKind.type_->TsType.getText)
        |> toEqual("string")
      )
    );

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
    );
  },
);

describe(
  "TsApi :: Type alias declaration binding a predefined type :: type myBoolType = boolean",
  () =>
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

describe(
  "TsApi :: Type alias declaration binding a predefined type :: type myObjType = any",
  () =>
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

describe(
  "TsApi :: Type alias declaration binding a predefined type :: type myObjType = void",
  () =>
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

describe(
  "TsApi :: Type alias declaration binding a predefined type :: type MyType = number",
  () =>
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
  "TsApi :: Type alias declaration binding a predefined type :: multline", () =>
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