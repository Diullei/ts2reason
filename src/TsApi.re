open Ts;

type t_tsType;
type t_tsParameter;
type t_tsEnumMember;
type t_tsNode;

module TypeKind = {
  [@bs.deriving jsConverter]
  type t =
    | [@bs.as 0] Regular
    | [@bs.as 1] Array
    | [@bs.as 2] Tuple
    | [@bs.as 3] Union
    | [@bs.as 4] Intersection
    | [@bs.as 5] TypeLiteral;
};

module TsType = {
  type t = t_tsType;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getTypeKind: t => TypeKind.t = "typeKind";
  [@bs.get] external getArrayType: t => t = "arrayType";
  [@bs.get] external getTupleTypes: t => Js.Array.t(t) = "tupleTypes";
  [@bs.get] external getMembers: t => Js.Array.t(t_tsNode) = "members";
};

module TsParameter = {
  type t = t_tsParameter;

  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getType: t => TsType.t = "type";
};

module TsEnumMember = {
  type t = t_tsEnumMember;

  [@bs.get] external getName: t => string = "name";
};

module TsNode = {
  type t = t_tsNode;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getName: t => string = "name";
  [@bs.get] external getKind: t => SyntaxKind.t = "kind";
  [@bs.get] external getType: t => TsType.t = "type";
  [@bs.get] external isConst: t => bool = "isConst";
  [@bs.get]
  external getParameters: t => Js.Array.t(TsParameter.t) = "parameters";
  [@bs.get]
  external getEnumMembers: t => Js.Array.t(TsEnumMember.t) = "enumMembers";
  [@bs.get] external isOptional: t => bool = "optional";
};

[@bs.module "../src/tsApiBridge"]
external extractTypesFromFile: string => Js.Array.t(TsNode.t) =
  "extractTypesFromFile";

[@bs.module "../src/tsApiBridge"]
external extractTypesFromCode: string => Js.Array.t(TsNode.t) =
  "extractTypesFromCode";