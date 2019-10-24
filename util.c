#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int strcmp_ptr(const char *a, const char *b) {
        if (a && b)
                return strcmp(a, b);
        if (!a && b)
                return -1;
        if (a && !b)
                return 1;
        return 0;
}

bool streq(const char *a, const char *b) {
	return strcmp_ptr(a, b) == 0;
}

bool streq_ptr(const char *a, const char *b) {
	return streq(a, b);
}


static char ascii_tolower(char x) {
        if (x >= 'A' && x <= 'Z')
                return x - 'A' + 'a';
        return x;
}

char *ascii_strlower(char *s) {
        char *p;
        assert(s);
        for (p = s; *p; p++)
                *p = ascii_tolower(*p);
        return s;
}

size_t strpcpy(char **dest, size_t size, const char *src) {
        size_t len;

        assert(dest);
        assert(src);

        if (size == 0)
                return 0;

        len = strlen(src);
        if (len >= size) {
                if (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
        } else {
                if (len > 0) {
                        *dest = mempcpy(*dest, src, len);
                        size -= len;
                }
        }
        *dest[0] = '\0';
        return size;
}

size_t strpcpyf(char **dest, size_t size, const char *src, ...) {
        va_list va;
        int i;

        assert(dest);
        assert(src);

        if (size == 0)
                return 0;

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        if (i < (int)size) {
                *dest += i;
                size -= i;
        } else {
                size = 0;
        }
        va_end(va);
        return size;
}

size_t strpcpyl(char **dest, size_t size, const char *src, ...) {
        va_list va;

        assert(dest);
        assert(src);

        va_start(va, src);
        do {
                size = strpcpy(dest, size, src);
                src = va_arg(va, char *);
        } while (src != NULL);
        va_end(va);
        return size;
}

size_t strscpy(char *dest, size_t size, const char *src) {
        char *s;

        assert(dest);
        assert(src);

        s = dest;
        return strpcpy(&s, size, src);
}

size_t strscpyl(char *dest, size_t size, const char *src, ...) {
        va_list va;
        char *s;

        assert(dest);
        assert(src);

        va_start(va, src);
        s = dest;
        do {
                size = strpcpy(&s, size, src);
                src = va_arg(va, char *);
        } while (src != NULL);
        va_end(va);

        return size;
}

char *strjoin_real(const char *x, ...) {
        va_list ap;
        size_t l;
        char *r, *p;

        va_start(ap, x);

        if (x) {
                l = strlen(x);

                for (;;) {
                        const char *t;
                        size_t n;

                        t = va_arg(ap, const char *);
                        if (!t)
                                break;

                        n = strlen(t);
                        if (n > ((size_t) -1) - l) {
                                va_end(ap);
                                return NULL;
                        }

                        l += n;
                }
        } else
                l = 0;

        va_end(ap);

        r = new(char, l+1);
        if (!r)
                return NULL;

        if (x) {
                p = stpcpy(r, x);

                va_start(ap, x);

                for (;;) {
                        const char *t;

                        t = va_arg(ap, const char *);
                        if (!t)
                                break;

                        p = stpcpy(p, t);
                }

                va_end(ap);
        } else
                r[0] = 0;

        return r;
}


int read_one_line_file(const char *fn, char **line) {
	const int size = 1024;
	FILE *f;
	int r;

	f = fopen(fn, "r");
	if (f == NULL)
		return -errno;

	*line = malloc(size);
	if (!*line)
		return -ENOMEM;
	r = fread(*line, 1, size, f);
	assert(r > 0);
}

int safe_atou_full(const char *s, unsigned base, unsigned int *ret) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(ret);

        s += strspn(s, WHITESPACE);

        errno = 0;
        l = strtoul(s, &x, base);
        if (errno > 0)
                return -errno;
        if (!x || x == s || *x != 0)
                return -EINVAL;
        if (s[0] == '-')
                return -ERANGE;

        *ret = (unsigned int) l;
        return 0;
}

int udev_builtin_add_property(struct udev_device *dev, bool test, const char *key, const char *val) {
        printf("%s=%s\n", key, val);
}
