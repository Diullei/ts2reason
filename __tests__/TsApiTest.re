open Jest;
open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Inspect a `type alias` declaration", () => {
  let node = extractTypesFromCode("type Typ = string")[0];

  Expect.(
    test("extractTypesFromCode typeAlias name", () =>
      expect(node->TsNode.getName) |> toEqual("Typ")
    )
  );

  Expect.(
    test("extractTypesFromCode typeAlias kind", () =>
      expect(node->TsNode.getKind)
      |> toEqual(SyntaxKind.TypeAliasDeclaration)
    )
  );

  Expect.(
    test("extractTypesFromCode typeAlias node->type", () =>
      expect(node->TsNode.getType->TsType.getName) |> toEqual("string")
    )
  );

  Expect.(
    test("extractTypesFromCode typeAlias isArray", () =>
      expect(node->TsNode.getType->TsType.getTypeKind)
      |> toEqual(TypeKind.Regular)
    )
  );

  Expect.(
    test("extractTypesFromCode typeAlias typeName", () =>
      expect(node->TsNode.getType->TsType.getName) |> toEqual("string")
    )
  );

  let node = extractTypesFromCode("type Typ = boolean[]")[0];

  Expect.(
    test("extractTypesFromCode typeAlias isArray", () =>
      expect(node->TsNode.getType->TsType.getTypeKind)
      |> toEqual(TypeKind.Array)
    )
  );

  Expect.(
    test("extractTypesFromCode typeAlias arrayElementType->typeName", () =>
      expect(node->TsNode.getType->TsType.getArrayType->TsType.getName)
      |> toEqual("boolean")
    )
  );

  let node = extractTypesFromCode("declare var a: number;")[0];

  Expect.(
    test("extractTypesFromCode varDeclaration name", () =>
      expect(node->TsNode.getName) |> toEqual("a")
    )
  );

  Expect.(
    test("extractTypesFromCode varDeclaration kind", () =>
      expect(node->TsNode.getKind) |> toEqual(SyntaxKind.VariableDeclaration)
    )
  );

  Expect.(
    test("extractTypesFromCode varDeclaration node->type", () =>
      expect(node->TsNode.getType->TsType.getName) |> toEqual("number")
    )
  );
});