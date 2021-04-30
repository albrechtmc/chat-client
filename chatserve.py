#!/usr/bin/env python
#CS372 Introduction to Networking, Project 1
#Chat Server Created by Matthew Albrecht
#May, 5 2019
#This program is a chat server. It starts on the port specified,
#and listens for the client.  It chats with the client until one member
#quits.  Upon quitting, the server begins to listen again for another
#chat client.
#Functions and desctiptions were taken from the following two websites.
#https://docs.python.org/2/howto/sockets.html
#https://docs.python.org/3/library/socket.html
#General outline for a server was taken from https://wiki.python.org/moin/TcpCommunication
import socket
import select 
import sys 
from thread import *

#function to start the server.  It binds the socket to
#the address passed in the command line arguments
#once set the server prepares to listen
#takes no arguments and returns the socket object
#function defenitions taken from the socket library at docs.python.org
def startup ():
    TCP_PORT = sys.argv[2]
    BUFFER_SIZE = 1024

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

    #Connect to the port and listen based on command line arguments
    server.bind((sys.argv[1], int(TCP_PORT))) 
    print("Waiting for Connection")

    server.listen(1) #maximum of one connection
    return server

#function to send the message.  It first checks if the message
#was quit, and closes the connection if it was.  If it was 
#not quit the function reads the line and sends it to the client.
#The message is returned so the main loop can quit if it was a quit message
#takes the connection object, the most recent message, and the quit string
#returns the message that was sent
def sendMessage(conn, message, quit):
    if message != quit: 
        print("{}> {}" .format(handle, message)) 
        message = raw_input('Server> ')

        if message != quit:
            conn.send(message)
    else: 

        message = "Server has Quit"
        conn.send(message)
        conn.close()
    return message

#function to receive the message.
#takes the connection object received from the accept function
#returns the message that was recevied
def receiveMessage(conn):
    message = conn.recv(2048)
    return message


 
server = startup()
loopValue = 1  #while loop will continue until value is changed
newUser = 0 #reset to 1 once a connection is made
quit = "\quit" #string to drop the client

while loopValue: 
    #newUser is set to 1 if there is an active chat
    if newUser == 0: 
        conn, addr = server.accept() 

        handle = conn.recv(2048)
        print("'{}' Connected" .format(handle))
        newUser = 1

        conn.send("'Server' Connected") 

    message = 1
	#run this loop until someone quits the chat
    while message != quit: 
            try: 

                message = receiveMessage(conn)

                if message == "":
                    print("Connection Closed by Client")
                    break
                message = sendMessage(conn, message, quit)

            except: 
                continue
    #tel the client the server has quit
    if message == quit:
        message = "Server has Quit"
        conn.send(message)
    conn.close()
	#listen for a new connection
    print('Listening for connection')
    newUser = 0
    server.listen(100) 
  
conn.close() 
print('Quitting')
server.close() 
exit()