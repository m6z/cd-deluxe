#include "cdd_options_init.h"
#include "cxxopts.hpp"

#include <cstdlib>
#include <filesystem>

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

void CddOptionsInit::print_init_script(const std::string& shell_type, const std::string& exe_path) const
{
    // Normalize path separators for shell usage (generic_string uses forward slashes)
    std::string safe_exe_path = fs::path(exe_path).generic_string();

    if (shell_type == "fish")
    {
        // Fish shell syntax is significantly different
        std::cout << "function cd\n";
        std::cout << "  if test -x \"" << safe_exe_path << "\"\n";
        std::cout << "    set -l output (dirs -p | \"" << safe_exe_path << "\" $argv)\n";
        std::cout << "    for x in $output\n";
        std::cout << "      eval $x\n";
        std::cout << "    end\n";
        std::cout << "  else\n";
        std::cout << "    echo \"cdd executable not found at: " << safe_exe_path << "\"\n";
        std::cout << "  end\n";
        std::cout << "end\n";
    }
    else
    {
        // Default to Bash/Zsh syntax (as provided in prompt)
        // We wrap the path in quotes to handle spaces in directory names
        std::cout << "if [[ -x \"" << safe_exe_path << "\" ]]; then\n";

        // Note: zsh supports 'function name', bash supports 'function name'.
        // To be safe for strictly POSIX shells that might not like 'function',
        // 'cd() { ... }' is safer, but the prompt specifically requested the 'function cd' style.
        std::cout << "  function cdd { while read x; do eval $x >/dev/null; done < <(dirs -l -p | \"" << safe_exe_path << "\" \"$@\"); }\n";
        std::cout << "  alias cd='cdd'\n";
        std::cout << "  echo \"-- cd-deluxe shell integration loaded. Use 'cd' as usual.\"\n";

        std::cout << "fi\n";
    }
}
