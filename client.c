#define MAX_MSG_SIZE 256 //gjatesia e mesazhit
#define SERVER_MSG_TYPE 1 //message type per mesazhet e serverit
#define CLIENT_MSG_TYPE 2 //message type per mesazhet e klientit

//Kerkesa 1
//message structure

typedef struct { //definimi i struktures Message
    long mtype; //anetari qe tregon llojin e mesazhit
    char mtext[MAX_MSG_SIZE]; //anetari varg i karaktereve qe tregon permbajtjen e mesazhit
} Message; 

int main(){
       key_t key; //celesi per komunikim
       int msgid; //ruan ID e message queue
       Message message; //instance e struktures Message

//gjenerojme nje celes unik per message queue
       key = ftok(".", 'C'); //funksion per gjenerimin e celesit unik sipas nje path dhe project identifier
       // . paraqet folderin e tanishem dhe C eshte project id.
       if (key == -1) {
        perror("Error creating key");
        exit(1); //nese funksioni deshton (celesi kthen -1) printohet nje error
    }

//krijimi i message queue
       msgid = msgget(key, IPC_CREAT | 0666); //funksion per krijimin e message queue 
       //merr celesin e gjeneruar nga ftok dhe argumentet qe specifikojn leje per queue
       if (msgid == -1) {
        perror("Error creating message queue");
        exit(1);
    }

printf("Client: Message queue created with ID %d\n", msgid); //printon nje mesazh qe tregon se message queue eshte krijuar dhe tregon ID e saj
}


//Kerkesa 2
//konektimi me server permes nje kerkese per konektim ne message queue
   
message.mtype = SERVER_MSG_TYPE; //anetari mtype i struktures behet set ne SERVER_MSG_TYPE duke treguar qe eshte nje mesazh i serverit
snprintf(message.mtext, MAX_MSG_SIZE, "%d", msgid); //funksion qe perdoret per te formatuar mtext me IDn e message queue te ruajtuar ne msgid
if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        perror("Error sending connection request");
        exit(1);
    }

printf("Client: Connection request sent to the server\n");

//Kerkesa 3
//dergimi i kerkeses ne server 

snprintf(message.mtext, MAX_MSG_SIZE, "Request from client %d", msgid); //funksion per formatimin e mtext me mesazhin e kerkeses duke perfshire ID e message queue te klientit
if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        perror("Error sending request to the server");
        exit(1);
    }

printf("Client: Request sent to the server\n");

//Kerkesa 4
// Wait for responses from the server
    if (msgrcv(msgid, &message, sizeof(message.mtext), CLIENT_MSG_TYPE, 0) == -1) {
        perror("Error receiving response from server");
        exit(1);
    }

    printf("Client: Received response from server: %s\n", message.mtext);

//Kerkesa 5
    // Gracefully disconnect from the server
    message.mtype = SERVER_MSG_TYPE;
    snprintf(message.mtext, MAX_MSG_SIZE, "Disconnect request from client %d", msgid);
    if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        perror("Error sending disconnect request to the server");
        exit(1);
    }

    printf("Client: Disconnect request sent to the server\n");

    // Clean up the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Error removing message queue");
        exit(1);
    }

    printf("Client: Message queue removed\n");

    return 0;
}