#ifndef __L_FSKV_PORT_H__
#define __L_FSKV_PORT_H__

#include "lierda_api.h"

/*
r或rb: 以只读文件打开，文件不存在则失败。
r+或rb+: 以读写文件打开，文件不存在则失败。
w或wb: 以只写文件打开，文件存在则清空，不存在则创建。
w+或wb+: 以读写方式打开文件，文件存在则清空，不存在则创建。
a或ab: 以只写追加方式打开文件，文件存在则在末尾追加，不存在则创建。
a+/ab+:以读写追加方式打开文件，文件存在则在末尾追加，不存在则创建
*/

#define FSKV_FILE_R     "r"
#define FSKV_FILE_RW    "r+"
#define FSKV_FILE_W     "w"
#define FSKV_FILE_WR    "w+"
#define FSKV_FILE_AW    "a"
#define FSKV_FILE_ARW   "a+"


int fskv_set(const char* key, void* data, size_t len);
int fskv_get(const char* key, void* data, size_t len);

#endif // __L_FSKV_PORT_H__