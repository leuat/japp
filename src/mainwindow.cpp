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
}

MainWindow::~MainWindow()
{
    delete ui;
}
