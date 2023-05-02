# import the socket library
import socket
#set the name of the server machine
#change it to the name of your machine!
host = '3.130.58.56'
#set port number of server to connect on
#must be same as set in server file
port = 8080
#create TCP/IP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#sets maximum size of a message
size = 1024
#connect to server
s.connect((host,port))
#create message
#change the message with your first name and last name!
message = 'FETCH'
#encode message
encodedMessage = message.encode()
#send message to server
s.send(encodedMessage)
#get message from server
data = s.recv(size)
#decode message from server
decodedData = data.decode()
#close socket
s.close()
#print message
print('[AWS]', decodedData)


s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2.connect((decodedData,8080))

s2.send("CREATE")
#get message from server
data = s2.recv(size)
#decode message from server
decodedData = data.decode()
#close socket
s2.close()
#print message
print('[LOCKER]', decodedData)
