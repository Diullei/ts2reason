open Ts;

module TsType = {
  type t;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getName: t => string = "name";
  [@bs.get] external isArray: t => bool = "isArray";
  [@bs.get] external getArrayType: t => t = "arrayType";
};

module TsParameter = {
  type t;

  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getType: t => TsType.t = "type";
};

module TsNode = {
  type t;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getKind: t => SyntaxKind.t = "kind";
  [@bs.get] external getType: t => TsType.t = "type";
  [@bs.get] external getReturnType: t => TsType.t = "returnType";
  [@bs.get]
  external getParameters: t => Js.Array.t(TsParameter.t) = "parameters";
};

[@bs.module "../src/tsApiBridge"]
external extractTypesFromFile: string => Js.Array.t(TsNode.t) =
  "extractTypesFromFile";

[@bs.module "../src/tsApiBridge"]
external extractTypesFromCode: string => Js.Array.t(TsNode.t) =
  "extractTypesFromCode";