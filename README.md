# Customized-UDP-Protocol
### ASSIGNMENT 1 

Initial Assumptions: 
The client ID is 1 
The payload sent is "HELLO" which is of length 5
The segment number of the packets increases sequentially for each packet sent (correct order)

Steps:
* There are two C files udp_client1.c and udp_server1.c containing the client and server code.
* The files are compiled by opening two separate terminals each for client and server.
* Move to the directory containing the files using cd command.
* The command used for compilation are as follows:
	Client : gcc udp_client1.c -o udp_client1.out
	Server : gcc udp_server1.c -o udp_server1.out
* Two output files udp_client1.out and udp_server1.out are generated.
* After compilation executable is generated and we run the generated executable using below command
	Client : ./udp_client1.out
	Server : ./udp_server1.out
* The client displays the following message 
	Case1 - Normal Operation
	Case2 - Change in sequence
	Case3 - Alter the length of Payload
	Case4 - Missing End of packet
	Case5 - Duplicate packet

	Enter the case number to continue:

* Enter the case number corresponding to our choice of operation
* The program then executes based on our input choice and client sends the packet to the server.
* The server sends back ACK if received correctly or sends reject code incase of errors.
* If the server doesnot respond within 3 seconds the packet is resent upto 3 times after which an error message "Server does not respond" is displayed. (This is tested by closing the server terminal)

Types
Data-0xfff1
Ack-0xfff2
Reject-0xfff3

Reject Codes
Case2 - Out of Sequence - 0xfff4
Case3 - Length mismatch - 0xfff5
Case4 - End of packet missing - 0xfff6
Case5 - Duplicate packet - 0xfff7

### ASSIGNMENT 2 

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
