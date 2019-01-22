/**********COEN 233 Programming Assignment-1 ********************/
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
}ackPack1;

int main(){
    int client_socket,nBytes;    
    struct sockaddr_in serverAddress;
    socklen_t addr_size;
    int packet_no;
    int retry_counter;
    int case_no;
    char *error_string;
    //struct ack_packet ackPack1;

    /*Creating UDP Socket*/
    client_socket = socket(PF_INET, SOCK_DGRAM, 0);  

    /*Settings configuration in address struct*/
    serverAddress.sin_family = AF_INET;                                      //Address Family
    serverAddress.sin_port = htons(7891);                                    //Port Number
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");                  //localhost address
    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);     //Setting memory
    addr_size = sizeof serverAddress;                                       //Size variable                                         
    //Initial values
    ackPack1.rec_segment_number = 0;
    packet_no = 1;
    retry_counter = 0;                                 
    while(1){
        setbuf(stdout, NULL);
        struct data_packet dataPack1 = {0xffff,1,0xfff1,packet_no,5,"HELLO",0xffff};
        if(retry_counter==0){
            printf("\nCase1 - Normal Operation \nCase2 - Change in sequence \nCase3 - Alter the length of Payload \nCase4 - Missing End of packet \nCase5 - Duplicate packet \n\n\nEnter the case number to continue:");
		    scanf("%d",&case_no);    
    		switch(case_no){
                case 2: printf("Enter the packet number to be sent:");
                scanf("%"SCNu8,&dataPack1.segment_number);
                break;

                case 3: printf("Enter the new payload length:");
                scanf("%"SCNu8,&dataPack1.payload_length);
                break;

		        case 4: printf("You selected packet with missing end of packet identifier");
			    dataPack1.end_of_packet_id = 0xfff0;
                break;
		
		        case 5: printf("Enter the packet number to be sent:");
  			    scanf("%"SCNu8,&dataPack1.segment_number);
                break;
            }
        }
        // Details of the packet sent
        printf("\n*****OUTGOING PACKET*****\n\n-----START_OF_PACKET-------CLIENT_ID------TYPE---------SEGMENT_NUMBER---------LENGTH---------PAYLOAD-----------END_OF_PACKET----");
        printf("\n---------%x----------------%d------------%x--------------%d--------------------%d------------%s------------------%x-------",
                dataPack1.start_of_packet_id,dataPack1.client_id,dataPack1.type,dataPack1.segment_number,dataPack1.payload_length,dataPack1.payload,dataPack1.end_of_packet_id);
        nBytes = sizeof(dataPack1)+1;
        if(case_no){
            /*Send packet to the server*/  
            sendto(client_socket,&dataPack1,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
	    char* time_string1;
            time_t t1 = time(0);    //Packet sent time
            /*Setting timeout for 3 seconds*/
            struct timeval timeout={3,0}; 
            /* set receive UDP message timeout */
             setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
            /*Receive the message from server*/
            nBytes = recvfrom(client_socket,&ackPack1,1024,0,NULL, NULL); 
	    char* time_string2;
            time_t t2 = time(0);       //Packet received time
            if((nBytes>0)&&(difftime(t2, t1)<=3)&&(retry_counter<=3)){
                retry_counter = 0;
		time_string1 = ctime(&t1);
		time_string2 = ctime(&t2);
                printf("\n\nReceived Packet.....\nPacket Sent_Time:%s \nPacket Received_Time:%s",time_string1,time_string2);
                if(ackPack1.type == 0xfff2){
                    printf("\n*****ACK RECEIVED*****\n\n-------START_OF_PACKET-------CLIENT_ID--------TYPE----------REC_SEGMENT_NO----------END_OF_PACKET-------------------------");
                    printf("\n-----------%x------------------%d------------%x---------------%d------------------------%x----------------------------\n\n",
                    ackPack1.start_of_packet_id,ackPack1.client_id,ackPack1.type,ackPack1.rec_segment_number,ackPack1.end_of_packet_id);
                    packet_no = ackPack1.rec_segment_number+1;
                }
                else{
                    switch (ackPack1.reject_subcode){                
                        case 0xfff4:
                            error_string = "OUT OF SEQUENCE";
                            break;
                        case 0xfff5:
                            error_string = "PAYLOAD LENGTH MISMATCH";
                            break;
                        case 0xfff6:
                            error_string = "END OF PACKET MISSING";
                            break;
                        case 0xfff7:
                            error_string = "DUPLICATE PACKET";
                            break;
                    }
                    printf("\n\nREJECT REASON: %s",error_string);
                    printf("\n---START_OF_PACKET-------CLIENT_ID--------TYPE------------REJECT_CODE----------REC_SEGMENT_NO----------END_OF_PACKET---------");
                    printf("\n--------%x-----------------%d------------%x----------------%x--------------------%d----------------------%x------------\n\n",
                    ackPack1.start_of_packet_id,ackPack1.client_id,ackPack1.type,ackPack1.reject_subcode,ackPack1.rec_segment_number,ackPack1.end_of_packet_id);	
                    packet_no = ackPack1.rec_segment_number;	
                }
            }   
            else{
                if(retry_counter == 3){	   		
                        printf("\n\n*****SERVER DOES NOT RESPOND*****\n");
                        break;
                }
                printf("\n\n Packet not received. Retrying attempt:%d",++retry_counter);
            }
        }//End of case
    }//End of while
    return 0;
}
