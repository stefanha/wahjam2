// SPDX-License-Identifier: Apache-2.0
#include <QMutex>
#include <QQuickView>
#include <QTimer>
#include "audio/AudioProcessor.h"

// The application including the user interface and audio.
class AppView : public QQuickView
{
    Q_OBJECT

public:
    AppView(const QUrl &url, QWindow *parent = nullptr);

    // Start/stop audio processing, can be called from any thread
    void setAudioRunning(bool enabled);

    // Can be called from any thread
    void setSampleRate(int sampleRate) {
        processor.setSampleRate(sampleRate);
    }

    // Process audio samples. Called from the real-time audio thread.
    void process(float *inOutSamples[CHANNELS_STEREO],
                 size_t nsamples,
                 SampleTime now);

signals:
    // Emitted periodically to allow draining capture streams and refilling
    // playback streams.
    void processAudioStreams();

private slots:
    void processAudioStreamsTick();
    void startProcessAudioStreamsTimer();
    void stopProcessAudioStreamsTimer();

private:
    AudioProcessor processor;

    // Protects setAudioRunning() vs processAudioStreamsTick()
    QMutex processorWriteLock;

    QTimer processAudioStreamsTimer;
};