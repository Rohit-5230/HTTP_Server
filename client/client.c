#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LENGTH 512

int main(int argc, char *argv[])
{
    int socket_fd, new_socket;
    char rec_buffer[LENGTH];
    struct sockaddr_in server_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR: failed to obtain socket descripter.");
        exit(1);
    }

    int PORT;
    printf("client PORT:");
    scanf("%d", &PORT);

    bzero(&(server_addr.sin_zero), 8);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("ERROR: failed to connect with server");
        exit(1);
    }
    else
    {
        printf("[Client] Connected to server at port %d...ok!\n", PORT);
    }
    while (1)
    {
        int k;
        printf(" choose 1 for listen hello from server.\n choose 2 for requesting webpage from server.\n choose 3 for accept file from server.\n Enter your choice:\n");
        scanf("%d", &k);
        switch (k)
        {
        case 1:
        {
            printf("=================\n");
            printf("Hello from server\n");
            printf("=================\n\n");
            break;
        }
        case 2:
        {
            printf("=====================================================\n");
            printf("go to your BROWSER and search = My_IP/Port/index.html\n\n");
            printf("=====================================================\n");
            break;
        }
        case 3:
        {
            printf("[Client] Receiveing file from Server and saving it as recieved.txt... \n\n");
            char *file_name = "./recieved.txt";
            FILE *file = fopen(file_name, "a");
            if (file == NULL)
                printf("File %s Cannot be opened.\n", file_name);
            else
            {
                bzero(rec_buffer, LENGTH);
                int file_block_size = 0;
                while ((file_block_size = recv(socket_fd, rec_buffer, LENGTH, 0)) > 0)
                {
                    int write_sz = fwrite(rec_buffer, sizeof(char), file_block_size, file);
                    if (write_sz < file_block_size)
                    {
                        perror("File write failed.\n");
                    }
                    bzero(rec_buffer, LENGTH);
                    if (file_block_size == 0 || file_block_size != 512)
                    {
                        break;
                    }
                }
                if (file_block_size < 0)
                {
                    if (errno == EAGAIN)
                    {
                        printf("recv() timed out.\n");
                    }
                    else
                    {
                        fprintf(stderr, "recv() failed due to errno = %d\n", errno);
                    }
                }
                printf("Ok received from server!\n");
                fclose(file);
            }
            close(socket_fd);
            break;
        }
        default:
        {
            printf("wrong choice");
        }
        }
    }
    return 0;
}