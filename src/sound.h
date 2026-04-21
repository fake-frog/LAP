#ifndef SOUND_H
#define SOUND_H

#define SAMPLE_RATE 44100.0
#define FREQ        440.0

typedef struct LAPDevice LAPDevice;

typedef struct {
  float sample_rate;
  int format_ID;
  char format_flags;
  int bits_per_channel;
  int channels_per_frame;
  int frames_per_packet;
  int bytes_per_frame;
  int bytes_per_packet;
} LAPDescriptor;


LAPDevice *lap_create_device(LAPDescriptor descriptor);
void lap_play_sound(LAPDevice *device);
void lap_cleanup(LAPDevice *device);

#endif
