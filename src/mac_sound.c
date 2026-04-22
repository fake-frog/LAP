#include "sound.h"
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

struct LAPDevice {
  AudioComponent comp;
  AudioUnit unit;
};

static volatile double g_freq = 440.0;
static volatile float g_gain = 0.2f;
static double phase = 0.0;
//static double phase_inc = 2.0 * M_PI * FREQ / SAMPLE_RATE;

static OSStatus render_cb(void *ref,
                          AudioUnitRenderActionFlags *flags,
                          const AudioTimeStamp *ts,
                          UInt32 bus,
                          UInt32 frames,
                          AudioBufferList *io)
{
    float *L = (float *)io->mBuffers[0].mData;
    float *R = (float *)io->mBuffers[1].mData;  // if non-interleaved stereo

    
    __sync_synchronize(); 
    double f   = g_freq;
    float gain = g_gain;
    double inc = 2.0 * M_PI * f / SAMPLE_RATE;

    for (UInt32 i = 0; i < frames; i++) {
        float s = (float)sin(phase) * gain;
        L[i] = s;
        R[i] = s;
        phase += inc;
        if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
    }
    return noErr;
}


LAPDevice *lap_create_device(LAPDescriptor descriptor) {
    AudioComponentDescription desc = {
        .componentType         = kAudioUnitType_Output,
        .componentSubType      = kAudioUnitSubType_DefaultOutput,
        .componentManufacturer = kAudioUnitManufacturer_Apple,
    };
    AudioComponent comp = AudioComponentFindNext(NULL, &desc);

    AudioUnit unit;
    AudioComponentInstanceNew(comp, &unit);

    // 2. Describe the format we'll hand it
    AudioStreamBasicDescription fmt = {0};
    fmt.mSampleRate       = descriptor.sample_rate;
    fmt.mFormatID         = kAudioFormatLinearPCM;
    fmt.mFormatFlags      = kAudioFormatFlagIsFloat
                          | kAudioFormatFlagIsNonInterleaved
                          | kAudioFormatFlagIsPacked;
    fmt.mBitsPerChannel   = descriptor.bits_per_channel;
    fmt.mChannelsPerFrame = descriptor.channels_per_frame;
    fmt.mFramesPerPacket  = descriptor.frames_per_packet;
    fmt.mBytesPerFrame    = descriptor.bytes_per_frame;   // per channel, since non-interleaved
    fmt.mBytesPerPacket   = descriptor.bytes_per_packet;

    AudioUnitSetProperty(unit,
                         kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Input, 0,
                         &fmt, sizeof(fmt));

    AURenderCallbackStruct cb = { .inputProc = render_cb, .inputProcRefCon = NULL };
    AudioUnitSetProperty(unit,
                         kAudioUnitProperty_SetRenderCallback,
                         kAudioUnitScope_Input, 0,
                         &cb, sizeof(cb));

    LAPDevice *device = malloc(sizeof(LAPDevice));
    device->comp = comp;
    device->unit = unit;
    
    return device;
}

void lap_change_gain(float amount) {
  if (g_gain > 0.05f && amount < 0) g_gain += amount; else g_gain = 0.0f;
  if (g_gain < 0.95f && amount > 0) g_gain += amount; else g_gain = 1.0f;
  __sync_synchronize();
}

void lap_change_freq(double amount) {
  g_freq *= amount;
  __sync_synchronize();
}

void lap_start_sound(LAPDevice *device) {
    AudioUnitInitialize(device->unit);
    AudioOutputUnitStart(device->unit);
    //    CFRunLoopRun();
}

void lap_cleanup(LAPDevice *device) {
    AudioOutputUnitStop(device->unit);
    AudioUnitUninitialize(device->unit);
    AudioComponentInstanceDispose(device->unit);
    free(device);
}
