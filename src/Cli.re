[@bs.scope "process"] [@bs.val] external processArgv: array(string) = "argv";

Main.convertFileToReason(processArgv[2]) |> Js.log;