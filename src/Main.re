open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertTypeAliasDeclaration =
    (node: TsNode.t, types: array(TsNode.t), state: Writer.writerState) =>
  state
  ->Writer.writeBeginModuleFromType(node)
  ->Writer.writeNewLine
  ->Writer.write("type t = ")
  ->Writer.writeType(node->TsNode.getType, types)
  ->Writer.write(";")
  ->Writer.writeEndModule
  ->Writer.writeNewLine;

let convertCodeToReason = (code: string, state: Writer.writerState) => {
  let types = TsApi.extractTypesFromCode(code);
  types
  |> Array.fold_left(
       (state, node) =>
         switch (node->TsNode.getKind) {
         | SyntaxKind.TypeAliasDeclaration =>
           node->convertTypeAliasDeclaration(types, state)

         | _ => state->Writer.write("/* ** !INVALID NODE! ** */")
         },
       state,
     );
};