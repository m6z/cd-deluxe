# cd-deluxe

A supercharged `cd` replacement for the command line. Navigate your directory history by recency, frequency, or position — with regex filtering and fuzzy-find on top.

It has a goal of being lightweight and fast, with a simple interface and minimal setup.  It runs completely from the directory stack and an environment variable, so it doesn't require external databases or file storage.

Supports **Bash**, **Zsh**, **Fish**, **PowerShell**, and **Windows CMD**. Runs on Linux, macOS, and Windows.

---

## Quick Start

```bash
# One-time shell setup (add to ~/.bashrc, ~/.zshrc, etc.)
eval "$(PATH_TO_BINARY/cd-deluxe --init)"        # bash
source <(PATH_TO_BINARY/cd-deluxe --init)        # zsh
PATH_TO_BINARY/cd-deluxe --init | source         # fish
```

After that, use `cdd` (or `cd`, which is aliased) normally. The rest is automatic.

---

## Navigation

cd-deluxe tracks your directory history in four independent vectors.

| Symbol | Direction | Meaning                  |
|--------|-----------|--------------------------|
| `-`    | Backward  | Most recent → oldest     |
| `+`    | Forward   | First visited → most recent |
| `,`    | Common    | Most visited → least     |
| `..`   | Upward    | Parent directory chain   |

The default direction when not specified is backward (`-`).  But you can switch to any of the other directions at any time, and the history is tracked independently in each direction.  To override the default direction, use environment `CDD_OPTIONS=--direction=+` (or `-d +`) to switch to forward direction, for example.

### By count or repetition

```
cd -          # go to previous directory (per usual)
cd -2         # two back (same as cd --)
cd ---        # three back (same as cd -3)

cd +          # go to earliest directory this session
cd +2         # second earliest (same as cd ++)

cd ,          # go to most-visited directory
cd ,2         # second most-visited (same as cd ,,)

cd ..         # up one level (as per usual)
cd ...        # up two levels (same as cd ..2)
cd ....       # up three levels (same as cd ..3)
```

### Change to the directory of an existing file:

```
cd /home/mike/projects/cd-deluxe/CMakeLists.txt → /home/mike/projects/cd-deluxe
```

The thinking here is that if you `cd` to a file, you probably meant to `cd` to its directory.  This is especially useful when pasting in a path from an editor or file explorer into a terminal session.

### List before jumping

Use `-l` or `--list` to list the history in the current direction without changing directories:

```
cd -l         # list history, most recent first (assuming default direction is backwards '-')
cd -l +       # list history, oldest first
cd -l ,       # list history, most visited first (with visit counts)
cd -l ..      # list parent directories upward
```

### Regex filtering

Add a pattern to narrow the match:

```
cd ee         # most recent directory whose path contains "ee"
cd cc$        # most recent path ending with "cc"
cd , src      # most-visited directory containing "src"
cd + projects # earliest directory containing "projects"
cd .. work    # nearest parent directory matching "work"
```

Patterns are standard C++ regex (so `^`, `$`, `.*`, `[...]`, etc. all work).

### Fuzzy find (fzf)

```
cd -f         # pipe backward list through fzf
cd -f ,       # pipe common list through fzf
cd -f ..      # pipe parent list through fzf
cd -f , abc   # Filter first by pattern "abc", then pipe through fzf (regex works as well))
```

Requires [fzf](https://github.com/junegunn/fzf) to be on your `PATH`.

---

## Listing and history size

```
cd            # default: list recent history (same as cd -?)
cd -l         # explicit list (respects current direction)
cd -a         # show all history (no truncation)
cd -m 20      # set max history length to 20 (default: 10)
```

The common listing includes visit counts:

```
$ cd ,?
 ,1: ( 7) /home/mike/projects/cd-deluxe
 ,2: ( 4) /home/mike/projects
 ,3: ( 1) /tmp
```

The above indicates that `/home/mike/projects/cd-deluxe` is the most-visited directory (7 visits), followed by `/home/mike/projects` (4 visits) and `/tmp` (1 visit).

To simply jump to the most-visited directory, type `cd ,`

---

## History management

```
cd --del -2         # remove the second-most-recent entry
cd --del +1         # remove the oldest entry
cd --gc             # garbage-collect duplicates
cd --reset          # wipe the entire history
```

The `--del` option takes an argument in the same format as the direction flags or pattern/regex to specify which entry to delete.

---

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

---

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

---

## Environment variable

Set default options in `CDD_OPTIONS` to apply them to every invocation:

```bash
export CDD_OPTIONS="--ignore-case --max=20"
```

Tip: set `CDD_OPTIONS=--direction=,` to change the `cd` default direction to the common (most-visited) directories.

---

## Shell setup details

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

### PowerShell
```powershell
cd-deluxe --init powershell         # generates cdd.ps1 in current directory
. .\cdd.ps1                         # dot-source it (add to $PROFILE)
```

### Windows CMD
```bat
cd-deluxe --init cmd                # generates cdd.cmd in current directory
doskey cd=cdd.cmd $*
```

For Windows CMD and PowerShell there is an installer which sets up the necessary scripts.  Follow the notes in the installer.

---

## Build from source

```bash
cmake -B build && cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

Requires a C++20 compiler and CMake 3.20+. Dependencies (Catch2, cxxopts) are fetched automatically.

---

## Options reference

TODO HERE HERE HERE

| Option | Short | Description |
|--------|-------|-------------|
| `--direction=DIR` | `-d DIR` | Set direction: `-`, `+`, `,`, `..` or `b`, `f`, `c`, `u` |
| `--db / --df / --dc / --du` | | Shorthand direction flags |
| `--list` | `-l` | List without changing directory |
| `--all` | `-a` | Show full history (no limit) |
| `--max=N` | `-m N` | Maximum history entries (default 10) |
| `--fzf` | `-f` | Pipe listing through fzf |
| `--ignore-case` | `-i` | Case-insensitive path comparison |
| `--del ENTRY` | | Delete one history entry |
| `--gc` | | Garbage-collect duplicate entries |
| `--reset` | | Clear all history |
| `--init [SHELL]` | | Print shell integration code |
| `--help` | `-h` | Show help |
| `--version` | `-v` | Show version |
