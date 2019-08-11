#ifndef DOUBLEPROPERTY_H
#define DOUBLEPROPERTY_H

#include <QDoubleSpinBox>
#include <float.h>

#include "propertyitem.h"
#include "propertybutton.h"

class DoubleProperty : public PropertyItem
{
public:
	DoubleProperty(const QString &name, double value = 0, double defaultValue = 0, int prec = 3, double min = DBL_MIN, double max = DBL_MAX);
	DoubleProperty(PropertyItem *top, const QString &name, double value = 0, double defaultValue = 0, int prec = 3, double min = DBL_MIN, double max = DBL_MAX);

	void setValue(double value);
	double getValue() const;

	void setDefaultValue(double value) { m_defaultValue = value; }
	double getDefaultValue() const { return m_defaultValue; }

	void setRange(double min, double max);
	void setDecimals(int prec);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();
	virtual void validateValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable double m_value;
	mutable QDoubleSpinBox *m_editor;
	double m_defaultValue;
	int m_prec;
	double m_min, m_max;

	class ButtonsEditor : public TPropertyButton<QDoubleSpinBox>
	{
	public:
		ButtonsEditor(QDoubleSpinBox *editor, DoubleProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		DoubleProperty *m_property;
	};
};

#endif // DOUBLEPROPERTY_H
