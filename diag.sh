#!/bin/bash
# V5 Controller Connection Diagnostics Script
# Run this to validate V5 connection issues independently

echo "=== V5 CONNECTION DIAGNOSTICS ==="
echo

echo "1. Checking for V5 Controller USB connection..."
system_profiler SPUSBDataType | grep -i "v5\|vex\|2888" || echo "❌ No V5 Controller detected via USB"
echo

echo "2. Checking for VEX serial devices..."
ls /dev/tty.usbmodem* 2>/dev/null | head -5 || echo "❌ No usbmodem devices found"
echo

echo "3. Testing bundled PROS CLI..."
if [ -f "native/runtime/robodash-pros/robodash-pros" ]; then
    echo "✓ Bundled PROS CLI exists"
    if [ -x "native/runtime/robodash-pros/robodash-pros" ]; then
        echo "✓ Bundled PROS CLI is executable"
        native/runtime/robodash-pros/robodash-pros --version 2>&1 | head -3
    else
        echo "❌ Bundled PROS CLI is not executable"
        ls -la native/runtime/robodash-pros/robodash-pros
    fi
else
    echo "❌ Bundled PROS CLI not found"
fi
echo

echo "4. Testing system PROS CLI (if available)..."
which pros >/dev/null 2>&1 && {
    echo "✓ System PROS CLI found: $(which pros)"
    pros --version 2>&1 | head -3
} || echo "❌ No system PROS CLI found"
echo

echo "5. Checking for PROS project..."
if [ -f "project.pros" ]; then
    echo "✓ project.pros found in current directory"
else
    echo "❌ No project.pros in current directory"
    echo "   You need to run this from a PROS project directory"
fi
echo

echo "=== MANUAL CHECKS ==="
echo "Please verify:"
echo "• V5 Controller is powered ON"
echo "• USB cable supports data (not just charging)"
echo "• V5 Controller screen shows battery/connection status"
echo "• macOS System Preferences > Security & Privacy allows USB access"
echo "• Try different USB port or cable if needed"