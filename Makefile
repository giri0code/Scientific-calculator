# Makefile for GTK Calculator

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11 `pkg-config --cflags gtk+-3.0`

# Linker flags
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Target executable
TARGET = calculator

# Source files
SOURCES = calculator.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt update
	sudo apt install -y build-essential libgtk-3-dev pkg-config

# Install dependencies (Fedora/RHEL)
install-deps-fedora:
	sudo dnf install -y gcc-c++ gtk3-devel pkgconfig

# Install dependencies (Arch Linux)
install-deps-arch:
	sudo pacman -S --needed base-devel gtk3 pkgconf

# Run the calculator
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean install-deps install-deps-fedora install-deps-arch run
