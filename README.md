# ts2reason

Automatically generation of ReasonML bindings using TypeScript definitions.

> ** THIS IS A WORK IN PROGRESS PROJECT **

I just started to learn ReasonML and something that I'm missing is the ability to find definitions for popular javascript libraries as we have for TypeScript on DefinitelyTyped.

Inspired by the [ts2fable](https://github.com/fable-compiler/ts2fable) project, my ambition with this project is to create a tool to generate ReasonML bindings from TypeScript definition.

*NOTE:* By convenience, I started this project in TypeScript. I'm working to convert the code to ReasonML

## How to use

If you want to explore the examples you can follow the commands below:

    yarn install
    node_modules/.bin/ts-node src/main.ts example/example01.d.ts > example/Exemple01.re

This will generate the `example/Exemple01.re` file. Please, don't expect to find something usable now, but feel free to contribute :)