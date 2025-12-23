Import("env")
import shutil
import os
from datetime import datetime

def copy_firmware(source, target, env):
    """
    Post-build script to copy firmware .hex file to releases directory
    for easy distribution via GitHub Releases.
    """
    firmware_path = str(target[0])
    
    # Get firmware version from platformio.ini or default
    try:
        firmware_version = env.GetProjectOption("firmware_version", "1.0.2")
    except:
        firmware_version = "1.0.2"
    
    # Create releases directory if it doesn't exist
    releases_dir = os.path.join(env["PROJECT_DIR"], "firmware_releases")
    os.makedirs(releases_dir, exist_ok=True)
    
    # Generate filename with version and timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"tank_encoder_v{firmware_version}_{timestamp}.hex"
    output_file = os.path.join(releases_dir, filename)
    
    # Copy the firmware file
    if os.path.exists(firmware_path):
        shutil.copy(firmware_path, output_file)
        print(f"\n✓ Firmware copied to: {output_file}")
        print(f"  Ready for GitHub Release upload\n")
    else:
        print(f"\n⚠ Warning: Firmware file not found at {firmware_path}\n")

# Register the post-action
env.AddPostAction("$BUILD_DIR/firmware.hex", copy_firmware)

