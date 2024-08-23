#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>

class Server {
public:
    Server(int port) : server_port(port) {}

    void start() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(server_port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0) {
            perror("Listen failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "Server listening on port " << server_port << std::endl;

        while (true) {
            sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int client_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);

            if (client_socket >= 0) {
                std::thread(&Server::handle_client, this, client_socket).detach();
            }
        }
    }

private:
    int server_port;
    std::mutex log_mutex;

    void handle_client(int client_socket) {
        char buffer[1024] = {0};
        ssize_t valread = read(client_socket, buffer, 1024);

        if (valread > 0) {
            std::lock_guard<std::mutex> lock(log_mutex);

            std::ofstream log_file("log.txt", std::ios::app);
            if (log_file.is_open()) {
                log_file << buffer << std::endl;
                log_file.close();
            } else {
                std::cerr << "Failed to open log file" << std::endl;
            }
        }

        close(client_socket);
    }
};

int main(int argc, char const* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    Server server(port);
    server.start();

    return 0;
}
