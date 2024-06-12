#include<stdio.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFERSIZE 128

int main(int *argc, char **argv){
    struct sockaddr_un client1;

    int ret = 0;
    int dataSocket = 0;
    int connection_socket = 0;
    char buffer[BUFFERSIZE];
    int i;

    connection_socket = socket(AF_UNIX, SOCK_STREAM,0);
    if(connection_socket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&client1, 0, sizeof(struct sockaddr_un));
    client1.sun_family = AF_UNIX;
    strncpy(client1.sun_path, SOCKET_NAME, sizeof(client1.sun_path)-1);

    dataSocket = connect(connection_socket, (const struct sockaddr *) &client1, sizeof(struct sockaddr_un));
      if(connection_socket == -1){
        perror("connect");
        exit(EXIT_FAILURE);
        }
    printf("Connected successfull\n");

   // memset(buffer, 0, BUFFERSIZE);
    do{
        printf("Enter the number to be sent");
        scanf("%d", &i);
        ret = write(connection_socket, &i, sizeof(int));
        if(ret == -1){
            perror("write");
            exit(EXIT_FAILURE);
        }
            // printf("written\n");
    }while(i);

     memset(buffer, 0, BUFFERSIZE);
        strncpy (buffer, "RES", strlen("RES"));
        buffer[strlen(buffer)] = '\0';
        printf("buffer = %s\n", buffer);

        ret = write(dataSocket, buffer, strlen(buffer));
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }


    memset(buffer, 0, BUFFERSIZE);
    ret = read(connection_socket, buffer, BUFFERSIZE);
     if(ret == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }
    printf("Result = %s", buffer);

    close(connection_socket);
    exit(EXIT_SUCCESS);
}
