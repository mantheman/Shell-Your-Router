#!/bin/bash

# Globals:
REPO_PATH=`dirname "$0"`
# Generate random version between 1 to 200.
RANDOM_FIRMWARE_VERSION=$(( (RANDOM % 200) + 1 ))

# User specific paths:
TARGET_DIRECTORY=$HOME'/shared_projects/embedded/tmp'
FIRMWARE_NAME="new-firmware_version.bin"

# Firmware-Mod-Kit paths:
FMK_DIR=$HOME'/tools/firmware-mod-kit-master'
FMK_FIRMWARE_DIR='fmk'
FMK_BUILD_FIRMWARE_SCRIPT='build-firmware.sh'
FMK_OUTPUT_FIRMWARE='new-firmware.bin'

compile_firmware() {
    echo "============================================="
    echo "Compiling the firmware using Firmware-Mod-Kit"
    echo "============================================="
    bash $FMK_DIR/$FMK_BUILD_FIRMWARE_SCRIPT $FMK_DIR/$FMK_FIRMWARE_DIR
    compilation_result=$?
    echo Firmware compilation result: $compilation_result

    if [[ $compilation_result -eq 0 ]]
    then
        echo "Successfully compiled the firmware!"
        return 0
    else
        echo "Failed in firmware compilation, exiting."
        exit 1
    fi
}

fix_checksum() {
    echo "============================"
    echo "Fixing the firmware checksum"
    echo "============================"
    python3 $REPO_PATH/scripts/update_checksum.py -u $FMK_DIR/$FMK_FIRMWARE_DIR/$FMK_OUTPUT_FIRMWARE
}

# Note: to update firmware version, this script must be run as sudo.
update_firmware_version() {
    echo "========================="
    echo "Updating firmware version"
    echo "========================="
    firmware_version_file=$FMK_DIR/$FMK_FIRMWARE_DIR/rootfs/etc.tmp/version

    echo "Trying to update fimrware version to: $RANDOM_FIRMWARE_VERSION" 
    echo $RANDOM_FIRMWARE_VERSION > $firmware_version_file
    update_result=$?
    if [[ $update_result -ne 0 ]]; then
        echo "Couldn't update firmware version."
    fi

    new_firmware_version=`cat $firmware_version_file`
    echo "Final firmware version:" $new_firmware_version
    return $new_firmware_version
}

save_firmware() {
    # New firmware version should be the first argument.
    echo $firmware_version_file
    if [[ $# -ge 1 ]];
    then
        new_firmware_name=${FIRMWARE_NAME//version/$1}
    else
        new_firmware_name=${FIRMWARE_NAME//version/unkown}
    fi
    new_firmware_filepath=$TARGET_DIRECTORY/$new_firmware_name
    echo "====================================================================================="
    echo "Saving new firmware in $new_firmware_filepath"
    echo "====================================================================================="

    cp $FMK_DIR/$FMK_FIRMWARE_DIR/$FMK_OUTPUT_FIRMWARE $new_firmware_filepath
}

main() {
    update_firmware_version
    firmware_version=$?
    compile_firmware
    fix_checksum
    save_firmware $firmware_version
}

main
