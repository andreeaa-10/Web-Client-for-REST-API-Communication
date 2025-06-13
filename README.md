# C HTTP Client for RESTful Web Service

## Overview
This project implements a command-line HTTP client in C that interacts with a RESTful web service for managing users, movies, and movie collections. The client uses POSIX sockets to send HTTP requests, handles session cookies and JWT-based authentication, and parses JSON payloads.

## Program Flow
1. Establish connection with the server.  
2. Read user command from stdin and dispatch to the corresponding handler.  
3. Execute operations until the `exit` command is received.  
4. Close the connection and free allocated resources.  

## Features

### Admin Functions
- **login_admin**: Prompt for admin credentials, send `POST /api/v1/tema/admin/login`, store session cookie on success.  
- **add_user**: Read new user details, send `POST /api/v1/tema/admin/users` with session cookie.  
- **get_users**: Send `GET /api/v1/tema/admin/users` with session cookie to list all users.  
- **delete_user**: Prompt for username, send `DELETE /api/v1/tema/admin/users/:username`.  
- **logout_admin**: Send `GET /api/v1/tema/admin/logout`, clear session cookie.  

### User Functions  
#### Authentication & Library Access  
- **login**: Prompt for user credentials, send `POST /api/v1/tema/user/login`, store session cookie.  
- **get_access**: Send `GET /api/v1/tema/library/access` with session cookie, store returned JWT.  
- **logout**: Send `GET /api/v1/tema/user/logout`, clear session cookie and JWT.  

#### Movie Management  
- **get_movies**: `GET /api/v1/tema/library/movies` with session cookie and JWT.  
- **add_movie**: Prompt for title, year, description, rating; send `POST /api/v1/tema/library/movies`.  
- **get_movie**: Prompt for movie ID; `GET /api/v1/tema/library/movies/:movieId`.  
- **update_movie**: Prompt for ID and updated details; `PUT /api/v1/tema/library/movies/:movieId`.  
- **delete_movie**: Prompt for movie ID; `DELETE /api/v1/tema/library/movies/:movieId`.  

#### Collection Management  
- **get_collections**: `GET /api/v1/tema/library/collections` to list collections.  
- **add_collection**: Create new collection via `POST /api/v1/tema/library/collections`, then add movies.  
- **delete_collection**: Remove collection with `DELETE /api/v1/tema/library/collections/:collectionId`.  
- **add_movie_to_collection**: Prompt for collection ID and movie ID; `POST /api/v1/tema/library/collections/:collectionId`.  
- **delete_movie_from_collection**: Prompt for collection ID and movie ID; `DELETE /api/v1/tema/library/collections/:collectionId/:movieId`.  

### Helper Functions
- **contains_spaces**: Check for spaces in usernames or passwords.  
- **check_for_error_and_print**: Scan server response for “error” and display details.  
- **get_session_cookie**: Extract session cookie from server response.  
- **get_jwt**: Extract JWT token from server response.  
- **parse_and_print_users**: Extract and display user list from JSON.  
- **parse_and_print_movies**: Extract and display movie list from JSON.  
- **parse_and_print_movie**: Extract and display a single movie’s details.  
- **parse_and_print_collections_simple**: Extract and display collection IDs and titles.  
- **parse_and_print_collection_details**: Extract and display full collection details.  

### Additional Functions
- **compute_delete_request**: Build DELETE requests.  
- **compute_put_request**: Build PUT requests.  

_All compute_* functions accept an optional JWT parameter for secure calls._

## Prerequisites
- GCC or compatible C compiler  
- POSIX-compliant environment (Linux, macOS) 

