#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <fstream>
#include <chrono>


#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using helloworld::Greeter;
using helloworld::PutReply;
using helloworld::PutRequest;
using helloworld::GetReply;
using helloworld::GetRequest;
using helloworld::DelReply;
using helloworld::DelRequest;
using helloworld::ConReply;
using helloworld::ConRequest;
using namespace std;
//new edit
using namespace std::chrono;
// //******************************** Client Token ************************************
 int token=-1;
// //******************************** Client Token ************************************
class GreeterClient {
 public:
  explicit GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}
  void PUT(const std::string& key , const std::string& value) {
    PutRequest request;
    request.set_putkey(key);
    request.set_putvalue(value);
    request.set_contoken(token);
    PutReply reply;
    ClientContext context;
    CompletionQueue cq;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<PutReply> > rpc(
        stub_->PrepareAsyncPUT(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&reply, &status, (void*)1);
    void* got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok);
    if (status.ok()) {
      if(reply.puterrorcode() == 200){
        cout<<"The value was inserted successfully"<<endl;
      }
      else{
        cout<<reply.puterror()<<endl;
      }
    } else {
      cout<<"RPC failed"<<endl;
    }
  }

  void GET(const std::string& key ) {
    GetRequest request;
    request.set_getkey(key);
    request.set_contoken(token);
    // request.set_getvalue(value);
    GetReply reply;
    ClientContext context;
    CompletionQueue cq;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<GetReply> > rpc(
        stub_->PrepareAsyncGET(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&reply, &status, (void*)2);
    void* got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)2);
    GPR_ASSERT(ok);
    if (status.ok()) {
      if(reply.geterrorcode() == 200){
        cout<<"The Value is : "<<reply.getvalue()<<endl;
      }
      else{
        cout<<reply.geterror()<<endl;
      }
    } else {
      cout<<"RPC failed"<<endl;
    }
  }

  void DEL(const std::string& key ) {
    DelRequest request;
    request.set_delkey(key);
    request.set_contoken(token);
    // request.set_getvalue(value);
    DelReply reply;
    ClientContext context;
    CompletionQueue cq;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<DelReply> > rpc(
        stub_->PrepareAsyncDEL(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&reply, &status, (void*)3);
    void* got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)3);
    GPR_ASSERT(ok);
    if (status.ok()) {
      if(reply.delerrorcode() == 200){
        cout<<"Deletion was successful."<<endl;
      }
      else{
        cout<<reply.delerror()<<endl;
      }
    } else {
      cout<<"RPC failed"<<endl;
    }
  }
  void CON() {
    ConRequest request;
    // request.set_delkey(key);
    // request.set_getvalue(value);
    ConReply reply;
    ClientContext context;
    CompletionQueue cq;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<ConReply> > rpc(
        stub_->PrepareAsyncCON(&context, request, &cq));
    rpc->StartCall();
    rpc->Finish(&reply, &status, (void*)4);
    void* got_tag;
    bool ok = false;
    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)4);
    GPR_ASSERT(ok);
    if (status.ok()) {
      token = reply.contoken();
    } else {
      cout<<"RPC failed"<<endl;
    }
  }
  private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  
// new edit
  greeter.CON();
  // cout<<token<<endl;
 

//open the log file

  int num_requests=0;

  //starting the client loop
  
 while(1){
     std::string key ,value ,batch_file,function;
  cout <<"enter 1 for interactive mode and 2 for batch mode"<< endl;
   std::string mode_input;
  cin >> mode_input;
  

  
  if(mode_input=="1")
  {  cout<<"Enter the command as GET/PUT/DEL <key> <value> :"<<endl;
     cin>>function;
     if(function=="GET")
     {  
         cin>>key;
         auto start = high_resolution_clock::now();
           greeter.GET(key);
         auto stop = high_resolution_clock::now();  
         num_requests++;      
        auto response_time= duration_cast<microseconds>(stop - start);        
         cout<<"Time Taken "<<response_time.count()<< endl;
  
     }
     else if(function=="PUT")
     {   
         cin>>key>>value;
         auto start = high_resolution_clock::now();
           greeter.PUT(key,value);
         auto stop = high_resolution_clock::now();
         num_requests++;
         auto response_time= duration_cast<microseconds>(stop - start);
          cout<<"Time Taken "<<response_time.count()<< endl;
        
     }
     else if(function=="DEL")
     {
         cin>>key;
         auto start = high_resolution_clock::now();
         greeter.DEL(key);
         auto stop = high_resolution_clock::now();
         num_requests++;
         auto response_time= duration_cast<microseconds>(stop - start);
         cout<<"Time Taken "<<response_time.count()<< endl;

        
     }
    //  cout << function<<" "<<key<<" "<<value;
     else 
     {
         cout<<"WRONG FORMAT"<<endl;
         continue;
     }
    
    
    }



  else if(mode_input=="2")
  {
     cout<<"enter file name or path"<<endl;
     cin>>batch_file;
     std::ifstream infile(batch_file);
        auto start1 = high_resolution_clock::now();
        int maximum=-1;
        int minimum=3492925;
        int totalrequests=0;
        while (infile >> function )
        {
                totalrequests++;
                if(function=="GET")
                {               
                    infile >> key;
                    auto start = high_resolution_clock::now();
                   greeter.GET(key);
                   auto stop = high_resolution_clock::now();
                    auto response_time= duration_cast<microseconds>(stop - start);
                    num_requests++;
                    cout<<"Time Taken "<<response_time.count()<< endl;
                    
                    if(response_time.count()<minimum){
                      minimum = response_time.count();
                    }

                    if(response_time.count()>maximum){
                      maximum = response_time.count();
                    }
                    
                }
                else if(function=="PUT")
                {                   
                    infile >>key>>value;
                     auto start = high_resolution_clock::now();
                     greeter.PUT(key,value);
                      auto stop = high_resolution_clock::now();
                    auto response_time= duration_cast<microseconds>(stop - start);
                    num_requests++;
                    cout<<"Time Taken "<<response_time.count()<< endl;

                    if(response_time.count()<minimum){
                      minimum = response_time.count();
                    }

                    if(response_time.count()>maximum){
                      maximum = response_time.count();
                    }
                }
                else if(function=="DEL")
                {
                    infile >>key;
                     auto start = high_resolution_clock::now();
                     greeter.DEL(key);
                      auto stop = high_resolution_clock::now();
                    auto response_time= duration_cast<microseconds>(stop - start);
                    num_requests++;
                    cout<<"Time Taken "<<response_time.count()<< endl;

                    if(response_time.count()<minimum){
                      minimum = response_time.count();
                    }

                    if(response_time.count()>maximum){
                      maximum = response_time.count();
                    }
                }
             
        }

      auto stop1 = high_resolution_clock::now();
      auto response_time1= duration_cast<microseconds>(stop1 - start1);

      cout<<"Mininum time taken for a request "<<minimum<< endl;
      cout<<"Maximum time taken for a request "<<maximum<< endl;
      cout<<"Total time taken "<<response_time1.count()<<endl;
      cout<<"Average time taken "<<response_time1.count()/totalrequests<<endl;

  }

  else if(mode_input!="1" || mode_input!="2")
  {
      cout<< "WRONG MODE!"<<endl;
     
  }
   
  }
 
  return 0;
}