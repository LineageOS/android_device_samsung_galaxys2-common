#!/system/bin/sh
echo "*************************************************"
echo "*** Build Magisk kernel for Samsung Galaxy S2 ***"
echo "*************************************************"
echo "WARNING: This script will update the device's kernel-partition /dev/block/mmcblk0p5."
echo "         When something goes wrong you need to restore this via ODIN."
echo ""
echo "INFO:    Magisk expects a valid boot.img in this partition which consist of zImage and ramdisk.cpio."
echo "         However our device only has the zImage directly and has the ramdisk.cpio embedded."
echo "         This script creates a valid boot.img based on fresh build ROM so Magisk can be installed via TWRP recovery."
echo "         After flashing Magisk, this script continues and extracts the ramdisk.cpio and rebuilds the kernel with Magisk modified ramdisk.cpio."
echo "         Finally it flashes /dev/block/mmcblk0p5 with the Magisk patched kernel."
echo ""
echo "IMPORTANT: This script needs a MicroSD-card."
echo "           Original /dev/block/mmcblk0p5 is written to /sdcard1/boot_orig.img"
echo "           Magisk reflashable file is written to /sdcard1/boot_magisk.img"
echo ""
echo "*** CONNECT YOUR PHONE AND MAKE SURE TWRP-RECOVERY IS RUNNING ***"
echo -n "OK to build and flash Magisk on your device (y/N)?"
read USERINPUT
case $USERINPUT in
 y|Y)
	echo "Backup /dev/block/mmcblk0p5 to /sdcard1/boot_orig.img..."
	cout
	adb shell dd if=/dev/block/mmcblk0p5 of=/sdcard1/boot_orig.img
	echo "Backup /dev/block/mmcblk0p5 to /sdcard1/boot_orig.img... Done!"
	echo ""

	echo "Creating valid bootimg for Magisk with ramdisk.cpio..."
	echo .>dummyKernel
	mkbootimg --kernel dummyKernel --ramdisk ramdisk.cpio -o boot.img
	adb push boot.img /dev/block/mmcblk0p5
	echo "Creating valid bootimg for Magisk with ramdisk.cpio... Done!"
	echo ""

	echo "Install Magisk in TWRP now!"
	echo "If you stop now, you need to flash /sdcard1/boot_orig.img before booting your phone!"
	read -p "Press [ENTER] to continue"
	echo ""

	echo "Extracting modified Magisk-ramdisk from /dev/block/mmcblk0p5..."
	adb shell dd if=/dev/block/mmcblk0p5 of=/sdcard1/boot.img.magisk
	adb pull /sdcard1/boot.img.magisk
	abootimg -x boot.img.magisk
	adb shell rm /sdcard1/boot.img.magisk
	rm ramdisk.cpio
	rm bootimg.cfg
	rm boot.img.magisk
	rm zImage
	mv initrd.img ramdisk.cpio
	echo "Extracting modified Magisk-ramdisk from /dev/block/mmcblk0p5... Done!"
	echo ""

	echo "Rebuilding kernel with Magisk modified ramdisk..."
	croot
        cp buildspec.mk buildspec.mk.org
	echo "WITH_MAGISKRAMDISK:=true" >>buildspec.mk
	mka bootimage
	echo "Rebuilding kernel with Magisk modified ramdisk... Done!"

	cout
	echo "Flashing Magisk-kernel to /dev/block/mmcblk0p5..."
	adb push boot.img /dev/block/mmcblk0p5
	echo "Flashing Magisk-kernel to /dev/block/mmcblk0p5... Done!"

	echo "Pushing flashable /sdcard1/boot_magisk.img..."
	adb push boot.img /sdcard1/boot_magisk.img
	echo "Pushing flashable /sdcard1/boot_magisk.img... Done!"
	echo ""
	croot
	rm buildspec.mk
	mv buildspec.mk.org buildspec.mk
	echo "*** You can now reboot your Magisk-enabled device. ***"
 ;;
 *)
	echo "Aborted"
 ;;
esac

