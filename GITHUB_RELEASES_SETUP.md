# GitHub Releases Setup - Summary

This document summarizes what has been set up for distributing firmware via GitHub Releases.

## What's Been Configured

### ✅ 1. Firmware Versioning
- Added `FIRMWARE_VERSION` define in `src/config.h`
- Version is displayed in serial output on device startup
- Version can be updated in two places (see RELEASE_WORKFLOW.md)

### ✅ 2. Post-Build Script
- Created `scripts/copy_firmware.py`
- Automatically copies `.hex` files to `firmware_releases/` directory after each build
- Files are named with version and timestamp for easy identification

### ✅ 3. GitHub Actions Workflow
- Created `.github/workflows/build-release.yml`
- Automatically builds firmware when you push a version tag (e.g., `v1.0.0`)
- Creates a GitHub Release with the `.hex` file attached
- Can also be triggered manually from the Actions tab

### ✅ 4. Documentation
- **FIRMWARE_FLASHING_GUIDE.md**: Complete guide for end users on how to flash firmware
- **RELEASE_WORKFLOW.md**: Guide for you on how to create new releases
- **.gitignore**: Excludes build artifacts and local firmware releases

## How to Use

### For You (Developer)

1. **Make code changes** as needed
2. **Update version** in `src/config.h` and `platformio.ini`
3. **Test locally**: `pio run -e teensy41`
4. **Commit and push**: `git commit && git push`
5. **Create tag**: `git tag v1.0.1 && git push origin v1.0.1`
6. **GitHub Actions automatically** builds and creates a release

See **RELEASE_WORKFLOW.md** for detailed instructions.

### For End Users (Testers)

1. Go to GitHub Releases page
2. Download the latest `tank_encoder_firmware.hex` file
3. Install Teensy Loader Application
4. Flash the firmware using Teensy Loader

See **FIRMWARE_FLASHING_GUIDE.md** for detailed instructions.

## Next Steps

1. **Update GitHub URLs** in `FIRMWARE_FLASHING_GUIDE.md`:
   - Replace placeholder text with your actual repository URL

2. **Test the workflow**:
   ```bash
   # Make a small change
   # Update version to 1.0.1
   git commit -m "Test release workflow"
   git push
   git tag v1.0.1
   git push origin v1.0.1
   ```

3. **Verify the release** appears on GitHub with the `.hex` file attached

4. **Share the repository** with your tester so they can access releases

## File Structure

```
.
├── .github/
│   └── workflows/
│       └── build-release.yml      # Automated build & release
├── scripts/
│   └── copy_firmware.py            # Post-build script
├── firmware_releases/               # Local .hex copies (gitignored)
├── src/
│   ├── config.h                    # Contains FIRMWARE_VERSION
│   └── main.cpp                    # Displays version on startup
├── platformio.ini                  # Contains firmware_version
├── FIRMWARE_FLASHING_GUIDE.md      # User guide
├── RELEASE_WORKFLOW.md             # Developer guide
└── .gitignore                      # Excludes build artifacts
```

## Important Notes

- **Version Sync**: Always update versions in BOTH `config.h` and `platformio.ini`
- **Tag Format**: Use semantic versioning tags like `v1.0.0`, `v1.0.1`, etc.
- **EEPROM Settings**: Flashing new firmware does NOT erase saved settings (they're in EEPROM)
- **Local Builds**: The post-build script creates local copies in `firmware_releases/` for manual distribution if needed

## Troubleshooting

- **Workflow not running?** Check that your tag format is `v*.*.*`
- **Version mismatch?** Ensure both version numbers match exactly
- **Build fails?** Check the Actions tab for error messages
- **Need manual release?** See RELEASE_WORKFLOW.md for manual steps

---

**You're all set!** The next time you want to release firmware, just update the version, commit, tag, and push. GitHub Actions will handle the rest.

