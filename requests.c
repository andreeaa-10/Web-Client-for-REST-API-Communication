#include <stdlib.h>    
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>     
#include <arpa/inet.h>
#include "helper.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *jwt_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    if (cookies != NULL) {
       for(int i = 0; i < cookies_count; i++){
            sprintf(line, "Cookie: %s;", cookies[i]);
            compute_message(message, line);
            memset(line, 0, LINELEN);
       }
    }
    memset(line, 0, LINELEN);

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, 
                            char *jwt_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    int offset = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        if (i == 0) {
            offset += sprintf(body_data_buffer + offset, "%s", body_data[i]);
        } else {
            offset += sprintf(body_data_buffer + offset, "&%s", body_data[i]);
        }
    }

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (cookies != NULL) {
       for(int i = 0; i < cookies_count; i++){
            sprintf(line, "Cookie: %s;", cookies[i]);
            compute_message(message, line);
            memset(line, 0, LINELEN);
       }
    }
    memset(line, 0, LINELEN);

    compute_message(message, "");
    memset(line, 0, LINELEN);

    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}

// functie pentru compunerea unei cereri de tip DELETE, similara cu GET, insa
// cu o sintaxa putin modificata
char *compute_delete_request(char *host, char *url,
                            char **cookies, int cookies_count, char *jwt_token){
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);

    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    if (cookies != NULL) {
       for(int i = 0; i < cookies_count; i++){
            sprintf(line, "Cookie: %s;", cookies[i]);
            compute_message(message, line);
            memset(line, 0, LINELEN);
       }
    }
    memset(line, 0, LINELEN);

    compute_message(message, "");
    return message;
}

// functie pentru compunerea unei cereri de tip PUT, similara cu POST insa cu o sintaxa
// putin modificata
char *compute_put_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, 
                            char *jwt_token){
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    int offset = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        if (i == 0) {
            offset += sprintf(body_data_buffer + offset, "%s", body_data[i]);
        } else {
            offset += sprintf(body_data_buffer + offset, "&%s", body_data[i]);
        }
    }

    sprintf(line, "PUT %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (cookies != NULL) {
       for(int i = 0; i < cookies_count; i++){
            sprintf(line, "Cookie: %s;", cookies[i]);
            compute_message(message, line);
            memset(line, 0, LINELEN);
       }
    }
    memset(line, 0, LINELEN);

    compute_message(message, "");
    memset(line, 0, LINELEN);

    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}