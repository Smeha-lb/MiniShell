# Minishell

A simple shell implementation with basic functionality similar to bash.

## Features

- Command line prompt display
- Command history
- Execute commands (with PATH variable or absolute paths)
- Signal handling (ctrl-C, ctrl-D, ctrl-\)
- Quote handling (single and double quotes)
- Redirections:
  - `<` input redirection
  - `>` output redirection
  - `<<` heredoc
  - `>>` append mode
- Pipes (`|`)
- Environment variable expansion ($VAR)
- $? for last exit status
- Builtin commands:
  - echo (with -n option)
  - cd (with relative/absolute path)
  - pwd
  - export
  - unset
  - env
  - exit

## Building and Running

To build the project:

```bash
make
```

To run the shell:

```bash
./minishell
```

## Implementation

The shell is implemented with several components:

1. **Lexer**: Tokenizes the input command line
2. **Parser**: Creates command structures from tokens
3. **Executor**: Executes commands with the appropriate redirections and pipes
4. **Builtins**: Handles implementation of built-in shell commands
5. **Environment**: Manages environment variables
6. **Signals**: Handles signals appropriately

## Requirements

- C compiler (gcc or clang)
- GNU Make
- ReadLine library 