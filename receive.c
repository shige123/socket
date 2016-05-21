/**************************************************************************************
 * ファイル受信クライアント
 *
 * 説明：
 * 　サーバに保存されているファイルを指定し受信する。
 * 　TCP/IPの機能のみを使用している。
 *
 * 作成日：2011/10/23
 **************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE_256 256

char *getFileName(char *path);
void jusin(int socket, char *path);

int main(int argc, char **argv)
{
  int s, ds;
  struct sockaddr_in server;
  struct in_addr dst_ip;
  unsigned short port = 50000;
  unsigned short port2 = 50001;

  dst_ip.s_addr = inet_addr(argv[1]);

  printf("アドレスをバイト列に変換\n");
  printf("addres to byte ... %d\n", dst_ip.s_addr);
  printf("バイト列をＩＰｖ４形式の文字列に変換\n");
  printf("byte to address ... %s\n", inet_ntoa(dst_ip));

  // メッセージ送受信用ソケット生成
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "ソケットの作成に失敗しました。\n");
      exit(1);
    }

  // サーバ接続
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = dst_ip.s_addr; 
  server.sin_port = port;

  if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
      fprintf(stderr, "サーバへの接続に失敗しました。\n");
      exit(1);
    }

  printf("connect ... %s\n", inet_ntoa(server.sin_addr));

  for (;;)
    {
      char buf[BUF_SIZE_256];

      printf("> ");
      fgets(buf, BUF_SIZE_256, stdin);

      send(s, buf, strlen(buf), 0);

      buf[strlen(buf)-1] = '\0';

      if (strcmp("exit", buf) == 0)
	{
	  printf("サーバとの接続を切断します。\n");
	  close(s);
	  break;
	}

      // ファイル受信用のソケットを作成
      if ((ds = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	  fprintf(stderr, "ソケットの作成に失敗しました。\n");
	  exit(1);
	}
      
      // サーバへファイル受信用(ポート番号5001)で接続
      server.sin_port = port2;
      if (connect(ds, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
	  fprintf(stderr, "データ通信用のソケット生成に失敗しました。\n");
	  exit(1);
	}
      
      // データ受信
      jusin(ds, getFileName(buf));

      printf("ファイル受信が完了しました。\n");

      close(ds);
    }

  return 0;
}

/**************************************************************************************
 * ファイルパスからファイル名を抽出
 *
 * 引数：path ファイルパス
 **************************************************************************************/
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

/**************************************************************************************
 * ファイル受信処理
 *
 * 引数：socket 受信用ソケット
 *       name 受信ファイル名
 **************************************************************************************/
void jusin(int socket, char *name)
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
