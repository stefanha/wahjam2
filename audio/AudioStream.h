// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include "RingBuffer.h"

// A time value, counted in audio samples
typedef uint64_t SampleTime;

enum {
    CHANNEL_LEFT = 0,
    CHANNEL_RIGHT = 1,
    CHANNELS_STEREO = 2, // channels
};

enum
{
    // How many milliseconds of buffer space to preallocate
    GENEROUS_BUFFER_MSEC = 200,

    // Periodic non-real-time buffer processing interval
    SAFE_PERIODIC_TICK_MSEC = 50,
};

// Number of samples for a given duration
inline constexpr int msecToSamples(int sampleRate, int msec)
{
    return sampleRate * 1000 / msec;
}

// Duration rounded up to the next millisecond for a given number of samples
inline constexpr int samplesToMsec(int sampleRate, int nsamples)
{
    return (nsamples + sampleRate / 1000 - 1) * 1000 / sampleRate;
}

inline void mixSamples(const float *in, float *out, size_t nsamples, float vol)
{
    while (nsamples > 0) {
        *out++ += *in++ * vol;
        nsamples--;
    }
}

// Mark a method safe to call from real-time code
#define realtime

/*
 * Audio data is transferred between real-time code and non-real-time code
 * via AudioStream.  Each stream is a single, non-interleaved channel of audio
 * samples.  Stereo audio data requires two streams: one for the left channel
 * and one for the right channel.
 */
class AudioStream
{
public:
    AudioStream(size_t sampleBufferSize = 0);
    ~AudioStream();

    // Call from non-real-time thread, discards queued data
    void setSampleBufferSize(size_t nsamples);

    // Returns true if stream has been reset (time has restarted from zero and
    // the sample rate may have changed).  Call from non-real-time thread.
    bool checkResetAndClear();

    // Returns the number of samples that there is space for
    realtime size_t numSamplesWritable() const;

    // Returns the number of samples available for reading
    realtime size_t numSamplesReadable() const;

    // Returns number of samples written (e.g. before buffer was full)
    realtime size_t write(SampleTime now, const float *samples,
                          size_t nsamples);

    // Returns number of samples read (e.g. before buffer was empty)
    realtime size_t read(SampleTime now, float *samples, size_t nsamples);
    realtime size_t readMixStereo(SampleTime now,
                                  float *samples[CHANNELS_STEREO],
                                  size_t nsamples);
    realtime size_t readDiscard(SampleTime now, size_t nsamples);

    realtime float getPan() const;
    realtime void setPan(float pan_);
    realtime bool monitorEnabled() const;
    realtime void setMonitorEnabled(bool enabled);

private:
    /*
     * Audio is transferred in a packet called AudioDescriptor.  Each descriptor
     * includes a timestamp for time synchronization.  Timestamps make it
     * possible to represent gaps in audio data.
     */
    struct AudioDescriptor
    {
        float *samples;
        size_t nsamples;
        SampleTime time;
    };

    RingBuffer<AudioDescriptor> ring;
    float *sampleBuffer;
    size_t sampleBufferSize;
    size_t writeIndex;
    std::atomic<size_t> samplesQueued;
    std::atomic<float> pan; // -1 - left, 0 - center,  1 - right
    std::atomic<bool> monitor; // mix into output?
    bool wasReset;

    // Read n samples from input[] with offset from beginning of the read operation
    typedef void ReadFn(size_t offset, const float *input, size_t n);

    realtime size_t readInternal(SampleTime now,
                                 std::function<ReadFn> fn,
                                 size_t nsamples);
};

#undef realtime
