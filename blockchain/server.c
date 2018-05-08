/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main(){
  int welcomeSocket, newSocket;
  char send_buffer[1024];
  char recv_buffer[1024];
  int recv_nonce=0;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  /*---- Send message to the socket of the incoming connection ----*/
  strcpy(send_buffer,"Hello World\n");
  send(newSocket,send_buffer,13,0);

  recv(newSocket,recv_buffer,1024,0);
  printf("Data received: %s",recv_buffer);


  return 0;
}

void hash_judge(int nonce){
  int version = 4;
  string prev_block = "0000000000000000005629ef6b683f8f6301c7e6f8e796e7c58702a079db14e8";
  string markle_root = "efb8011cb97b5f1599b2e18f200188f1b8207da2884392672f92ac7985534eeb";
  string timestamp = "2016-01-30 13:23:09";
  int timestamp_s=1454160189;
  int bits = 403253488;
  //int nonce=1440181011;

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

  if(double_hash[0]=='0'){
    printf("0");
  }else{
    printf("non 0");
  }
}
