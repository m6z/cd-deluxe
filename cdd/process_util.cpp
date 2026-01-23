// #include <algorithm> // for trim logic
// #include <iostream>
// #include <unistd.h> // for getppid()

#include "process_util.h"

// // Platform specific headers
// #if defined(__linux__)
// #include <fstream>
// #elif defined(__APPLE__)
// #include <libproc.h>
// #endif
//
// namespace
// {
// // Helper to remove newlines (common in Linux /proc files)
// void trim(std::string& s)
// {
//     s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
//     s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
// }
// } // namespace
//
// std::string get_parent_process_name()
// {
//     pid_t ppid = getppid(); // Get Parent PID
//     std::string name;
//
// #if defined(__linux__)
//     // LINUX: Read from /proc/[pid]/comm
//     std::string path = "/proc/" + std::to_string(ppid) + "/comm";
//     std::ifstream ifs(path);
//     if (ifs.is_open())
//     {
//         std::getline(ifs, name);
//     }
//
// #elif defined(__APPLE__)
//     // MACOS: Use libproc API
//     char buffer[PROC_PIDPATHINFO_MAXSIZE];
//     // proc_name gets the short name (e.g. "zsh"),
//     // proc_pidpath gets the full path (e.g. "/bin/zsh")
//     int ret = proc_name(ppid, buffer, sizeof(buffer));
//     if (ret > 0)
//     {
//         name = std::string(buffer);
//     }
// #else
//     name = "unknown";
// #endif
//
//     trim(name);
//     return name;
// }

std::string get_parent_process_name()
{
    return "TODO";
}
