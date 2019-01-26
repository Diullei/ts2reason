# Spec - TypeScript conversion to ReasonML

This file contains the specification of the conversions that should be applied in order to represent the TypeScript definition as a ReasonML bonding.

## TypeScript grammar

> This grammar will be extended as new features are implemented

```
TypeAliasDeclaration:
    'type' BindingIdentifier TypeParameters? '=' Type ';' 

Type:
    Union
    Intersection
    PrimaryType
    FunctionType
    ConstructorType

PrimaryType:
    ParenthesizedType
    PredefinedType
    TypeReference
    ObjectType
    ArrayType
    TupleType
    TypeQuery
    ThisType

PredefinedType:
    any
    number
    boolean
    string
    symbol
    void
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
