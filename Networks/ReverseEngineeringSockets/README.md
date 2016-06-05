CS 60 Computer Networks

Lab2

Reverse Engineering Sockets: a puzzle

In this lab, we will build on our new found skills of sniffing packets using wireshark to reverse engineer a protocol that uses sockets. After this lab you will have gained additional skills of socket programming and you will get some insights into reverse engineering a protoco - this is all cool! We will learn the important skill of distributed programming using socket programming in C and the socket API. A socket is similar to the pipe abstraction not between two processes on the same machine but between process on different machines on the Internet.
You have been tasked you to rewrite an outdated sensor network system deployed at the Three Mile Island nuclear reactor. This particular sensor network is so outdated that the source code has since gone missing. But there is hope! Because this sensor network is still operational we can give you binary executable that the staff uses to monitor the various sensors. It is your task to reverse engineer the protocol these two components speak and rewrite the client in C.
____________________________________________________________________________
Luckily, we have executables for the more recent operating systems. You can download them below. Note, you should download the executable file using “save as a file” option. Once you done that make sure you have the right file permissions to execute the file; try“ chmod +x client-macos” under unix. Here are the binaries for Windows, Mac and Linux machines:
Windows Executable
Linux Executable
Intel Mac OS X Executable
If you have any problems contact the TA - when you run the program, you will see a prompt as follows:



  WELCOME TO THE THREE MILE ISLAND SENSOR NETWORK


  Which sensor would you like to read:

    (1) Water temperature
    (2) Reactor temperature
    (3) Power level

  Selection:

Your task is to run Wireshark at the same time and watch the packets going from your computer to the server(s) the executable it connecting and talking to. Sometimes it helps if you stop all other internet-related activity because it can be difficult to tell what the program is doing amongst a whole slew of other traffic. You’ll want to capture all the packets associated with each option since you’re are trying to faithfully reproduce the source code of the given executable. Note, that if you are unable to run the executable, contact the TA immediately and we’ll see what we can do. If worse comes to worse, the TA can provide you with a packet trace of the executable that you can use to reverse engineer the protocol. It is important that you reverse engineer the protocol before coding your client. If you are having trouble understanding or need clarification ask the TA.
Once you think you fully understand the protocol (it might be useful to draw a diagram outline the messages that need to be sent, etc.), start coding your C client. Your C client should be able to connect to the appropriate server(s), retrieve the sensor reading and output this to the user.
Extra hacker credit: This is hard so you may not want to do this. After you have reserved engineered the client and have it sucessfully working with the existing servers then write the server side too. You will have to use a different ports. Submit the client and server.
Good luck!
Submitting assignment: We are using SVN for the submission of assignments. Create a directory for each new lab (viz. lab1, lab2, lab3, lab3, lab5, lab6 and lab7). We will make a copy of the directory after the deadline.
Please make sure that each lab directory (e.g., lab2) contains a simple text file (called README) briefly describing the source code in the directory and anything “unusual” about how your solution should be located, executed, and considered. Essentially, your README gives us a quick overview of the content on the directory and how to run your programs. You also have to include a Makefile so we can build your programs.
Your svn repository root is at https://svn.cs.dartmouth.edu/classes/cs60-S12/yourreponame. For example: https://svn.cs.dartmouth.edu/classes/cs60-S12/campbell. Note, repo in yourreponame is short for respository.
But replace campbell with your repsoitory account username. You should have received this from Wayne or the TA. When you click on the svn URL above you will be asked to enter a username and password: enter your full DND (also blitz name) as your username and Blitz password as your password; DO NOT use your CS account name and password to login to the svn server - it will not work.
NOTE, for classes after 2012 this is relevant: change cs60-s12 to the correct year and term for example W13 changes the svn commands below to cs60-W13.
All detailed svn instruction is here:http://www.cs.dartmouth.edu/~campbell/cs50/svn.html.
Coding style: Please put comments in your code to help increase its understanding.
The final software should be include of the following components:

A Wireshark dump of the packets you analyzed to reverse engineer the protocol;
The reversed engineered client; and
Some example outputs of your program.
Again, you should also provide a Makefile to build your program (one for the client and one for the server) and a concise README file describing briefly your design choicesand how to run the program.
Good luck!
Tip: Make sure you always logout when you are done and see the prompt to login again before you leave the terminal.