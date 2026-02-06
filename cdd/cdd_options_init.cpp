#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>

#include "cdd_options_init.h"
#include "cdd_util.h"
#include "cxxopts.hpp"
#include "process_util.h"

namespace fs = std::filesystem;

int CddOptionsInit::parse(int argc, char* argv[], bool force_default_setup)
{
    try
    {
        // We use a dedicated parser for init/help to avoid conflict with the
        // main CddOptions
        cxxopts::Options options(argv[0], " - Shell integration setup");

        std::string shell;

        options.add_options()       //
            ("h,help", "Show help") //
            ("v,version", "Show version")
        // explicit_value = false, implicit_value = "auto" allows:
        // --init        -> "auto"
        // --init bash   -> "bash" (Linux/macOS)
        // --init cmd    -> "cmd" (Windows)
#ifdef _WIN32
                ("init", "Run shell setup (auto/cmd/powershell/pwsh)", cxxopts::value<std::string>(shell)->implicit_value("auto"))
#else
                ("init", "Print shell integration code (auto/bash/zsh/fish)", cxxopts::value<std::string>(shell)->implicit_value("auto"))
#endif
            ;

        auto result = options.parse(argc, argv);

        std::cerr << "XX: parsed options - help: " << result.count("help") << ", version: " << result.count("version") << ", init: " << result.count("init")
                  << ", shell: '" << shell << "'\n";

        if (result.count("help") && !force_default_setup)
        {
            output_stream_ << options.help() << std::endl;
            return 0; // Handled
        }

        if (result.count("version"))
        {
            output_stream_ << "cd-deluxe version " << get_cdd_version() << std::endl;
            return 0; // Handled
        }

        // don't allow positional arguments
        if (!result.unmatched().empty())
        {
            output_stream_ << "Error - positional arguments not recognized:";
            for (const auto& arg : result.unmatched())
            {
                output_stream_ << " '" << arg << "'";
            }
            output_stream_ << std::endl;
            output_stream_ << options.help() << std::endl;
            return 1;
        }

        // Resolve shell type
        if (shell == "auto" || shell.empty())
        {
            shell = get_parent_process_name();
        }
        std::string exe_path = get_self_executable_path(argv[0]);

#ifdef _WIN32
        // On Windows, --init runs the setup steps (check/create wrapper + print help)
        // This is different from Linux/macOS where --init prints shell script code
        if (result.count("init") && !force_default_setup)
        {
            // Check if wrapper scripts exist and offer to create them
            if (shell == "cmd")
            {
                check_and_create_cmd_wrapper(exe_path);
            }
            else if (shell == "powershell" || shell == "pwsh")
            {
                check_and_create_ps1_wrapper(exe_path);
            }
            // Print setup help
            print_shell_setup_help(shell, exe_path);
            return 0; // Handled
        }

        // Default behavior (no options): also run setup steps
        if (shell == "cmd")
        {
            check_and_create_cmd_wrapper(exe_path);
        }
        else if (shell == "powershell" || shell == "pwsh")
        {
            check_and_create_ps1_wrapper(exe_path);
        }
#else
        // On Linux/macOS, --init prints shell script code for sourcing
        if (result.count("init") && !force_default_setup)
        {
            print_init_script(shell, exe_path);
            return 0; // Handled
        }
#endif

        // default: print setup help
        print_shell_setup_help(shell, exe_path);
        return 1; // Error condition
    }
    catch (const std::exception& e)
    {
        output_stream_ << "Error parsing options: " << e.what() << std::endl;
    }

    return false; // Not an init/help command, let main program continue
}

std::string CddOptionsInit::get_self_executable_path(const char* argv0) const
{
    std::error_code ec;
    std::string result;

    // 1. Try to get path via OS-specific symlink (Linux/macOS)
    // /proc/self/exe is Linux standard.
    fs::path p = fs::read_symlink("/proc/self/exe", ec);
    if (!ec)
    {
        result = p.string();
    }

    // 2. Fallback: use argv[0] and make it absolute
    // This handles cases where the program was called via relative path ./cdd
    if (result.empty())
    {
        p = fs::absolute(argv0, ec);
        if (!ec)
        {
            result = p.string();
        }
    }
    if (result.empty())
    {
        result = "cd-deluxe"; // Rely on PATH
    }

    // Normalize path separators for shell usage (generic_string uses forward
    // slashes)
    result = fs::path(result).generic_string();
    return result;
}

