open Jest;

[@bs.val] [@bs.module "os"] external eol: string = "EOL";

describe("Type alias declaration binding a predefined type :: string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type Typ = string")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module Typ = {
  type t = string;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: boolean", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyBoolType = boolean")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyBoolType = {
  type t = bool;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: any", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type myObjType = any")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyObjType = {
  type t = 'any0;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: unknown", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type myObjType = unknown")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyObjType = {
  type t = 'unknown0;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: void", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type myObjType = void")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyObjType = {
  type t = unit;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: number", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = number;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = float;
}

")
    )
  )
);

describe(
  "Type alias declaration binding a predefined type :: multline declaration",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "
type myBoolType = boolean;
type MyType = number;
",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "module MyBoolType = {
  type t = bool;
}

module MyType = {
  type t = float;
}

",
         )
    )
  )
);

describe("Type alias declaration binding a predefined type :: null", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = null;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Js.Types.null_val;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: undefined", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = undefined;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Js.Types.undefined_val;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: object", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = object;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Js.Types.obj_val;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: symbol", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = symbol;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Js.Types.symbol;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: bigint", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = bigint;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Int64.t;
}

")
    )
  )
);

describe("Type alias declaration binding a predefined type :: void", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = void;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = unit;
}

")
    )
  )
);

describe("Type alias declaration binding an array type :: string[]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = string[];")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = Js.Array.t(string);
}

")
    )
  )
);

describe("Type alias declaration binding an array type :: string[][]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = string[][];")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "module MyType = {
  type t = Js.Array.t(Js.Array.t(string));
}

",
         )
    )
  )
);

describe("Type alias declaration binding an array type :: number[][][]", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = number[][][];")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "module MyType = {
  type t = Js.Array.t(Js.Array.t(Js.Array.t(float)));
}

",
         )
    )
  )
);

describe(
  "Type alias declaration binding an array type :: [string, number, boolean]",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = [string, number, boolean];",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = (string, float, bool);
}

")
    )
  )
);

describe(
  "Type alias declaration binding an array type :: [string, number[], boolean]",
  () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = [string, number[], boolean];",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "module MyType = {
  type t = (string, Js.Array.t(float), bool);
}

",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("declare var a: number;")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: float = \"\";
let setA = (_value: float): float => [%bs.raw {| a = _value |}];

",
         )
    )
  )
);

describe("Variable declaration with no type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("declare var value;")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external value: 'any0 = \"\";
let setValue = (_value: 'any0): 'any0 => [%bs.raw {| value = _value |}];

",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: (let) string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("declare let a: number;")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: float = \"\";
let setA = (_value: float): float => [%bs.raw {| a = _value |}];

",
         )
    )
  )
);

describe("Variable declaration binding a simple type :: (const) string", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("declare const a: number;")
        |> Utils.checkReasonCode,
      )
      |> toEqual("[@bs.val] external a: float = \"\";

")
    )
  )
);

describe("Variable declaration list - simple type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("declare var a: boolean, b, c: number;")
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external a: bool = \"\";
let setA = (_value: bool): bool => [%bs.raw {| a = _value |}];

[@bs.val] external b: 'any0 = \"\";
let setB = (_value: 'any0): 'any0 => [%bs.raw {| b = _value |}];

[@bs.val] external c: float = \"\";
let setC = (_value: float): float => [%bs.raw {| c = _value |}];

",
         )
    )
  )
);

describe("Function declaration - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare function greet(greeting: string): void;",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual("[@bs.send] external greet: (string) => unit = \"\";

")
    )
  )
);

describe("Function declaration - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare function myFunc(arg1: string, arg2: boolean): number;",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.send] external myFunc: (string, bool) => float = \"\";

",
         )
    )
  )
);

describe("Function declaration - 003", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare function myFunc(arg1: string, arg2: boolean);",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.send] external myFunc: (string, bool) => 'any0 = \"\";

",
         )
    )
  )
);

describe("Enum declaration - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare enum EnumTyp { Val1, Val2, Val3, };",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_EnumTyp;

module EnumTyp = {
  type t = t_EnumTyp;
  
  let val1: t = [%bs.raw {| EnumTyp.Val1 |}];
  let val2: t = [%bs.raw {| EnumTyp.Val2 |}];
  let val3: t = [%bs.raw {| EnumTyp.Val3 |}];
  
  let name_ = (_key: t): option(string) =>
    switch ([%bs.raw {| EnumTyp[_key] |}]) {
    | Some(v) => Some(v)
    | _ => None
    };
  
  let fromName_ = (_name: string): option(t) =>
    switch ([%bs.raw {| EnumTyp[_name] |}]) {
    | Some(v) => Some(v)
    | _ => None
    };
}

",
         )
    )
  )
);

