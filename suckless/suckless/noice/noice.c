/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <locale.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

#ifdef DEBUG
#define DEBUG_FD 8
#define DPRINTF_D(x) dprintf(DEBUG_FD, #x "=%d\n", x)
#define DPRINTF_U(x) dprintf(DEBUG_FD, #x "=%u\n", x)
#define DPRINTF_S(x) dprintf(DEBUG_FD, #x "=%s\n", x)
#define DPRINTF_P(x) dprintf(DEBUG_FD, #x "=0x%p\n", x)
#else
#define DPRINTF_D(x)
#define DPRINTF_U(x)
#define DPRINTF_S(x)
#define DPRINTF_P(x)
#endif /* DEBUG */

#define NR_ARGS	32
#define LEN(x) (sizeof(x) / sizeof(*(x)))
#undef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define ISODD(x) ((x) & 1)
#define CONTROL(c) ((c) ^ 0x40)
#define META(c) ((c) ^ 0x80)

struct assoc {
	char *regex; /* Regex to match on filename */
	char *file;
	char *argv[NR_ARGS];
	regex_t regcomp;
};

struct cpair {
	int fg;
	int bg;
};

/* Supported actions */
enum action {
	SEL_QUIT = 1,
	SEL_BACK,
	SEL_GOIN,
	SEL_FLTR,
	SEL_NEXT,
	SEL_PREV,
	SEL_PGDN,
	SEL_PGUP,
	SEL_HOME,
	SEL_END,
	SEL_CD,
	SEL_CDHOME,
	SEL_TOGGLEDOT,
	SEL_DSORT,
	SEL_MTIME,
	SEL_ICASE,
	SEL_REDRAW,
	SEL_RUN,
	SEL_RUNARG,
};

struct key {
	int sym;         /* Key pressed */
	enum action act; /* Action */
	char *run;       /* Program to run */
	char *env;       /* Environment variable to run */
};

#include "config.h"

struct entry {
	char name[PATH_MAX];
	mode_t mode;
	time_t t;
};

/* Global context */
struct entry *dents;
int ndents, cur;
int idle;

/*
 * Layout:
 * .---------
 * | cwd: /mnt/path
 * |
 * |    file0
 * |    file1
 * |  > file2
 * |    file3
 * |    file4
 *      ...
 * |    filen
 * |
 * | Permission denied
 * '------
 */

void printmsg(char *);
void printwarn(void);
void printerr(int, char *);

#undef dprintf
int
dprintf(int fd, const char *fmt, ...)
{
	char buf[BUFSIZ];
	int r;
	va_list ap;

	va_start(ap, fmt);
	r = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (r > 0)
		write(fd, buf, r);
	va_end(ap);
	return r;
}

void *
xmalloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (p == NULL)
		printerr(1, "malloc");
	return p;
}

void *
xrealloc(void *p, size_t size)
{
	p = realloc(p, size);
	if (p == NULL)
		printerr(1, "realloc");
	return p;
}

char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (p == NULL)
		printerr(1, "strdup");
	return p;
}

/* Some implementations of dirname(3) may modify `path' and some
 * return a pointer inside `path'. */
char *
xdirname(const char *path)
{
	static char out[PATH_MAX];
	char tmp[PATH_MAX], *p;

	strlcpy(tmp, path, sizeof(tmp));
	p = dirname(tmp);
	if (p == NULL)
		printerr(1, "dirname");
	strlcpy(out, p, sizeof(out));
	return out;
}

void
spawnvp(char *dir, char *file, char *argv[])
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (dir != NULL)
			chdir(dir);
		execvp(file, argv);
		_exit(1);
	} else {
		/* Ignore interruptions */
		while (waitpid(pid, &status, 0) == -1)
			DPRINTF_D(status);
		DPRINTF_D(pid);
	}
}

void
spawnlp(char *dir, char *file, char *argv0, ...)
{
	char *argv[NR_ARGS];
	va_list ap;
	int argc;

	va_start(ap, argv0);
	argv[0] = argv0;
	for (argc = 1; argv[argc] = va_arg(ap, char *); argc++)
		;
	argv[argc] = NULL;
	va_end(ap);
	spawnvp(dir, file, argv);
}

