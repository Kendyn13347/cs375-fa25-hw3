CC = gcc

CFLAGS = -Wall -Wextra -std=gnu11 -g

SRCDIR = src
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.c)
PROGS = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SRCS))

all: $(PROGS)

$(BINDIR)/%: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR) build

.PHONY: all clean
