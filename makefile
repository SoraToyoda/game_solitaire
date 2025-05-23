CC = gcc
CFLAGS = -Wall -lncursesw
TARGET = solitaire

SRCDIR = .
SRCS = $(SRCDIR)/main.c $(SRCDIR)/ui.c $(SRCDIR)/deck.c $(SRCDIR)/game.c $(SRCDIR)/input.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
