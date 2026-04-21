#include "sound.h"

int main(void) {
  LAPDescriptor desc = {0};
  desc.sample_rate	  = SAMPLE_RATE;
  desc.format_ID	  = 0;
  desc.format_flags	  = 0;
  desc.bits_per_channel	  = 32;
  desc.channels_per_frame = 2;
  desc.frames_per_packet  = 1;
  desc.bytes_per_frame	  = 4;
  desc.bytes_per_packet	  = 4;
  
  LAPDevice *device = lap_create_device(desc);
  
  lap_play_sound(device);
  lap_cleanup(device);
}
