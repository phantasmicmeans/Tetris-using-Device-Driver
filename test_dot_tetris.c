#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>
#include "dot_matrix_font.h"
#define MAX_BUTTON 9
unsigned char quit = 0;
typedef enum { NO, LEFT, RIGHT, DOWN, ROTATE } input_key;
unsigned char push_buff[MAX_BUTTON];
void user_signal1(int sig)
{
   quit = 1;
}

void Create();
void Init(int fd);
void draw(int fd);
void drawGameOver(int fd);
input_key input_user(int fd2);
void BlockMove(input_key key, int fd, int fd3);
int DetectFloor();
int DetectBlock();
int DetectLeft();
int DetectRight();
int DetectTop();
void DeleteLine(int fd, int fd3);
int Delete();
int LevelCheck(int fd, int level, int fd3);
void PileBlock(int fd, int fd3);
void drawClear(int fd);
//#include "dot_matrix_font.h"
int buff_size;

int X = 2;
int Y = 0;
int rotate;
int INDEX;
int SCORE = 0;
int LEVEL = 1;
int sconum = 0;
unsigned char data[4];
char *arg[54];
int main()
{

   // int input=atoi(argv[1]);
   //unsigned char push_sw_buff[9];
   int fd;
   int fd2;
   int fd3;
   // char *arg[54];
   int i;


   arg[0] = "0000";
   arg[1] = "0010";
   arg[2] = "0020";
   arg[6] = "0060";
   arg[10] = "0100";
   arg[3] = "0030";
   arg[7] = "0070";
   arg[4] = "0040";
   arg[8] = "0080";
   arg[5] = "0050";
   arg[9] = "0090";

   input_key key = 0;
   if (0> (fd = open("/dev/csemad_dot_matrix", O_WRONLY)))
   {
      printf("failed to open\n");
      return -1;
   }


   if (0> (fd2 = open("/dev/csemad_push_switch_driver", O_RDONLY)))
   {
      printf("failed to opend 2 \n");
      return -1;
   }

   if (0> (fd3 = open("/dev/fpga_fnd", O_RDWR)))
   {
      printf("failed to opend 3 \n");
      return -1;
   }


   printf("DOT_MATRIX_TEST APPLICATION START\n");

   (void)signal(SIGINT, user_signal1);

   printf("Press <ctrl+c> to quit. \n");


   Create();

   buff_size = sizeof(push_buff);

   while (!quit)
   {
      usleep(200000);

      while (!quit)
      {


         draw(fd);
         BlockMove(DOWN, fd, fd3);
         key = input_user(fd2);
         if (key != 0) {
            BlockMove(key, fd, fd3);
            draw(fd);
            usleep(500000);
         }
         /*
         for (i = 0;i<4;i++)
         {
         data[i] = arg[sconum][i] - 0x30;
         }

         write(fd3, &data, 4);
         */
         if (LEVEL == 1)
            usleep(500000);
         else if (LEVEL == 2)
            usleep(400000);
         else if (LEVEL == 3)
            usleep(300000);


      }


   }
   close(fd);
   close(fd2);
   close(fd3);

   return 0;

}
void Create()
{
   int index;
   srand(time(NULL));
   index = rand() % 4;
   INDEX = index;
   X = 2;
   Y = 0;
   rotate = 0;

}
void Init(int fd)
{
   int i = 0;
   for (i = 0;i<10;i++)
   {
      matrix_window[i] = 0;

   }
   write(fd, matrix_window, sizeof(matrix_window));


}

