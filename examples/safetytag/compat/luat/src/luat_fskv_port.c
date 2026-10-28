#include "luat_fskv.h"

#include <stdio.h>
#include <string.h>

#include "liot_fs_api.h"

/* All KV files are stored in the root FS directory with this prefix. */
#define FSKV_PREFIX      "fskv_"
#define FSKV_PREFIX_LEN  5
#define FSKV_NAME_MAX    128

/* Build the on-disk filename for a given key. */
static int fskv_make_path(const char *key, char *out, size_t out_len)
{
    int n;

    if (key == NULL || out == NULL)
    {
        return -1;
    }

    n = snprintf(out, out_len, "%s%s", FSKV_PREFIX, key);
    if (n < 0 || (size_t)n >= out_len)
    {
        return -1;
    }

    return 0;
}

int luat_fskv_init(void)
{
    /* Filesystem is initialised by the platform at boot – nothing to do. */
    return 0;
}

int luat_fskv_set(const char *key, const void *value, size_t len)
{
    char fname[FSKV_NAME_MAX];
    LFILE fd;
    int written;

    if (key == NULL || value == NULL || len == 0)
    {
        return -1;
    }

    if (fskv_make_path(key, fname, sizeof(fname)) != 0)
    {
        return -1;
    }

    fd = liot_fopen(fname, "wb");
    if (fd < (LFILE)LIOT_FS_OK)
    {
        return -1;
    }

    written = liot_fwrite((void *)value, 1, len, fd);
    liot_fclose(fd);

    return ((size_t)written == len) ? 0 : -1;
}

int luat_fskv_get(const char *key, void *buf, size_t len)
{
    char fname[FSKV_NAME_MAX];
    LFILE fd;
    int rd;

    if (key == NULL || buf == NULL || len == 0)
    {
        return -1;
    }

    if (fskv_make_path(key, fname, sizeof(fname)) != 0)
    {
        return -1;
    }

    if (liot_file_exist(fname) != 0)
    {
        return -1;
    }

    fd = liot_fopen(fname, "rb");
    if (fd < (LFILE)LIOT_FS_OK)
    {
        return -1;
    }

    rd = liot_fread(buf, 1, len, fd);
    liot_fclose(fd);

    return (rd > 0) ? rd : -1;
}

int luat_fskv_del(const char *key)
{
    char fname[FSKV_NAME_MAX];

    if (key == NULL)
    {
        return -1;
    }

    if (fskv_make_path(key, fname, sizeof(fname)) != 0)
    {
        return -1;
    }

    return (liot_remove(fname) == LIOT_FS_OK) ? 0 : -1;
}

int luat_fskv_clear(void)
{
    LDIR *dir;
    ldirent *ent;

    dir = liot_opendir("/");
    if (dir == NULL)
    {
        return -1;
    }

    while ((ent = liot_readdir(dir)) != NULL)
    {
        if (strncmp(ent->d_name, FSKV_PREFIX, FSKV_PREFIX_LEN) == 0)
        {
            liot_remove(ent->d_name);
        }
    }

    liot_closedir(dir);
    return 0;
}
