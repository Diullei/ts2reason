open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertTypeAliasDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      state: Writer.writerState,
    ) => (
  state
  ->Writer.writeBeginModuleFromType(node)
  ->Writer.writeNewLine
  ->Writer.write("type t = ")
  ->Writer.writeType(node->TsNode.getType, types)
  ->Writer.write(";")
  ->Writer.writeEndModule
  ->Writer.writeNewLine,
  names,
);

let convertVariableDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      state: Writer.writerState,
    ) => {
  let (valName, names) =
    Utils.lowerFirst(node->TsNode.getName)->Utils.toUniqueName(names);

  let (setName, names) =
    ("set" ++ Utils.capitalize(node->TsNode.getName))
    ->Utils.toUniqueName(names);
  (
    state
    ->Writer.write("[@bs.val] external ")
    ->Writer.write(valName)
    ->Writer.write(": ")
    ->Writer.writeType(node->TsNode.getType, types)
    ->Writer.write(" = \"")
    ->Writer.write(node->TsNode.getName)
    ->Writer.write("\";")
    ->Writer.writeNewLine
    |> (
      state =>
        /* the set function will be generated if the
           declaration is not a 'const' declaration */
        if (!node->TsNode.isConst) {
          state
          ->Writer.write("let ")
          ->Writer.write(setName)
          ->Writer.write(" = (_value: ")
          ->Writer.writeType(node->TsNode.getType, types)
          ->Writer.write("): ")
          ->Writer.writeType(node->TsNode.getType, types)
          ->Writer.write(" => [%bs.raw {| ")
          ->Writer.write(node->TsNode.getName)
          ->Writer.write(" = _value |}];")
          ->Writer.writeNewLine;
        } else {
          state;
        }
    ),
    names,
  );
};

let convertCodeToReason = (code: string, state: Writer.writerState) => {
  let names: list(string) = [];
  let types = TsApi.extractTypesFromCode(code);
  types
  |> Array.fold_left(
       ((state, names), node) =>
         switch (node->TsNode.getKind) {
         | SyntaxKind.TypeAliasDeclaration =>
           node->convertTypeAliasDeclaration(types, names, state)

         | SyntaxKind.VariableDeclaration =>
           node->convertVariableDeclaration(types, names, state)

         | _ => (state->Writer.write("/* ** !INVALID NODE! ** */"), names)
         },
       (state, names),
     )
  |> (((s, _)) => s);
};