void draw(int fd)
{

   char temp_window[10] = { 0 };
   int i;
   int j = 0;
   for (i = 0;i<10;i++)
   {
      temp_window[i] = matrix_window[i];
   }

   for (i = Y; i<Y + 3; i++)
   {
      temp_window[i] = matrix_window[i] | dot_tetris[INDEX][rotate][j++] << X;
   }


   write(fd, temp_window, sizeof(temp_window));
   printf(" Now Y : %d\n", Y);
}
void drawGameOver(int fd)
{
   //      char temp_window[10]={0};
   sconum = 0;
   int i = 0;
   for (i = 0; i<50; i++)
   {
      write(fd, dot_matrix_full, sizeof(dot_matrix_full));
      usleep(50000);
      write(fd, dot_matrix_blank, sizeof(dot_matrix_blank));
      usleep(50000);
   }

   for (i = 3; i>0;i--)
   {
      write(fd, dot_matrix_font[i], sizeof(dot_matrix_font[i]));
      usleep(500000);
      usleep(500000);
   }

}
input_key input_user(int fd2)
{
   //unsigned char push_buff[MAX_BUTTON];
   //      int buff_size;


   buff_size = sizeof(push_buff);

   read(fd2, push_buff, buff_size);


   if (push_buff[4] == 1) return ROTATE;
   else if (push_buff[6] == 1)return LEFT;
   else if (push_buff[7] == 1)return DOWN;
   else if (push_buff[8] == 1)return RIGHT;
   else return NO;

}
void BlockMove(input_key key, int fd, int fd3)
{

   switch (key)
   {
   case LEFT:
      if (DetectLeft())
      {
         ;
      }
      else
      {
         X++;
      }
      break;
   case RIGHT:
      if (DetectRight())
      {
         ;
      }
      else {
         X--;
      }
      break;
   case DOWN:
      if (DetectFloor() || DetectBlock())
      {
         PileBlock(fd, fd3);
         if (DetectTop())
         {

            drawGameOver(fd);
            Init(fd);
         }
         Create();
      }
      else

         Y++;

      break;
   case ROTATE:
      rotate = (rotate + 1) % 4;
      break;
   default:
      break;
   }

}
int DetectFloor()
{


   if (Y == 7)
   {

      if ((dot_tetris[INDEX][rotate][2] << X) & 0x7f) {
         printf("Y = %d, INDEX = %d , rotate = %d \n", Y, INDEX, rotate);
         return 1;
      }

   }

   if (Y == 8)
   {

      if ((dot_tetris[INDEX][rotate][1] << X) & 0x7f) {
         printf("Y = %d, INDEX = %d , rotate = %d \n", Y, INDEX, rotate);
         return 1;
      }

   }

   if (Y == 9)
   {

      if ((dot_tetris[INDEX][rotate][0] << X) & 0x7f) {
         printf("Y = %d, INDEX = %d , rotate = %d \n", Y, INDEX, rotate);
         return 1;
      }

   }


   return 0;
}



int DetectBlock()
{
   int k = 0;
   for (k = 0;k<3;k++)
   {
      if (matrix_window[k + Y + 1] & (dot_tetris[INDEX][rotate][k] << X))
      {
         printf("Detect Block  Y : %d\n", Y);

         return 1;
      }
   }
   return 0;
}
void PileBlock(int fd, int fd3)
{

   int i = 0;
   for (i = 0;i<3;i++)
   {
      matrix_window[i + Y] |= (dot_tetris[INDEX][rotate][i] << X);

   }

   draw(fd);
   printf("PileBlock  Y : %d\n", Y);

   if (Delete()) {

      DeleteLine(fd, fd3);
   }


   //      write(fd,matrix_window,sizeof(matrix_window));

}
int DetectLeft()
{
   int i = 0;
   for (i = 0;i<3;i++)
   {
      if (0x40 & (dot_tetris[INDEX][rotate][i] << X))
         return 1;
   }

   return 0;
}
int DetectRight()
{
   int i = 0;
   for (i = 0;i<3;i++)
   {
      if (0x1 & (dot_tetris[INDEX][rotate][i] << X))
         return 1;
   }

   return 0;
}
int DetectTop()
{
   int i = 0;
   if (matrix_window[0] & 0x7f)
   {
      return 1;
   }
   return 0;
}