void
spawnassoc(struct assoc *assoc, char *arg)
{
	char *argv[NR_ARGS];
	int i;

	for (i = 0; assoc->argv[i]; i++) {
		if (strcmp(assoc->argv[i], "{}") == 0) {
			argv[i] = arg;
			continue;
		}
		argv[i] = assoc->argv[i];
	}
	argv[i] = NULL;
	spawnvp(NULL, assoc->file, argv);
}

char *
xgetenv(char *name, char *fallback)
{
	char *value;

	if (name == NULL)
		return fallback;
	value = getenv(name);
	return value && value[0] ? value : fallback;
}

struct assoc *
openwith(char *file)
{
	int i;

	for (i = 0; i < LEN(assocs); i++) {
		if (regexec(&assocs[i].regcomp, file, 0, NULL, 0) == 0) {
			DPRINTF_S(assocs[i].argv[0]);
			return &assocs[i];
		}
	}

	return NULL;
}

int
setfilter(regex_t *regex, char *filter)
{
	char errbuf[LINE_MAX];
	size_t len;
	int r;

	r = regcomp(regex, filter, REG_NOSUB | REG_EXTENDED | REG_ICASE);
	if (r != 0) {
		len = COLS;
		if (len > sizeof(errbuf))
			len = sizeof(errbuf);
		regerror(r, regex, errbuf, len);
		printmsg(errbuf);
	}
	return r;
}

void
freefilter(regex_t *regex)
{
	regfree(regex);
}

void
initfilter(int dot, char **ifilter)
{
	*ifilter = dot ? "." : "^[^.]";
}

int
visible(regex_t *regex, char *file)
{
	return regexec(regex, file, 0, NULL, 0) == 0;
}

int
dircmp(mode_t a, mode_t b)
{
	if (S_ISDIR(a) && S_ISDIR(b))
		return 0;
	if (!S_ISDIR(a) && !S_ISDIR(b))
		return 0;
	if (S_ISDIR(a))
		return -1;
	else
		return 1;
}

int
entrycmp(const void *va, const void *vb)
{
	const struct entry *a = va, *b = vb;

	if (dirorder) {
		if (dircmp(a->mode, b->mode) != 0)
			return dircmp(a->mode, b->mode);
	}

	if (mtimeorder)
		return b->t - a->t;
	if (icaseorder)
		return strcasecmp(a->name, b->name);
	else
		return strcmp(a->name, b->name);
}

void
initcolor(void)
{
	int i;

	start_color();
	use_default_colors();
	for (i = 1; i < LEN(pairs); i++)
		init_pair(i, pairs[i].fg, pairs[i].bg);
}

void
initcurses(void)
{
	char *term;

	if (initscr() == NULL) {
		term = getenv("TERM");
		if (term != NULL)
			fprintf(stderr, "error opening terminal: %s\n", term);
		else
			fprintf(stderr, "failed to initialize curses\n");
		exit(1);
	}
	if (usecolor && has_colors())
		initcolor();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(FALSE); /* Hide cursor */
	timeout(1000); /* One second */
}

void
exitcurses(void)
{
	endwin(); /* Restore terminal */
}

/* Messages show up at the bottom */
void
printmsg(char *msg)
{
	move(LINES - 1, 0);
	printw("%s\n", msg);
}

/* Display warning as a message */
void
printwarn(void)
{
	printmsg(strerror(errno));
}

/* Kill curses and display error before exiting */
void
printerr(int ret, char *prefix)
{
	exitcurses();
	fprintf(stderr, "%s: %s\n", prefix, strerror(errno));
	exit(ret);
}

/* Clear the last line */
void
clearprompt(void)
{
	printmsg("");
}

/* Print prompt on the last line */
void
printprompt(char *str)
{
	clearprompt();
	printw(str);
}

int
xgetch(void)
{
	int c;

	c = getch();
	if (c == -1)
		idle++;
	else
		idle = 0;
	return c;
}

/* Returns SEL_* if key is bound and 0 otherwise.
 * Also modifies the run and env pointers (used on SEL_{RUN,RUNARG}) */
int
nextsel(char **run, char **env)
{
	int c, i;

	c = xgetch();
	if (c == 033)
		c = META(xgetch());

	for (i = 0; i < LEN(bindings); i++)
		if (c == bindings[i].sym) {
			*run = bindings[i].run;
			*env = bindings[i].env;
			return bindings[i].act;
		}
	return 0;
}

