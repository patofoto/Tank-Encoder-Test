# Tank Controller Configuration Guide

## 🎉 NEW: Runtime Configuration System!

Your Tank Controller now supports **runtime configuration** - adjust settings without reflashing!

### 🌟 Two Ways to Configure:

1. **🌐 Web UI (Recommended for Users)**
   - Open `TankConfigurator.html` in Chrome/Edge
   - Drag sliders to adjust settings in real-time
   - Click presets for instant configurations
   - Changes apply immediately!
   - See `USER_GUIDE.md` for details

2. **💻 Code Configuration (For Developers)**
   - Edit `src/config.h` and reflash
   - Useful for setting factory defaults
   - Required for hardware pin changes

---

## 📖 Documentation Files

- **`USER_GUIDE.md`** - Complete guide for end users (Web UI + Serial commands)
- **`CONFIG_README.md`** (this file) - Developer reference

---

## Quick Start for Users

**No coding required!**

1. Plug in Tank Controller via USB
2. Open `TankConfigurator.html` in Chrome/Edge browser
3. Click "Connect" and select your device
4. Drag sliders or click preset buttons
5. Click "Save to Device" to make settings permanent
6. Done! Settings persist even on different computers

---

## Quick Start for Developers

To change **factory defaults** or **hardware pins**:

1. Edit `src/config.h`
2. Modify the `FACTORY_DEFAULTS` struct or pin assignments
3. Rebuild and upload: `pio run -e teensy41 -t upload`

---

## Configuration Sections

### 📌 Pin Configuration
```cpp
HORIZONTAL_ENCODER_PIN_A = 1
HORIZONTAL_ENCODER_PIN_B = 2
VERTICAL_ENCODER_PIN_A = 3
VERTICAL_ENCODER_PIN_B = 4
FIRE_BUTTON_1_PIN = 5
FIRE_BUTTON_2_PIN = 6
LED_PIN = 13
```
**Change these if you wire your encoders/buttons to different pins.**

---

### 🎮 Control Mode
**Runtime adjustable via Web UI or command: `set arrows true`**

- `false` = WASD keys (default)
- `true` = Arrow keys

---

### ⚙️ Realistic Tank Gearing
**Runtime adjustable via Web UI or commands: `set gear_h 20.0`**

#### Gear Ratios
- `gear_h`: Horizontal/Turret rotation (default: 15.0)
- `gear_v`: Vertical/Cannon elevation (default: 12.0)

**What this means:**
- Higher = need more encoder turns per action = more realistic/slower
- Lower = less encoder turns per action = more arcade/faster

**Real tank reference:**
- Real tanks: 20-30:1 for turret, 15-20:1 for cannon
- Current default: 15:1 turret, 12:1 cannon (good balance)

**Tuning guide:**
- Too slow/sluggish? **Decrease** (try 10.0)
- Too fast/twitchy? **Increase** (try 20.0)
- Want different horizontal vs vertical? Adjust independently
- **Use Web UI sliders for real-time feedback!**

---

### ⚡ Speed Sensitivity
**Runtime adjustable via Web UI**

- `min_hold`: Minimum tap for slow movements (default: 30ms)
- `max_hold`: Maximum hold for fast movements (default: 200ms)
- `speed_sense`: Speed multiplier (default: 3.0)

**How it works:**
- Slow encoder turns = brief key tap (min_hold)
- Fast encoder cranking = longer key hold (max_hold)
- Creates realistic momentum feel

**Tuning:**
- Want faster max speed? **Increase** max_hold
- Want snappier slow movements? **Decrease** min_hold
- Want more speed range? **Increase** speed_sense
- **Drag Web UI sliders to experiment!**

---

### 🎯 Momentum & Inertia
**Runtime adjustable: `set decay 0.85`**

`decay`: Speed decay when you stop turning (default: 0.85)

**Controls how fast speed decays when you stop turning:**
- `0.95` = lots of momentum (drifts after you stop)
- `0.85` = realistic inertia (default)
- `0.50` = quick stop (arcade feel)
- `0.10` = instant stop (no momentum)

---

### 🔫 Fire Buttons
**Runtime adjustable: `set fire_debounce 50`**

`fire_debounce`: Debounce time to prevent false triggers (default: 50ms)
- 50ms is good for most buttons
- Increase if you get double-fires
- Decrease if buttons feel sluggish

---

### 🖥️ Serial Configuration
**Fixed in firmware (requires reflash to change)**

- Baud rate: 115200 (serial communication speed)
- Status print interval: adjustable via `statusPrintIntervalMs`

---

