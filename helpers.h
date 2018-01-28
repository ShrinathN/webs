//important header files to include
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>

int status;

int soket, acpt, sock_len, data_len ,send_bytes, pid;
int number;
char sent_data[1024], sent_variable[1024], data_part[10240];
uint8_t one = 1;
char *variable_array[1024], *data_array[1024];
char buffer[1024], method[8], resource[64], file_buffer[65536], filename[128], response[1024], ext[16];
unsigned long clength;
time_t now;
struct sockaddr_in info,remote;
FILE *res, *debug;

void mimetype(char *, char *);
unsigned long file_size(FILE *);
void ShowHelp(int, char *);
void SetupSocket(int);
int ParseForData(char *, char *[], char *[]);

void mimetype(char *resource, char *other)
{
    int counter = 0, counter2 = 0;
    char extension[16], mime[32];
    while(resource[counter] != '.')
        counter++;
    counter2 = 0;
    while(resource[counter] != '\0')
    {
        extension[counter2] = resource[counter];
        counter++;
        counter2++;
    }
    if(strcmp(extension,".html") == 0)
        strcpy(mime, "text/html");
    else if(strcmp(extension,".htm") == 0)
        strcpy(mime, "text/html");
    else if(strcmp(extension,".css") == 0)
        strcpy(mime, "text/css");
    else if(strcmp(extension,".txt") == 0)
        strcpy(mime, "text/plain");

    else if(strcmp(extension,".png") == 0)
        strcpy(mime, "image/png");
    else if(strcmp(extension,".jpg") == 0)
        strcpy(mime, "image/jpeg");
    else if(strcmp(extension,".jpeg") == 0)
        strcpy(mime, "image/jpeg");
    else if(strcmp(extension,".gif") == 0)
        strcpy(mime, "image/gif");

    else if(strcmp(extension,".js") == 0)
        strcpy(mime, "application/javascript");
    else if(strcmp(extension,".json") == 0)
        strcpy(mime, "application/json");
    else if(strcmp(extension,".pdf") == 0)
        strcpy(mime, "application/pdf");
    else if(strcmp(extension,".xml") == 0)
        strcpy(mime, "application/xml");
    else if(strcmp(extension,".zip") == 0)
        strcpy(mime, "application/zip");
    else
        strcpy(mime, "data");
    strcpy(other, mime);
}

unsigned long file_size(FILE *in)
{
    unsigned long siz;
    fseek(in, 0, SEEK_END);
    siz = ftell(in);
    fseek(in, 0, SEEK_SET);
    return siz;
}

void ShowHelp(int args, char *program_name)
{
    if(args < 2)
    {
        printf("%s [port]\n",program_name);
        exit(0);
    }
}

void SetupSocket(int port)
{
    soket = socket(AF_INET, SOCK_STREAM, 0);
    sock_len = sizeof(remote);
    info.sin_family = AF_INET;
    info.sin_port = htons(port);
    info.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(soket, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
    bind(soket, (struct sockaddr*)&info, sizeof(info));
    listen(soket, 5);
}

int ParseForData(char *resource, char *variable_array[], char *data_array[])
{
    int vals_found = 0;
    char tmp_arr[1024], tmp_arr2[1024],var_temp[1024], data_temp[1024];
    strcpy(tmp_arr, strstr(resource,"?"));
    sscanf(tmp_arr, "?%[^=]=%[^&]",var_temp, data_temp);
    variable_array[vals_found] = malloc(strlen(var_temp));
    data_array[vals_found] = malloc(strlen(data_temp));
    strcpy(variable_array[vals_found], var_temp);
    strcpy(data_array[vals_found], data_temp);
    bzero(var_temp, sizeof(var_temp));
    bzero(data_temp, sizeof(data_temp));
    vals_found++;
    if(strstr(tmp_arr, "&") == 0)
        return vals_found;
    else
    {
        do
        {
            strcpy(tmp_arr, strstr(tmp_arr, "&"));
            bzero(var_temp, sizeof(var_temp));
            bzero(data_temp, sizeof(data_temp));
            sscanf(tmp_arr, "&%[^=]=%[^&]",var_temp, data_temp);
            variable_array[vals_found] = malloc(strlen(var_temp));
            data_array[vals_found] = malloc(strlen(data_temp));
            strcpy(variable_array[vals_found], var_temp);
            strcpy(data_array[vals_found], data_temp);
            tmp_arr[0] = 'x';
            vals_found++;
        }
        while(strstr(tmp_arr, "&") != 0);
    }
    return vals_found;
}
