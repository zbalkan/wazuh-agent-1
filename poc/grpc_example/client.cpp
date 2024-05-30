#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "example.grpc.pb.h"
#include "example.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class XmlClient {
 public:
  XmlClient(std::shared_ptr<Channel> channel) : stub_(XmlService::NewStub(channel)) {}

  std::string SendXml(const std::string& xml_data) {
    XmlRequest request;
    request.set_xml_data(xml_data);
    XmlResponse response;
    ClientContext context;
    Status status = stub_->SendXml(&context, request, &response);
    if (status.ok()) {
      return response.message();
    } else {
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<XmlService::Stub> stub_;
};

int main(int argc, char** argv) {
  XmlClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  std::string xml_data =
  "<group name=\"policy_violation,\">"
  "<rule id=\"17101\" level=\"9\">"
    "<if_group>authentication_success</if_group>"
    "<time>6 pm - 8:30 am</time>"
    "<description>Successful login during non-business hours.</description>"
    "<mitre>"
      "<id>T1078</id>"
    "</mitre>"
    "<group>login_time,pci_dss_10.2.5,pci_dss_10.6.1,gpg13_7.1,gpg13_7.2,gdpr_IV_35.7.d,gdpr_IV_32.2,hipaa_164.312.b,nist_800_53_AU.14,nist_800_53_AC.7,nist_800_53_AU.6,tsc_CC6.8,tsc_CC7.2,tsc_CC7.3,</group>"
  "</rule>";
  std::string reply = client.SendXml(xml_data);
  std::cout << "Client received: " << reply << std::endl;
  return 0;
}
