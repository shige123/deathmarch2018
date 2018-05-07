#include <iostream>
#include <ctime>
#include <unistd.h>
#include <vector>
#include "./PicoSHA2/picosha2.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <bitset>
using namespace std;


// Reverse order byte sequence
void reverse(string src , string *result){
  int i;
  cout <<src.size()<<endl;
  cout <<src[1]<<endl;
  for(i=0;i<=src.size();i=i+2){
    result->push_back(src[src.size()-i]);
    result->push_back(src[src.size()-i+1]);
  }
}

void int2string_hex(int val, string *return_hex){
  int i=0,j=0;
  char henkan[17] = "0123456789ABCDEF";
  char answer[10];
  int bin;

  while(val > 0) {
  // 入力した数値を16で割り、その余りと商をだす
  bin=0;
  j = val % 16;
  val = val / 16;
  answer[i] = henkan[j];
  bin =j;

  j = val % 16;
  val = val / 16;
  answer[i+1] = henkan[j];

  j=j<<4;
  bin=bin+j;

  return_hex->push_back(char(bin));

  i=i+2;
}
}

void hex_string2byte(string src,string *ret){
  cout<<"src---"<<endl;
  int i;
  unsigned int bin;
  char c[2];
  //cout <<src.size()<<endl;
  for(i=2;i<=src.size();i=i+2){
    c[0]=src[src.size()-i];
    c[1]=src[src.size()-i+1];
    //cout<<c[0]<<c[1]<<endl;
    sscanf(c,"%x",&bin);
    //cout<<"bin:"<<bin<<endl;
    ret->push_back(char(bin));
  }
}


int main()
{
  int version = 4;
  string prev_block = "0000000000000000005629ef6b683f8f6301c7e6f8e796e7c58702a079db14e8";
  string markle_root = "efb8011cb97b5f1599b2e18f200188f1b8207da2884392672f92ac7985534eeb";
  string timestamp = "2016-01-30 13:23:09";
  int timestamp_s=1454160189;
  int bits = 403253488;
  int nonce=1;

  string version_h;
  string bits_h;
  string nonce_h;
  string timestamp_h;

  int2string_hex(version,&version_h);
  int2string_hex(bits,&bits_h);
  int2string_hex(nonce,&nonce_h);
  int2string_hex(timestamp_s,&timestamp_h);
  cout <<"v_h:"<< version_h<<endl;
  cout <<"bits_h:"<< bits_h<<endl;
  cout <<"nonce_h:"<< nonce_h<<endl;
  cout <<"timestamp_h:"<< timestamp_h<<endl;

  string prev_block_h;
  string  markle_root_h;

  hex_string2byte(prev_block,&prev_block_h);
  hex_string2byte(markle_root,&markle_root_h);
  cout<<"prev_block_h"<<prev_block_h<<endl;
  cout<<"markle_root_h"<<markle_root_h<<endl;

  string header;
  header = version_h + prev_block_h + markle_root_h + timestamp_h + bits_h + nonce_h;
  cout <<"header:"<< header<<endl;

  string header_hash;
  string double_hash;
  picosha2::hash256_hex_string(header, header_hash);
  cout<<"header_hash:"<<endl<<header_hash<<endl;
  picosha2::hash256_hex_string(header_hash, double_hash);
  cout<<"double_hash:"<<endl<<double_hash<<endl;

  /*string src_str;
  string hash_str;
  src_str="0000000000000000005629ef6b683f8f6301c7e6f8e796e7c58702a079db14e8";
  picosha2::hash256_hex_string(src_str, hash_str);
  cout << "hash_str"<<endl << hash_str<<endl;

  string result_str;
  reverse(hash_str,&result_str);
  cout<<"result_str"<<result_str<<endl;
*/
  return 0;

}
