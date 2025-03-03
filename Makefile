# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Target executable
TARGET = MiniShell

# Source files
SRCS = main.c env_utils.c

INCLUDES = mini_shell.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS)

# Clean up build files and executable
fclean: clean
	rm -f $(TARGET)

# Rebuild the project
re: fclean all

# Phony targets
.PHONY: all clean fclean re