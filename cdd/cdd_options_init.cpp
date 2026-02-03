#include <cstdlib>
#include <filesystem>
#include <format>

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

        options.add_options()("h,help", "Show help")
            // explicit_value = false, implicit_value = "auto" allows:
            // --init        -> "auto"
            // --init bash   -> "bash"
            ("init", "Print shell integration code (auto/bash/zsh/fish)", cxxopts::value<std::string>(shell)->implicit_value("auto"));

        auto result = options.parse(argc, argv);

        if (result.count("help") && !force_default_setup)
        {
            output_stream_ << options.help() << std::endl;
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

        if (result.count("init") && !force_default_setup)
        {
            print_init_script(shell, exe_path);
            return 0; // Handled
        }

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

static constexpr const char* _bash_setup = R"(
# Add the following line to ~/.bashrc to enable cd-deluxe integration:
source <("{}" --init)
)";

static constexpr const char* _zsh_setup = R"(
# Add the following line to ~/.zshrc to enable cd-deluxe integration:
source <("{}" --init)
)";

static constexpr const char* _fish_setup = R"(
# Add the following line to ~/.config/fish/config.fish to enable cd-deluxe integration:
"{}" --init | source
)";

void CddOptionsInit::print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const
{
    if (shell_type == "fish")
    {
        output_stream_ << std::format(_fish_setup, exe_path) << std::endl;
    }
    else if (shell_type == "bash")
    {
        output_stream_ << std::format(_bash_setup, exe_path) << std::endl;
    }
    else if (shell_type == "zsh")
    {
        output_stream_ << std::format(_zsh_setup, exe_path) << std::endl;
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
        echo "Error: cd-deluxe executable not found at ${{_cdd_exe}}" >&2
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
