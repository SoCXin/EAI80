/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          stdout_console.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief        
 *
 * @note
 *    2018/03/24, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#include <stdio.h>
#include <misc/libc-hooks.h>
#include <syscall_handler.h>
#include <string.h>

static int _stdout_hook_default(int c)
{
	(void)(c);  /* Prevent warning about unused argument */

	return EOF;
}

static int (*_stdout_hook)(int) = _stdout_hook_default;

void __stdout_hook_install(int (*hook)(int))
{
	_stdout_hook = hook;
}

int _impl__ugelis_fputc(int c, FILE *stream)
{
	return (stdout == stream) ? _stdout_hook(c) : EOF;
}

#ifdef CONFIG_USERSPACE
Z_SYSCALL_HANDLER(_ugelis_fputc, c, stream)
{
	return _impl__ugelis_fputc(c, (FILE *)stream);
}
#endif

int fputc(int c, FILE *stream)
{
	return _ugelis_fputc(c, stream);
}

int fputs(const char *_MLIBC_RESTRICT string, FILE *_MLIBC_RESTRICT stream)
{
	int len = strlen(string);
	int ret;

	ret = fwrite(string, len, 1, stream);

	return len == ret ? 0 : EOF;
}

size_t _impl__ugelis_fwrite(const void *_MLIBC_RESTRICT ptr, size_t size,
			    size_t nitems, FILE *_MLIBC_RESTRICT stream)
{
	size_t i;
	size_t j;
	const unsigned char *p;

	if ((stream != stdout) || (nitems == 0) || (size == 0)) {
		return 0;
	}

	p = ptr;
	i = nitems;
	do {
		j = size;
		do {
			if (_stdout_hook((int) *p++) == EOF)
				goto done;
			j--;
		} while (j > 0);

		i--;
	} while (i > 0);

done:
	return (nitems - i);
}

#ifdef CONFIG_USERSPACE
Z_SYSCALL_HANDLER(_ugelis_fwrite, ptr, size, nitems, stream)
{

	Z_OOPS(Z_SYSCALL_MEMORY_ARRAY_READ(ptr, nitems, size));
	return _impl__ugelis_fwrite((const void *_MLIBC_RESTRICT)ptr, size,
				    nitems, (FILE *_MLIBC_RESTRICT)stream);
}
#endif

size_t fwrite(const void *_MLIBC_RESTRICT ptr, size_t size, size_t nitems,
			  FILE *_MLIBC_RESTRICT stream)
{
	return _ugelis_fwrite(ptr, size, nitems, stream);
}


int puts(const char *string)
{
	if (fputs(string, stdout) == EOF) {
		return EOF;
	}

	return fputc('\n', stdout) == EOF ? EOF : 0;
}
