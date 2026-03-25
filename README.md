# cd-deluxe

A supercharged `cd` replacement for the command line. Navigate your directory history by recency, frequency, or position — with regex filtering and fuzzy-find integration as options.

It has a goal of being lightweight and fast, with a simple interface and minimal setup.  It runs completely from the directory so it doesn't require external databases or file storage.

Supports **Bash**, **Zsh**, **Fish**, **PowerShell**, and **Windows CMD**. Runs on Linux, macOS, and Windows.

# Quick Start

| Command | Description |
|---------|-------------|
| cd -          |   Change to previous directory. As per typical Unix Shell but now available for Windows cmd.exe. |
| cd --         |   Change to the second to last directory |
| cd ---        |   Change the third previous directory, same as `cd -3` |
| cd -4         |   And then the fourth - can use a negative number instead of a series of dashes |
| cd +1         |   Change to the first directory visitied. Positive numbers are relative to the starting directory |
| cd +2         |   Change to the second directory visited |
| cd +++        |   Change to the third visited, and so on |
| cd ,          |   A comma means to change to the "the most common directory". Comma = "Common" |
| cd ,,         |   Change to the second most commonly visited directory |
| cd ,3         |   Change to the third most visited |
| cd ...        |   Change to the grandparent directory. Same as `cd ../..` |
| cd ..4        |   Change to the great-great-grandparent directory. Same as `cd ../../../..` |
| cd abc        |   Change to the most recent directory in history whose path contains "abc". Regex also works, so `cd ^abc` would match the most recent directory starting with "abc", for example. See regex section below for more details. |
| cd + def      |   Change to the earliest directory in history whose path contains "def". The `+` forces the search to be in the forward direction (oldest first) instead of the default backward direction (most recent first). |
| cd , ghi      |   Change to the most commonly visited directory whose path contains "ghi". The `,` forces the search to be in the common direction instead of the default backward direction. |
| cd .. jkl     |   Change to the nearest parent directory whose path contains "jkl". The `..` forces the search to be in the upward direction instead of the default backward direction. |
| cd -l         |   List the directory history in the current direction (default backward) without changing directories |
| cd -l abc     |   List the directory history for entries containing "abc" |
| cd -l + def   |   List the directory history in the forward direction (oldest first) for entries containing "def" |
| cd -l , ghi   |   List the common (most-visited) directories containing "ghi" |
| cd -f         |   Pipe the directory history in the current direction through fzf and then change to the selected entry |
| cd cpp20/main/main.cpp |   When the argument is a file, change to its directory |

# Details

## Installation Overview

