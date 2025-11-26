nasm -f bin src/boot.asm -o build/boot.bin 
qemu-system-x86_64 -drive format=raw,file=build/boot.bin
