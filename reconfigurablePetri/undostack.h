#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QUndoStack>
#include "command.h"
#include"ptnet.h"
class UndoStack:public QUndoStack
{
  Q_OBJECT
 public:
    explicit UndoStack(QObject * parent = 0 );
    UndoStack(PTNet *net,QObject * parent = 0);
    ~UndoStack();
    PTNet*mynet;
public slots:
    void arcInserted(QGraphicsItem * item1, QGraphicsItem * item2,
                        const QPainterPath &arc_path,
                    const QString &id, PTNscene * scene,double ArcWeight);
    void connectorInserted(QGraphicsItem * item1, QGraphicsItem * item2,
                        const QPainterPath &connector_path,
                    const QString &id, PTNscene * scene);
    void itemMoved (QGraphicsItem * movedItem, const QPointF &oldPosition);
        void nodeRemoved (QGraphicsItem * item, PTNscene * scene);
        void arcRemoved (QGraphicsItem * item, PTNscene * scene);
        void arcClicked(QGraphicsItem * item, PTNscene * scene);

};

#endif
