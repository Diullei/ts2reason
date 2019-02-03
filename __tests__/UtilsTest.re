open Jest;

describe("Utils", () => {
  Expect.(
    test("range", () =>
      expect(Utils.range(2, 6)) |> toEqual([2, 3, 4, 5])
    )
  );

  Expect.(
    test("uniq", () =>
      expect(Utils.uniq([1, 1, 2, 3, 3, 4, 4, 4, 2, 1]))
      |> toEqual([1, 2, 3, 4])
    )
  );

  Expect.(
    test("capitalize", () =>
      expect(Utils.capitalize("capitalize")) |> toEqual("Capitalize")
    )
  );

  Expect.(
    test("lowerFirst", () =>
      expect(Utils.lowerFirst("LowerFirst")) |> toEqual("lowerFirst")
    )
  );

  Expect.(
    test("normalizeName", () =>
      expect(Utils.normalizeName("\"Module\".'Src'.Types.$type_name"))
      |> toEqual("Module_Src_Types__type_name")
    )
  );

  Expect.(
    test("fixIfItsAReservedWork", () =>
      expect(Utils.fixIfItsAReservedWork("as")) |> toEqual("as_")
    )
  );

  let (n1, lst) = "make"->Utils.toUniqueName([]);
  let (n2, lst) = "make"->Utils.toUniqueName(lst);
  let (n3, lst) = "make"->Utils.toUniqueName(lst);

  Expect.(
    testAll(
      "toUniqueName",
      [
        (lst, ["make", "make", "make"]),
        ([n1, n2, n3], ["make", "make1", "make2"]),
      ],
      ((input, output)) =>
      expect(input) |> toEqual(output)
    )
  );

  Expect.(
    test("makeIndent", () =>
      expect(Utils.makeIndent(5)) |> toEqual("     ")
    )
  );

  Expect.(
    test("createNameSpaceName", () =>
      expect(
        Utils.createNamespaceName([|
          "\"Module\"",
          "'Src'",
          "Types",
          "$type_name",
        |]),
      )
      |> toEqual("Module_Src_Types__type_name")
    )
  );
});