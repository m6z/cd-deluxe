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

    => D:\Users\Mike\
    => cdd Development

    => D:\Users\Mike\Development\

In the examples above and following, the top line indicates the current directory, the line below it shows the cdd command that was issued, and the last line shows the resulting directory after cdd command.

When entered without any parameters it will by default list out this history of directories visited in reverse order (without changing the directory):

    => D:\Users\Mike\Development\
    => cdd
     -1: D:\Users\Mike

As more directories are visited, the history of directories gets built up:

    => D:\Users\Mike\Development\
    => cdd cd_deluxe

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd
     -1: D:\Users\Mike\Development
     -2: D:\Users\Mike

    => D:\Users\Mike\Development\cd_deluxe\

The number listed before each directory in the history listing is a relative offset from the current directory. To return to a previous directory a '-' and number can be entered:

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd -2
    cdd: D:\Users\Mike

    => D:\Users\Mike\

This changes to the current directory to the second previous directory. Note: whenever cdd changes to a directory with a behavior different to the standard cd command it prints a message indicating the action it has taken. In the above example "cdd: D:\Users\Mike" is displayed as the directory is being changed.

To change to the previous directory a single '-' can be entered, or else '-1':

    => D:\Users\Mike\
    => cdd -
    cdd: D:\Users\Mike\Development\cd_deluxe

    => D:\Users\Mike\Development\cd_deluxe\

Of course the "cd -" command has been available in Unix operating systems forever but this has not been available on Windows.

As an alternative to specifying a '-' and number a sequence of dashes can be used:

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd --
    cdd: D:\Users\Mike\Development

    => D:\Users\Mike\Development\

Entering "cdd --" is the same as entering "cdd -2". This is sometimes more convenient that typing a '-' and a number. Also it is generally quicker since after typing the first '-' it is not necessary to move to another location on the keyboard. Every second counts!

Any '-' and number can be specified, or consecutive series of dashes. For example "cdd -3" or "cdd ---" will go back to the third most recent directory.

The default action for when cdd is typed with no parameters is to show the history in the reverse order. The default action can be changed (more on that later). To specifically indicate that the reverse history is to be displayed the '-?' option can be specified:

    => D:\Users\Mike\Development\
    => cdd -?
     -1: D:\Users\Mike\Development\cd_deluxe
     -2: D:\Users\Mike

    => D:\Users\Mike\Development\

The question mark '?' when used in a parameter is an instruction to display the directory history. The '-' before the question mark signifies the direction. So the '-?' has the full meaning of: display the directory history from last visited to first visited.

As an alternative to the '-' direction, the '+' direction can be specified. This displays the directory history from first visited to last visited:

    => D:\Users\Mike\Development\
    => cdd +?
      0: D:\Users\Mike
      1: D:\Users\Mike\Development
      2: D:\Users\Mike\Development\cd_deluxe

    => D:\Users\Mike\Development\

This displays the directories from the zeroth visited onwards. Note that this uses base 0 numbering so 0 signifies the first. In the history listing the plus direction is inferred as opposed to the backwards direction where the '-' is indicated.

Similar to using the '-' and number command to move backwards through the directory history, a '+' and number can be used to move to a directory numbered from first to last visited:

    => D:\Users\Mike\Development\
    => cdd +0
    cdd: D:\Users\Mike

    => D:\Users\Mike\

Since this is the positive (forwards) direction, the +0 can simply be stated as 0. So "cdd 0" is the same as "cdd +0". Of course as before any number can be specified to jump to that directory. And a consecutive series of pluses can be used. For example to jump to the third visited directory:

    => D:\Users\Mike\
    => cdd +++
    cdd: D:\Users\Mike\Development\cd_deluxe

    => D:\Users\Mike\Development\cd_deluxe\

So '-' is used to specify a directory in backwards visited order, and '+' is used to specify in forwards visited order. So that's it, right? Not so fast! There is also a way to specify s directory in most common visited order. That direction is the comma: ','. Comma sort of sounds like "common" so that makes it easy to remember. So, to see the history of directories listed in most to least visited enter the comma direction and a question mark ",?":

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd ,?
     ,0: ( 3) D:\Users\Mike\Development\cd_deluxe
     ,1: ( 3) D:\Users\Mike
     ,2: ( 2) D:\Users\Mike\Development

    => D:\Users\Mike\Development\cd_deluxe\

Here the history list is similar but a little different. Each directory has a precise specification of ',' and a number. The most frequently visited directory is indicated as ',0'. The second most commonly visited is ',1' and so on. In case of multiple directories only being visited once the most recently visited directory has precedence in the list. The number in parenthesis in the history listing is the number of times the directory has been visited. So in the above example the top directory in the history has been visited 3 times. A shortcut for getting back to the most visited directory is "cdd ,".

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd test

    => D:\Users\Mike\Development\cd_deluxe\test\
    => cdd ,
    cdd: D:\Users\Mike\Development\cd_deluxe

    => D:\Users\Mike\Development\cd_deluxe\

Commas can be strung together to change to the second most common directory, and so on. Or a comma and a number can be specified.

    => D:\Users\Mike\Development\cd_deluxe\
    => cdd ,,
    cdd: D:\Users\Mike

    => D:\Users\Mike\

So to review - there is the possibility to list out the directory history in three different ways or directions: forwards (first to last visited), backwards (last to first visited) and common (most to least visited). And directories can be specified by a direction: plus '+', minus: '-', or comma ',' and a number. Or by a repeated series of directions.

One more note, as a convenience the '??' option is available as a kind of shorthand for ',?'. That is, entering "cdd ??" is the same as entering "cdd ,?", in that the history of directories in most common order will be listed. It is shorthand in the sense that the question mark key can be simply pressed twice instead of typing a comma and then a question mark.

A caveat here is that '??' may be interpreted on a unix system as a wildcard patten matching a subdirectory in the current directory with just two letters. So on on unix typing "cdd ??" may result in changing to a new directory rather than listing the history. Likewise with "cdd ?". This may match a single lettered directory. On unix then it is safest to enter pattern and question mark formats: "cdd ,?" or "cdd +?" or "cdd -?".
