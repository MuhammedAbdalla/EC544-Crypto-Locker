# import the socket library
import socket
import sys
from datetime import date
from datetime import datetime
import encryption


# keys
encryption.generateKey()
publicKey = encryption.getPubKey()


# datetime object containing current date and time
now = datetime.now()
 
print("now =", now)

# dd/mm/YY H:M:S
dt_string = now.strftime("%d/%m/%Y %H:%M:%S")
print("date and time =", dt_string)


#set the name of the server machine
#change it to the name of your machine!
host = '3.130.58.56' 
#set port number of server to connect on
#must be same as set in server file
port = 8080
#create TCP/IP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
hostname = socket.gethostname()
IPAddr = socket.gethostbyname(hostname)

#sets maximum size of a message
size = 1024
#connect to server

#create message
#change the message with your first name and last name!
for i in range(0,10):
    message = ['CREATE',hostname+str(i),IPAddr,dt_string,"key","","",i,"\0"]
    #encode message

    #send message to server
    for i in range(0,len(message)):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host,port))
        s.send(str(message[i]).encode())
        s.close()


commands = [
    ['DISPLAY',hostname+str(0),"","","","","","","\0"],
    ['DELETE',hostname+str(0),"","","","","","","\0"],
    ['DELETE',hostname+str(1),"","","","","","","\0"],
    ['DELETE',hostname+str(2),"","","","","","","\0"],
    ['DISPLAY',hostname+str(0),"","","","","","","\0"]
]


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))

# message = commands[0]
# for i in range(0,len(message)):
#     s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     s.connect((host,port))
#     s.send(str(message[i]).encode())
#     s.close()

for cmd in commands:
    for msg in cmd:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host,port))
        s.send(str(msg).encode())
        s.close()

