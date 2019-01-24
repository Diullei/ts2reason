open Jest;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

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
        ->Writer.writeReasonType({
            ns: [|"aaa", "bbb"|],
            id: "ccc",
            kind: Types.SyntaxKind.ModuleDeclaration,
            node: None,
          })
        ->Writer.getCode,
      )
      |> toEqual("t_aaa_bbb_ccc");
    })
  );

  Expect.(
    testAll(
      "writeType",
      [
        ([%bs.raw {| { getText: () => "string" } |}], [||], "string"),
        ([%bs.raw {| { getText: () => "boolean" } |}], [||], "bool"),
        ([%bs.raw {| { getText: () => "number" } |}], [||], "float"),
        ([%bs.raw {| { getText: () => "xyz" } |}], [||], "t_TODO"),
        (
          [%bs.raw {| { getText: () => "MyObj" } |}],
          [|
            (
              {
                ns: [||],
                id: "MyObj",
                kind: Types.SyntaxKind.ModuleDeclaration,
                node: None,
              }: Types.TsNode.t
            ),
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
});