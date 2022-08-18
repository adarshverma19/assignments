#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <csignal>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iterator>
#include <bits/stdc++.h>

using namespace std;

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::PutRequest;
using helloworld::PutReply;
using helloworld::GetRequest;
using helloworld::GetReply;
using helloworld::DelRequest;
using helloworld::DelReply;
using helloworld::ConReply;
using helloworld::ConRequest;


 
///////////////////////////GLOBAL VARIABLES//////////////////////////////////////

const int totalfiles = 53;
string filesdirectory = "../../filestore/";
string filecontextdir = "../../context/";
string logfilename = "../../logs/logfile";
int logfiledc;
string filenames[totalfiles] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z", 
                                "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0"};

int fileoffsets[totalfiles];

map<string, int> filelocationmap;
int fds[totalfiles];

int listeningport = -1;
int cachetype = -1;
int cachecapacity = 0; 
int threadpoolsize = 0 ;


std::mutex clientcountmutex;
std::mutex getmutex;
std::mutex putmutex;
std::mutex delmutex;
std::mutex logmutex;

//**************** For client token ********************
int clientcount=0 ;
//************************************ // jatin
struct Node {
    string key, value;
    int cnt;
    Node *next; 
    Node *prev;
    Node(string _key, string _value) {
        key = _key;
        value = _value; 
        cnt = 1; 
    }
}; 


///////////////////////////CACHES//////////////////////////////////////

struct List {
    int size; 
    Node *head; 
    Node *tail; 
    List() {
        head = new Node("0","0"); 
        tail = new Node("0","0"); 
        head->next = tail;
        tail->prev = head; 
        size = 0;
    }
    
    void addtoFront(Node *node) {
        Node* temp = head->next;
        node->next = temp;
        node->prev = head;
        head->next = node;
        temp->prev = node;
        size++; 
    }
    
    void removeNode(Node* delnode) {
        Node* delprev = delnode->prev;
        Node* delnext = delnode->next;
        delprev->next = delnext;
        delnext->prev = delprev;
        size--; 
    }
    
    
    
};
class LFUCache {
    map<string, Node*> knMap; 
    map<int, List*> flMap; 
    int mxsize;
    int minFreq; 
    int currentSize; 
public:
    LFUCache(int capacity) {
        mxsize = capacity; 
        minFreq = 0;
        currentSize = 0; 
    }
    void updateflMap(Node *node) {
        knMap.erase(node->key); 
        flMap[node->cnt]->removeNode(node); 
        if(node->cnt == minFreq && flMap[node->cnt]->size == 0) {
            minFreq++; 
        }
        
        List* nextHigherFreqList = new List();
        if(flMap.find(node->cnt + 1) != flMap.end()) {
            nextHigherFreqList = flMap[node->cnt + 1];
        } 
        node->cnt += 1; 
        nextHigherFreqList->addtoFront(node); 
        flMap[node->cnt] = nextHigherFreqList; 
        knMap[node->key] = node;
    }
    
    string get(string key) {
        if(knMap.find(key) != knMap.end()) {
            Node* node = knMap[key]; 
            string val = node->value; 
            updateflMap(node); 
            return val; 
        }
        return "-1"; 
    }
    
    void put(string key, string value) {
        if (mxsize == 0) {
            return;
        }
        if(knMap.find(key) != knMap.end()) {
            Node* node = knMap[key]; 
            node->value = value; 
            updateflMap(node); 
        }
        else {
            if(currentSize == mxsize) {
                List* list = flMap[minFreq]; 
                knMap.erase(list->tail->prev->key); 
                flMap[minFreq]->removeNode(list->tail->prev);
                currentSize--; 
            }
            currentSize++; 
            // new value has to be added who is not there previously 
            minFreq = 1; 
            List* listFreq = new List(); 
            if(flMap.find(minFreq) != flMap.end()) {
                listFreq = flMap[minFreq]; 
            }
            Node* node = new Node(key, value); 
            listFreq->addtoFront(node);
            knMap[key] = node; 
            flMap[minFreq] = listFreq; 
        }
    }

