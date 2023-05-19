#define MAX_MSG_SIZE 256
#define SERVER_MSG_TYPE 1
#define CLIENT_MSG_TYPE 2

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