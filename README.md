# ts2reason

Automatic generation of ReasonML bindings using TypeScript definitions.

> ** THIS IS A WORK IN PROGRESS PROJECT **

I just started to learn ReasonML and something that I'm missing is the ability to find definitions for popular javascript libraries as we have for TypeScript on DefinitelyTyped.

Inspired by the [ts2fable](https://github.com/fable-compiler/ts2fable) project, my ambition with this project is to create a tool to generate ReasonML bindings from TypeScript definition.

Do not expect to find something usable now, but feel free to [contribute](https://github.com/Diullei/ts2reason/issues/1) :)

## Spec - TypeScript conversion to ReasonML

This section contains the specification of the conversions that should be applied in order to represent the TypeScript definition as a ReasonML binding.

### TypeScript grammar

> This grammar will be extended as new features are implemented

```
 Status | Feature
 -------+--------
        |
 [    ] | TypeAliasDeclaration:
 [    ] |     'type' BindingIdentifier TypeParameters? '=' Type ';'
        |
 [    ] | Type:
 [    ] |     Union
 [    ] |     Intersection
 [    ] |     PrimaryType
 [    ] |     FunctionType
 [    ] |     ConstructorType
        |
 [    ] | PrimaryType:
 [done] |     ParenthesizedType
 [done] |     PredefinedType
 [    ] |     TypeReference
 [    ] |     ObjectType
 [done] |     ArrayType
 [done] |     TupleType
 [    ] |     TypeQuery
 [    ] |     ThisType
        |
 [    ] | ParenthesizedType:
 [    ] |     '(' Type ')'
        |
 [done] | PredefinedType:
 [done] |     'any'
 [done] |     'number'
 [done] |     'boolean'
 [done] |     'string'
 [done] |     'symbol'
 [done] |     'void'
 [done] |     'null'
 [done] |     'undefined'
 [done] |     'never'
 [done] |     'object'
 [done] |     'bigint'
 [done] |     'unknown'
        |
 [    ] | AmbientDeclaration:
 [done] |     'declare' AmbientVariableDeclaration
 [done] |     'declare' AmbientFunctionDeclaration
 [    ] |     'declare' AmbientClassDeclaration
 [    ] |     'declare' InterfaceDeclaration
 [    ] |     'declare' AmbientEnumDeclaration
 [    ] |     'declare' AmbientNamespaceDeclaration
        |
 [done] | AmbientVariableDeclaration:
 [done] |     'var' AmbientBindingList ';'
 [done] |     'let' AmbientBindingList ';'
 [done] |     'const' AmbientBindingList ';'
        |
 [done] | AmbientBindingList:
 [done] |     AmbientBinding
 [done] |     AmbientBindingList ',' AmbientBinding
        |
 [done] | AmbientBinding:
 [done] |     BindingIdentifier TypeAnnotation?
        |
 [done] | AmbientFunctionDeclaration:
 [done] |     'function' BindingIdentifier CallSignature ';'
        |
 [done] | CallSignature:
 [done] |     TypeParameter? '(' ParameterList? ')' TypeAnnotation?
        |
 [done] | AmbientEnumDeclaration:
 [done] |    EnumDeclaration
        |
 [done] | EnumDeclaration:
 [done] |     'const'? 'enum' BindingIdentifier '{' EnumBody? '}'
        |
 [done] | EnumBody:
 [done] |     EnumMemberList ',' ?
        |
 [done] | EnumMemberList:
 [done] |     EnumMember
 [done] |     EnumMemberList ',' EnumMember
        |
 [done] | EnumMember:
 [done] |     PropertyName
 [done] |     PropertyName '=' EnumValue
        |
 [done] | EnumValue:
 [done] |     AssignmentExpression
        |
 [    ] | InterfaceDeclaration:
 [    ] |     'interface' BindingIdentifier TypeParameters? InterfaceExtendsClause? ObjectType
        |
 [    ] | InterfaceExtendsClause:
 [    ] |     'extends' ClassOrInterfaceTypeList
        |
 [    ] | ClassOrInterfaceTypeList:
 [    ] |     ClassOrInterfaceType
 [    ] |     ClassOrInterfaceTypeList ',' ClassOrInterfaceType
        |
 [    ] | ObjectType:
 [    ] |     '{' TypeBody? '}'
        |
 [    ] | TypeBody:
 [    ] |     TypeMemberList ';'?
 [    ] |     TypeMemberList ','?
        |
 [    ] | TypeMemberList:
 [    ] |     TypeMember
 [    ] |     TypeMemberList ';' TypeMember
 [    ] |     TypeMemberList ',' TypeMember
        |
 [    ] | TypeMember:
 [    ] |     PropertySignature
 [    ] |     CallSignature
 [    ] |     ConstructSignature
 [    ] |     IndexSignature
 [    ] |     MethodSignature
```

### Type alias declaration binding a predefined type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' PredefinedType ';' 
```

Performs a simple binding of the javascript primitive types: `any`, `number`, `boolean`, `string`, `symbol` and `void`.

Example of a TypeScript declaration:

````typescript
type MyNameType = string;
````

ReasonML equivalent code:

````reason
module MyNameType = {
    type t = string;
}
````

#### The "any" type

In TypeScript, the `any` type is used to describe dynamic values. To represent that type in a `PredefinedType` declaration on ReasonML we can use a generic type:

Example of a TypeScript declaration:

````typescript
type MyObjType = any;
````

ReasonML equivalent code:

````reason
module MyObjType = {
    type t = 'any;
}
````

#### The "unknown" type

In TypeScript the `unknown` type is the type-safe counterpart of `any`. Anything is assignable to unknown, but unknown isn’t assignable to anything but itself and any without a type assertion or a control flow based narrowing. For now the equivalent type in ReasonML will be a generic type (`'unknown`).

Example of a TypeScript declaration:

````typescript
type MyObjType = unknown;
````

ReasonML equivalent code:

````reason
module MyObjType = {
    type t = 'unknown;
}
````

#### The "void" type

In TypeScript, `void` represents the absence of having a type. It's commonly used as the return type of functions that do not return a value. The equivalent in ReasonML is the `unit` type.

Example of a TypeScript declaration:

````typescript
type MyObjType = void;
````

ReasonML equivalent code:

````reason
module MyObjType = {
    type t = unit;
}
````

#### Types: "symbol", "null", "undefined", "object" and "biting"

The types `symbol`, `null`, `undefined` and `object` wull follow the following conversion:

 - `symbol` -> `Js.Types.symbol`
 - `null` -> `Js.Types.null_val`
 - `undefined` -> `Js.Types.undefined_val`
 - `object` -> `Js.Types.obj_val`
 - `biting` -> `Int64.t`

#### The "never" type

In TypeScript, the `never` type represents the type of values that never occur. For now we will use the `unit` type to represent it in ReasonML.

Example of a TypeScript declaration:

````typescript
type MyObjType = never;
````

ReasonML equivalent code:

````reason
module MyObjType = {
    type t = unit;
}
````

### Type alias declaration binding an array type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' ArrayType ';' 
```

Performs a simple binding of an array type. In ReasonML we will use the `Js.Array.t('t)` type.

Example of a TypeScript declaration:

````typescript
type MyType = string[];
````

ReasonML equivalent code:

````reason
module MyType = {
    type t = Js.Array.t(string);
}
````

### Type alias declaration binding a tuple type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' TupleType ';' 
```

Performs a simple binding of a tuple type.

Example of a TypeScript declaration:

````typescript
type MyType = [string, number, boolean];
````

ReasonML equivalent code:

````reason
module MyType = {
    type t = (string, int, bool);
}
````

## Type alias declaration with an object block

Example of a TypeScript declaration:

````typescript
type MyType = {
    prop1: string;
    prop2: number;
};
````

ReasonML equivalent code:

````reason
type t_MyType;

module MyType = {
    type t = t_MyType;

    [@bs.get] external getProp1: (t) => string = "prop1";
    [@bs.send] external setProp1: (t, string) => string = "prop1";

    [@bs.get] external getProp2: (t) => string = "prop2";
    [@bs.send] external setProp2: (t, string) => string = "prop2";
}
````

### Ambient variable declaration

```
AmbientVariableDeclaration:
    'declare' 'var' AmbientBindingList ';'
    'declare' 'let' AmbientBindingList ';'
    'declare' 'const' AmbientBindingList ';'
```

When representing an ambient variable declaration, we will use the `@bs.val` attribute for read the variable value. If the variable is not a constant the code needs to allow one to update the variable value. To do that we will generate an update function (set):

Example of a TypeScript declaration:

````typescript
declare let value1: number;
declare const value2: number;
````

ReasonML equivalent code:

````reason
[@bs.val] external value1: float = "value1";
let setValue1 = (_value: float): float => [%bs.raw {| value1 = _value |}];

[@bs.val] external value2: float = "value2";
````

> NOTE: This declaration is an experimental approach.

### Ambient function declaration

The `[@bs.send]` attribute will be used to declare the reasonml function.

```
AmbientFunctionDeclaration:
    'declare' 'function' BindingIdentifier CallSignature ';'

CallSignature:
    TypeParameter? '(' ParameterList? ')' TypeAnnotation?
```

Example of a TypeScript declaration:

````typescript
declare function greet(greeting: string): void;
````

ReasonML equivalent code:

````reason
[@bs.send] external greet: (string) => unit = "greet";
````

### Enum declaration

In TypeScript enums are used to create named constants.

```
EnumDeclaration:
    'declare' 'const'? 'enum' BindingIdentifier '{' EnumMemberList '}'
```

In TypeScript we can assign an expression value to each of the enum keys. Example:

```typescript
declare enum Enum1 {
    No = 0,
    Yes = 1,
    Cancel = "Cancel",
}

declare enum Enum2 {
    None = 0,
    Other = 1 + 3 + 4 * 6,
}
```

One alternative that I normally use when I'm writing some bindings is to write the `enum` as a ReasonML `variant`:

Example of a TypeScript declaration:

````typescript
declare enum EnumTyp { 
    Val1 = 0, 
    Val2 = 1, 
    Val3 = 2, 
};
````

ReasonML equivalent:

````reason
module EnumTyp = {
  [@bs.deriving jsConverter]
  type t =
    | [@bs.as 0] Val1
    | [@bs.as 1] Val2
    | [@bs.as 2] Val3;
}
````

The problem here is that by design the `[@bs.as ...]` attrbute will not allow us to defined all the possible expressions from the TypeScript side. One alternative that I'm trying to use on `ts2reason` is to create the `enum` equivalent as a regular module type. See the next example.

TypeScript declaration using static expressions as value:

````typescript
declare enum EnumTyp { 
    Val1 = 0, 
    Val2 = 1 + 3, 
    Val3 = 2 + a + 4, 
};
````

ReasonML equivalent:

````reason
module EnumTyp = {
  type t;

  let val1: t = [%bs.raw {| EnumTyp.Val1 |}];
  let val2: t = [%bs.raw {| EnumTyp.Val2 |}];
  let val3: t = [%bs.raw {| EnumTyp.Val3 |}];
  // ...
};
````

It is not a beautful design bot it allows to recreate any `enum` type, no metter with kind of expression it has.

Continuing with the TypeScript `enum` notation, if we want to get the `enum` property name as a string we can use:

````typescript
declare enum EnumTyp { 
    Val1, 
    Val2, 
    Val3, 
};

console.log(EnumTyp[EnumTyp.Val2])
// => "Val2"

console.log(EnumTyp["Val2"])
// => EnumTyp.Val2
````

To have the equivalent behaviour on ReasonML side the code will generate two aditional functions `name_(...)` and `fromName_(...)`:

````reason
module EnumTyp = {
  type t;

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
};

EnumTyp.val2->EnumTyp.name_ |> Js.log;
/* => "Val2" */

"Val2"->EnumTyp.fromName_ |> Js.log;
/* => EnumTyp.Val2 */
````

---

> See the [api tests](./__tests__/MainTest.re) for more details.

