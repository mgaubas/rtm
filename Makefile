.DEFAULT_GOAL     := evt

override CPPFLAGS += -MMD
override CPPFLAGS += -MP

override CFLAGS   += -c
override CFLAGS   += -std=c99
override CFLAGS   += -pedantic
override CFLAGS   += -pedantic-errors
override CFLAGS   += -Wall
override CFLAGS   += -Wextra
override CFLAGS   += -Werror
override CFLAGS   += -Wfatal-errors

override LDFLAGS  += -Wl,--build-id=0x$(shell git rev-parse HEAD)

override LDLIBS   +=

.PHONY: all
all: evt

.PHONY: clean
clean:
	@rm -f *.[do] *.so.* evt

evt: $(patsubst %.c,%.o,$(wildcard *.c))
	$(CC) $(LDFLAGS) -o $@ $(LDLIBS) $^

-include $(patsubst %.c,%.d,$(wildcard *.c))
