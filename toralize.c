#include "toralize.h"

Req *request(const char* dstip, const int dstport){
    Req *req;
    req = malloc(sizeof(Req));
    if (req == NULL) {
        perror("Failed to allocate memory for request\n");
        return NULL;
    }

    req->vn = 4;
    req->cd = 1; // connect
    req->dstip = inet_addr(dstip);
    req->dstport = htons(dstport);
    strncpy(req->userId, USERNAME, 8);
    return req;
}

int main(int argc, char *argv[]){
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int success;
    char tmp[512];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return -1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Failed to initialize socket\n");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (connect(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
        perror("Connection to proxy server failed\n");
        close(s);
        return -1;
    }

    printf("Connected to Proxy Successfully\n");
    req = request(host, port);
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

    free(req);

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

    printf("Successfully connected through proxy to %s:%d\n", host, port);

    memset(tmp, 0 ,512);
    snprintf(tmp, 511, "HEAD / HTTP/1.0\r\n"
                        "Host:www.google.com\r\n"
                        "\r\n"
            );

    write(s, tmp, strlen(tmp));
    memset(tmp, 0, 512);
    read(s, tmp, 511);
    printf("'%s'\n",tmp);

    
    close(s);
    free(req);
    return 0;
}
