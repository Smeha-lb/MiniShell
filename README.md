# MiniShell

A custom shell implementation written in C that mimics core functionality of bash. This project implements a fully functional command-line interpreter with support for pipes, redirections, variable expansion, wildcards, and built-in commands.

## Features

### Core Functionality
- **Interactive shell** with readline support and command history
- **Command execution** for external programs and built-in commands
- **Environment variable management** with automatic inheritance and manipulation

### Built-in Commands
- `echo` - Display text with options support
- `cd` - Change directory (supports `~` and `-` for previous directory)
- `pwd` - Print working directory
- `export` - Export environment variables
- `unset` - Unset environment variables
- `env` - Display environment variables
- `exit` - Exit the shell with optional exit code

### Advanced Features
- **Pipes (`|`)** - Connect multiple commands together
- **Redirections:**
  - Input redirection (`<`)
  - Output redirection (`>`)
  - Append output (`>>`)
  - Heredoc (`<<`)
- **Logical Operators:**
  - `&&` - Execute next command only if previous succeeds
  - `||` - Execute next command only if previous fails
- **Subshells** - Support for parentheses `(command)` to create subshells
- **Variable Expansion** - `$VAR` and `$?` for exit status
- **Wildcard Expansion** - Pattern matching with `*` for filenames
- **Quote Handling:**
  - Single quotes (`'`) - Literal text, no expansion
  - Double quotes (`"`) - Allow variable expansion
- **Signal Handling** - Proper handling of Ctrl+C and Ctrl+\

## Requirements

- **Compiler:** `cc` (or compatible C compiler)
- **OS:** Unix-like system (Linux, macOS)
- **Libraries:**
  - `readline` library (for command line editing)
  - Standard C library

### Installing readline

**On Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libreadline-dev
```

**On macOS (with Homebrew):**
```bash
brew install readline
```

**On Fedora/RHEL:**
```bash
sudo dnf install readline-devel
```

## Building

1. Clone the repository:
```bash
git clone <repository-url>
cd MiniShell
```

2. Build the project:
```bash
make
```

This will:
- Build the `libft` library
- Compile all source files
- Link the executable `minishell`

3. Clean build artifacts:
```bash
make clean      # Remove object files
make fclean     # Remove object files and executable
make re         # Rebuild from scratch
```

## Usage

Run the shell:
```bash
./minishell
```

The shell will start with a prompt:
```
minishell> 
```

Type commands and press Enter to execute them. Type `exit` to quit.

### Examples

**Basic commands:**
```bash
minishell> ls -la
minishell> pwd
minishell> echo "Hello, World!"
```

**Environment variables:**
```bash
minishell> export MY_VAR="Hello"
minishell> echo $MY_VAR
minishell> unset MY_VAR
```

**Pipes:**
```bash
minishell> ls -la | grep ".c" | wc -l
```

**Redirections:**
```bash
minishell> echo "Hello" > file.txt
minishell> cat < file.txt
minishell> echo "Append" >> file.txt
minishell> cat << EOF
> This is a heredoc
> EOF
```

**Logical operators:**
```bash
minishell> cd /nonexistent && echo "Success"
minishell> cd /nonexistent || echo "Failed"
```

**Subshells:**
```bash
minishell> (cd /tmp && pwd)
```

**Wildcards:**
```bash
minishell> ls *.c
minishell> cat srcs/*.c | grep "function"
```

## Project Structure

```
MiniShell/
├── includes/
│   └── minishell.h          # Main header file with all declarations
├── libft/                    # Custom C library (libft)
│   ├── *.c                  # Library source files
│   ├── libft.h              # Library header
│   └── Makefile             # Library build file
├── srcs/                     # Main source files
│   ├── main.c               # Entry point
│   ├── shell.c              # Shell initialization and loop
│   ├── signals.c            # Signal handling
│   ├── lexer*.c             # Lexical analysis (tokenization)
│   ├── parser*.c            # Parsing (AST construction)
│   ├── execution*.c         # Command execution
│   ├── builtins*.c          # Built-in command implementations
│   ├── redirections*.c      # Redirection handling
│   ├── expansion*.c         # Variable expansion
│   ├── wildcard*.c          # Wildcard expansion
│   └── utils*.c             # Utility functions
├── Makefile                 # Main build file
└── README.md               # This file
```

## Architecture

The shell follows a traditional interpreter architecture:

1. **Lexer** - Tokenizes input into tokens (words, operators, redirections)
2. **Parser** - Builds an abstract syntax tree (AST) from tokens
3. **Expansion** - Expands variables, wildcards, and removes quotes
4. **Executor** - Executes commands, handles pipes, redirections, and built-ins

### Key Data Structures

- `t_token` - Represents a lexical token
- `t_command` - Represents a command with arguments and redirections
- `t_shell` - Main shell state (environment, commands, exit status)
- `t_redir` - Redirection information

## Error Handling

The shell provides meaningful error messages for:
- Syntax errors (unexpected tokens, unclosed quotes)
- Command not found
- Permission denied
- File not found
- Invalid environment variable names
- Invalid exit codes

## Limitations

- No job control (`&` for background processes, `fg`, `bg`, `jobs`)
- No brace expansion `{a,b,c}`
- No command substitution `` `command` `` or `$(command)`
- No aliases
- No function definitions
- Limited signal handling compared to full-featured shells

## Code Standards

- Follows 42 school coding standards (Norminette)
- Uses `-Wall -Wextra -Werror` compilation flags
- Proper memory management with no leaks
- Handles edge cases and error conditions

## Testing

Test the shell with various commands and edge cases:

```bash
# Test pipes
./minishell
minishell> ls | grep "test"

# Test redirections
minishell> echo "test" > file && cat file

# Test variable expansion
minishell> export TEST=value && echo $TEST

# Test wildcards
minishell> ls *.c
```

## Troubleshooting

**Build fails with "readline not found":**
- Install the readline development package for your system (see Requirements)

**Commands not working:**
- Ensure external commands are in your `PATH` environment variable
- Check that the command exists and is executable

**Memory leaks:**
- Use `valgrind` to check for leaks:
```bash
valgrind --leak-check=full ./minishell
```

## License

This project is part of the 42 school curriculum and follows their academic guidelines.

## Author

Developed as part of the 42 school curriculum project.
