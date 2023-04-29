#!/bin/bash

device_ipv4=.keys/device_ipv4.pem
ls -l
ls -l ~/.keys
AWS_SSH="ssh -i ~/.keys/ec2sshkey.pem ec2-user@18.222.161.150"

server_ipv4=$($AWS_SSH "cat EC544-CRYPTO-LOCKER/$device_ipv4")
client_ipv4=$(hostname -I)

#char*
#"CREATE"
#"DELETE"
#"MODIFY"
client_cmd=$1

#uint4_t
#1-24
client_duration=$2

#char*
#"12:30" "2:30"
client_startT=$3
client_endT=$4

gcc "`pwd`/client.c" -o "client"

# severIP, clientIP, clientMSG
./"client" $server_ipv4 $client_ipv4 $client_cmd $client_duration $client_startT $client_endT