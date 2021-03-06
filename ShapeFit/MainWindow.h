#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "Canvas.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindowClass ui;
	Canvas* canvas;

public:
	MainWindow(QWidget *parent = 0);

public slots:
	void onNew();
};

#endif // MAINWINDOW_H
