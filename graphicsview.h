#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsEllipseItem>
#include "lineitem.h"
#include <QVector>

class GraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    GraphicsView(QWidget *parent = nullptr);
    ~GraphicsView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QGraphicsScene *scene;
    LineItem *currentLine;
    QPointF startPoint;
    bool isDrawing;
    bool isPanning;
    QPoint panStartPos;
    QGraphicsEllipseItem *snapIndicator;
    QVector<LineItem *> lines; // Храним нарисованные линии

    QPointF findClosestSnapPoint(const QPointF &pos);
    void closeContourAndFillGrid(); // Закрытие контура и создание сетки
};

#endif // GRAPHICSVIEW_H
