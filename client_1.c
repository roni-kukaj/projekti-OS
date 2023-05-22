#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/msg.h>
#include <string.h>

#define BUFFER_SIZE 128

typedef struct { // Connection file descriptor
    int client_id; // Id unike e klientit
    int message_queue_id; // Message Queue korresponduese e klientit
} client_t;

typedef struct{
    long mtype; // Message type
    client_t client;
} Connection_Request;

struct message_buffer {
    long request_type;
    char data_payload[BUFFER_SIZE];
};

int main(){
    long server_id;
    printf("Welcome client!\n");
    begin:
    printf("To make a connection with the server enter the server key - ");
    scanf("%ld", &server_id);

    printf("You entered: %ld\n", server_id);
    int msgid = msgget((key_t)server_id, IPC_EXCL); // Ketu behet lidhja klient-server nese serveri eshte running
    if(msgid == -1){
        printf("This server does not exist!\n");
        goto begin;
    } else {
        Connection_Request conn_request;
        client_t client_;
        client_.client_id = getpid();
        client_.message_queue_id = 0;

        conn_request.mtype = 1;
        conn_request.client = client_;

        msgsnd(msgid, &conn_request, sizeof(Connection_Request) - sizeof(long), 0);
        sleep(3);
        int client_queue = msgget((key_t)conn_request.client.client_id, IPC_EXCL);
        if(client_queue == -1){
            printf("The queue could not be created!");
            goto begin;
        }

        printf("Connection has been established!\n");

        printf("\n=== Now let's begin the request ===\n");

        char msg[BUFFER_SIZE];        
        while(1){
            printf("Type 0 - To end\n");
            printf("Type 1 - It's good\n");
            printf("Type 2 - It's good\n");
            printf("Type 3 - It's good\n");
            printf("Type 4 - It's good\n");
            printf("Your request >>> ");
            scanf("%s", msg);
            struct message_buffer mesazhi_dergim;
            mesazhi_dergim.request_type = 1;
            strcpy(mesazhi_dergim.data_payload, msg);
            msgsnd(client_queue, &mesazhi_dergim, strlen(mesazhi_dergim.data_payload)+1, 0);
            sleep(1);

            struct message_buffer mesazhi_pranim;
            msgrcv(client_queue, &mesazhi_pranim, BUFFER_SIZE, 1, 0);
            printf("Data received ---> %s\n", mesazhi_pranim.data_payload);
            if(strcmp(mesazhi_pranim.data_payload, "exit") == 0){
                break;
            }
        }
    }
    
    
}