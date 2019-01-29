open Ts;

module TypeKind = {
  [@bs.deriving jsConverter]
  type t =
    | [@bs.as 0] Regular
    | [@bs.as 1] Array
    | [@bs.as 2] Tuple
    | [@bs.as 3] Union
    | [@bs.as 4] Intersection
}

module TsType = {
  type t;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getTypeKind: t => TypeKind.t = "typeKind";
  [@bs.get] external getArrayType: t => t = "arrayType";
  [@bs.get] external getTupleTypes: t => Js.Array.t(t) = "tupleTypes";
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
  [@bs.get]
  external getParameters: t => Js.Array.t(TsParameter.t) = "parameters";
  [@bs.get] external isConst: t => bool = "isConst";
};

[@bs.module "../src/tsApiBridge"]
external extractTypesFromFile: string => Js.Array.t(TsNode.t) =
  "extractTypesFromFile";

[@bs.module "../src/tsApiBridge"]
external extractTypesFromCode: string => Js.Array.t(TsNode.t) =
  "extractTypesFromCode";