    int del(string key){
        if(knMap.find(key) != knMap.end()) {
            Node* node = knMap[key]; 
            knMap.erase(node->key); 
            flMap[node->cnt]->removeNode(node); 
        }
        return 0; 
    }

    string cachestate(){
        string state = "";
        for(auto it = knMap.cbegin(); it != knMap.cend(); ++it){
            state = state + it->first + " ";
        }
    return state;
    }

};


class LRUCache {
public:
    class node {
        public:
            string key;
            string val;
            node* next;
            node* prev;
        node(string _key, string _val) {
            key = _key;
            val = _val; 
        }
    };

    node* head = new node("-1","-1");
    node* tail = new node("-1","-1");

    int cap;
    unordered_map<string, node*>m;

    LRUCache(int capacity) {
        cap = capacity;    
        head->next = tail;
        tail->prev = head;
    }

    void addnode(node* newnode) {
        node* temp = head->next;
        newnode->next = temp;
        newnode->prev = head;
        head->next = newnode;
        temp->prev = newnode;
    }

    void deletenode(node* delnode) {
        node* delprev = delnode->prev;
        node* delnext = delnode->next;
        delprev->next = delnext;
        delnext->prev = delprev;
    }

    string get(string k) {
        if (m.find(k) != m.end()) {
            node* resnode = m[k];
            string res = resnode->val;
            m.erase(k);
            deletenode(resnode);
            addnode(resnode);
            m[k] = head->next;
            return res; 
        }

        return "-1";
    }

    void put(string k, string value) {
        if(m.find(k) != m.end()) {
            node* existingnode = m[k];
            m.erase(k);
            deletenode(existingnode);
        }
        if(m.size() == cap) {
          m.erase(tail->prev->key);
          deletenode(tail->prev);
        }

        addnode(new node(k, value));
        m[k] = head->next; 
    }

    void del(string k){
        if(m.find(k) != m.end()) {
            node* existingnode = m[k];
            m.erase(k);
            deletenode(existingnode);
        }
    }

    string cachestate(){
        string state = "";
        for(auto it = m.cbegin(); it != m.cend(); ++it){
            state = state + it->first + " ";
        }
    return state;
    }


};


///////////////////////////OPEN CLOSE FILES//////////////////////////////////////

int openfiledc(){
    for( int i =0;i<totalfiles;i++){
        string filename = filesdirectory+filenames[i];
        const char* f = filename.c_str();
        fds[i] = open(f, O_CREAT | O_RDWR | O_APPEND,0666);
        // cout<<"Opened file with name "+filename<<"\n";
    }
     return 0; 
}

int closefiledc(){
        for( int i =0;i<totalfiles;i++){
        close( fds[i]);
    }
     return 0; 
}

string trim(string str){
    int index = str.find_first_of(" ");
    return str.substr(0, index);
}


///////////////////////////CONTEXT SAVE and RETREIVE//////////////////////////////////////

int contexttofile(){

    ofstream filecontextout;
    ofstream fileoffsetout;

    string filecontextname = filecontextdir+"filecontext";
    string fileoffsetname = filecontextdir+"fileoffsets";

    filecontextout.open(filecontextname);
    
    for(auto it = filelocationmap.cbegin(); it != filelocationmap.cend(); ++it){
            filecontextout<< it->first <<" "<< it->second<<"\n";
    }
    filecontextout.close();

    fileoffsetout.open(fileoffsetname);
    for(int i =0; i<totalfiles;i++){
            fileoffsetout << fileoffsets[i] <<"\n";
    }

    fileoffsetout.close();
    return 0;
}

int initializefileinfo(){

    // Open Log file , create one if it doesnt exist

    logfiledc = open(logfilename.c_str(), O_CREAT | O_RDWR | O_APPEND,0666);

    // 

    ifstream filecontextif;
    ifstream fileoffsetif;

    string filecontextname = filecontextdir+"filecontext";
    string fileoffsetname = filecontextdir+"fileoffsets";

    filecontextif.open(filecontextname);

    string key; int offset;
    while (filecontextif >> key >> offset)
    {
        filelocationmap[key] = offset;
    }

    filecontextif.close();

    fileoffsetif.open(fileoffsetname);

    int o;
    int i=0;
    while(fileoffsetif>>o){
        fileoffsets[i] = o;
        i++; 
    }

    fileoffsetif.close();
    return 0;
}

