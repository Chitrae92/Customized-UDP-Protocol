COEN 233 - PROGRAMMING ASSIGNMENT 2 
NAME:CHITRA ELANGOVAN
ID: W1284522

Initial Assumptions: 
The client ID is 1 
The payload length is set to 10 since the subscriber number consists of 10 digits
The default value for technology is 02 and source subscriber number is 4089999999.

Steps:
* There are two C files udp_client2.c and udp_server2.c containing the client and server code.
* The files are compiled by opening two separate terminals each for client and server.
* Move to the directory containing the files using cd command.
* The command used for compilation are as follows:
	Client : gcc udp_client2.c -o udp_client2.out
	Server : gcc udp_server2.c -o udp_server2.out
* Two output files udp_client1.out and udp_server1.out are generated.
* After compilation executable is generated and we run the generated executable using below command
	Client : ./udp_client2.out
	Server : ./udp_server2.out
* The client displays the following message 
Alter the source subscriber number and Technology (0-No or 1-Yes) ? 

* Enter 1 if you would like to change the default value of source and technology else enter 0
* The program then executes based on our input choice and client sends the packet to the server
* The server verifies the contents of the packet with the contents of the text file "Verification_Database" and sends ACK with corresponding messages
* The message is received by the client and displayed
* If the server doesnot respond within 3 seconds the packet is resent upto 3 times after which an error message "Server does not respond" is displayed. (This is tested by closing the server terminal)


Codes used:
Access Permission - 0xfff8
Bill not paid - 0xfff9
Bill paid (Access ok) - 0xfffB
Subscriber doesnot exist (Either subscriber number not found or if found(technology doesnot match))- 0xfffA





