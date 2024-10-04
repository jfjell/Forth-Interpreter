CC := clang
LD := clang
CFLAGS += -std=c99 \
	      -Wall \
	      -Wpedantic \
          -Wextra \
          -Werror \
          -pedantic-errors \
          -Wfatal-errors \
          -Os \
          -march=native -O0 -g \
          -D_CRT_SECURE_NO_WARNINGS
LDFLAGS  :=
CFILES   := $(wildcard src/*.c)
OBJFILES := $(patsubst src/%, obj/%.o, $(CFILES))
BIN := fgf.exe

.PHONY: all clean rebuild

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJFILES)

rebuild: clean all

$(BIN): $(OBJFILES) Makefile
	$(LD) -o $@ $(OBJFILES) $(LDFLAGS)

obj/%.o: src/% Makefile
	$(CC) -o $@ $< $(CFLAGS) -c

