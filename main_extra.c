#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

size_t  ft_strlen(const char *s);
char    *ft_strcpy(char *dst, const char *src);
int     ft_strcmp(const char *s1, const char *s2);
ssize_t ft_write(int fd, const void *buf, size_t count);
ssize_t ft_read(int fd, void *buf, size_t count);
char    *ft_strdup(const char *s);

static int g_total;
static int g_failed;
static int g_before;

#define CHECK(cond, ...)                                                   \
    do {                                                                   \
        g_total++;                                                         \
        if (!(cond)) {                                                     \
            g_failed++;                                                    \
            printf(RED "  FAIL" RESET " line %d: ", __LINE__);             \
            printf(__VA_ARGS__);                                           \
            printf("\n");                                                  \
        }                                                                  \
    } while (0)

#define BEGIN(name)                                                        \
    do {                                                                   \
        printf(GREEN "== %s" RESET "\n", name);                            \
        g_before = g_failed;                                               \
    } while (0)

#define END()                                                              \
    printf("   %s\n", g_failed == g_before ? GREEN "OK" RESET : RED "KO" RESET)

static int sgn(int x) { return (x > 0) - (x < 0); }

static const size_t g_lens[] = {
    0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 31, 32, 33, 63, 64, 65,
    127, 128, 129, 255, 256, 257, 1023, 1024, 4095, 4096, 4097,
    65535, 65536, 100000
};
#define NLENS (sizeof(g_lens) / sizeof(g_lens[0]))

/* fills with bytes 1..255 (never 0) */
static void fill_rand(unsigned char *p, size_t n)
{
    for (size_t i = 0; i < n; i++)
        p[i] = (unsigned char)(rand() % 255 + 1);
}

/* ---------------------------------------------------------------- strlen */
static void test_strlen(void)
{
    BEGIN("ft_strlen");

    const char *fixed[] = {"", "a", "ab", "hello", "hello world",
                           "\xff", "\xff\x80\x01", "tab\there\nnewline", NULL};
    for (int i = 0; fixed[i]; i++)
        CHECK(ft_strlen(fixed[i]) == strlen(fixed[i]), "fixed case %d", i);

    /* many lengths, many alignments; bytes after the terminator are non-zero
       so a function that reads past '\0' would return a bigger number */
    for (size_t k = 0; k < NLENS; k++)
        for (size_t off = 0; off < 16; off++)
        {
            size_t len = g_lens[k];
            unsigned char *buf = malloc(len + 40);
            fill_rand(buf, len + 40);
            unsigned char *s = buf + off;
            s[len] = 0;
            CHECK(ft_strlen((char *)s) == len,
                  "len=%zu off=%zu got %zu", len, off, ft_strlen((char *)s));
            free(buf);
        }
    END();
}

/* ---------------------------------------------------------------- strcpy */
static void test_strcpy(void)
{
    BEGIN("ft_strcpy");
    static const size_t offs[] = {0, 1, 3, 8};

    for (size_t k = 0; k < NLENS; k++)
        for (size_t o = 0; o < 4; o++)
        {
            size_t len = g_lens[k];
            size_t off = offs[o];
            size_t total = len + 1 + 64;
            unsigned char *src = malloc(len + 1);
            unsigned char *dst = malloc(total);
            fill_rand(src, len);
            src[len] = 0;
            memset(dst, 0xAA, total);

            char *ret = ft_strcpy((char *)dst + off, (char *)src);
            CHECK(ret == (char *)dst + off, "len=%zu: wrong return value", len);
            CHECK(memcmp(dst + off, src, len + 1) == 0,
                  "len=%zu off=%zu: content (or terminator) differs", len, off);

            int canary_ok = 1;
            for (size_t i = 0; i < off; i++)
                if (dst[i] != 0xAA)
                    canary_ok = 0;
            for (size_t i = off + len + 1; i < total; i++)
                if (dst[i] != 0xAA)
                    canary_ok = 0;
            CHECK(canary_ok, "len=%zu off=%zu: wrote outside the copied range",
                  len, off);
            free(src);
            free(dst);
        }
    END();
}

