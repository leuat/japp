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
    QString server, uid, pwd, sid;
    JInfo() {}
    JInfo(QString s, QString u, QString p):server(s), uid(u), pwd(p) {
    }
};

class JellyfinAudioPlayer : public QObject {
    Q_OBJECT

public:
    QString m_accessToken;
    explicit JellyfinAudioPlayer(QObject *parent = nullptr);
    JInfo jinfo;
    ~JellyfinAudioPlayer() = default;

    void connectAndPlayAudio( const QString &itemId);
    void getData(const QString &id, QJsonObject, int type);
    void playAudio(QString s);

signals:
    void emitConnected();
    void emitData(QJsonDocument, int type);

private slots:
    void onDataResponse(QNetworkReply *reply, int type);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onNetworkError(QNetworkReply::NetworkError error);
    void onAuthResponse(QNetworkReply *reply);

private:
    QSharedPointer<QNetworkRequest> request;
    void playAudioStream(const QString &streamUrl);
    QNetworkAccessManager *m_networkManager;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QJsonDocument CreateDoc(QString api, QJsonObject json);

    QString m_itemId;
};
