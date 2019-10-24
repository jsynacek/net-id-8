#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <libudev.h>

#define WHITESPACE        " \t\n\r"
#define DIGITS            "0123456789"
#define UPPERCASE_LETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define xsprintf sprintf
#define snprintf_ok(buf, len, fmt, ...) \
        ((size_t) snprintf(buf, len, fmt, __VA_ARGS__) < (len))
#define _cleanup_(f) __attribute__((cleanup(f)))
#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_closedir_ _cleanup_(closedirp)

static inline void closep(int *fd) {
        if (fd)
                close(*fd);
}

static inline void closedirp(DIR **d) {
        if (*d)
                closedir(*d);
}

static inline bool isempty(const char *p) {
        return !p || !p[0];
}

#define FOREACH_DIRENT_ALL(de, d, on_error)                             \
        for (errno = 0, de = readdir(d);; errno = 0, de = readdir(d))   \
                if (!de) {                                              \
                        if (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        break;                                          \
                } else

#define new(t, n) ((t*)malloc(sizeof(t)*(n)))

#define ELEMENTSOF(x)                                                    \
        __extension__ (__builtin_choose_expr(                            \
                !__builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                sizeof(x)/sizeof((x)[0]),                                \
                (void *)0))

#define strjoina(a, ...)                                                \
        ({                                                              \
                const char *_appendees_[] = { a, __VA_ARGS__ };         \
                char *_d_, *_p_;                                        \
                size_t _len_ = 0;                                          \
                size_t _i_;                                           \
                for (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = alloca(_len_ + 1);                          \
                for (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

static inline bool in_charset(const char *s, const char* charset) {
        assert(s);
        assert(charset);
        return s[strspn(s, charset)] == '\0';
}

static inline char *startswith(const char *s, const char *prefix) {
        size_t l;

        l = strlen(prefix);
        if (strncmp(s, prefix, l) == 0)
                return (char*) s + l;

        return NULL;
}

int strcmp_ptr(const char *a, const char *b);
bool streq(const char *a, const char *b);
bool streq_ptr(const char *a, const char *b);
char *ascii_strlower(char *s);
size_t strpcpy(char **dest, size_t size, const char *src);
size_t strpcpyf(char **dest, size_t size, const char *src, ...);
size_t strpcpyl(char **dest, size_t size, const char *src, ...);
size_t strscpy(char *dest, size_t size, const char *src);
size_t strscpyl(char *dest, size_t size, const char *src, ...);
char *strjoin_real(const char *x, ...);
#define strjoin(a, ...) strjoin_real((a), __VA_ARGS__, NULL)
int read_one_line_file(const char *fn, char **line);
int safe_atou_full(const char *s, unsigned base, unsigned int *ret);
int udev_builtin_add_property(struct udev_device *dev, bool test, const char *key, const char *val);
