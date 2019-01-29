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

let convertFunctionDeclaration =
    (
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
      state: Writer.writerState,
    ) => {
  let (name, names) =
    Utils.lowerFirst(node->TsNode.getName)->Utils.toUniqueName(names);

  let state =
    state
    ->Writer.write("[@bs.send] external ")
    ->Writer.write(name)
    ->Writer.write(": ")
    ->Writer.writeTypeArgumentsToFunctionDecl(
        node->TsNode.getParameters,
        types,
      )
    ->Writer.write(" => ")
    ->Writer.writeType(node->TsNode.getType, types)
    ->Writer.write(" = \"")
    ->Writer.write(node->TsNode.getName)
    ->Writer.write("\";")
    ->Writer.writeNewLine;

  (state, names);
};

let convertEnumDeclaration =
    (
      node: TsNode.t,
      _types: array(TsNode.t),
      names: list(string),
      state: Writer.writerState,
    ) => (
  state
  ->Writer.writeBeginModuleFromType(node)
  ->Writer.writeNewLine
  ->Writer.write("[@bs.deriving jsConverter]")
  ->Writer.writeNewLine
  ->Writer.write("type t =")
  ->Writer.increaseIndent
  ->(
      state =>
        node->TsNode.getEnumMembers
        |> Array.fold_left(
             ((state, i), enum) =>
               (
                 state
                 ->Writer.writeNewLine
                 ->Writer.write({j|| [@bs.as $i] |j})
                 ->Writer.write(enum->TsEnumMember.getName),
                 i + 1,
               ),
             (state, 0),
           )
        |> (((s, _)) => s)
    )
  ->Writer.write(";")
  ->Writer.decreaseIndent
  ->Writer.decreaseIndent
  ->Writer.writeNewLine
  ->Writer.increaseIndent
  ->Writer.writeNewLine
  ->Writer.write("let name = (v: t) =>")
  ->Writer.increaseIndent
  ->Writer.writeNewLine
  ->Writer.write("switch (v) {")
  ->(
      state =>
        node->TsNode.getEnumMembers
        |> Array.fold_left(
             (state, enum) =>
               state
               ->Writer.writeNewLine
               ->Writer.write("| ")
               ->Writer.write(enum->TsEnumMember.getName)
               ->Writer.write(" => \"")
               ->Writer.write(enum->TsEnumMember.getName)
               ->Writer.write("\""),
             state,
           )
    )
  ->Writer.writeNewLine
  ->Writer.write("};")
  ->Writer.decreaseIndent
  ->Writer.writeEndModule
  ->Writer.writeNewLine,
  names,
);

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

         | SyntaxKind.FunctionDeclaration =>
           node->convertFunctionDeclaration(types, names, state)

         | SyntaxKind.EnumDeclaration =>
           node->convertEnumDeclaration(types, names, state)

         | _ => (state->Writer.write("/* ** !INVALID NODE! ** */"), names)
         },
       (state, names),
     )
  |> (((s, _)) => s);
};