#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>

class Commander {
private:
    std::vector<nlohmann::json> commandQueue;
    const std::string commandQueueFile = "command_queue.txt";

    // Load command queue from file if it exists
    void load_command_queue() {
        std::ifstream infile(commandQueueFile);
        if (infile.is_open()) {
            std::string line;
            while (std::getline(infile, line)) {
                try {
                    nlohmann::json command = nlohmann::json::parse(line);
                    commandQueue.push_back(command);
                } catch (nlohmann::json::parse_error& e) {
                    std::cerr << "Error parsing line: " << line << std::endl;
                }
            }
            infile.close();
            std::cout << "Command queue loaded from file." << std::endl;
        }
    }

    // Save command queue to file
    void save_command_queue() {
        std::ofstream outfile(commandQueueFile);
        if (outfile.is_open()) {
            for (const auto& command : commandQueue) {
                outfile << command.dump() << std::endl;
            }
            outfile.close();
            std::cout << "Command queue saved to file." << std::endl;
        }
    }

public:
    // Constructor to load command queue from file
    Commander() {
        load_command_queue();
    }

    // Function to receive commands from the user
    void commander_receiver() {
        std::string command_str;
        while (true) {
            std::cout << "Enter command in JSON format (or type 'quit' to stop): ";
            std::getline(std::cin, command_str);
            if (command_str == "quit") {
                break;
            }
            try {
                nlohmann::json command = nlohmann::json::parse(command_str);
                commandQueue.push_back(command);
                save_command_queue();
                std::cout << "Command received and added to the queue." << std::endl;
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "Invalid JSON format. Please try again." << std::endl;
            }
        }
    }

    // Function to process commands from the queue
    void commander_sender() {
        while (true) {
            if (!commandQueue.empty()) {
                nlohmann::json &command = commandQueue.front();

                dispatch_command(command);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    // Simulate the command processing
    void dispatch_command(nlohmann::json &command) {
        if (command["parameters"]["status"] == "pending") {
            command["parameters"]["status"] = "processing";
            std::cout << "Processing command..." << std::endl;
            save_command_queue();
            execute_command(command);
        }

        send_to_stateful(command);
        commandQueue.erase(commandQueue.begin());
        save_command_queue();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Send executed command to stateful queue
    void send_to_stateful(const nlohmann::json& command) {
        // Simulate command sending to stateful queue
        std::cout << std::endl << "Sending command to stateful queue: " << command.dump() << std::endl;
    }

    // Simulate command execute
    void execute_command(const nlohmann::json& command) {
        // Every command type should have the specified processing code
        std::cout << "Status: " << command["command"]["type"] << std::endl;
        if (command["command"]["type"] == "restart-agent") {
            std::cout << "Restarting program..." << std::endl;
            std::exit(0); // For testing purpose, exit to simulate restart
        } else {
            std::cout << "Executing command..." << std::endl;
        }
    }
};

int main() {
    Commander commander;
    std::thread receiverThread(&Commander::commander_receiver, &commander);
    std::thread senderThread(&Commander::commander_sender, &commander);
    receiverThread.join();
    senderThread.join();
    return 0;
}
