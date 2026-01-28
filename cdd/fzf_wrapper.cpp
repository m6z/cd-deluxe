#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace
{
// Helper to remove the trailing newline fzf returns
std::string trim_newline(std::string s)
{
    if (!s.empty() && s.back() == '\n')
    {
        s.pop_back();
    }
    if (!s.empty() && s.back() == '\r')
    {
        s.pop_back();
    }
    return s;
}
} // namespace

#ifdef WIN32

std::string run_fzf(const std::string& inputs)
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create pipes for stdin
    HANDLE hStdinRead, hStdinWrite;
    if (!CreatePipe(&hStdinRead, &hStdinWrite, &sa, 0))
    {
        throw std::runtime_error("Failed to create stdin pipe");
    }
    // Ensure write handle is not inherited
    SetHandleInformation(hStdinWrite, HANDLE_FLAG_INHERIT, 0);

    // Create pipes for stdout
    HANDLE hStdoutRead, hStdoutWrite;
    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0))
    {
        CloseHandle(hStdinRead);
        CloseHandle(hStdinWrite);
        throw std::runtime_error("Failed to create stdout pipe");
    }
    // Ensure read handle is not inherited
    SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hStdinRead;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // fzf.exe should be in PATH
    char cmdLine[] = "fzf";

    if (!CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(hStdinRead);
        CloseHandle(hStdinWrite);
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        throw std::runtime_error("Failed to start fzf. Check that fzf is installed and in your PATH.");
    }

    // Close handles not needed by parent
    CloseHandle(hStdinRead);
    CloseHandle(hStdoutWrite);

    // Write inputs to fzf
    DWORD bytesWritten;
    WriteFile(hStdinWrite, inputs.c_str(), static_cast<DWORD>(inputs.size()), &bytesWritten, NULL);
    CloseHandle(hStdinWrite); // Send EOF

    // Read output from fzf
    std::string result;
    std::array<char, 128> buffer;
    DWORD bytesRead;
    while (ReadFile(hStdoutRead, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, NULL) && bytesRead > 0)
    {
        result.append(buffer.data(), bytesRead);
    }
    CloseHandle(hStdoutRead);

    // Wait for fzf to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // fzf exit codes: 0 = success, 1 = no match, 130 = cancelled
    if (exitCode == 0)
    {
        return trim_newline(result);
    }

    return "";
}

#else // POSIX

std::string run_fzf(const std::string& inputs)
{
    int pipe_in[2];  // Parent writes to fzf, Child reads from Parent
    int pipe_out[2]; // Child writes result, Parent reads from fzf

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
    {
        throw std::runtime_error("Failed to create pipes");
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        throw std::runtime_error("Failed to fork");
    }

    if (pid == 0)
    {
        // --- CHILD PROCESS (fzf) ---

        // 1. Setup Stdin: Read from pipe_in[0]
        dup2(pipe_in[0], STDIN_FILENO);

        // 2. Setup Stdout: Write to pipe_out[1]
        dup2(pipe_out[1], STDOUT_FILENO);

        // 3. Close unused pipe ends in child
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);

        // 4. Execute fzf
        // Note: fzf will automatically open /dev/tty for the interactive UI
        // because we haven't redirected Stderr.
        execlp("fzf", "fzf", nullptr);

        // If we get here, exec failed
        std::cerr << "Failed to run fzf: " << strerror(errno) << std::endl;
        std::cerr << "Check that fzf is installed and in your PATH." << std::endl;
        _exit(1);
    }
    else
    {
        // --- PARENT PROCESS ---

        // 1. Close the ends we don't need
        close(pipe_in[0]);  // We don't read from input pipe
        close(pipe_out[1]); // We don't write to output pipe

        // 2. Write inputs to fzf
        FILE* fd = fdopen(pipe_in[1], "w");
        if (fd)
        {
            fprintf(fd, "%s", inputs.c_str());

            // IMPORTANT: We must close the stream/fd to send EOF to fzf.
            // fzf will not show the list until it receives EOF or fills a buffer.
            fclose(fd);
        }
        // pipe_in[1] is now closed by fclose

        // 3. Read the selection from fzf
        std::string result;
        std::array<char, 128> buffer;
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_out[0], buffer.data(), buffer.size())) > 0)
        {
            result.append(buffer.data(), bytesRead);
        }
        close(pipe_out[0]);

        // 4. Wait for child to finish to prevent zombies
        int status;
        waitpid(pid, &status, 0);

        // Check if fzf exited successfully (0 = success, 1 = no match, 130 = user cancel)
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            return trim_newline(result);
        }

        // User cancelled or no match
        return "";
    }
}

#endif
