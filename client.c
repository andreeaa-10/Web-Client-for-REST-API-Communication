#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helper.h"
#include "requests.h"
#include "parson.h"

#define URL_LEN 2048

char *admin_session_cookie = NULL;
char *user_session_cookie = NULL;
char *jwt_token = NULL;

void login_admin(int sockfd) {
    // if (admin_session_cookie != NULL) {
    //     printf("ERROR: Already logged in\n");
    //     return;
    // }

    // citesc datele de autentificare ale adminului
    char username[LINELEN], password[LINELEN];
    
    getchar();
    printf("username=");
    fgets(username, LINELEN, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("password=");
    fgets(password, LINELEN, stdin);
    password[strcspn(password, "\n")] = '\0';

    // verificarea corectitudinii datelor
    if(contains_spaces(username) || contains_spaces(password)){
        printf("ERROR: Data contains spaces\n");
        return;
    }

    // creez un obiect JSON unde adaug datele de autentificare ale utilizatorului
    JSON_Value *json_object = json_value_init_object();  
    JSON_Object *json_object_data = json_value_get_object(json_object);  
    json_object_set_string(json_object_data, "username", username);  
    json_object_set_string(json_object_data, "password", password);  

    char *json_data_string = json_serialize_to_string(json_object);  
    // printf("%s\n", json_body);

    char *body_data[] = { json_data_string };
    char *message = compute_post_request("63.32.125.183", "/api/v1/tema/admin/login", "application/json", body_data, 1, NULL, 0, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    // caut eroarea, daca exista, si o afisez
    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);  
        json_value_free(json_object);  
        free(message);  
        free(response);
        return; 
    }
    
    printf("SUCCESS: Login successful\n");
    // retin cookie-ul de sesiune al adminului
    admin_session_cookie = get_session_cookie(response);

    json_free_serialized_string(json_data_string);  
    json_value_free(json_object);  
    free(message);  
    free(response);
}

