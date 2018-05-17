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

#include <chrono>

const int BUFFER_SIZE = 128;
const int PLAYERNUM = 2;
int dst_socket[PLAYERNUM];


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


        // std::cout << "new hash:  " <<hash_str.front() <<std::endl;
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

int InitializeSocket(struct sockaddr_in &addr, int port){
    int addr_size;
    int sock;
;
    /*ソケットの作成(TCP/IP通信)*/
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock<0){
        perror("ERROR opening socket");
        exit(1);
    }
    /*ソケットの設定*/
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;//どれでも要求を受け付ける
    bind(sock,(struct sockaddr*)&addr, sizeof(addr));

    /*TCPクライアントからの接続要求を持てる状態にする*/
    listen(sock,1);
    /*TCPクライアントからの要求を受付*/
    addr_size = sizeof(addr);
    sock = accept(sock,(struct sockaddr*)&addr, (socklen_t*)&addr_size);
    if(sock<0){
        cout << "Connection Failed. " << "\n";
        exit(1);
    }else{
        cout << "Conneted Clients."<< "\n";
    }
    return sock;
}

int main(){
    int sock[PLAYERNUM];
    char buf[BUFFER_SIZE];
    int data;
    struct sockaddr_in addr[PLAYERNUM];
    

    for (int i = 0; i < PLAYERNUM; i++) {
        sock[i] = InitializeSocket(addr[i], 10050+i);
        string connect_message("Player");
        connect_message += to_string(i);
        cout << "connect_message" << connect_message << endl;
        sendto(sock[i],connect_message.c_str(), connect_message.size()+1,
               0,(struct sockaddr*)&addr[i],sizeof(addr[i]));
    }
    vector<Block> blockchain; //ブロックを保持していくための動的配列
    int num_blocks = 5; // ブロックを追加する回数
    Block previous_block;
    Block current_block;
    string client_nonce[PLAYERNUM];
    string  snd_str;
    int block_num = 3;

//    vector<chrono::duration> elapsed_time;
    
    const string dummy_nonce = "0000";
    
    blockchain.push_back(CreateGenesisBlock()); // 最初のブロックを用意、追加
    cout << "Create genesis Block" << endl;
    previous_block = blockchain[0];
    previous_block.CheckBlockInfo();
    cout << endl;
    
//---blockchain
    for (int i = 0; i < block_num; i++) {
        

        for (int team_num = 0; team_num < PLAYERNUM; team_num++) {

            current_block = CreateNextBlock(previous_block, dummy_nonce);

            memset(buf, 0, sizeof(buf));
            snd_str = to_string(current_block.index_) +
                to_string(current_block.timestamp_) +
                current_block.data_ +
                current_block.previous_hash_;

            //   auto start = std::chrono::system_clock::now();

            sendto(sock[team_num],snd_str.c_str(),snd_str.size()+1,0,
                   (struct sockaddr*)&addr[team_num],sizeof(addr[team_num]));

            memset(buf, 0, sizeof(buf));
            data = read(sock[team_num], buf, sizeof(buf));
            //    auto end = std::chrono::system_clock::now();
            //    auto dur = end - start;
            // elapsed_time.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
            client_nonce[team_num]=buf;
            cout<<"team" << team_num <<" nonce: "<<client_nonce[team_num]
                //  <<" elapsed_time: " << elapsed_time[team_num] << "msec"
                << endl;
        }
//nonce の更新
        current_block.nonce_=client_nonce[0];
        current_block.hash_=current_block.GenerateHash();
        current_block.CheckBlockInfo();

        previous_block = current_block;
    }
    return 0;
}