## 🎛️ Runtime Adjustable Settings

All settings can now be changed via Web UI or serial commands **without reflashing!**

### Available Settings

| Setting | Range | Description |
|---------|-------|-------------|
| `arrows` | true/false | Use Arrow Keys instead of WASD |
| `gear_h` | 5.0 - 30.0 | Horizontal gear ratio (turret) |
| `gear_v` | 5.0 - 30.0 | Vertical gear ratio (cannon) |
| `speed_sense` | 1.0 - 10.0 | Speed sensitivity multiplier |
| `min_hold` | 10 - 500 | Minimum key hold time (ms) |
| `max_hold` | 10 - 500 | Maximum key hold time (ms) |
| `decay` | 0.0 - 1.0 | Speed decay (momentum) |
| `fire_debounce` | 10 - 200 | Fire button debounce (ms) |

### Built-in Presets

Users can instantly load these via Web UI:

#### 🏎️ ARCADE
- Fast, responsive
- Gear ratios: 5:1
- Good for: Action games, quick gameplay

#### ⚖️ BALANCED (Default)
- All-around feel
- Gear ratios: 15:1 turret, 12:1 cannon
- Good for: Most users

#### 🛡️ SIMULATION
- Realistic, slow, deliberate
- Gear ratios: 25:1 turret, 20:1 cannon
- Good for: Tank simulators, maximum realism

#### 🎯 SNIPER
- Ultra-precise aiming
- Gear ratios: 30:1 turret, 25:1 cannon
- Good for: Long-range precision shots

---

## 💡 Tuning Tips

### Problem: Turret moves too slow
**Solution:** 
- Open Web UI
- Drag "Horizontal Gear Ratio" slider left (try 10.0)
- Or try "Arcade" preset

### Problem: Can't aim precisely
**Solution:** 
- Drag gear ratio sliders right (try 20.0+)
- Or try "Sniper" preset

### Problem: Feels laggy/unresponsive
**Solution:** 
- Decrease min_hold (drag slider left)
- Increase speed_sense (drag slider right)
- Or try "Arcade" preset

### Problem: Too twitchy/jerky
**Solution:**
- Increase decay/momentum slider
- Increase gear ratios
- Or try "Simulation" preset

### Problem: Need different horizontal vs vertical feel
**Solution:** Adjust horizontal and vertical gear ratio sliders independently

### 🎯 Pro Tip: Real-Time Tuning
The Web UI applies changes **instantly**! You can:
1. Keep the controller connected
2. Test in your game
3. Alt-Tab to Web UI
4. Adjust sliders
5. Instantly feel the difference
6. Once perfect, click "Save to Device"

---

## 🎮 Game-Specific Recommendations

### War Thunder
- Gear ratios: 15-20:1
- Speed sensitivity: 3.0-4.0

### World of Tanks
- Gear ratios: 12-18:1
- Speed sensitivity: 2.5-3.5

### Arcade tank games
- Use PRESET_ARCADE
- Or gear ratios: 5-8:1

### Simulators (DCS, Steel Beasts)
- Use PRESET_SIMULATION
- Or gear ratios: 20-30:1

---

## 📝 Notes

### For End Users:
- **No coding or reflashing needed!**
- Settings saved to device EEPROM (persist forever)
- Works on any computer - just plug and play
- Share `TankConfigurator.html` file with friends
- See `USER_GUIDE.md` for complete instructions

### For Developers:
- Settings structure in `config.h` (Settings struct)
- EEPROM functions in `main.cpp`
- Serial command parser handles text commands
- Factory defaults in `FACTORY_DEFAULTS` constant
- Pin configuration still requires reflashing

### Best Practices:
- Start with a preset, then fine-tune
- Test in your specific game
- Use Web UI for experimentation
- Save to EEPROM once you find the perfect feel
- Different games may need different settings (no problem - adjust on the fly!)

### Distribution:
When handing controller to users:
1. Flash firmware once (with this configuration system)
2. Give them `TankConfigurator.html`
3. Send them `USER_GUIDE.md`
4. They can configure it themselves - forever!

---

## 🔧 Serial Commands Quick Reference

Connect at 115200 baud:

```bash
help                       # Show all commands
list                       # Display all settings
get gear_h                 # Get specific setting
set gear_h 20.0           # Change setting
save                       # Save to EEPROM (persist)
load                       # Reload from EEPROM
reset                      # Factory defaults
preset arcade              # Load preset
preset simulation          # Load preset
preset sniper              # Load preset
```

See `USER_GUIDE.md` for complete command documentation.

---

**Happy tanking! 🚀🎮**

