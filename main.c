#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#define _PR(_fp, ...)                         \
    do {                                      \
        fprintf(_fp, "[srsly] " __VA_ARGS__); \
        fputc('\n', _fp);                     \
    } while (0)

#define PR(...) _PR(stdout, __VA_ARGS__)
#define ER(...) _PR(stderr, __VA_ARGS__)

static int flags;
static long long n_scanned_dirs, n_loops;
#define FLAG_VERBOSE     1
#define FLAG_IGNORE_PERM 2

static void usage(const char *execname)
{
    printf("Usage: %s dir [-v] [-q]\n"
           "  -v: output all scanned directories\n"
           "  -q: do not report permission errors\n"
           "  -h: This help message\n",
           execname);
    exit(EXIT_SUCCESS);
}

/* Print n_spaces of ' ' characters */
static char *pspaces(int n_spaces)
{
    static char spaces[64];
    const int max = n_spaces < 64 ? n_spaces : 63;
    memset(spaces, ' ', max);
    spaces[max + 1] = '\0';
    return spaces;
}

/* Recursively scan a directory (not following symlinks) and locate recursive
 * symlinks (i.e., those that returned ELOOP error when called with stat()
 */
static void scan_dir(const char *dname, int depth)
{
    size_t len;
    char path[PATH_MAX];
    DIR *dir;
    struct dirent *dent;
    struct stat st;

    if (flags & FLAG_VERBOSE)
      printf("%s%s ", pspaces(depth * 2), dname);

    if (!(dir = opendir(dname)))
    {
        if (!(errno == EACCES && (flags & FLAG_IGNORE_PERM)))
          ER("Could not open dir: %s [%s]", dname, strerror(errno));
        return;
    }
    
    ++n_scanned_dirs;

    /* Is this directory a symlink? Probably not unless main passes us a
     * symlink directory on start.
     */   
    stat(dname, &st);
    if (flags & FLAG_VERBOSE)
      printf("%s\n", st.st_mode & S_IFLNK ? " [link]" : "");

    /* For each directory entry */
    while ((dent = readdir(dir)))
    {
        /* Avoid scanning symlinks and anything not a directory */
        if ((dent->d_type == DT_LNK))
        {
            snprintf(path, sizeof(path), "%s/%s", dname, dent->d_name);
            if (stat(path, &st) && errno == ELOOP)
            {
                PR("symlink loop detected %s", path);
                ++n_loops;
            }
            continue;
        }
        else if (dent->d_type != DT_DIR)
          continue;

        /* Ignore '.' and '..' directories */
        len = strlen(dent->d_name);
        if ( ((len==1) && dent->d_name[0]=='.') ||
           ( ((len==2) && dent->d_name[0]=='.' && dent->d_name[1]=='.')) )
          continue;

        /* Build the dname + dent path name and scan it */
        if (dname[strlen(dname)-1] != '/')
          snprintf(path, sizeof(path), "%s/%s", dname, dent->d_name);
        else
          snprintf(path, sizeof(path), "%s%s", dname, dent->d_name);

        scan_dir(path, depth + 1);
    }

    closedir(dir);
}

int main(int argc, char **argv)
{
    int i;
    const char *dname = NULL;

    for (i=1; i<argc; ++i)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'v')
          flags |= FLAG_VERBOSE;
        else if (argv[i][0] == '-' && argv[i][1] == 'q')
          flags |= FLAG_IGNORE_PERM;
        else if (argv[i][0] == '-')
          usage(argv[0]);
        else
          dname = argv[i];
    }

    if (!dname)
      usage(argv[0]);

    scan_dir(dname, 0);
    printf("== Scanned %lld directories and found %lld recursive symlinks ==",
           n_scanned_dirs, n_loops);

    return 0;
}
