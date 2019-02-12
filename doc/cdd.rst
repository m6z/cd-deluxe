----------------------------------------------------------------------
Cd Deluxe
----------------------------------------------------------------------

Version 1.0.3

.. contents::

.. sectnum::

Overview
--------

Cd Deluxe is a drop-in replacement for the standard cd ("change directory") command.  It supports easier access to the history of directories visited.  It is kind of a "Swiss Army Knife" of changing directories.  It is designed to increase productivity by speeding up the workflow of command line use.  It is available for Windows and Unix style operating systems.

Following is a tutorial.  The cdd command behaves like the normal cd command but supports many extra features.

Basic usage
-----------

The most simplest use is to change to another directory.  In this way it is no different than the standard cd command:

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd Development**

    => D:\\Users\\Mike\\Development\\

In the examples above and following, the top line indicates the current directory, the line below it shows the cdd command that was issued, and the last line shows the resulting directory after cdd command.

When entered without any parameters it will by default list out this history of directories visited in reverse order (without changing the directory):

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\
    => **cdd**
     -1: D:\\Users\\Mike

    => D:\\Users\\Mike\\Development\\

As more directories are visited, the history of directories gets built up:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\
    => **cdd cd_deluxe**

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd**
     -1: D:\\Users\\Mike\\Development
     -2: D:\\Users\\Mike

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

The number listed before each directory in the history listing is a relative offset from the current directory.  To return to a previous directory a '-' and number can be entered:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd -2**
    cdd: D:\\Users\\Mike

    => D:\\Users\\Mike\\

This changes to the current directory to the second previous directory.  Note: whenever cdd changes to a directory with a behavior different to the standard cd command it prints a message indicating the action it has taken.  In the above example "cdd: D:\\Users\\Mike" is displayed as the directory is being changed.

To change to the previous directory a single '-' can be entered, or else '-1':

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd -**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

Of course the "cd -" command has been available in Unix operating systems forever but this has not been available on Windows.

As an alternative to specifying a '-' and number a sequence of dashes can be used:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd --**
    cdd: D:\\Users\\Mike\\Development

    => D:\\Users\\Mike\\Development\\

Entering "cdd --" is the same as entering "cdd -2".  This is sometimes more convenient that typing a '-' and a number.  Also it is generally quicker since after typing the first '-' it is not necessary to move to another location on the keyboard.  Every second counts!

Any '-' and number can be specified, or consecutive series of dashes.  For example "cdd -3" or "cdd ---" will go back to the third most recent directory.

The default action for when cdd is typed with no parameters is to show the history in the reverse order.  The default action can be changed (more on that later).  To specifically indicate that the reverse history is to be displayed the '-?' option can be specified:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\
    => **cdd -?**
     -1: D:\\Users\\Mike\\Development\\cd_deluxe
     -2: D:\\Users\\Mike

    => D:\\Users\\Mike\\Development\\

The question mark '?' when used in a parameter is an instruction to display the directory history.  The '-' before the question mark signifies the direction.  So the '-?' has the full meaning of: display the directory history from last visited to first visited.

As an alternative to the '-' direction, the '+' direction can be specified.  This displays the directory history from first visited to last visited:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\
    => **cdd +?**
      0: D:\\Users\\Mike
      1: D:\\Users\\Mike\\Development
      2: D:\\Users\\Mike\\Development\\cd_deluxe

    => D:\\Users\\Mike\\Development\\

This displays the directories from the zeroth visited onwards.  Note that this uses base 0 numbering so 0 signifies the first.  In the history listing the plus direction is inferred as opposed to the backwards direction where the '-' is indicated.

Similar to using the '-' and number command to move backwards through the directory history, a '+' and number can be used to move to a directory numbered from first to last visited:

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\
    => **cdd +0**
    cdd: D:\\Users\\Mike

    => D:\\Users\\Mike\\

Since this is the positive (forwards) direction, the +0 can simply be stated as 0.  So "cdd 0" is the same as "cdd +0".  Of course as before any number can be specified to jump to that directory.  And a consecutive series of pluses can be used.  For example to jump to the third visited directory:

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd +++**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

