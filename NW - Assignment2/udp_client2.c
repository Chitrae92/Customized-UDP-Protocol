/**********COEN 233 Programming Assignment-2 ********************/
/**********User Datagram Protocol(UDP) Client********************/
/* Name - Chitra Elangovan , Student ID - W1284522 */
#include <stdio.h>              // Standard input/output functions
#include <sys/socket.h>         //Internet Protocol Family
#include <netinet/in.h>         //Socket address datatypes
#include <time.h>           //Time options
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h> 
#include <inttypes.h>

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
    int client_socket,nBytes;    
    struct sockaddr_in serverAddress;
    socklen_t addr_size;
    int packet_no;
    int retry_counter;
    char *error_string;
    int alter_packet;
    struct ack_packet ackPack1;

    /*Creating UDP Socket*/
    client_socket = socket(PF_INET,SOCK_DGRAM,0);  

    /*Settings configuration in address struct*/
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(7891);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");                  //localhost address
    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);     //Setting memory
    addr_size = sizeof serverAddress;                                       //Size variable                                         
    //Initial values
    ackPack1.rec_segment_number = 0;
    packet_no = 1;
    retry_counter = 0;       
    alter_packet = 0;                          
    while(1){
        setbuf(stdout, NULL);
        fflush(stdout);
        struct data_packet dataPack1 = {0xffff,1,0xfff8,packet_no,10,02,4089999999,0xffff};
        if(retry_counter==0){

            printf("\n\nAlter the source subscriber number and Technology (0-No or 1-Yes) ? ");
            scanf("%d",&alter_packet);

            if(alter_packet) {
                printf("\nEnter the source subscriber number: ");
                scanf("%" SCNu32,&dataPack1.source_subscriber_number);

                printf("\nEnter the Technology: ");
                scanf("%" SCNu8,&dataPack1.technology);
            }
        }
        // Details of the packet sent
        printf("\n**********OUTGOING PACKET**********\n\n-----START_OF_PACKET-------CLIENT_ID------ACCESS_PERMISSION---------SEGMENT_NUMBER---------LENGTH---------TECHNOLOGY------SUBSCRIBER_NUMBER-----END_OF_PACKET----");
        printf("\n----------%x----------------%u-----------------%x-----------------------%u------------------%u--------------%u---------------%u---------------%x------",
                dataPack1.start_of_packet_id,dataPack1.client_id,dataPack1.acc_permission,dataPack1.segment_number,dataPack1.length,dataPack1.technology,dataPack1.source_subscriber_number,dataPack1.end_of_packet_id);
        nBytes = sizeof(dataPack1) + 1;
        if(alter_packet!=-1){
            /*Send packet to the server*/  
            sendto(client_socket,&dataPack1,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
	    char* time_string1;
            time_t t1 = time(0);    //Packet sent time
            /*Setting timeout for 3 seconds*/
            struct timeval timeout={3,0}; 
            /* set receive UDP message timeout */
             setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
            /*Receive message from server*/
            nBytes = recvfrom(client_socket,&ackPack1,1024,0,NULL, NULL); 
            char* time_string2;
	    time_t t2 = time(0);       //Packet received time
            if((nBytes>0)&&(difftime(t2, t1)<=3)&&(retry_counter<=3)){
                retry_counter = 0;
		time_string1 = ctime(&t1);
		time_string2 = ctime(&t2);
                printf("\n\nReceived Packet.....\nPacket Sent_Time:%s \nPacket Received_Time:%s",time_string1,time_string2);
                if(ackPack1.user_status == 0xfff9)
                    error_string = "User has not paid bill";
                else if(ackPack1.user_status == 0xfffA)
                    error_string = "User doesnot exist";
                else if(ackPack1.user_status == 0xfffB)
                    error_string = "User has paid the bill - Access OK";
                printf("\n******ACK: %s******",error_string);
                printf("\n\n-------START_OF_PACKET-------CLIENT_ID--------STATUS--------REC_SEGMENT_NUMBER--------LENGTH--------TECHNOLOGY---------SUBSCRIBER_NUMBER---------END_OF_PACKET-----");
                printf("\n-----------%x------------------%u-------------%x----------------%u--------------------%u--------------%u-----------------%u-----------------%x----------",
                       ackPack1.start_of_packet_id,ackPack1.client_id,ackPack1.user_status,ackPack1.rec_segment_number,ackPack1.length,ackPack1.technology,ackPack1.source_subscriber_number,ackPack1.end_of_packet_id);
                packet_no=ackPack1.rec_segment_number+1;            	
            }   
            else{
                if(retry_counter == 3){	   		
                        printf("\n\n/*****SERVER DOES NOT RESPOND*****/\n");
                        break;
                }                
                printf("\n\n Packet not received. Retrying attempt:%d",++retry_counter);
            }
        }//End of If
    }//End of while
    return 0;
}
