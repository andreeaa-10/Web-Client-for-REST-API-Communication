#ifndef _HELPER_
#define _HELPER_

#define BUFLEN 4096
#define LINELEN 1000

void error(const char *msg);

void compute_message(char *message, const char *line);

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

void close_connection(int sockfd);

void send_to_server(int sockfd, char *message);

char *receive_from_server(int sockfd);

char *basic_extract_json_response(char *str);

int contains_spaces(char *str);

int check_for_error_and_print(char *response);

char *get_session_cookie(char *message);

void parse_and_print_users(char *response);

char *get_jwt(char *message);

void parse_and_print_movies(char *response);

void parse_and_print_movie(char *response);

void parse_and_print_collections_simple(char *response);

void parse_and_print_collection_details(char *response);

#endif