/* ---------------------------------------------------------------- strcmp */
static void test_strcmp(void)
{
    BEGIN("ft_strcmp");

    const char *a[] = {"", "", "a", "abc", "abc", "abd", "abc", "Hello", "Hello",
                       "Hell", "\xff", "a", "\x80", "\x7f", "same", NULL};
    const char *b[] = {"", "a", "", "abc", "abd", "abc", "abcd", "World", "Hell",
                       "Hello", "a", "\xff", "\x7f", "\x80", "same", NULL};
    for (int i = 0; a[i]; i++)
        CHECK(sgn(ft_strcmp(a[i], b[i])) == sgn(strcmp(a[i], b[i])),
              "table case %d: ft=%d libc=%d", i, ft_strcmp(a[i], b[i]),
              strcmp(a[i], b[i]));

    /* every pair of single bytes 1..255, then the same with a common prefix
       and a different last char (checks unsigned comparison for all bytes) */
    int bad = 0;
    for (int i = 1; i < 256; i++)
        for (int j = 1; j < 256; j++)
        {
            char x[3] = {(char)i, 0, 0};
            char y[3] = {(char)j, 0, 0};
            if (sgn(ft_strcmp(x, y)) != sgn(strcmp(x, y)))
                bad++;
            char px[4] = {'p', (char)i, 0, 0};
            char py[4] = {'p', (char)j, 0, 0};
            if (sgn(ft_strcmp(px, py)) != sgn(strcmp(px, py)))
                bad++;
        }
    CHECK(bad == 0, "%d mismatches in the all-bytes sweep", bad);

    /* identical strings at different addresses must give 0 */
    char c1[] = "identical string";
    char c2[] = "identical string";
    CHECK(ft_strcmp(c1, c2) == 0, "identical copies should give 0");
    CHECK(ft_strcmp(c1, c1) == 0, "same pointer should give 0");

    /* random fuzz over a small alphabet so prefixes are often equal */
    static const unsigned char alpha[] = {1, 2, 'a', 'b', 0x7f, 0x80, 0xff};
    int fuzz_bad = 0;
    for (int n = 0; n < 100000; n++)
    {
        unsigned char x[24], y[24];
        size_t lx = (size_t)(rand() % 20), ly = (size_t)(rand() % 20);
        for (size_t i = 0; i < lx; i++)
            x[i] = alpha[rand() % sizeof(alpha)];
        for (size_t i = 0; i < ly; i++)
            y[i] = alpha[rand() % sizeof(alpha)];
        x[lx] = 0;
        y[ly] = 0;
        if (sgn(ft_strcmp((char *)x, (char *)y)) != sgn(strcmp((char *)x, (char *)y)))
            fuzz_bad++;
    }
    CHECK(fuzz_bad == 0, "%d mismatches in 100000 random pairs", fuzz_bad);
    END();
}

