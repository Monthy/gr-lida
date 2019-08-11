#include <QPainter>
#include <QPaintEvent>

#include "qcolorbutton.h"

QColorButton::QColorButton(QWidget *parent):
	QToolButton(parent)
{
	setText("");
	setCurrentColor(Qt::black);
}

void QColorButton::paintEvent(QPaintEvent *event)
{
	QToolButton::paintEvent(event);

	int w = size().width();
	int h = size().height();

	QPainter painter(this);
	painter.fillRect(3, 3, w - 7, h - 7, m_color);
	painter.setPen(Qt::black);
	painter.drawRect(3, 3, w - 7, h - 7);
	painter.end();
}

QColor QColorButton::currentColor() const
{
	return m_color;
}

void QColorButton::setCurrentColor(const QColor &color)
{
	m_color = color;
	Q_EMIT currentColorChanged(color);
}

QIcon QColorButton::colorIcon(const QColor &color, QSize size)
{
// create icon
	QPixmap pixmap(size.width(), size.height());
	pixmap.fill(color);
	QPen pen(Qt::black, 1);
	QPainter painter(&pixmap);
	painter.drawRect(QRect(0, 0, size.width() - 1, size.height() - 1));
	return QIcon(pixmap);
}

QString QColorButton::colorName(const QColor &color) const
{
	return color.name();
}
