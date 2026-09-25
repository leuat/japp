#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "jellyfin.h"
#include <QHash>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class JItem {
public:
    JItem() {

    }
    JItem(QString name, QString id, QString type): m_name(name), m_id(id), m_type(type) {

    }
    QString m_name, m_id, m_type;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    JellyfinAudioPlayer player;
    QString m_curPath = "";
    QMap<QString, JItem> m_ids;

private slots:
    void onConnected();
    void onData(QJsonDocument doc, int type);
    void on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
