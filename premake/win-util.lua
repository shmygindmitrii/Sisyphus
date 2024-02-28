function latest_installed_visual_studio()
  local vs_get_version_cmd = "\"\"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe\"\" -latest -property catalog_productLineVersion"
  local handle = io.popen(vs_get_version_cmd)
  local output = handle:read("*a")
  local version = output:gsub('[\n\r]', '')
  return "vs" .. version
end
