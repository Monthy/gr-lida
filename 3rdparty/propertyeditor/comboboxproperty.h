#ifndef COMBOBOXPROPERTY_H
#define COMBOBOXPROPERTY_H

#include <QComboBox>

#include "listdataitem.h"
#include "propertyitem.h"
#include "propertybutton.h"

class ComboBoxProperty : public PropertyItem
{
public:
	ComboBoxProperty(const QString &name, const ListData &list, int index = 0, int defaultIndex = 0, QSize iconSize = QSize(16, 16));
	ComboBoxProperty(PropertyItem *top, const QString &name, const ListData &list, int index = 0, int defaultIndex = 0, QSize iconSize = QSize(16,16));

	void setIndex(int index);
	int getIndex() const;

	void setIconSize(const QSize &size);
	QSize iconSize() const { return m_iconSize; }

	void setMinimumHeight(int minh) { m_minHeight = minh; }

	void setDefaultIndex(int index) { m_defaultIndex = index; }
	int getDefaultIndex() const { return m_defaultIndex; }

	bool isEditable() const { return m_editable; }
	void setEditable(bool editable) { m_editable = editable; }

	void setList(const ListData &list);
	ListData getList() const { return m_listData; }

	int indexOf(const QString &value, int from = 0) const;
	int indexOfData(const QString &value, int from = 0) const;

// event handlers
	virtual void onShowEditor(QWidget *editWidget);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();
	virtual void validateValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable int m_index;
	mutable QComboBox *m_editor;
	ListData m_listData;
	int m_defaultIndex;
	bool m_editable;
	QSize m_iconSize;
	int m_minHeight;

	class ButtonsEditor : public TPropertyButton<QComboBox>
	{
	public:
		ButtonsEditor(QComboBox *editor, ComboBoxProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		ComboBoxProperty *m_property;
	};

private:
	int findIndexOf(const QString &value, int from = 0, bool isData = false) const;

};

#endif // COMBOBOXPROPERTY_H
