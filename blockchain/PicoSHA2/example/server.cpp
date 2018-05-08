#include <iostream>
#include <ctime>
#include <unistd.h>
#include <vector>
//#include "./PicoSHA2/picosha2.h"
#include "../picosha2.h"
//---------------------------------------------
class Block{
public:
    int index_;
    int timestamp_;
    std::string data_;
    std::string previous_hash_;
    std::string hash_;

    Block(){index_ = 0; timestamp_ = 0; data_ = ""; previous_hash_ = "";} //メンバ変数の初期化

    Block(int index, int timestamp, std::string data, std::string previous_hash){
        index_ = index;
        timestamp_ = timestamp;
        data_ = data;
        previous_hash_ = previous_hash;
        hash_ = GenerateHash();
    }
private:
    std::string GenerateHash(){
                //ID、タイムスタンプ、データ、一つ前のハッシュ値から新たなハッシュ値を生成する
        std::string src_str;
        std::string hash_str;

        src_str = std::to_string(index_) + std::to_string(timestamp_) + data_ + previous_hash_;
        picosha2::hash256_hex_string(src_str, hash_str);
        return  hash_str;
    }
public:
    void CheckBlockInfo(){
                //ブロックに保存されている内容を表示する
        std::cout << "index:         " << index_ << std::endl;
        std::cout << "timestamp:     " << timestamp_ << std::endl;
        std::cout << "data:          " << data_ << std::endl;
        std::cout << "previous hash: " << previous_hash_ << std::endl;
        std::cout << "hash:          " << hash_ << std::endl;
        std::cout << std::endl;
    }
};

Block CreateGenesisBlock(){
    Block genesis_block(0, time(NULL), "Genesis Block", "0");
    return genesis_block;
}

Block CreateNextBlock(Block last_block){
    int this_index = last_block.index_ + 1;
    int this_timestamp = time(NULL);
    std::string this_data = "this is block" + std::to_string(this_index);
    std::string this_hash = last_block.hash_;

    Block next_block(this_index, this_timestamp, this_data, this_hash);
    return next_block;
}

int main() {
    std::vector<Block> blockchain; //ブロックを保持していくための動的配列
        int num_blocks = 5; // ブロックを追加する回数
    Block previous_block;
    Block new_block;

    blockchain.push_back(CreateGenesisBlock()); // 最初のブロックを用意、追加
    previous_block = blockchain[0];


    for(int i=0; i<num_blocks; i++){
        new_block = CreateNextBlock(previous_block); // 最後のブロックを引数に取り新たなブロックを生成
        blockchain.push_back(new_block); //ブロックチェーンに追加
        previous_block = new_block;

        std::cout << "a new block has been added to the blcokchain!" << std::endl;
        new_block.CheckBlockInfo();

        sleep(3); //タイムスタンプをずらすために3秒停止
    }
}

