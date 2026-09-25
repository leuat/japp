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
    JItem(QString name, QString id, QString type, QString info): m_name(name), m_id(id), m_type(type), m_info(info) {

    }
    QString m_name, m_id, m_type, m_artist, m_info;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    JellyfinAudioPlayer player;
    QString m_curPath = "";
    bool m_isDraggingSlider = false;
    const int TYPE_LIST = 1;
    const int TYPE_INFO = 2;
    QNetworkAccessManager networkManager;

    float scale = 1000;
    QMap<QString, JItem> m_ids;
    QTreeWidgetItem *curItem = nullptr;
private slots:
    void onConnected();
    void onData(QJsonDocument doc, int type);
    void on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void onTreeEnter();

    void onUpdateProgress(qint64 progress);

    void on_btnStop_clicked();

    void on_btnPlay_clicked();



    void on_sliderProgress_actionTriggered(int action);

    void on_sliderProgress_sliderMoved(int position);

    void on_sliderProgress_sliderReleased();

    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void setPlayIcon();
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