static constexpr const char* _bash_setup = R"_(
# Add the following lines to ~/.bash_profile or ~/.bashrc to enable cd-deluxe integration:
if [ "${{BASH_VERSINFO:-0}}" -le 3 ]; then
    # For bash version 3 or lower
    eval "$('{}' --init)"
else
    #For bash version 4 or higher
    source <('{}' --init)
fi
)_";

static constexpr const char* _zsh_setup = R"(
# Add the following line to ~/.zshrc to enable cd-deluxe integration:
source <("{}" --init)
)";

static constexpr const char* _fish_setup = R"(
# Add the following line to ~/.config/fish/config.fish to enable cd-deluxe integration:
"{}" --init | source
)";

static constexpr const char* _cmd_setup = R"(
==== Windows Command Prompt CD-Deluxe Usage ====

To use cd-deluxe from the Windows command prompt (cmd.exe), the script 'cdd.cmd' is needed as a wrapper.

There are two options for easy integration:
    1. Create a 'cd' macro to invoke 'cdd.cmd'.  This will override the built-in 'cd' command.
        -or-
    2. Add the path to 'cdd.cmd' to your PATH environment variable and use by invoking 'cdd'

Option 1:
    To create a 'cd' macro, add the following line to your cmd.exe startup or run it in the command prompt:
        doskey cd="{0}\cdd.cmd" $*

Option 2
    To use cdd.cmd directly without a macro, add the following to your PATH environment variable:
        {0}
    For example, to add it for the current session, run:
        set PATH=%PATH%;{0}

When using the cd-deluxe windows installer, cdd.cmd is created automatically and the PATH is updated.
)";

static constexpr const char* _powershell_setup = R"(
==== Windows PowerShell CD-Deluxe Usage ====

To use cd-deluxe from PowerShell, dot-source the 'cdd.ps1' script in your PowerShell profile.

Add the following line to your PowerShell profile ($PROFILE):
    . "{0}\cdd.ps1"

To optionally override the built-in 'cd' command, add this after the dot-source line:
    Set-Alias -Name cd -Value cdd -Option AllScope -Scope Global

The cdd.ps1 script provides the 'cdd' function for enhanced directory navigation.
)";

void CddOptionsInit::print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const
{
    if (shell_type == "fish")
    {
        output_stream_ << std::format(_fish_setup, exe_path) << std::endl;
    }
    else if (shell_type == "bash")
    {
        output_stream_ << std::format(_bash_setup, exe_path, exe_path) << std::endl;
    }
    else if (shell_type == "zsh")
    {
        output_stream_ << std::format(_zsh_setup, exe_path) << std::endl;
    }
    else if (shell_type == "cmd")
    {
        fs::path exe_dir = fs::path(exe_path).parent_path().make_preferred();
        output_stream_ << std::format(_cmd_setup, exe_dir.string());
    }
    else if (shell_type == "powershell" || shell_type == "pwsh")
    {
        fs::path exe_dir = fs::path(exe_path).parent_path().make_preferred();
        output_stream_ << std::format(_powershell_setup, exe_dir.string());
    }
    else
    {
        output_stream_ << "# Shell type '" << shell_type << "' not specifically supported.\n";
    }
}

// NOTE: the "while read x" loop (below) works across both bash and zsh to ensure eval is run in the current shell context.

static constexpr const char* _bash_init = R"(
# cd-deluxe integration for bash/zsh

cdd() {{
    _cdd_exe="{}"
    if [ ! -f "${{_cdd_exe}}" ]; then
        echo "Error: cd-deluxe executable not found at ${{_cdd_exe}}" >&2
        echo "Remove cdd function or unalias cd until the issue is resolved." >&2
        return 1
    fi
    while read x
    do
       eval $x > /dev/null
    done < <(dirs -l -p | "${{_cdd_exe}}" "$@")
}}

