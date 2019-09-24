# Quick Start

Cd Deluxe is a drop-in replacement for the standard cd ("change directory") command. It supports easier access to the history of directories visited. It is kind of a "Swiss Army Knife" of changing directories. It is designed to increase productivity by speeding up the workflow of command line use. It is available for Windows and Unix style operating systems.

# Feature summary

| Example | Description |
| --- | --- |
| cdd | Show history of directories visited. |
| cdd <directory> | Change to <directory>. |
| cdd - | Change to previous directory. |
| cdd -- | Change to second previous directory. |
| cdd -2 | Same as above - change to second previous directory. |
| cdd --- | Change to third previous directory, and so on. |
| cdd -3 | As above.  Any integer can be specified. |
| cdd -? | Show history of directories in reverse order. |
| cdd +? | Show history of directories in forward order, from first directory to last visited. |
| cdd + | Change to the original directory from the start of the session. |
| cdd +0 | Same as above. |
| cdd 0 | Same as above. The '+' is optional. |
| cdd +1 | Change to second visited directory. |
| cdd ++ | As above. |
| cdd +2 | Change to the third visited directory. |
| cdd +++ | As above, and etcetera. |
| cdd ,? | Show history of most commonly visited directories (using "comma" = "common") |
| cdd , | Change to the most commonly visited directory. |
| cdd ,, | Change to the second most common. |
| cdd ,2 | Change to the third most common. |
| cdd <regex> | Change to directory in history matching regular expression. |
| cdd , <regex> | Change to most commonly visited directory in history matching regular expression. |
| cdd + <regex> | Change to first visited directory in history matching regular expression. |
| cdd - <regex> | Change to previous visited directory in history matching regular expression. |
| cdd .. | Change up one directory. |
| cdd ... | Change up two directories. |
| cdd .... | Change up three directories, and etcetera. |
| cdd --del <directory> | Delete from history directory. |
| cdd --del -1 | Delete from history previous visited directory. |
| cdd --del +0 | Delete from history the first visited directory. |
| cdd --reset | Delete the entire history. |
| cdd --gc | Garbage collect the history.  In case it gets too big/slow. |

# Examples

The simplest use is to change to another directory. In this way it is no different than the standard cd command:

'''
=> D:\Users\Mike\
=> cdd Development

=> D:\Users\Mike\Development\
'''

In the examples above and following, the top line indicates the current directory, the line below it shows the cdd command that was issued, and the last line shows the resulting directory after cdd command.

When entered without any parameters it will by default list out this history of directories visited in reverse order (without changing the directory):

'''
=> D:\Users\Mike\Development\
=> cdd
 -1: D:\Users\Mike

=> D:\Users\Mike\Development\
'''
