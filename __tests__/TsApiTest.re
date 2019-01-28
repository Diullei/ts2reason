open Jest;
open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Inspect a `type alias` declaration", () => {
  let node01 = extractTypesFromCode("type Typ = string")[0];

  Expect.(
    test("extractTypesFromCode id", () =>
      expect(node01->TsNode.getName) |> toEqual("Typ")
    )
  );

  Expect.(
    test("extractTypesFromCode kind", () =>
      expect(node01->TsNode.getKind)
      |> toEqual(SyntaxKind.TypeAliasDeclaration)
    )
  );

  Expect.(
    test("extractTypesFromCode node->type", () =>
      expect(node01->TsNode.getType->TsType.getName) |> toEqual("string")
    )
  );

  Expect.(
    test("extractTypesFromCode isArray", () =>
      expect(node01->TsNode.getType->TsType.getTypeKind)
      |> toEqual(TypeKind.Regular)
    )
  );

  Expect.(
    test("extractTypesFromCode typeName", () =>
      expect(node01->TsNode.getType->TsType.getName) |> toEqual("string")
    )
  );

  let node02 = extractTypesFromCode("type Typ = boolean[]")[0];

  Expect.(
    test("extractTypesFromCode isArray", () =>
      expect(node02->TsNode.getType->TsType.getTypeKind)
      |> toEqual(TypeKind.Array)
    )
  );

  Expect.(
    test("extractTypesFromCode arrayElementType->typeName", () =>
      expect(node02->TsNode.getType->TsType.getArrayType->TsType.getName)
      |> toEqual("boolean")
    )
  );
});