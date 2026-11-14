// SPDX-License-Identifier: MIT
struct device_table {
    char* dt_string;
    int dt_type;
    int dt_bsize;
    char* dt_desc;
    int (*dt_init)();
    int (*dt_open)();
    int (*dt_strategy)();
    int (*dt_close)();
    int (*dt_ioctl)();
    int (*dt_read)();
    int (*dt_write)();
    int (*dt_delete)();
    int (*dt_undelete)();
    int (*dt_firstfile)();
    int (*dt_nextfile)();
    int (*dt_format)();
    int (*dt_cd)();
    int (*dt_rename)();
    int (*dt_remove)();
    int (*dt_else)();
};

struct DIRENTRY;

#define DEVTAB (*(struct device_table**)0x150)
#define DEVTAB_SIZE (*(unsigned int*)0x154)

int strcmp(char*, char*);
struct DIRENTRY* firstfile2(char*, struct DIRENTRY*);

static int (*sfunc)();
static char dev[32];

static int _firstfile(int* fd, int a1, int a2);

static int save_firstfile(char* name) {
    struct device_table* d;
    struct device_table* tbl = DEVTAB;
    int n = DEVTAB_SIZE / sizeof(struct device_table);

    for (d = tbl; d < tbl + n; d++) {
        if (d->dt_string != 0 && strcmp(d->dt_string, name) == 0) {
            sfunc = d->dt_firstfile;
            return 1;
        }
    }
    return 0;
}

static void set_firstfile(char* name, int (*func)()) {
    struct device_table* d;
    struct device_table* tbl = DEVTAB;
    int n = DEVTAB_SIZE / sizeof(struct device_table);

    for (d = tbl; d < tbl + n; d++) {
        if (d->dt_string != 0 && strcmp(d->dt_string, name) == 0) {
            d->dt_firstfile = func;
            return;
        }
    }
}

struct DIRENTRY* firstfile(char* name, struct DIRENTRY* dir) {
    char* s;
    char* p;

    s = name;
    p = dev;
    while (*s >= ';') {
        *p++ = *s++;
    }
    *p = 0;

    if (!save_firstfile(dev)) {
        return 0;
    }
    set_firstfile(dev, _firstfile);
    return firstfile2(name, dir);
}

static int _firstfile(int* fd, int a1, int a2) {
    if (*fd == 0) {
        *fd = 1;
    }
    set_firstfile(dev, sfunc);
    return sfunc(fd, a1, a2);
}
