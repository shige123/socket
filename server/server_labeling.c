//サーバー

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "Labeling.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#define BUFFER_SIZE 256

char* getFileName(char *path);
void data_receive(int socket, );
void tranceport(int socket, char *path);

// 適当な色を出す用
cv::RNG rnd(1192);
cv::Scalar randomColor()
{
  return cv::Scalar(rnd.next() & 0xFF, rnd.next() & 0xFF, rnd.next() & 0xFF);
}

cv::Mat labelingh(cv::Mat img)
{
  struct timeval s,e;

  gettimeofday(&s,NULL);
	
  // グレイスケール化(3チャンネル→1チャンネル化)
  cv::Mat gryimg;
  cv::cvtColor(img, gryimg, CV_BGR2GRAY);
  // 二値化
  cv::Mat binimg;
  cv::threshold(gryimg, binimg, 0, 255, CV_THRESH_BINARY);

  // Lblingの結果を受け取る
  cv::Mat lbl(img.size(), CV_16SC1);

  // ラベリングを実施 ２値化した画像に対して実行する
  LabelingBS	lbling;
  lbling.Exec(binimg.data, (short *)lbl.data, img.cols, img.rows, false, 0);	
	
  // ラベリング結果を出力する、真っ白な状態で初期化
  cv::Mat outimg(img.size(), CV_8UC3, cv::Scalar(0));
	
  // ラベルされた領域をひとつずつ描画
  for( int i = 0; i < lbling.GetNumOfRegions(); i++)
    {
      // ラベリング結果の領域を抽出する
      cv::Mat lblarea;
      cv::compare(lbl, i + 1, lblarea, CV_CMP_EQ);
      // 抽出した領域にランダムな色を設定して出力画像に追加
      cv::Mat color(img.size(), CV_8UC3, randomColor());
      color.copyTo(outimg, lblarea);
    }

  // ラベルされた出力画像を保存
  
  gettimeofday(&e,NULL);

  //時間を計測
  std::cout << "time =" << (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec ) / 100000.0 << std::endl;

  return outimg;
}

int main()
{
  /* ポート番号、ソケット */
  unsigned short port = 50000;
  int srcSocket;  // 自分
  int dstSocket;  // 相手
  int size;
  /* sockaddr_in 構造体 */
  struct sockaddr_in srcAddr;
  struct sockaddr_in dstAddr;
  socklen_t dstAddrSize = sizeof(dstAddr);

  /* 各種パラメータ */
  char buf[BUFFER_SIZE];
  std::vector img_buf;

  /************************************************************/
  /* sockaddr_in 構造体のセット */
  memset(&srcAddr, 0, sizeof(srcAddr));
  srcAddr.sin_port = htons(port);
  srcAddr.sin_family = AF_INET;
  srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* ソケットの生成 */
  srcSocket = socket(AF_INET, SOCK_STREAM, 0);

  /* ソケットのバインド */
  bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));

  /* 接続の許可 */
  listen(srcSocket, 1);

  /* 接続の受付け */
  printf("Waiting for connection ...\n");
  dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
  printf("Connected from %s\n", inet_ntoa(dstAddr.sin_addr));

  if(size = recv(dstSocket, buf, 15, 0) < 0)
    {
      fprintf(stderr, "error\n");
    }
  data_receive(dstSocket, img_buf);
  
  
  
  labelingh();
  



  printf("debug\n");
  char fname[20] = "result_img_0.png";
  
  printf("debug1\n");
  if(size = send(dstSocket, fname, 20, 0) < 0)
    {
      fprintf(stderr, "error\n");
    }
  tranceport(dstSocket, fname);
  printf("finish\n");
  
  close(srcSocket);
  return 0;
}
  
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
  
void data_receive(int socket, std::vector buf)
{
  //FILE *fp;
  int size;
  //char buf[4096];

  // ファイルオープン
  //fp = fopen(name, "wb");
  if (fp == NULL)
    {
      fprintf(stderr, "%s could not open.", name);
      exit(1);
    }

  // ファイル受信
  for (;;)
    {
      size = recv(socket, buf, 4096, 0);
      printf("size = %d\n" ,size);
      if (size == 0 || size < 4096)
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
