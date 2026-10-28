/*
 * 定义 luat_init_task 段的边界符号
 * 
 * GCC 链接器会为每个段自动生成 __start_SECTIONNAME 和 __stop_SECTIONNAME 符号，
 * 但这需要在链接脚本中正确配置。如果链接器没有自动生成，我们需要手动提供。
 * 
 * 这里我们使用一个技巧：定义两个实际的变量放在 luat_init_task 段的两端，
 * 然后导出它们的地址作为遍历的边界。
 */

#include "luat_init_export.h"
#include "lierda_app_main.h"

#include <stdio.h>
#include <string.h>

#include "liot_os.h"
#include "mem_map.h"
#include "stdlib.h"

/* 
 * 在 luat_init_task 段中放置两个标记
 * 编译器会按照它们在源文件中的顺序（以及其他使用该段的对象）将它们放入段中
 */

/* 开始标记 - 期望它位于段的开头 */
__attribute__((section("luat_init_task")))
__attribute__((used))
const luat_init_task_entry_t __luat_init_task_start_marker = {0, NULL};

/* 结束标记 - 期望它位于段的末尾 */
__attribute__((section("luat_init_task")))
__attribute__((used))
const luat_init_task_entry_t __luat_init_task_end_marker = {0, NULL};

/* 
 * 导出符号供 luat_init_export.c 使用
 * 注意：由于我们不能保证上面两个标记一定在段的两端，
 * 更好的方法是依赖链接器自动生成的符号。
 * 
 * 如果链接器已经生成了 __start_luat_init_task 和 __stop_luat_init_task，
 * 这个文件就不需要了。如果链接器没有生成，我们需要通过其他方式（如链接脚本）来定义。
 */