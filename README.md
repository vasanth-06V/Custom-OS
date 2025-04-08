# 🐚 Custom Linux Shell

A custom Linux shell built in C with support for renamed Linux commands, auto-completion, script execution, and task scheduling.

---

## 📄 Abstract

This project implements a user-friendly Linux shell in C that supports auto-completion, renamed commands for better readability, shell script execution, and task scheduling. It provides a simplified interface for beginners to interact with a Unix-like system using intuitive command names.

---

## 🚀 Introduction

Linux shell environments like Bash are powerful but can seem complex for new users. This custom shell replaces standard commands with more readable alternatives (e.g., `list` for `ls`, `goto` for `cd`) and provides extended functionality like:

- 🔁 Auto-completion for custom commands  
- 📜 Script file execution  
- ⏱️ Task scheduling support  

---

## 🛠️ Methodology / Project Work

### Features

- **Custom Command Translation**  
  Maps readable commands (e.g., `create`, `move`, `show`) to real Linux commands (`touch`, `mv`, `cat`).

- **Auto-Completion**  
  Implemented using GNU Readline to suggest commands based on partial input.

- **Script Execution**  
  Supports running a `.myscript` file line-by-line like shell scripts.

- **Task Scheduling**  
  Enables background task execution using `&` or `sleep` commands within scripts.

### Technologies Used

- Programming Language: C  
- Libraries: GNU Readline, POSIX system calls

---

## 📊 Results and Testing

### ✅ Manual Testing Scenarios

| Feature              | Test Command                     | Expected Result                         | Status |
|----------------------|----------------------------------|------------------------------------------|--------|
| Change directory     | `goto myfolder`                  | Changes current working directory       | ✅     |
| File operations      | `create file.txt`                | Creates a file using `touch`            | ✅     |
| Script execution     | `runmyscript test.myscript`      | Executes all commands in the file       | ✅     |
| Auto-completion      | `sh<tab>`                        | Suggests `show`, `showmore`, `showless` | ✅     |
| Background tasks     | `pause 10 &`                     | Runs sleep in background                | ✅     |

---

## 🔚 Conclusion

The custom shell provides a simplified and extendable interface for Linux users. It bridges the gap between command-line power and beginner accessibility by introducing intuitive command naming, ease of use, and automation.

---

## 🔮 Future Work

- Add piping and redirection support  
- Implement environment variables and aliases  
- Create a graphical frontend for ease of access  
- Add built-in help documentation  

---


## 📁 Example Commands

```bash
me                         # pwd
goto folder/               # cd folder/
list                       # ls
create file.txt            # touch file.txt
remove file.txt            # rm file.txt
runmyscript myscript.mys   # Executes script
pause 10 &                 # Runs in background
bye / quit                 # Exit shell
