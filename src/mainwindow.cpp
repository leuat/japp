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
    player.connectAndPlayAudio("088a8036cb52a2a346dbea83dd2a6dc5");

//    connect(this, SLOT(onConnected()), &player, SIGNAL(emitConnected()));
    connect(&player, SIGNAL(emitConnected()), this, SLOT(onConnected()));
    connect(&player, SIGNAL(emitData(QJsonDocument, int)), this, SLOT(onData(QJsonDocument, int)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onData(QJsonDocument doc, int type) {
    qDebug() << "ONDATA";
//    qDebug().noquote() << doc.toJson(QJsonDocument::Indented);;
    ui->tree->clear();
    auto lst = doc["Items"].toArray();
    int i;
    for (auto item: lst) {
        auto name = item.toObject()["Name"].toString();
        auto mtype = item.toObject()["Type"].toString();
        qDebug() << name;
        QStringList l = QStringList() << name;// <<item.toObject()["ID"].toString();
        ui->tree->insertTopLevelItem(0,new QTreeWidgetItem(l));
        auto id = item.toObject()["Id"].toString();
        m_ids[name] = JItem(name, id, mtype);
    }

}

void MainWindow::onConnected()
{
//    qDebug() << "Connected";
    QJsonObject obj;
    obj["limit"] = "10";
//    player.getData("/Users/"+player.jinfo.sid+"/Views",obj);
//    player.getData("/Users/"+player.jinfo.sid+"/Views",obj);
//    player.getData("/Items"+m_curPath,obj,1);
//"e63d82536ac787e9ffd9f68b75a00e8e
    player.getData("/Items?ParentID="+m_curPath,obj,1);
}

void MainWindow::on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QString m = item->text(0);
    if (m_ids[m].m_type=="Audio") {
        player.playAudio(m_ids[m].m_id);
        return;
    }
    m_curPath = m_ids[m].m_id;//m_curPath + "/" + item->text(0);
    onConnected();
}

