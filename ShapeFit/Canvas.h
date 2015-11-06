#pragma once

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <iostream>
#include "ShapeFit.h"
class Canvas : public QWidget {
	Q_OBJECT

private:
	QImage image;
	QPoint lastPoint;
	ShapeFit shapeFit;

public:
	Canvas(QWidget* parent = 0);

	void clear();
	void resizeImage(const QSize &newSize);
	void drawLineTo(const QPoint &endPoint);

protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent* keyEvent);
	void keyReleaseEvent(QKeyEvent* keyEvent);

};