So '-' is used to specify a directory in backwards visited order, and '+' is used to specify in forwards visited order.  So that's it, right?  Not so fast!  There is also a way to specify s directory in most common visited order.  That direction is the comma: ','.  Comma sort of sounds like "common" so that makes it easy to remember.  So, to see the history of directories listed in most to least visited enter the comma direction and a question mark ",?":

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd ,?**
     ,0: ( 3) D:\\Users\\Mike\\Development\\cd_deluxe
     ,1: ( 3) D:\\Users\\Mike
     ,2: ( 2) D:\\Users\\Mike\\Development

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

Here the history list is similar but a little different.  Each directory has a precise specification of ',' and a number.  The most frequently visited directory is indicated as ',0'.  The second most commonly visited is ',1' and so on.  In case of multiple directories only being visited once the most recently visited directory has precedence in the list.  The number in parenthesis in the history listing is the number of times the directory has been visited.  So in the above example the top directory in the history has been visited 3 times.  A shortcut for getting back to the most visited directory is "cdd ,".

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd test**

    => D:\\Users\\Mike\\Development\\cd_deluxe\\test\\
    => **cdd ,**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

Commas can be strung together to change to the second most common directory, and so on.  Or a comma and a number can be specified.

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd ,,**
    cdd: D:\\Users\\Mike

    => D:\\Users\\Mike\\

So to review - there is the possibility to list out the directory history in three different ways or directions: forwards (first to last visited), backwards (last to first visited) and common (most to least visited).  And directories can be specified by a direction: plus '+', minus: '-', or comma ',' and a number.  Or by a repeated series of directions.

One more note, as a convenience the '??' option is available as a kind of shorthand for ',?'.  That is, entering "cdd ??" is the same as entering "cdd ,?", in that the history of directories in most common order will be listed.  It is shorthand in the sense that the question mark key can be simply pressed twice instead of typing a comma and then a question mark.

A caveat here is that '??' may be interpreted on a unix system as a wildcard patten matching a subdirectory in the current directory with just two letters.  So on on unix typing "cdd ??" may result in changing to a new directory rather than listing the history.  Likewise with "cdd ?".  This may match a single lettered directory.  On unix then it is safest to enter pattern and question mark formats: "cdd ,?" or "cdd +?" or "cdd -?".

Pattern Matching
----------------

Directories can also be specified by a pattern.  In this case a pattern is a series of alpha numeric characters that match any part of a name of a directory in the history.

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd lux**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe
     -2: D:\\Users\\Mike\\Development\\cd_deluxe\\test

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

In the above example the pattern "lux" matches two directories in the history.  The default direction is backward ('-') so the directory history is searched from last to first visited order.  The first directory that matches is used in the actual change directory operation.  Any other matching directories are displayed as well.  This is so that if the action taken by cdd is not the intended operation then it is easy to see what kind of command can be used to get to the desired directory.  In the above example if the second directory is the one that was intended, then the "cdd -2" can by issued.

The search direction can be specified when using pattern matching.   Here the forwards direction is specified.

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd + users**
    cdd: D:\\Users\\Mike
      1: D:\\Users\\Mike\\Development
      2: D:\\Users\\Mike\\Development\\cd_deluxe
      3: D:\\Users\\Mike\\Development\\cd_deluxe\\test

    => D:\\Users\\Mike\\

Note that pattern matching is done in a case insensitive manner.

Similarly the comma direction (most to least common) can be specified.

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd , lux**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe
     ,3: ( 1) D:\\Users\\Mike\\Development\\cd_deluxe\\test

    => D:\\Users\\Mike\\Development\\cd_deluxe\\

Pattern strings support regular expression syntax.  For example a '$' can be specified at the end of a pattern in order to precisely match a directory.

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\
    => **cdd mike$**
    cdd: D:\\Users\\Mike

    => D:\\Users\\Mike\\

Miscellaneous Features
----------------------

A series of dots can be used to move upwards in the current directory tree.  As per usual convention "cdd .." will change to the parent directory.  But it is also possible to change to the the "grandparent" directory with "cdd ...", or change to the "great grandparent" directory with "cdd ....", and so on.  (OS-9 anyone?)

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\test\\
    => **cdd ....**
    cdd: ..\\..\\..

    => D:\\Users\\Mike\\

If a name of an existing file is passed to cdd, then instead of complaining cdd will simple change to the directory of the file.  This is useful for times when editing lines in a unix command history or when pasting in filenames to a command window from the clipboard.

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd D:\\Users\\Mike\\Development\\cd_deluxe\\main\\main.cpp**
    cdd: D:\\Users\\Mike\\Development\\cd_deluxe\\main

    => D:\\Users\\Mike\\Development\\cd_deluxe\\main\\

