#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/un.h>
#include<stdlib.h>
#include<unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int *argc, char **argv){

    struct sockaddr_un client2;
        int ret = 0;
        int data_socket = 0;
        char buffer[BUFFER_SIZE];
        int connection_socket = 0;

        connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        if(connection_socket == -1){
            perror("socket");
            exit(EXIT_FAILURE);
        }
        printf("Socket connected\n");
        
        memset(&client2, 0, sizeof(struct sockaddr_un));
        client2.sun_family = AF_UNIX;
        strncpy(client2.sun_path, SOCKET_NAME, sizeof(client2.sun_path)-1);
        ret = connect(connection_socket, (const struct sockaddr *) &client2, sizeof(struct sockaddr_un));
        
        memset(buffer, 0,BUFFER_SIZE);
        int i = 0;
        do{
            printf("Enter number to be sent: \n");
            scanf("%d", &i);
            data_socket = write(connection_socket, &i , sizeof(int));
            if (data_socket == -1) {
                perror("write");
                break;
            }
            printf("Value of the size is %d and the value is %d\n", ret,i);
        }while(i);
      
        // memset(buffer, 0, BUFFER_SIZE);
        // strncpy (buffer, "RES", strlen("RES"));
        // buffer[strlen(buffer)] = '\0';
        // printf("buffer = %s\n", buffer);
        
        ret = read(connection_socket, buffer , BUFFER_SIZE);
            if (ret == -1) {
                perror("read");
            }

        printf("Result is %s\n", buffer);

      close(connection_socket);
    exit(EXIT_SUCCESS);

}