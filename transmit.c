#include <stdio.h>  /* Standard input/output definitions */
#include <string.h> /* String function definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <stdlib.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>

int sendbytes = 16;
int sending = 1;
void stopSending();

int main(int argc, char *argv[])
{
  //Open communication with arduino
  signal(SIGINT, stopSending);
  if (argc != 2 && argc != 3)
  {
    printf("command format: ./send <interval>\n");
    return 1;
  }
  char *command = "gpio -1 mode 3 out";
  system(command);
  command = "gpio -1 write 3 0";

  system(command);

  char *data = "time to gay\n";
  if (argc > 2)
  {
    data = argv[2];
  }
  int calibrateRemaining = 10;
  int sent = 0;
  int remaining = sizeof(char) * strlen(data);
  time_t previousMillis = 0;
  int ledState = 0; // will store last time LED was updated
  int currentByte = 255;
  int index = 0;
  int flip = 0;
  int nextByte = -1;

  // constants won't change:
  long interval = strtol(argv[1], NULL, 10);
  time_t start = clock();
  struct timeval tv;
  while (sending)
  {
    //Serial.write(0x01);
    // put your main code here, to run repeatedly:
    gettimeofday(&tv, NULL);
    time_t currentMillis = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    /*if (lastAvailable != 0 && currentMillis - lastAvailable >= 5000) {

    digitalWrite(LED_BUILTIN, LOW);
    exit;
  }*/

    if (currentMillis - previousMillis >= interval)
    {

      if (currentByte & (int)(pow(2, index) + 0.5))
      {
        ledState = flip ? 0 : 1;
        //printf("%d",flip ? 0 : 1);
      }
      else
      {
        ledState = flip ? 1 : 0;
        //printf("%d",flip ? 1 : 0);
      }
      previousMillis = currentMillis;
      gettimeofday(&tv, NULL);
      time_t currentMillis = tv.tv_sec * 1000 + tv.tv_usec / 1000;
      command = calloc(18, 1);
      sprintf(command, "gpio -1 write 3 %d", ledState);
      system(command);
      //printf("t=%d\n", currentMillis);
      free(command);

      if (flip)
      {
        index++;
        flip = 0;
      }
      else
      {
        flip = 1;
      }
      if (index >= 8)
      {
        index = 0;
        flip = 0;
        printf("%d\n", currentByte);
        if (currentByte == 2)
        {
          printf("%d\n", ledState);
          sent=0;
          usleep(interval * 1000);
          return (0);
        }
        if (!calibrateRemaining && sent < strlen(data))
        {
          currentByte = data[sent];
          sent++;
        }
        else if (calibrateRemaining == 1)
        {
          currentByte = 0x55;
          calibrateRemaining--;
        }
        else if (sent == strlen(data))
        {
          currentByte = 2;
        }
        else
        {
          calibrateRemaining--;
        }
      }

      // set the LED with the ledState of the variable:
    }

    /*FILE* fileptr = fileptr = fopen(argv[2],"rb");
    if(fileptr == NULL) {
        printf("error: file could not be opened\n");
        return 2;
    }*/
    /*int buffer[sendbytes / sizeof(int) / 2];
    fread(&buffer, sendbytes, 1, fileptr);
    int a = buffer[0];
    int opposite = a ^ 0xffffffff;
    printf("%d%d\n", a, opposite);*/
  }
}
void stopSending(int sig)
{
  sending = 0;
}
