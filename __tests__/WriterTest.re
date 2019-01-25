open Jest;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let makeFakeTsNode =
    (ns: array(string), id: string, kind: Types.SyntaxKind.t): Types.TsNode.t => {
  ns,
  id,
  kind,
  node: [%bs.raw {|{getType: () => ({ getText: () => "string" })}|}],
};

let makeFakeTsType = (_typ: string): Types.TsType.t => [%bs.raw
  {| { getText: () => _typ } |}
];

let makeFakeTsParDec = (_name: string, _typ: string): Types.TsParDecl.t => [%bs.raw
  {|
    {
      getName: () => _name,
      getType: () => ({ getText: () => _typ })
    }
  |}
];

describe("Writer", () => {
  Expect.(
    test("write", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.write("some text")
        ->Writer.write(" !!")
        ->Writer.getCode,
      )
      |> toEqual("some text !!");
    })
  );

  Expect.(
    test("increaseIndent", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.increaseIndent
        ->Writer.increaseIndent
        ->Writer.writeNewLine
        ->Writer.write("aaa")
        ->Writer.getCode,
      )
      |> toEqual("\n    aaa");
    })
  );

  Expect.(
    test("decreaseIndent", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.increaseIndent
        ->Writer.increaseIndent
        ->Writer.decreaseIndent
        ->Writer.writeNewLine
        ->Writer.write("aaa")
        ->Writer.getCode,
      )
      |> toEqual("\n  aaa");
    })
  );

  Expect.(
    test("writeComment", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(wState->Writer.writeComment("some comment")->Writer.getCode)
      |> toEqual("/* some comment */");
    })
  );

  Expect.(
    test("writeNewLine", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(wState->Writer.writeNewLine->Writer.getCode) |> toEqual(eol);
    })
  );

  Expect.(
    test("writeRawJs", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(wState->Writer.writeRawJs("a b c d e")->Writer.getCode)
      |> toEqual("[%bs.raw {| a b c d e |}]");
    })
  );

  Expect.(
    test("writeReasonType", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeReasonType(
            makeFakeTsNode(
              [|"aaa", "bbb"|],
              "ccc",
              Types.SyntaxKind.ModuleDeclaration,
            ),
          )
        ->Writer.getCode,
      )
      |> toEqual("t_aaa_bbb_ccc");
    })
  );

  Expect.(
    testAll(
      "writeType",
      [
        (makeFakeTsType("string"), [||], "string"),
        (makeFakeTsType("boolean"), [||], "bool"),
        (makeFakeTsType("number"), [||], "float"),
        (makeFakeTsType("xyz"), [||], "t_TODO"),
        (
          makeFakeTsType("MyObj"),
          [|
            makeFakeTsNode([||], "MyObj", Types.SyntaxKind.ModuleDeclaration),
          |],
          "t_MyObj",
        ),
      ],
      ((fakeObj, types, output)) =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        ->Writer.writeType(fakeObj, types)
        ->Writer.getCode,
      )
      |> toEqual(output)
    )
  );

  Expect.(
    test("writeIf", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeIf(true, "true expr", "false expr")
        ->Writer.getCode,
      )
      |> toEqual("true expr");
    })
  );

  Expect.(
    testAll(
      "writeParameterName",
      [
        ("type", true, "_type"),
        ("type", false, "type_"),
        ("name", false, "name"),
      ],
      ((name, startWithUnderline, output)) =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        ->Writer.writeParameterName(name, startWithUnderline)
        ->Writer.getCode,
      )
      |> toEqual(output)
    )
  );

  Expect.(
    test("writeParameter", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeParameter(makeFakeTsParDec("param", "number"), [||])
        ->Writer.getCode,
      )
      |> toEqual("_param: float");
    })
  );

  Expect.(
    testAll(
      "writeArgumentsToMethodDecl",
      [
        ([||], "(_inst: t)"),
        (
          [|
            makeFakeTsParDec("param01", "number"),
            makeFakeTsParDec("param02", "boolean"),
            makeFakeTsParDec("param03", "string"),
          |],
          "(_inst: t, _param01: float, _param02: bool, _param03: string)",
        ),
      ],
      ((parList, output)) =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        ->Writer.writeArgumentsToMethodDecl(parList, [||])
        ->Writer.getCode,
      )
      |> toEqual(output)
    )
  );

  Expect.(
    testAll(
      "writeArgumentsToFunctionDecl",
      [
        ([||], "()"),
        (
          [|
            makeFakeTsParDec("param01", "number"),
            makeFakeTsParDec("param02", "boolean"),
            makeFakeTsParDec("param03", "string"),
          |],
          "(_param01: float, _param02: bool, _param03: string)",
        ),
      ],
      ((parList, output)) =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        ->Writer.writeArgumentsToFunctionDecl(parList, [||])
        ->Writer.getCode,
      )
      |> toEqual(output)
    )
  );

  Expect.(
    test("writeModuleNameFrom", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeModuleNameFrom(
            makeFakeTsNode(
              [||],
              "'myModule'",
              Types.SyntaxKind.InterfaceDeclaration,
            ),
          )
        ->Writer.getCode,
      )
      |> toEqual("MyModule");
    })
  );

  Expect.(
    test("writeModuleName", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeModuleName([|"aaa", "bbb", "ccc"|])
        ->Writer.getCode,
      )
      |> toEqual("Ccc");
    })
  );

  Expect.(
    test("writeGetPropertyDecl", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeGetPropertyDecl(
            makeFakeTsNode(
              [||],
              "propName",
              Types.SyntaxKind.PropertyDeclaration,
            ),
            [||],
            [],
          )
        ->(((s, _)) => s)
        ->Writer.getCode,
      )
      |> toEqual(
           "let getPropName = (_inst: t): string => [%bs.raw {| _inst.propName |}];",
         );
    })
  );

  Expect.(
    test("writeSetPropertyDecl", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeSetPropertyDecl(
            makeFakeTsNode(
              [||],
              "propName",
              Types.SyntaxKind.PropertyDeclaration,
            ),
            [||],
            [],
          )
        ->(((s, _)) => s)
        ->Writer.getCode,
      )
      |> toEqual(
           "let setPropName = (_inst: t, _value: string): unit => [%bs.raw {| _inst.propName = _value |}];",
         );
    })
  );
});