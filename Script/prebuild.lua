package.path = "../Script/?.lua"
local dds2c = require("dds2c")

dds2c("../Resource/editor.dds", "../Source/editor/res/main.h", "bee__editor_res_main")