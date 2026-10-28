CUSTOM_DIR := examples/$(PROJECT)
include $(TOP)/$(CUSTOM_DIR)/config

# 工程专用链接脚本：保留 INIT_*_EXPORT 注册段。
LD_FILE := $(TOP)/$(CUSTOM_DIR)/app.ld
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/inc
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/compat/luat/include
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/compat/adapter/include

# -----------------------------------------------------------------------------
# 模块选择
#
# 每个模块的目录格式：
#   examples/safetytag/modules/<模块名>/src/*.c
#   examples/safetytag/modules/<模块名>/include/*.h  （可选）
#
# 在 config 中启用模块，或在命令行覆盖：
#   make gccall PROJECT=safetytag SAFETYTAG_MODULES="gnss sensor"
# 列表为空时，所有可选模块均不编译。启用模块的 src 目录下每个 .c 都会
# 自动编译，无须在此维护源文件或 .o 文件清单。
# -----------------------------------------------------------------------------
SAFETYTAG_MODULES ?=
SAFETYTAG_ENABLE_LUAT_COMPAT ?= 1
# 兼容层默认仅启用已具备依赖条件的 RTOS 适配。若启用其他适配文件，
# 还必须为其补齐对应的平台头文件、实现和链接库。
SAFETYTAG_LUAT_COMPAT_PORTS ?= luat_rtos_port


# src/ 是基础应用，始终启用。其源码及启用模块的源码均直接参与链接，
# 从而确保 INIT_*_EXPORT 注册段对链接器可见。
SAFETYTAG_DIRECT_SRC_DIRS := $(TOP)/$(CUSTOM_DIR)/src
SAFETYTAG_DIRECT_SRC_DIRS += $(foreach module,$(SAFETYTAG_MODULES),$(TOP)/$(CUSTOM_DIR)/modules/$(module)/src)
CFLAGS_INC += $(foreach module,$(SAFETYTAG_MODULES),-I $(TOP)/$(CUSTOM_DIR)/modules/$(module)/include)

APP_SRCS := $(foreach dir,$(SAFETYTAG_DIRECT_SRC_DIRS),$(wildcard $(dir)/*.c))
obj-y += $(patsubst $(TOP)/%.c,%.o,$(APP_SRCS))

# luat 兼容层可单独开关。其中注册源码必须直接参与链接，其他兼容层源码
# 自动归档到静态库。
ifeq ($(SAFETYTAG_ENABLE_LUAT_COMPAT),1)
LUAT_EXPORT_SRCS := $(TOP)/$(CUSTOM_DIR)/compat/luat/src/luat_init_export.c
obj-y += $(patsubst $(TOP)/%.c,%.o,$(LUAT_EXPORT_SRCS))

COMPAT_SRCS := $(addprefix $(TOP)/$(CUSTOM_DIR)/compat/luat/src/,$(addsuffix .c,$(SAFETYTAG_LUAT_COMPAT_PORTS)))
CUSTOM_PORT_OBJS := $(patsubst $(TOP)/%.c,$(BUILDDIR)/%.o,$(COMPAT_SRCS))

libusr-y += lib_safetytag_compat.a
$(BUILDDIR)/lib/libusr/lib_safetytag_compat.a: $(CUSTOM_PORT_OBJS)
	@mkdir -p $(dir $@)
	$(ECHO) AR $@
	$(Q)$(AR) -cr $@ $^
endif
