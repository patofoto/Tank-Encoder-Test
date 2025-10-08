# Tank Controller Configuration Guide

All adjustable parameters are now centralized in `src/config.h` for easy tuning.

## Quick Start

1. Open `src/config.h`
2. Adjust the parameters you want to change
3. Save the file
4. Rebuild and upload: `pio run -e teensy41 -t upload`

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
```cpp
USE_ARROW_KEYS = false
```
- `false` = WASD keys (default)
- `true` = Arrow keys

---

### ⚙️ Realistic Tank Gearing

#### Gear Ratios
```cpp
HORIZONTAL_GEAR_RATIO = 15.0f  // Turret rotation
VERTICAL_GEAR_RATIO = 12.0f    // Cannon elevation
```

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

---

### ⚡ Speed Sensitivity
```cpp
MIN_KEY_HOLD_MS = 30      // Minimum tap for slow movements
MAX_KEY_HOLD_MS = 200     // Maximum hold for fast movements
SPEED_SENSITIVITY = 3.0f   // Speed multiplier
```

**How it works:**
- Slow encoder turns = brief key tap (MIN_KEY_HOLD_MS)
- Fast encoder cranking = longer key hold (MAX_KEY_HOLD_MS)
- Creates realistic momentum feel

**Tuning:**
- Want faster max speed? **Increase** MAX_KEY_HOLD_MS
- Want snappier slow movements? **Decrease** MIN_KEY_HOLD_MS
- Want more speed range? **Increase** SPEED_SENSITIVITY

---

### 🎯 Momentum & Inertia
```cpp
SPEED_DECAY = 0.85f
```

**Controls how fast speed decays when you stop turning:**
- `0.95` = lots of momentum (drifts after you stop)
- `0.85` = realistic inertia (default)
- `0.50` = quick stop (arcade feel)
- `0.10` = instant stop (no momentum)

---

### 🔫 Fire Buttons
```cpp
FIRE_DEBOUNCE_MS = 50
```
Debounce time to prevent false triggers.
- 50ms is good for most buttons
- Increase if you get double-fires
- Decrease if buttons feel sluggish

---

### 🖥️ Serial Debug
```cpp
SERIAL_BAUD_RATE = 115200
STATUS_PRINT_INTERVAL_MS = 3000
```
- Baud rate for serial monitor
- How often to print status (in milliseconds)

---

## 🎛️ Quick Presets

Uncomment ONE of these in `config.h` to quickly switch profiles:

### ARCADE MODE
```cpp
#define PRESET_ARCADE
```
- Fast, responsive
- Gear ratios: 5:1
- Good for: Action games, quick gameplay

### SIMULATION MODE
```cpp
#define PRESET_SIMULATION
```
- Realistic, slow, deliberate
- Gear ratios: 25:1 turret, 20:1 cannon
- Good for: Tank simulators, maximum realism

### SNIPER MODE
```cpp
#define PRESET_SNIPER
```
- Ultra-precise aiming
- Gear ratios: 30:1 turret, 25:1 cannon
- Faster tap speed for micro-adjustments
- Good for: Long-range precision shots

---

## 💡 Tuning Tips

### Problem: Turret moves too slow
**Solution:** Decrease `HORIZONTAL_GEAR_RATIO` (try 10.0)

### Problem: Can't aim precisely
**Solution:** Increase gear ratios (try 20.0 or more)

### Problem: Feels laggy/unresponsive
**Solution:** 
- Decrease `MIN_KEY_HOLD_MS` (try 20)
- Increase `SPEED_SENSITIVITY` (try 5.0)

### Problem: Too twitchy/jerky
**Solution:**
- Increase `SPEED_DECAY` (try 0.90)
- Increase gear ratios

### Problem: Need different horizontal vs vertical feel
**Solution:** Adjust `HORIZONTAL_GEAR_RATIO` and `VERTICAL_GEAR_RATIO` independently

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

- All changes require rebuild and re-upload
- Start with defaults, then adjust incrementally
- Test in your specific game before fine-tuning
- Different games may need different profiles
- You can create multiple config files and swap them

---

**Happy tanking! 🚀**