Also on Windows cdd will gracefully handle directory names with forward slashes.

.. parsed-literal::

    => D:\\Users\\Mike\\Development\\cd_deluxe\\main\\
    => **cdd c:/tmp**
    cdd: c:\\tmp

    => C:\\tmp\\

A directory can be removed from the directory history with the --del option.  Of course the directory is not deleted from disk but just removed from the directory history.  The --del option requires a directory specification, which is any of the standard ways to specify a directory.  For example with an optional direction and a number, or an optional direction and a pattern.

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd**
     -1: C:\\tmp
     -2: D:\\Users\\Mike\\Development\\cd_deluxe\\test
     -3: D:\\Users\\Mike\\Development\\cd_deluxe
     -4: D:\\Users\\Mike\\Development

    => D:\\Users\\Mike\\
    => **cdd --del -1**
    cdd del: C:\\tmp

    => D:\\Users\\Mike\\
    => **cdd**
     -1: D:\\Users\\Mike\\Development\\cd_deluxe\\test
     -2: D:\\Users\\Mike\\Development\\cd_deluxe
     -3: D:\\Users\\Mike\\Development

The entire directory history can be erased with the --reset option.

.. parsed-literal::

    => D:\\Users\\Mike\\
    => **cdd --reset**
    cdd reset

    => D:\\Users\\Mike\\
    => **cdd**
    No history of other directories

    => D:\\Users\\Mike\\

There is another history management option available and that is --gc or "garbage collect".  Since cdd uses the standard "pushd" directory stack for storing directories, this can become quite deep over time.  Generally it is not a problem, even with visiting up to 1000 different directories.  The --gc command will remove all duplicate directories in the directory history thus reducing the size of the pushd directory stack.  Note that when --gc command is issued no directories are actually removed from the forwards '+', backwards '-' or most common ',' directory listings.  The most common (',') listing is affected in that the number of visits per directory is reset to just one.

Changing default behavior
-------------------------

Some of the behavior of cdd can be controlled by setting the CDD_OPTIONS environment variable.  For example, there are limits to the number of directories displayed in history listings.  The limit for the backwards direction is 8 directories displayed.  The reason for this is that as the history of directories grows long, simply listing the history becomes quite verbose.  These limits can be controlled with the --limit-backwards, --limit-forwards and --limit-common options.

.. parsed-literal::

    set CDD_OPTIONS=--limit-backward=10 --limit-common=5 --limit-forwards=0

In the above example there are limits set for each individual history listing.  A value of 0 indicates that there is no limit.

The default direction can be set in the CDD_OPTIONS environment variable.  By default the direction is backwards.  That is, if a direction is not specified in a cdd command, the direction is taken to be backwards.  For a history listing without a direction, the backwards history is displayed.  For pattern passed to cdd the history of directories is searched in the backwards direction.  But the default direction can be changed to forwards '+' or most common ','.

.. parsed-literal::

    set CDD_OPTIONS=--direction=,

The above example with change the default direction to "most common".  So when a cdd is entered with no parameters the directory history of most to least visited will be displayed instead of the backwards history.  Also when pattern matching is done the directories will be searched in most to least visited order instead of backwards order.

There is also the --action parameter that can be specified in CDD_OPTIONS.  The default action is to display the directory history according to the default (or overridden) direction.  But different a completely different default action can be specified.  For example:

.. parsed-literal::

    set CDD_OPTIONS=--action=0

The above example changes the behavior of cdd so that when it is typed without any parameters it will change to the zeroth directory.

.. parsed-literal::

    set CDD_OPTIONS=--action=,

The above example changes the behavior so that when "cdd" is typed on its own it will change to the most commonly visited directory.  These are just a few examples of the kinds of default custom actions that can be specified.  It is worth experimenting if a different default action is desired.

Installing
----------

Windows
==========

For Windows the easiest way to install is to `download the latest version of the installer`__.  The installer by default creates a director C:\\Program Files\\Cd Deluxe\\ and a desktop and start menu shortcut to an demonstration shell named "cdd shell".  However all that is really needed is just the _cdd.exe executable and cdd.cmd wrapper script.  These two files can simply be manually installed into any directory or copied to any other machines.

__ http://code.google.com/p/cd-deluxe/downloads/detail?name=CddInstaller_1.0.3_20110326.exe

