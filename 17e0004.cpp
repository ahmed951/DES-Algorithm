#include <iostream>
#include <vector>
#include <string.h>
#include <assert.h>
#include <bitset>
#ifdef __GNUC__
# define __rdtsc __builtin_ia32_rdtsc
#else
# include<intrin.h>
#endif

using namespace std;
typedef unsigned long long u64;


u64 readHex( string data){
    //assert(strlen(data)==16);
    u64 value=0;
    for(int i=0;i<16;++i)
    {
        char c=data[i];
        if(c>='0'&&c<='9')
            value|=(u64)(c-'0')<<((15-i)<<2);
        else if(c>='A'&&c<='F')
            value|=(u64)(c-'A'+10)<<((15-i)<<2);
        else if(c>='a'&&c<='f')
            value|=(u64)(c-'a'+10)<<((15-i)<<2);
    }
    return value;
}

u64 perm(u64 in,int * permutation,int inLength,int outLength){

    u64 out=0;
    for(int i=0;i<outLength;++i)
        out|=(in>>(inLength-permutation[outLength-1-i])&1)<<i;
    return out;
}


u64 rotl(u64 value, int shift) {
    if ((shift &= 31) == 0)
      return value;
    int x = 0;
    int y = 0;
    if (shift == 1) {
        x=0x1;
        y = 27;
    }else{
        x= 0x3;
        y = 26;

    }
    return (value << shift) & ~(x<<28) | (value >> y);
}

u64 rotr(u64 value, int shift) {
    if ((shift &= 31) == 0)
      return value;
    return (value >> shift) | (value << (32 - shift));
}

vector<u64>  shiftLeft(u64 s ){
    //cout<<s<<endl;
    //cout << bitset<56>{s} <<endl;
    u64 left  = s >>28;
    //cout<<"left "<<left<<endl;
    //cout<<bitset<56>{left} <<endl;
    //cout << bitset<56>{s} <<endl;
    //cout<<"===="<<endl;
    u64 right  = s & 0xfffffff;
    //cout<<"right "<<right<<endl;
    //cout<<bitset<56>{right} <<endl;
    //cout<<left<<endl;
    //cout<<right<<endl<<endl;

    vector<u64> keys;
    int shiftTable[16] = {
        1, 1, 2, 2,
        2, 2, 2, 2,
        1, 2, 2, 2,
        2, 2, 2, 1 };
    u64 l = left,r =right;
    //return l=left.substr(1,27) + left[0];
    for(int i = 0; i < 16; i++){

        l = rotl(l,shiftTable[i]);
        r = rotl(r,shiftTable[i]);
        //r = r >> shiftTable[i];

        keys.push_back(l<<28|r);

    }

    return keys;

}


u64 sBoxOutputFunc(u64 input,int sBox[]){
	u64 result =0 ,index=0 ;

    for (int i =0; i<8; i++) {

        index = input >> (7-i)*6 & 0x3F;
        index = (index>>1&15)|(index&1)<<4|(index&0x20);
        result |= sBox[index+ i*64] << (7-i)*4;

    }
    return result;
}

u64 encrypt(string data1,int * ip,int * expand,u64 * keys,int * p, int * ipInverse){

    int sBox[512]={  14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 , 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 , 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 , 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 , 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 , 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 , 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 , 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11  };
    u64 data = readHex(data1);
    u64 dataAfterIp = perm(data,ip,64,64);
    u64 left  = dataAfterIp >> 32;

    u64 right  = dataAfterIp & 0xffffffff;

    for(int i = 0; i < 16; i++){

        //cout<<right<<endl;
        u64 rightExpand = perm(right,expand,32,48);
       // cout<<bitset<48>{rightExpand}<<endl;
        u64 xOR = rightExpand^keys[i];
        //cout<< xor1<<endl;
        u64 sBoxOutput = sBoxOutputFunc(xOR,sBox);
        u64 per = perm(sBoxOutput,p,32,32);
        //cout<<bitset<32>{sBoxOutput}<<endl;
        xOR = per^left;
        left = xOR;

        if (i != 15) {
            swap(left, right);
        }
        //cout<<left<<"  " <<right<<endl;
//        cout << "Round " << i + 1 << " " << binToHex(left) << " "
//             << binToHex(right)<<endl;
    }
    u64 finalValue = left << 32 | right;
    finalValue = perm(finalValue,ipInverse,64,64);
    //cout<<finalValue<<endl;
    //cout<<binToHex(finalValue);
    return finalValue;


}

u64 decrypt(string cipher,int * ip,int * expand,u64 * keys,int * p, int * ipInverse){

    u64 reverseKeys[16];
    int counter = 0;
    for(int i=15; i>=0; i--){
        reverseKeys[counter] = keys[i];
        //cout << reverseKeys[counter] << endl;
        counter ++;
    }
    u64 plain = encrypt(cipher,ip,expand,reverseKeys,p,ipInverse);

    return plain;
}

int main(int argc ,  char ** argv){

    if(argc != 4){
    	cout<<"Usage : ./17e0004 {operation} {data} {key}"<<endl;
    	return 0;
    }

    string data = argv[2];
    string key = argv[3];
    u64 k = readHex(key);
    //cout<<uppercase << hex<<k<<dec <<endl;
   // cout<<uppercase << bitset<64>{k} <<endl;
    //cout<<k<<endl;


    int pc1[56]={
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4

    };

    int pc2[48] = {
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32
    };

    int ip[64] = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
    };
    int expand[48] = {

        32, 1, 2, 3, 4, 5, 4, 5,
        6, 7, 8, 9, 8, 9, 10, 11,
        12, 13, 12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21, 20, 21,
        22, 23, 24, 25, 24, 25, 26, 27,
        28, 29, 28, 29, 30, 31, 32, 1
    };

    int p[32] = {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25
    };

    int ipInverse[64] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25
    };

    //cout<<perm(k,pc1,64,56)<<endl;

    /*Start getting key*/
   // cout<<key<<endl;
    u64 keyAfterPC1 = perm(k,pc1,64,56);
    //cout<<keyAfterPC1<<endl;
//    string keyAfterPC1Hex = binToHex(keyAfterPC1);
    //cout<<keyAfterPC1Hex<<endl;
    vector<u64> keys = shiftLeft(keyAfterPC1);
    u64 finalKeys[16];
    int counter = 0;
    for (auto i = keys.begin(); i != keys.end(); ++i){
        //finalKeys[counter] = permutation(*i,pc2,48);
        finalKeys[counter] = perm(*i,pc2,56,48);
        //cout << bitset<48>{finalKeys[counter] }<< endl;
        counter++;
    }
    /*finish getting key*/


    /*Start data*/

     if(argv[1] == "encrypt"s){
        long long t1 =__rdtsc();
    	u64 cipher = encrypt(data,ip,expand,finalKeys,p,ipInverse);
    	long long t2 =__rdtsc();

    	cout<<"cipher: "<<uppercase << hex<<cipher<<dec<<endl;
    	cout<<"cycles: " <<t2-t1<<endl;
    } else if(argv[1] == "decrypt"s){
        long long t1 =__rdtsc();
    	u64 plain = decrypt(data,ip,expand,finalKeys,p,ipInverse);
    	long long t2 =__rdtsc();
    	cout<<"plain: "<<uppercase << hex<<plain<<dec<<endl;
    	cout<<"cycles: " <<t2-t1<<endl;
    } else{
    	cout<<"Wrong operation"<<endl;
    }


    return 0;
}
