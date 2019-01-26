open TsApi;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let convertCodeToReason = (code: string, state: Writer.writerState) => {
  TsApi.extractTypesFromCode(code)
  |> Array.fold_left(
       (state, node) =>
         switch (node->TsNode.getKind) {
         | SyntaxKind.TypeAliasDeclaration =>
           state
           ->Writer.writeBeginModuleFromType(node)
           ->Writer.writeNewLine
           ->Writer.write("type t = ")
           ->Writer.write(
               node->TsNode.getNode->TypeKind.type_->TsType.getText,
             )
           ->Writer.write(";")
           ->Writer.writeEndModule
           ->Writer.writeNewLine

         | _ => state->Writer.write("/* ** !INVALID NODE! ** */")
         },
       state,
     );
};