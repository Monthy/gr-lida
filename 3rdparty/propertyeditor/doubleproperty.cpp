#include <QDoubleSpinBox>

#include "doubleproperty.h"

DoubleProperty::DoubleProperty(const QString &name, double value, double defaultValue, int prec, double min, double max):
	PropertyItem(name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_prec(prec), m_min(min), m_max(max)
{
	setValue(m_value);
}

DoubleProperty::DoubleProperty(PropertyItem *top, const QString &name, double value, double defaultValue, int prec, double min, double max):
	PropertyItem(top, name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_prec(prec), m_min(min), m_max(max)
{
	setValue(m_value);
}

void DoubleProperty::setValue(double value)
{
	m_value = value;
	PropertyItem::setValue();
}

double DoubleProperty::getValue() const
{
	return m_value;
}

void DoubleProperty::setRange(double min, double max)
{
	m_min = min;
	m_max = max;
	PropertyItem::setValue();
}

void DoubleProperty::setDecimals(int prec)
{
	m_prec = prec;
}

// reimp
QVariant DoubleProperty::getVariantValue() const
{
	return m_value;
}

void DoubleProperty::displayValue()
{
	setText(1, QString::number(m_value));
}

void DoubleProperty::validateValue()
{
	if (m_value < m_min)
		m_value = m_min;
	if (m_value > m_max)
		m_value = m_max;
}

QWidget *DoubleProperty::createEditor() const
{
	m_editor = new QDoubleSpinBox();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<DoubleProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void DoubleProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setDecimals(m_prec);
		m_editor->setRange(m_min, m_max);
		m_editor->setValue(m_value);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(valueChanged(double)), treeWidget(), SLOT(onValueChanged(double)));
	}
}

void DoubleProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		double value = m_editor->value();
		if (value != m_value)
			setValue(value);
	}
}

// ButtonsEditor --
DoubleProperty::ButtonsEditor::ButtonsEditor(QDoubleSpinBox *editor, DoubleProperty *property):
	TPropertyButton<QDoubleSpinBox>(editor)
{
	m_property = property;
}

void DoubleProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setValue(m_property->getDefaultValue());
	m_property->finishEdit();
}
