//�T�[�o�[

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

// �K���ȐF���o���p
cv::RNG rnd(1192);
cv::Scalar randomColor()
{
  return cv::Scalar(rnd.next() & 0xFF, rnd.next() & 0xFF, rnd.next() & 0xFF);
}

cv::Mat labelingh(cv::Mat img)
{
  struct timeval s,e;

  gettimeofday(&s,NULL);
	
  // �O���C�X�P�[����(3�`�����l����1�`�����l����)
  cv::Mat gryimg;
  cv::cvtColor(img, gryimg, CV_BGR2GRAY);
  // ��l��
  cv::Mat binimg;
  cv::threshold(gryimg, binimg, 0, 255, CV_THRESH_BINARY);

  // Lbling�̌��ʂ��󂯎��
  cv::Mat lbl(img.size(), CV_16SC1);

  // ���x�����O�����{ �Q�l�������摜�ɑ΂��Ď��s����
  LabelingBS	lbling;
  lbling.Exec(binimg.data, (short *)lbl.data, img.cols, img.rows, false, 0);	
	
  // ���x�����O���ʂ��o�͂���A�^�����ȏ�Ԃŏ�����
  cv::Mat outimg(img.size(), CV_8UC3, cv::Scalar(0));
	
  // ���x�����ꂽ�̈���ЂƂ��`��
  for( int i = 0; i < lbling.GetNumOfRegions(); i++)
    {
      // ���x�����O���ʂ̗̈�𒊏o����
      cv::Mat lblarea;
      cv::compare(lbl, i + 1, lblarea, CV_CMP_EQ);
      // ���o�����̈�Ƀ����_���ȐF��ݒ肵�ďo�͉摜�ɒǉ�
      cv::Mat color(img.size(), CV_8UC3, randomColor());
      color.copyTo(outimg, lblarea);
    }

  // ���x�����ꂽ�o�͉摜��ۑ�
  
  gettimeofday(&e,NULL);

  //���Ԃ��v��
  std::cout << "time =" << (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec ) / 100000.0 << std::endl;

  return outimg;
}

int main()
{
  /* �|�[�g�ԍ��A�\�P�b�g */
  unsigned short port = 50000;
  int srcSocket;  // ����
  int dstSocket;  // ����
  int size;
  /* sockaddr_in �\���� */
  struct sockaddr_in srcAddr;
  struct sockaddr_in dstAddr;
  socklen_t dstAddrSize = sizeof(dstAddr);

  /* �e��p�����[�^ */
  char buf[BUFFER_SIZE];
  std::vector img_buf;

  /************************************************************/
  /* sockaddr_in �\���̂̃Z�b�g */
  memset(&srcAddr, 0, sizeof(srcAddr));
  srcAddr.sin_port = htons(port);
  srcAddr.sin_family = AF_INET;
  srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* �\�P�b�g�̐��� */
  srcSocket = socket(AF_INET, SOCK_STREAM, 0);

  /* �\�P�b�g�̃o�C���h */
  bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));

  /* �ڑ��̋��� */
  listen(srcSocket, 1);

  /* �ڑ��̎�t�� */
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

  // �t�@�C���I�[�v��
  //fp = fopen(name, "wb");
  if (fp == NULL)
    {
      fprintf(stderr, "%s could not open.", name);
      exit(1);
    }

  // �t�@�C����M
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

  // �o�b�t�@���e���t���b�V��
  fflush(fp);

  // �t�@�C���N���[�Y
  fclose(fp);
}

void tranceport(int socket, char *path)
{
  FILE *fp;
  int size;
  char buf[4096];

  // �t�@�C���I�[�v��
  fp = fopen(path, "rb");
  if (fp == NULL)
    {
      fprintf(stderr, "%s not found.",path);
      exit(1);
    }

  // �t�@�C�����M
  for (;;)
    {
      size = fread(buf, 1, 4096, fp);

      if (size == 0)
	{
	  break;
	}

      send(socket, buf, size, 0);
    }

  // �t�@�C���N���[�Y
  fclose(fp);
}