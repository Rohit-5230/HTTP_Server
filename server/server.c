#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>

#define LENGTH 512

char webpage[] = "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                 "<!DOCTYPE html>\r\n"
                 "<html><head><title>indian5230</title>\r\n"
                 "<style>body { background: linear-gradient(to left, yellow, rgb(224, 230, 233)); background-attachment: fixed; } </style></head>\r\n"
                 "<body><center><h1>Team name: indian5230</h1>\r\n"
                 "<h1>Name: Rohit kumar singh</h1>\r\n"
                 "<h1>Roll no: BE/10280/17</h1>\r\n"
                 "<h1>Email: indian5230@gmail.com</h1></center></body></html>\r\n";

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);
    int addrlen2 = sizeof(client_addr);
    int on = 1;
    char rec_buffer[LENGTH], send_buffer[LENGTH];

    int PORT;
    printf("client PORT:");
    scanf("%d", &PORT);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[Server] Obtaining socket descripter successfully.\n");
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    bzero((char *)&server_addr, addrlen);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&server_addr, addrlen) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[Server] Binded TCP port %d in addr sucessfully.\n", PORT);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[Server] Listening the port %d successfully.\n", PORT);
    }

    while (1)
    {

        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen2);
        if (new_socket < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("[Server] Server has got connected from %s.\n", inet_ntoa(client_addr.sin_addr));
        }

        valread = read(new_socket, rec_buffer, LENGTH);
        printf("%s\n%ld\n", rec_buffer, valread);

        if (!strncmp(rec_buffer, "GET /index.html", 15))
        {
            write(new_socket, webpage, sizeof(webpage));
            printf("------------------webpage sent-------------------\n");
        }
        //send file to client
        else
        {
            char *file_name = "./file.txt";
            printf("[Server] Sending %s to the client\n", file_name);
            FILE *file = fopen(file_name, "r");

            if (file == NULL)
            {
                perror("File not found");
                exit(1);
            }

            bzero(send_buffer, LENGTH);
            int file_block_size;
            while ((file_block_size = fread(send_buffer, sizeof(char), LENGTH, file)) > 0)
            {
                if (send(new_socket, send_buffer, file_block_size, 0) < 0)
                {
                    perror("ERROR: failed to send the file");
                    exit(1);
                }
                bzero(send_buffer, LENGTH);
            }
            printf("File sent to the client.");
        }
        close(new_socket);
    }
    return 0;
}
