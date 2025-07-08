# MiniShell Testing Guide

This document contains comprehensive test cases for the minishell project, covering all major features and edge cases.

## Table of Contents
1. [Basic Commands](#basic-commands)
2. [Built-in Commands](#built-in-commands)
3. [Environment Variables](#environment-variables)
4. [Redirections](#redirections)
5. [Pipes](#pipes)
6. [Logical Operators](#logical-operators)
7. [Quotes and Escaping](#quotes-and-escaping)
8. [Wildcards](#wildcards)
9. [Subshells](#subshells)
10. [Error Handling](#error-handling)
11. [Edge Cases](#edge-cases)

---

## Basic Commands

### Test 1: Simple external commands
```bash
minishell> ls
# Expected: List directory contents
minishell> pwd
# Expected: Print current working directory
minishell> whoami
# Expected: Print current user
```

### Test 2: Commands with arguments
```bash
minishell> ls -la
# Expected: List directory contents with details
minishell> cat /etc/hostname
# Expected: Display hostname file content
minishell> grep "text" file.txt
# Expected: Search for "text" in file.txt
```

---

## Built-in Commands

### Test 3: echo command
```bash
minishell> echo Hello World
# Expected: Hello World

minishell> echo -n Hello World
# Expected: Hello World (no newline)

minishell> echo -nnnnnn Hello World
# Expected: Hello World (no newline)

minishell> echo -nnn -n Hello
# Expected: Hello (no newline)

minishell> echo -nx Hello
# Expected: -nx Hello (invalid flag, treated as text)

minishell> echo ""
# Expected: (empty line)

minishell> echo
# Expected: (empty line)
```

### Test 4: cd command
```bash
minishell> cd /tmp
minishell> pwd
# Expected: /tmp

minishell> cd
minishell> pwd
# Expected: /home/username (or $HOME)

minishell> cd ..
minishell> pwd
# Expected: Parent directory

minishell> cd /nonexistent
# Expected: cd: /nonexistent: No such file or directory

minishell> cd file.txt
# Expected: cd: file.txt: Not a directory
```

### Test 5: pwd command
```bash
minishell> pwd
# Expected: Current working directory path

minishell> pwd extra_args
# Expected: Current working directory path (ignores extra args)
```

### Test 6: export command
```bash
minishell> export VAR=value
minishell> echo $VAR
# Expected: value

minishell> export VAR="hello world"
minishell> echo $VAR
# Expected: hello world

minishell> export
# Expected: List all exported variables in sorted order

minishell> export INVALID-VAR=value
# Expected: export: INVALID-VAR: not a valid identifier

minishell> export 123VAR=value
# Expected: export: 123VAR: not a valid identifier
```

### Test 7: unset command
```bash
minishell> export VAR=value
minishell> echo $VAR
# Expected: value
minishell> unset VAR
minishell> echo $VAR
# Expected: (empty)

minishell> unset PATH
minishell> ls
# Expected: ls: command not found

minishell> unset INVALID-VAR
# Expected: unset: INVALID-VAR: not a valid identifier
```

### Test 8: env command
```bash
minishell> env
# Expected: List all environment variables

minishell> env extra_args
# Expected: List all environment variables (ignores extra args)
```

### Test 9: exit command
```bash
minishell> exit
# Expected: Shell exits with status 0

minishell> exit 42
# Expected: Shell exits with status 42

minishell> exit abc
# Expected: exit: numeric argument required
# Shell exits with status 2

minishell> exit 1 2 3
# Expected: exit: too many arguments
# Returns 1, does not exit
```

---

## Environment Variables

### Test 10: Variable expansion
```bash
minishell> export VAR=hello
minishell> echo $VAR
# Expected: hello

minishell> echo $VAR world
# Expected: hello world

minishell> echo ${VAR}world
# Expected: helloworld

minishell> echo $NONEXISTENT
# Expected: (empty)

minishell> echo $?
# Expected: Exit status of last command (0 if successful)
```

### Test 11: Special variables
```bash
minishell> echo $?
# Expected: 0 (or last exit status)

minishell> false
minishell> echo $?
# Expected: 1

minishell> echo $$
# Expected: $$ (literal, not expanded in minishell)
```

### Test 12: Variable expansion in different contexts
```bash
minishell> export VAR="hello world"
minishell> echo $VAR
# Expected: hello world

minishell> echo "$VAR"
# Expected: hello world

minishell> echo '$VAR'
# Expected: $VAR (literal)

minishell> echo $VAR$VAR
# Expected: hello worldhello world
```

---

## Redirections

### Test 13: Output redirection
```bash
minishell> echo hello > output.txt
minishell> cat output.txt
# Expected: hello

minishell> echo world >> output.txt
minishell> cat output.txt
# Expected: hello
#          world

minishell> ls > /dev/null
# Expected: No output (redirected to /dev/null)
```

### Test 14: Input redirection
```bash
minishell> echo "line1" > input.txt
minishell> echo "line2" >> input.txt
minishell> cat < input.txt
# Expected: line1
#          line2

minishell> wc -l < input.txt
# Expected: 2
```

### Test 15: Here document
```bash
minishell> cat << EOF
> line1
> line2
> EOF
# Expected: line1
#          line2

minishell> cat << "EOF"
> $VAR will not expand
> EOF
# Expected: $VAR will not expand

minishell> export VAR=test
minishell> cat << EOF
> $VAR will expand
> EOF
# Expected: test will expand
```

### Test 16: Multiple redirections
```bash
minishell> echo hello > file1.txt > file2.txt
minishell> cat file1.txt
# Expected: (empty - only file2.txt gets the output)
minishell> cat file2.txt
# Expected: hello

minishell> cat < input.txt > output.txt
minishell> cat output.txt
# Expected: Contents of input.txt copied to output.txt
```

---

## Pipes

### Test 17: Simple pipes
```bash
minishell> echo hello | cat
# Expected: hello

minishell> ls | grep ".txt"
# Expected: List only .txt files

minishell> echo "hello world" | wc -w
# Expected: 2
```

### Test 18: Multiple pipes
```bash
minishell> echo "hello world test" | tr ' ' '\n' | wc -l
# Expected: 3

minishell> ls | grep ".c" | wc -l
# Expected: Number of .c files

minishell> cat file.txt | sort | uniq | wc -l
# Expected: Number of unique lines in file.txt
```

### Test 19: Pipes with redirections
```bash
minishell> echo hello | cat > output.txt
minishell> cat output.txt
# Expected: hello

minishell> ls | grep ".txt" > txt_files.txt
minishell> cat txt_files.txt
# Expected: List of .txt files
```

---

## Logical Operators

### Test 20: AND operator (&&)
```bash
minishell> true && echo success
# Expected: success

minishell> false && echo failure
# Expected: (no output)

minishell> echo first && echo second
# Expected: first
#          second

minishell> cd /nonexistent && pwd
# Expected: cd: /nonexistent: No such file or directory
#          (pwd not executed)
```

### Test 21: OR operator (||)
```bash
minishell> true || echo not executed
# Expected: (no output from echo)

minishell> false || echo executed
# Expected: executed

minishell> cd /nonexistent || echo "cd failed"
# Expected: cd: /nonexistent: No such file or directory
#          cd failed
```

### Test 22: Complex logical combinations
```bash
minishell> true && echo a || echo b
# Expected: a

minishell> false && echo a || echo b
# Expected: b

minishell> true && true && echo success
# Expected: success

minishell> true && false && echo not_reached
# Expected: (no output)

minishell> false || false || echo final
# Expected: final
```

---

## Quotes and Escaping

### Test 23: Single quotes
```bash
minishell> echo 'hello world'
# Expected: hello world

minishell> export VAR=test
minishell> echo '$VAR'
# Expected: $VAR (no expansion)

minishell> echo 'special chars: | && || > < $'
# Expected: special chars: | && || > < $
```

### Test 24: Double quotes
```bash
minishell> echo "hello world"
# Expected: hello world

minishell> export VAR=test
minishell> echo "$VAR"
# Expected: test

minishell> echo "special: $VAR but not | && ||"
# Expected: special: test but not | && ||
```

### Test 25: Mixed quotes
```bash
minishell> echo 'single' "double" mixed
# Expected: single double mixed

minishell> echo "contain 'single' quotes"
# Expected: contain 'single' quotes

minishell> echo 'contain "double" quotes'
# Expected: contain "double" quotes

minishell> export VAR=test
minishell> echo '$VAR' "$VAR" $VAR
# Expected: $VAR test test
```

### Test 26: Empty quotes
```bash
minishell> echo "" '' ""
# Expected: (two spaces - three empty strings)

minishell> echo "hello"''world
# Expected: helloworld

minishell> echo 'a'"b"'c'
# Expected: abc
```

---

## Wildcards

### Test 27: Basic wildcards
```bash
minishell> echo *.c
# Expected: List all .c files (or *.c if no matches)

minishell> echo src/*.c
# Expected: List all .c files in src directory

minishell> echo file?.txt
# Expected: Match files like file1.txt, filea.txt, etc.
```

### Test 28: Wildcard in different contexts
```bash
minishell> ls *.txt
# Expected: List .txt files

minishell> echo "*.txt"
# Expected: *.txt (literal, no expansion in quotes)

minishell> echo '*.txt'
# Expected: *.txt (literal, no expansion in quotes)
```

---

## Subshells

### Test 29: Basic subshells
```bash
minishell> (echo hello)
# Expected: hello

minishell> (cd /tmp && pwd)
minishell> pwd
# Expected: /tmp
#          (original directory - cd was in subshell)

minishell> (export VAR=test && echo $VAR)
minishell> echo $VAR
# Expected: test
#          (empty - export was in subshell)
```

### Test 30: Subshells with pipes and redirections
```bash
minishell> (echo hello; echo world) | wc -l
# Expected: 2

minishell> (echo error >&2; echo output) 2>/dev/null
# Expected: output

minishell> echo before && (echo inside) && echo after
# Expected: before
#          inside
#          after
```

---

## Error Handling

### Test 31: Syntax errors
```bash
minishell> echo hello |
# Expected: Syntax error near unexpected token `|'

minishell> echo hello &&
# Expected: Syntax error near unexpected token `&&'

minishell> echo hello ||
# Expected: Syntax error near unexpected token `||'

minishell> (echo hello
# Expected: Syntax error near unexpected token `)'

minishell> echo hello)
# Expected: Syntax error near unexpected token `)'
```

### Test 32: Command not found
```bash
minishell> nonexistent_command
# Expected: nonexistent_command: command not found

minishell> /nonexistent/path
# Expected: /nonexistent/path: No such file or directory
```

### Test 33: Permission errors
```bash
minishell> touch test_file
minishell> chmod 000 test_file
minishell> cat test_file
# Expected: cat: test_file: Permission denied

minishell> ./test_file
# Expected: ./test_file: Permission denied
```

### Test 34: Redirection errors
```bash
minishell> echo hello > /root/test
# Expected: bash: /root/test: Permission denied

minishell> cat < nonexistent_file
# Expected: cat: nonexistent_file: No such file or directory

minishell> echo hello >
# Expected: Syntax error near unexpected token `newline'
```

---

## Edge Cases

### Test 35: Empty commands
```bash
minishell> 
# Expected: (new prompt, no error)

minishell>     
# Expected: (new prompt, no error)

minishell> ;
# Expected: Syntax error (if semicolons are implemented)
```

### Test 36: Long commands and arguments
```bash
minishell> echo $(python3 -c "print('a' * 1000)")
# Expected: 1000 'a' characters

minishell> echo very long command with many arguments that should still work correctly
# Expected: very long command with many arguments that should still work correctly
```

### Test 37: Special characters in filenames
```bash
minishell> touch "file with spaces.txt"
minishell> ls "file with spaces.txt"
# Expected: file with spaces.txt

minishell> echo hello > "file with spaces.txt"
minishell> cat "file with spaces.txt"
# Expected: hello
```

### Test 38: Signal handling
```bash
minishell> sleep 10
# Press Ctrl+C
# Expected: Process interrupted, return to prompt

minishell> cat
# Press Ctrl+D
# Expected: cat exits normally

minishell> sleep 10
# Press Ctrl+\ 
# Expected: Process killed with SIGQUIT
```

### Test 39: Complex combinations
```bash
minishell> export VAR="hello world"
minishell> echo "$VAR" | tr ' ' '\n' | sort | uniq | wc -l
# Expected: 2

minishell> (echo start && false) || echo fallback
# Expected: start
#          fallback

minishell> ls *.c 2>/dev/null | head -5 | tail -1
# Expected: 5th .c file (if exists)
```

### Test 40: Memory and cleanup
```bash
# Run these commands and check with valgrind for memory leaks
minishell> ls
minishell> echo $PATH
minishell> cat < /etc/hostname > output.txt
minishell> rm output.txt
minishell> exit
# Expected: No memory leaks reported by valgrind
```

---

## Test Execution Notes

1. **Before testing**: Ensure minishell is compiled with `make`
2. **Memory testing**: Run with `valgrind --leak-check=full ./minishell`
3. **Comparison testing**: Compare outputs with bash behavior
4. **File cleanup**: Remove test files between test sessions
5. **Environment**: Some tests depend on system environment and available commands

## Expected Behavior Standards

- All commands should behave similarly to bash
- Error messages should be informative and match bash format when possible
- Memory should be properly managed (no leaks)
- Signal handling should work correctly
- Exit codes should match bash conventions
- Prompt should remain responsive and functional after errors 