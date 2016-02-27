#!/usr/bin/python2

import serial
from termcolor import colored
import os
import re
from rpy2 import robjects

r=robjects.r
r.X11(5,5)
r.layout( r.matrix( robjects.IntVector( [1,2,3] ),nrow=3,ncol=1 ) )

try:
    uart_com=os.popen('''dmesg|grep "FTDI USB Serial Device converter now attached to"''').read()
    uart_com=re.findall('ttyUSB\d',uart_com)
    uart_com='/dev/'+uart_com[-1]

    while 1:
        i=0
        k=1

        uart_decode=""
        fore_time=[]
        fore_time_code=[]
        back_time=[]
        back_time_1=[]
        back_time_0=[]

        uart=serial.Serial( uart_com,57600,timeout=3 )
        while(1):
            if( uart.read() != '' ):
                print 'Recieving...'
                break
        uart.read();

        try:
            while 1:
                fore_time.append( round( (ord( uart.read() )*256 + ord( uart.read() )) ) )
                back_time.append( round( (ord( uart.read() )*256 + ord( uart.read() )) ) )

        except TypeError:
            uart.close()

            try:
                line_flag=0
                long_flag=0
                while i<len(fore_time)-1:
                    if fore_time[i]<2500:

                        if long_flag==0:
                            uart_decode+='\n'
                            long_flag+=1

                        if line_flag==0 and long_flag>0:
                            uart_decode+='   '
                            print

                        if back_time[i]<1000:
                            back_time_0.append( back_time[i] )
                            uart_decode+='0'
                            line_flag=(line_flag+1)%5

                        elif back_time[i]<2500 and back_time[i]>1000:
                            back_time_1.append( back_time[i] )
                            uart_decode+='1'
                            line_flag=(line_flag+1)%5

                        else:
                            uart_decode+='\n'
                            line_flag=0
                            print

                        fore_time_code.append( fore_time[i] )

                        print "%2d." % (k) , colored("%5d" % fore_time[i],'yellow'),
                        print colored("%5d" % back_time[i],'green'),


                        i+=1
                        k+=1

                    else:
                        if long_flag>0:
                            uart_decode+='\n'
                            print
                        print "%2d." % (i) , colored("%5d" % fore_time[i],'yellow'),
                        print colored("%5d" % back_time[i],'green'),
                        i+=1

            except IndexError:
                pass

            print uart_decode
            print

            # r.plot( robjects.IntVector(fore_time_code),xlab="sequence",ylab="fore_time" )
            # r.plot( robjects.IntVector(back_time_0),xlab="sequence",ylab="back_time_0" )
            # r.plot( robjects.IntVector(back_time_1),xlab="sequence",ylab="back_time_1" )

except OSError:
    print "No USB2Serial"
except KeyboardInterrupt:
    print
    print 'GoodBye!'