char *
readln(void)
{
	static char ln[LINE_MAX];

	timeout(-1);
	echo();
	curs_set(TRUE);
	memset(ln, 0, sizeof(ln));
	wgetnstr(stdscr, ln, sizeof(ln) - 1);
	noecho();
	curs_set(FALSE);
	timeout(1000);
	return ln[0] ? ln : NULL;
}

int
canopendir(char *path)
{
	DIR *dirp;

	dirp = opendir(path);
	if (dirp == NULL)
		return 0;
	closedir(dirp);
	return 1;
}

char *
mkpath(char *dir, char *name, char *out, size_t n)
{
	/* Handle absolute path */
	if (name[0] == '/') {
		strlcpy(out, name, n);
	} else {
		/* Handle root case */
		if (strcmp(dir, "/") == 0) {
			strlcpy(out, "/", n);
			strlcat(out, name, n);
		} else {
			strlcpy(out, dir, n);
			strlcat(out, "/", n);
			strlcat(out, name, n);
		}
	}
	return out;
}

void
printent(struct entry *ent, int active)
{
	char name[PATH_MAX];
	unsigned int len = COLS - strlen(CURSR) - 1;
	char cm = 0;
	int attr = 0;

	/* Copy name locally */
	strlcpy(name, ent->name, sizeof(name));

	/* No text wrapping in entries */
	if (strlen(name) < len)
		len = strlen(name) + 1;

	if (S_ISDIR(ent->mode)) {
		cm = '\0';
		attr |= DIR_ATTR;
	} else if (S_ISLNK(ent->mode)) {
		cm = '@';
		attr |= LINK_ATTR;
	} else if (S_ISSOCK(ent->mode)) {
		cm = '=';
		attr |= SOCK_ATTR;
	} else if (S_ISFIFO(ent->mode)) {
		cm = '|';
		attr |= FIFO_ATTR;
	} else if (ent->mode & S_IXUSR) {
		cm = '*';
		attr |= EXEC_ATTR;
	}

	if (active)
		attr |= CURSR_ATTR;

	if (cm) {
		name[len - 1] = cm;
		name[len] = '\0';
	}

	attron(attr);
	printw("%s%s\n", active ? CURSR : EMPTY, name);
	attroff(attr);
}

int
dentfill(char *path, struct entry **dents,
	 int (*filter)(regex_t *, char *), regex_t *re)
{
	char newpath[PATH_MAX];
	DIR *dirp;
	struct dirent *dp;
	struct stat sb;
	int r, n = 0;

	dirp = opendir(path);
	if (dirp == NULL)
		return 0;

	while ((dp = readdir(dirp)) != NULL) {
		/* Skip self and parent */
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		if (filter(re, dp->d_name) == 0)
			continue;
		*dents = xrealloc(*dents, (n + 1) * sizeof(**dents));
		strlcpy((*dents)[n].name, dp->d_name, sizeof((*dents)[n].name));
		/* Get mode flags */
		mkpath(path, dp->d_name, newpath, sizeof(newpath));
		r = lstat(newpath, &sb);
		if (r == -1)
			printerr(1, "lstat");
		(*dents)[n].mode = sb.st_mode;
		(*dents)[n].t = sb.st_mtime;
		n++;
	}

	/* Should never be null */
	r = closedir(dirp);
	if (r == -1)
		printerr(1, "closedir");
	return n;
}

void
dentfree(struct entry *dents)
{
	free(dents);
}

/* Return the position of the matching entry or 0 otherwise */
int
dentfind(struct entry *dents, int n, char *cwd, char *path)
{
	char tmp[PATH_MAX];
	int i;

	if (path == NULL)
		return 0;
	for (i = 0; i < n; i++) {
		mkpath(cwd, dents[i].name, tmp, sizeof(tmp));
		DPRINTF_S(path);
		DPRINTF_S(tmp);
		if (strcmp(tmp, path) == 0)
			return i;
	}
	return 0;
}

int
populate(char *path, char *oldpath, char *fltr)
{
	regex_t re;
	int r;

	/* Can fail when permissions change while browsing */
	if (canopendir(path) == 0)
		return -1;

	/* Search filter */
	r = setfilter(&re, fltr);
	if (r != 0)
		return -1;

	dentfree(dents);

	ndents = 0;
	dents = NULL;

	ndents = dentfill(path, &dents, visible, &re);
	freefilter(&re);
	if (ndents == 0)
		return 0; /* Empty result */

	qsort(dents, ndents, sizeof(*dents), entrycmp);

	/* Find cur from history */
	cur = dentfind(dents, ndents, path, oldpath);
	return 0;
}

