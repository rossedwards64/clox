CC 	    := gcc
BIN     ?= clox

LDLIBS  :=
CFLAGS  := -std=c17 -Wall -Wpedantic -Wextra -Wno-unused-parameter -g3 -pedantic

SRCDIR  ?= src
SRCS    := $(wildcard $(SRCDIR)/*.c)

OBJDIR  ?= build
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS) $(INCLUDE)

.phony: clean

clean:
	rm -rf $(OBJDIR) clox
