/**********COEN 233 Programming Assignment-2 ********************/
/**********User Datagram Protocol(UDP) Server********************/
/* Name - Chitra Elangovan , Student ID - W1284522 */
#include <stdio.h>              // Standard input/output functions
#include <sys/socket.h>         //Internet Protocol Family
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h> 
#include <stdlib.h>

//Access permission request packet format
struct data_packet{
    uint16_t start_of_packet_id;        //2 Bytes
    uint8_t client_id;                  //1 Byte
    uint16_t acc_permission;            //2 Bytes
    uint8_t segment_number;             //1 Byte
    uint8_t length;                     //1 Byte
    uint8_t technology;			        //1 Byte
    uint32_t source_subscriber_number;	//4 Bytes
    uint16_t end_of_packet_id;          //2 Bytes
};

//Subscriber status message format
struct ack_packet{
    uint16_t start_of_packet_id;        //2 Bytes
    uint8_t client_id;                  //1 Byte
    uint16_t user_status;               //2 Bytes
    uint8_t rec_segment_number;         //1 Byte
    uint8_t length;                     //1 Byte
    uint8_t technology;			        //1 Byte
    uint32_t source_subscriber_number;	//4 Bytes
    uint16_t end_of_packet_id;          //2 Bytes
};

int main(){
    int udp_socket,nBytes;
    struct sockaddr_in serverAddress;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    struct data_packet dataPack2;
    int access_code;
    unsigned int packet_no = 1;
    /*Creating UDP Socket*/
    udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
    /*Settings configuration in address struct*/ 
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(7891);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");                     //localhost address
    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);        //Setting memory
    /*Binding socket with an address struct*/ 
    bind(udp_socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    addr_size = sizeof serverStorage;                                           //Size variable
    struct ack_packet server_ackPack = {0xffff,1,0xfffb,0,10,0,0,0xffff};
    while(1){
        setbuf(stdout, NULL); 
        fflush(stdout);
        
        /* Trying to receive any incoming UDP datagram. Address and port of 
        requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(udp_socket,&dataPack2,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
        printf("\n\nIncoming packet:%d Expected packet:%d \n",dataPack2.segment_number,packet_no);
        if(packet_no >= dataPack2.segment_number){
             /*Creating file pointer and opening the file in read mode*/
            FILE *fp;
            fp = fopen("Verification_Database.txt","r");
            if(fp == NULL)
            {
                //if fp file pointer is NULL, then exit the program with an error message displayed.
                printf("Error in reading the file at the specified location. Retry with the correct location");
                exit(1);
            }
            else {
                printf("\n*****INCOMING PACKET FROM CLIENT*****\n\n---START_OF_PACKET-------CLIENT_ID--------ACCESS----------SEGMENT_NUMBER---------LENGTH-----TECHNOLOGY---------SUBSCRIBER_NUMBER-----------END_OF_PACKET-----------");
                printf("\n-------%x------------------%u-------------%x------------------%u-----------------%u-----------%u------------------%u-----------------%x-----------------",
                       dataPack2.start_of_packet_id,dataPack2.client_id,dataPack2.acc_permission,dataPack2.segment_number,dataPack2.length,dataPack2.technology,dataPack2.source_subscriber_number,dataPack2.end_of_packet_id);

                char line[256],*word;      
                int flag = 0;
                while (fgets(line,sizeof(line),fp) != NULL) {	/*Read line by line from the text file*/
                    int index = 1;
                    printf("\nData content = %s\n", line);   	/*Print the contents in each line of the file*/
                    word = strtok(line," ");		  	        /* Splitting the line contents word by word using space as delimiter */
                    while(word != NULL) {
                        /* For each line in the file first compare the source subscriber number and if they match, compare the technology followed by payment status(paid/not paid) */
                        /* Index = 1 (SOURCE SUBSCRIBER NUMBER)
                           Index = 2 (TECHNOLOGY)
                           Index = 3 (PAYMENT STATUS - PAID 1 /NOT PAID 0)  */
                        if(index == 1) {
                            if(dataPack2.source_subscriber_number==atol(word)) {
                                printf("SOURCE SUBSCRIBER NUMBER MATCHES\n");
                                flag = 1;
                            }
                            else {
                                flag = -1;
                                break;              //If subscriber number doesnot match exit the loop and proceed to the next line
                            }
                        }
                        else if(index == 2) {
                            if(dataPack2.technology == atoi(word)) {
                                printf("TECHNOLOGY MATCHES\n");
                                flag = 1;
                            }
                            else {
                                printf("TECHNOLOGY DOES NOT MATCH\n");
                                flag = -2;
                                break;
                            }
                        }
                        else if(index == 3) {
                            if(atoi(word)) {
                                printf("SUBSCRIBER PAYMENT STATUS : PAID\n");
                                flag = 99;
                                break;
                            }
                            else {
                                printf("SUBSCRIBER PAYMENT STATUS : NOT PAID\n");
                                flag=-99;
                                break;
                            }
                        }
                        word = strtok(NULL," "); 		//Read the remaining contents of the line (word by word)
                        index += 1;
                    }

                    if((flag==-2)||(flag == 99)||(flag == -99)) {                        					
                        if(flag == -2) {
                            access_code = 0xfffa;       //Access code - 0xfffa (Subscriber found but technology doesn't match)
                        }
                        else if(flag == 99) {
                            access_code = 0xfffb;       //Access code - 0xfffb (Subscriber has paid the bill)
                        }
                        else if(flag == -99) {
                            access_code = 0xfff9;       //Access code - 0xfff9 (Subscriber has not paid the bill)
                        }
                        break;
                    }
                }
                if(flag == -1){  				        // Flag value -1 indicates that the subscriber does not exist in the database
                    access_code = 0xfffa;
		            printf("SUBSCRIBER NUMBER NOT FOUND\n");		
		        }	        				
            }
            fclose(fp);					               //Closing the file pointer

            printf("\n\n******Sending ACK******\n");
            server_ackPack.user_status=access_code;
            server_ackPack.rec_segment_number = dataPack2.segment_number;            
            server_ackPack.technology = dataPack2.technology;
            server_ackPack.source_subscriber_number = dataPack2.source_subscriber_number;
            printf("\n---START_OF_PACKET-------CLIENT_ID--------STATUS----------REC_SEGMENT_NUMBER--------LENGTH------TECHNOLOGY----------SUBSCRIBER_NUMBER-------------END_OF_PACKET-------");
            printf("\n-------%x------------------%u-------------%x------------------%u--------------------%u------------%u------------------%u---------------------%x------------",
                   server_ackPack.start_of_packet_id,server_ackPack.client_id,server_ackPack.user_status,server_ackPack.rec_segment_number,server_ackPack.length,server_ackPack.technology,
                   server_ackPack.source_subscriber_number,server_ackPack.end_of_packet_id);
            sendto(udp_socket,&server_ackPack,nBytes,0,(struct sockaddr *)&serverStorage,addr_size); //Sending the ACK to client
            packet_no += 1;
        } // End of If
    } // End of While
    return 0;
}
