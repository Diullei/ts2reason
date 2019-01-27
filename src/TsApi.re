module SyntaxKind = {
  [@bs.deriving jsConverter]
  type t =
    | [@bs.as 0] Unknown
    | [@bs.as 1] EndOfFileToken
    | [@bs.as 2] SingleLineCommentTrivia
    | [@bs.as 3] MultiLineCommentTrivia
    | [@bs.as 4] NewLineTrivia
    | [@bs.as 5] WhitespaceTrivia
    | [@bs.as 6] ShebangTrivia
    | [@bs.as 7] ConflictMarkerTrivia
    | [@bs.as 8] NumericLiteral
    | [@bs.as 9] BigIntLiteral
    | [@bs.as 10] StringLiteral
    | [@bs.as 11] JsxText
    | [@bs.as 12] JsxTextAllWhiteSpaces
    | [@bs.as 13] RegularExpressionLiteral
    | [@bs.as 14] NoSubstitutionTemplateLiteral
    | [@bs.as 15] TemplateHead
    | [@bs.as 16] TemplateMiddle
    | [@bs.as 17] TemplateTail
    | [@bs.as 18] OpenBraceToken
    | [@bs.as 19] CloseBraceToken
    | [@bs.as 20] OpenParenToken
    | [@bs.as 21] CloseParenToken
    | [@bs.as 22] OpenBracketToken
    | [@bs.as 23] CloseBracketToken
    | [@bs.as 24] DotToken
    | [@bs.as 25] DotDotDotToken
    | [@bs.as 26] SemicolonToken
    | [@bs.as 27] CommaToken
    | [@bs.as 28] LessThanToken
    | [@bs.as 29] LessThanSlashToken
    | [@bs.as 30] GreaterThanToken
    | [@bs.as 31] LessThanEqualsToken
    | [@bs.as 32] GreaterThanEqualsToken
    | [@bs.as 33] EqualsEqualsToken
    | [@bs.as 34] ExclamationEqualsToken
    | [@bs.as 35] EqualsEqualsEqualsToken
    | [@bs.as 36] ExclamationEqualsEqualsToken
    | [@bs.as 37] EqualsGreaterThanToken
    | [@bs.as 38] PlusToken
    | [@bs.as 39] MinusToken
    | [@bs.as 40] AsteriskToken
    | [@bs.as 41] AsteriskAsteriskToken
    | [@bs.as 42] SlashToken
    | [@bs.as 43] PercentToken
    | [@bs.as 44] PlusPlusToken
    | [@bs.as 45] MinusMinusToken
    | [@bs.as 46] LessThanLessThanToken
    | [@bs.as 47] GreaterThanGreaterThanToken
    | [@bs.as 48] GreaterThanGreaterThanGreaterThanToken
    | [@bs.as 49] AmpersandToken
    | [@bs.as 50] BarToken
    | [@bs.as 51] CaretToken
    | [@bs.as 52] ExclamationToken
    | [@bs.as 53] TildeToken
    | [@bs.as 54] AmpersandAmpersandToken
    | [@bs.as 55] BarBarToken
    | [@bs.as 56] QuestionToken
    | [@bs.as 57] ColonToken
    | [@bs.as 58] AtToken
    | [@bs.as 59] EqualsToken
    | [@bs.as 60] PlusEqualsToken
    | [@bs.as 61] MinusEqualsToken
    | [@bs.as 62] AsteriskEqualsToken
    | [@bs.as 63] AsteriskAsteriskEqualsToken
    | [@bs.as 64] SlashEqualsToken
    | [@bs.as 65] PercentEqualsToken
    | [@bs.as 66] LessThanLessThanEqualsToken
    | [@bs.as 67] GreaterThanGreaterThanEqualsToken
    | [@bs.as 68] GreaterThanGreaterThanGreaterThanEqualsToken
    | [@bs.as 69] AmpersandEqualsToken
    | [@bs.as 70] BarEqualsToken
    | [@bs.as 71] CaretEqualsToken
    | [@bs.as 72] Identifier
    | [@bs.as 73] BreakKeyword
    | [@bs.as 74] CaseKeyword
    | [@bs.as 75] CatchKeyword
    | [@bs.as 76] ClassKeyword
    | [@bs.as 77] ConstKeyword
    | [@bs.as 78] ContinueKeyword
    | [@bs.as 79] DebuggerKeyword
    | [@bs.as 80] DefaultKeyword
    | [@bs.as 81] DeleteKeyword
    | [@bs.as 82] DoKeyword
    | [@bs.as 83] ElseKeyword
    | [@bs.as 84] EnumKeyword
    | [@bs.as 85] ExportKeyword
    | [@bs.as 86] ExtendsKeyword
    | [@bs.as 87] FalseKeyword
    | [@bs.as 88] FinallyKeyword
    | [@bs.as 89] ForKeyword
    | [@bs.as 90] FunctionKeyword
    | [@bs.as 91] IfKeyword
    | [@bs.as 92] ImportKeyword
    | [@bs.as 93] InKeyword
    | [@bs.as 94] InstanceOfKeyword
    | [@bs.as 95] NewKeyword
    | [@bs.as 96] NullKeyword
    | [@bs.as 97] ReturnKeyword
    | [@bs.as 98] SuperKeyword
    | [@bs.as 99] SwitchKeyword
    | [@bs.as 100] ThisKeyword
    | [@bs.as 101] ThrowKeyword
    | [@bs.as 102] TrueKeyword
    | [@bs.as 103] TryKeyword
    | [@bs.as 104] TypeOfKeyword
    | [@bs.as 105] VarKeyword
    | [@bs.as 106] VoidKeyword
    | [@bs.as 107] WhileKeyword
    | [@bs.as 108] WithKeyword
    | [@bs.as 109] ImplementsKeyword
    | [@bs.as 110] InterfaceKeyword
    | [@bs.as 111] LetKeyword
    | [@bs.as 112] PackageKeyword
    | [@bs.as 113] PrivateKeyword
    | [@bs.as 114] ProtectedKeyword
    | [@bs.as 115] PublicKeyword
    | [@bs.as 116] StaticKeyword
    | [@bs.as 117] YieldKeyword
    | [@bs.as 118] AbstractKeyword
    | [@bs.as 119] AsKeyword
    | [@bs.as 120] AnyKeyword
    | [@bs.as 121] AsyncKeyword
    | [@bs.as 122] AwaitKeyword
    | [@bs.as 123] BooleanKeyword
    | [@bs.as 124] ConstructorKeyword
    | [@bs.as 125] DeclareKeyword
    | [@bs.as 126] GetKeyword
    | [@bs.as 127] InferKeyword
    | [@bs.as 128] IsKeyword
    | [@bs.as 129] KeyOfKeyword
    | [@bs.as 130] ModuleKeyword
    | [@bs.as 131] NamespaceKeyword
    | [@bs.as 132] NeverKeyword
    | [@bs.as 133] ReadonlyKeyword
    | [@bs.as 134] RequireKeyword
    | [@bs.as 135] NumberKeyword
    | [@bs.as 136] ObjectKeyword
    | [@bs.as 137] SetKeyword
    | [@bs.as 138] StringKeyword
    | [@bs.as 139] SymbolKeyword
    | [@bs.as 140] TypeKeyword
    | [@bs.as 141] UndefinedKeyword
    | [@bs.as 142] UniqueKeyword
    | [@bs.as 143] UnknownKeyword
    | [@bs.as 144] FromKeyword
    | [@bs.as 145] GlobalKeyword
    | [@bs.as 146] BigIntKeyword
    | [@bs.as 147] OfKeyword
    | [@bs.as 148] QualifiedName
    | [@bs.as 149] ComputedPropertyName
    | [@bs.as 150] TypeParameter
    | [@bs.as 151] Parameter
    | [@bs.as 152] Decorator
    | [@bs.as 153] PropertySignature
    | [@bs.as 154] PropertyDeclaration
    | [@bs.as 155] MethodSignature
    | [@bs.as 156] MethodDeclaration
    | [@bs.as 157] Constructor
    | [@bs.as 158] GetAccessor
    | [@bs.as 159] SetAccessor
    | [@bs.as 160] CallSignature
    | [@bs.as 161] ConstructSignature
    | [@bs.as 162] IndexSignature
    | [@bs.as 163] TypePredicate
    | [@bs.as 164] TypeReference
    | [@bs.as 165] FunctionType
    | [@bs.as 166] ConstructorType
    | [@bs.as 167] TypeQuery
    | [@bs.as 168] TypeLiteral
    | [@bs.as 169] ArrayType
    | [@bs.as 170] TupleType
    | [@bs.as 171] OptionalType
    | [@bs.as 172] RestType
    | [@bs.as 173] UnionType
    | [@bs.as 174] IntersectionType
    | [@bs.as 175] ConditionalType
    | [@bs.as 176] InferType
    | [@bs.as 177] ParenthesizedType
    | [@bs.as 178] ThisType
    | [@bs.as 179] TypeOperator
    | [@bs.as 180] IndexedAccessType
    | [@bs.as 181] MappedType
    | [@bs.as 182] LiteralType
    | [@bs.as 183] ImportType
    | [@bs.as 184] ObjectBindingPattern
    | [@bs.as 185] ArrayBindingPattern
    | [@bs.as 186] BindingElement
    | [@bs.as 187] ArrayLiteralExpression
    | [@bs.as 188] ObjectLiteralExpression
    | [@bs.as 189] PropertyAccessExpression
    | [@bs.as 190] ElementAccessExpression
    | [@bs.as 191] CallExpression
    | [@bs.as 192] NewExpression
    | [@bs.as 193] TaggedTemplateExpression
    | [@bs.as 194] TypeAssertionExpression
    | [@bs.as 195] ParenthesizedExpression
    | [@bs.as 196] FunctionExpression
    | [@bs.as 197] ArrowFunction
    | [@bs.as 198] DeleteExpression
    | [@bs.as 199] TypeOfExpression
    | [@bs.as 200] VoidExpression
    | [@bs.as 201] AwaitExpression
    | [@bs.as 202] PrefixUnaryExpression
    | [@bs.as 203] PostfixUnaryExpression
    | [@bs.as 204] BinaryExpression
    | [@bs.as 205] ConditionalExpression
    | [@bs.as 206] TemplateExpression
    | [@bs.as 207] YieldExpression
    | [@bs.as 208] SpreadElement
    | [@bs.as 209] ClassExpression
    | [@bs.as 210] OmittedExpression
    | [@bs.as 211] ExpressionWithTypeArguments
    | [@bs.as 212] AsExpression
    | [@bs.as 213] NonNullExpression
    | [@bs.as 214] MetaProperty
    | [@bs.as 215] SyntheticExpression
    | [@bs.as 216] TemplateSpan
    | [@bs.as 217] SemicolonClassElement
    | [@bs.as 218] Block
    | [@bs.as 219] VariableStatement
    | [@bs.as 220] EmptyStatement
    | [@bs.as 221] ExpressionStatement
    | [@bs.as 222] IfStatement
    | [@bs.as 223] DoStatement
    | [@bs.as 224] WhileStatement
    | [@bs.as 225] ForStatement
    | [@bs.as 226] ForInStatement
    | [@bs.as 227] ForOfStatement
    | [@bs.as 228] ContinueStatement
    | [@bs.as 229] BreakStatement
    | [@bs.as 230] ReturnStatement
    | [@bs.as 231] WithStatement
    | [@bs.as 232] SwitchStatement
    | [@bs.as 233] LabeledStatement
    | [@bs.as 234] ThrowStatement
    | [@bs.as 235] TryStatement
    | [@bs.as 236] DebuggerStatement
    | [@bs.as 237] VariableDeclaration
    | [@bs.as 238] VariableDeclarationList
    | [@bs.as 239] FunctionDeclaration
    | [@bs.as 240] ClassDeclaration
    | [@bs.as 241] InterfaceDeclaration
    | [@bs.as 242] TypeAliasDeclaration
    | [@bs.as 243] EnumDeclaration
    | [@bs.as 244] ModuleDeclaration
    | [@bs.as 245] ModuleBlock
    | [@bs.as 246] CaseBlock
    | [@bs.as 247] NamespaceExportDeclaration
    | [@bs.as 248] ImportEqualsDeclaration
    | [@bs.as 249] ImportDeclaration
    | [@bs.as 250] ImportClause
    | [@bs.as 251] NamespaceImport
    | [@bs.as 252] NamedImports
    | [@bs.as 253] ImportSpecifier
    | [@bs.as 254] ExportAssignment
    | [@bs.as 255] ExportDeclaration
    | [@bs.as 256] NamedExports
    | [@bs.as 257] ExportSpecifier
    | [@bs.as 258] MissingDeclaration
    | [@bs.as 259] ExternalModuleReference
    | [@bs.as 260] JsxElement
    | [@bs.as 261] JsxSelfClosingElement
    | [@bs.as 262] JsxOpeningElement
    | [@bs.as 263] JsxClosingElement
    | [@bs.as 264] JsxFragment
    | [@bs.as 265] JsxOpeningFragment
    | [@bs.as 266] JsxClosingFragment
    | [@bs.as 267] JsxAttribute
    | [@bs.as 268] JsxAttributes
    | [@bs.as 269] JsxSpreadAttribute
    | [@bs.as 270] JsxExpression
    | [@bs.as 271] CaseClause
    | [@bs.as 272] DefaultClause
    | [@bs.as 273] HeritageClause
    | [@bs.as 274] CatchClause
    | [@bs.as 275] PropertyAssignment
    | [@bs.as 276] ShorthandPropertyAssignment
    | [@bs.as 277] SpreadAssignment
    | [@bs.as 278] EnumMember
    | [@bs.as 279] SourceFile
    | [@bs.as 280] Bundle
    | [@bs.as 281] UnparsedSource
    | [@bs.as 282] InputFiles
    | [@bs.as 283] JSDocTypeExpression
    | [@bs.as 284] JSDocAllType
    | [@bs.as 285] JSDocUnknownType
    | [@bs.as 286] JSDocNullableType
    | [@bs.as 287] JSDocNonNullableType
    | [@bs.as 288] JSDocOptionalType
    | [@bs.as 289] JSDocFunctionType
    | [@bs.as 290] JSDocVariadicType
    | [@bs.as 291] JSDocComment
    | [@bs.as 292] JSDocTypeLiteral
    | [@bs.as 293] JSDocSignature
    | [@bs.as 294] JSDocTag
    | [@bs.as 295] JSDocAugmentsTag
    | [@bs.as 296] JSDocClassTag
    | [@bs.as 297] JSDocCallbackTag
    | [@bs.as 298] JSDocEnumTag
    | [@bs.as 299] JSDocParameterTag
    | [@bs.as 300] JSDocReturnTag
    | [@bs.as 301] JSDocThisTag
    | [@bs.as 302] JSDocTypeTag
    | [@bs.as 303] JSDocTemplateTag
    | [@bs.as 304] JSDocTypedefTag
    | [@bs.as 305] JSDocPropertyTag
    | [@bs.as 306] SyntaxList
    | [@bs.as 307] NotEmittedStatement
    | [@bs.as 308] PartiallyEmittedExpression
    | [@bs.as 309] CommaListExpression
    | [@bs.as 310] MergeDeclarationMarker
    | [@bs.as 311] EndOfDeclarationMarker
    | [@bs.as 312] Count
    | [@bs.as 59] FirstAssignment
    | [@bs.as 71] LastAssignment
    | [@bs.as 60] FirstCompoundAssignment
    | [@bs.as 71] LastCompoundAssignment
    | [@bs.as 73] FirstReservedWord
    | [@bs.as 108] LastReservedWord
    | [@bs.as 73] FirstKeyword
    | [@bs.as 147] LastKeyword
    | [@bs.as 109] FirstFutureReservedWord
    | [@bs.as 117] LastFutureReservedWord
    | [@bs.as 163] FirstTypeNode
    | [@bs.as 183] LastTypeNode
    | [@bs.as 18] FirstPunctuation
    | [@bs.as 71] LastPunctuation
    | [@bs.as 0] FirstToken
    | [@bs.as 147] LastToken
    | [@bs.as 2] FirstTriviaToken
    | [@bs.as 7] LastTriviaToken
    | [@bs.as 8] FirstLiteralToken
    | [@bs.as 14] LastLiteralToken
    | [@bs.as 14] FirstTemplateToken
    | [@bs.as 17] LastTemplateToken
    | [@bs.as 28] FirstBinaryOperator
    | [@bs.as 71] LastBinaryOperator
    | [@bs.as 148] FirstNode
    | [@bs.as 283] FirstJSDocNode
    | [@bs.as 305] LastJSDocNode
    | [@bs.as 294] FirstJSDocTagNode
    | [@bs.as 305] LastJSDocTagNode
    | [@bs.as 118] FirstContextualKeyword
    | [@bs.as 147] LastContextualKeyword;
};