/* ----------------------------------------------------------------- write */
static void test_write(void)
{
    BEGIN("ft_write");
    int p[2];
    char buf[64];

    /* success through a pipe, read back with libc; errno must stay untouched */
    CHECK(pipe(p) == 0, "pipe failed");
    errno = 1234;
    ssize_t r = ft_write(p[1], "hello", 5);
    CHECK(r == 5, "ret=%zd, expected 5", r);
    CHECK(errno == 1234, "errno changed on success (now %d)", errno);
    memset(buf, 0, sizeof(buf));
    CHECK(read(p[0], buf, sizeof(buf)) == 5 && memcmp(buf, "hello", 5) == 0,
          "data did not arrive");

    /* zero-length write */
    errno = 1234;
    r = ft_write(p[1], "x", 0);
    CHECK(r == 0, "count 0: ret=%zd", r);
    CHECK(errno == 1234, "errno changed on count 0");

    /* error cases: ft_write must match libc on both return value and errno */
    int dirfd = open(".", O_RDONLY);
    int rofd = open("/dev/null", O_RDONLY);
    int fds[] = {-1, 9999, dirfd, rofd};
    const char *names[] = {"fd -1", "fd 9999", "directory fd", "read-only fd"};
    for (int i = 0; i < 4; i++)
    {
        errno = 0;
        ssize_t r1 = write(fds[i], "x", 1);
        int e1 = errno;
        errno = 0;
        ssize_t r2 = ft_write(fds[i], "x", 1);
        int e2 = errno;
        CHECK(r1 == r2 && e1 == e2,
              "%s: libc ret=%zd errno=%d, ft ret=%zd errno=%d",
              names[i], r1, e1, r2, e2);
        CHECK(r2 == -1 && e2 > 0, "%s: expected -1 and errno set", names[i]);
    }
    close(dirfd);
    close(rofd);

    /* EPIPE: reader closed */
    signal(SIGPIPE, SIG_IGN);
    close(p[0]);
    errno = 0;
    ssize_t r1 = write(p[1], "x", 1);
    int e1 = errno;
    errno = 0;
    ssize_t r2 = ft_write(p[1], "x", 1);
    int e2 = errno;
    CHECK(r1 == -1 && r2 == -1 && e1 == EPIPE && e2 == EPIPE,
          "EPIPE: libc %zd/%d, ft %zd/%d", r1, e1, r2, e2);
    close(p[1]);

    /* ENOSPC on /dev/full (Linux only; skipped if missing) */
    int full = open("/dev/full", O_WRONLY);
    if (full >= 0)
    {
        errno = 0;
        r1 = write(full, "x", 1);
        e1 = errno;
        errno = 0;
        r2 = ft_write(full, "x", 1);
        e2 = errno;
        CHECK(r1 == r2 && e1 == e2 && e2 == ENOSPC,
              "/dev/full: libc %zd/%d, ft %zd/%d", r1, e1, r2, e2);
        close(full);
    }

    /* big write to a regular file, compare content */
    char path[] = "/tmp/libasm_write_XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0, "mkstemp failed");
    if (fd >= 0)
    {
        size_t big = 1 << 20;
        unsigned char *data = malloc(big);
        unsigned char *back = malloc(big);
        fill_rand(data, big);
        CHECK(ft_write(fd, data, big) == (ssize_t)big, "big write returned wrong count");
        lseek(fd, 0, SEEK_SET);
        CHECK(read(fd, back, big) == (ssize_t)big && memcmp(data, back, big) == 0,
              "big write: content differs");
        free(data);
        free(back);
        close(fd);
        unlink(path);
    }
    END();
}

/* ------------------------------------------------------------------ read */
static void test_read(void)
{
    BEGIN("ft_read");
    char path[] = "/tmp/libasm_read_XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0, "mkstemp failed");
    if (fd < 0)
    {
        END();
        return;
    }
    size_t size = 10000;
    unsigned char *data = malloc(size);
    unsigned char *back = malloc(size + 20016);
    fill_rand(data, size);
    CHECK(write(fd, data, size) == (ssize_t)size, "setup write failed");
    close(fd);

    /* read the whole file in chunks of different sizes */
    static const size_t chunks[] = {1, 7, 64, 4096, 20000};
    for (size_t c = 0; c < 5; c++)
    {
        fd = open(path, O_RDONLY);
        size_t got = 0;
        ssize_t r;
        errno = 1234;
        while (got < size && (r = ft_read(fd, back + got, chunks[c])) > 0)
            got += (size_t)r;
        CHECK(got == size && memcmp(data, back, size) == 0,
              "chunk %zu: got %zu bytes or content differs", chunks[c], got);
        CHECK(errno == 1234, "chunk %zu: errno changed on success", chunks[c]);
        errno = 1234;
        r = ft_read(fd, back, 16);
        CHECK(r == 0, "chunk %zu: EOF should return 0, got %zd", chunks[c], r);
        CHECK(errno == 1234, "chunk %zu: errno changed at EOF", chunks[c]);
        close(fd);
    }

    /* count 0 */
    fd = open(path, O_RDONLY);
    errno = 1234;
    CHECK(ft_read(fd, back, 0) == 0, "count 0 should return 0");
    CHECK(errno == 1234, "errno changed on count 0");
    close(fd);

    /* pipe: returns what is available, not necessarily count */
    int p[2];
    CHECK(pipe(p) == 0, "pipe failed");
    CHECK(write(p[1], "12345", 5) == 5, "pipe setup");
    memset(back, 0, 16);
    ssize_t r = ft_read(p[0], back, 100);
    CHECK(r == 5 && memcmp(back, "12345", 5) == 0, "pipe read: ret=%zd", r);
    close(p[0]);
    close(p[1]);

    /* /dev/null gives immediate EOF */
    int nullfd = open("/dev/null", O_RDONLY);
    CHECK(ft_read(nullfd, back, 16) == 0, "/dev/null should return 0");
    close(nullfd);

    /* error cases must match libc */
    int dirfd = open(".", O_RDONLY);
    int wofd = open(path, O_WRONLY);
    int fds[] = {-1, 9999, dirfd, wofd};
    const char *names[] = {"fd -1", "fd 9999", "directory fd", "write-only fd"};
    for (int i = 0; i < 4; i++)
    {
        errno = 0;
        ssize_t r1 = read(fds[i], back, 8);
        int e1 = errno;
        errno = 0;
        ssize_t r2 = ft_read(fds[i], back, 8);
        int e2 = errno;
        CHECK(r1 == r2 && e1 == e2,
              "%s: libc ret=%zd errno=%d, ft ret=%zd errno=%d",
              names[i], r1, e1, r2, e2);
        CHECK(r2 == -1 && e2 > 0, "%s: expected -1 and errno set", names[i]);
    }
    close(dirfd);
    close(wofd);
    free(data);
    free(back);
    unlink(path);
    END();
}

