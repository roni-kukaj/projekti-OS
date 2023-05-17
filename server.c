#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

#define BUFFER_SIZE 2048 // Madhesia e buffer per lexim mesazhesh
#define MAX_CLIENTS_NR 50

typedef struct {
    int connection; // Connection file descriptor
    int client_id; // Id unike e klientit
    char name[50]; // Emri i klientit
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
void largo_klient(int client_id){
    pthread_mutex_lock(&mutex_client); // Hyrje ne critical section per largimin e klientit nga queue

    for(int i = 0; i < MAX_CLIENTS_NR; ++i){
        if(clients[i]->client_id == client_id){
            clients[i] = NULL; // Largo klientin sipas id-se
            break;
        }
    }

    pthread_mutex_unlock(&mutex_client); // Dalje nga critical section pas largimit
}


// Kerkesa 3 - For each connected client, spawn a new thread that listens for messages from that client 
//              and processes them

// Kerkesa 4 - Each message from a client should contain a request type and data payload.
//              The server should process each request and generate a response, which 
//              should be sent back to the client using its message queue

// Kerkesa 5 - When a client disconnects, its message queue should be removed from the list 
//              of connected clients and the corresponding thread should be terminated.

int main(){

}