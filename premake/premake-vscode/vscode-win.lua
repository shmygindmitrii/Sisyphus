include("win-util")

local function create_build_task_json(wkspc, cnfg)
  --find MSBuild first with inner visual studio utility
  local handle = io.popen("\"\"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe\"")
  local output = handle:read('*a')
  local MSBUILD = output:gsub('[\n\r]', '')
  MSBUILD = path.normalize(MSBUILD)
  local indent = "      "
  local content = indent .. "\"type\": \"shell\",\n"
  content = content .. indent .. "\"label\": \"build_" .. os.target() .. "_" .. cnfg.name .. "\",\n"
  content = content .. indent .. "\"dependsOrder\": \"sequence\",\n"
  content = content .. indent .. "\"dependsOn\": [\n"
  content = content .. indent .. "  \"generate_" .. wkspc.name .. "_" .. os.target() .. "\",\n"
  content = content .. indent .. "],\n"
  content = content .. indent .. "\"command\": \"" .. MSBUILD .. "\",\n"
  content = content .. indent .. "\"options\": {\n"
  content = content .. indent .. "  \"cwd\": \"" .. "${workspaceRoot}/premake/" .. SOLUTION_VARS.project_output_directory .. "\",\n"
  content = content .. indent .. "},\n"
  content = content .. indent .. "\"args\": [\n"
  content = content .. indent .. "  \"" .. wkspc.name .. ".sln\",\n"
  content = content .. indent .. "  \"/p:Configuration=" ..  cnfg.name .. "\",\n"
  content = content .. indent .. "  \"/p:Platform=x64\",\n"
  content = content .. indent .. "  \"/property:GenerateFullPaths=true\",\n"
  content = content .. indent .. "]"
  return "    {\n" .. content .. "\n    },"
end

local function create_generation_task_json(wkspc)
  local vs_version = latest_installed_visual_studio()
  local indent = "      "
  local content = indent .. "\"type\": \"shell\",\n"
  content = content .. indent .. "\"label\": \"generate_" .. wkspc.name .. "_" .. os.target() .. "\",\n"
  content = content .. indent .. "\"command\": \"${workspaceRoot}/premake/premake5.exe\",\n"
  content = content .. indent .. "\"options\": {\n"
  content = content .. indent .. "  \"cwd\": \"${workspaceRoot}/premake\",\n"
  content = content .. indent .. "},\n"
  content = content .. indent .. "\"args\": [\n"
  content = content .. indent .. "  \"" .. vs_version .. "\",\n"
  content = content .. indent .. "]"
  return "    {\n" .. content .. "\n    },"
end

local function create_launch_json(prj, cnfg)
  local indent = "      "
  --local content = indent .. "\"name\": \"Tests_windows_Debug\",\n"
  local content = indent .. "\"name\": \"" .. prj.name .. "_" .. os.target() .. "_" .. cnfg.name .. "\",\n"
  content = content .. indent .. "\"type\": \"cppvsdbg\",\n"
  content = content .. indent .. "\"request\": \"launch\",\n"
  content = content .. indent .. "\"program\": \"" .. cnfg.targetdir .. "/" .. prj.name .. ".exe\",\n"
  content = content .. indent .. "\"args\": [],\n"
  content = content .. indent .. "\"stopAtEntry\": false,\n"
  content = content .. indent .. "\"cwd\": \"${workspaceRoot}\",\n"
  content = content .. indent .. "\"environment\": [],\n"
  content = content .. indent .. "\"console\": \"internalConsole\",\n"
  content = content .. indent .. "\"preLaunchTask\": \"build_" .. os.target() .. "_" .. cnfg.name .. "\""
  return "    {\n" .. content .. "\n    },"
end

local function create_vscode_jsons()
  --by default we assume that premake-build scripts are located in the
  --premake directory of the root folder
  local tasks_path = "../" .. ".vscode/tasks.json"
  local launch_path = "../" .. ".vscode/launch.json"
  if not os.isdir("../.vscode") then
    os.mkdir("../.vscode")
  end
  --iterate over all projects and configs
  local wkspc = premake.global.eachWorkspace()()
  local launch_content = "{\n  \"version\": \"0.1.0\",\n  \"configurations\": [\n"
  local tasks_content = "{\n  \"version\": \"2.0.0\",\n  \"tasks\": [\n"
  tasks_content = tasks_content .. create_generation_task_json(wkspc) .. "\n"
  local configs_made = {}
  for prj in premake.workspace.eachproject(wkspc) do
    for cnfg in premake.project.eachconfig(prj) do
      if (prj.kind == "WindowedApp" or prj.kind == "ConsoleApp") then
        if configs_made[cnfg.name] == nil then
          tasks_content = tasks_content .. create_build_task_json(wkspc, cnfg) .. "\n"
          configs_made[cnfg.name] = true
        end
        launch_content = launch_content .. create_launch_json(prj, cnfg) .. "\n"
      end
    end
  end
  tasks_content = tasks_content .. "  ]\n}"
  launch_content = launch_content .. "  ]\n}"
  io.writefile(tasks_path, tasks_content)
  io.writefile(launch_path, launch_content)
end

newaction {
  trigger = 'vscode',
  description = 'Export vscode task.json and launch.json, create project files',
  execute = create_vscode_jsons
}
