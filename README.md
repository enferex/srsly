## srsly: SRSLY Recursive Symbolic Link Yak: Locates recursive symbolic links

### What
srsly is a tool to locate recursive symbolic links.  srsly recursively scans
all non-symbolic links in a specified directory and analyzes each file that is
a symbolic link.  If that link is recursive, then the file name is reported to
standard out.


### Why
Because I wanted to!  Sure, you could probably obtain the same functionality
from `cleanlinks` or `ls`, but this tool is primarily for locating recursive
symlinks.  Perhaps adding graphviz/dot output in the future, or the ability to
compute graph problems (if the ability to safely follow symlinks is added).
Right now the tool does not follow symlinks and would probably produce directed
graphs with no loops (e.g., boring).

### Build
Run `make`

### Run
The only required argument to srsly is a directory to start scanning from.

Options:
   * -v: Output all directories scanned (this can be super annoying)
   * -h: Help
   * -q: Quiet mode, do not report any permission errors when trying to scan a
   directory that you do not have access to.

### Contact
Matt Davis (enferex)
mattdavis9@gmail.com
