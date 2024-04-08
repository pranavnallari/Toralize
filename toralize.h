/* Using Socks v4 for this project */
// https://www.openssh.com/txt/socks4.protocol
// refer above link for more information about Socks4 Protocol

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<dlfcn.h>

#define PROXY "127.0.0.1"
#define PROXY_PORT 9050
#define reqsize sizeof(struct proxy_request)
#define ressize sizeof(struct proxy_response)
#define USERNAME "toraliz"


typedef unsigned char int8;
typedef unsigned short int16;
typedef unsigned int int32;





/*         
                    SOCKS REQUEST FORMAT
		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----
            1    1      2              4           variable       1
                    No of Bytes 



                SOCKS RESPONSE FORMAT
        +----+----+----+----+----+----+----+----+
		| VN | CD | DSTPORT |      DSTIP        |
		+----+----+----+----+----+----+----+----+
          1    1      2              4
            No of Bytes


        VN is the version of the reply code and should be 0. CD is the result
        code with one of the following values:

	    90: request granted
	    91: request rejected or failed
	    92: request rejected becasue SOCKS server cannot connect to
	    identd on the client
	    93: request rejected because the client program and identd
	    report different user-ids.
*/



struct proxy_request{
    int8 vn;
    int8 cd;
    int16 dstport;
    int32 dstip;
    unsigned char userId[8];
};


struct proxy_response{
    int8 vn;
    int8 cd;
    int16 _; // dst port not important
    int32 __; // dst ip not important
};

typedef struct proxy_request Req;
typedef struct proxy_response Res;

Req *request(struct sockaddr_in*);

int connect(int, const struct sockaddr*, socklen_t);