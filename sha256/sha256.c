#include <stdio.h>

#define N 1//64byteピッタリを1個

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

int main(void)
{
  int i,j,t;
  unsigned int a,b,c,d,e,f,g,h,t1,t2;
  unsigned int W[64];
  unsigned int H[8][65];

  unsigned int M[N][16];
  //初期化
  for(i=0;i<N;i++){
	  for(j=0;j<16;j++){
		M[i][j]= 0;
	  }
  }
  M[0][0]=0x80000000;

  
  H[0][0] = 0x6a09e667;
  H[1][0] = 0xbb67ae85;
  H[2][0] = 0x3c6ef372;
  H[3][0] = 0xa54ff53a;
  H[4][0] = 0x510e527f;
  H[5][0] = 0x9b05688c;
  H[6][0] = 0x1f83d9ab;
  H[7][0] = 0x5be0cd19;

 for(i=1;i<=N;i++){

	for(t=0;t<=63;t++){
	  if((0<=t) && (t<=15)){
		W[t] = M[i-1][t];
	  }
	  else{
		W[t] = small_sigma1(W[t-2]) + W[t-7] + small_sigma0(W[t-15]) + W[t-16];
	  }
	}
	
	a = H[0][i-1];
	b = H[1][i-1];
	c = H[2][i-1];
	d = H[3][i-1];
	e = H[4][i-1];
	f = H[5][i-1];
	g = H[6][i-1];
	h = H[7][i-1];

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
	
	H[0][i] = a + H[0][i-1];
	H[1][i] = b + H[1][i-1];
	H[2][i] = c + H[2][i-1];
	H[3][i] = d + H[3][i-1];
	H[4][i] = e + H[4][i-1];
	H[5][i] = f + H[5][i-1];
	H[6][i] = g + H[6][i-1];
	H[7][i] = h + H[7][i-1];
						
 }

  printf("M:: %x | %x | %x | %x | %x | %x | %x | %x \n",
		 H[0][N],H[1][N],H[2][N],H[3][N],H[4][N],H[5][N],H[6][N],H[7][N]);

  return 0;
}
