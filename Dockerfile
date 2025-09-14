FROM gcc:latest

WORKDIR /app

COPY . .

RUN g++ -std=c++17 -O3 -Wall -Wextra -o server dictionary.cpp server.cpp

EXPOSE 8080

CMD ["./server"]