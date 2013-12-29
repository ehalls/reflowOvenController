#ifndef STDIO_OUTBUF_SIZE
#define STDIO_OUTBUF_SIZE	16
#endif

typedef struct _stdio_file FILE;

struct _stdio_file_ops {
        void *(* init)(void);
        size_t (* write)(const uint8_t *buf, size_t len, void *ops_data);
};

struct _stdio_file {
        const struct _stdio_file_ops *ops;
        void *ops_data;
        size_t outbuf_pos;
        uint8_t outbuf[STDIO_OUTBUF_SIZE];
};


extern FILE *stdout;

int printf(const char *fmt, ...)  __attribute__((__format__ (printf, 1, 2)));
int vfprintf(FILE *f, const char *fmt, va_list args);
void fputc(int c, FILE *f);