void add_user(int sockfd){
    char username[LINELEN], password[LINELEN];
    
    // citesc de la tastatura datele utilizatorului
    getchar();
    printf("username=");
    fgets(username, LINELEN, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("password=");
    fgets(password, LINELEN, stdin);
    password[strcspn(password, "\n")] = '\0';

    // verificarea corectitudinii datelor
    if(contains_spaces(username) || contains_spaces(password)){
        printf("ERROR: Data contains spaces\n");
        return;
    }

    // creez un obiect JSON unde pun datele utilizatorlui
    JSON_Value *json_object = json_value_init_object();  
    JSON_Object *json_object_data = json_value_get_object(json_object);  
    json_object_set_string(json_object_data, "username", username);  
    json_object_set_string(json_object_data, "password", password);  

    char *json_data_string = json_serialize_to_string(json_object);  
    //printf("%s\n", json_body);

    char *cookies[] = { admin_session_cookie };

    char *body_data[] = { json_data_string };
    // trimit cererea, partajand cookie-ul de sesiune pentru a demonstra accesul privilegiat
    char *message = compute_post_request("63.32.125.183", "/api/v1/tema/admin/users", "application/json", body_data, 1, cookies, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);  
        json_value_free(json_object);  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: User created successfully\n");

    json_free_serialized_string(json_data_string);  
    json_value_free(json_object);  
    free(message);  
    free(response);
}

void get_users(int sockfd){
    // trimit cererea, partajand cookie-ul de sesiune pentru a demonstra accesul privilegiat
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/admin/users", NULL, &admin_session_cookie, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    // afisez userii prin parasarea JSON-ului in formatul cerut
    parse_and_print_users(response);
    // printf("%s", response);

    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    free(message);  
    free(response);
}

void delete_user(int sockfd){
    char username[LINELEN];

    // citesc de la tastatura numele utilizatorului ce va fi sters
    getchar();
    printf("username=");
    fgets(username, LINELEN, stdin);
    username[strcspn(username, "\n")] = '\0';

    // verificarea corectitudinii datelor
    if(contains_spaces(username)){
        printf("ERROR: Data contains spaces\n");
        return;
    }

    // concatenez la URL numele utlizatorului pentru a forma calea corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/admin/users/%s", username);

    // trimit cererea, partajand cookie-ul de sesiune pentru a demonstra accesul privilegiat
    char *message = compute_delete_request("63.32.125.183", url, &admin_session_cookie, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    if (check_for_error_and_print(response)) {  
        free(message);  
        free(response);
        return; 
    }
    // printf("%s", response);

    printf("SUCCES: User %s deleted", username);

    free(message);  
    free(response);
}

void login(int sockfd){
    char admin_username[LINELEN], username[LINELEN], password[LINELEN];
    
    // citesc de la tastatura datele de autentificare
    getchar();
    printf("admin_username=");
    fgets(admin_username, LINELEN, stdin);
    admin_username[strcspn(admin_username, "\n")] = '\0';

    printf("username=");
    fgets(username, LINELEN, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("password=");
    fgets(password, LINELEN, stdin);
    password[strcspn(password, "\n")] = '\0';

    // verificarea corectitudinii datelor
    if(contains_spaces(admin_username) || contains_spaces(username) || contains_spaces(password)){
        printf("ERROR: Data contains spaces\n");
        return;
    }

    // creez un obiect JSON unde pun datele de autentificare
    JSON_Value *json_object = json_value_init_object();  
    JSON_Object *json_object_data = json_value_get_object(json_object);  
    json_object_set_string(json_object_data, "admin_username", admin_username);
    json_object_set_string(json_object_data, "username", username);  
    json_object_set_string(json_object_data, "password", password);  

    char *json_data_string = json_serialize_to_string(json_object);  
    // printf("%s\n", json_body);

    char *body_data[] = { json_data_string };
    // trimit mesajul, partajand cookie-ul de sesiune pentru a demonstra sesiunea activa a adminului
    char *message = compute_post_request("63.32.125.183", "/api/v1/tema/user/login", "application/json", body_data, 1, NULL, 0, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);  
        json_value_free(json_object);  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: User login successful\n");
    // retin cookie-ul de sesiune al utilizatorului
    user_session_cookie = get_session_cookie(response);
  
    json_free_serialized_string(json_data_string);  
    json_value_free(json_object); 
    free(message);  
    free(response);
}

void logout_admin(int sockfd){
    // trimit cererea, partajand cookie-ul de sesiune al adminului pentru a demonstra accesul privilegiat
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/admin/logout", NULL, &admin_session_cookie, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s", response);

    if (check_for_error_and_print(response)) {  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: Admin logged out successfully\n");

    // eliberez cookie-ul de sesiune al adminului
    if (admin_session_cookie != NULL){
        free(admin_session_cookie);
        admin_session_cookie = NULL;
    }

    free(message);  
    free(response);
}

void get_access(int sockfd){
    // trimit cererea, partajand cookie-ul de sesiune al userului, pentru a demonstra sesiunea activa
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/library/access", NULL, &user_session_cookie, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    if (check_for_error_and_print(response)) { 
        free(message);  
        free(response);
        return; 
    }

    // printf("%s", response);
    // retin JWT-ul pentru validari ulterioare
    jwt_token = get_jwt(response);
}

void get_movies(int sockfd){
    // trimit cererea, partajand cookie-ul de sesiune si JWT-ul pentru a demonstra accesul la biblioteca
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/library/movies", NULL, &user_session_cookie, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s", response);
    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    // afisez lista de filme parsata din JSON
    parse_and_print_movies(response);

    free(message);  
    free(response);
}

void add_movie(int sockfd){
    char title[LINELEN], description[LINELEN];
    int year;
    float rating;
    char extra;
    
    // citesc datele filmului
    getchar();
    printf("title=");
    fgets(title, LINELEN, stdin);
    title[strcspn(title, "\n")] = '\0'; 

    // verific daca primesc tipul de date corect
    printf("year=");
    if (scanf("%d%c", &year, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n"); 
        return;
    }

    printf("description=");
    fgets(description, LINELEN, stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("rating=");
    if (scanf("%f", &rating) != 1) {
        printf("ERROR: Invalid input type\n");
        return;
    }

    // creez un obiect JSON unde adaug datele filmului
    JSON_Value *json_object = json_value_init_object();  
    JSON_Object *json_object_data = json_value_get_object(json_object);  
    
    json_object_set_string(json_object_data, "title", title);
    json_object_set_number(json_object_data, "year", year);
    json_object_set_string(json_object_data, "description", description);
    json_object_set_number(json_object_data, "rating", rating);

    char *json_data_string = json_serialize_to_string(json_object);  

    char *cookies[] = { user_session_cookie };

    char *body_data[] = { json_data_string };
    // trimit cererea, impreuna cu cookie-ul de sesiune si JWT pentru a demonstra accesul
    char *message = compute_post_request("63.32.125.183", "/api/v1/tema/library/movies", "application/json", body_data, 1, cookies, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);  
        json_value_free(json_object);  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: Movie added\n");
    
    json_free_serialized_string(json_data_string);  
    json_value_free(json_object);  
    free(message);  
    free(response);
}

void get_movie(int sockfd){
    int id;
    char extra;
    // citesc id-ul filmului
    printf("id=");
    if (scanf("%d%c", &id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    // concatenez id-ul pentru a construi calea corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/movies/%d", id);

    // trimit mesajul impreuna cu cookie-ul de sesiune si JWT-ul
    char *message = compute_get_request("63.32.125.183", url, NULL, &user_session_cookie, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    // afisez datele filmului, parsate adecvat din JSON-ul din raspuns
    parse_and_print_movie(response);

    free(message);  
    free(response);
}

void update_movie(int sockfd){
    char title[LINELEN], description[LINELEN];
    int id, year;
    float rating;
    char extra;
    
    // citesc noile date ale filmului
    // verific daca primesc tipul de date corect
    printf("id=");
    if (scanf("%d%c", &id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    printf("title=");
    fgets(title, LINELEN, stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("year=");
    if (scanf("%d%c", &year, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    printf("description=");
    fgets(description, LINELEN, stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("rating=");
    if(scanf("%f", &rating) != 1){
        printf("ERROR: Invalid input type\n");
        return;
    }
    getchar();

    // creez un obiect JSON si adaug datele in el
    JSON_Value *json_object = json_value_init_object();  
    JSON_Object *json_object_data = json_value_get_object(json_object);  
    
    json_object_set_string(json_object_data, "title", title);
    json_object_set_number(json_object_data, "year", year);
    json_object_set_string(json_object_data, "description", description);
    json_object_set_number(json_object_data, "rating", rating);

    char *json_data_string = json_serialize_to_string(json_object);  

    char *cookies[] = { user_session_cookie };

    char *body_data[] = { json_data_string };

    // concatenez id-ul pentru a construi calea URL corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/movies/%d", id);

    // trimit mesajul impreuna cu cookie-ul de sesiune si JWT-ul
    char *message = compute_put_request("63.32.125.183", url, "application/json", body_data, 1, cookies, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);  
        json_value_free(json_object);  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: Movie updated successfully\n");

    json_free_serialized_string(json_data_string);  
    json_value_free(json_object);  
    free(message);  
    free(response);
}

void delete_movie(int sockfd){
    int id;
    char extra;

    // citesc id-ul filmului
    // verific daca primesc tipul de date corect
    printf("id=");
    if (scanf("%d%c", &id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    // concatenez id-ul filmului pentru a obtine calea corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/movies/%d", id);

    // trimit mesajul, impreuna cu cookie-ul de sesiune si JWT-ul
    char *message = compute_delete_request("63.32.125.183", url, &user_session_cookie, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s", response);

    char success_message[LINELEN];
    snprintf(success_message, sizeof(success_message), "SUCCESS: Movie with id %d deleted successfully\n", id);

    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    printf("%s", success_message);
    
    free(message);  
    free(response);
}

void logout(int sockfd){
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/user/logout", NULL, &user_session_cookie, 1, NULL);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s", response);

    if (check_for_error_and_print(response)) {  
        free(message);  
        free(response);
        return; 
    }

    printf("SUCCESS: User logged out successfully\n");

    // eliberez JWT-ul si cookie-ul de sesiune
    if (user_session_cookie != NULL) {
        free(user_session_cookie);
        user_session_cookie = NULL;
    }

    if (jwt_token != NULL) {
        free(jwt_token);
        jwt_token = NULL;
    }

    free(message);  
    free(response);
}

void get_collections(int sockfd){
    char *message = compute_get_request("63.32.125.183", "/api/v1/tema/library/collections", NULL, &user_session_cookie, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s", response);
    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    // afisez lista de colectii, parsata adecvat din JSON-ul din raspuns
    parse_and_print_collections_simple(response);

    free(message);  
    free(response);
}

void get_collection(int sockfd){
    int id;
    char extra;

    // citesc id-ul colectiei
    // verific daca primesc tipul de date corect
    printf("id=");
    if (scanf("%d%c", &id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    // concatenez id-ul colectiei pentru a obtine calea corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/collections/%d", id);
    char *message = compute_get_request("63.32.125.183", url, NULL, &user_session_cookie, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }

    // afisez detaliile colectiei parsate din JSON
    parse_and_print_collection_details(response);

    free(message);  
    free(response);
}

void delete_collection(int sockfd, int id){
    char extra;
    // id este campul suplimentar care indica daca este nevoie sa fac citire de
    // la tastatura sau lucrez cu id-ul primit ca parametru
    if(id <= -1){
        // daca id este -1(valoare de referinta setata de mine) voi citi de la tastatura
        // id-ul
        printf("id=");
        // verific daca primesc tipul de date corect
        if (scanf("%d%c", &id, &extra) != 2 || extra != '\n') {
            printf("ERROR: Invalid input type\n");
            return;
        }
    }

    // concatenez id-ul pentru a forma calea URL corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/collections/%d", id);

    char *cookies[] = { user_session_cookie };

    // trimit mesajul cu cookie-ul de sesiune si JWT-ul 
    char *message = compute_delete_request("63.32.125.183", url, cookies, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    if (check_for_error_and_print(response)) {
        free(message);
        free(response);
        return;
    }

    printf("SUCCESS: Collection deleted\n");

    free(message);
    free(response);
}

int add_movie_to_collection(int sockfd, int collection_id, int movie_id){
    char extra;
    // collection_id si movie_id sunt parametri suplimentari care indica daca se va face 
    // citire de la tastatura sau nu

    // daca vreunul dintre parametri este setat la -1(valoare de referinta aleasa de mine)
    // citesc de la tastatura
    if (collection_id <= -1) {
        printf("collection_id=");
        // verific daca primesc tipul de date corect
        if (scanf("%d%c", &collection_id, &extra) != 2 || extra != '\n') {
            printf("ERROR: Invalid input type\n");
            return 1;
        }
    }

    if (movie_id <= -1) {
        printf("movie_id=");
        if (scanf("%d%c", &movie_id, &extra) != 2 || extra != '\n') {
            printf("ERROR: Invalid input type\n");
            return 1;
        }
    }

    // concatenez datele pentru a forma URL-ul corect
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/collections/%d/movies", collection_id);

    // creez un obiect de tip JSON in care se stocheaza id-ul
    JSON_Value *json_object = json_value_init_object();
    JSON_Object *json_object_data = json_value_get_object(json_object);
    json_object_set_number(json_object_data, "id", movie_id);
    char *json_data_string = json_serialize_to_string(json_object);

    char *cookies[] = { user_session_cookie };
    char *body_data[] = { json_data_string };

    // trimit mesajul cu cookie-ul de sesiune si JWT-ul
    char *message = compute_post_request("63.32.125.183", url,
                                        "application/json", body_data, 1, cookies, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    //printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);
        json_value_free(json_object);
        free(message);  
        free(response);
        return 1;
    }
    
    printf("SUCCESS: Movie %d added to collection %d\n", movie_id, collection_id);

    json_free_serialized_string(json_data_string);
    json_value_free(json_object);
    free(message);
    free(response);

    return 0;
}

void add_collection(int sockfd){
    char title[LINELEN];
    int num_movies;
    char extra;

    // citesc datele colectiei
    getchar();
    printf("title=");
    fgets(title, LINELEN, stdin);
    title[strcspn(title, "\n")] = '\0';

    // verific daca primesc tipul de date corect
    printf("num_movies=");
    if (scanf("%d%c", &num_movies, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    int movie_ids[num_movies];
    for (int i = 0; i < num_movies; i++) {
        printf("movie_id[%d]=", i);
        if (scanf("%d%c", &movie_ids[i], &extra) != 2 || extra != '\n') {
            printf("ERROR: Invalid input type\n");
            return;
        }
    }

    // creez un obiect JSON in care stochez doar titlul
    JSON_Value *json_object = json_value_init_object();
    JSON_Object *json_object_data = json_value_get_object(json_object);
    json_object_set_string(json_object_data, "title", title);

    char *json_data_string = json_serialize_to_string(json_object);

    char *cookies[] = { user_session_cookie };
    char *body_data[] = { json_data_string };

    // trimit mesajul in urma caruia s-a creat colectia si primesc id-ul ei
    char *message = compute_post_request("63.32.125.183", "/api/v1/tema/library/collections",
                                        "application/json", body_data, 1, cookies, 1, jwt_token);

    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (check_for_error_and_print(response)) {
        json_free_serialized_string(json_data_string);
        json_value_free(json_object);
        free(message);
        free(response);
        return;
    }

    // extrag id-ul din raspuns
    char *json_start = strchr(response, '{');
    JSON_Value *json_start_object = json_parse_string(json_start);
    JSON_Object *json_start_object_data = json_value_get_object(json_start_object);
    int collection_id = (int)json_object_get_number(json_start_object_data, "id");
    json_value_free(json_start_object);

    // pentru fiecare film din colectie il adaug folosind add_movie_to_collection
    for (int i = 0; i < num_movies; i++) {
        if (add_movie_to_collection(sockfd, collection_id, movie_ids[i]) != 0) {
            // daca primesc vreun mesaj de eroare, atunci inseamna ca sunt invalide datele,
            // deci sterg colectia
            delete_collection(sockfd, collection_id);
            return; 
        }
    }

    printf("SUCCESS: Collection added\n");

    json_free_serialized_string(json_data_string);
    json_value_free(json_object);
    free(message);
    free(response);
}

void delete_movie_from_collection(int sockfd){
    int collection_id, movie_id;
    char extra;

    // citesc datele colectiei
    // verific daca primesc tipul de date corect
    printf("collection_id=");
    if (scanf("%d%c", &collection_id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    printf("movie_id=");
    if (scanf("%d%c", &movie_id, &extra) != 2 || extra != '\n') {
        printf("ERROR: Invalid input type\n");
        return;
    }

    // concatenez datele colectiei pentru a forma calea corecta
    char url[URL_LEN];
    snprintf(url, sizeof(url), "/api/v1/tema/library/collections/%d/movies/%d", collection_id, movie_id);

    char *cookies[] = { user_session_cookie };

    // trimit mesajul catre server
    char *message = compute_delete_request("63.32.125.183", url, cookies, 1, jwt_token);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    if (check_for_error_and_print(response)) {
        free(message);  
        free(response);
        return; 
    }
    
    printf("SUCCESS: Movie %d deleted from collection %d\n", movie_id, collection_id);

    free(message);
    free(response);
}

int main(int argc, char *argv[])
{
    int sockfd;
    char command[LINELEN];
        
    // deschid conexiunea
    sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
    
    //iau de la tastatura setul de comenzi pana la intalnirea cuvantului "exit"
    while(1){
        scanf("%s", command);
        if (strncmp(command, "login_admin", 11) == 0 && strlen(command) == 11)
            login_admin(sockfd);
        if (strncmp(command, "add_user", 8) == 0 && strlen(command) == 8)
            add_user(sockfd);
        if (strncmp(command, "get_users", 9) == 0 && strlen(command) == 9)
            get_users(sockfd);
        if (strncmp(command, "delete_user", 11) == 0 && strlen(command) == 11)
            delete_user(sockfd);
        if (strncmp(command, "login", 5) == 0 && strlen(command) == 5)
            login(sockfd);
        if (strncmp(command, "logout_admin", 12) == 0 && strlen(command) == 12)
            logout_admin(sockfd);
        if(strncmp(command, "get_access", 10) == 0 && strlen(command) == 10)
            get_access(sockfd);
        if(strncmp(command, "get_movies", 10) == 0 && strlen(command) == 10)
            get_movies(sockfd);
        if(strncmp(command, "add_movie", 9) == 0 && strlen(command) == 9)
            add_movie(sockfd);
        if(strncmp(command, "get_movie", 9) == 0 && strlen(command) == 9)
            get_movie(sockfd);
        if(strncmp(command, "update_movie", 12) == 0 && strlen(command) == 12)
            update_movie(sockfd);
        if(strncmp(command, "delete_movie", 12) == 0 && strlen(command) == 12)
            delete_movie(sockfd);
        if (strncmp(command, "logout", 6) == 0 && strlen(command) == 6)
            logout(sockfd);
        if(strncmp(command, "get_collections", 15) == 0 && strlen(command) == 15)
            get_collections(sockfd);
        if(strncmp(command, "get_collection", 14) == 0 && strlen(command) == 14)
            get_collection(sockfd);
        if(strncmp(command, "add_collection", 14) == 0 && strlen(command) == 14)
            add_collection(sockfd);
        if(strncmp(command, "add_movie_to_collection", 23) == 0 && strlen(command) == 23)
            add_movie_to_collection(sockfd, -1, -1);
        if(strncmp(command, "delete_collection", 17) == 0 && strlen(command) == 17)
            delete_collection(sockfd, -1);
        if(strncmp(command, "delete_movie_from_collection", 28) == 0 && strlen(command) == 28)
            delete_movie_from_collection(sockfd);
        if (strncmp(command, "exit", 4) == 0 && strlen(command) == 4){
            if (admin_session_cookie != NULL) {
                free(admin_session_cookie);
                admin_session_cookie = NULL;
            }
            if (user_session_cookie != NULL) {
                free(user_session_cookie);
                user_session_cookie = NULL;
            }
            if (jwt_token != NULL) {
                free(jwt_token);
                jwt_token = NULL;
            }
            break;
        }
    }
    
    // inchid conexiunea
    close_connection(sockfd);

    return 0;
}