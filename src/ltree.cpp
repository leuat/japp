#include "ltree.h"

LTree::LTree() {}

void LTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        event->accept();
        emit emitOnEnter();
    }
    else
    {
        QTreeView::keyPressEvent(event); // call the default implementation
    }
}
