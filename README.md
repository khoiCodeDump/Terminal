# Installation

1. **Ensure WSL is installed** (if using Windows).
2. **Clone the repository**:  
   ```bash
   git clone https://github.com/khoiCodeDump/Terminal.git
   ```
3. **Navigate into the Terminal directory**:  
   ```bash
   cd Terminal
   ```
4. **Install necessary packages**:  
   ```bash
   sudo apt install gcc meson libgtk-3-dev libxml2-dev libgirepository1.0-dev xsltproc
   ```
5. **Install Glade**:  
   ```bash
   sudo apt install glade
   ```
6. **Compile the main program**:  
   ```bash
   make main_compile
   ```
7. **Run the terminal**:  
   ```bash
   ./terminal
   ```
8. An external terminal should open with the current directory path printed.

# Supported Commands

> **DISCLAIMER**: Any key input will be read in sequence and will be passed into the argument. Commands should be typed character for character.  
> **Note**: Only some keys are handled. For example, `[command]ls` does not read as 'ls' even though that is what is displayed in the shell and will result in an error.

- **`cd <directory name>`**  
  Change your directory if it exists under the parent folder. Supports `..`.

- **`pwd`**  
  Display the current working directory.

- **`mkdir <directory name>`**  
  Create a new child directory with the given name unless it already exists.

- **`rmdir <directory name>`**  
  Remove the directory unless it is not empty. Will output an error message if not empty.

- **`ls <directory name>(optional)`**  
  List the contents of the specified directory. If no directory is specified, lists contents of the current directory.

- **`cp <file name to be copied> <file name copied to>`**  
  Copy the contents of the first listed file to the other.

- **`exit`**  
  Exit the shell.

- **`./<executable name>`**  
  Run executables and print the output in the terminal.
