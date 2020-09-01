#ifndef ERR_H
#define ERR_H 1

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

static const char  eror_look[] =
	"eror|file: %s|func: %s|line: %u|text: %s\n";

static const char *eror_text = NULL;
static unsigned    eror_line = UINT_MAX;

static inline void print_error(const char *__eror_func__)
{
	dprintf(STDERR_FILENO, \
		eror_look, \
		__FILE__,      \
		__eror_func__, \
		eror_line, \
		eror_text);
}

#endif/*ERR_H*/
