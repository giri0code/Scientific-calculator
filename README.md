# Scientific Calculator

A feature-rich scientific calculator built with GTK3, designed to work like a professional desktop application on Linux and Windows.

## Features

### 🧮 **Calculator Functions**
- **Basic Arithmetic**: Addition, Subtraction, Multiplication, Division
- **Scientific Functions**: sin, cos, tan, log, ln, √, x², xʸ, factorial
- **Constants**: π (pi), e (Euler's number)
- **Memory Functions**: M+, M-, MR (recall), MC (clear)
- **Utility Functions**: Percentage, Sign change, Parentheses

### 🖥️ **Desktop App Features**
- ✅ **Full Window Controls**: Close, minimize, maximize buttons
- ✅ **Draggable Window**: Click and drag from title bar
- ✅ **Resizable Window**: Resize to your preferred size
- ✅ **Keyboard Support**: Full keyboard input support
- ✅ **Menu Bar**: File, View, Help menus
- ✅ **Always on Top**: Keep calculator above other windows
- ✅ **About Dialog**: Application information
- ✅ **Desktop Integration**: Shows in applications menu
- ✅ **Application Icon**: Professional calculator icon

### ⌨️ **Keyboard Shortcuts**
- **Numbers**: 0-9, Numpad 0-9
- **Operations**: +, -, *, / (×, ÷)
- **Equals**: Enter, Return, =
- **Decimal**: . (period)
- **Clear All**: Escape (AC)
- **Clear Entry**: Delete (CE)
- **Backspace**: Backspace (⌫)
- **Parentheses**: (, )
- **Percentage**: %

## Installation

### Linux (Ubuntu/Debian/Pop!_OS)

1. **Install Dependencies**:
   ```bash
   sudo apt update
   sudo apt install build-essential libgtk-3-dev pkg-config
   ```

2. **Build and Install**:
   ```bash
   git clone <repository-url>
   cd mojoprac
   make
   sudo ./install.sh
   ```

3. **Launch**:
   - From Applications Menu: Search "Scientific Calculator"
   - From Terminal: `scientific-calculator`

### Windows (MSYS2/MinGW)

1. **Install MSYS2** from https://www.msys2.org/

2. **Install Dependencies**:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config
   ```

3. **Build**:
   ```bash
   cd mojoprac
   make -f Makefile.cross-platform
   ```

4. **Run**: Double-click `calculator.exe`
## instruction 
```
gtk_window_set_resizable(GTK_WINDOW(window), FALSE); // Disable resizing
to enable change from FALSE TO TRUE 
```
<h5>it will be defaule FALSE </h5>
## Usage

### Basic Calculations
1. Click numbers or use keyboard
2. Click operations (+, -, ×, ÷)
3. Press = or Enter for result

### Scientific Functions
1. Enter a number
2. Click scientific function (sin, cos, tan, etc.)
3. Result appears instantly

### Memory Functions
- **M+**: Add current number to memory
- **M-**: Subtract current number from memory
- **MR**: Recall memory value
- **MC**: Clear memory

### Advanced Features
- **Always on Top**: View → Always on Top
- **Keyboard Input**: Use keyboard for all operations
- **History**: View expression history in top display

## Building from Source

### Requirements
- GCC or Clang compiler
- GTK3 development libraries
- pkg-config
- make

### Compilation
```bash
# Standard build
make

# Cross-platform build
make -f Makefile.cross-platform

# Clean build files
make clean

# Create distribution package
make package
```

## File Structure
```
mojoprac/
├── calculator.cpp              # Main source code
├── Makefile                   # Linux build file
├── Makefile.cross-platform   # Cross-platform build file
├── scientific-calculator.desktop # Linux desktop file
├── install.sh                # Linux installation script
└── README.md                 # This file
```

## Technical Details

### Architecture
- **Language**: C++11
- **GUI Framework**: GTK3
- **Build System**: Make
- **Platform**: Linux, Windows (via MSYS2)

### Key Components
- **Calculator Class**: Main application logic
- **GTK Window**: Native window with decorations
- **Event Handling**: Mouse clicks and keyboard input
- **CSS Styling**: Modern button appearance
- **Menu System**: Professional menu bar

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on your platform
5. Submit a pull request

## License

This project is open source. Feel free to modify and distribute.

## Support

For issues or questions:
1. Check existing issues in the repository
2. Create a new issue with details
3. Include your OS and GTK version

---

**Enjoy calculating!** 🧮✨
