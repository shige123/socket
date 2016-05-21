//�N���C�A���g

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
  /* IP �A�h���X�A�|�[�g�ԍ��A�\�P�b�g */
 // char destination[80];
  unsigned short port = 50000;
  int dstSocket;
  int size;

  /* sockaddr_in �\���� */
  struct sockaddr_in dstAddr;

  /* �e��p�����[�^ */
  char buf[BUFFER_SIZE];

  /************************************************************/
  /* �����A�h���X�̓��� */
  char destination[] = "127.0.0.1";

  /* sockaddr_in �\���̂̃Z�b�g */
  memset(&dstAddr, 0, sizeof(dstAddr));
  dstAddr.sin_port = htons(port);
  dstAddr.sin_family = AF_INET;
  dstAddr.sin_addr.s_addr = inet_addr(destination);
 
  /* �\�P�b�g���� */
  dstSocket = socket(AF_INET, SOCK_STREAM, 0);

  /* �ڑ� */
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
*/
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