describe("Enum declaration - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "
enum FileAccess {
    // constant members
    None,
    Read    = 1 << 1,
    Write   = 1 << 2,
    ReadWrite  = Read | Write,
    // computed member
    G = \"123\".length
}
             ",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_FileAccess;

module FileAccess = {
  type t = t_FileAccess;
  
  let none: t = [%bs.raw {| FileAccess.None |}];
  let read: t = [%bs.raw {| FileAccess.Read |}];
  let write: t = [%bs.raw {| FileAccess.Write |}];
  let readWrite: t = [%bs.raw {| FileAccess.ReadWrite |}];
  let g: t = [%bs.raw {| FileAccess.G |}];
  
  let name_ = (_key: t): option(string) =>
    switch ([%bs.raw {| EnumTyp[_key] |}]) {
    | Some(v) => Some(v)
    | _ => None
    };
  
  let fromName_ = (_name: string): option(t) =>
    switch ([%bs.raw {| EnumTyp[_name] |}]) {
    | Some(v) => Some(v)
    | _ => None
    };
}

",
         )
    )
  )
);

describe("ParenthesizedType type :: (number)", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = (number);")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = float;
}

")
    )
  )
);

describe("ParenthesizedType type :: (((number)))", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason("type MyType = (((number)));")
        |> Utils.checkReasonCode,
      )
      |> toEqual("module MyType = {
  type t = float;
}

")
    )
  )
);

describe("Type alias declaration binding a literal type - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = { prop1: string; prop2: number; };",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_MyType;

module MyType = {
  type t = t_MyType;
  
  [@bs.get] external getProp1: (t) => string = \"prop1\";
  [@bs.send] external setProp1: (t, string) => string = \"prop1\";
  
  [@bs.get] external getProp2: (t) => float = \"prop2\";
  [@bs.send] external setProp2: (t, float) => float = \"prop2\";
}

",
         )
    )
  )
);

describe("Type alias declaration binding a literal type - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "
             type MyType = { prop1: string; prop2: number; };
             type MyType2 = { prop1: string; prop2: number; };",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_MyType2;
type t_MyType;

module MyType = {
  type t = t_MyType;
  
  [@bs.get] external getProp1: (t) => string = \"prop1\";
  [@bs.send] external setProp1: (t, string) => string = \"prop1\";
  
  [@bs.get] external getProp2: (t) => float = \"prop2\";
  [@bs.send] external setProp2: (t, float) => float = \"prop2\";
}

module MyType2 = {
  type t = t_MyType2;
  
  [@bs.get] external getProp1: (t) => string = \"prop1\";
  [@bs.send] external setProp1: (t, string) => string = \"prop1\";
  
  [@bs.get] external getProp2: (t) => float = \"prop2\";
  [@bs.send] external setProp2: (t, float) => float = \"prop2\";
}

",
         )
    )
  )
);

describe("Type alias declaration binding a literal type - 003", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
            "
            type GreetingSettings = {
              greeting: string;
              duration: number;
              color: string;
            };
            ",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_GreetingSettings;

module GreetingSettings = {
  type t = t_GreetingSettings;
  
  [@bs.get] external getGreeting: (t) => string = \"greeting\";
  [@bs.send] external setGreeting: (t, string) => string = \"greeting\";
  
  [@bs.get] external getDuration: (t) => float = \"duration\";
  [@bs.send] external setDuration: (t, float) => float = \"duration\";
  
  [@bs.get] external getColor: (t) => string = \"color\";
  [@bs.send] external setColor: (t, string) => string = \"color\";
}

",
         )
    )
  )
);

describe("Var declaration binding a literal type - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
            "
            const myVar: {
              greeting: string;
              duration: {
                name: string;
                isValid: boolean;
              };
              color: string;
            };
            ",
           )
        |> Utils.checkReasonCode
        ,
      )
      |> toEqual(
           "module MyVar = {
  type t;
  
  [@bs.get] external getGreeting: (t) => string = \"greeting\";
  [@bs.send] external setGreeting: (t, string) => string = \"greeting\";
  
  module MyVar_Duration = {
    type t;
    
    [@bs.get] external getName: (t) => string = \"name\";
    [@bs.send] external setName: (t, string) => string = \"name\";
    
    [@bs.get] external getIsValid: (t) => bool = \"isValid\";
    [@bs.send] external setIsValid: (t, bool) => bool = \"isValid\";
  }
  
  [@bs.get] external getDuration: (t) => MyVar_Duration.t = \"duration\";
  [@bs.send] external setDuration: (t, MyVar_Duration.t) => MyVar_Duration.t = \"duration\";
  
  [@bs.get] external getColor: (t) => string = \"color\";
  [@bs.send] external setColor: (t, string) => string = \"color\";
}

