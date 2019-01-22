/**********COEN 233 Programming Assignment-1 ********************/
/**********User Datagram Protocol(UDP) Server********************/
/* Name - Chitra Elangovan , Student ID - W1284522 */
#include <stdio.h>              // Standard input/output functions
#include <sys/socket.h>         //Internet Protocol Family
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h> 
#include <stdlib.h>

//Data Packet format
struct data_packet{
    uint16_t start_of_packet_id;    //2 Bytes
    uint8_t client_id;              //1 Byte
    uint16_t type;                  //2 Bytes
    uint8_t segment_number;         //1 Byte
    uint8_t payload_length;         //1 Byte
    char payload[255];              //255 Bytes
    uint16_t end_of_packet_id;      //2 Bytes
};

//Combined Acknowledge & Reject Packet format
struct ack_packet{
    uint16_t start_of_packet_id;    //2 Bytes
    uint8_t client_id;              //1 Byte
    uint16_t type;                  //2 Bytes
    uint16_t reject_subcode;        //2 Bytes
    uint8_t rec_segment_number;     //1 Byte
    uint16_t end_of_packet_id;      //2 Bytes
};

int main(){
    int udp_socket,nBytes;
    struct sockaddr_in serverAddress;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    struct data_packet dataPack2;
    uint16_t error_flag = 0;
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
    while(1){
        setbuf(stdout, NULL);    
        struct ack_packet server_ackPack = {0xffff,1,0xfff2,0,0,0xffff};
        /* Trying to receive any incoming UDP datagram. Address and port of 
        requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(udp_socket,&dataPack2,1024,0,(struct sockaddr *)&serverStorage, &addr_size);
        // Printing details of the incoming packet
        printf("\n\n*****INCOMING PACKET*****\n\n-----START_OF_PACKET-------CLIENT_ID------TYPE---------SEGMENT_NUMBER---------LENGTH---------PAYLOAD-----------END_OF_PACKET----");
        printf("\n---------%x----------------%d------------%x--------------%d--------------------%d------------%s------------------%x-------",
                dataPack2.start_of_packet_id,dataPack2.client_id,dataPack2.type,dataPack2.segment_number,dataPack2.payload_length,dataPack2.payload,dataPack2.end_of_packet_id);
        error_flag=0;
        printf("\n\nIncoming packet:%d Expected packet:%d \n",dataPack2.segment_number,packet_no);
        if(packet_no >= dataPack2.segment_number){
            if(dataPack2.payload_length != strlen(dataPack2.payload)){
                //Sending REJECT for mismatch in length of Payload
                error_flag=0xfff5;
		    }                
		    else if(!((dataPack2.end_of_packet_id == 0xffff)&&(dataPack2.end_of_packet_id))){
                //Sending REJECT for wrong or missing end_of_packet_id
                error_flag=0xfff6;
		    }                
            else if(packet_no > dataPack2.segment_number){
                //Sending REJECT for Duplicate packet
                error_flag=0xfff7;
		    }
		    else{
                printf("\n*****Sending ACKNOWLEDGEMENT*****\n");
                error_flag=0;
                //Sending ACK for that packet
                server_ackPack.rec_segment_number = dataPack2.segment_number;
                printf("\n---START_OF_PACKET-------CLIENT_ID--------TYPE----------REC_SEGMENT_NO----------END_OF_PACKET--------------------------");
                printf("\n--------%x-----------------%d------------%x-----------------%d----------------------%x-----------------------------",
                server_ackPack.start_of_packet_id,server_ackPack.client_id,server_ackPack.type,server_ackPack.rec_segment_number,server_ackPack.end_of_packet_id);
                sendto(udp_socket,&server_ackPack,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
                packet_no += 1;
            }
        }
        else{
            //Sending REJECT for out of sequence
   		    error_flag=0xfff4;
	    }
        if(error_flag){
            printf("\n\nSending REJECT : %x",error_flag);
		    struct ack_packet server_ackPack = {0xffff,1,0xfff3,error_flag,dataPack2.segment_number,0xffff};
		    printf("\n---START_OF_PACKET-------CLIENT_ID--------TYPE------------REJECT_SUBCODE----------REC_SEGMENT_NO----------END_OF_PACKET------");
            printf("\n--------%x----------------%u-------------%x----------------%x-----------------------%u---------------------%x----------",
            server_ackPack.start_of_packet_id,server_ackPack.client_id,server_ackPack.type,server_ackPack.reject_subcode,server_ackPack.rec_segment_number,server_ackPack.end_of_packet_id);	
		    sendto(udp_socket,&server_ackPack,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
	    }     
    }
  return 0;
}



 
