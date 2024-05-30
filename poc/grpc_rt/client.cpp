#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <grpcpp/grpcpp.h>
#include "realtime.grpc.pb.h"
#include "../buffer.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

CircularBuffer message_buffer(100);
std::atomic<bool> keep_running(true);

class RealTimeClient {
public:
    RealTimeClient(std::shared_ptr<Channel> channel)
        : stub_(RealTimeService::NewStub(channel)) {}

    void RealTimeChat() {
        ClientContext context;
        std::shared_ptr<ClientReaderWriter<RealTimeMessage, RealTimeMessage>> stream(
            stub_->RealTimeStream(&context));

        while (keep_running.load()) {  // Explicitly load the atomic flag
            if (!message_buffer.empty()) {
                auto msg = message_buffer.get();
                RealTimeMessage message;
                message.set_message(msg);
                stream->Write(message);
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Control the sending rate
            }
        }

        stream->WritesDone();

        RealTimeMessage message;
        while (stream->Read(&message)) {
            std::cout << "Received: " << message.message() << std::endl;
        }
        grpc::Status status = stream->Finish();
        if (!status.ok()) {
            std::cerr << "RealTimeStream rpc failed." << std::endl;
        }
    }

    void SendMessage(const std::string& text) {
        message_buffer.put(text);
    }

private:
    std::unique_ptr<RealTimeService::Stub> stub_;
};

void client_thread_function(RealTimeClient& client) {
    client.RealTimeChat();
}

int main(int argc, char** argv) {
    RealTimeClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    // Run the client in a separate thread
    std::thread client_thread(client_thread_function, std::ref(client));

    // Main thread for user input
    std::string user_input;
    while (true) {
        std::getline(std::cin, user_input);
        if (user_input == "exit") {
            keep_running.store(false);  // Explicitly store false in the atomic flag
            break;
        }
        client.SendMessage(user_input);
    }

    client_thread.join();
    return 0;
}
