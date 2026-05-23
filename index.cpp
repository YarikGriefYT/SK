#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 4096

std::atomic<bool> running(true);
int sock = 0;

// Поток для приёма сообщений от сервера
void receive_messages() {
    char buffer[BUFFER_SIZE];
    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            std::cout << "\n[Disconnected from server]" << std::endl;
            running = false;
            break;
        }
        std::string msg(buffer);
        // Обработка системных сообщений
        if (msg.rfind("PRIVATE ", 0) == 0) {
            // Личное сообщение
            std::string content = msg.substr(8);
            std::cout << "\n[PM] " << content << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("STATUS ", 0) == 0) {
            // Уведомление о смене статуса
            std::string rest = msg.substr(7);
            std::cout << "\n[STATUS] " << rest << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("OFFLINE ", 0) == 0) {
            std::cout << "\n[Offline message] " << msg.substr(8) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("USERS ", 0) == 0) {
            std::cout << "\n[All users] " << msg.substr(6) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("ONLINE ", 0) == 0) {
            std::cout << "\n[Online] " << msg.substr(7) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("FRIENDS ", 0) == 0) {
            std::cout << "\n[Friends] " << msg.substr(8) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("HISTORY ", 0) == 0) {
            std::cout << "\n[History]\n" << msg.substr(8) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("OK ", 0) == 0) {
            std::cout << "[OK] " << msg.substr(3) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("ERROR ", 0) == 0) {
            std::cout << "[ERROR] " << msg.substr(6) << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
        else if (msg.rfind("BYE", 0) == 0) {
            std::cout << "Server closing connection." << std::endl;
            running = false;
            break;
        }
        else {
            std::cout << "\n[SYS] " << msg << std::endl;
            std::cout << "> ";
            std::cout.flush();
        }
    }
}

int main() {
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed. Is server running?" << std::endl;
        return 1;
    }

    std::cout << "Connected to server." << std::endl;
    std::cout << "Commands:\n";
    std::cout << "  /reg username password   - register new user\n";
    std::cout << "  /login username password - login\n";
    std::cout << "  /msg user text           - send private message\n";
    std::cout << "  /list                    - show all registered users\n";
    std::cout << "  /online                  - show online users\n";
    std::cout << "  /addfriend username      - add user to friends\n";
    std::cout << "  /friends                 - show your friends list\n";
    std::cout << "  /history username        - show chat history\n";
    std::cout << "  /exit                    - quit\n";
    std::cout << "----------------------------------------" << std::endl;

    // Запускаем поток приёма сообщений
    std::thread receiver(receive_messages);

    std::string input;
    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (!running) break;
        if (input.empty()) continue;

        if (input == "/exit") {
            send(sock, input.c_str(), input.size(), 0);
            running = false;
            break;
        }

        send(sock, input.c_str(), input.size(), 0);
    }

    receiver.join();
    close(sock);
    return 0;
}