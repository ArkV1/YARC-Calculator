CC = gcc
CFLAGS = -Wall -Wextra -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lraylib -lm

TARGET = calculator
SRC = calculator.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) 