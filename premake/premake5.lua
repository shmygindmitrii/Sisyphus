--[[
    testing premake file
    not really used, created for
    learning purposes first
--]]

require("premake-vscode/vscode-win")

include("common-util") --use to create similar to each other projects

-- without that option nothing can be built

-- global namespace for all other manipulations
SOLUTION_VARS = {}
SOLUTION_VARS.project_output_directory = "project/"..os.target()
--for case of different platforms here customization is needed

if os.target() == "windows" then
    include("win.lua")
    create_solution_win("Sisyphus")
end

SOLUTION_VARS.target_directory = SOLUTION_VARS.project_output_directory.."/".."bin/%{cfg.buildcfg}"
SOLUTION_VARS.intermediate_directory = SOLUTION_VARS.project_output_directory.."/".."obj/%{cfg.buildcfg}"
SOLUTION_VARS.resources_folder = "\""..path.join(os.getcwd().."/../", "Resources/").."\""
defines { "SISYPHUS_RESOURCES_FOLDER=%{SOLUTION_VARS.resources_folder}" } --common for all

group "External"
local thirdparty_proj = create_isolated_static_lib("Thirdparty")

group "Math"
local base_proj = create_isolated_static_lib("Base")

group "Render"
local render_proj = create_static_lib(
    "Render",
    { "../Source/Base/inc", "../Source/Thirdparty/inc" }
)

group "Util"
local util_proj = create_static_lib(
    "Util",
    { "../Source/Base/inc", "../Source/Thirdparty/inc", "../Source/Render/inc" }
)

group "Samples"
local tests_proj = create_binary(
    "Tests",
    "ConsoleApp",
    { "../Source/Base/inc", "../Source/Thirdparty/inc", "../Source/Render/inc" },
    { base_proj, thirdparty_proj },
    {}
)

group "Samples" --each create function reverts scope back to general
local sample_proj = create_binary(
    "Sample", --projname
    "WindowedApp", --kind
    { "../Source/Base/inc", "../Source/Thirdparty/inc", "../Source/Render/inc", "../Source/Util/inc" }, --includes
    { base_proj, thirdparty_proj, render_proj, util_proj }, --links
    { "../Source/Sample/*.ico", "../Source/Sample/*.rc" } --additional files except standard
)

startproject(sample_proj)

group "" --no group
