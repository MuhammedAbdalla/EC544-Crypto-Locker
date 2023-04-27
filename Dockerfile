FROM dokken/ubuntu-22.04

WORKDIR /app

RUN apt-get update
RUN apt-get install gcc git python3 nano build-essential -y

COPY . .

ENV PORT=8080

EXPOSE 8080

CMD ["./client/client_setup.sh"]