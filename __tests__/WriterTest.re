open Jest;
open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let makeFakeTsNode =
    (_ns: array(string), _name: string, _kind: SyntaxKind.t): TsNode.t => [%bs.raw
  {| {
      ns: _ns,
      name: _name,
      kind: _kind,
      type: {
        ns: [],
        name: "string",
        typeKind: 0,
        arrayType: undefined,
      },
      parameters: [{
          name: "arg01",
          type: { ns: [], name: "string", typeKind: 0, arrayType: undefined },
        }, {
          name: "arg02",
          type: { ns: [], name: "number", typeKind: 0, arrayType: undefined },
        }, {
          name: "arg03",
          type: { ns: [], name: "boolean", typeKind: 0, arrayType: undefined },
        }
      ],
      isConst: false,
    } |}
];

let makeFakeTsType = (_typ: string): TsType.t => [%bs.raw
  {| {
      ns: [],
      name: _typ,
      typeKind: 0,
      arrayType: undefined,
    }
  |}
];

let makeFakeTsParDec = (_name: string, _typ: string): TsParameter.t => [%bs.raw
  {| {
      name: _name,
      type: { ns: [], name: _typ, typeKind: 0, arrayType: undefined },
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
              SyntaxKind.ModuleDeclaration,
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
          [|makeFakeTsNode([||], "MyObj", SyntaxKind.ModuleDeclaration)|],
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
    testAll(
      "writeArgumentsToFunctionCall",
      [
        ([||], "()"),
        (
          [|
            makeFakeTsParDec("param01", "number"),
            makeFakeTsParDec("param02", "boolean"),
            makeFakeTsParDec("param03", "string"),
          |],
          "(_param01, _param02, _param03)",
        ),
      ],
      ((parList, output)) =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        ->Writer.writeArgumentsToFunctionCall(parList)
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
              SyntaxKind.InterfaceDeclaration,
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
            makeFakeTsNode([||], "propName", SyntaxKind.PropertyDeclaration),
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
            makeFakeTsNode([||], "propName", SyntaxKind.PropertyDeclaration),
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

  Expect.(
    test("writeMethodDecl", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeMethodDecl(
            makeFakeTsNode([||], "myFunc", SyntaxKind.MethodDeclaration),
            [||],
            [],
          )
        ->(((s, _)) => s)
        ->Writer.getCode,
      )
      |> toEqual(
           "let myFunc = (_inst: t, _arg01: string, _arg02: float, _arg03: bool): string => [%bs.raw {| _inst.myFunc(_arg01, _arg02, _arg03) |}];",
         );
    })
  );

  Expect.(
    test("writeFunctionDecl", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeFunctionDecl(
            makeFakeTsNode([||], "myFunc", SyntaxKind.FunctionDeclaration),
            [||],
            [|"myModule"|],
            [],
          )
        ->(((s, _)) => s)
        ->Writer.getCode,
      )
      |> toEqual(
           "[@bs.module \"myModule\"] external myFunc: (_arg01: string, _arg02: float, _arg03: bool) => string = \"myFunc\"",
         );
    })
  );

  Expect.(
    test("writeVariableDecl", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeVariableDecl(
            makeFakeTsNode([||], "myVar", SyntaxKind.VariableDeclaration),
            [||],
            [|"myModule"|],
            [],
          )
        ->(((s, _)) => s)
        ->Writer.getCode,
      )
      |> toEqual(
           "[@bs.module \"myModule\"] external myVar: string = \"myVar\"",
         );
    })
  );

  Expect.(
    test("writeBeginModuleFromNs", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeBeginModuleFromNs([|"aaa", "bbb", "ccc"|])
        ->Writer.writeEndModule
        ->Writer.getCode,
      )
      |> toEqual("\nmodule Ccc = {\n}");
    })
  );

  Expect.(
    test("writeBeginModuleFromType", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeBeginModuleFromType(
            makeFakeTsNode([||], "myClass", SyntaxKind.ClassDeclaration),
          )
        ->Writer.writeEndModule
        ->Writer.getCode,
      )
      |> toEqual("\nmodule MyClass = {\n}");
    })
  );

  Expect.(
    test("writeAbstractTypeDeclaration", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeAbstractTypeDeclaration(
            makeFakeTsNode([||], "myClass", SyntaxKind.ClassDeclaration),
          )
        ->Writer.getCode,
      )
      |> toEqual("type t = t_myClass;");
    })
  );
});
