#include "l_fskv_port.h"

#ifdef debug
#undef debug
#define debug(...)
#endif

/******************************************************************************************************
 * Function: fskv_set
 *
 * Description:
 *   设置键值对数据。key如果存在就会先删除再创建，key不存在则直接创建。每次写入都是覆盖式的，无法追加。
 *
 * Parameters:
 *
 * Return:
 *   LIOT_DATACALL_SUCCESS   成功
 *   other                   错误码
 *
 ******************************************************************************************************/
int fskv_set(const char* key, void* data, size_t len) 
{
    int ret = -1;
    LFILE fd = -1;

    if (key == NULL || data == NULL || len == 0){
        return ret;
    }
    
    ret = liot_file_exist(key);
    debug("=== before create, check file exist. ret(%d) ===", ret);
    if (ret == 0)
    {
        debug("=== remove file. ret(%d) ===", liot_remove(key));
        debug("=== remove file, check file exist. ret(%d) ===", liot_file_exist(key));
    }

    debug("=== creat file ===\r\n");
    fd = liot_fopen(key, FSKV_FILE_WR);
    if (fd < LIOT_FS_OK)
    {
        debug("=== create file fail. fd(%d) ===", fd);
        return fd;
    }

    debug("=== write data to file ====");
    ret = liot_fwrite(data, 1, len, fd);
    if (ret != len)
    {
        debug("=== write data fail. ret(%d) ===", ret);
    }

    debug("=== close file ===");
    ret = liot_fclose(fd);
    if (ret != LIOT_FS_OK)
    {
        debug("=== close file fail. ret(%d) ===", ret);
    }

    return ret;
}

int fskv_get(const char* key, void* data, size_t len) 
{
    int ret = -1;
    LFILE fd = -1;
    int len_r = 0;

    if (key == NULL || data == NULL || len == 0){
        return ret;
    }

    debug("=== open file ===");
    fd = liot_fopen(key, FSKV_FILE_R);
    if (fd < LIOT_FS_OK)
    {
        debug("=== open file fail. fd(%d) ===", fd);
        return fd;
    }

    len_r = liot_fsize(fd);
    debug("=== file size, ret(%d) ===", len_r);

    if (len_r > len)
    {
        debug("=== buffer too small. file size(%d) > buffer size(%d) ===", len_r, len);
        liot_fclose(fd);
        return -1;
    }   

    liot_fseek(fd, 0, LIOT_SEEK_SET);// 从文件开头读取

    debug("=== read data from file ====");
    ret = liot_fread(data, len_r, 1, fd);
    if (ret != len_r)
    {
        debug("=== read data fail. ret(%d) ===", ret);
    }
    else
    {
        debug(" read data success ");
    }

    debug("=== close file ===");
    ret = liot_fclose(fd);
    if (ret != LIOT_FS_OK)
    {
        debug("=== close file fail. ret(%d) ===", ret);
    }

    return ret;
}

int fskv_del(const char* key) 
{
    int ret = -1;

    if (key == NULL){
        return ret;
    }

    ret = liot_file_exist(key);
    liot_trace("=== before create, check file exist. ret(%d) ===", ret);
    if (ret == 0)
    {
        liot_trace("=== remove file. ret(%d) ===", liot_remove(key));
        liot_trace("=== remove file, check file exist. ret(%d) ===", liot_file_exist(key));
    }

    return ret;
}

// int luat_fskv_clear(void) {
//     int ret = 0;
//     ret = lfs_format(&fskv_lfs->lfs, &fskv_lfs->conf);
//     if (ret != LFS_ERR_OK) {
//         DBG("fskv clear ret %d", ret);
//         return ret;
//     }
//     ret = lfs_mount(&fskv_lfs->lfs, &fskv_lfs->conf);
//     if (ret != LFS_ERR_OK) {
//         DBG("fskv reinit ret %d", ret);
//         return ret;
//     }
//     return 0;
// }

// int luat_fskv_stat(size_t *using_sz, size_t *total, size_t *kv_count) {
//     *using_sz = lfs_fs_size(&fskv_lfs->lfs) * LFS_BLOCK_DEVICE_ERASE_SIZE;
//     *total = fskv_lfs->total_len;
//     lfs_dir_t dir = {0};
//     int ret = lfs_dir_open(&fskv_lfs->lfs, &dir, "");
//     if (ret != LFS_ERR_OK) {
//         DBG("lfs_dir_open ret %d", ret);
//         return -1;
//     }
//     size_t count = 0;
//     struct lfs_info info = {0};
//     while (1) {
//         ret = lfs_dir_read(&fskv_lfs->lfs, &dir, &info);
//         if (ret > 0) {
//             if (info.type == LFS_TYPE_REG)
//                 count ++;
//         }
//         else
//             break;
//     }
//     lfs_dir_close(&fskv_lfs->lfs, &dir);
//     *kv_count = count;
//     return 0;
// }

