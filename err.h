#ifndef ERR_H
#define ERR_H 1

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

static const char  __eror_form__[] =
	"eror|file: %s|func: %s|line: %u|text: %s\n";

static const char *__eror_text__ = NULL;
static unsigned    __eror_line__ = UINT_MAX;

static inline void print_error(const char *__eror_func__)
{
	dprintf(STDERR_FILENO, \
		__eror_form__, \
		__FILE__,      \
		__eror_func__, \
		__eror_line__, \
		__eror_text__);
}

#endif/*ERR_H*/
