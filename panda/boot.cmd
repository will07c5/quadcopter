fatload mmc 0 0x90000000 m3-boot.bin
fatload mmc 0 0x9e100000 m3-image.bin
bootm 0x90000000
