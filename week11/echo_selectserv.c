#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
// 에러 핸들링 함수
void error_handling(char *buf);

int main(int argc, char *argv[]) {
    // 소켓, 주소 구조체등 선언
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;

    // file descriptor set을 보관 
    fd_set reads, cpy_reads;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 서버 주소체계 설정
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // bind, listen
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error");

    // FD_SET Zero로 초기화
    FD_ZERO(&reads);
    // 관찰 대상에 서버 소켓을 등록
    FD_SET(serv_sock, &reads);
    fd_max=serv_sock;

    while(1)
    {
        // reads를 복사해 둠
        cpy_reads = reads;
        timeout.tv_sec=5;
        timeout.tv_usec=5000;
        // 관찰에 필요한 인자만 전달
        if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
       
        if(fd_num==0)
            continue;

        // 변화된 값들을 관찰함
        for(i=0; i<fd_max+1; i++) {
            if(FD_ISSET(i, &cpy_reads)) // 서버 소켓에 변화가 있는지
            {
                if(i==serv_sock) { // connection request
                    adr_sz=sizeof(clnt_adr);
                    clnt_sock=
                        accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    char *client_ip = inet_ntoa(clnt_adr.sin_addr);
                    // inet_ntoa: 정수형태 IP => 문자열 형태 IP로 변환
                    // ntohs: 네트워크 주소 => 호스트 주소체계
                    printf("connected client: %s %d\n", client_ip, ntohs(clnt_adr.sin_port));
		    FD_SET(clnt_sock, &reads);
                    if(fd_max<clnt_sock)
                        fd_max=clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                }
                else { // read message!
                    str_len =read(i, buf, BUF_SIZE);
                    if(str_len==0) {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else {
                        write(i, buf, str_len);
                    }

                }
            }
        }
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *buf) {
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}


