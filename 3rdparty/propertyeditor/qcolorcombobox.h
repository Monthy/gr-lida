#ifndef QCOLORCOMBOBOX_H
#define QCOLORCOMBOBOX_H

#include <QComboBox>

class QColorComboBox : public QComboBox
{
	Q_OBJECT
public:
	explicit QColorComboBox(QWidget *parent = 0);

	void setColorsList(const QStringList& colorNames);
	void setColorsList(const QList<QColor>& colors);

	QColor currentColor() const;

	void allowListColorsOnly(bool on);

	bool isListColorsOnly() const { return m_listOnly; }

	QString colorName(const QColor &color) const;

	static QIcon colorIcon(const QColor &color, int size = 16);

	static QStringList defaultColors();
	static QStringList baseColors();

Q_SIGNALS:
	void currentColorChanged(const QColor &color);

public Q_SLOTS:
	void setCurrentColor(const QColor &color);

protected Q_SLOTS:
	void onCurrentIndexChanged(int index);
	void onEdited();
	void onSelectionTextChanged();

private:
	bool m_listOnly;
};

#endif // QCOLORCOMBOBOX_H
