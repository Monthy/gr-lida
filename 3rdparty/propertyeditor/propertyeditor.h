#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QTreeWidget>

#include "propertyitem.h"

class PropertyEditor : public QTreeWidget
{
	Q_OBJECT
public:
	explicit PropertyEditor(QWidget *parent = 0);

	void adjustToContents();
	bool addProperty(PropertyItem *prop, const QByteArray &key = "");
	bool removeProperty(PropertyItem *prop);

Q_SIGNALS:
	void itemChanged(PropertyItem *item, int column);
	void valueChanged(PropertyItem *item, const QVariant &value);

private Q_SLOTS:
	void onItemClicked(QTreeWidgetItem *item, int column);
	void onItemChanged(QTreeWidgetItem *item, int column);
	void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void onValueChanged(const QString &value);
	void onValueChanged(int value);
	void onValueChanged(bool value);
	void onValueChanged(double value);
	void onValueChanged(const QColor &value);
	void onValueChanged(const QFont &value);
	void onValueChanged(const QDate &value);
	void onValueChanged(const QTime &value);
	void onValueChanged(const QDateTime &value);

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	bool m_addingItem;

private:
	void emitValueChanged(const QVariant &value);
};

#endif // PROPERTYEDITOR_H
