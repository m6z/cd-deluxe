# cd-deluxe

A supercharged `cd` replacement for the command line. Navigate your directory history by recency, frequency, or position — with regex filtering and fuzzy-find on top.

Supports **Bash**, **Zsh**, **Fish**, **PowerShell**, and **Windows CMD**. Runs on Linux, macOS, and Windows.

---

## Quick Start

```bash
# One-time shell setup (add to ~/.bashrc, ~/.zshrc, etc.)
eval "$(cd-deluxe --init)"        # bash
source <(cd-deluxe --init)        # zsh
cd-deluxe --init | source         # fish
```

After that, use `cdd` (or `cd`, which is aliased) normally. The rest is automatic.

---

## Navigation

cd-deluxe tracks your directory history in three independent orderings:

| Symbol | Direction | Meaning                  |
|--------|-----------|--------------------------|
| `-`    | Backward  | Most recent → oldest     |
| `+`    | Forward   | First visited → most recent |
| `,`    | Common    | Most visited → least     |
| `..`   | Upward    | Parent directory chain   |

### By count or repetition

```
cdd -          # go to previous directory
cdd -2         # two back  (same as cdd --)
cdd ---        # three back (same as cdd -3)

cdd +          # go to earliest directory this session
cdd +2         # second earliest  (same as cdd ++)

cdd ,          # go to most-visited directory
cdd ,2         # second most-visited  (same as cdd ,,)

cdd ..         # up one level
cdd ...        # up two levels  (same as cdd ..2)
cdd ....       # up three levels
```

### List before jumping

Append `?` to any direction to display the list without changing directory:

```
cdd -?         # list history, most recent first
cdd +?         # list history, oldest first
cdd ,?         # list history, most visited first  (with visit counts)
cdd ..?        # list parent directories upward
```

### Regex filtering

Add a pattern to narrow the match:

```
cdd ee         # most recent directory whose path contains "ee"
cdd cc$        # most recent path ending with "cc"
cdd , src      # most-visited directory containing "src"
cdd + projects # earliest directory containing "projects"
cdd .. work    # nearest parent directory matching "work"
```

Patterns are standard C++ regex (so `^`, `$`, `.*`, `[...]`, etc. all work).

### Fuzzy find (fzf)

```
cdd -f         # pipe backward list through fzf
cdd -f ,       # pipe common list through fzf
cdd -f ..      # pipe parent list through fzf
```

Requires [fzf](https://github.com/junegunn/fzf) to be on your `PATH`.

---

## Listing and history size

```
cdd            # default: list recent history (same as cdd -?)
cdd -l         # explicit list  (respects current direction)
cdd -a         # show all history (no truncation)
cdd -m 20      # set max history length to 20 (default: 10)
```

The common listing includes visit counts:

```
$ cdd ,?
 ,1: ( 7) /home/mike/projects/cd-deluxe
 ,2: ( 4) /home/mike/projects
 ,3: ( 1) /tmp
```

---

## History management

```
cdd --del -2         # remove the second-most-recent entry
cdd --del +0         # remove the oldest entry
cdd --gc             # garbage-collect duplicates
cdd --reset          # wipe the entire history
```

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
cdd -d f src         # forward search for "src"
cdd --dc             # most-visited directory
```

---

## Upward navigation in detail

`..` and its shorthands walk up the current directory's parent chain.

```
# Current directory: /home/mike/projects/cd-deluxe/test

cdd ..             →  /home/mike/projects/cd-deluxe
cdd ...            →  /home/mike/projects
cdd ....           →  /home/mike
cdd .. projects    →  /home/mike/projects   (first parent matching "projects")
cdd ..?            lists all parents
```

Dot expansion also works as part of a path argument: `cdd .../foo` expands to `../../foo`.

---

## Environment variable

Set default options in `CDD_OPTIONS` to apply them to every invocation:

```bash
export CDD_OPTIONS="--ignore-case --max=20"
```

---

## Shell setup details

### Bash
```bash
eval "$(cd-deluxe --init)"          # add to ~/.bashrc
```

### Zsh
```zsh
source <(cd-deluxe --init)          # add to ~/.zshrc
```

### Fish
```fish
cd-deluxe --init | source           # add to ~/.config/fish/config.fish
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

---

## Build from source

```bash
cmake -B build && cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

Requires a C++20 compiler and CMake 3.20+. Dependencies (Catch2, cxxopts) are fetched automatically.

---

## Options reference

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
