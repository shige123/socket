//�T�[�o�[

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 256

char* getFileName(char *path);
void data_receive(int socket, char *name);
//void tranceport(int socket, char *path);


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
  int dstAddrSize = sizeof(dstAddr);

  /* �e��p�����[�^ */
  char buf[BUFFER_SIZE];

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
  data_receive(dstSocket, getFileName(buf));
  /*
  char fname[15] = "result_png_0.png";
  if(size = send(dstSocket, fname, 15, 0) < 0)
  {
  	fprintf(stderr, "error\n");
  }
  tranceport(dstSocket, fname);
  printf("finish\n");
  */
  close(dstAddr);
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
  
void data_receive(int socket, char *name)
{
  FILE *fp;
  int size;
  char buf[4096];

  // �t�@�C���I�[�v��
  fp = fopen(name, "wb");
  if (fp == NULL)
    {
      fprintf(stderr, "%s could not open.", name);
      exit(1);
    }

  // �t�@�C����M
  for (;;)
  {
   size = recv(socket, buf, 4096, 0);
    if (size == 0)
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
/*
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
*/
