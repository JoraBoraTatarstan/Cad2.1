#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    graphicsView = new GraphicsView(this);
    setCentralWidget(graphicsView);
    setWindowTitle("Qt AutoCAD-like Line Drawer");
    resize(800, 600);
}

MainWindow::~MainWindow() {}
