# Firmware Flashing Guide

This guide explains how to update the firmware on your Tank Encoder Controller device.

## Prerequisites

- **Teensy 4.1** device
- **USB cable** (must support data transfer, not just charging)
- **Computer** (Windows, macOS, or Linux)
- **Teensy Loader Application** (download below)

## Step 1: Download Teensy Loader Application

1. Visit the official Teensy Loader page: https://www.pjrc.com/teensy/loader.html
2. Download the appropriate version for your operating system:
   - **Windows**: `Teensy.exe` (standalone executable, no installation needed)
   - **macOS**: `Teensy.app` (drag to Applications folder)
   - **Linux**: Follow the installation instructions on the page

## Step 2: Download Firmware

1. Go to the GitHub Releases page for this project
   - Replace `YOUR_USERNAME/YOUR_REPO` with your actual repository path
   - Example: `https://github.com/username/tank-encoder/releases`
2. Find the latest release (or the specific version you need)
3. Download the `tank_encoder_firmware.hex` file
4. Save it to an easy-to-find location (e.g., Desktop or Downloads folder)

## Step 3: Prepare Your Device

1. **Connect** your Teensy 4.1 to your computer via USB cable
2. **Press the button** on the Teensy board to put it into program mode
   - The LED may blink or change state when in program mode
   - You only need to press it once briefly

## Step 4: Flash the Firmware

### Using Teensy Loader Application

1. **Open** the Teensy Loader Application
2. **Click** "Open HEX File" button (or use File → Open HEX File)
3. **Navigate** to and select the `tank_encoder_firmware.hex` file you downloaded
4. **Verify** the device is detected:
   - You should see "Teensy Loader" or "Teensy 4.1" in the status area
   - If you see "Press Button on Teensy to Activate", press the button on the board
5. **Click** the "Program" button (or press Ctrl+P / Cmd+P)
6. **Wait** for the upload to complete (usually takes 5-10 seconds)
7. **Success!** The device will automatically reset and run the new firmware

### Verification

After flashing, you can verify the firmware version:

1. Open a serial monitor (115200 baud)
2. Connect to the device's serial port
3. Look for the startup message that includes:
   ```
   Firmware Version: X.X.X
   ```

## Troubleshooting

### Teensy Loader doesn't detect the device

- **Make sure** you've pressed the button on the Teensy board to enter program mode
- **Try** unplugging and replugging the USB cable
- **Check** that your USB cable supports data transfer (some cables are power-only)
- **Try** a different USB port on your computer
- **On Windows**: You may need to install drivers. Teensy Loader should prompt you if drivers are needed

### "Press Button on Teensy to Activate" message

- **Press** the physical button on the Teensy 4.1 board
- The button is usually located near the USB connector
- You may need to hold it briefly or press it multiple times

### Upload fails or times out

- **Check** that no other program is using the serial port (close serial monitors, Arduino IDE, etc.)
- **Try** closing and reopening Teensy Loader
- **Unplug** and replug the USB cable
- **Restart** Teensy Loader and try again

### Device doesn't work after flashing

- **Verify** you downloaded the correct firmware file for Teensy 4.1
- **Try** flashing again (sometimes uploads can be corrupted)
- **Check** the serial monitor for error messages
- **Contact** support if the issue persists

## Manual Flashing (Alternative Method)

If Teensy Loader doesn't work, you can also use PlatformIO:

1. Install PlatformIO: https://platformio.org/install
2. Clone or download this repository
3. Connect your device
4. Run: `pio run -e teensy41 -t upload`

## Checking Current Firmware Version

To check what firmware version is currently installed:

1. Connect the device via USB
2. Open a serial monitor at 115200 baud
3. Look for the startup message when the device boots
4. The version will be displayed as: `Firmware Version: X.X.X`

## Need Help?

If you encounter issues not covered in this guide:

1. Check the GitHub Issues page for this project
2. Review the serial monitor output for error messages
3. Contact the developer with:
   - Your operating system
   - Teensy Loader version
   - Error messages (if any)
   - Steps you've already tried

---

**Note**: Flashing new firmware will not erase your saved settings (stored in EEPROM). Your key bindings and configuration will be preserved.

