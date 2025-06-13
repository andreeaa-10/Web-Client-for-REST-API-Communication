#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>    
#include <arpa/inet.h>
#include "helper.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

// functie care verifica ca username-ul si parola sunt introduse cum specifica
// enuntul, fara spatii
int contains_spaces(char *str) {
    return strchr(str, ' ') != NULL;
}

// functie care afiseaza mesajul de eroare dat de server
int check_for_error_and_print(char *response) {
    // fac pattern match pe inceputul JSON-ului
    char *json_start = strchr(response, '{');

    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // extrag bucata efectiva de continut pentru eroare si o afisez
    char *error_msg = (char *)json_object_get_string(json_start_object_data, "error");
    if (error_msg != NULL) {
        printf("ERROR: %s\n", error_msg);
        json_value_free(json_start_object);
        return 1;
    }

    json_value_free(json_start_object);
    return 0; 
}

// functie care extrage cookie-ul de sesiune
char *get_session_cookie(char *message){
    // fac pattern match pe cuvantul de dinainte de continutul cookie-ului de sesiune
    char *cookie_start = strstr(message, "Set-Cookie: ");
    cookie_start += 12; 
    char *cookie_end = strchr(cookie_start, ';');

    int cookie_len = cookie_end - cookie_start;
    char *cookie = malloc(cookie_len + 1);
    strncpy(cookie, cookie_start, cookie_len);
    cookie[cookie_len] = '\0'; 

    return cookie;
}

void parse_and_print_users(char *response) {
    // fac pattern match pe inceputul de JSON
    char *json_start = strchr(response, '{');
    
    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // transform in array continutul
    JSON_Array *users_array = json_object_get_array(json_start_object_data, "users");

    int users_count = json_array_get_count(users_array);
    printf("SUCCESS: Users list\n");

    // parcurg array-ul, extrag pe rand datele si le afisez
    for (int i = 0; i < users_count; i++) {
        JSON_Object *json_user_obj = json_array_get_object(users_array, i);
        int id = (int)json_object_get_number(json_user_obj, "id");
        char *username = (char *)json_object_get_string(json_user_obj, "username");
        char *password = (char *)json_object_get_string(json_user_obj, "password");
        printf("#%d %s:%s\n", id, username, password);
    }

    json_value_free(json_start_object);
}

char *get_jwt(char *message){
    // fac pattern match pe inceputul de JSON
    char *json_start = strchr(message, '{');

    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // extrag partea efectiva de token
    char *token = (char *)json_object_get_string(json_start_object_data, "token");
    char *token_out = NULL;
    if (token != NULL) {
        token_out = strdup(token);
        printf("SUCCESS: Received JWT token\n");
    } else {
        printf("ERROR: Token not found in response\n");
    }

    json_value_free(json_start_object);

    return token_out;
}

void parse_and_print_movies(char *response) {
    // fac pattern match pe inceputul de JSON
    char *json_start = strchr(response, '{');

    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // transform intr-un array de filme
    JSON_Array *movies_array = json_object_get_array(json_start_object_data, "movies");

    int movies_count = json_array_get_count(movies_array);
    if (movies_count == 0) {
        printf("SUCCESS: No movies available\n");
        json_value_free(json_start_object);
        return;
    }

    // pentru fiecare element din array ii iau datele si le afisez
    printf("SUCCESS: Movies list\n");
    for (int i = 0; i < movies_count; i++) {
        JSON_Object *movie_obj = json_array_get_object(movies_array, i);

        int id = (int)json_object_get_number(movie_obj, "id");
        char *title = (char *)json_object_get_string(movie_obj, "title");

        printf("#%d %s\n", id, title);
    }

    json_value_free(json_start_object);
}

void parse_and_print_movie(char *response) {
    // fac pattern match pe JSON
    char *json_start = strchr(response, '{');

    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // extrag din JSON datele filmului
    char *title = (char *)json_object_get_string(json_start_object_data, "title");
    int year = (int)json_object_get_number(json_start_object_data, "year");
    char *description = (char *)json_object_get_string(json_start_object_data, "description");

    char *rating_str = (char *)json_object_get_string(json_start_object_data, "rating");
    float rating = 0.0f;
    rating = atof(rating_str);

    // afisez datele filmului
    printf("SUCCESS: Movie details\n");
    printf("title: %s\n", title);
    printf("year: %d\n", year);
    printf("description: %s\n", description);
    printf("rating: %.1f\n", rating);

    json_value_free(json_start_object);
}

void parse_and_print_collections_simple(char *response) {
    // fac pattern match pe inceputul colectiilor
    char *json_start = strchr(response, '[');

    JSON_Value *json_start_object = json_parse_string(json_start);

    // transform intr-un array de colectii
    JSON_Array *collections = json_value_get_array(json_start_object);
    int n = json_array_get_count(collections);

    // pentru fiecare colectie afisez id-ul si titlul
    printf("SUCCESS: Collections list\n");
    for (int i = 0; i < n; i++) {
        JSON_Object *collection = json_array_get_object(collections, i);
        int id = (int)json_object_get_number(collection, "id");
        const char *title = json_object_get_string(collection, "title");

        printf("#%d: %s\n", id, title);
    }

    json_value_free(json_start_object);
}

void parse_and_print_collection_details(char *response) {
    // fac pattern match pe inceputul colectiei
    char *json_start = strchr(response, '{');

    JSON_Value *json_start_object = json_parse_string(json_start);

    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);

    // extrag titlul si ownerul colectiei
    char *title = (char *)json_object_get_string(json_start_object_data, "title");
    char *owner = (char *)json_object_get_string(json_start_object_data, "owner");

    printf("SUCCESS: Collection details\n");
    if (title) printf("title: %s\n", title);
    if (owner) printf("owner: %s\n", owner);

    // transform intr-un array de filme si parcurg acest array, extragand la fiecare pas id-ul si
    // titlul filmului, apoi afisandu-le
    JSON_Array *movies = json_object_get_array(json_start_object_data, "movies");
    if (movies) {
        int count = json_array_get_count(movies);
        for (int i = 0; i < count; i++) {
            JSON_Object *movie = json_array_get_object(movies, i);
            int movie_id = (int)json_object_get_number(movie, "id");
            char *movie_title = (char *)json_object_get_string(movie, "title");
            if (movie_title)
                printf("#%d: %s\n", movie_id, movie_title);
        }
    }

    json_value_free(json_start_object);
}