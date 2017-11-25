#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Buffer SIZE
#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock; // 소켓 정의
	char message[BUF_SIZE]; // client 메세지
	int str_len, recv_len, recv_cnt; // string length
	struct sockaddr_in serv_adr; // server 소켓 주소 구조체

	if(argc!=3) { // argument 갯수가 맞지 않으면
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
    //PF_INET: IPv4 인터넷 프로토콜, PF_INET, SOCKET STEAM: 연결 지향성 소켓, 0: 서비스 공급자에게 맡김
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
    // 0으로 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; // Address Family
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]); // 첫 argument는 서버의 주소
	serv_adr.sin_port=htons(atoi(argv[2])); // 두번째 argument는 서버의 포트
	
    // 서버에 connect
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
        // 메세지 출력
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
        // q가 아니라면 계속 진행
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;

		str_len=write(sock, message, strlen(message));
		
		recv_len=0;
        // 서버로 오는 메세지를 읽음, 읽은 값과 길이가 작으면 반복
		while(recv_len<str_len)
		{
            // 읽은 값, 길이를 구함
			recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
			if(recv_cnt==-1)
				error_handling("read() error!");
			recv_len+=recv_cnt; // 길이를 더함
		}
		
        // 마지막길이는 0을 넣음
		message[recv_len]=0;
        // 메세지 출력
		printf("Message from server: %s", message);
	}
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
