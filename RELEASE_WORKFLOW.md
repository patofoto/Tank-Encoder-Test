# Release Workflow Guide

This guide explains how to create and distribute new firmware releases using GitHub Releases.

## Quick Start

1. **Update version numbers** in two places:
   - `src/config.h`: Update `FIRMWARE_VERSION` (e.g., `"1.0.1"`)
   - `platformio.ini`: Update `firmware_version` under `[platformio]` section

2. **Commit and push** your changes:
   ```bash
   git add .
   git commit -m "Release v1.0.1"
   git push
   ```

3. **Create a git tag** to trigger the automated build:
   ```bash
   git tag v1.0.1
   git push origin v1.0.1
   ```

4. **GitHub Actions will automatically**:
   - Build the firmware
   - Create a GitHub Release
   - Attach the `.hex` file for download

## Detailed Steps

### Step 1: Update Version Numbers

**In `src/config.h`:**
```cpp
#define FIRMWARE_VERSION "1.0.1"  // Update this
```

**In `platformio.ini`:**
```ini
[platformio]
firmware_version = 1.0.1  // Update this (no quotes)
```

**Important**: Both version numbers should match!

### Step 2: Test Your Changes Locally

Before releasing, test the build:

```bash
pio run -e teensy41
```

This will:
- Compile the firmware
- Copy the `.hex` file to `firmware_releases/` directory
- Verify everything builds correctly

### Step 3: Commit Your Changes

```bash
git add .
git commit -m "Release v1.0.1 - [describe your changes]"
git push
```

### Step 4: Create and Push a Tag

Git tags trigger the automated GitHub Actions workflow:

```bash
# Create the tag
git tag v1.0.1

# Push the tag to GitHub
git push origin v1.0.1
```

**Tag naming convention**: Use semantic versioning (e.g., `v1.0.0`, `v1.0.1`, `v1.1.0`, `v2.0.0`)

### Step 5: Monitor the Build

1. Go to your GitHub repository
2. Click on the **Actions** tab
3. Watch the "Build and Release Firmware" workflow run
4. The workflow will:
   - Build the firmware
   - Create a GitHub Release
   - Attach the `.hex` file

### Step 6: Verify the Release

1. Go to the **Releases** page on GitHub
2. You should see your new release with:
   - Release notes
   - The firmware `.hex` file attached
   - Build date and version information

## Manual Release (Alternative)

If you prefer to create releases manually:

1. Build the firmware locally:
   ```bash
   pio run -e teensy41
   ```

2. Find the `.hex` file:
   - Location: `.pio/build/teensy41/firmware.hex`
   - Or in: `firmware_releases/tank_encoder_v1.0.1_YYYYMMDD_HHMMSS.hex`

3. Go to GitHub → Releases → Draft a new release
4. Upload the `.hex` file
5. Add release notes describing the changes

## Version Numbering

Follow [Semantic Versioning](https://semver.org/):

- **MAJOR** (1.0.0 → 2.0.0): Breaking changes
- **MINOR** (1.0.0 → 1.1.0): New features, backward compatible
- **PATCH** (1.0.0 → 1.0.1): Bug fixes, backward compatible

## Troubleshooting

### GitHub Actions workflow fails

- Check the Actions tab for error messages
- Ensure your `platformio.ini` is valid
- Verify the firmware version format matches (e.g., `1.0.1` not `v1.0.1` in config files)

### Tag doesn't trigger workflow

- Ensure the tag format is `v*.*.*` (e.g., `v1.0.1`)
- Check that the workflow file is in `.github/workflows/`
- Verify you pushed the tag: `git push origin v1.0.1`

### Version mismatch

- Make sure `FIRMWARE_VERSION` in `config.h` matches `firmware_version` in `platformio.ini`
- The version in `config.h` should have quotes: `"1.0.1"`
- The version in `platformio.ini` should NOT have quotes: `1.0.1`

## Best Practices

1. **Always test locally** before creating a release
2. **Update version numbers** in both places before tagging
3. **Write clear release notes** describing what changed
4. **Tag from the main/master branch** to ensure stable releases
5. **Keep a changelog** of what changed in each version

## Example Release Workflow

```bash
# 1. Make your code changes
# ... edit files ...

# 2. Update versions
# Edit src/config.h: #define FIRMWARE_VERSION "1.0.1"
# Edit platformio.ini: firmware_version = 1.0.1

# 3. Test build
pio run -e teensy41

# 4. Commit
git add .
git commit -m "Release v1.0.1 - Fixed encoder sensitivity issue"

# 5. Push
git push

# 6. Tag and push
git tag v1.0.1
git push origin v1.0.1

# 7. Wait for GitHub Actions to complete (check Actions tab)
# 8. Verify release is created with .hex file attached
```

---

**Note**: The automated workflow only runs when you push a tag. For development builds, you can manually trigger the workflow from the Actions tab, but it won't create a release (just uploads artifacts).

