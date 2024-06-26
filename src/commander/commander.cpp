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
                std::string command_name = command["command"]["name"];

                // Every command type should have the specified processing code
                if (command_name == "restart-agent") {
                    if (command["status"] == "processing") {
                        process_command(command);
                        generate_feedback(command);
                        commandQueue.erase(commandQueue.begin());
                        save_command_queue();
                    } else {
                        command["status"] = "processing";
                        save_command_queue();
                        restart_program();
                    }
                } else {
                    command["status"] = "processing";
                    save_command_queue();
                    process_command(command);
                    generate_feedback(command);
                    commandQueue.erase(commandQueue.begin());
                    save_command_queue();
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    // Simulate the command processing
    void process_command(nlohmann::json &command) {
        std::cout << "Processing command: " << command.dump() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // Generate feedback for the processed command
    void generate_feedback(const nlohmann::json& command) {
        nlohmann::json feedback;
        feedback["command"] = command;
        feedback["status"] = "completed";
        feedback["message"] = "Command processed successfully.";
        std::cout << "Feedback: " << feedback.dump() << std::endl;
    }

    // Simulate program restart
    void restart_program() {
        std::cout << "Restarting program... (simulate)" << std::endl;
        std::exit(0); // For testing purpose, exit to simulate restart
    }
};

int main() {
    Commander commander;
    std::thread receiverThread(&Commander::commander_receiver, &commander);
    std::thread senderThread(&Commander::commander_sender, &commander);
    receiverThread.join();
    senderThread.detach();
    return 0;
}
