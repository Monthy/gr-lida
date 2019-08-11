#ifndef COMBOBOXCHECKPROPERTY_H
#define COMBOBOXCHECKPROPERTY_H

#include "3rdparty/qcheckcombobox.h"
#include "listdataitem.h"
#include "propertyitem.h"
#include "propertybutton.h"

class ComboBoxCheckProperty : public PropertyItem
{
public:
	ComboBoxCheckProperty(const QString &name, const ListData &list, const QString &value = "", const QString &defaultValue = "", const QString &sep = ";", bool useData = false);
	ComboBoxCheckProperty(PropertyItem *top, const QString &name, const ListData &list, const QString &value = "", const QString &defaultValue = "", const QString &sep = ";", bool useData = false);

	void setValue(const QStringList &value);
	void setValue(const QString &value);
	QString getValue() const;

	void setDefaultValue(const QString &value) { m_defaultValue = value; }
	const QString &getDefaultValue() const { return m_defaultValue; }

	void setList(const ListData &list);

// event handlers
	virtual void onShowEditor(QWidget *editWidget);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	ListData m_listData;
	mutable QString m_value;
	mutable QCheckComboBox *m_editor;
	QString m_defaultValue, m_sep;
	bool m_useData;
	int m_index;

	class ButtonsEditor : public TPropertyButton<QCheckComboBox>
	{
	public:
		ButtonsEditor(QCheckComboBox *editor, ComboBoxCheckProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		ComboBoxCheckProperty *m_property;
	};
};

#endif // COMBOBOXCHECKPROPERTY_H
