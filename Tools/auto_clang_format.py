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
    # check for arguments
    # --------------------------------------------------- #
    if len(sys.argv) < 2:
        print("Not enough arguments: python auto_clang_format.py <source_folder> <skipped_folder0,skipped_folder1>* Exit.")
    # --------------------------------------------------- #
    # check for sources folder existance
    # --------------------------------------------------- #
    sources_folder = sys.argv[1]
    if not os.path.exists(sources_folder):
        print("Source files folder '{source}' was not found! Exit.".format(source=sources_folder))
        sys.exit(3)
    # --------------------------------------------------- #
    # add skipped folders
    # these folders can be anywhere in the path
    # --------------------------------------------------- #
    skipped_folders = set([])
    if len(sys.argv) > 2:
        skipped_folders_arg = sys.argv[2].split(",")
        for skipped_folder in skipped_folders_arg:
            if skipped_folder.startswith("\""):
                skipped_folders.add(skipped_folder[1:len(skipped_folder)-1])
            else:
                skipped_folders.add(skipped_folder)
    # --------------------------------------------------- #
    # look for all source file paths
    # --------------------------------------------------- #
    def splitter(path):
        for split_string in ["\\", "/"]:
            path = path.replace(split_string, "__PATH_SPLITTER__")
        return path.split("__PATH_SPLITTER__")
    abs_filepaths = []
    for (root, folders, files) in os.walk(sources_folder):
        if skipped_folders:
            root_parts = splitter(root)
            skip_current_folder = False
            for root_part in root_parts:
                if root_part in skipped_folders:
                    print("Skip folder \"{folder}\"".format(folder=root))
                    skip_current_folder = True
                    break
            if skip_current_folder:
                continue
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