The [Releases](https://github.com/m6z/cd-deluxe/releases/latest) page contains pre-built binaries
for Linux and macOS, as well as an installer for Windows which sets up the necessary scripts for CMD
and PowerShell.  Or clone the repo and build from source with CMake.  See below for more detailed
instructions.

## Navigation Directions

cd-deluxe makes it easy to access your directory history in four independent vectors.

| Symbol | Direction | Meaning                  |
|--------|-----------|--------------------------|
| `-`    | Backward   | Most recent → oldest          |
| `+`    | Forward    | First visited → most recent   |
| `,`    | Common     | Most visited → least          |
| `..`   | Upward     | Parent directory chain          |

The default direction when not specified is backward (`-`).  But you can switch to any of the other directions at any time, and the history is tracked independently in each direction.  To override the default direction, use environment `CDD_OPTIONS=--direction=+` (or `-d +`) to switch to forward direction, for example.

## The common listing includes visit counts

```
$ cd -l ,
 ,1: ( 7) /home/mike/projects/cd-deluxe
 ,2: ( 4) /home/mike/projects
 ,3: ( 1) /tmp
```

The above indicates that `/home/mike/projects/cd-deluxe` is the most-visited directory (7 visits), followed by `/home/mike/projects` (4 visits) and `/tmp` (1 visit).

To simply jump to the most-visited directory, type `cd ,`.  To visit the second most-visited, type `cd ,2` or `cd ,,`.  And so on.

For listing the most commonly visited directories (without changing directory), all of the following are equivalent:

```
cd -l ,                   # list common history
cd --list --direction=c   # list common history
cd -c                     # list common history
cd --dc                   # list common history
```

## Upward navigation in detail

`..` and its shorthands walk up the current directory's parent chain.  This is most useful when working in deeply nested directories, where the standard `cd ..` is tedious to repeat and `cd ../../..` is error-prone.

```
# Current directory: /home/mike/projects/cd-deluxe/test

cd ..             →  /home/mike/projects/cd-deluxe
cd ...            →  /home/mike/projects
cd ....           →  /home/mike
cd .. projects    →  /home/mike/projects   (first parent matching "projects")
cd -l ..          lists all parents
cd -f ..          lists all parent steps through fzf and then jumps to the selected one
```

Dot expansion also works as part of a path argument: `cd .../foo` expands to `../../foo`.

## Change to the directory of an existing file

```
cd /home/mike/projects/cd-deluxe/CMakeLists.txt → /home/mike/projects/cd-deluxe
```

The logic here is that if you `cd` to a file, you probably meant to `cd` to its directory.  This is especially useful when pasting in a path from an editor or file explorer into a terminal session.

## Regex filtering

Add a pattern to narrow the match:

```
cd aa         # directory in history whose path contains "aa"
cd ^bb        # directory in history starting with "bb"
cd cc$        # directory in history ending with "cc"
cd end/       # directory in history whose final component ends with "end" (note: this is not standard regex but a cd-deluxe extension for convenience)
cd , src      # most-visited directory containing "src"
cd + projects # earliest directory containing "projects"
cd .. work    # nearest parent directory matching "work"
```

Patterns are standard C++ regex (so `^`, `$`, `.*`, `[...]`, etc. all work).  Note: regex matching is used instead of glob matching.

## List before jumping

Use `-l` or `--list` to list the history in the current direction without changing directories:

```
cd -l         # list history, most recent first (assuming default direction is backwards '-')
cd -l +       # list history, oldest first
cd -l ,       # list history, most visited first (with visit counts)
cd -l ..      # list parent directories upward
```

Note: issuing `cd` with no arguments lists the recent history in the default direction.

Use `-l` or `--list` with a pattern to filter the listing:

```
cd -l src     # list history entries containing "src"
cd -l , src   # list entries containing "src" organized by visit count
```

## Fuzzy find (fzf)

```
cd -f         # pipe default listing of directories visited through fzf and then change to selection
cd -f ,       # pipe common list through fzf
cd -f ..      # pipe parent list through fzf
cd -f , abc   # Filter first by pattern "abc", then pipe through fzf (regex works as well))
```

Requires [fzf](https://github.com/junegunn/fzf) to be on your `PATH`.

## Listing history size

```
cd -a         # show all history (no truncation)
cd -m 20      # set max history length to 20 (default: 10)
```

## History management

```
cd --del -2         # remove the second-most-recent entry
cd --del +1         # remove the oldest entry
cd --gc             # garbage-collect duplicates (flattens visit counts into single entries)
cd --reset          # wipe the entire history
```

The `--del` option takes an argument in the same format as the direction flags or pattern/regex to specify which entry to delete.

## Direction aliases

For shells where bare `+`, `-`, or `,` are awkward (e.g. PowerShell), use letter aliases:

| Flag        | Equivalent |
|-------------|-----------|
| `--db`      | `-`       |
| `--df`      | `+`       |
| `--dc`      | `,`       |
| `--du`      | `..`      |
| `-d b/f/c/u`| any of the above |

```
cd -d f src         # forward search for "src"
cd --dc             # most-visited directory
```

## Environment variable

Set default options in `CDD_OPTIONS` to apply them to every invocation:

```bash
export CDD_OPTIONS="--ignore-case --max=8"
```

Tip: set `CDD_OPTIONS=--direction=,` to change the `cd` default direction to the common (most-visited) directories.

## Shell setup details

Note: when building from source the binary name is "cd-deluxe".  When downloading a pre-built binary
from the Releases page, the binary name will be "cd-deluxe-linux" or "cd-deluxe-macos" depending on
the platform.  Adjust `PATH_TO_BINARY/cd-deluxe` in the examples below accordingly.

### Bash
```bash
eval "$(PATH_TO_BINARY/cd-deluxe --init)"          # add to ~/.bashrc
```

### Zsh
```zsh
source <(PATH_TO_BINARY/cd-deluxe --init)          # add to ~/.zshrc
```

### Fish
```fish
PATH_TO_BINARY/cd-deluxe --init | source           # add to ~/.config/fish/config.fish
```

For Windows CMD and PowerShell there is an installer which sets up the necessary scripts.  See Releases section in github, and then follow the instructions displayed by the installer.

### PowerShell
```powershell
cd-deluxe --init powershell         # generates cdd.ps1 in current directory
. .\cdd.ps1                         # dot-source it (add to $PROFILE)

# To optionally override the built-in 'cd' command after dot-sourcing the above:
Set-Alias -Name cd -Value cdd -Option AllScope -Scope Global
```

### Windows CMD
```bat
cd-deluxe --init cmd                # generates cdd.cmd in current directory
doskey cd=PATH_TO_INSTALL\cdd.cmd $*
```

### Build from source

```bash
cmake -B build && cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

Requires a C++20 compiler and CMake 3.20+. Dependencies (Catch2, cxxopts) are fetched automatically.

### Permissions - macOS

If downloading the macOS pre-built binary, you will need to give it execute permissions and remove
the quarantine attribute before running it.  Note: building from source on macOS does not require
these steps, only the pre-built binary download.

```bash
# Give execute permissions
chmod +x cd-deluxe-macos

# Remove the quarantine attribute
xattr -d com.apple.quarantine cd-deluxe-macos

# Then run normally
./cd-deluxe-macos
```

### Permissions - Windows

If downloading the Windows installer you may get a warning like the following when you try to run
it.  This is because the installer is not signed by a recognized publisher.  You can click "More
info" and then "Run anyway" to proceed with the installation.

```
Windows protected your PC

Microsoft Defender SmartScreen prevented an unrecognized app from starting.
Running this app might put your PC at risk.

App: CddInstaller_2.0.6.163.exe
Publisher: Unknown publisher
```
Otherwise, clone the repo and build the project.

```
:: The following creates the executable: build\main\Release\cd-deluxe.exe
cmake --build build --config Release

:: The following builds the executable and a Windows installer:
cmake --build build --config Release --target installer
```

## Options reference

| Option | Description |
|--------|-------------|
| `-d DIR, --direction=DIR` | Set direction: `-`, `+`, `,`, `..` or `b`, `f`, `c`, `u` |
| `--db / --df / --dc / --du` | Shorthand direction flags |
| `-l, --list` | List without changing directory |
| `-m N, --max=N` | Maximum history entries (default 10) |
| `--max-backwards=N` | Maximum backwards history entries (overrides `--max`) |
| `--max-forwards=N` | Maximum forwards history entries (overrides `--max`) |
| `--max-common=N` | Maximum common history entries (overrides `--max`) |
| `--max-upwards=N` | Maximum upwards history entries (overrides `--max`) |
| `-a, --all` | Show all history (no limit) |
| `-i, --ignore-case` | Ignore case when comparing paths |
| `-h, --help` | Show help message |
| `-v, --version` | Show the version string |
| `--del ENTRY` | Delete an entry from history |
| `--reset` | Reset history (clear all entries) |
| `--gc` | Garbage collect history (remove duplicates) |
| `--init [SHELL]` | Generate shell initialization code (optionally specify shell) |

Note: most options can be set via the `CDD_OPTIONS` environment variable for persistent defaults.  For example, `export CDD_OPTIONS="--ignore-case --direction=c"` to set pattern matching to be case insensitive and the default direction to be "common" (most-visited).