///////////////////////////FILE OPERATIONS//////////////////////////////////////


int putkey(string key, string value){

    char firstletter = key[0];
    int keylength = key.length();
    int valuelength = value.length();
    string enckey, encvalue;

    if(keylength<256){
         string str = "";
         enckey = key + str.insert(0, 256-keylength, ' ');
    }

    if(valuelength<256){
         string str = "";
         encvalue = value + str.insert(0, 256-valuelength, ' ');
    }


    if(firstletter>='A' && firstletter<='Z' ){
        write(fds[firstletter -'A'], (enckey+encvalue).c_str(),512);
        filelocationmap[key]= fileoffsets[firstletter -'A'];
        fileoffsets[firstletter -'A']++;
    }
    else if(firstletter>='a' && firstletter<='z'){
         write(fds[firstletter -'a' +26], (enckey+encvalue).c_str(),512);
        // fds[firstletter -'a' +26].w<<enckey<<encvalue;
        filelocationmap[key]= fileoffsets[firstletter -'a' +26];
        fileoffsets[firstletter -'a' +26]++;
    }
    else{
        write(fds[52], (enckey+encvalue).c_str(),512);
        // fds[52]<<enckey<<encvalue;
        filelocationmap[key] = fileoffsets[52];         
        fileoffsets[52]++;
    }
   
    return 0;

}


string getkey(string key){

   char firstletter = key[0];
   auto it  = filelocationmap.find(key);

    if(it==filelocationmap.end()){
        return "-1";
    }

   int offset = it->second;

   int valueindex = offset*512 + 256;
   char valuebuffer[256];
   
    if(firstletter>='A' && firstletter<='Z' ){
        lseek(fds[firstletter -'A'],valueindex,SEEK_SET);
        read(fds[firstletter -'A'],valuebuffer,256);
    }
    else if(firstletter>='a' && firstletter<='z'){
         lseek(fds[firstletter -'a' +26],valueindex,SEEK_SET);
         read(fds[firstletter -'a' +26],valuebuffer,256);   
    }
    else{
         lseek(fds[52],valueindex,SEEK_SET);
         read(fds[52],valuebuffer,256);
    }

  string value(valuebuffer);

    return trim(value);

}



int deletekey(string key){

    auto it = filelocationmap.find(key);

    if(it!=filelocationmap.end()){
        filelocationmap.erase(key);
    }
    else{
    return 0;
    }

    return 1;
}


///////////////////////////OBJECT CREATION//////////////////////////////////////
LRUCache* lrucache;
LFUCache* lfucache;


///////////////////////////MAIN GET PUT DEL FUNCTIONS//////////////////////////////////////


string getcachestate(){
    if(cachetype==1){
        return "CACHE KEYS : " + lrucache->cachestate() +"\n";
    }
    else if(cachetype==2){
        return "CACHE KEYS : " + lfucache->cachestate() +"\n";
    }
    return "INVALID CACHE NUMBER";
}



string get(string key){
    string resvalue;
    if(cachetype==1){
        getmutex.lock();
        string value = lrucache->get(key);
        
        if(value!="-1"){
            resvalue =  value;
        }
        else{
            string filevalue =  getkey(key);

            if(filevalue !="-1"){
                lrucache->put(key,filevalue);
                resvalue = filevalue;
            }
            else{
                resvalue =  "";
            }
        }

        getmutex.unlock();
    }

    if(cachetype==2){
        getmutex.lock();
        string value = lfucache->get(key);
        if(value!="-1"){
            getmutex.unlock();
            return value;
        }
        else{

            string filevalue =  getkey(key);

            if(filevalue !="-1"){
                lfucache->put(key,filevalue);
                resvalue =  filevalue;
            }
            else{
                resvalue = "";
            }
        }

        getmutex.unlock();

    }

  return resvalue ;
    
}

