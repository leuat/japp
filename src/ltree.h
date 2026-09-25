#ifndef LTREE_H
#define LTREE_H

#include <QTreeWidget>
#include <QKeyEvent>
#include <QDebug>


class LTree : public QTreeWidget
{
    Q_OBJECT
public:
    LTree();
    LTree(QWidget* other) {

    }

    void keyPressEvent(QKeyEvent *event) override;

signals:
    void emitOnEnter();
};

#endif // LTREE_H
