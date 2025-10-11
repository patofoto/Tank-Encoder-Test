# 🎮 Tank Controller Configuration Guide

## Overview

Your Tank Controller now features **runtime-adjustable settings** that can be changed without reflashing the firmware! Settings are saved to EEPROM and persist across power cycles.

## 🌐 Method 1: Web Configurator (Recommended)

### Requirements
- **Browser**: Chrome, Edge, or Opera (supports Web Serial API)
- **Operating System**: Windows, Mac, or Linux

### How to Use

1. **Open the Configurator**
   - Locate `TankConfigurator.html` in your project folder
   - Double-click to open in your browser
   - *(You can also email this file to users - it's completely standalone!)*

2. **Connect Your Tank Controller**
   - Plug the Teensy into your computer via USB
   - Click the **"Connect"** button
   - Select your Teensy device from the popup (usually labeled "Teensy" or "USB Serial")
   - Status indicator will turn green when connected

3. **Adjust Settings**
   - **Quick Presets**: Click preset buttons for instant configurations
     - 🏎️ **Arcade**: Fast and responsive (great for beginners)
     - ⚖️ **Balanced**: Default settings (good all-around feel)
     - 🛡️ **Simulation**: Slow and realistic (like real tank controls)
     - 🎯 **Sniper**: Ultra-precise for careful aiming
   
   - **Fine Tuning**: Drag sliders to adjust individual settings
     - Changes apply **instantly** as you move sliders
     - See real-time feedback on the controller

4. **Save Your Settings**
   - Click **"Save to Device"** to store settings permanently in EEPROM
   - Settings will persist even when unplugged or used on different computers
   - If you don't save, changes only last until the device is unplugged

5. **Console Output**
   - The black console at the bottom shows device responses
   - Look for ✓ checkmarks to confirm successful operations
   - If you see ✗ errors, try reconnecting the device

---

## 💻 Method 2: Serial Terminal (Advanced)

### Requirements
- Serial terminal software (Arduino Serial Monitor, PuTTY, screen, CoolTerm, etc.)
- Baud rate: **115200**

### Basic Commands

```bash
help                    # Show all available commands
list                    # Display current settings
get <setting>           # Get specific setting value
set <setting> <value>   # Change a setting
save                    # Save to EEPROM
load                    # Reload from EEPROM
reset                   # Reset to factory defaults
preset <name>           # Load preset (arcade/simulation/sniper/default)
```

### Settings Reference

| Setting Name      | Range        | Description                                    |
|-------------------|--------------|------------------------------------------------|
| `arrows`          | true/false   | Use Arrow Keys (false = WASD)                  |
| `gear_h`          | 5.0 - 30.0   | Horizontal gear ratio (turret rotation)        |
| `gear_v`          | 5.0 - 30.0   | Vertical gear ratio (cannon elevation)         |
| `speed_sense`     | 1.0 - 10.0   | Speed sensitivity multiplier                   |
| `min_hold`        | 10 - 500     | Minimum key hold duration (ms)                 |
| `max_hold`        | 10 - 500     | Maximum key hold duration (ms)                 |
| `decay`           | 0.0 - 1.0    | Speed decay (momentum/inertia)                 |
| `min_tail`        | 10 - 1000    | Minimum tail hold time (ms)                    |
| `max_tail`        | 10 - 1000    | Maximum tail hold time (ms)                    |
| `idle_release`    | 100 - 2000   | Hard idle release timeout (ms)                 |
| `fire_debounce`   | 10 - 200     | Fire button debounce time (ms)                 |

### Example Usage

```bash
# View all current settings
list

# Adjust horizontal gear ratio to make turret faster
set gear_h 10.0

# Make the controller more sensitive to speed changes
set speed_sense 5.0

# Save changes permanently
save

# Load the arcade preset
preset arcade

# Switch to arrow keys instead of WASD
set arrows true

# Reset everything to factory defaults
reset
```

---

## ⚙️ Settings Explained

### Gear Ratios (gear_h, gear_v)
- **What it does**: Controls how many encoder clicks = one key press
- **Lower values (5-10)**: Fast, arcade-like feel
- **Medium values (12-15)**: Balanced, default feel
- **Higher values (20-30)**: Slow, realistic simulation feel
- **Tip**: Start at 15, adjust down for faster or up for more precision

### Speed Sensitivity (speed_sense)
- **What it does**: How much turning speed affects key hold duration
- **Lower values (1-2)**: Speed has less effect, more consistent
- **Medium values (3-4)**: Balanced response
- **Higher values (5-10)**: Fast turns = longer holds, very dynamic
- **Tip**: Increase if controller feels unresponsive during fast movements

### Momentum / Decay (decay)
- **What it does**: How quickly movement "slows down" after you stop turning
- **Lower values (0.5-0.7)**: Quick stop, snappy feel
- **Medium values (0.8-0.85)**: Realistic inertia
- **Higher values (0.9-0.95)**: Lots of momentum, smooth but less precise
- **Tip**: Decrease for instant stops, increase for realistic "heavy tank" feel

### Key Hold Times (min_hold, max_hold)
- **What it does**: Duration of key presses sent to the game
- **min_hold**: Shortest tap for very slow, precise movements
- **max_hold**: Longest hold for fast cranking
- **Tip**: If game responds too slowly, decrease both values

### Control Mode (arrows)
- **false** = WASD keys (W/S = up/down, A/D = left/right)
- **true** = Arrow keys (Up/Down/Left/Right)
- **Tip**: Use whatever your game expects

---

## 🎯 Preset Configurations

### 🏎️ Arcade
Fast and responsive for casual gaming.
- Gear ratios: 5.0 / 5.0
- Quick reactions, minimal momentum
- Great for beginners or fast-paced games

### ⚖️ Balanced (Default)
All-around feel for most users.
- Gear ratios: 15.0 / 12.0
- Moderate speed with some inertia
- Recommended starting point

### 🛡️ Simulation
Slow and realistic like a real tank.
- Gear ratios: 25.0 / 20.0
- High momentum, deliberate movements
- For hardcore simulation fans

### 🎯 Sniper
Ultra-precise for careful aiming.
- Gear ratios: 30.0 / 25.0
- Very short minimum taps
- Perfect for precision shots

---

## 🔧 Troubleshooting

### Web Configurator Won't Connect
- ✅ Use Chrome, Edge, or Opera browser (Safari and Firefox don't support Web Serial API)
- ✅ Make sure Teensy is plugged in via USB
- ✅ Close any other programs using the serial port (Arduino IDE, Serial Monitor, etc.)
- ✅ Try unplugging and replugging the Teensy
- ✅ On Linux, make sure you have permission to access serial ports

### Settings Don't Persist After Unplugging
- ✅ Click **"Save to Device"** button before unplugging
- ✅ Look for "✓ Settings saved to EEPROM" in console
- ✅ Try sending `save` command via serial terminal

### Controller Feels Unresponsive
- Try **Arcade preset** for faster response
- Decrease `gear_h` and `gear_v` values
- Increase `speed_sense` value
- Decrease `min_hold` and `max_hold` values

### Controller Too Sensitive/Fast
- Try **Simulation preset** for slower response
- Increase `gear_h` and `gear_v` values
- Decrease `speed_sense` value

### Movements Feel Jerky
- Increase `decay` value (more momentum)
- Adjust `speed_sense` to be more moderate

### Fire Buttons Triggering Multiple Times
- Increase `fire_debounce` value (try 100ms)

---

## 📋 Quick Start Workflow

1. **First Time Setup**
   - Open `TankConfigurator.html`
   - Connect to device
   - Try the **Arcade**, **Balanced**, **Simulation**, and **Sniper** presets
   - Choose the one that feels best

2. **Fine Tuning**
   - Adjust gear ratios to taste
   - Tweak speed sensitivity if needed
   - Test in your game

3. **Save**
   - Click **"Save to Device"**
   - Done! Settings are now permanent

4. **Hand Off to User**
   - Send them `TankConfigurator.html`
   - Tell them to open it in Chrome/Edge
   - They can adjust settings on any computer!

---

## 🚀 Advanced Tips

### Creating Custom Presets
While the web UI has 4 built-in presets, you can create your own by:
1. Adjusting all settings to your preference
2. Saving to device
3. These become your "personal defaults"

### Sharing Settings Between Devices
Settings are stored in the Teensy's EEPROM, not on your computer. To copy settings:
1. Open serial terminal on Device A
2. Type `list` to see all settings
3. Note down the values
4. Connect to Device B
5. Use `set` commands to replicate settings
6. Type `save` to store

### Rapid Testing
The web configurator applies changes **instantly** without saving. This lets you:
- Drag sliders and feel changes in real-time
- Test multiple configurations quickly
- Only save once you find the perfect feel

### Using Multiple Computers
Once settings are saved to EEPROM, they travel with the device:
- Configure at home
- Plug into friend's computer
- Same feel, no setup needed!

---

## 📞 Support

If you encounter issues:
1. Type `help` in serial terminal to verify firmware is responding
2. Try `reset` to restore factory defaults
3. Check that baud rate is set to 115200
4. Verify Web Serial API support at: https://caniuse.com/web-serial

---

## 🎉 Enjoy Your Tank Controller!

Experiment with different settings to find your perfect feel. The beauty of this system is that **anyone can adjust it** without needing to touch code or reflash firmware!

**Happy Gaming! 🎮🔥**

