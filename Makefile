CFLAGS = -Wall -Wextra -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -O2 -Wformat=2 -fstack-protector-all -fPIE -Wno-unused-result -Wno-return-type -Wno-missing-field-initializers -Wstrict-prototypes -Wunreachable-code  -Wwrite-strings -Wpointer-arith -Wbad-function-cast -Wcast-align -Wcast-qual
LDFLAGS = -Wl,-z,now -Wl,-z,relro -lcrypto -lcurl

CLIENT_SOURCES = src/client/*.c src/onexit.c
SERVER_SOURCES = src/server/*.c src/onexit.c

OUT_CLIENT = ftpclient
OUT_SERVER = ftpserver

all: $(OUT_CLIENT) $(OUT_SERVER)

$(OUT_CLIENT): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(CLIENT_SOURCES) -o $(OUT_CLIENT) $(LDFLAGS)

$(OUT_SERVER): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) $(SERVER_SOURCES) -o $(OUT_SERVER) $(LDFLAGS)
