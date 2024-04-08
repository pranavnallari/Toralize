#include "toralize.h"

Req *request(struct sockaddr_in *sock2){
    Req *req;
    req = malloc(sizeof(Req));
    if (req == NULL) {
        perror("Failed to allocate memory for request\n");
        return NULL;
    }

    req->vn = 4;
    req->cd = 1; // connect
    req->dstip = sock2->sin_addr.s_addr;
    req->dstport = sock2->sin_port;
    strncpy(req->userId, USERNAME, 8);
    return req;
}

 int connect(int s2, const struct sockaddr *sock2, socklen_t addrlen)
 {
    int s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int success;
    char tmp[512];

    int (*p) (int ,const struct sockaddr*, socklen_t);
   
    p = dlsym(RTLD_NEXT, "connect");


    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Failed to initialize socket\n");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (p(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
        perror("Connection to proxy server failed\n");
        close(s);
        return -1;
    }

    printf("Connected to Proxy Successfully\n");
    req = request((struct sockaddr_in*)sock2);
    if (req == NULL) {
        close(s);
        return -1;
    }

    if (write(s, req, sizeof(Req)) < 0) {
        perror("Error writing request to proxy\n");
        free(req);
        close(s);
        return -1;
    }

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1){
        perror("Error occurred while reading response\n");
        close(s);
        return -1;
    }

    res = (Res *)buf;
    success = (res->cd == 90);
    if(!success){
        fprintf(stderr, "Unable to traverse the proxy, error code: %d\n", res->cd);
        close(s);
        return -1;
    }

    printf("Successfully connected through the proxy \n");

    dup2(s, s2);
    free(req);

    return 0;
}
