#include "helpers.h"

char temporer[2] = "?\0";
void main(int argc, char * argv[])
{
    ShowHelp(argc, argv[0]);
    SetupSocket(atoi(argv[1]));
    while(1)
    {
        printf("\nListening...\n");
        acpt = accept(soket, (struct sockaddr*)&remote, &sock_len); //accepting connection
        printf("Accepted connection from %s !\nForking to child\n", inet_ntoa(remote.sin_addr));
        pid = fork();
        waitpid(pid, NULL, 0);
        if(pid == 0) //child
        {
            pid = fork();
            if(pid == 0) //grandchild
            {
                data_len = read(acpt, buffer, sizeof(buffer));
                if(data_len == -1)
                {
                    shutdown(acpt,2);
                    exit(0);
                }
                sscanf(buffer, "%s /%s", method, resource);
                res = 0;
                res = fopen(resource, "r");
                if(res == 0) //file not readable etc
                {
                    now = time(NULL);
                    printf("Unable to access requested file %s %d\n",resource, (int)strlen(resource));
                    sprintf(response, "HTTP/1.1 404 Not Found\r\nDate: %sConnection: close\r\n\r\n", ctime(&now));
                    send(acpt, response, strlen(response),0);
                    if(strstr(resource,"?") != NULL)
                    {
                        number = ParseForData(resource, variable_array, data_array);
                        while(number--)
                        {
                            debug = fopen(variable_array[number], "w");
                            fprintf(debug, "%s", data_array[number]);
                            fclose(debug);
                        }
                    }
                    shutdown(acpt,2);
                    printf("Closed Connection, grandchild killing itself\n");
                    exit(0);
                }
                else
                {
                    printf("File loaded~\n");
                    now = time(NULL);
                    mimetype(resource,ext);
                    clength = file_size(res);
                    sprintf(response,"HTTP/1.1 200 OK\r\nDate: %sContent-Type: %s\r\nContent-Length: %lu\r\nConnection: keep-alive\r\n\r\n",ctime(&now), ext , clength);
                    send(acpt, response, strlen(response), 0);
                    printf("%s",response);
                    while((data_len = fread(file_buffer, 1, sizeof(file_buffer), res)) > 0)
                    {
                        send_bytes = send(acpt, file_buffer, data_len, 0);
                        if(send_bytes == -1)
                            break;
                        else
                            memset(file_buffer, 0, sizeof(file_buffer));
                    }
                }
                fclose(res);
                shutdown(acpt,2);
                printf("Closed Connection, grandchild killing itself\n");
                exit(0);
            }
            else //child
            {
                exit(0);
            }
        }
    }
}