int Delete()
{
   int i = 0;
   int j = 0;
   for (i = 0; i< 10; i++)
   {
      if (matrix_window[i] == 0x7f)
         return 1;
   }

   return 0;
}

void DeleteLine(int fd, int fd3)
{

   sconum += 1;
   int k = 0;
   for (k = 0;k<4;k++)
   {
      data[k] = arg[sconum][k] - 0x30;
   }

   write(fd3, &data, 4);

   int i = 0;
   int j = 9;
   while (j != 0)
   {

      if (matrix_window[j] == 0x7f)
      {
         for (i = 0;i<4;i++)
         {
            matrix_window[j] = 0x0;
            usleep(80000);
            write(fd, matrix_window, sizeof(matrix_window));
            matrix_window[j] = 0x7f;
            usleep(80000);
            write(fd, matrix_window, sizeof(matrix_window));

         }

         for (i = 0;i<6;i++)
         {
            matrix_window[j] = matrix_window[j] ^ (0x1 << i);
            write(fd, matrix_window, sizeof(matrix_window));

            usleep(80000);
         }

         for (i = j; i >= 0; i--)
         {
            matrix_window[i] = matrix_window[i - 1];
         }

         SCORE++;
      }

      if (LevelCheck(fd, LEVEL, fd3))
      {
         Init(fd);
         Create();
      }

      j--;

   }

}

int LevelCheck(int fd, int level, int fd3)
{

   if (level == 1) {
      if (SCORE == 1) {
         LEVEL = 2;
         SCORE = 0;
         sconum = 0;
         int k = 0;
         for (k = 0;k<4;k++)
         {
            data[k] = arg[sconum][k] - 0x30;
         }
         write(fd, dot_matrix_blank, sizeof(dot_matrix_blank));
         drawClear(fd);
         usleep(500000);
         write(fd, dot_matrix_font[2], sizeof(dot_matrix_font[2]));
         usleep(500000);
         write(fd3, &data, 4);
         return 1;
      }
   }
   else if (level == 2)
   {
      if (SCORE == 2)
      {
         LEVEL = 3;
         SCORE = 0;
         sconum = 0;
         int k = 0;
         for (k = 0;k<4;k++)
         {
            data[k] = arg[sconum][k] - 0x30;
         }
         write(fd, dot_matrix_blank, sizeof(dot_matrix_blank));
         drawClear(fd);
         usleep(500000);
         write(fd, dot_matrix_font[3], sizeof(dot_matrix_font[3]));
         usleep(500000);
         write(fd3, &data, 4);
         return 1;
      }
   }
   else if (level == 3)
   {
      if (SCORE == 3)
      {

         int i = 0;
         for (i = 0;i<10;i++)
         {
            sconum = 0;
            write(fd, dot_matrix_blank, sizeof(dot_matrix_blank));
            write(fd, dot_matrix_full, sizeof(dot_matrix_full));
         }

         return 1;
      }

   }

   return 0;
}

void drawClear(int fd)
{


   unsigned char levelup_matrix[10] = { 0 };
   int i = 0;
   for (i = 9;i>4;i--)
   {


      levelup_matrix[i] = (0x1 << 3);
      write(fd, levelup_matrix, sizeof(levelup_matrix));
      usleep(100000);

   }

   for (i = 0;i<3;i++)
   {
      write(fd, dot_matrix_levelup[i], sizeof(dot_matrix_levelup[i]));
      usleep(500000);
   }

   for (i = 0;i<10;i++)
   {
      int j = 0;

      write(fd, dot_matrix_levelup[0], sizeof(dot_matrix_levelup[0]));
      usleep(70000);
      write(fd, dot_matrix_levelup[1], sizeof(dot_matrix_levelup[1]));
      usleep(70000);
      write(fd, dot_matrix_levelup[2], sizeof(dot_matrix_levelup[2]));
      usleep(70000);
      write(fd, dot_matrix_levelup[1], sizeof(dot_matrix_levelup[1]));
      usleep(70000);
   }

}

