FROM ubuntu

WORKDIR /EC544-CRYPTO-LOCKER

COPY . .


RUN /bin/bash
RUN apt-get update
RUN apt-get install gcc git python3 nano build-essential -y

RUN mkdir ~/.keys
RUN chmod 0700 ~/.keys
RUN cp .keys/ec2sshkey.pem  ~/.keys/ec2sshkey.pem
RUN chmod 0400 ~/.keys/ec2sshkey.pem
RUN chmod u+x client/client_setup.sh

ENV PORT=8080

EXPOSE 8080

WORKDIR /EC544-CRYPTO-LOCKER/client

CMD ["./client_setup.sh"]