RAW_ENABLE = yes
VIA_ENABLE = no
COMMAND_ENABLE = yes
CONSOLE_ENABLE = yes

EXTRAINCDIRS += $(QUANTUM_DIR)/msgpack/
SRC += ./keymaps/msgpack.c

CFLAGS += -Wno-format
