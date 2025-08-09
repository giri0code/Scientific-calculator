#!/bin/bash

# Scientific Calculator Installation Script

echo "Installing Scientific Calculator..."

# Create application directory
INSTALL_DIR="/opt/scientific-calculator"
sudo mkdir -p "$INSTALL_DIR"

# Copy executable
sudo cp calculator "$INSTALL_DIR/"
sudo chmod +x "$INSTALL_DIR/calculator"

# Update desktop file with correct path
sed "s|/home/user/Music/mojoprac/calculator|$INSTALL_DIR/calculator|g" scientific-calculator.desktop > /tmp/scientific-calculator.desktop

# Install desktop file
sudo cp /tmp/scientific-calculator.desktop /usr/share/applications/
sudo chmod 644 /usr/share/applications/scientific-calculator.desktop

# Create symlink for command line usage
sudo ln -sf "$INSTALL_DIR/calculator" /usr/local/bin/scientific-calculator

echo "Installation complete!"
echo "You can now:"
echo "1. Find 'Scientific Calculator' in your applications menu"
echo "2. Run 'scientific-calculator' from terminal"
echo "3. Right-click on files and 'Open with Scientific Calculator'"

# Update desktop database
sudo update-desktop-database /usr/share/applications/ 2>/dev/null || true

echo ""
echo "Desktop features enabled:"
echo "✓ Window decorations (close, minimize, maximize)"
echo "✓ Draggable window"
echo "✓ Resizable window"
echo "✓ Keyboard shortcuts"
echo "✓ Menu bar with File, View, Help"
echo "✓ Always on top option"
echo "✓ About dialog"
echo "✓ Desktop integration"
echo "✓ Application icon"
