#ifndef PROPERTYITEM_H
#define PROPERTYITEM_H

#include <QTreeWidgetItem>

class PropertyItem: public QTreeWidgetItem
{
public:
	PropertyItem(const QString &name);
	PropertyItem(PropertyItem *top, const QString &name);

	const QString &getName() const { return m_name; }
	void setKey(const QByteArray &key) { m_key = key; }
	const QByteArray &getKey() const { return m_key; }
	bool isChecked() const;
	bool isCheckable() const;

	void setBackground(const QBrush &bg);
	void setTextColor(const QColor &color);

// handlers to reimplement
	virtual void onAdded() {}
	virtual void onShowEditor(QWidget */*editWidget*/) {}
	virtual bool onKeyPressed(QKeyEvent */*event*/, QWidget */*editWidget*/) { return false; }

// actions to reimplement
	virtual QVariant getVariantValue() const = 0;
	virtual void setValue();
	virtual void validateValue() {}
	virtual void displayValue() {}

	virtual QWidget *createEditor() const { return NULL; }
	virtual void valueToEditor() {}
	virtual void valueFromEditor() {}
	virtual void startEdit();
	virtual void finishEdit(bool cancel = false);

public Q_SLOTS:
	void setChecked(Qt::CheckState state = Qt::Unchecked);
	void setChecked(bool checked = false);

protected:
	friend class PropertyEditor;

	QWidget *getActiveEditor();
	void setEditorPrivate();

	QString m_name;
	QByteArray m_key;
	bool m_isCheckable;
	bool m_editorIsPrivate;
};

#endif // PROPERTYITEM_H
