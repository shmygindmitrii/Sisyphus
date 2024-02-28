local p = premake

p.modules.export_compile_commands = {}
local m = p.modules.export_compile_commands

local workspace = p.workspace
local project = p.project

local sdk = nil

if (COMPILER_PATHS == nil) then
  COMPILER_PATHS = {}
end

local CHOSEN_CFG = nil

function m.getToolset(cfg)
  return p.tools[cfg.toolset or 'gcc']
end

function m.getIncludeDirs(cfg)
  local flags = {}

  local inc = '-I'
  local sys_inc = nil

  if (os.target() == "windows") then
    sys_inc = "-I"
  end

  for _, dir in ipairs(cfg.includedirs) do
    table.insert(flags, inc .. ' ' .. p.quoted(dir))
  end
  for _, dir in ipairs(cfg.externalincludedirs or {}) do
    table.insert(flags, sys_inc .. ' ' .. p.quoted(dir))
  end

  return flags
end

function m.getCommonFlags(cfg, node)
  local toolset = m.getToolset(cfg)
  local flags = toolset.getdefines(cfg.defines)
  flags = table.join(flags, toolset.getundefines(cfg.undefines))
  flags = table.join(flags, m.getIncludeDirs(cfg))
  if (path.iscfile(node.abspath)) then
    flags = table.join(flags, toolset.getcflags(cfg))
  else
    flags = table.join(flags, toolset.getcxxflags(cfg))
  end

  return table.join(flags, cfg.buildoptions)
end

function m.getObjectPath(prj, cfg, node)
  return path.join(cfg.objdir, path.appendExtension(node.objname, '.o'))
end

function m.getDependenciesPath(prj, cfg, node)
  return path.join(cfg.objdir, path.appendExtension(node.objname, '.d'))
end

function m.getFileFlags(prj, cfg, node)
  local t = table.join(m.getCommonFlags(cfg, node), { '-o', m.getObjectPath(prj, cfg, node) });
  t = table.join(t, { '-c', node.abspath });

  local toolset = m.getToolset(cfg)
  local fcfg = p.fileconfig.getconfig(node, cfg)

  local getflags = nil

  if (path.iscfile(fcfg.name)) then
    getflags = toolset.getcflags
  else
    getflags = toolset.getcxxflags
  end

  t = table.join(t, getflags(fcfg))

  return t
end

function m.generateCompileCommand(prj, cfg, node)
  local cc = COMPILER_PATHS[cfg.system]
  if (cc == nil) then
    cc = "cc"
  end

  return {
    directory = prj.location,
    file = node.abspath,
    command = "\"" .. cc .. "\" " .. table.concat(m.getFileFlags(prj, cfg, node), ' ')
  }
end

function m.includeFile(prj, node, depth)
  return path.iscppfile(node.abspath)
end

function m.getConfig(prj)
  if _OPTIONS['export-compile-commands-config'] then
    return project.getconfig(prj, _OPTIONS['export-compile-commands-config'],
      _OPTIONS['os'])
  end
  for cfg in project.eachconfig(prj) do
    -- just use the first configuration which is usually "Debug"
    return cfg
  end
end

function m.getProjectCommands(prj, cfg)
  local tr = project.getsourcetree(prj)
  local cmds = {}
  p.tree.traverse(tr, {
    onleaf = function(node, depth)
      if not m.includeFile(prj, node, depth) then
        return
      end
      if (p.fileconfig.getconfig(node, cfg)) then

        local opt_cfg = _OPTIONS["export-compile-commands-config"]

        if (((cfg.buildcfg == opt_cfg) or opt_cfg == nil)) then
          table.insert(cmds, m.generateCompileCommand(prj, cfg, node))
        end
      end
    end
  })
  return cmds
end

local function execute()

  if (os.host() == "windows") then
    local handle = io.popen("\"\"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find **\\Hostx64\\x64\\cl.exe\"")
    local output = handle:read('*a')
    local cl = output:gsub('[\n\r]', '')
    cl = path.normalize(cl)

    COMPILER_PATHS["windows"] = cl
  end

  local cfgCmds = {}
  local gen_wks = {}
  for wks in p.global.eachWorkspace() do
    gen_wks = wks
    for prj in workspace.eachproject(wks) do
      for cfg in project.eachconfig(prj) do
        local opt_cfg = _OPTIONS["export-compile-commands-config"]
        local opt_sys = os.target()

        if (((opt_cfg ~= nil) and (opt_cfg ~= cfg.buildcfg)) or ((opt_sys ~= nil) and (opt_sys ~= cfg.system))) then
          ::continue::
        else
          local cfgKey = ""

          if not cfgCmds[cfgKey] then
            cfgCmds[cfgKey] = {}
          end
          cfgCmds[cfgKey] = table.join(cfgCmds[cfgKey], m.getProjectCommands(prj, cfg))

          break
        end
      end
    end
  end

  local outfile = _MAIN_SCRIPT_DIR .. "/../.vscode/compile_commands.json"
  print(outfile)
  for cfgKey,cmds in pairs(cfgCmds) do
    p.generate(gen_wks, outfile, function(wks)
      p.w('[')
      for i = 1, #cmds do
        local item = cmds[i]
        local command = string.format([[
        {
          "directory": "%s",
          "file": "%s",
          "command": "%s"
        }]],
        item.directory,
        item.file,
        item.command:gsub('\\', '\\\\'):gsub('"', '\\"'))
        if i > 1 then
          p.w(',')
        end
        p.w(command)
      end
      p.w(']')
    end)
  end
end

newoption {
  trigger = "export-compile-commands-config",
  description = "export-compile-commands-config",
  default = nil
}

newaction {
  trigger = 'export-compile-commands',
  description = 'Export compiler commands in JSON Compilation Database Format',
  execute = execute
}

return m
