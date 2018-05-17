//for socke
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>

#include <ctime>
#include <vector>
#include "./PicoSHA2/picosha2.h"
#include <iomanip>
#include <bitset>

#include <chrono>
#include <vector>

#include <time.h>
using namespace std;

void hash(string nonce){
    cout <<"----CALL HASH----"<<endl;
    string header_hash;
    string double_hash;
    string header;
    //header = version_h + prev_block_h + markle_root_h + timestamp_h + bits_h + nonce_h;
    header=nonce;
    cout <<"header:"<< header<<endl;
    picosha2::hash256_hex_string(header, header_hash);
    cout<<"header_hash:"<<endl<<header_hash<<endl;
    picosha2::hash256_hex_string(header_hash, double_hash);
    cout<<"double_hash:"<<endl<<double_hash<<endl;
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

int calc_nonce(string src, int zero_count){
    const int N = 1000*1000;
    std::vector<int> v;
    auto start = std::chrono::system_clock::now();

    cout<<"CALL calc_nonce: src:"<<src<<endl;

    string result_str;
    string cmp_str;
    string cmp_str2;
    int nonce=rand();
    int count=0;
    bool hash_judge=false;
    while (hash_judge==false){
        nonce=rand();
        //cout<<"nonce:"<<nonce<<endl;
        cmp_str=src+std::to_string(nonce);
        picosha2::hash256_hex_string(cmp_str,cmp_str2);
        picosha2::hash256_hex_string(cmp_str2,result_str);
        count++;
        hash_judge=judge_nonce(result_str,zero_count);
    }
    cout << "count: "<< count << endl;
    cout << "nonce: " << nonce << endl;
    cout << "hash: "<<result_str<<endl;

    auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
    auto dur = end - start;        // 要した時間を計算
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    // 要した時間をミリ秒（1/1000秒）に変換して表示
    std::cout << msec << " msec \n";

    return nonce;

}

int main(int argc,char *argv[])
{
    // 引数: 0の個数、生成するブロック数、IPアドレス
    struct sockaddr_in addr;
    int sock;
    char buf[256];
    char send_buffer[1024];
    string nonce;
    string dst_ip;
    int dst_port;
    int zero_num;
    int data;
    string team_name;

    srand((unsigned) time(NULL));

    cout << "dst_ip: ";
    cin >> dst_ip;

    cout << "dst_port: ";
    cin >> dst_port;

    cout << "zero_num: ";
    cin >> zero_num;

    cout << "team_name: ";
    cin >> team_name;
    
    /* ソケットの作成 */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    /* 接続先指定用構造体の準備 */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dst_port);
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());

    /* サーバに接続 */
    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        cout << "Connection Failed" << endl;
    }else{
        cout << "Connected Server" << endl;
    }

    /* サーバからデータを受信 */
    memset(buf, 0, sizeof(buf));
    data = read(sock, buf, sizeof(buf));
    cout << "print debug" << endl;
    /*受信したデータを表示*/
    cout << "rcv_msg: " << buf << endl;
    sendto(sock,team_name.c_str(),team_name.size(),0,(struct sockaddr*)&addr,sizeof(addr));

    data = read(sock, buf, sizeof(buf));
    cout << "recv_msg: " << buf << endl;
    int block_loop_count;
    string rcv_str;
    for(;;){
        memset(buf, 0, sizeof(buf));
        data = read(sock, buf, sizeof(buf));
        printf("%d, %s\n", data, buf);

        //string rcv_str;
        rcv_str=buf;
        cout<<"rcv_str: "<<rcv_str<<endl;
        if(rcv_str == "FINISH"){
            break;
        }
        nonce=to_string(calc_nonce(rcv_str,zero_num));

        sendto(sock,nonce.c_str(),nonce.size()+1,0,(struct sockaddr*)&addr,sizeof(addr));
    }

    /* socketの終了 */
    close(sock);

    return 0;
}
