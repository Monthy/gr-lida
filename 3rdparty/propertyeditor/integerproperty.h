#ifndef INTEGERPROPERTY_H
#define INTEGERPROPERTY_H

#include <QSpinBox>

#include "propertyitem.h"
#include "propertybutton.h"

class IntegerProperty : public PropertyItem
{
public:
	IntegerProperty(const QString &name, int value = 0, int defaultValue = 0, int min = INT_MIN, int max = INT_MAX);
	IntegerProperty(PropertyItem *top, const QString &name, int value = 0, int defaultValue = 0, int min = INT_MIN, int max = INT_MAX);

	void setValue(int value);
	int getValue() const;

	void setDefaultValue(int value) { m_defaultValue = value; }
	int getDefaultValue() const { return m_defaultValue; }

	void setRange(int min, int max);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();
	virtual void validateValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable int m_value;
	mutable QSpinBox *m_editor;
	int m_defaultValue;
	int m_min, m_max;

	class ButtonsEditor : public TPropertyButton<QSpinBox>
	{
	public:
		ButtonsEditor(QSpinBox *editor, IntegerProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		IntegerProperty *m_property;
	};
};

#endif // INTEGERPROPERTY_H
