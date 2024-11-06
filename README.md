# cshell

**cshell** is a simple Unix-like shell created as a learning project based on the "OS in Three Easy Steps" module.
This project introduces core concepts in systems programming, including process management, command execution, and basic shell functionality.
The shell allows users to execute commands interactively or from batch files, handling various common features like redirection, parallel command execution, and built-in commands.

Link to [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
Link to [project description and testing info](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/processes-shell/README.md)

This shell has passed all the 22 test and the necessary conditions mentioned in the project requirements.

## Project Objectives

The primary goals of creating **cshell** are:

- To deepen familiarity with the Linux programming environment and command-line interface (CLI) basics.
- To understand how processes are created, managed, and executed in Unix-based systems.
- To gain hands-on experience implementing essential shell functionality, including handling built-in commands and system calls.

## Features

### Basic Shell Operations

**cshell** operates as a loop:

- Prints a prompt (`cshell>`) in interactive mode or reads commands from a file in batch mode.
- Parses and executes commands.
- Waits for command completion before returning control to the user.

### Built-in Commands

- **exit**: Exits the shell.
- **cd**: Changes the working directory (expects a single argument).
- **path**: Sets the search paths for executable files (can take multiple arguments).

### Redirection

**cshell** supports redirecting command output to a file using the `>` symbol. If the file exists, it overwrites the file with new output.

### Parallel Commands

The shell supports parallel command execution with the `&` symbol, running commands in parallel and waiting until all have completed.

## Usage

### Interactive Mode

To start **cshell** interactively:

```bash
./cshell
```
### Batch Mode

To execute commands from a file:

```bash
./cshell batch.txt
```

## Notes

This project builds a foundational understanding of shell operations without covering advanced shell features like absolute/relative paths. 
Testing thoroughly across different commands ensures robustness, especially with varied whitespace and argument syntax.
