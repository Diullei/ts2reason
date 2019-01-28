# Spec - TypeScript conversion to ReasonML

This file contains the specification of the conversions that should be applied in order to represent the TypeScript definition as a ReasonML binding.

## TypeScript grammar

> This grammar will be extended as new features are implemented

```
 Status | feature
 -------+---------
        |
 [    ] | TypeAliasDeclaration:
 [    ] |     'type' BindingIdentifier TypeParameters? '=' Type ';'

 [    ] | Type:
 [    ] |     Union
 [    ] |     Intersection
 [    ] |     PrimaryType
 [    ] |     FunctionType
 [    ] |     ConstructorType

 [    ] | PrimaryType:
 [    ] |     ParenthesizedType
 [done] |     PredefinedType
 [    ] |     TypeReference
 [    ] |     ObjectType
 [done] |     ArrayType
 [done] |     TupleType
 [    ] |     TypeQuery
 [    ] |     ThisType

 [done] | PredefinedType:
 [done] |     any
 [done] |     number
 [done] |     boolean
 [done] |     string
 [done] |     symbol
 [done] |     void
 [done] |     null
 [done] |     undefined
 [done] |     never
 [done] |     object
 [done] |     bigint
```

## Type alias declaration binding a predefined type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' PredefinedType ';' 
```

Performs a simple binding of the javascript primitive types: `any`, `number`, `boolean`, `string`, `symbol` and `void`.

Example of a TypeScript declaration:

````typescript
type MyNameType = string;
````

Example of a ReasonML equivalent:

````reason
module MyNameType = {
    type t = string;
}
````

### The "any" type

In TypeScript, the `any` type is used to describe dynamic values. To represent that type in a `PredefinedType` declaration on ReasonML we can use a generic type:

Example of a TypeScript declaration:

````typescript
type MyObjType = any;
````

Example of a ReasonML equivalent:

````reason
module MyObjType = {
    type t = 'any;
}
````

### The "void" type

In TypeScript, `void` represents the absence of having a type. It's commonly used as the return type of functions that do not return a value. The equivalent in ReasonML is the `unit` type.

Example of a TypeScript declaration:

````typescript
type MyObjType = void;
````

Example of a ReasonML equivalent:

````reason
module MyObjType = {
    type t = unit;
}
````

### Types: "symbol", "null", "undefined", "object" and "biting"

The types `symbol`, `null`, `undefined` and `object` wull follow the following conversion:

 - `symbol` -> `Js.Types.symbol`
 - `null` -> `Js.Types.null_val`
 - `undefined` -> `Js.Types.undefined_val`
 - `object` -> `Js.Types.obj_val`
 - `biting` -> `Int64.t`

### The "never" type

In TypeScript, the `never` type represents the type of values that never occur. For now we will use the `unit` type to represent it in ReasonML.

Example of a TypeScript declaration:

````typescript
type MyObjType = never;
````

Example of a ReasonML equivalent:

````reason
module MyObjType = {
    type t = unit;
}
````

## Type alias declaration binding an array type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' ArrayType ';' 
```

Performs a simple binding of an array type. In ReasonML we will use the `Js.Array.t('t)` type.

Example of a TypeScript declaration:

````typescript
type MyType = string[];
````

Example of a ReasonML equivalent:

````reason
module MyType = {
    type t = Js.Array.t(string);
}
````

## Type alias declaration binding a tuple type

```
TypeAliasDeclaration:
    'type' BindingIdentifier '=' TupleType ';' 
```

Performs a simple binding of a tuple type.

Example of a TypeScript declaration:

````typescript
type MyType = [string, number, boolean];
````

Example of a ReasonML equivalent:

````reason
module MyType = {
    type t = (string, int, bool);
}
````

---

> See the [api tests](./__tests__/MainTest.re) for more details.