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

#include <fstream>

const int BUFFER_SIZE = 256;
const int PLAYERNUM = 2;
const int PORTNUM = 10050;

using namespace std;

//---------------------------------------------
class Block{
public:
    int index_;
    int timestamp_;
    string data_;
    string previous_hash_;
    string hash_;
    string nonce_;

    Block(){index_ = 0; timestamp_ = 0; data_ = ""; previous_hash_ = "";nonce_ ="";} //メンバ変数の初期化

    Block(int index, int timestamp, string data, string previous_hash,string nonce){
        index_ = index;
        timestamp_ = timestamp;
        data_ = data;
        previous_hash_ = previous_hash;
        nonce_ = nonce;
        hash_ = GenerateHash();
    }

public:
    string GenerateHash(){
        //ID、タイムスタンプ、データ、一つ前のハッシュ値から新たなハッシュ値を生成する
        string header;
        string header_hash;
        string double_hash;

        header = to_string(index_) + to_string(timestamp_) + data_ + previous_hash_ + nonce_;
        picosha2::hash256_hex_string(header, header_hash);
        picosha2::hash256_hex_string(header_hash, double_hash);

        return  double_hash;
    }

public:
    void CheckBlockInfo(){
        //ブロックに保存されている内容を表示する
        cout << "index:         " << index_ << endl;
        cout << "timestamp:     " << timestamp_ << endl;
        cout << "data:          " << data_ << endl;
        cout << "previous hash: " << previous_hash_ << endl;
        cout << "nonce: " << nonce_ << endl;
        cout << "hash:          " << hash_ << endl;
        cout << endl;
    }
};

Block CreateGenesisBlock(){
    Block genesis_block(0, time(NULL), "Genesis Block", "0","1234");
    return genesis_block;
}

Block CreateNextBlock(Block last_block, string nonce){
    int32_t this_index = last_block.index_ + 1;
    int64_t this_timestamp = time(NULL);
    string this_data;
    picosha2::hash256_hex_string(to_string(time(NULL)), this_data);
    string this_hash = last_block.hash_;
    string this_nonce =nonce;

    Block next_block(this_index, this_timestamp, this_data, this_hash,this_nonce);
    return next_block;
}

bool judge_nonce(string src, int zero_count){
    bool judge=false;
    int i=0;
    string judge_str;
    string zero_count_str;
    for(i=0;i<zero_count;i++){
        judge_str.push_back(src[i]);
        zero_count_str.push_back('0');
    }
    //cout<<"judge"<<judge_str<<endl;
    if(judge_str==zero_count_str){
        judge=true;
    }
    return judge;
}

string calc_nonce(string src, int zero_count){
    const int N = 1000*1000;
    std::vector<int> v;
    auto start = std::chrono::system_clock::now();

    //    cout<<"CALL calc_nonce: src:"<<src<<endl;

    string result_str;
    string cmp_str;
    string cmp_str2;
    int nonce_value;
    string nonce;
    int count=0;
    bool hash_judge=false;
    stringstream ss;
    while (hash_judge==false){
        nonce_value=rand();
        ss << setfill('0') << setw(8) << hex << nonce_value;
        ss >> nonce;
        ss.str("");
        ss.clear(stringstream::goodbit);
        // cout<<"nonce:"<<nonce<<endl;
        cmp_str=src+nonce;
        picosha2::hash256_hex_string(cmp_str,cmp_str2);
        picosha2::hash256_hex_string(cmp_str2,result_str);
        count++;
        hash_judge=judge_nonce(result_str,zero_count);
    }
    /*
    cout << "count: "<< count << endl;
    cout << "nonce: " << nonce << endl;
    cout << "hash: "<<result_str<<endl;
    */
    auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
    auto dur = end - start;        // 要した時間を計算
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    // 要した時間をミリ秒（1/1000秒）に変換して表示
    //std::cout << msec << " msec \n";

    return nonce;

}


//-------------------------

int main(){
    int sock[PLAYERNUM];
    char buf[BUFFER_SIZE];
    int read_size;
    struct sockaddr_in addr[PLAYERNUM];

    vector<Block> blockchain; //ブロックを保持していくための動的配列
    int num_blocks = 5; // ブロックを追加する回数
    Block previous_block;
    Block current_block;
    string client_nonce;
    int block_num;
    string input_hash;
    string judge_zero_num;
    string nonce;
    double elapsed_time;
    const string dummy_nonce = "00000000";

    cout << "generate_block_num: ";
    cin >> block_num;
    cout << "judge_zero_num: ";
    cin >> judge_zero_num;

    string output_file_name("./data/zero_");
    output_file_name += judge_zero_num;
    output_file_name += "_block_num_";
    output_file_name += to_string(block_num);
    output_file_name += ".cvs";

    ofstream ofs(output_file_name);

    ofs << "elapsed time, nonce, hash" << endl;
    
    blockchain.push_back(CreateGenesisBlock()); // 最初のブロックを用意、追加
    cout << "Create genesis Block" << endl;
    previous_block = blockchain[0];
    previous_block.CheckBlockInfo();
    cout << endl;
    
//---blockchain
    for (int i = 0; i < block_num; i++) {

        current_block = CreateNextBlock(previous_block, dummy_nonce);

        memset(buf, 0, sizeof(buf));
        input_hash = to_string(current_block.index_) +
            to_string(current_block.timestamp_) +
            current_block.data_ +
            current_block.previous_hash_;
        int zero_num = atoi(judge_zero_num.c_str());
        auto start = chrono::system_clock::now();

        nonce=calc_nonce(input_hash,zero_num);

        auto end = chrono::system_clock::now();

        elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        ofs << elapsed_time << ", " << nonce << ", ";
        cout << elapsed_time << ", " << nonce << ", ";

        //nonce の更新
        current_block.nonce_=nonce;
        current_block.hash_=current_block.GenerateHash();

        ofs << current_block.hash_ << endl;
        cout << current_block.hash_ << endl;
        
        previous_block = current_block;

        blockchain.push_back(current_block);
    }

    return 0;
}
