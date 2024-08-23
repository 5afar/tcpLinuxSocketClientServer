#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class Client {
public:
    Client(const std::string& name, const std::string& server_ip, int server_port, int period)
        : client_name(name), server_ip(server_ip), server_port(server_port), period(period) {}

    void start() {
        while (true) {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                perror("Socket creation error");
                exit(EXIT_FAILURE);
            }

            sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(server_port);

            if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
                perror("Invalid address or Address not supported");
                close(sock);
                exit(EXIT_FAILURE);
            }

            if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                perror("Connection Failed");
                close(sock);
                exit(EXIT_FAILURE);
            }

            std::string message = get_current_time() + " \"" + client_name + "\"";
            send(sock, message.c_str(), message.length(), 0);

            close(sock);

            std::this_thread::sleep_for(std::chrono::seconds(period));
        }
    }

private:
    std::string client_name;
    std::string server_ip;
    int server_port;
    int period;

    std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm* tm_info = std::localtime(&time);

        char buffer[26];
        strftime(buffer, 26, "[%Y-%m-%d %H:%M:%S", tm_info);

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        return std::string(buffer) + "." + std::to_string(ms.count()) + "]";
    }
};

int main(int argc, char const* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <name> <server_port> <period>" << std::endl;
        return 1;
    }

    std::string name = argv[1];
    int port = std::stoi(argv[2]);
    int period = std::stoi(argv[3]);

    Client client(name, "127.0.0.1", port, period);
    client.start();

    return 0;
}
