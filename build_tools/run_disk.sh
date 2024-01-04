# Set the paths and filenames
ISO_PATH="build/os_image.img"
GRUB_FOLDER="build/CordellOS"
EMPTY_SPACE_FILE="empty.img"
COMBINED_ISO="build/final_os_image.img"

# Create the GRUB configuration file (grub.cfg) if it doesn't exist
if [ ! -f "${GRUB_FOLDER}/boot/grub/grub.cfg" ]; then
    echo "Create your GRUB configuration file (${GRUB_FOLDER}/boot/grub/grub.cfg) before running this script."
    exit 1
fi

# Create the ISO image if it doesn't exist
if [ ! -f "${ISO_PATH}" ]; then
    grub2-mkrescue -o "${ISO_PATH}" "${GRUB_FOLDER}/"
else
    echo "ISO image already exists at ${ISO_PATH}. Skipping creation."
fi

# Create additional empty space file if it doesn't exist
if [ ! -f "${EMPTY_SPACE_FILE}" ]; then
    dd if=/dev/zero of="${EMPTY_SPACE_FILE}" bs=1M count=100
else
    echo "Empty space file already exists at ${EMPTY_SPACE_FILE}. Skipping creation."
fi

# Combine the GRUB ISO and empty space
if [ ! -f "${COMBINED_ISO}" ]; then
    cat "${ISO_PATH}" "${EMPTY_SPACE_FILE}" > "${COMBINED_ISO}"
else
    echo "Combined ISO image already exists at ${COMBINED_ISO}. Skipping creation."
fi

# Run QEMU with the combined ISO image
qemu-system-i386 -debugcon stdio -hda "${COMBINED_ISO}" -vga std -d int

