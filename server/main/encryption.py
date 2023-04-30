import os

# Asymmetric key Summary
# create a message
# encrypt it
# send over TCP
# decrypt it

"""
Variables Files:
        privateKey.pem
        publicKey.pem
        myPassword
        inFile
        outFile
        secretKey.enc -> base64 encryption -> secretKey64.enc
        secretMsg64.enc -> base64 decryption -> secretMsg.enc
        msg
"""

# 1 openssl genpkey -algorithm RSA -out [privateKey.pem]
# 1 openssl pkey -in [privateKey.pem] -pubout -out [publicKey.pem]
# 3 openssl aes-256-cbc -pbkdf2 -a -in [inFile] -out [outFile] -pbkdf2 -pass pass:[password]
# 5 openssl rsautl -encrypt -pubin -inkey [partnerPubKey] -in [myPassword] -out [secretKey.enc]
# 5 openssl base64 -in [secretKey.enc] -out [secretKey64.enc]
# 6 openssl base64 -d -in [secretMsg64.enc] -out [secretMsg.enc]
# 6 openssl rsautl -decrypt -inkey [privateKey.pem] -in [secretMSG64] -out [msg]
# 6 openssl aes-256-cbc -a -d -in [cipertext] -pass pass:[msg]

# openssl sh1/sha256/md5 -in [text file] -out [file]
msg = "hello, Im Muhammed"
os.system("openssl enc -aes-256-cbc -pbkdf2 -a -in plaintext -out ciphertext")

# create key pair

def generateKey():
    key = {}

    if not os.path.exists("./key.pem"): 
        os.system("openssl genpkey -algorithm RSA -out key.pem")
    
    if not os.path.exists("./publicKey.pem"):
        os.system("openssl pkey -in key.pem -pubout -out publicKey.pem")

    key["private"] = open("key.pem",'r').read()
    key["public"] = open("publicKey.pem",'r').read()

    return key

def encryptMessage(msg):

    f = open("message",'w')
    f.write(msg)
    f.close()

    os.system("openssl rand -hex 8 >> password.pem")
    os.system("openssl aes-256-cbc -pbkdf2 -a -in message -out encryptedMessage.enc -pbkdf2 -pass pass:[password.pem]")
    os.system("openssl rsautl -encrypt -pubin -inkey publicKey.pem -in password.pem -out encryptedPassword.enc")
    os.system("openssl base64 -in encryptedPassword.enc -out encryptedPassword64.enc")
    os.system("rm password.pem message")
    return open("encryptedMessage.enc",'r').read(), open("encryptedPassword64.enc",'r').read()



keys = generateKey()
print(keys["public"])
print(keys["private"])

encryptMessage(input("Enter a message: "))


# opening a socket to the ESP32 from Docker
# Open a socket program on ESP32
    # on client machine and send an ACK or Yes as a request
# GPIO on the ESP32

# locker is centralized
    # containers are clients
    # can spin up 100s of clients
    