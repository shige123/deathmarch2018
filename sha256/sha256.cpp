#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//動的確保はまだ
#define N 1024

const unsigned int K[64]=
  {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

//基本的な操作
unsigned int rotr(unsigned int x, unsigned int n)
{
  return (x >> n)|(x << (32 - n));
}

unsigned int shr(unsigned int x, unsigned int n)
{
  return (x >> n);
}

//各関数
unsigned int ch(unsigned int x,unsigned int y,unsigned int z)
{
  return (x & y)^(~x & z);
}

unsigned int maj(unsigned int x, unsigned int y, unsigned int z){
  return (x & y)^(x & z)^(y & z);
}

unsigned int large_sigma0(unsigned int x)
{
  return rotr(x,2) ^ rotr(x, 13) ^ rotr(x,22);
}

unsigned int large_sigma1(unsigned int x)
{
  return rotr(x,6) ^ rotr(x, 11) ^ rotr(x,25);
}

unsigned int small_sigma0(unsigned int x)
{
    return rotr(x,7) ^ rotr(x, 18) ^ shr(x,3);
}

unsigned int small_sigma1(unsigned int x)
{
    return rotr(x,17) ^ rotr(x, 19) ^ shr(x,10);
}

//int main(int argc, char *argv[])
int main(void)
{
  int i,j,t;
  unsigned int l_byte,l_bit,k;
  unsigned int one;
  unsigned int kn,km;
  int n;

  unsigned int a,b,c,d,e,f,g,h,t1,t2;
  unsigned int W[64];

  unsigned int H[N][8];
  unsigned int M[N][16];

  unsigned int tmp1,tmp2,tmp3,tmp4;
  //適当な入力
  char *s = "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD";
  
  //入力メッセージの長さ[Byte]を取得
  l_byte = strlen(s);
  l_bit = l_byte * 8;
  //  k = 448 - (l+1); // 入力データ長が447bitを超えないとき限定

  //メッセージブロック数
  //"入力バイト数(l)"+"0x8000(1byte)"+ "length(8byte)"
  n = ((l_byte + 1 + 8) / 64) + 1;
  one = 0x80000000 >> (l_byte%4)*8;

  memset(M,0,64*4*n);
  memcpy(M,s,l_byte);
  
  //エンディアン対応
  for(i=0;i<n;i++){
	  for(j=0;j<64;j++){
		  tmp1 = M[i][j] & 0x000000ff; //4byte目を一時待避
		  tmp2 = M[i][j] & 0x0000ff00; //3byte目を一時待避
		  tmp3 = M[i][j] & 0x00ff0000; //2byte目を一時待避
		  tmp4 = M[i][j] & 0xff000000; //1byte目を一時待避
		  M[i][j] = (tmp1 << 24) + (tmp2 << 8) + (tmp3 >> 8) + (tmp4 >> 24);
	  }
  }

  kn = n-1;
  km = (l_byte % 64)/4;
  M[kn][km] = M[kn][km] + one;

  memset(&M[kn][km+1],0,64-8);
  M[kn][15]=l_bit;
//  printf("%x\n",M[0][0]);


  H[0][0] = 0x6a09e667;
  H[0][1] = 0xbb67ae85;
  H[0][2] = 0x3c6ef372;
  H[0][3] = 0xa54ff53a;
  H[0][4] = 0x510e527f;
  H[0][5] = 0x9b05688c;
  H[0][6] = 0x1f83d9ab;
  H[0][7] = 0x5be0cd19;

 for(i=1;i<=n;i++){

	for(t=0;t<=63;t++){
	  if((0<=t) && (t<=15)){
		W[t] = M[i-1][t];
	  }
	  else{
		W[t] = small_sigma1(W[t-2]) + W[t-7] + small_sigma0(W[t-15]) + W[t-16];
	  }
	}
	
	a = H[i-1][0];
	b = H[i-1][1];
	c = H[i-1][2];
	d = H[i-1][3];
	e = H[i-1][4];
	f = H[i-1][5];
	g = H[i-1][6];
	h = H[i-1][7];

	for(t=0;t<=63;t++){
	  t1 = h + large_sigma1(e) + ch(e,f,g) + K[t] + W[t];
	  t2 = large_sigma0(a) + maj(a,b,c);
	  h = g;
	  g = f;
	  f = e;
	  e = d + t1;
	  d = c;
	  c = b;
	  b = a;
	  a = t1 + t2;
	}
	
	H[i][0] = a + H[i-1][0];
	H[i][1] = b + H[i-1][1];
	H[i][2] = c + H[i-1][2];
	H[i][3] = d + H[i-1][3];
	H[i][4] = e + H[i-1][4];
	H[i][5] = f + H[i-1][5];
	H[i][6] = g + H[i-1][6];
	H[i][7] = h + H[i-1][7];
 }

  printf("M:: %x | %x | %x | %x | %x | %x | %x | %x \n",
		 H[n][0],H[n][1],H[n][2],H[n][3],H[n][4],H[n][5],H[n][6],H[n][7]);
  return 0;
}
