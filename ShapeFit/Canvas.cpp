#include "Canvas.h"
#include <QPainter>
#include <QString>
#include <QFile>
#include <iostream>
#include <fstream>

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);

	this->setFocusPolicy(Qt::StrongFocus);
	//this->setMouseTracking(true);
	//this->setCursor(Qt::BlankCursor);
}

void Canvas::clear() {
	image.fill(qRgba(255, 255, 255, 0));
	update();
}

void Canvas::resizeImage(const QSize &newSize) {
	QImage newImage(newSize, QImage::Format_RGB888);
	newImage.fill(qRgba(255, 255, 255, 0));
	QPainter painter(&newImage);
	painter.drawImage(QPoint(0, 0), image);
	image = newImage;
}

void Canvas::drawLineTo(const QPoint &endPoint) {
	QPoint pt1((float)lastPoint.x() - 0.5, (float)lastPoint.y() - 0.5);
	QPoint pt2((float)endPoint.x() - 0.5, (float)endPoint.y() - 0.5);

	QPainter painter(&image);
	painter.setPen(QPen(QColor(0, 0, 0), 1));
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.drawLine(pt1, pt2);

	lastPoint = endPoint;
}

void Canvas::paintEvent(QPaintEvent * /* event */) {
	QPainter painter(this);
	painter.drawImage(QPoint(0, 0), image);
}

void Canvas::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		lastPoint = event->pos();
	}
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
	drawLineTo(event->pos());
	update();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
	cv::Mat mat(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
	cv::Mat grayMat;
	cv::cvtColor(mat, grayMat, CV_BGR2GRAY);

	std::vector<float> params = shapeFit.fit(grayMat, mat.cols * mat.rows * 0.1);

	// show the result
	cv::rectangle(mat, cv::Rect(params[0], params[1], params[2], params[3]), cv::Scalar(255, 0, 0), 2, CV_AA);
	update();
}

void Canvas::resizeEvent(QResizeEvent *event) {
	resizeImage(event->size());
	QWidget::resizeEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent* event) {
}

void Canvas::keyReleaseEvent(QKeyEvent* event) {
}
