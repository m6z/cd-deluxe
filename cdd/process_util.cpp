
#include "process_util.h"

#ifdef _WIN32

//----------------------------------------------------------------------

#include <windows.h>
#include <tlhelp32.h>
#include <string>

std::string get_parent_process_name()
{
    DWORD current_pid = GetCurrentProcessId();
    DWORD parent_pid = 0;
    std::string name;

    // Create snapshot of all processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return "";
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    // First pass: find parent PID of current process
    if (Process32FirstW(snapshot, &pe32))
    {
        do
        {
            if (pe32.th32ProcessID == current_pid)
            {
                parent_pid = pe32.th32ParentProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &pe32));
    }

    // Second pass: find parent process name
    if (parent_pid != 0)
    {
        pe32.dwSize = sizeof(pe32);
        if (Process32FirstW(snapshot, &pe32))
        {
            do
            {
                if (pe32.th32ProcessID == parent_pid)
                {
                    // Convert wide string to narrow string
                    int len = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1,
                                                  nullptr, 0, nullptr, nullptr);
                    if (len > 0)
                    {
                        name.resize(len - 1);
                        WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1,
                                            name.data(), len, nullptr, nullptr);
                    }
                    break;
                }
            } while (Process32NextW(snapshot, &pe32));
        }
    }

    CloseHandle(snapshot);

    // Remove .exe extension if present
    const std::string ext = ".exe";
    if (name.size() >= ext.size() &&
        name.compare(name.size() - ext.size(), ext.size(), ext) == 0)
    {
        name.erase(name.size() - ext.size());
    }

    return name;
}

#else

//----------------------------------------------------------------------

#include <algorithm> // for trim logic
#include <iostream>
#include <unistd.h> // for getppid()

// Platform specific headers
#if defined(__linux__)
#include <fstream>
#elif defined(__APPLE__)
#include <libproc.h>
#endif

namespace
{
// Helper to remove newlines (common in Linux /proc files)
void trim(std::string& s)
{
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
}
} // namespace

std::string get_parent_process_name()
{
    pid_t ppid = getppid(); // Get Parent PID
    std::string name;

#if defined(__linux__)
    // LINUX: Read from /proc/[pid]/comm
    std::string path = "/proc/" + std::to_string(ppid) + "/comm";
    std::ifstream ifs(path);
    if (ifs.is_open())
    {
        std::getline(ifs, name);
    }

#elif defined(__APPLE__)
    // MACOS: Use libproc API
    char buffer[PROC_PIDPATHINFO_MAXSIZE];
    // proc_name gets the short name (e.g. "zsh"),
    // proc_pidpath gets the full path (e.g. "/bin/zsh")
    int ret = proc_name(ppid, buffer, sizeof(buffer));
    if (ret > 0)
    {
        name = std::string(buffer);
    }
#else
    name = "unknown";
#endif

    trim(name);
    return name;
}

#endif
