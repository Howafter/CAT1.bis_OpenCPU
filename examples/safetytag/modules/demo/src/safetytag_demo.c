#include "safetytag_demo.h"

/*
 * 普通模块源文件示例：启用 demo 模块后，本文件会被自动编译和直接链接。
 * 此文件不含 INIT_*_EXPORT；需要注册初始化回调时，可在模块源文件中使用
 * 对应的 INIT_*_EXPORT 宏，Makefile 无须额外修改。
 */
uint32_t safetytag_demo_version(void)
{
    return 1U;
}
