ENC_OPS := $(shell find .. -maxdepth 1 -name '*.c')
# TOOLS := $(shell find ../tools -name "*.c")
global-incdirs-y += ../ ../include ../../include
srcs-y += ops_ta.c crypto.c $(ENC_OPS) $(TOOLS) 
# libdirs += ../mbedtls/lib
# libnames += mbedcrypto
# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
# cflags-ops_ta.c-y += -pg
cflags-y += -DTEE_TZ
# link math
# libnames += timer stdc++
# libdirs += ../host 
libnames += mbedtls
