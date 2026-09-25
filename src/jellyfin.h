#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>


class JInfo {
public:
    QString server, uid, pwd;
    JInfo() {}
    JInfo(QString s, QString u, QString p):server(s), uid(u), pwd(p) {
    }
};

class JellyfinAudioPlayer : public QObject {
    Q_OBJECT

public:
    explicit JellyfinAudioPlayer(QObject *parent = nullptr);
    JInfo jinfo;
    ~JellyfinAudioPlayer() = default;

    // Start the process: Login -> Get Token -> Build Stream -> Play
    void connectAndPlayAudio( const QString &itemId);


private slots:
    void onAuthResponse(QNetworkReply *reply);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onNetworkError(QNetworkReply::NetworkError error);
private:
    void playAudioStream(const QString &streamUrl);
    QNetworkAccessManager *m_networkManager;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;

    QString m_serverUrl;
    QString m_itemId;
    QString m_accessToken;
};