void
redraw(char *path)
{
	char cwd[PATH_MAX], cwdresolved[PATH_MAX];
	size_t ncols;
	int nlines, odd;
	int i;

	nlines = MIN(LINES - 4, ndents);

	/* Clean screen */
	erase();

	/* Strip trailing slashes */
	for (i = strlen(path) - 1; i > 0; i--)
		if (path[i] == '/')
			path[i] = '\0';
		else
			break;

	DPRINTF_D(cur);
	DPRINTF_S(path);

	/* No text wrapping in cwd line */
	ncols = COLS;
	if (ncols > PATH_MAX)
		ncols = PATH_MAX;
	strlcpy(cwd, path, ncols);
	cwd[ncols - strlen(CWD) - 1] = '\0';
	realpath(cwd, cwdresolved);

	printw(CWD "%s\n\n", cwdresolved);

	/* Print listing */
	odd = ISODD(nlines);
	if (cur < nlines / 2) {
		for (i = 0; i < nlines; i++)
			printent(&dents[i], i == cur);
	} else if (cur >= ndents - nlines / 2) {
		for (i = ndents - nlines; i < ndents; i++)
			printent(&dents[i], i == cur);
	} else {
		for (i = cur - nlines / 2;
		     i < cur + nlines / 2 + odd; i++)
			printent(&dents[i], i == cur);
	}
}

void
browse(char *ipath, char *ifilter)
{
	char path[PATH_MAX], oldpath[PATH_MAX], newpath[PATH_MAX];
	char fltr[LINE_MAX];
	char *dir, *tmp, *run, *env;
	struct assoc *assoc;
	struct stat sb;
	regex_t re;
	int r, fd;

	strlcpy(path, ipath, sizeof(path));
	strlcpy(fltr, ifilter, sizeof(fltr));
	oldpath[0] = '\0';
begin:
	r = populate(path, oldpath, fltr);
	if (r == -1) {
		printwarn();
		goto nochange;
	}

	for (;;) {
		redraw(path);
nochange:
		switch (nextsel(&run, &env)) {
		case SEL_QUIT:
			dentfree(dents);
			return;
		case SEL_BACK:
			/* There is no going back */
			if (strcmp(path, "/") == 0 ||
			    strcmp(path, ".") == 0 ||
			    strchr(path, '/') == NULL)
				goto nochange;
			dir = xdirname(path);
			if (canopendir(dir) == 0) {
				printwarn();
				goto nochange;
			}
			/* Save history */
			strlcpy(oldpath, path, sizeof(oldpath));
			strlcpy(path, dir, sizeof(path));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr));
			goto begin;
		case SEL_GOIN:
			/* Cannot descend in empty directories */
			if (ndents == 0)
				goto nochange;

			mkpath(path, dents[cur].name, newpath, sizeof(newpath));
			DPRINTF_S(newpath);

			/* Get path info */
			fd = open(newpath, O_RDONLY | O_NONBLOCK);
			if (fd == -1) {
				printwarn();
				goto nochange;
			}
			r = fstat(fd, &sb);
			if (r == -1) {
				printwarn();
				close(fd);
				goto nochange;
			}
			close(fd);
			DPRINTF_U(sb.st_mode);

			switch (sb.st_mode & S_IFMT) {
			case S_IFDIR:
				if (canopendir(newpath) == 0) {
					printwarn();
					goto nochange;
				}
				strlcpy(path, newpath, sizeof(path));
				/* Reset filter */
				strlcpy(fltr, ifilter, sizeof(fltr));
				goto begin;
			case S_IFREG:
				assoc = openwith(newpath);
				if (assoc == NULL) {
					printmsg("No association");
					goto nochange;
				}
				exitcurses();
				spawnassoc(assoc, newpath);
				initcurses();
				continue;
			default:
				printmsg("Unsupported file");
				goto nochange;
			}
		case SEL_FLTR:
			/* Read filter */
			printprompt("filter: ");
			tmp = readln();
			if (tmp == NULL)
				tmp = ifilter;
			/* Check and report regex errors */
			r = setfilter(&re, tmp);
			if (r != 0)
				goto nochange;
			freefilter(&re);
			strlcpy(fltr, tmp, sizeof(fltr));
			DPRINTF_S(fltr);
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			goto begin;
		case SEL_NEXT:
			if (cur < ndents - 1)
				cur++;
			break;
		case SEL_PREV:
			if (cur > 0)
				cur--;
			break;
		case SEL_PGDN:
			if (cur < ndents - 1)
				cur += MIN((LINES - 4) / 2, ndents - 1 - cur);
			break;
		case SEL_PGUP:
			if (cur > 0)
				cur -= MIN((LINES - 4) / 2, cur);
			break;
		case SEL_HOME:
			cur = 0;
			break;
		case SEL_END:
			cur = ndents - 1;
			break;
		case SEL_CD:
			/* Read target dir */
			printprompt("chdir: ");
			tmp = readln();
			if (tmp == NULL) {
				clearprompt();
				goto nochange;
			}
			mkpath(path, tmp, newpath, sizeof(newpath));
			if (canopendir(newpath) == 0) {
				printwarn();
				goto nochange;
			}
			strlcpy(path, newpath, sizeof(path));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr));
			DPRINTF_S(path);
			goto begin;
		case SEL_CDHOME:
			tmp = getenv("HOME");
			if (tmp == NULL) {
				clearprompt();
				goto nochange;
			}
			if (canopendir(tmp) == 0) {
				printwarn();
				goto nochange;
			}
			strlcpy(path, tmp, sizeof(path));
			/* Reset filter */
			strlcpy(fltr, ifilter, sizeof(fltr));
			DPRINTF_S(path);
			goto begin;
		case SEL_TOGGLEDOT:
			showhidden ^= 1;
			initfilter(showhidden, &ifilter);
			strlcpy(fltr, ifilter, sizeof(fltr));
			goto begin;
		case SEL_MTIME:
			mtimeorder = !mtimeorder;
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			goto begin;
		case SEL_DSORT:
			dirorder = !dirorder;
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			goto begin;
		case SEL_ICASE:
			icaseorder = !icaseorder;
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			goto begin;
		case SEL_REDRAW:
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			goto begin;
		case SEL_RUN:
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			run = xgetenv(env, run);
			exitcurses();
			spawnlp(path, run, run, NULL);
			initcurses();
			goto begin;
		case SEL_RUNARG:
			/* Save current */
			if (ndents > 0)
				mkpath(path, dents[cur].name, oldpath, sizeof(oldpath));
			run = xgetenv(env, run);
			exitcurses();
			spawnlp(path, run, run, dents[cur].name, NULL);
			initcurses();
			goto begin;
		}
		/* Screensaver */
		if (idletimeout != 0 && idle == idletimeout) {
			idle = 0;
			exitcurses();
			spawnlp(NULL, idlecmd, idlecmd, NULL);
			initcurses();
		}
	}
}