module TsType = {
  type t;

  [@bs.send] external getText: t => string = "getText";
};

module TsParDecl = {
  type t;

  [@bs.send] external getType: t => TsType.t = "getType";
  [@bs.send] external getName: t => string = "getName";
};

module TypeKind = {
  type t;

  [@bs.get] external type_: t => TsType.t = "type";
  [@bs.send]
  external getParameters: t => Js.Array.t(TsParDecl.t) = "getParameters";
  [@bs.send] external getReturnType: t => TsType.t = "getReturnType";
};

module TsNode = {
  type t;

  [@bs.get] external getNs: t => Js.Array.t(string) = "ns";
  [@bs.get] external getId: t => string = "id";
  [@bs.get] external getKind: t => SyntaxKind.t = "kind";
  [@bs.get] external getNode: t => TypeKind.t = "node";
  [@bs.get] external isArray: t => bool = "isArray";
  [@bs.get] external getTypeName: t => string = "typeName";
  [@bs.get] external getArrayElementType: t => t = "arrayElementType";
};

[@bs.module "../src/tsApiBridge"]
external extractTypesFromFile: string => Js.Array.t(TsNode.t) =
  "extractTypesFromFile";

[@bs.module "../src/tsApiBridge"]
external extractTypesFromCode: string => Js.Array.t(TsNode.t) =
  "extractTypesFromCode";