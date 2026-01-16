#include <cstdlib>
#include <filesystem>
#include <format>

#include "cdd_options_init.h"
#include "cxxopts.hpp"

namespace fs = std::filesystem;

int CddOptionsInit::parse(int argc, char* argv[], bool force_default_setup)
{
    try
    {
        // We use a dedicated parser for init/help to avoid conflict with the
        // main CddOptions
        cxxopts::Options options(argv[0], " - Shell integration setup");

        std::string init_shell_type;

        options.add_options()("h,help", "Show help")
            // explicit_value = false, implicit_value = "auto" allows:
            // --init        -> "auto"
            // --init bash   -> "bash"
            ("init", "Print shell integration code (auto/bash/zsh/fish)", cxxopts::value<std::string>(init_shell_type)->implicit_value("auto"));

        auto result = options.parse(argc, argv);

        if (result.count("help") && !force_default_setup)
        {
            output_stream_ << options.help() << std::endl;
            return 0; // Handled
        }

        // Resolve shell type
        std::string shell = init_shell_type;
        if (shell == "auto" || shell.empty())
        {
            shell = detect_shell();
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
        error_message_ = e.what();
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

std::string CddOptionsInit::detect_shell() const
{
    const char* shell_env = std::getenv("SHELL");
    if (!shell_env)
        return "bash"; // Default fallback

    std::string shell_path(shell_env);
    fs::path p(shell_path);

    // Returns "zsh", "bash", "fish", etc.
    return p.filename().string();
}

static constexpr const char* _bash_setup = R"(
# Add the following line to .bashrc or .zshrc to enable cd-deluxe integration:
source <("{}" --init)
)";

void CddOptionsInit::print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const
{
    if (shell_type == "fish")
    {
        // TODO
    }
    else
    {
        output_stream_ << std::format(_bash_setup, exe_path) << std::endl;
    }
}

static constexpr const char* _bash_init = R"(
cdd() {{
    _cdd_exe="{}"
    if [ ! -f "${{_cdd_exe}}" ]; then
        echo "Error: cd-deluxe executable not found at ${{_cdd_exe}}" >&2
        echo "Remove cdd function or unalias cd until the issue is resolved." >&2
        return 1
    fi

    dirs -l -p | "${{_cdd_exe}}" "$@" | while read x
    do
        eval $x > /dev/null
    done
}}

alias cd=cdd
echo "-- cd-deluxe shell integration loaded. See: cd --help."
)";

void CddOptionsInit::print_init_script(const std::string& shell_type, const std::string& exe_path) const
{
    if (shell_type == "fish")
    {
        // Fish shell syntax is significantly different
        output_stream_ << "function cdd\n";
        output_stream_ << "  if test -x \"" << exe_path << "\"\n";
        output_stream_ << "    set -l output (dirs -p | \"" << exe_path << "\" $argv)\n";
        output_stream_ << "    for x in $output\n";
        output_stream_ << "      eval $x\n";
        output_stream_ << "    end\n";
        output_stream_ << "  else\n";
        output_stream_ << "    echo \"cdd executable not found at: " << exe_path << "\"\n";
        output_stream_ << "  end\n";
        output_stream_ << "end\n";
        output_stream_ << "alias cd cdd\n";
        output_stream_ << "echo \"-- cd-deluxe shell integration loaded for fish. See: "
                          "cd --help.\"\n";
    }
    else
    {
        output_stream_ << std::format(_bash_init, exe_path);
    }
}