alias cd=cdd
echo "-- cd-deluxe shell integration loaded. See: cd --help."
)";

static constexpr const char* _fish_init = R"(
# cd-deluxe integration for fish shell

function cdd
    # Recursion guard - use builtin cd if called from pushd/popd
    if string match -q -- "*pushd*" (status stack-trace) \
       or string match -q -- "*popd*" (status stack-trace)
        builtin cd $argv
        return
    end
    # Verify the cd-deluxe executable exists
    set -l _cdd_exe "{}"
    if not test -f "$_cdd_exe"
        echo "Error: cd-deluxe executable not found at $_cdd_exe" >&2
        echo "Remove cd or cdd functions until the issue is resolved." >&2
        return 1
    end
    # Run the executable and process its output
    for x in (string join \n $PWD $dirstack | "$_cdd_exe" $argv)
        # Also ensure explicit 'cd' commands from the C++ app use builtin
        eval $x > /dev/null
    end
end

alias cd cdd
echo "-- cd-deluxe shell integration loaded. See: cd --help."
)";

void CddOptionsInit::print_init_script(const std::string& shell_type, const std::string& exe_path) const
{
    if (shell_type == "fish")
    {
        output_stream_ << std::format(_fish_init, exe_path);
    }
    else if (shell_type == "bash" || shell_type == "zsh")
    {
        output_stream_ << std::format(_bash_init, exe_path);
    }
    else
    {
        output_stream_ << "Error: shell '" << shell_type << "' unknown or not supported.\n";
    }
}

#ifdef _WIN32
static constexpr const char* _cmd_wrapper_content = R"(@echo off
set pushd_tmp=%TEMP%\pushd.tmp
set cdd_tmp_cmd=%TEMP%\cdd.tmp.cmd
pushd > %pushd_tmp%
%~dps0cd-deluxe.exe %* < %pushd_tmp% > %cdd_tmp_cmd%
%cdd_tmp_cmd%
)";

void CddOptionsInit::check_and_create_cmd_wrapper(const std::string& exe_path)
{
    fs::path exe_fs_path(exe_path);
    fs::path exe_dir = exe_fs_path.parent_path();
    fs::path cmd_path = exe_dir / "cdd.cmd";

    if (fs::exists(cmd_path))
    {
        // print a message that the file was successfully found
        output_stream_ << "Found existing cdd.cmd wrapper at: " << cmd_path.make_preferred().string() << "\n";
        return; // cdd.cmd already exists
    }

    output_stream_ << "\nThe wrapper script 'cdd.cmd' was not found in: " << exe_dir.make_preferred().string() << "\n";
    output_stream_ << "This script is required to use cd-deluxe from the Windows command prompt.\n";
    output_stream_ << "Would you like to create it now? [Y/n]: ";
    output_stream_.flush();

    std::string response;
    std::getline(input_stream_, response);

    // Default to yes if empty or starts with 'y' or 'Y'
    if (response.empty() || response[0] == 'y' || response[0] == 'Y')
    {
        std::ofstream cmd_file(cmd_path);
        if (cmd_file)
        {
            cmd_file << _cmd_wrapper_content;
            cmd_file.close();
            output_stream_ << "Created: " << cmd_path.make_preferred().string() << "\n";
            // output_stream_ << "You can now use 'cdd' from the command prompt.\n";
            // output_stream_ << "To use it from any directory, add the following to your PATH:\n";
            // output_stream_ << "  " << exe_dir.string() << "\n";
        }
        else
        {
            output_stream_ << "Error: Failed to create " << cmd_path.string() << "\n";
        }
    }
    else
    {
        output_stream_ << "Skipped creating cdd.cmd. You can manually copy it from the install directory.\n";
    }
}

static constexpr const char* _ps1_wrapper_content = R"PS1(# cd-deluxe integration for PowerShell
# This script provides the 'cdd' function for enhanced directory navigation
# Dot-source this file in your PowerShell profile: . "PATH\TO\cdd.ps1"

