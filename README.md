# ts2reason

Automatically generation of ReasonML bindings using TypeScript definitions.

> ** THIS IS A WORKING IN PROGRESS PROJECT **

I just started to learn ReeasonML and something that I'm missing is the ability to find definitions for popular javascript libraries as we have for TypeScript on DefinitelyTyped. Looking on Github I didn't find a tool to convert my TypeScript definition files to ReasonML bindings. 

My ambition with this project is to help to create a tool to generate ReasonML bindings for any kind of TypeScript definition.

TypeScript and ReasonML are totally different languages and create this binding generation tool is a big challenge. To fill the gap between those languages my intention is to generate the reason bindings using the ReasonML "idiomatic way" instead to try to make an inline translation of the files.

*NOTE:* By convenience, I started this project in TypeScript. I'll convert the code to ReasonML soon.

## Basic conversion approach

Typescript input (*.d.ts file)

```typescript
declare interface Company {
    owner: Person;
    address: any;
    getOwnerFullName(): string;
}

declare interface Person {
    firstName: string;
    lastName: string;
    age: number;
}
```

...which generates:

```reasonml
type t_TODO;
type t_Company;
type t_Person;

module Company = {
  type t = t_Company;
  
  [@bs.get] external getOwner: t => t_Person = "owner";
  [@bs.send] external setOwner: t => t_Person => unit = "owner";
  
  [@bs.get] external getAddress: t => t_TODO = "address";
  [@bs.send] external setAddress: t => t_TODO => unit = "address";
  
  [@bs.send.pipe: t] external ownerFullName: t => string = "ownerFullName";
}

module Person = {
  type t = t_Person;
  
  [@bs.get] external getFirstName: t => string = "firstName";
  [@bs.send] external setFirstName: t => string => unit = "firstName";
  
  [@bs.get] external getLastName: t => string = "lastName";
  [@bs.send] external setLastName: t => string => unit = "lastName";
  
  [@bs.get] external getAge: t => float = "age";
  [@bs.send] external setAge: t => float => unit = "age";
  
  
  let make: (~firstName: string, ~lastName: string, ~age: float) => t = [%bs.raw {| 
    function (firstName, lastName, age) {
      return {
        firstName: firstName, 
        lastName: lastName, 
        age: age
      }
    }
  |}]
}
```

About the generated code above:

* The `t_TODO` type has been declared to fill the cases that are not supported yet. See the `any` type used to define the `address` property.

* As we can see TypeScript allows us to use a type name that will be declared after. See the `Company` interface that has the property `owner: Person` where the `Person` interface is been defined after the `Company` interface. To address this reference problem, the script is generating the abstract types in the top, this allows us to use the type `t_Person` inside the `Company` module no matter the modules order.

* The binding code is been created with a `get` and `set` function for each object property.

* When an interface has just properties the code generates a `make(...)` function to be used as a constructor.

The code above could be used like this:

```reasonml
let person = Person.make(~firstName="Diullei", ~lastName="Gomes", ~age=34.0)
person->Person.getFirstName |> Js.log
```

Another example:

```typescript
declare module "myModule" {
    interface Person {
        firstName: string;
        lastName: string;
        age: number;
        giveMeSomeNumber(): number;
    }
}

declare interface Setting {
    value1: string;
    value2: number;
}
```

...which generates:

```reasonml
type t_TODO;
type t_Setting;
type t_myModule_Person;


module Setting = {
  type t = t_Setting;
  
  [@bs.get] external getValue1: t => string = "value1";
  [@bs.send] external setValue1: t => string => unit = "value1";
  
  [@bs.get] external getValue2: t => float = "value2";
  [@bs.send] external setValue2: t => float => unit = "value2";
  
  
  let make: (~value1: string, ~value2: float) => t = [%bs.raw {| 
    function (value1, value2) {
      return {
        value1: value1, 
        value2: value2
      }
    }
  |}]
}

module MyModule = {
  
  module Person = {
    type t = t_myModule_Person;
    
    [@bs.get] external getFirstName: t => string = "firstName";
    [@bs.send] external setFirstName: t => string => unit = "firstName";
    
    [@bs.get] external getLastName: t => string = "lastName";
    [@bs.send] external setLastName: t => string => unit = "lastName";
    
    [@bs.get] external getAge: t => float = "age";
    [@bs.send] external setAge: t => float => unit = "age";
    
    [@bs.send.pipe: t] external giveMeSomeNumber: t => float = "giveMeSomeNumber";
  }
}
```

## How to use

If you want to explore those examples you can follow the commands below:

    yarn install
    node_modules/.bin/ts-node main.ts example/example01.d.ts > example/Exemple01.re

This will generate the binding for the `example/example01.d.ts` as the `example/Exemple01.re` file.

*NOTE:* Don't expect to find something usable now, but feel free to contribute with a PR or creating an issue.