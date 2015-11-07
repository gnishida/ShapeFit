#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionOpenCGA, SIGNAL(triggered()), this, SLOT(onOpenCGA()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionPredict, SIGNAL(triggered()), this, SLOT(onPredict()));

	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);
}

void MainWindow::onNew() {
	glWidget->clearSketch();
}

void MainWindow::onOpenCGA() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open CGA file..."), "", tr("CGA Files (*.xml)"));
	if (filename.isEmpty()) return;

	glWidget->loadCGA(filename.toUtf8().data());
}

void MainWindow::onPredict() {
	glWidget->predict();
}