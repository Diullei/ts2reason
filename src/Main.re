open TsApi;
open Ts;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

let createTypeAssignDeclaration = (state: Writer.writerState, node: TsNode.t) =>
  state
  ->Writer.write("type t = t_")
  ->Writer.write(Utils.capitalize(node->TsNode.getName))
  ->Writer.write(";");

let createTypeDeclaration =
    (state: Writer.writerState, node: TsNode.t, types: array(TsNode.t)) =>
  state
  ->Writer.write("type t = ")
  ->Writer.writeType(node->TsNode.getType, types)
  ->Writer.write(";");

let createGetName = (node: TsNode.t, names: list(string)) =>
  ("get" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names);

let createSetName = (node: TsNode.t, names: list(string)) =>
  ("set" ++ Utils.capitalize(node->TsNode.getName))
  ->Utils.toUniqueName(names);

let createUniqueName = (name: string, names: list(string)) =>
  name->Utils.lowerFirst->Utils.toUniqueName(names);

let createMemberGetFunction =
    (
      state: Writer.writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) = node->createGetName(names);

  (
    state
    ->Writer.write({j|[@bs.get] external $name: (t) => |j})
    ->Writer.writeType(node->TsNode.getType, types)
    ->Writer.write(" = \"")
    ->Writer.write(node->TsNode.getName)
    ->Writer.write("\";"),
    names,
  );
};

let createMemberSetFunction =
    (
      state: Writer.writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (name, names) = node->createSetName(names);

  (
    state
    ->Writer.write({j|[@bs.send] external $name: (t, |j})
    ->Writer.writeType(node->TsNode.getType, types)
    ->Writer.write(") => ")
    ->Writer.writeType(node->TsNode.getType, types)
    ->Writer.write(" = \"")
    ->Writer.write(node->TsNode.getName)
    ->Writer.write("\";"),
    names,
  );
};

let createGetSetFunction =
    (
      state: Writer.writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      names: list(string),
    ) => {
  let (state, names) = state->createMemberGetFunction(node, types, names);
  let state = state->Writer.writeNewLine;
  let (state, names) = state->createMemberSetFunction(node, types, names);
  (state, names);
};

let convertTypeAliasDeclarationType =
    (
      state: Writer.writerState,
      node: TsNode.t,
      types: array(TsNode.t),
      _names: list(string),
    ) =>
  switch (node->TsNode.getType->TsType.getTypeKind) {
  | TypeKind.TypeLiteral =>
    state
    ->createTypeAssignDeclaration(node)
    ->(
        state =>
          node->TsNode.getType->TsType.getMembers
          |> Array.fold_left(
               ((state, memberNames), member) =>
                 state
                 ->Writer.writeNewLine
                 ->Writer.writeNewLine
                 ->createGetSetFunction(member, types, memberNames),
               (state, []),
             )
          |> (((s, _)) => s)
      )
  | _ => state->createTypeDeclaration(node, types)
  };

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
  ->convertTypeAliasDeclarationType(node, types, names)
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
  let (valName, names) = node->TsNode.getName->createUniqueName(names);
  let (setName, names) = node->createSetName(names);
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
  let (name, names) = node->TsNode.getName->createUniqueName(names);
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
  ->Writer.write("type t;")
  ->Writer.writeNewLine
  ->(
      state =>
        node->TsNode.getEnumMembers
        |> Array.fold_left(
             ((state, keyNames), enum) => {
               let (name, keyNames) =
                 enum->TsEnumMember.getName->createUniqueName(keyNames);
               (
                 state
                 ->Writer.writeNewLine
                 ->Writer.write("let ")
                 ->Writer.write(name)
                 ->Writer.write(": t = [%bs.raw {| ")
                 ->Writer.write(Utils.capitalize(node->TsNode.getName))
                 ->Writer.write(".")
                 ->Writer.write(enum->TsEnumMember.getName)
                 ->Writer.write(" |}];"),
                 keyNames,
               );
             },
             (state, ["name_", "fromName_"]),
           )
        |> (((s, _)) => s)
    )
  /* Enum helper functions. We are not writing as a continous
     string because we need to keep the indentations */
  ->Writer.writeNewLine
  ->Writer.writeNewLine
  ->Writer.write("let name_ = (_key: t): option(string) =>")
  ->Writer.increaseIndent
  ->Writer.writeNewLine
  ->Writer.write("switch ([%bs.raw {| EnumTyp[_key] |}]) {")
  ->Writer.writeNewLine
  ->Writer.write("| Some(v) => Some(v)")
  ->Writer.writeNewLine
  ->Writer.write("| _ => None")
  ->Writer.writeNewLine
  ->Writer.write("};")
  ->Writer.decreaseIndent
  ->Writer.writeNewLine
  ->Writer.writeNewLine
  ->Writer.write("let fromName_ = (_name: string): option(t) =>")
  ->Writer.increaseIndent
  ->Writer.writeNewLine
  ->Writer.write("switch ([%bs.raw {| EnumTyp[_name] |}]) {")
  ->Writer.writeNewLine
  ->Writer.write("| Some(v) => Some(v)")
  ->Writer.writeNewLine
  ->Writer.write("| _ => None")
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