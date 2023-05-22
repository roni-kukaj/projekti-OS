/*
    Pershkrimi i idese
        Te krijojme nje server i cili i pergjigjet kerkesave te klienteve
        Kerkesat e klienteve i marrim nga nje message queue, dhe secila ka nga nje type.
        Sipas tipit te kerkeses serveri e kthen nje pergjigje
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/msg.h>

#define BUFFER_SIZE 256 // Madhesia e buffer per lexim mesazhesh
#define MAX_CLIENTS_NR 50
int count = 0;

key_t server_id = 1234;
pthread_t tid[MAX_CLIENTS_NR];

typedef struct { // Connection file descriptor
    int client_id; // Id unike e klientit
    int message_queue_id; // Message Queue korresponduese e klientit
    int message_queue_key;
} client_t;

struct message_buffer{
    int request_type;
    char data_payload[BUFFER_SIZE];
} message;

// Kerkesa 1 - Initialize a message queue for accepting connections from clients

client_t *clients[MAX_CLIENTS_NR]; // Liste e klienteve
pthread_mutex_t mutex_client = PTHREAD_MUTEX_INITIALIZER; // mutex lock per procesim te klienteve
pthread_mutex_t mutex_message = PTHREAD_MUTEX_INITIALIZER; // mutex lock per procesim mesazhesh

// Kerkesa 2 - Listen for connections from clients and add them to a list of connected clients

// Funksion qe sherben per shtimin e nje klienti ne queue
void shto_klient(client_t *client){
    pthread_mutex_lock(&mutex_client); // Hyrje ne critical section per shtimin e klientit ne queue

    for(int i = 0; i < MAX_CLIENTS_NR; ++i){
        if(!clients[i]){
            clients[i] = client; // Vendos klientin ne poziten e pare te lire
            break;
        }
    }

    pthread_mutex_unlock(&mutex_client); // Dalje nga critical section kur perfundon shtimi i klientit
}

// Funksion qe sherben per largimin e nje klienti nga queue
void largo_klient(int clientId){
    pthread_mutex_lock(&mutex_client); // Hyrje ne critical section per largimin e klientit nga queue

    for(int i = 0; i < MAX_CLIENTS_NR; ++i){
        if(clients[i]->client_id== clientId){
            clients[i] = NULL; // Largo klientin sipas id-se
            break;
        }
    }

    pthread_mutex_unlock(&mutex_client); // Dalje nga critical section pas largimit
}


// Kerkesa 3 - For each connected client, spawn a new thread that listens for messages from that client 
//              and processes them

// Funksioni i cili ekzektuohet kur krijohet nje thread i ri - vjen request nga klienti
void *thread_function(client_t *klienti){
    shto_klient(klienti);
    while(1){
        if(msgrcv(klienti->message_queue_id, &message, sizeof(message)+1, 1, 0) == -1) continue;
        if(message.request_type == 0){
            largo_klient(klienti->client_id);
            pthread_exit(NULL);
            break;
        }
        char *pergjigja = pergjigju_klientit(message.request_type);
        msgsnd(klienti->message_queue_id, *pergjigja, sizeof(*pergjigja) + 1, 0);
    }
}

// Funksion i cili i pergjigjet klientit, varesisht nga kerkesa
char *pergjigju_klientit(int kerkesa){
    char *str = malloc(BUFFER_SIZE);
    if(kerkesa == 1){
        *str = "Hello, 1";
    }
    else if(kerkesa == 2){
        *str = "Hello, 2";
    }
    else if(kerkesa == 3){
        *str = "Hello, 3";
    }
    return *str;
}

// Kerkesa 4 - Each message from a client should contain a request type and data payload.
//              The server should process each request and generate a response, which 
//              should be sent back to the client using its message queue

// Kerkesa 5 - When a client disconnects, its message queue should be removed from the list 
//              of connected clients and the corresponding thread should be terminated.

int main(){
    int msgid = msgget(server_id, 0666|IPC_CREAT);
    client_t client;
    while(1){
        if(count >= MAX_CLIENTS_NR) continue;
        if(msgrcv(msgid, &client, sizeof(client) + 1, 1, 0) == -1) continue;
        client.message_queue_id = msgget(client.message_queue_key, 0666|IPC_CREAT);
        pthread_create(&(tid[count]), NULL, &thread_function, &client);
        count += 1;
    }
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}