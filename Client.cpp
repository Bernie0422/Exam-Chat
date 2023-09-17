#include <winsock2.h>	
#include <stdio.h>
#include <windows.h>


#pragma comment(lib,"WS2_32.lib")
#define BUF_SIZE 256
#define NAME_SIZE 30

DWORD WINAPI send_msg(LPVOID lpParam);
DWORD WINAPI recv_msg(LPVOID lpParam);
void error_handling(const char* msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];
char answer[BUF_SIZE];
char password[BUF_SIZE];
bool is_authed = false;
bool login = true;

int main()
{
	HANDLE hThread[2];
	DWORD dwThreadId;

	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);




	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		error_handling("Failed socket()");
	

	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8888);
	//使用本機ip
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	send(sock, msg, strlen(msg), 0);
	if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		error_handling("Failed connect()");

	while (is_authed == false){

		printf("Are you new to the chat syetem?(y/n)");
		scanf("%s", answer);
		getchar();
		send(sock, answer, strlen(answer), 0);
		printf("Please type your chat name:");
		scanf("%s", name);
		getchar();
		send(sock, name, strlen(name), 0);
		printf("Please type your password:");
		scanf("%s", password);
		getchar();
		send(sock, password, strlen(password), 0);
		is_authed = true;
		}

	// printf("connect success\n");
	hThread[0] = CreateThread(
		NULL,
		NULL,
		send_msg,
		&sock,
		0,
		&dwThreadId);
	hThread[1] = CreateThread(
		NULL,
		NULL,
		recv_msg,
		&sock,
		0,
		&dwThreadId);


	WaitForMultipleObjects(2, hThread, true, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	printf(" Thread Over,Enter anything to over.\n");
	getchar();
	// 關閉連線
	closesocket(sock);

	WSACleanup();
	return 0;
}

DWORD WINAPI send_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		sprintf(name_msg, "[%s]: %s", name, msg);
		int nRecv = send(sock, name_msg, strlen(name_msg), 0);
	}
	return NULL;
}

DWORD WINAPI recv_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;
	while (1)
	{
		str_len = recv(sock, name_msg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (str_len == -1)
			return -1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

void error_handling(const char* msg)
{
	printf("%s\n", msg);
	WSACleanup();
	exit(1);
}