/* ----------------------------------------------------------------- strdup */
static size_t vm_bytes(void)
{
    FILE *f = fopen("/proc/self/statm", "r");
    unsigned long pages = 0;
    if (!f)
        return 0;
    if (fscanf(f, "%lu", &pages) != 1)
        pages = 0;
    fclose(f);
    return pages * (size_t)sysconf(_SC_PAGESIZE);
}

static void test_strdup(int skip_oom)
{
    BEGIN("ft_strdup");

    for (size_t k = 0; k < NLENS; k++)
    {
        size_t len = g_lens[k];
        unsigned char *src = malloc(len + 1);
        fill_rand(src, len);
        src[len] = 0;
        char *d = ft_strdup((char *)src);
        CHECK(d != NULL, "len=%zu: returned NULL", len);
        if (d)
        {
            CHECK(d != (char *)src, "len=%zu: returned the same pointer", len);
            CHECK(memcmp(d, src, len + 1) == 0, "len=%zu: content differs", len);
            memset(d, 'Z', len + 1);               /* whole block must be writable */
            CHECK(src[0] == 0 || src[0] != 'Z' || len == 0,
                  "len=%zu: copy shares memory with the source", len);
            free(d);
        }
        free(src);
    }

    /* out of memory: malloc fails, ft_strdup must return NULL and set ENOMEM */
    if (skip_oom)
        printf("   (out-of-memory case skipped)\n");
    else
    {
        size_t big = (size_t)64 << 20;
        char *src = malloc(big + 1);
        size_t vm = vm_bytes();
        if (!src || !vm)
            printf("   (out-of-memory case skipped: setup failed)\n");
        else
        {
            memset(src, 'x', big);
            src[big] = 0;
            vm = vm_bytes();
            struct rlimit old, lim;
            getrlimit(RLIMIT_AS, &old);
            lim = old;
            lim.rlim_cur = vm + ((size_t)4 << 20);
            if (setrlimit(RLIMIT_AS, &lim) != 0)
                printf("   (out-of-memory case skipped: setrlimit failed)\n");
            else
            {
                errno = 0;
                char *d = ft_strdup(src);
                int e = errno;
                setrlimit(RLIMIT_AS, &old);
                CHECK(d == NULL, "expected NULL when malloc fails");
                CHECK(e == ENOMEM, "expected errno ENOMEM, got %d", e);
                free(d);
            }
            free(src);
        }
    }
    END();
}

int main(int argc, char **argv)
{
    int skip_oom = argc > 1 && strcmp(argv[1], "--skip-oom") == 0;

    signal(SIGPIPE, SIG_IGN);
    srand(42);

    test_strlen();
    test_strcpy();
    test_strcmp();
    test_write();
    test_read();
    test_strdup(skip_oom);

    printf("\n%d checks, %d failed: %s\n", g_total, g_failed,
           g_failed ? RED "FAILURES" RESET : GREEN "ALL PASSED" RESET);
    return g_failed != 0;
}