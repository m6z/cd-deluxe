# Quick Start

Cd Deluxe is a drop-in replacement for the standard cd ("change directory") command. It supports easier access to the history of directories visited. It is kind of a "Swiss Army Knife" of changing directories. It is designed to increase productivity by speeding up the workflow of command line use. It is available for Windows and Unix style operating systems.

# Feature summary

| Example | Description |
| --- | --- |
| cdd | Show history of directories visited. |
| cdd <directory> | Change to <directory>. |
| cd - | Change to previous directory. |
| cd -- | Change to second previous directory. |
| cd -2 | Same as above - change to second previous directory. |
| cd --- | Change to third previous directory, and so on. |
| cd -3 | As above.  Any integer can be specified. |
| cd -? | Show history of directories in reverse order. |
| cd +? | Show history of directories in forward order, from first directory to last visited. |
| cd + | Change to the original directory from the start of the session. |
| cd +0 | Same as above. |
| cd 0 | Same as above. The '+' is optional. |
| cd +1 | Change to second visited directory. |
| cd ++ | As above. |
| cd +2 | Change to the third visited directory. |
| cd +++ | As above, and etcetera. |
| cd ,? | Show history of most commonly visited directories (using "comma" = "common") |
| cd , | Change to the most commonly visited directory. |
| cd ,, | Change to the second most common. |
| cd ,2 | Change to the third most common. |
| cd <regex> | Change to directory in history matching regular expression. |
| cd , <regex> | Change to most commonly visited directory in history matching regular expression. |
| cd + <regex> | Change to first visited directory in history matching regular expression. |
| cd - <regex> | Change to previous visited directory in history matching regular expression. |
| cd .. | Change up one directory. |
| cd ... | Change up two directories. |
| cd .... | Change up three directories, and etcetera. |
| cd --del <directory> | Delete from history directory. |
| cd --del -1 | Delete from history previous visited directory. |
| cd --del +0 | Delete from history the first visited directory. |
| cd --reset | Delete the entire history. |
| cd --gc | Garbage collect the history.  In case it gets too big/slow. |

