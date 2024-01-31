function create_solution_win(solution_name)
    print("Create solution \"" .. solution_name .. "\" for platform WIN")
    workspace(solution_name .. "-win")
        configurations { "Debug", "Release" }
        platforms { "x64" }
        defines { "WIN32", "_CONSOLE" }
        language "C++"
        location(TEMPLE.project_output_directory)

    filter "platforms:x64"
        system "Windows"

    filter "configurations:Debug"
        defines { "_DEBUG" }
        optimize "Off"
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"

    filter {}
end



