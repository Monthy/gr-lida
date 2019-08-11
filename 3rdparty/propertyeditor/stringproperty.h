#ifndef STRINGPROPERTY_H
#define STRINGPROPERTY_H

#include <QLineEdit>

#include "propertyitem.h"
#include "propertybutton.h"

class StringProperty : public PropertyItem
{
public:
	StringProperty(const QString &name, const QString &value = "", const QString &defaultValue = "", bool multiLine = false);
	StringProperty(PropertyItem *top, const QString &name, const QString &value = "", const QString &defaultValue = "", bool multiLine = false);

	void setValue(const QString &value);
	const QString &getValue() const;

	void setDefaultValue(const QString &value) { m_defaultValue = value; }
	const QString &getDefaultValue() const { return m_defaultValue; }

	void setTextMultiLine(bool multiLine) { m_multiLine = multiLine; }

	void setReadOnly(bool readonly) { m_readonly = readonly; }
	bool isReadOnly() const { return m_readonly; }

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable QString m_value;
	mutable QLineEdit *m_editor;
	QString m_defaultValue;
	bool m_multiLine, m_readonly;

	class ButtonsEditor : public TPropertyButton<QLineEdit>
	{
	public:
		ButtonsEditor(QLineEdit *editor, StringProperty *property);
	protected:
		virtual void onButtonClicked();
		virtual void onButtonDefaultClicked();
		StringProperty *m_property;
	};
};

#endif // STRINGPROPERTY_H
