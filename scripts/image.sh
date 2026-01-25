#!/bin/bash
set -e

# --- Configuration ---
MODE="$1"
DISK_BIN="$2"
MOUNT_DIR="$3"
FS_DIR="$4"
USERLAND_DIR="$5"
KERNEL_BIN="$6"
EXTRA_BIN="$7"

# Global variable for loop device
LOOP_DEV=""

cleanup() {
    echo "Cleaning up"
    # Runs on exit or error
    if mountpoint -q "$MOUNT_DIR"; then
        sudo umount "$MOUNT_DIR"
    fi
    if [ -n "$LOOP_DEV" ]; then
        sudo losetup -d "$LOOP_DEV"
    fi
}
trap cleanup EXIT

create_disk() {
    echo "--- Creating Disk Image ---"
    rm -f "$DISK_BIN"
    # 1. Create 32MB blank image
    dd if=/dev/zero of="$DISK_BIN" bs=1M count=32 status=none

    # 2. Create MBR
    parted -s "$DISK_BIN" mklabel msdos
    # 3. Create FAT16 partition
    parted -s "$DISK_BIN" mkpart primary fat16 1MiB 100%
}

setup_loop() {
    echo "--- Setting up Loop Device ---"

    # 1. Create loop device
    LOOP_DEV=$(sudo losetup -f --show -P "$DISK_BIN")
    
    # Wait for partition node to appear
    if [ ! -e "${LOOP_DEV}p1" ]; then
        sleep 1
    fi
    
    # Fail fast if partition didn't appear
    if [ ! -e "${LOOP_DEV}p1" ]; then
        echo "Error: Partition ${LOOP_DEV}p1 not found."
        exit 1
    fi
}

mount_fs() {
    local PARTITION="${LOOP_DEV}p1"
    echo "--- Formatting & Mounting ---"
    
    # 1. Format partition
    sudo mkfs.vfat -F 16 "$PARTITION" > /dev/null
    
    # 2. Mount partition
    mkdir -p "$MOUNT_DIR"
    sudo mount "$PARTITION" "$MOUNT_DIR"
}

copy_files() {
    echo "--- Copying Files ---"
    
    # 1. Copy Filesystem Root (Contains boot/grub/grub.cfg)
    if [ -d "$FS_DIR" ]; then
        # Copy contents of FS_DIR to Root of mount
        sudo cp -r "$FS_DIR"/. "$MOUNT_DIR"/.
    fi

    # 2. Copy Userland
    if [ -d "$USERLAND_DIR" ]; then
        sudo cp -r "$USERLAND_DIR"/. "$MOUNT_DIR"/.
    fi
}

install_grub() {
    echo "--- Installing GRUB ---"
    
    # 1. Copy Kernel
    sudo cp "$KERNEL_BIN" "$MOUNT_DIR"/

    # 2. Install GRUB
    sudo grub-install --target=i386-pc \
         --boot-directory="$MOUNT_DIR/boot" \
         --modules="part_msdos fat normal multiboot" \
         "$LOOP_DEV" 2>/dev/null
}

install_gitboot() {
    echo "--- Installing Gitboot ---"
    local STAGE1="$1"
    echo "Gitboot binary: $STAGE1"

    if [ -z "$STAGE1" ]; then
        echo "Error: Gitboot requires stage1 binary."
        exit 1
    fi

    # 0. Unmount FS before raw writing to prevent corruption/caching issues
    sudo umount "$MOUNT_DIR"

    # 1. Write Stage 1 to MBR (Bytes 0-446)
    # Preservation of partition table is handled by count=1 bs=446
    sudo dd if="$STAGE1" of="$LOOP_DEV" bs=446 count=1 seek=0 status=none

    # 2. Write Kernel to hidden sectors (Sector 1 to 2047)
    # Partition 1 starts at 1MiB (Sector 2048), so we have space.
    sudo dd if="$KERNEL_BIN" of="$LOOP_DEV" bs=512 seek=1 status=none
}

if [[ "$MODE" != "grub" && "$MODE" != "gitboot" ]]; then
    echo "Usage: $0 [grub|gitboot] [disk] [mount] [fs] [userland] [kernel] [bootloader]"
    exit 1
fi

create_disk
setup_loop
mount_fs
copy_files

if [ "$MODE" == "grub" ]; then
    install_grub
elif [ "$MODE" == "gitboot" ]; then
    install_gitboot "$EXTRA_BIN"
fi

echo "Success."