int put(string key, string value){

    getmutex.lock();
    putkey(key,value);
    getmutex.unlock();

    return 0; 
}

int del(string key){
    
    if(cachetype==1){
        getmutex.lock();
        lrucache->del(key);
        int status = deletekey(key);
        getmutex.unlock();

        return status;

        // if(status ==-1){
        //    return "KEY NOT EXIST";
        // }
    }

    if(cachetype==2){
        getmutex.lock();
        lfucache->del(key);
        getmutex.unlock();
        return deletekey(key);

        // if(status ==-1){
        //    return "KEY NOT EXIST";
        // }   
    }

    return 0;
}

/////////////////////////////CREATING LOGS////////////////////////////////////

void createlog(string message){
    logmutex.lock();
    write(logfiledc, message.c_str(),message.size());
    logmutex.unlock();
}


/////////////////////////////CONFIG FILE//////////////////////////////////////

void readconfigfile(){
    ifstream configfile;
    configfile.open("../../config");

    configfile >> listeningport;
    configfile >> cachetype;
    configfile >> cachecapacity;
    configfile >> threadpoolsize; 

    if(cachetype==1){
        lrucache = new LRUCache(cachecapacity);
    }

    if(cachetype==2){
        lfucache = new LFUCache(cachecapacity);
    }

    configfile.close();
}


///////////////////////////SETUP EVERYTHING//////////////////////////////////////

void setupeverything(){
    openfiledc();
    readconfigfile();
    initializefileinfo();

}

void setdowneverything(){
    contexttofile();
    closefiledc();
}



int SUCCESS=200;
int FAILURE=400;


class ServerImpl final {
 public:
  ~ServerImpl() {
    server_->Shutdown();
    for (const auto& cq : compl_queues_)
        cq->Shutdown();
  }

void Run() {
    std::string server_address("0.0.0.0:50051");
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    for(auto i{0}; i<threadpoolsize; i++)
        compl_queues_.emplace_back(builder.AddCompletionQueue());
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;
    std::vector<std::thread> threads;
    for(auto i{0}; i<threadpoolsize; i++)
        threads.emplace_back(std::thread(&ServerImpl::HandleRpcs, this, compl_queues_[i].get()));
    for(auto i{0}; i<threads.size(); i++)
        threads[i].join();
  }

 private:
  class CallData {
   public:
    virtual void Proceed() = 0;
  };


  class PutCallData final: public CallData{
   public:
    PutCallData(Greeter::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      Proceed();
    }

    void Proceed() {
        // cout<<"from put call"<<endl;
      if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestPUT(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == PROCESS) {
        new PutCallData(service_, cq_);
        int len1 = request_.putkey().length();
        int len2 = request_.putvalue().length();
       
        // tokencounter<<request_.contoken();

        if(len1==0 || len2==0){
          reply_.set_puterror("Either key or Value is null");
          createlog(to_string(request_.contoken())+" PUT: PARAMETERS : "+request_.putkey()+" : "+request_.putvalue() + " , RESULT: KEY OR VALUE IS NULL , STATUS: FAILURE\n");
          reply_.set_puterrorcode(FAILURE);
        }
        else if(len1 > 256 || len2> 256){
          reply_.set_puterror("Either key or Value is greater than 256 Bytes.");
           createlog(to_string(request_.contoken())+" PUT: PARAMETERS : "+request_.putkey()+" : "+request_.putvalue() + " , RESULT: KEY OR VALUE GREATER THAN 256 BYTES , STATUS: FAILURE\n");
          reply_.set_puterrorcode(FAILURE);
        }
        else{
            put(request_.putkey(),request_.putvalue());
            createlog(to_string(request_.contoken())+" PUT: PARAMETERS : "+request_.putkey()+" : "+request_.putvalue() + " , RESULT: KEY-VALUE PUT , STATUS: SUCCESS\n");
            createlog(getcachestate());
          reply_.set_puterror("");
          reply_.set_puterrorcode(SUCCESS);
        }
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
      }
    }

