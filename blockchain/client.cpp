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

int main()
{
struct sockaddr_in addr;
int sock;
char buf[32];
char send_buffer[1024];
string nonce="1234";

int data;

 /* ソケットの作成 */
 sock = socket(AF_INET, SOCK_STREAM, 0);
 /* 接続先指定用構造体の準備 */
 addr.sin_family = AF_INET;
 addr.sin_port = htons(10050);
 addr.sin_addr.s_addr = inet_addr("127.0.0.1");

 /* サーバに接続 */
 connect(sock, (struct sockaddr*)&addr, sizeof(addr));

 /* サーバからデータを受信 */
 memset(buf, 0, sizeof(buf));
 data = read(sock, buf, sizeof(buf));

 /*受信したデータを表示*/
 printf("%d, %s\n", data, buf);

 char* cstr = new char[nonce.size() + 1];
 strcpy(cstr, nonce.c_str());
 sendto(sock,cstr,nonce.size()+1,0,(struct sockaddr*)&addr,sizeof(addr));

 nonce="5678";
 memset(buf, 0, sizeof(buf));
 data = read(sock, buf, sizeof(buf));
 printf("%d, %s\n", data, buf);

 strcpy(cstr, nonce.c_str());
 sendto(sock,cstr,nonce.size()+1,0,(struct sockaddr*)&addr,sizeof(addr));
 delete[] cstr;


 /* socketの終了 */
 close(sock);

 return 0;
}
