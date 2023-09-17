#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#define MAX_CLNT 256
#define BUF_SIZE 100


#pragma comment(lib,"WS2_32.lib")

void error_handling(const char* msg);
DWORD WINAPI ThreadProc(LPVOID lpParam);
void send_msg(char* msg, int len);
HANDLE g_hEvent;
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
HANDLE hThread[MAX_CLNT];
char msg[BUF_SIZE];

bool is_authed = false;
bool is_first_time = true;
bool is_name_null = true;
bool is_pwd_null = true;

int main()
{

	// 初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);	//使用2.2個socket版本

	g_hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);


	SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == INVALID_SOCKET)
		error_handling("Failed socket()");
	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);	
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	// 绑定
	if (bind(serv_sock, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		error_handling("Failed bind()");

	// 開始監聽
	if (listen(serv_sock, 256) == SOCKET_ERROR)
		error_handling("Failed listen()");
	printf("Start listening:\n");
	// 接受新要求
	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	DWORD dwThreadId;
	SOCKET clnt_sock;

	while (TRUE)
	{
		printf("Waiting for client...\n");
		// 接受新連接
		clnt_sock = accept(serv_sock, (SOCKADDR*)&remoteAddr, &nAddrLen);
		if (clnt_sock == INVALID_SOCKET)
		{
			printf("Failed accept()");
			continue;
		}
		msg[BUF_SIZE] = NULL; //清空buffer
		WaitForSingleObject(g_hEvent, INFINITE);
		hThread[clnt_cnt] = CreateThread(
			NULL,
			NULL,
			ThreadProc,
			(void*)&clnt_sock,
			0,
			&dwThreadId);
		clnt_socks[clnt_cnt++] = clnt_sock;
		SetEvent(g_hEvent);
		printf("Received a new client:%s ID:%d\r\n", inet_ntoa(remoteAddr.sin_addr), dwThreadId);
		is_authed = false;
		is_first_time = true;
		is_name_null = true;
		is_pwd_null = true;
	}
	WaitForMultipleObjects(clnt_cnt, hThread, true, INFINITE);
	for (int i = 0; i < clnt_cnt; i++)
	{
		CloseHandle(hThread[i]);
	}
	// 停止連線
	closesocket(serv_sock);
	WSACleanup();
	return 0;
}


void error_handling(const char* msg)
{
	printf("%s\n", msg);
	WSACleanup();
	exit(1);
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	int clnt_sock = *((int*)lpParam);
	int str_len = 0, i;
	char name[30];
	char *name_new;
	char pwd[30];
	char tmp[BUF_SIZE];
	while ((str_len = recv(clnt_sock, msg, sizeof(msg), 0)) != -1)
	{
		
		while(is_authed == false){
			while(is_pwd_null){
				while(is_name_null){
					while(is_first_time){
						if(msg[0] == 'y'){
							msg[BUF_SIZE] = NULL; //清空buffer
							msg[BUF_SIZE] = recv(clnt_sock, msg, sizeof(msg), 0);
							strcpy(name, msg);
							printf("name=%s\n", name);
							if(name != NULL){
								FILE *fptr;
								fptr = fopen("Users.txt","a");
								char *content;
								if(fptr){
									fprintf(fptr,"\n%s", name);
									fclose(fptr);
								}
								is_first_time = false;
							}
							
						}
						if(msg[0] == 'Y'){
							msg[BUF_SIZE] = NULL; //清空buffer
							msg[BUF_SIZE] = recv(clnt_sock, msg, sizeof(msg), 0);
							strcpy(name, msg);
							printf("name=%s\n", name);
							if(name != NULL){
								FILE *fptr;
								fptr = fopen("Users.txt","a");
								char *content;
								if(fptr){
									fprintf(fptr,"\n%s", name);
									fclose(fptr);
								}
								is_first_time = false;
							}
						}
						if(msg[0] == 'n'){
							// send_msg(msg, str_len);
							msg[BUF_SIZE] = NULL;
							msg[BUF_SIZE] = recv(clnt_sock, msg, sizeof(msg), 0);
							printf("name=%s\n", msg);
							if(name != NULL){
								FILE *fptr;
								fptr = fopen("Users.txt","r");
								char *content;
								if(fptr){
									int i, line;
									while(*content = fgetc(fptr) != EOF){
										if(*content == '\n'){
											line++;
										}
									}
									for(i=0;i<line;i++){
										fscanf(fptr,"%[^\n]", content);
										fscanf(fptr,"%s", &name);
									
									}
									

									printf("name:%s\n", name);
									fclose(fptr);
								}
								is_first_time = false;
							}
						}
						if(msg[0] == 'N'){
							// send_msg(msg, str_len);
							msg[BUF_SIZE] = NULL;
							msg[BUF_SIZE] = recv(clnt_sock, msg, sizeof(msg), 0);
							printf("name=%s\n", msg);
							is_first_time = false;
						}
					msg[BUF_SIZE] = NULL; //清空buffer
					is_name_null = false;
					}

					// send_msg(msg, str_len);
					// printf("msg=%s\n", msg);
					// is_authed = true;
				}
				
			pwd[30] = recv(clnt_sock, msg, sizeof(msg), 0);
			strcpy(pwd, msg);
			printf("Pwd:%s\n", pwd);
			is_pwd_null = false;
			}

			is_authed = true;
		}
		send_msg(msg, str_len);
		printf("%s\n", msg);
	}
	printf("Client exit:%d\n", GetCurrentThreadId());

	WaitForSingleObject(g_hEvent, INFINITE);
	for (i = 0; i < clnt_cnt; i++)
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i++ < clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	SetEvent(g_hEvent);
	// 關閉連線
	closesocket(clnt_sock);
	return NULL;
}

void send_msg(char* msg, int len)
{
	int i;

	WaitForSingleObject(g_hEvent, INFINITE);
	for (i = 0; i < clnt_cnt; i++)
		send(clnt_socks[i], msg, len, 0);
	SetEvent(g_hEvent);
}

