# Installation Guide for Kernel Module

This guide provides instructions on how to install the kernel module using TWRP (Team Win Recovery Project).

## Prerequisites

- Ensure you have TWRP installed on your device.
- Download the kernel module ZIP file.

## Steps to Install Kernel Module

1. **Boot into TWRP Recovery**:
    - Power off your device.
    - Press and hold the Volume Down + Power buttons simultaneously to boot into TWRP recovery mode.

2. **Transfer ZIP File**:
    - Connect your device to your computer using a USB cable.
    - Transfer the downloaded kernel module ZIP file to your device's internal storage.

3. **Install the ZIP File**:
    - In TWRP, tap on the `Install` button.
    - Navigate to the location where you transferred the ZIP file.
    - Select the ZIP file and swipe to confirm the installation.

4. **Reboot Your Device**:
    - Once the installation is complete, tap on the `Reboot System` button.

## Verifying Installation

After your device reboots, you can verify if the kernel module is installed and functioning correctly:

1. Open a terminal or command prompt on your device.
2. Check the installed kernel modules using the following command:
    ```sh
    lsmod
    ```

If the kernel module appears in the list, the installation was successful.

## Troubleshooting

- If the installation fails, ensure that the ZIP file is not corrupted and try the installation process again.
- Make sure that your device's bootloader is unlocked.

For any further assistance, feel free to open an issue on the repository.

---

By following these steps, you should be able to successfully install the kernel module using TWRP flash. If you encounter any issues, please refer to the troubleshooting section or seek help from the community.
