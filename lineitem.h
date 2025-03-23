#ifndef LINEITEM_H
#define LINEITEM_H

#include <QGraphicsLineItem>
#include <QPointF>

class LineItem : public QGraphicsLineItem {
public:
    LineItem(const QPointF &start,
             const QPointF &end,
             QGraphicsItem *parent = nullptr); // Конструктор с параметрами
    void setEndPoint(const QPointF &end);

private:
    QPointF startPoint;
};

#endif // LINEITEM_H
