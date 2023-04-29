#!/bin/bash

device_ipv4=.keys/device_ipv4.pem
ls -l
ls -l ~/.keys
AWS_SSH="ssh -i ~/.keys/ec2sshkey.pem ec2-user@18.222.161.150"

server_ipv4=$($AWS_SSH "cat EC544-CRYPTO-LOCKER/$device_ipv4")
client_ipv4=$(hostname -I)


#"CREATE"
#"DELETE"
#"MODIFY"
client_msg=$1

gcc "`pwd`/client.c" -o "client"

# severIP, clientIP, clientMSG
./"client" $server_ipv4 $client_ipv4 $client_msg