Once installed by the installer or manually the default cd command can be replaced by the following alias:

.. parsed-literal::

    doskey cd="C:\\Program Files\\Cd Deluxe\\cdd.cmd" $*

Or if it has been installed in a different directory then use that directory in the doskey alias command instead of the default "C:\\Program Files\\Cd Deluxe\\".

Unix/Linux
==========

For Unix and Linux like systems such as Ubuntu cdd needs to be built through a few simple steps.  It needs to be checked out from subversion and then built.  It depends upon Scons__ for building and various Boost C++ libraries.  Here is an example:

__ http://www.scons.org/

.. parsed-literal::

    # Get the tools
    sudo apt-get install subversion
    sudo apt-get install scons
    sudo apt-get install g++
    sudo apt-get install libboost-regex-dev libboost-program-options-dev libboost-filesystem-dev libboost-test-dev

    # Get the source
    svn co http://cd-deluxe.googlecode.com/svn/trunk cd_deluxe

    # Build it
    cd cd_deluxe
    scons

This will create an executable named '_cdd'.  Look in the 'main' subdirectory.

To use this with the bash shell a cdd function is required.  This typically can be placed in ~/.bashrc

.. parsed-literal::

    function cdd { while read x; do eval $x >/dev/null; done < <(dirs -l -p | /usr/local/bin/_cdd "$@"); }

Then, to replace the default cd command add the following alias in ~/.bashrc or elsewhere:

.. parsed-literal::

    alias cd=cdd

(Note: any csh experts out there?  If so email me the steps necessary to use _cdd on csh and I will add to these notes)

Cygwin
==========

Installation in Cygwin is similar to the Unix/Linux steps above.  Instead of using "apt-get" the Cygwin Setup program is used to build up the tool environment.

Install the following Cygwin packages: subversion, gcc4-g++, libboost-devel and python.  The python interpreter is need by the Scons build system.  Once python has been installed these steps (or similar) can be followed to install Scons:

.. parsed-literal::

    wget http://peak.telecommunity.com/dist/ez_setup.py
    python ez_setup.py
    easy_install scons

Options Reference
-----------------

FREEFORM_OPTIONs are options that are specified in shorthand like "+?" or ",,,".  Long name options are specifically named options like "--history --direction=+ --all" or "--path=,,,".  A PATH_SPEC can be a number, a repeated direction, or a direction and a pattern.

.. csv-table:: Options
   :header: "Long Name Option", "Free Form Equivalent", "Description", "Can be specified in CDD_OPTIONS?"
   :class: options
   :widths: 25, 20, 47, 8

   "--history", "?", "Show directory history depending on the direction.  When using the free form '?, a number can be passed after the question mark which indicates the depth of the history.  A value of 0 indicates no limit on the depth.", "no"
   "--path=PATH_SPEC", "PATH_SPEC", "Change the current directory according to path specification (a number, a repeated direction, or a direction and a pattern).", "no"
   "--direction={-\|+\|,}", "{-\|+\|,}", "Specify direction (backwards, forwards, most common) for history or PATH_SPEC.", "Yes"
   "--limit-backwards=n", "-? n", "Show at most n directories for last to first history.  A value of zero indicates no limit.  Applies to history display only.", "Yes"
   "--limit-forwards=n", "+? n", "Show at most n directories for first to last history.  A value of zero indicates no limit.  Applies to history display only.", "Yes"
   "--limit-common=n", ",? n", "Show at most n directories for most to least visited directories.  A value of zero indicates no limit.  Applies to history display only.", "Yes"
   "--all", "{-\|+\|,}? 0", "Show all directories in the history (overriding any 'limit' options).", "Yes"
   "--action=FREEFORM_OPTION", "FREEFORM_OPTION", "Default freeform option to use when nothing else specified.  This is typically only used in the CDD_OPTIONS environment variable.", "Yes"
   "--gc", "", "Do garbage collection by minimizing directory stack.", "no"
   "--del=PATH_SPEC", "", "Remove from directory history the path matching PATH_SPEC.", "no"
   "--reset", "", "Reset the directory stack which clears all history.", "no"
   "--help", "", "Show help.", "no"
   "--version", "", "Show version.", "no"

----------------------------------------------------------------------

.. class:: trailer

Copyright (c) 2010-2019 `Michael Graz`__

__ mailto:mgraz.cdd@plan10.com

.. vim: spell
