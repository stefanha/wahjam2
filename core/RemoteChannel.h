// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "audio/AudioStream.h"
#include "AppView.h"
#include "RemoteInterval.h"

// A remote audio channel. Handles remote interval playback, including
// controlling pan and monitoring.
class RemoteChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    // TODO mono or stereo?
    // TODO gain?
    Q_PROPERTY(float pan READ pan WRITE setPan);
    Q_PROPERTY(bool monitorEnabled READ monitorEnabled WRITE setMonitorEnabled);

    // Playback stream is out of remote audio samples
    Q_PROPERTY(bool underflow READ underflow NOTIFY underflowChanged);

    // Remote client is sending audio
    Q_PROPERTY(bool remoteSending READ remoteSending NOTIFY remoteSendingChanged);

public:
    typedef std::shared_ptr<RemoteInterval> SharedRemoteInterval;

    RemoteChannel(const QString &name,
                  AppView *appView,
                  QObject *parent = nullptr);
    ~RemoteChannel();

    QString name() const;
    void setName(const QString &name);
    float pan() const;
    void setPan(float pan);
    bool monitorEnabled() const;
    void setMonitorEnabled(bool enable);
    bool underflow() const;
    bool remoteSending() const;

signals:
    void nameChanged(const QString &newName);
    void underflowChanged(bool newValue);
    void remoteSendingChanged(bool newValue);

public slots:
    void processAudioStreams();
    void enqueueRemoteInterval(SharedRemoteInterval remoteInterval);

private:
    AppView *appView;
    AudioStream *playbackStreams[CHANNELS_STEREO];
    QVector<SharedRemoteInterval> intervals;
    QString name_;
    SampleTime nextPlaybackTime;

    bool fillPlaybackStreams();
};
