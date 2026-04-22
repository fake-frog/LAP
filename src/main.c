#include "sound.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

static void raw_mode(void) {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main(void) {
  
  LAPDescriptor desc = {0};
  desc.sample_rate	  = SAMPLE_RATE;
  desc.bits_per_channel	  = 32;
  desc.channels_per_frame = 2;
  desc.frames_per_packet  = 1;
  desc.bytes_per_frame	  = 4;
  desc.bytes_per_packet	  = 4;
  
  LAPDevice *device = lap_create_device(desc);
  
  lap_start_sound(device);

   raw_mode();
   
   printf("a/z: freq   s/x: gain   q: quit\n");

    int c;
    while ((c = getchar()) != 'q') {
        switch (c) {
	    case 'a': lap_change_freq(0.9439); break;
            case 'z': lap_change_freq(1.0595); break;
	    case 's': lap_change_gain(-0.05f); break;
	    case 'x': lap_change_gain(0.05f); break;
        }
        //printf("freq=%.1f Hz  gain=%.2f   \r", g_freq, g_gain);
        fflush(stdout);
    }
  
  lap_cleanup(device);
  
  return 0;
}
