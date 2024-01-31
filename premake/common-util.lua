-- very common functions

function concat_tables(t1, t2) 
   for i=1, #t2 do
       t1[#t1+1] = t2[i]
   end
   return t1
end

-- lib section

function create_lib(project_name, lib_type, include_dirs)
    print("    Create library project for \"" .. project_name .. "\" of type \"" .. lib_type .. "\" for platform " .. string.upper(TEMPLE.platform))
    local platfromed_project_name = project_name .. "-" .. TEMPLE.platform
    project(platfromed_project_name)
        kind(lib_type)
        targetname(platfromed_project_name)
        files {
            "../" .. project_name .. "/src/*.c",
            "../" .. project_name .. "/src/*.cpp",
            "../" .. project_name .. "/inc/*.h",
            "../" .. project_name .. "/inc/*.hpp",
        }
        includedirs(concat_tables({ "../" .. project_name .. "/inc" }, include_dirs))
        location(TEMPLE.output_directory)
        targetdir(TEMPLE.target_directory .. "/" .. project_name)
        objdir(TEMPLE.intermediate_directory .. "/" .. project_name)
        defines { "_LIB" }
    project "*"
    return platfromed_project_name
end

function create_isolated_static_lib(project_name)
    return create_lib(project_name, "StaticLib", {})
end

function create_static_lib(project_name, include_dirs)
    return create_lib(project_name, "StaticLib", include_dirs)
end

-- binary section

function create_binary(project_name, binary_type, include_dirs, link_projs, resource_files)
    print("    Create runable project for \"" .. project_name .. "\" of type \"" .. binary_type .. "\" for platform " .. string.upper(TEMPLE.platform))
    local platfromed_project_name = project_name .. "-" .. TEMPLE.platform
    project(platfromed_project_name)
        kind(binary_type)
        targetname(platfromed_project_name)
        local proj_files = {
            "../" .. project_name .. "/src/*.c",
            "../" .. project_name .. "/src/*.cpp",
            "../" .. project_name .. "/inc/*.h",
            "../" .. project_name .. "/inc/*.hpp",
        }
        if #resource_files > 0 then
            proj_files = concat_tables(proj_files, resource_files)
        end
        files(proj_files)
        location(TEMPLE.output_directory)
        targetdir(TEMPLE.target_directory .. "/" .. project_name)
        objdir(TEMPLE.intermediate_directory .. "/" .. project_name)
        includedirs(concat_tables({ "../" .. project_name .. "/inc" }, include_dirs))
        links(link_projs)
    project "*"
    return platfromed_project_name
end
