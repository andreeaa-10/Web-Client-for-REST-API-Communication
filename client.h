#ifndef _CLIENT_
#define _CLIENT_

void login_admin(int sockfd);

void add_user(int sockfd);

void get_users(int sockfd);

void delete_user(int sockfd)

void login(int sockfd);

void logout_admin(int sockfd);

void get_access(int sockfd);

void get_movies(int sockfd);

void add_movie(int sockfd);

void get_movie(int sockfd);

void update_movie(int sockfd);

void delete_movie(int sockfd);

void logout(int sockfd);

void get_collections(int sockfd);

void get_collection(int sockfd);

void delete_collection(int sockfd, int id);

int add_movie_to_collection(int sockfd, int collection_id, int movie_id);

void add_collection(int sockfd);

void delete_movie_from_collection(int sockfd);

#endif