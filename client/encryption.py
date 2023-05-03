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
# msg = "hello, Im Muhammed"
# os.system("openssl enc -aes-256-cbc -pbkdf2 -a -in plaintext -out ciphertext")

# create key pair

key = {
    "public":None,
    "private":None
} 

def generateKey():
    key = {}

    if not os.path.exists(".enc/key.pem"): 
        os.system("openssl genpkey -algorithm RSA -out enc/key.pem")
    
    if not os.path.exists(".enc/publicKey.pem"):
        os.system("openssl pkey -in enc/key.pem -pubout -out enc/publicKey.pem")

    key["private"] = open("enc/key.pem",'r').read()
    key["public"] = open("enc/publicKey.pem",'r').read()

def encryptMessage(msg):

    f = open("message",'w')
    f.write(msg)
    f.close()

    os.system("openssl rand -hex 8 >> enc/password.pem")
    os.system("openssl aes-256-cbc -pbkdf2 -a -in message -out enc/encryptedMessage.enc -pbkdf2 -pass pass:[enc/password.pem]")
    os.system("openssl rsautl -encrypt -pubin -inkey enc/publicKey.pem -in enc/password.pem -out enc/encryptedPassword.enc")
    os.system("openssl base64 -in enc/encryptedPassword.enc -out enc/encryptedPassword64.enc")
    os.system("rm enc/password.pem enc/message")
    return open("enc/encryptedMessage.enc",'r').read(), open("enc/encryptedPassword64.enc",'r').read()

def getPubKey():
    if not os.path.exists(".enc/publicKey.pem"):
        return open("enc/publicKey.pem",'r').read()


# print(keys["public"])
# print(keys["private"])

# encryptMessage(input("Enter a locker command (CREATE, DELETE):"))


# opening a socket to the ESP32 from Docker
# Open a socket program on ESP32
    # on client machine and send an ACK or Yes as a request
# GPIO on the ESP32

# locker is centralized
    # containers are clients
    # can spin up 100s of clients
    