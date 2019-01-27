open Jest;
open TsApi;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Inspect a `type alias` declaration", () => {
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
});