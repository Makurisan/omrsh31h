RAW_ENABLE = yes
VIA_ENABLE = no
COMMAND_ENABLE = yes
CONSOLE_ENABLE = yes

EXTRAINCDIRS += $(QUANTUM_DIR)/msgpack/
SRC += $(QUANTUM_DIR)/msgpack/mpack-common.c \
       $(QUANTUM_DIR)/msgpack/mpack-expect.c \
       $(QUANTUM_DIR)/msgpack/mpack-node.c \
       $(QUANTUM_DIR)/msgpack/mpack-platform.c \
       $(QUANTUM_DIR)/msgpack/mpack-reader.c \
       $(QUANTUM_DIR)/msgpack/mpack-writer.c \
	   ./keymaps/msgpack.c

CFLAGS += -Wno-format
