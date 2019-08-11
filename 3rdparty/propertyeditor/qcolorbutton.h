#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include <QToolButton>

class QColorButton : public QToolButton
{
	Q_OBJECT
public:
	explicit QColorButton(QWidget *parent = 0);

	QColor currentColor() const;
	QString colorName(const QColor &color) const;
	static QIcon colorIcon(const QColor &color, QSize size = QSize(14, 14));

Q_SIGNALS:
	void currentColorChanged(const QColor &color);

public Q_SLOTS:
	void setCurrentColor(const QColor &color);

protected:
	void paintEvent(QPaintEvent *event);

private:
	QColor m_color;
};

#endif // QCOLORBUTTON_H