void
initassocs(void)
{
	char errbuf[256];
	int i, r;

	for (i = 0; i < LEN(assocs); i++) {
		r = regcomp(&assocs[i].regcomp, assocs[i].regex,
			    REG_NOSUB | REG_EXTENDED | REG_ICASE);
		if (r != 0) {
			regerror(r, &assocs[i].regcomp, errbuf, sizeof(errbuf));
			fprintf(stderr, "invalid regex assocs[%d]: %s: %s\n",
			        i, assocs[i].regex, errbuf);
			exit(1);
		}
	}
}

void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [dir]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	char cwd[PATH_MAX], *ipath;
	char *ifilter;

	if (argc > 2)
		usage(argv[0]);

	/* Confirm we are in a terminal */
	if (!isatty(0) || !isatty(1)) {
		fprintf(stderr, "stdin or stdout is not a tty\n");
		exit(1);
	}

	if (getuid() == 0)
		showhidden = 1;
	initfilter(showhidden, &ifilter);

	if (argv[1] != NULL) {
		ipath = argv[1];
	} else {
		ipath = getcwd(cwd, sizeof(cwd));
		if (ipath == NULL)
			ipath = "/";
	}

	signal(SIGINT, SIG_IGN);

	/* Test initial path */
	if (canopendir(ipath) == 0) {
		fprintf(stderr, "%s: %s\n", ipath, strerror(errno));
		exit(1);
	}

	/* Set locale before curses setup */
	setlocale(LC_ALL, "");
	initassocs();
	initcurses();
	browse(ipath, ifilter);
	exitcurses();
	exit(0);
}
