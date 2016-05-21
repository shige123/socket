//クライアント

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 256

//char* getFileName(char *path);
//void data_receive(int socket, char *name);
void tranceport(int socket, char *path);

int main()
{
  /* IP アドレス、ポート番号、ソケット */
 // char destination[80];
  unsigned short port = 50000;
  int dstSocket;
  int size;

  /* sockaddr_in 構造体 */
  struct sockaddr_in dstAddr;

  /* 各種パラメータ */
  char buf[BUFFER_SIZE];

  /************************************************************/
  /* 相手先アドレスの入力 */
  char destination[] = "127.0.0.1";

  /* sockaddr_in 構造体のセット */
  memset(&dstAddr, 0, sizeof(dstAddr));
  dstAddr.sin_port = htons(port);
  dstAddr.sin_family = AF_INET;
  dstAddr.sin_addr.s_addr = inet_addr(destination);
 
  /* ソケット生成 */
  dstSocket = socket(AF_INET, SOCK_STREAM, 0);

  /* 接続 */
  printf("Trying to connect to %s: \n", destination);
  connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr));
  
  
  char fname[15] = "adad7a7a-s.jpg";
    if(size = send(dstSocket, fname, 15, 0) < 0){
  	fprintf(stderr, "error\n");
  }
    tranceport(dstSocket, fname);
  printf("finish\n");

  /*
  if(size = recv(dstSocket, buf, 15, 0) < 0)
  {
    fprintf(stderr, "error\n");
  }
  data_receive(dstSocket, getFileName(buf));
  */
  close(dstAddr);
  return 0;
  
  
  
}
/*  
  
  
  
char *getFileName(char *path)
  {
  char *ptoken;
  char *pret;

  ptoken = strtok(path, "/");
  while ((pret = strtok(NULL, "/")) != NULL)
    {
      ptoken = pret;
    }

  return ptoken;
}
  
void data_receive(int socket, char *name)
{
  FILE *fp;
  int size;
  char buf[4096];

  // ファイルオープン
  fp = fopen(name, "wb");
  if (fp == NULL)
    {
      fprintf(stderr, "%s could not open.", name);
      exit(1);
    }

  // ファイル受信
  for (;;)
  {
   size = recv(socket, buf, 4096, 0);
    if (size == 0)
    {
      break;
    }

    fwrite(buf, 1, size, fp);
  }

  // バッファ内容をフラッシュ
  fflush(fp);

  // ファイルクローズ
  fclose(fp);
}
*/
void tranceport(int socket, char *path)
{
	FILE *fp;
	int size;
	char buf[4096];

	// ファイルオープン
	fp = fopen(path, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "%s not found.",path);
		exit(1);
	}

	// ファイル送信
	for (;;)
	{
	size = fread(buf, 1, 4096, fp);

	if (size == 0)
	{
		break;
	}

	send(socket, buf, size, 0);
	}

	// ファイルクローズ
	fclose(fp);
}
