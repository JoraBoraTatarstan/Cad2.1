#include "graphicsview.h"
#include <QGraphicsLineItem>
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsEllipseItem>
#include <QPainterPath>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent), currentLine(nullptr), isDrawing(false), isPanning(false), snapIndicator(nullptr) {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);

    setDragMode(QGraphicsView::NoDrag);
    scene->setSceneRect(0, 0, 5000, 5000);

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setRenderHint(QPainter::SmoothPixmapTransform);
}

GraphicsView::~GraphicsView() {}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = true;
        panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::LeftButton) {
        startPoint = mapToScene(event->pos());
        startPoint = findClosestSnapPoint(startPoint);

        currentLine = new LineItem(startPoint, startPoint, nullptr);
        currentLine->setPen(QPen(Qt::black, 1));
        scene->addItem(currentLine);
        isDrawing = true;

        lines.append(currentLine); // Сохраняем линию
    } else if (event->button() == Qt::RightButton) {
        closeContourAndFillGrid(); // Закрываем контур и создаем сетку
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (isPanning) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(panStartPos);
        panStartPos = event->pos();
        QTransform transform = this->transform();
        transform.translate(delta.x() * 0.5, delta.y() * 0.5);
        setTransform(transform);
    } else if (isDrawing && currentLine) {
        QPointF rawEndPoint = mapToScene(event->pos());
        QPointF snappedEndPoint = findClosestSnapPoint(rawEndPoint);

        currentLine->setEndPoint(snappedEndPoint);

        if (!snapIndicator) {
            snapIndicator = new QGraphicsEllipseItem();
            snapIndicator->setRect(-3, -3, 6, 6);
            snapIndicator->setBrush(Qt::blue);
            snapIndicator->setPen(Qt::NoPen);
            scene->addItem(snapIndicator);
        }
        snapIndicator->setPos(snappedEndPoint);
        snapIndicator->setVisible(true);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    } else if (event->button() == Qt::LeftButton && isDrawing) {
        isDrawing = false;
        currentLine = nullptr;

        if (snapIndicator) {
            snapIndicator->setVisible(false);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    QPointF scenePos = mapToScene(event->position().toPoint());
    qreal scaleFactor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    scale(scaleFactor, scaleFactor);

    QPointF delta = mapToScene(event->position().toPoint()) - scenePos;
    translate(delta.x(), delta.y());
}

QPointF GraphicsView::findClosestSnapPoint(const QPointF &pos) {
    constexpr qreal SNAP_RADIUS = 10.0;
    QPointF closestPoint = pos;
    qreal minDist = SNAP_RADIUS;

    for (QGraphicsItem *item : scene->items()) {
        if (auto *line = dynamic_cast<LineItem *>(item)) {
            QPointF start = line->line().p1();
            QPointF end = line->line().p2();
            QPointF middle = (start + end) / 2;

            std::vector<QPointF> snapPoints = {start, end, middle};
            for (const QPointF &p : snapPoints) {
                qreal dist = QLineF(pos, p).length();
                if (dist < minDist) {
                    minDist = dist;
                    closestPoint = p;
                }
            }
        }
    }
    return closestPoint;
}

void GraphicsView::closeContourAndFillGrid() {
    if (lines.isEmpty()) return;

    // Проверяем, замкнут ли контур
    if (lines.first()->line().p1() != lines.last()->line().p2()) return;

    // Получаем границы контура
    QPainterPath path;
    path.moveTo(lines.first()->line().p1());
    for (LineItem *line : lines) {
        path.lineTo(line->line().p2());
    }

    QRectF bounds = path.boundingRect();
    qreal step = 20.0; // Шаг сетки

    // Создаём сетку
    for (qreal x = bounds.left(); x <= bounds.right(); x += step) {
        for (qreal y = bounds.top(); y <= bounds.bottom(); y += step) {
            QPointF gridPoint(x, y);
            if (path.contains(gridPoint)) {
                QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(x - 1, y - 1, 2, 2);
                dot->setBrush(Qt::red);
                dot->setPen(Qt::NoPen);
                scene->addItem(dot);
            }
        }
    }
}
