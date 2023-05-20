#define MAX_MSG_SIZE 256
#define SERVER_MSG_TYPE 1
#define CLIENT_MSG_TYPE 2

//Kerkesa 1
//message structure

typedef struct {
    long mtype;
    char mtext[MAX_MSG_SIZE];
} Message;

int main(){
       key_t key;
       int msgid;
       Message message;

//gjenerojme nje celes unik per message queue
       key = ftok(".", 'C');
       if (key == -1) {
        perror("Error creating key");
        exit(1);
    }

//krijimi i message queue
       msgid = msgget(key, IPC_CREAT | 0666);
       if (msgid == -1) {
        perror("Error creating message queue");
        exit(1);
    }

printf("Client: Message queue created with ID %d\n", msgid);
}


//Kerkesa 2
//konektimi me server permes nje kerkese per konektim ne message queue
   
message.mtype = SERVER_MSG_TYPE;
snprintf(message.mtext, MAX_MSG_SIZE, "%d", msgid);
if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        perror("Error sending connection request");
        exit(1);
    }

printf("Client: Connection request sent to the server\n");

//Kerkesa 3
//dergimi i kerkeses ne server 

snprintf(message.mtext, MAX_MSG_SIZE, "Request from client %d", msgid);
if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        perror("Error sending request to the server");
        exit(1);
    }

printf("Client: Request sent to the server\n");