#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "example.grpc.pb.h"
#include "example.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class XmlServiceImpl final : public XmlService::Service {
  Status SendXml(ServerContext* context, const XmlRequest* request, XmlResponse* response) override {
    std::string xml_data = request->xml_data();
    std::cout << "Received XML: " << xml_data << std::endl;
    response->set_message("XML received successfully");
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  XmlServiceImpl service;
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