   private:
    Greeter::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    PutRequest request_;
    PutReply reply_;
    ServerAsyncResponseWriter<PutReply> responder_;
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

class GetCallData final: public CallData{
   public:
    GetCallData(Greeter::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      Proceed();
    }

    void Proceed() {
        // cout<<"from get call"<<endl;
      if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestGET(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == PROCESS) {
        new GetCallData(service_, cq_);
        string value = get(request_.getkey());
        if(value==""){
          createlog(to_string(request_.contoken())+" GET: PARAMETERS : "+request_.getkey()+" , RESULT: KEY NOT EXIST , STATUS: FAILURE\n");
          reply_.set_getvalue("");
          reply_.set_geterrorcode(FAILURE);
          reply_.set_geterror("The key-value pair does not exist.");
        }
        else{
          createlog(to_string(request_.contoken())+" GET: PARAMETERS : "+request_.getkey()+" , RESULT: "+value+" , STATUS: SUCCESS\n");
          createlog(getcachestate());          
          reply_.set_getvalue(value);
          reply_.set_geterrorcode(SUCCESS);
          reply_.set_geterror("");

        }
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
      }
    }

   private:
    Greeter::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    GetRequest request_;
    GetReply reply_;
    ServerAsyncResponseWriter<GetReply> responder_;
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

  class DelCallData final: public CallData{
   public:
    DelCallData(Greeter::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      Proceed();
    }

    void Proceed() {
      if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestDEL(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == PROCESS) {
        new DelCallData(service_, cq_);
        int delstatus = del(request_.delkey());
        if(delstatus){
          createlog(to_string(request_.contoken())+" DEL: PARAMETERS : "+request_.delkey()+" , RESULT: KEY-VALUE DELETED , STATUS: SUCCESS\n");
          createlog(getcachestate());
          reply_.set_delerrorcode(SUCCESS);
          reply_.set_delerror("");
        }
        else{
          createlog(to_string(request_.contoken())+" DEL: PARAMETERS : "+request_.delkey()+" , RESULT: KEY NOT EXIST , STATUS: FAILURE\n");
          reply_.set_delerrorcode(FAILURE);
          reply_.set_delerror("The key does not exist");
        }
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
      }
    }

   private:
    Greeter::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    DelRequest request_;
    DelReply reply_;
    ServerAsyncResponseWriter<DelReply> responder_;
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

  class ConCallData final: public CallData{
   public:
    ConCallData(Greeter::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      Proceed();
    }

    void Proceed() {
        // cout<<"from put call"<<endl;
      if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestCON(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == PROCESS) {
        new ConCallData(service_, cq_);
        reply_.set_contoken(clientcount);
        createlog("Connection Established with : " + to_string(clientcount) + "\n");
        clientcountmutex.lock();
        clientcount++;
        clientcountmutex.unlock();
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
      }
    }

   private:
    Greeter::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    ConRequest request_;
    ConReply reply_;
    ServerAsyncResponseWriter<ConReply> responder_;
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

  void HandleRpcs(grpc::ServerCompletionQueue* cq) {
    new PutCallData(&service_, cq);
    new GetCallData(&service_, cq);
    new DelCallData(&service_, cq);
    new ConCallData(&service_, cq);

    void* tag;  
    bool ok;
    while (true) {
      GPR_ASSERT(cq->Next(&tag, &ok));
      GPR_ASSERT(ok);
      static_cast<CallData*>(tag)->Proceed();
    }
  }

//   std::unique_ptr<ServerCompletionQueue> cq_;
  std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> compl_queues_;
  Greeter::AsyncService service_;
  std::unique_ptr<Server> server_;
};
void signalHandler(int signum) { 
   setdowneverything();
	  cout<<"\nserver shutting down"<<endl;
   exit(0);  
}
int main(int argc, char** argv) {
  cout<<"Setting up Server.......\n";
  signal(SIGINT, signalHandler);   
  setupeverything();
  createlog("SERVER STARTED SUCCESSFULLY\n");
  ServerImpl server;
  server.Run();

  return 0;
}
