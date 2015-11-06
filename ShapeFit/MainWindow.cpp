#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	canvas = new Canvas(this);
	this->setCentralWidget(canvas);
}

void MainWindow::onNew() {
	canvas->clear();
}
