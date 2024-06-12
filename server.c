#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/un.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define MAX_CLIENT 30

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFERSIZE 128

int MULTI_CLIENT[MAX_CLIENT];
int CLIENT_RES[MAX_CLIENT] = {0};

static void initilze_multi_fd_set(){
    for (int i = 0 ; i < MAX_CLIENT ; i++){
        MULTI_CLIENT[i] = -1;
    }
}
static void add_multi_fd_set(int sk_fd){
    int i = 0;
    for (; i < MAX_CLIENT ; i++){
        if(MULTI_CLIENT[i] != -1)
            continue;
        MULTI_CLIENT[i] = sk_fd;
        break;
    }
}

static void remove_multi_fd_set(int sk_fd){
    int i = 0;
    for (; i < MAX_CLIENT ; i++){
        if(MULTI_CLIENT[i] == sk_fd){
            MULTI_CLIENT[i] = -1;
            break;
        }
    }
}

static void refresh_fd_set(fd_set *ptr_fd_set){
    FD_ZERO(ptr_fd_set);
    for (int i = 0 ; i < MAX_CLIENT ; i++){
        if(MULTI_CLIENT[i] != -1){
            FD_SET(MULTI_CLIENT[i], ptr_fd_set);
        }
    }
}

static int getMax_fd_set(){
    int max = 0;
    for (int i = 0 ; i < MAX_CLIENT ; i++){
        if(max < MULTI_CLIENT[i]){
            max = MULTI_CLIENT[i];
        }
    }
    return max;
}
int main(int *argc, char **argv){
    struct sockaddr_un jio;

    int ret = 0;
    int dataSocket = 0;
    int connection_socket = 0;
    fd_set freads;
    char buffer[BUFFERSIZE];
    int common_socketfd, i;
    int data = 0;

    initilze_multi_fd_set();
    add_multi_fd_set(0);

    unlink(SOCKET_NAME);

    connection_socket = socket(AF_UNIX, SOCK_STREAM,0);
    if(connection_socket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Master Socket is Created\n");
    
    memset(&jio, 0 , sizeof(struct sockaddr_un));

    jio.sun_family = AF_UNIX;

    memcpy(jio.sun_path, SOCKET_NAME, sizeof(jio.sun_path)-1);


    ret  = bind(connection_socket, (const struct sockaddr *) &jio, sizeof(struct sockaddr_un));
    if(ret == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    printf("Binding is done\n");

    ret =  listen(connection_socket, 20);
    if(ret == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("listenig ...\n");

    add_multi_fd_set(connection_socket);
   
    for(;;){
        refresh_fd_set(&freads);

        printf("Waiting on select() sys call\n ");

        select(getMax_fd_set() + 1, &freads, NULL,NULL,NULL);


        if(FD_ISSET(connection_socket, &freads)){
            dataSocket = accept(connection_socket,NULL, NULL);

            if(dataSocket == -1){
                perror("Accept");
                exit(EXIT_FAILURE);
            }
            printf("Client Information added\n");
            add_multi_fd_set(dataSocket);
        }
    
        else{
            common_socketfd = -1; i = 0;
            for( ; i< MAX_CLIENT ; i++){
                if(FD_ISSET(MULTI_CLIENT[i], &freads)){
                    common_socketfd = MULTI_CLIENT[i];
                    memset(buffer, 0, BUFFERSIZE);
                    ret  = read(common_socketfd, buffer, BUFFERSIZE);

                    if(ret == -1){
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                    printf("Input read is %d\n", data);
                    memcpy(&data, buffer, sizeof(int));
                    sprintf(buffer, "Result = %d\n", CLIENT_RES[i]);
                    
                    if(data == 0){
                        printf("sending result\n");
                        ret = write(common_socketfd, buffer,BUFFERSIZE);
                        if(ret == -1){
                            perror("write");
                            exit(EXIT_FAILURE);
                        }
                        close(common_socketfd);
                        CLIENT_RES[i] = 0;
                        remove_multi_fd_set(common_socketfd);
                        continue;
                    }
                    CLIENT_RES[i] += data;
                }
            }

        }
    }
    close(connection_socket);
    remove_multi_fd_set(connection_socket);
    printf("connection Closed\n");
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}