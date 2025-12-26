#include "cdd_options_init.h"
#include "cxxopts.hpp"

#include <cstdlib>
#include <filesystem>
#include <format>

namespace fs = std::filesystem;

bool CddOptionsInit::parse(int argc, char* argv[])
{
    try
    {
        // We use a dedicated parser for init/help to avoid conflict with the main CddOptions
        cxxopts::Options options(argv[0], " - Shell integration setup");

        std::string init_shell_type;

        options.add_options()("h,help", "Show help")
            // explicit_value = false, implicit_value = "auto" allows:
            // --init        -> "auto"
            // --init bash   -> "bash"
            ("init", "Print shell integration code", cxxopts::value<std::string>(init_shell_type)->implicit_value("auto"));

        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return true; // Handled
        }

        if (result.count("init"))
        {
            std::string exe_path = get_self_executable_path(argv[0]);

            // Resolve shell type
            std::string shell = init_shell_type;
            if (shell == "auto" || shell.empty())
            {
                shell = detect_shell();
            }

            print_init_script(shell, exe_path);
            return true; // Handled
        }
    }
    catch (const std::exception& e)
    {
        error_message = e.what();
    }

    return false; // Not an init/help command, let main program continue
}

std::string CddOptionsInit::get_self_executable_path(const char* argv0) const
{
    std::error_code ec;

    // 1. Try to get path via OS-specific symlink (Linux/macOS)
    // /proc/self/exe is Linux standard.
    fs::path p = fs::read_symlink("/proc/self/exe", ec);
    if (!ec)
        return p.string();

    // 2. Fallback: use argv[0] and make it absolute
    // This handles cases where the program was called via relative path ./cdd
    p = fs::absolute(argv0, ec);
    if (!ec)
        return p.string();

    // 3. Ultimate fallback (should rarely happen)
    return "cdd";
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

static constexpr const char* _bash_init = R"(
function cdd {{
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

void CddOptionsInit::print_init_script(const std::string& shell_type, const std::string& exe_path) const
{
    // Normalize path separators for shell usage (generic_string uses forward slashes)
    std::string safe_exe_path = fs::path(exe_path).generic_string();

    if (shell_type == "fish")
    {
        // Fish shell syntax is significantly different
        std::cout << "function cdd\n";
        std::cout << "  if test -x \"" << safe_exe_path << "\"\n";
        std::cout << "    set -l output (dirs -p | \"" << safe_exe_path << "\" $argv)\n";
        std::cout << "    for x in $output\n";
        std::cout << "      eval $x\n";
        std::cout << "    end\n";
        std::cout << "  else\n";
        std::cout << "    echo \"cdd executable not found at: " << safe_exe_path << "\"\n";
        std::cout << "  end\n";
        std::cout << "end\n";
        std::cout << "alias cd cdd\n";
        std::cout << "echo \"-- cd-deluxe shell integration loaded for fish. See: cd --help.\"\n";
    }
    else
    {
        std::cout << std::format(_bash_init, safe_exe_path);
    }
}
