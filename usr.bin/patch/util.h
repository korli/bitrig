/*	$OpenBSD: util.h,v 1.9 2003/07/25 02:12:45 millert Exp $	*/

char		*fetchname(char *, int, int);
int		backup_file(char *);
int		move_file(char *, char *);
int		copy_file(char *, char *);
void		say(char *, ...);
void		fatal(char *, ...)
		    __attribute__((__format__(__printf__, 1, 2)));
void		pfatal(char *, ...)
		    __attribute__((__format__(__printf__, 1, 2)));
void		ask(char *, ...)
		    __attribute__((__format__(__printf__, 1, 2)));
char		*savestr(char *);
void		set_signals(int);
void		ignore_signals(void);
void		makedirs(char *, bool);
void		version(void);
void            my_exit(int) __attribute__((noreturn));
