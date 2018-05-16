// for socket
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

//include file for nonce.cpp
#include <ctime>
#include <vector>
#include "./PicoSHA2/picosha2.h"
#include <iomanip>
#include <bitset>

const int CLIENTNUM = 3;
const int BUFFER_SIZE = 128;

int dst_socket[CLIENTNUM];


using namespace std;

//---------------------------------------------
class Block{
public:
    int index_;
    int timestamp_;
    std::string data_;
    std::string previous_hash_;
    std::string hash_;
    std::string nonce_;

    Block(){index_ = 0; timestamp_ = 0; data_ = ""; previous_hash_ = "";nonce_ ="";} //メンバ変数の初期化

    Block(int index, int timestamp, std::string data, std::string previous_hash,std::string nonce){
        index_ = index;
        timestamp_ = timestamp;
        data_ = data;
        previous_hash_ = previous_hash;
        nonce_ = nonce;
        hash_ = GenerateHash();
    }

public:
    std::string GenerateHash(){
        //ID、タイムスタンプ、データ、一つ前のハッシュ値から新たなハッシュ値を生成する
        std::string src_str;
        std::string hash_str;

        src_str = std::to_string(index_) + std::to_string(timestamp_) + data_ + previous_hash_ +nonce_;
        picosha2::hash256_hex_string(src_str, hash_str);


        std::cout << "new hash:  " <<hash_str.front() <<std::endl;
        return  hash_str;
    }

public:
    void CheckBlockInfo(){
        //ブロックに保存されている内容を表示する
        std::cout << "index:         " << index_ << std::endl;
        std::cout << "timestamp:     " << timestamp_ << std::endl;
        std::cout << "data:          " << data_ << std::endl;
        std::cout << "previous hash: " << previous_hash_ << std::endl;
        std::cout << "nonce: " << nonce_ << std::endl;
        std::cout << "hash:          " << hash_ << std::endl;
        std::cout << std::endl;
    }
};

Block CreateGenesisBlock(){
    Block genesis_block(0, time(NULL), "Genesis Block", "0","1234");
    return genesis_block;
}

Block CreateNextBlock(Block last_block, std::string nonce){
    int this_index = last_block.index_ + 1;
    int this_timestamp = time(NULL);
    //std::string this_data = "this is block" + std::to_string(this_index);
    std::string this_data = std::to_string(this_index);
    std::string this_hash = last_block.hash_;
    std::string this_nonce =nonce;

    Block next_block(this_index, this_timestamp, this_data, this_hash,this_nonce);
    return next_block;
}

//-------------------------

int InitializeSocket(struct sockaddr_in &addr){
    int client_data_size;
    int sock;
    int client_addr;

    /*ソケットの作成(TCP/IP通信)*/
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock<0){
        perror("ERROR opening socket");
        exit(1);
    }

    /*ソケットの設定*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10050);
    addr.sin_addr.s_addr = INADDR_ANY;//どれでも要求を受け付ける
    bind(sock,(struct sockaddr*)&addr, sizeof(addr));

    /*TCPクライアントからの接続要求を持てる状態にする*/
    listen(sock,1);
    /*TCPクライアントからの要求を受付*/
    client_data_size = sizeof(client_addr);
    sock = accept(sock,(struct sockaddr*)&client_addr, (socklen_t*)&client_data_size);
    if(sock<0){
        cout << "Connection Failed. " << "\n";
        exit(1);
    }else{
        cout << "Conneted Clients."<< "\n";
    }
    return sock;
}

int main(){
    int sock;
    char buf[BUFFER_SIZE];
    int data;
    struct sockaddr_in addr;

    InitializeSocket(addr);
    sendto(sock,"Connected_Server.",17,0,(struct sockaddr*)&addr,sizeof(addr));

    vector<Block> blockchain; //ブロックを保持していくための動的配列
    int num_blocks = 5; // ブロックを追加する回数
    Block previous_block;
    Block new_block;
    string client_nonce;
    string first_nonce;

    blockchain.push_back(CreateGenesisBlock()); // 最初のブロックを用意、追加
    previous_block = blockchain[0];

//---blockchain
    first_nonce="0000";
    client_nonce=first_nonce;
    new_block = CreateNextBlock(previous_block,client_nonce); // 最後のブロックを引数に取り新たなブロックを生成
    blockchain.push_back(new_block); //ブロックチェーンに追加
    previous_block = new_block;
    cout << "a new block has been added to the blcokchain!" << endl;
    new_block.CheckBlockInfo();

    memset(buf, 0, sizeof(buf));
    data = read(sock, buf, sizeof(buf));
    cout << "157:data: " << data << endl;
    
    string  snd_str;
    snd_str=to_string(new_block.index_) + to_string(new_block.timestamp_) + new_block.data_ + new_block.previous_hash_ ;
    cout<<"block_index"<<snd_str<<endl;

    char* cstr = new char[snd_str.size() + 1];
    strcpy(cstr, snd_str.c_str());
    sendto(sock,cstr,snd_str.size()+1,0,(struct sockaddr*)&addr,sizeof(addr));

    memset(buf, 0, sizeof(buf));
    data = read(sock, buf, sizeof(buf));
    cout << "157:data: " << data << endl;

    
    
//string client_nonce;
    client_nonce=buf;
//printf("%d, %s\n", data, buf);
    cout<<"client_nonce: "<<client_nonce<<endl;
    cout<<"---UPDATE NONCE---"<<client_nonce<<endl;
//hash(client_nonce);

//nonce の更新
    previous_block.nonce_=client_nonce;
    previous_block.hash_=previous_block.GenerateHash();
    previous_block.CheckBlockInfo();

//---blockchain
    new_block = CreateNextBlock(previous_block,client_nonce); // 最後のブロックを引数に取り新たなブロックを生成
    blockchain.push_back(new_block); //ブロックチェーンに追加
    previous_block = new_block;
    cout << "a new block has been added to the blcokchain!" << endl;
    new_block.CheckBlockInfo();
///

    delete[] cstr;







//sendto(sock,"OK.",2,0,(struct sockaddr*)&addr,sizeof(addr));
//----
    snd_str=to_string(new_block.index_) + to_string(new_block.timestamp_) + new_block.data_ + new_block.previous_hash_ ;
    cout<<"block_index"<<snd_str<<endl;

    char* cstr2 = new char[snd_str.size() + 1];
    strcpy(cstr2, snd_str.c_str());
    sendto(sock,cstr2,snd_str.size()+1,0,(struct sockaddr*)&addr,sizeof(addr));

    memset(buf, 0, sizeof(buf));
    data = read(sock, buf, sizeof(buf));
//string client_nonce;
    client_nonce=buf;
//printf("%d, %s\n", data, buf);
    cout<<"client_nonce: "<<client_nonce<<endl;
    cout<<"---UPDATE NONCE---"<<client_nonce<<endl;
//hash(client_nonce);
    cout<<"debug1"<<endl;
//nonce の更新
    previous_block.nonce_=client_nonce;
    previous_block.hash_=previous_block.GenerateHash();
    previous_block.CheckBlockInfo();

    data = read(sock, buf, sizeof(buf));
    client_nonce=buf;

    new_block = CreateNextBlock(previous_block,client_nonce); // 最後のブロックを引数に取り新たなブロックを生成
    blockchain.push_back(new_block); //ブロックチェーンに追加
    previous_block = new_block;
    cout << "a new block has been added to the blcokchain!" << endl;
    new_block.CheckBlockInfo();

    delete[] cstr2;

    return 0;
}
