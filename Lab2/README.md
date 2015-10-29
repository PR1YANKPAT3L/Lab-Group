<help>
Version: 1.0
Operating System MyShell Project

The most commonly used MyShell commands are:
  about      Displays information about MyShell
  cd         Change the current default directory to <directory> 
  clear      Clear the screen 
  environ    List all environment strings
  echo       Display <comment> on the display followed by a new line
  pause      Pause operation until <ENTER> is pressed
  quit       Quit MyShell
  dir        List contents of directory <directory>

Entire User Manual can be read by entering <help more> command.
#

<help about>
MyShell is a command line intepreter, which is the fundamental user interface to an Operating System.
#

<help version>
Version: 1.0
#

<help commands>
MyShell supports the following commands:
    cd      clr       environ     echo    help
    pause   myshell   quit        dir     quit 
Type "help <command>" to get the details for each command.
Example: help about - Display the information specific to the shell
#

<help cd>
[Usage]: cd <directory>
[Description]: Change the current default directory to
<directory>. If the <directory> argument is not present, report
the current directory. If the directory does not exist an appropriate
error should be reported. This command should also change the PWD
environment variable.
#

<help clr>
[Usage]: clear
[Description]: Clear the screen
#

<help dir>
[Usage]: dir <directory>
[Description]:  List the contents of directory <directory>.
#

<help environ>
[Usage]: environ
[Description]:  List all the environment strings
#

<help echo>
[Usage]: echo <comment>
[Description]:   Display <comment> on the display followed by a
new line (multiple spaces/tabs may be reduced to a single space).
#

<help pause>
[Usage]: pause
[Description]:  Pause operation of the shell until <ENTER> is pressed
#

<help quit>
[Usage]: quit
[Description]:  Quit the shell
#