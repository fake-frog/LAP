#include "sound.h"
#include <jack/jack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct LAPDevice {
  jack_port_t *out_port;
  jack_client_t *client;
};

static volatile double g_freq = 440.0;
static volatile float g_gain = 0.2f;
static double phase = 0.0;
// static double phase_inc = 2.0 * M_PI * FREQ / SAMPLE_RATE;
int process(jack_nframes_t nframes, void *arg) {
  LAPDevice *device = (LAPDevice *)arg;
  float *buf = jack_port_get_buffer(device->out_port, nframes);
  jack_nframes_t rate = jack_get_sample_rate(device->client);

  __sync_synchronize();
  double f = g_freq;
  float gain = g_gain;
  double inc = 2.0 * M_PI * f / SAMPLE_RATE;

  for (int i = 0; i < nframes; i++) {
    buf[i] = sinf(phase) * gain;
    phase += inc;
    if (phase > 2.0 * M_PI)
      phase -= 2.0 * M_PI;
  }
  return 0;
}

LAPDevice *lap_create_device(LAPDescriptor descriptor) {
  LAPDevice *device = malloc(sizeof(LAPDevice));
  device->client = jack_client_open("sine", JackNullOption, NULL);
  device->out_port = jack_port_register(
      device->client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  jack_set_process_callback(device->client, process, device);
  jack_activate(device->client);

  return device;
}

void lap_change_gain(float amount) {
  if (g_gain <= 0.05f && amount < 0) {
    g_gain = 0.0f;
    return;
  }
  if (g_gain >= 0.95f && amount > 0) {
    g_gain = 1.0f;
    return;
  }
  g_gain += amount;
  __sync_synchronize();
}

void lap_change_freq(double amount) {
  g_freq *= amount;
  __sync_synchronize();
}

void lap_start_sound(LAPDevice *device) {
  jack_connect(device->client, "sine:out", "system:playback_1");
  jack_connect(device->client, "sine:out", "system:playback_2");
}

void lap_cleanup(LAPDevice *device) {
  jack_client_close(device->client);
  free(device);
}
