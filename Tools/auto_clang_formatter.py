import os
import sys
import subprocess

def main():
    # --------------------------------------------------- #
    # check for clang-format.exe
    # --------------------------------------------------- #
    clang_format_exe_tmpl = "C:\\Program Files\\Microsoft Visual Studio\\{version}\\Community\\VC\\Tools\\Llvm\\bin\\clang-format.exe"
    for ver in (2022, 2019, 2017):
        clang_format_exe = clang_format_exe_tmpl.format(version=ver)
        if os.path.exists(clang_format_exe):
            break
        else:
            clang_format_exe = None
    if clang_format_exe is None:
        print("clang-format.exe was not found in Visual Studio directories! Exit.")
        sys.exit(1)
    # --------------------------------------------------- #
    # found clang-format.exe
    # check for sources folder existance
    # --------------------------------------------------- #
    sources_folder = sys.argv[1]
    if not os.path.exists(sources_folder):
        print("Source files folder '{source}' was not found! Exit.".format(source=sources_folder))
        sys.exit(2)
    # --------------------------------------------------- #
    # look for all source file paths
    # --------------------------------------------------- #
    abs_filepaths = []
    for (root, folders, files) in os.walk(sources_folder):
        for filepath in files:
            if filepath.endswith(".h") or filepath.endswith(".hpp") or filepath.endswith(".cpp") or filepath.endswith(".c"):
                abs_filepaths.append(os.path.join(root, filepath))
    # --------------------------------------------------- #
    # transform every file with clang-format.exe
    # --------------------------------------------------- #
    for filepath in abs_filepaths:
        cmd = "\"{clang_format}\" -i \"{filepath}\"".format(clang_format=clang_format_exe, filepath=filepath)
        print(cmd)
        subprocess.run(cmd, shell=True)

if __name__ == "__main__":
    main()
