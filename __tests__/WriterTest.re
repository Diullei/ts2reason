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
      ((fakeObj, types, output)) => {
        let (strVal, _, _) =
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          ->Writer.buildType(
              "aaa",
              fakeObj,
              types,
              [],
              Writer.make(~nl=eol, ~code="", ~currentIdentation=0),
            );
        expect(strVal) |> toEqual(output);
      },
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
      "writeTypeArgumentsToFunctionDecl",
      [
        ([||], "()"),
        (
          [|
            makeFakeTsParDec("param01", "number"),
            makeFakeTsParDec("param02", "boolean"),
            makeFakeTsParDec("param03", "string"),
          |],
          "(float, bool, string)",
        ),
      ],
      ((parList, output)) => {
        let (writer, _, _) =
          Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
          ->Writer.writeTypeArgumentsToFunctionDecl(
              parList,
              [||],
              Writer.make(~nl=eol, ~code="", ~currentIdentation=0),
              [],
            );

        expect(writer->Writer.getCode) |> toEqual(output);
      },
    )
  );

  /* Expect.(
    test("writeModuleName", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeModuleName([|"aaa", "bbb", "ccc"|])
        ->Writer.getCode,
      )
      |> toEqual("Ccc");
    })
  ); */

  /* Expect.(
    test("writeBeginModuleFromNs", () => {
      let wState = Writer.make(~nl=eol, ~code="", ~currentIdentation=0);
      expect(
        wState
        ->Writer.writeBeginModuleFromNs([|"aaa", "bbb", "ccc"|])
        ->Writer.decreaseIndent
        ->Writer.writeNewLine
        ->Writer.write("}")
        ->Writer.getCode,
      )
      |> toEqual("\nmodule Ccc = {\n}");
    })
  ); */

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