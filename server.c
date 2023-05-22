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
#include <string.h>

#define BUFFER_SIZE 128 // Madhesia e buffer per lexim mesazhesh
#define MAX_CLIENTS_NR 50
int count = 0;

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
char* pergjigju_klientit(const char kerkesa[]);
void shto_klient(client_t *client);
void largo_klient(int clientId);


key_t server_id = 1234;
pthread_t tid[MAX_CLIENTS_NR];

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
void* thread_function(void* arg){
    client_t* klienti = (client_t*) arg;
    printf("Klienti - %d\n", klienti->client_id);
    shto_klient(klienti);
    klienti->message_queue_id = msgget(klienti->client_id, 0666|IPC_CREAT);
    printf("Queue - %d\n", klienti->message_queue_id);
    struct message_buffer message_pranim;
    struct message_buffer message_dergim;
    while(1){
        printf("Edhe qetu mir osht\n");
        msgrcv(klienti->message_queue_id, &message_pranim, sizeof(message_pranim.data_payload)+1, 1, 0);
        printf("Kerkesa - %s\n", message_pranim.data_payload);

        char* pergjigja = pergjigju_klientit(message_pranim.data_payload);
        message_dergim.request_type = 1;
        strcpy(message_dergim.data_payload, pergjigja);
        msgsnd(klienti->message_queue_id, &message_dergim, strlen(message_dergim.data_payload) + 1, 0);
        sleep(1);
        if (strcmp(message_pranim.data_payload, "0") == 0) {
            largo_klient(klienti->client_id);
            msgctl(klienti->message_queue_id, IPC_RMID, NULL);
            break;
        }
    }
    pthread_exit(NULL);
}

// Funksion i cili i pergjigjet klientit, varesisht nga kerkesa
char* pergjigju_klientit(const char kerkesa[]){ // TODO Rewrite this thing
    if (strcmp(kerkesa, "1") == 0) {
        return "Happy Birthday";
    }
    else if (strcmp(kerkesa, "2") == 0) {
        return "Nice day";
    }
    else if (strcmp(kerkesa, "3") == 0) {
        return "How are you";
    }
    else if (strcmp(kerkesa, "4") == 0) {
        return "Hello there";
    } 
    else if(strcmp(kerkesa, "0") == 0){
        return "exit";
    }
    else {
        return "Unknown";
    }
}

// Kerkesa 4 - Each message from a client should contain a request type and data payload.
//              The server should process each request and generate a response, which 
//              should be sent back to the client using its message queue

// Kerkesa 5 - When a client disconnects, its message queue should be removed from the list 
//              of connected clients and the corresponding thread should be terminated.

int main(){
    int msgid = msgget(server_id, 0666|IPC_CREAT); // Krijimi i nje message queue me anen e se ciles behet lidhja me server
    Connection_Request conn_request;
    while(1){
        if(count >= MAX_CLIENTS_NR){
            continue;
        }
        msgrcv(msgid, &conn_request, BUFFER_SIZE, 1, 0);
        printf("Hi3, %ld\n", conn_request.mtype);

        client_t client = conn_request.client;
        pthread_create(&(tid[count]), NULL, *thread_function, &client);
        count += 1;
    }
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}