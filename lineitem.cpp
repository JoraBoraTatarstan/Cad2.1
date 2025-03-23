#include "lineitem.h"
#include <QPen>

LineItem::LineItem(const QPointF &start,
                   const QPointF &end,
                   QGraphicsItem *parent)
    : QGraphicsLineItem(QLineF(start, end), parent), startPoint(start) {
    setPen(QPen(Qt::black, 2));
}

void LineItem::setEndPoint(const QPointF &end) {
    QLineF newLine(startPoint, end);
    setLine(newLine);
}
