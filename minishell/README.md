# Minishell

A simple shell implementation for the 42 school curriculum.

## Description

Minishell is a simplified shell that implements basic shell functionalities including command execution, pipes, redirections, environment variables, and built-in commands.

## Features

- Command execution with arguments
- Pipes (`|`)
- Redirections (`<`, `>`, `>>`)
- Environment variable expansion (`$VAR`)
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+\)
- Built-in commands:
  - `echo` with option `-n`
  - `cd` with relative or absolute path
  - `pwd` without options
  - `export` without options
  - `unset` without options
  - `env` without options or arguments
  - `exit` with exit status

## Architecture

The project is organized into several components:

1. **Lexer**: Tokenizes the input string into a list of tokens
2. **Parser**: Builds an Abstract Syntax Tree (AST) from tokens
3. **Executor**: Executes the AST by setting up pipes, redirections, and running commands
4. **Built-ins**: Implements the built-in commands
5. **Utils**: Contains utility functions for environment variables, signals, and error handling

## Installation

```bash
git clone <repository-url>
cd minishell
make
```

## Usage

```bash
./minishell
```

## Example Commands

```bash
# Simple command
ls -la

# Pipes
ls -l | grep .c

# Redirections
ls > output.txt
cat < input.txt
echo "hello" >> output.txt

# Environment variables
echo $HOME

# Built-in commands
cd ..
pwd
echo -n "Hello"
export VAR=value
env
exit 0
```

## Requirements

- GCC compiler
- GNU Make
- Readline library

## License

This project is licensed under the MIT License. 