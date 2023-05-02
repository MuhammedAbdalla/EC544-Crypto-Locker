#!/bin/bash

device_ipv4=.keys/device_ipv4.pem
AWS_SSH="ssh -i ~/.keys/ec2sshkey.pem ec2-user@3.130.58.56"

ipv4=$(hostname -I)

$AWS_SSH "rm -r EC544-CRYPTO-LOCKER/$device_ipv4"

$AWS_SSH "echo $ipv4 >> EC544-CRYPTO-LOCKER/$device_ipv4"

# $AWS_SSH