function cdd {
    # Use $args to avoid PowerShell's parameter parsing interfering with cd-deluxe arguments
    # (e.g., "-d," would cause parser errors with declared parameters)

    $exePath = Join-Path $PSScriptRoot "cd-deluxe.exe"
    if (-not (Test-Path $exePath)) {
        Write-Error "cd-deluxe.exe not found at $exePath"
        return
    }

    # Build the directory stack input (current dir + pushed locations)
    # This mimics what 'pushd' outputs in cmd.exe
    $stackInput = @((Get-Location).Path)
    $locationStack = Get-Location -Stack -ErrorAction SilentlyContinue
    if ($locationStack) {
        $stackInput += $locationStack.Path
    }

    # Run cd-deluxe.exe with input and capture output
    $output = $stackInput | & $exePath @args 2>&1

    # Separate stdout and stderr
    $stdoutLines = @()
    $stderrLines = @()
    foreach ($item in $output) {
        if ($item -is [System.Management.Automation.ErrorRecord]) {
            $stderrLines += $item.ToString()
        } else {
            $stdoutLines += $item
        }
    }

    # Display stderr messages to user
    foreach ($line in $stderrLines) {
        Write-Host $line
    }

    # Process each line of stdout, translating cmd.exe commands to PowerShell
    foreach ($line in $stdoutLines) {
        # Skip empty lines
        if ([string]::IsNullOrWhiteSpace($line)) { continue }

        # Translate cmd.exe commands to PowerShell
        if ($line -match '^for /l %%i in \(1,1,(\d+)\) do popd$') {
            # Batch loop to pop N times
            $count = [int]$Matches[1]
            1..$count | ForEach-Object { Pop-Location -ErrorAction SilentlyContinue }
        }
        elseif ($line -match '^chdir/d (.+?)(?: 2>nul)?$') {
            # chdir/d path -> Set-Location path
            Set-Location $Matches[1] -ErrorAction SilentlyContinue
        }
        elseif ($line -match '^pushd "?(.+?)"?(?: 2>nul)?$') {
            # pushd path (with or without quotes)
            Push-Location $Matches[1] -ErrorAction SilentlyContinue
        }
        elseif ($line -match '^popd$') {
            Pop-Location -ErrorAction SilentlyContinue
        }
        elseif ($line -match '^cd /d "?(.+?)"?$') {
            # cd /d path -> Set-Location path
            Set-Location $Matches[1] -ErrorAction SilentlyContinue
        }
        else {
            # Try to execute as-is for any other commands
            try {
                Invoke-Expression $line 2>$null
            } catch {
                # Silently ignore execution errors
            }
        }
    }
}

Write-Host "-- cd-deluxe PowerShell integration loaded. See: cdd --help"
)PS1";

void CddOptionsInit::check_and_create_ps1_wrapper(const std::string& exe_path)
{
    fs::path exe_fs_path(exe_path);
    fs::path exe_dir = exe_fs_path.parent_path();
    fs::path ps1_path = exe_dir / "cdd.ps1";

    if (fs::exists(ps1_path))
    {
        output_stream_ << "Found existing cdd.ps1 wrapper at: " << ps1_path.make_preferred().string() << "\n";
        return; // cdd.ps1 already exists
    }

    output_stream_ << "\nThe wrapper script 'cdd.ps1' was not found in: " << exe_dir.make_preferred().string() << "\n";
    output_stream_ << "This script is required to use cd-deluxe from PowerShell.\n";
    output_stream_ << "Would you like to create it now? [Y/n]: ";
    output_stream_.flush();

    std::string response;
    std::getline(input_stream_, response);

    // Default to yes if empty or starts with 'y' or 'Y'
    if (response.empty() || response[0] == 'y' || response[0] == 'Y')
    {
        std::ofstream ps1_file(ps1_path);
        if (ps1_file)
        {
            ps1_file << _ps1_wrapper_content;
            ps1_file.close();
            output_stream_ << "Created: " << ps1_path.make_preferred().string() << "\n";
        }
        else
        {
            output_stream_ << "Error: Failed to create " << ps1_path.make_preferred().string() << "\n";
        }
    }
    else
    {
        output_stream_ << "Skipped creating cdd.ps1.\n";
    }
}
#endif
