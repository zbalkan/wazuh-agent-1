#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "realtime.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

class RealTimeServiceImpl final : public RealTimeService::Service {
    Status RealTimeStream(ServerContext* context,
                          ServerReaderWriter<RealTimeMessage, RealTimeMessage>* stream) override {
        RealTimeMessage message;
        while (stream->Read(&message)) {
            std::cout << "Received: " << message.message() << std::endl;
            std::string response = "Server received: " + message.message();
            message.set_message(response);
            stream->Write(message);  // Echo the message back to the client
        }
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    RealTimeServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
