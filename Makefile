.DEFAULT_GOAL  := evt

CPPFLAGS       := -MMD
CPPFLAGS       += -MP

CFLAGS         := -fpic
CFLAGS         += -pedantic
CFLAGS         += -pedantic-errors
CFLAGS         += -std=c99
CFLAGS         += -Wall
CFLAGS         += -Wextra
CFLAGS         += -Werror
CFLAGS         += -Wfatal-errors

LDFLAGS        := -Wl,--build-id=0x$(shell git rev-parse HEAD)

LDLIBS         :=

.PHONY: all
all: evt

.PHONY: clean
clean:
	@rm -f *.[do] *.so.*

evt: $(patsubst %.c,%.o,$(wildcard *.c))
	$(CC) $(LDFLAGS) -o $@ $(LDLIBS) $^

-include $(patsubst %.c,%.d,$(wildcard *.c))
