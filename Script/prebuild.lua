package.path = "../Script/?.lua"
local dds2c = require("dds2c")

dds2c("../Resource/editor.dds", "../Source/editor/res/editor.h", "bee__editor_res_editor")