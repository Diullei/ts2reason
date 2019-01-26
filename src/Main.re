open TsApi;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertTypeAliasDeclaration = (node: TsNode.t, state: Writer.writerState) =>
  state
  ->Writer.writeBeginModuleFromType(node)
  ->Writer.writeNewLine
  ->Writer.write("type t = ")
  ->Writer.write(node->TsNode.getNode->TypeKind.type_->TsType.getText)
  ->Writer.write(";")
  ->Writer.writeEndModule
  ->Writer.writeNewLine;

let convertCodeToReason = (code: string, state: Writer.writerState) => {
  TsApi.extractTypesFromCode(code)
  |> Array.fold_left(
       (state, node) =>
         switch (node->TsNode.getKind) {
         | SyntaxKind.TypeAliasDeclaration =>
           node->convertTypeAliasDeclaration(state)

         | _ => state->Writer.write("/* ** !INVALID NODE! ** */")
         },
       state,
     );
};
