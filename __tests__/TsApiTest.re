open Jest;
open TsApi;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Inspect a `type alias` declaration", () => {
  let node01 = extractTypesFromCode("type Typ = string")[0];

  Expect.(
    test("extractTypesFromCode id", () =>
      expect(node01->TsNode.getId) |> toEqual("Typ")
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
      expect(node01->TsNode.getNode->TypeKind.type_->TsType.getText)
      |> toEqual("string")
    )
  );

  Expect.(
    test("extractTypesFromCode isArray", () =>
      expect(node01->TsNode.isArray) |> toEqual(false)
    )
  );

  Expect.(
    test("extractTypesFromCode typeName", () =>
      expect(node01->TsNode.typeName) |> toEqual("string")
    )
  );

  let node02 = extractTypesFromCode("type Typ = boolean[]")[0];

  Expect.(
    test("extractTypesFromCode isArray", () =>
      expect(node02->TsNode.isArray) |> toEqual(true)
    )
  );

  Expect.(
    test("extractTypesFromCode arrayElementType->typeName", () =>
      expect(node02->TsNode.arrayElementType->TsNode.typeName)
      |> toEqual("boolean")
    )
  );
});