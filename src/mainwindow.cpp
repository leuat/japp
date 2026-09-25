#include "mainwindow.h"
#include "ui/ui_mainwindow.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player.jinfo = JInfo("http://192.168.8.138:30013", "resten","Lilleplutt");

    // Start connection and music stream
    player.auth();

//    connect(this, SLOT(onConnected()), &player, SIGNAL(emitConnected()));
    connect(&player, SIGNAL(emitConnected()), this, SLOT(onConnected()));
    connect(&player, SIGNAL(emitData(QJsonDocument, int)), this, SLOT(onData(QJsonDocument, int)));
    connect(ui->tree, SIGNAL(emitOnEnter()), this, SLOT(onTreeEnter()));
    connect(player.m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onUpdateProgress(qint64)));
    ui->tree->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onData(QJsonDocument doc, int type) {
//    ui->tree->clear();q
//    auto curItem = ui->tree->currentItem();
    if (type==TYPE_LIST) {
//            qDebug().noquote() << doc.toJson(QJsonDocument::Indented);;
        if (ui->tree->topLevelItemCount()==0) {
            ui->tree->clear();
            curItem = new QTreeWidgetItem(QStringList()<< "My Library");
            ui->tree->addTopLevelItem(curItem);
        }

        auto lst = doc["Items"].toArray();
        int i;
        QVector<QTreeWidgetItem*> items;
        for (auto item: lst) {
            auto it = item.toObject();
            auto name = it["Name"].toString();
            auto mtype = it["Type"].toString();
            QString info = "<h1>"+name+"</h1><h2>" + mtype+"</h2>";
            if (it.contains("Album")) {
                info+="<p>Album : " + it["Album"].toString();
                info+="<br>Album Artist : " + it["AlbumArtist"].toString();
                auto artists = it["Artists"].toArray();
                info+="<br>Artists: ";
                int cnt = 0;
                for (auto e : artists) {
                    info += e.toString();
                    if (++cnt!=artists.count())
                        info+=", ";
                }

                info+="<br>Production year " +QString::number(it["ProductionYear"].toInt());
                info+="<br>Premiere date " +it["PremiereDate"].toString();
                info+="<br>Has lyrics: " +it["HasLyrics"].toBool()?"True":"False";

            }
//            info.append(it[""])
    //        qDebug() << name;
            QStringList l = QStringList() << name;// <<item.toObject()["ID"].toString();
            auto id = item.toObject()["Id"].toString();
            m_ids[name] = JItem(name, id, mtype, info);

            items.append(new QTreeWidgetItem(l));
        }
      //  qDebug() <<curItem->text(0);
        curItem->addChildren(items);
        curItem->setExpanded(true);
    }
    if (type==TYPE_INFO) {
        qDebug().noquote() << doc.toJson(QJsonDocument::Indented);;

    }
}

void MainWindow::onConnected()
{
    QJsonObject obj;
    player.getData("/Items?ParentID="+m_curPath,obj,TYPE_LIST);
}

void MainWindow::on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item==nullptr)
        return;

    QString m = item->text(0);

    if (m_ids[m].m_type=="Audio") {
        player.playAudio(m_ids[m].m_id);
        setPlayIcon();
        return;
    }
    curItem = item;
    m_curPath = m_ids[m].m_id;//m_curPath + "/" + item->text(0);
    onConnected();
}

void MainWindow::onTreeEnter()
{
    on_tree_itemDoubleClicked(ui->tree->currentItem(), 0);
    //qDebug() << "TREE ENTER";
}

void MainWindow::setPlayIcon()
{
    if (player.m_mediaPlayer->isPlaying()) {
        ui->btnPlay->setIcon(QIcon(":resources/icons/pause.png"));
    }
    else {
        ui->btnPlay->setIcon(QIcon(":resources/icons/play.png"));
    }

}

void MainWindow::onUpdateProgress(qint64 progress)
{
    if (!m_isDraggingSlider)
        ui->sliderProgress->setValue(progress/scale);
    ui->sliderProgress->setMaximum(player.m_mediaPlayer->duration()/scale);
}


void MainWindow::on_btnStop_clicked()
{
    player.m_mediaPlayer->stop();
    setPlayIcon();
}

void MainWindow::on_btnPlay_clicked()
{
    if (player.m_mediaPlayer->isPlaying()) {
        player.m_mediaPlayer->pause();
    }
    else {
        player.m_mediaPlayer->play();
    }
    setPlayIcon();
}






void MainWindow::on_sliderProgress_actionTriggered(int action)
{
    m_isDraggingSlider = true;
}


void MainWindow::on_sliderProgress_sliderMoved(int position)
{
    m_isDraggingSlider = true;
}


void MainWindow::on_sliderProgress_sliderReleased()
{
    player.m_mediaPlayer->setPosition(ui->sliderProgress->sliderPosition()*scale);
    m_isDraggingSlider = false;


}


void MainWindow::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QString c = current->text(0);
    QString imageUrl = player.jinfo.server+"/Items/"+m_ids[c].m_id+"/Images/Primary";
    auto label = ui->lblImage;
    ui->txtInfo->setText(m_ids[c].m_info);
    QNetworkReply *reply = networkManager.get(QNetworkRequest(QUrl(imageUrl)));
    QObject::connect(reply, &QNetworkReply::finished, [reply, label]() {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            int s = label->width()*0.75;
            QImage img = pixmap.toImage().scaled(s,s, Qt::KeepAspectRatio);
            label->setPixmap(QPixmap::fromImage(img));
        } else {
            qDebug() << reply->error();
        }
        reply->deleteLater();
    });
}

