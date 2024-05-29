# Web Client

### Description: 
We have to implement a client who communicate with a server using **HTTP** protocol, 
the server is a simply library where we can add a book, delete it,see all the book, login and register a user.
### Implementation:
First I implemented **compute_get_request** and **compute_post_request** where
bot function compute the **GET** and **POST** message what respect the protocol
and with useful information like **cookie** or/and **jwt_token**, the function
**compute_get_request** can also compute a **DELETE** message if we call
the function with delet set as true. After i implement the client where we wait  
for the command from the input for evrey command we open a new connection when we close it.
```
-   register - register the user to server
-   login - login the user to server
-   enter_library - enter the library to have acces to other command
-   get_books - view all books
-   get_book - see the infromation about a book
-   add_book - add a book to library
-   delete_book - delete a book from library
-   logout - logout the user
-   exit - exit the program
```
for the **register** and **login** command we will input username and password 
to login or register, if the user all ready exist or the input is not correct we will show the error message
**enter_libray** give as the jwt_token so we can use other commnad 
,get_books show all books **title+ID**, for get_book we input the id and show
all the detailed about the book ,we show a **error** in the book has not been found, same for delete_book,
 for add_book we have to put this:
```
title=`testbook`
author=`student`
genre=`comedy`
publisher=`PCom`
page_count=`10`
```
then we validate all the filed if are not empty and page_count is number then we send to server.
Logout is the same as login, and exit simple close the program. All the **payload**
send to server is in **JSON** format.

### Reference: 
For json parser i used nlohmann library:
 https://github.com/nlohmann/json
For other function and skelet i used Lab9 from PCom reposetory:
https://pcom.pages.upb.ro/labs/lab9/lecture.html

