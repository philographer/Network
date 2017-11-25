#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 읽을 문자열, 버퍼 사이즈
#define BUF_SIZE 1024
void error_handling(char *message); // 에러 핸들 함수 정의

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; // 서버 소켓, 클라이언트 소켓 정의
	char message[BUF_SIZE]; // 메세지를 담을 공간 str
	int str_len, i; // str 길이, 반복문을 위한 변수
	
	struct sockaddr_in serv_adr; // 서버 주소정보 구조체
	struct sockaddr_in clnt_adr; // 클라이언트 주소정보 구조체
	socklen_t clnt_adr_sz; // 클라이언트 주소 길이, 32비트
	 
	if(argc!=2) { // 인자가 2개가 아니라면
		printf("Usage : %s <port>\n", argv[0]); // 사용방법 print
		exit(1);
	}
	
    // 서버 소켓, 프로토콜 패밀리 PE_INET: IPv4, SOCK_STREAM: 연결 지향형 소켓
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(serv_sock==-1) // -1은 에러
		error_handling("socket() error");
	
    // server_adr을 0으로 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; // 주소체계
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); // htonl: host to network long, 바이트 순서를 바꿈
	serv_adr.sin_port=htons(atoi(argv[1])); // atoi: str to int
    // bint: 소켓에 ip와 port번호 지정
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	
    // 클라이언트에서 접속할 수 있도록, 대기 메세지 큐 5개
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_adr_sz=sizeof(clnt_adr);

    // 5개의 클라이언트를 받을 수 있음
	for(i=0; i<5; i++)
	{
        // client 소켓을 받아들임
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        
        /***
         과제 부분
         ***/
        // client의 주소 정보에서 ip,port를 알아냄
        int raw_port = clnt_adr.sin_port;
        int clnt_port = ntohs(clnt_adr.sin_port);
        char *clnt_ip = inet_ntoa(clnt_adr.sin_addr); // 32비트 주소체계를 .(dot) format으로 변경
        // printf("raw_port %d\n", raw_port);
        // printf("clnt port %d\n", clnt_port);
        // printf("clnt addr %s\n", cln qt_ip);
        /***
         과제 부분
         ***/
        
        // client error handling
		if(clnt_sock==-1)
			error_handling("accept() error");
		else{
			printf("Connected client : %s %d\n", clnt_ip, clnt_port);
			printf("Connected client %d\n", i+1);
		}
        
        // 사용자가 보낸 메세지를 그대로 돌려줌
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len);

        // 소켓 종료
		close(clnt_sock);
	}

	close(serv_sock);
	return 0;
}

// 에러 메세지 핸들링 f함수, 에러 메세지 출력 후 종료
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
