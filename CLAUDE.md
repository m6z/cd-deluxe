# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CD-Deluxe is an enhanced `cd` command replacement providing advanced directory navigation with history support, pattern matching, and fuzzy finding. Supports Linux, macOS, and Windows.

## Build Commands

```bash
# Build (release)
cmake -B build1 && cmake --build build1

# Run all tests
./go-test.sh

# Run specific test by pattern
ctest --test-dir build1 -R <pattern>

# Debug with lldb
./go-debug.sh
```

## Architecture

### Directory Structure

- **cdd/** - Static library with core logic
- **main/** - Executable entry point (reads stdin, outputs shell commands)
- **test/** - Catch2 v3.6.0 test suite

### Key Source Files

- **cdd2.h/cpp** - Main C++20 implementation with `Cdd2` processor class
- **cdd_options.h/cpp** - Command-line argument parsing via cxxopts
- **cdd_options_init.h/cpp** - Shell initialization script generation
- **cdd_util.h/cpp** - Utility functions for path manipulation
- **fzf_wrapper.h/cpp** - FZF fuzzy finder integration

### Core Classes (cdd2.h)

- `Cdd2` - Main processor that handles all navigation modes
- `KeyedPath` - Normalized path comparison with case-sensitivity handling
- `CommonPath` - Tracks directory visit frequency
- `TaggedPath` - Display format with prefix tags for listing
- `RegexFilter` - Pattern matching support

### Navigation Model (Four Directions)

1. **Backward (`-`)** - Recent to oldest (via `create_dirs_last_to_first`)
2. **Forward (`+`)** - First to most recent (via `create_dirs_first_to_last`)
3. **Common (`,`)** - Most to least frequent (via `create_dirs_most_to_least`)
4. **Upward (`..` or `^`)** - Parent directories (via `create_dirs_upwards`)

### Data Flow

```
Shell → stdin (directory stack) → cdd executable → Cdd2 processing → stdout (shell commands)
```

The executable communicates via stdout (commands for shell) and stderr (user messages).

## Testing

Tests use Catch2 with `TEST_CASE` and `SECTION` organization. Run specific sections via:

```bash
lldb ./build1-debug/test/testmain -- -c "section name"
```

## Dependencies

Managed via CMake FetchContent:
- **cxxopts** v3.3.1 - CLI parsing
- **Catch2** v3.6.0 - Testing

## Standards

- C++20 required
- Uses `std::filesystem` for path operations
- Case-insensitive mode auto-enabled on Windows
