Currently, in order to compile the kernel:

    make x86_64-obj/kernel.elf
    
If you require debugging symbols (for use with `gdb`) in the compilation:

    DEBUG=1 make x86_64-obj/kernel.elf
    
To build the UEFI bootloader:

    make uefi.efi
    
You will need to place this on a UEFI disk.  See `./bin/mkuefidisk` for hints on how to do that.  Once you've done that, you can 
mount and unmount the disk using `./bin/mnt-uefi` and `./bin/umnt-uefi`.  The script `./bin/assemble` puts these steps together.

To run the image on qemu, see `./bin/start-qemu`.  If this script is passed the `-g` flag, then you can attach `gdb` to the running
kernel.  After starting `gdb`, in this case, do this in the `gdb` shell:

    file /path/to/git/for/the-k-os/x86_64-obj/kernel.elf
    target remote localhost:1234
    
At present, the bootloader isn't compiled/compilable with debugging symbols, so `kmain()` will be your entry point.

Once you've started `qemu`, you can attach a VNC console.  If you use the Mac-native method to connect to `vnc`, you must set a
password for the VNC session.  Do so by typing this in the `qemu` monitor shell:

    change vnc password
    
