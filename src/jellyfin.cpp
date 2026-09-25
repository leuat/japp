#include "jellyfin.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
#include <QNetworkAccessManager>

JellyfinAudioPlayer::JellyfinAudioPlayer(QObject *parent) : QObject(parent) {
    m_networkManager = new QNetworkAccessManager(this);

    // Initialize the media player engine
    m_mediaPlayer = new QMediaPlayer(this);

    // Qt 6 requires attaching an explicit audio output device
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    // Optional: Set default volume (range 0.0 to 1.0)
    m_audioOutput->setVolume(0.8f);

    // FIX: Clean, classic Qt connection without lambdas
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged,
            this, &JellyfinAudioPlayer::onPlaybackStateChanged);

}

void JellyfinAudioPlayer::onAuthResponse(QNetworkReply *reply) {
    reply->deleteLater();

    qDebug() << "Reply READY";

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Authentication Failed:" << reply->errorString();
        return;
    }

    // 4. Extract API access token
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = doc.object();

    m_accessToken = jsonObj["AccessToken"].toString();
    qDebug() << "Authenticated! Token successfully retrieved.";

    // 5. Construct the Direct MP3 Audio Stream URL
    // We explicitly request container=mp3 and audioCodec=mp3 to enforce standard MP3 routing.
    QString streamUrl = QString("%1/Audio/%2/stream?static=true&container=mp3&audioCodec=mp3&api_key=%3")
                            .arg(m_serverUrl)
                            .arg(m_itemId)
                            .arg(m_accessToken);

    playAudioStream(streamUrl);
}

void JellyfinAudioPlayer::playAudioStream(const QString &streamUrl) {
    qDebug() << "Streaming from target URL:" << streamUrl;

    // 6. Set source and play background audio
    m_mediaPlayer->setSource(QUrl(streamUrl));
    m_mediaPlayer->play();
}

void JellyfinAudioPlayer::onNetworkError(QNetworkReply::NetworkError error) {
    qDebug() << "onNetworkError";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        qDebug() << "Network Error Code:" << error << "Message:" << reply->errorString();
    }
}

void JellyfinAudioPlayer::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
    qDebug() << "Player State Changed:" << state;
}

void JellyfinAudioPlayer::connectAndPlayAudio(const QString &itemId) {
    m_serverUrl = jinfo.server;
    m_itemId =  itemId;

    if (m_serverUrl.endsWith("/")) {
        m_serverUrl.chop(1);
    }

    QUrl url(m_serverUrl + "/Users/AuthenticateByName");
    QNetworkRequest request(url);

    // Explicitly define both what we are sending and what we accept back
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    // Format the authorization parameters exactly how Jellyfin requests them
    QString authHeader = "MediaBrowser,  Client=\"QtAudioPlayer\", "
                         "Device=\"PC\", "
                         "DeviceId=\"QtAudio123\", "
                         "Version=\"1.0.0\"";

    request.setRawHeader("X-Emby-Authorization", authHeader.toUtf8());
    request.setRawHeader("Authorization", "MediaBrowser " + authHeader.toUtf8());

    // Build payload
    QJsonObject json;
    json["Username"] =  jinfo.uid;
    json["Pw"] =  jinfo.pwd;

    QJsonDocument doc(json);


    // Send request (ensure we use Compact json parsing to prevent payload corruption)
    QNetworkReply *reply = m_networkManager->post(request, doc.toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() { this->onAuthResponse(reply); });
    connect(reply, &QNetworkReply::errorOccurred, this, &JellyfinAudioPlayer::onNetworkError);
}
