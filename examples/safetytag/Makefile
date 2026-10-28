CUSTOM_DIR := examples/$(PROJECT)
include $(TOP)/$(CUSTOM_DIR)/config

# Project-local linker script preserves INIT_*_EXPORT registration sections.
LD_FILE := $(TOP)/$(CUSTOM_DIR)/app.ld
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/inc
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/compat/luat/include
CFLAGS_INC += -I $(TOP)/$(CUSTOM_DIR)/compat/adapter/include

# Registration-bearing translation units must be direct link inputs, NOT archive-only.
# Add each new INIT_HW/DRV/TASK_EXPORT source here by its relative .o path.
obj-y += $(CUSTOM_DIR)/src/user_main.o
obj-y += $(CUSTOM_DIR)/src/safetytag_boot.o
obj-y += $(CUSTOM_DIR)/compat/luat/src/luat_init_export.o


# Non-registration implementation files can be archived separately.
CUSTOM_PORT_OBJS := $(BUILDDIR)/$(CUSTOM_DIR)/compat/luat/src/luat_rtos_port.o

libusr-y += lib_safetytag_compat.a
$(BUILDDIR)/lib/libusr/lib_safetytag_compat.a: $(CUSTOM_PORT_OBJS)
	@mkdir -p $(dir $@)
	$(ECHO) AR $@
	$(Q)$(AR) -cr $@ $^
