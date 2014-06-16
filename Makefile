CFLAGS = -Wall -Wextra -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -O2 -Wformat=2 -fstack-protector-all -fPIE -Wstrict-prototypes -Wunreachable-code  -Wwrite-strings -Wpointer-arith -Wbad-function-cast -Wcast-align -Wcast-qual
LDFLAGS = -Wl,-z,now -Wl,-z,relro

CLIENT_SOURCES = src/*.c

OUT_CLIENT = ftpclient

all: $(OUT_CLIENT)

$(OUT_CLIENT): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(CLIENT_SOURCES) -o $(OUT_CLIENT) $(LDFLAGS)
