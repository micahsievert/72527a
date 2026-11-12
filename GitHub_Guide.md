# VEX 72527A — GitHub Quick Guide

## First-time setup (Windows or Mac)

1. Install Git, VS Code, and the official **PROS** extension from Purdue SIGBots.
   Complete the extension's CLI and toolchain installation for your operating system:
   https://pros.cs.purdue.edu/v5/getting-started/windows.html
2. Clone this repository and open the folder containing `project.pros` in VS Code.
   On Windows, use a simple path such as `C:/Robot/72527a` to avoid build-tool issues with spaces.
3. Copy `.vscode/c_cpp_properties.example.json` to `.vscode/c_cpp_properties.json`
   (replace an old local copy if it contains another computer's paths). This local file
   is ignored by Git because editor tools can rewrite it with absolute paths.
4. Open a terminal through PROS so its ARM compiler and build tools are available. Run:

   ```sh
   pros make clean
   pros make
   pros build-compile-commands
   ```

Use PROS Build for this project, not VS Code's generic "Run C++ File" button.
`compile_commands.json` is generated separately on each computer for editor completion;
regenerate it after moving the project or changing the toolchain. It and `.d/` are ignored
by Git because they are generated local files.

If includes have red underlines, ensure VS Code opened the project folder itself (the
folder containing `include`, `src`, and `project.pros`), regenerate the compile database
above, then run **C/C++: Reset IntelliSense Database** and **Developer: Reload Window**
from the Command Palette. The local compiler path must point to your PROS ARM compiler,
not another person's installation or a desktop C++ compiler.
If PROS Build fails too, capture the first compiler error and the command used to build;
editor include settings do not change the Makefile's compiler include paths.

The current build requests `gnu23` / `gnu++26` and is verified with ARM GCC 14.3.1.
If a compiler reports an unsupported standard or linker option, check the PROS toolchain
version on that computer; this is separate from an incorrect project path.

The `location` entries in `project.pros` record the original template cache locations.
The build uses the checked-in `include/` and `firmware/` files, not those cache directories.
Keep those folders and `project.pros` in Git. `diag.sh` is an optional Mac-only USB diagnostic,
not part of compiling the robot program.

## BEFORE CODING — Get the latest code
Run this command in VS Code terminal:

git pull

Always do this before making changes so you have the latest version from GitHub.

## AFTER CODING — Save and upload changes

Run these three commands in VS Code terminal. Replace "change" with what you changed.
All 3 lines can be run at once:

git add .
git commit -m "change"
git push

## CHECK STATUS — See what's changed

git status

Shows which files have changed and whether you have uncommitted work.

## TEAM RULES

1. Only one person edits the code at a time.
2. Always **pull before coding**.
3. Always **commit and push after coding**.
4. Tell the other person when you're finished.
5. If Git reports an error or merge conflict, don't force anything. Resolve it before continuing.

## Repository

https://github.com/micahsievert/72527a


**Remember: Pull → Code → Add → Commit → Push.**