[@bs.val] external myVar: MyVar.t = \"\";

",
         )
    )
  )
);

describe("Var declaration binding a literal type - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
            "
            declare const myVar: {
                name: string;
                isValid: boolean;
            }[];
            ",
           )
        |> Utils.checkReasonCode
        ,
      )
      |> toEqual(
           "module MyVar = {
  type t;
  
  [@bs.get] external getName: (t) => string = \"name\";
  [@bs.send] external setName: (t, string) => string = \"name\";
  
  [@bs.get] external getIsValid: (t) => bool = \"isValid\";
  [@bs.send] external setIsValid: (t, bool) => bool = \"isValid\";
}

[@bs.val] external myVar: Js.Array.t(MyVar.t) = \"\";

",
         )
    )
  )
);

describe("Var declaration binding a literal type - 003", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
            "
            declare let myVar: {
                name: string[];
                post: {title: string; content: string}
            }[];
            ",
           )
        |> Utils.checkReasonCode
        ,
      )
      |> toEqual(
           "module MyVar = {
  type t;
  
  [@bs.get] external getName: (t) => Js.Array.t(string) = \"name\";
  [@bs.send] external setName: (t, Js.Array.t(string)) => Js.Array.t(string) = \"name\";
  
  module MyVar_Post = {
    type t;
    
    [@bs.get] external getTitle: (t) => string = \"title\";
    [@bs.send] external setTitle: (t, string) => string = \"title\";
    
    [@bs.get] external getContent: (t) => string = \"content\";
    [@bs.send] external setContent: (t, string) => string = \"content\";
  }
  
  [@bs.get] external getPost: (t) => MyVar_Post.t = \"post\";
  [@bs.send] external setPost: (t, MyVar_Post.t) => MyVar_Post.t = \"post\";
}

[@bs.val] external myVar: Js.Array.t(MyVar.t) = \"\";
let setMyVar = (_value: Js.Array.t(MyVar.t)): Js.Array.t(MyVar.t) => [%bs.raw {| myVar = _value |}];

",
         )
    )
  )
);

describe("Var declaration binding a literal type - 004", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
            "
            declare let myVar: {
                name: string[];
                greet(greeting: string): void;
            }[];
            ",
           )
        |> Utils.checkReasonCode
        ,
      )
      |> toEqual(
           "module MyVar = {
  type t;
  
  [@bs.get] external getName: (t) => Js.Array.t(string) = \"name\";
  [@bs.send] external setName: (t, Js.Array.t(string)) => Js.Array.t(string) = \"name\";
  
  [@bs.send] external greet: (string) => unit = \"\";
  
}

[@bs.val] external myVar: Js.Array.t(MyVar.t) = \"\";
let setMyVar = (_value: Js.Array.t(MyVar.t)): Js.Array.t(MyVar.t) => [%bs.raw {| myVar = _value |}];

",
         )
    )
  )
);

describe("Optional property - 001", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = { prop1?: string; prop2?: number; };",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_MyType;

module MyType = {
  type t = t_MyType;
  
  [@bs.get] external getProp1: (t) => option(string) = \"prop1\";
  [@bs.send] external setProp1: (t, string) => string = \"prop1\";
  
  [@bs.get] external getProp2: (t) => option(float) = \"prop2\";
  [@bs.send] external setProp2: (t, float) => float = \"prop2\";
}

",
         )
    )
  )
);

describe("Optional property - 002", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = { prop1: string; prop2?: number; };",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_MyType;

module MyType = {
  type t = t_MyType;
  
  [@bs.get] external getProp1: (t) => string = \"prop1\";
  [@bs.send] external setProp1: (t, string) => string = \"prop1\";
  
  [@bs.get] external getProp2: (t) => option(float) = \"prop2\";
  [@bs.send] external setProp2: (t, float) => float = \"prop2\";
}

",
         )
    )
  )
);

describe("Optional argument", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare function add(a, b?: string)",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.send] external add: ('any0, ~b: string=?) => 'any1 = \"\";

",
         )
    )
  )
);

describe("Literal type with an empty literal type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "type MyType = {};",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "type t_MyType = Js.Types.obj_val;

module MyType = {
  type t = t_MyType;
}

",
         )
    )
  )
);

describe("Var type with an empty literal type", () =>
  Expect.(
    test("convertCodeToReason", () =>
      expect(
        Writer.make(~nl=eol, ~code="", ~currentIdentation=0)
        |> Main.convertCodeToReason(
             "declare const myVar: {};",
           )
        |> Utils.checkReasonCode,
      )
      |> toEqual(
           "[@bs.val] external myVar: Js.Types.obj_val = \"\";

",
         )
    )
  )
);

