/**************************************************************************************
 * ファイル送信サーバ
 *
 * 説明：
 * 　クライアントから指定されたファイルをクライアントに送信する。
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
#define PORT1 50000
#define PORT2 50001

int pid;

void tranceport(int socket, char *path);

int main(int argc, char **argv)
{
  int s1;// 接続受付用ソケット(メッセージ送受信用ソケット生成担当)
  int s2;// 接続受付用ソケット(データ送受信用のソケット生成担当)
  int ms;// メッセージ送受信用ソケット
  int ds;// データ送受信用ソケット
  struct sockaddr_in server;// サーバ設定(メッセージ送受信用)
  struct sockaddr_in dserver;// サーバ設定(データ送受信用サーバ設定)
  struct sockaddr_in client;// クライアント設定
  int len;

  /*
   * 接続受付用ソケットの生成
   */
  // 接続受付用ソケット生成
  if ((s1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "ソケット作成に失敗しました。\n");
      exit(1);
    }

  // 接続受付用ソケット生成
  if ((s2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "ソケット作成に失敗しました。\n");
      exit(1);
    }

  /*
   * サーバ情報の設定
   */
  // メッセージ送受信用設定
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = PORT1;

  // データ送受信用設定
  memset(&dserver, 0, sizeof(dserver));
  dserver.sin_family = AF_INET;
  dserver.sin_addr.s_addr = htonl(INADDR_ANY);
  dserver.sin_port = PORT2;

  // サーバ情報の設定
  if (bind(s1, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
      fprintf(stderr, "バインドに失敗しました。\n");
      exit(1);
    }

  // サーバ情報の設定
  if (bind(s2, (struct sockaddr *) &dserver, sizeof(dserver)) < 0)
    {
      fprintf(stderr, "バインドに失敗しました。\n");
      exit(1);
    }

  /*
   * 接続の受付開始
   */
  if (listen(s1, 5) < 0)
    {
      fprintf(stderr, "コネクション受付開始\n");
      exit(1);
    }


  if (listen(s2, 5) < 0)
    {
      fprintf(stderr, "コネクション受付開始\n");
      exit(1);
    }

  /*
   * 通信処理
   */
  for (;;)
    {
      // メッセージ送受信用接続の受付
      len = sizeof(client);
      if ((ms = accept(s1, (struct sockaddr *) &client, &len)) < 0)
	{
	  fprintf(stderr, "クライアントとの接続に失敗しました。\n");
	  exit(1);
	}

      // プロセス生成
      // 以降の処理はプロセスごとに処理分岐させる。
      pid = fork();

      // プロセス生成失敗
      if (pid == -1)
	{
	  fprintf(stderr, "プロセス生成に失敗しました。\n");
	  exit(1);
	}
      // 親プロセス処理
      if (pid != 0)
	{
	  // 親は送受信しないため、通信用ソケットクローズ
	  close(ms);
	  continue;
	}
      // 子プロセス処理
      else if (pid == 0)
	{
	  printf("通信を開始します。\n");
	  printf("client address ... %s\n", inet_ntoa(client.sin_addr));

	  for (;;)
	    {
	      int recvByte;
	      char buf[BUF_SIZE_256];

	      // データ受信(改行含む)
	      // 受信データから改行は除去する。
	      recvByte = recv(ms, buf, BUF_SIZE_256, 0);
	      buf[recvByte - 1] = '\0';

	      if (strcmp("exit", buf) == 0)
		{
		  printf("クライアントとの接続を切断します。\n");
		  printf("client address ... %s\n", inet_ntoa(client.sin_addr));
		  close(ms);
		  goto end;
		}

	      printf("file name : ");
	      printf("%s\n", buf);

	      // データ送受信用接続の受付
	      len = sizeof(client);
	      if ((ds = accept(s2, (struct sockaddr *) &client, &len)) < 0)
		{
		  fprintf(stderr, "error\n");
		  exit(1);
		}
	      
	      // データ送信
	      tranceport(ds, buf);

	      printf("転送完了\n");
	      close(ds);
	    }
	}
    }

 end:
  close(s1);
  close(s2);

  return 0;
}

/**************************************************************************************
 * ファイル送信処理
 *
 * 引数：socket 送信用ソケット
 *       path 送信ファイルのパス
 **************************************************************************************/
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
