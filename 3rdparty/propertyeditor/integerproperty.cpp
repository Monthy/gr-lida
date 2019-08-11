#include "integerproperty.h"

IntegerProperty::IntegerProperty(const QString &name, int value, int defaultValue, int min, int max):
	PropertyItem(name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_min(min), m_max(max)
{
	setValue(m_value);
}

IntegerProperty::IntegerProperty(PropertyItem *top, const QString &name, int value, int defaultValue, int min, int max):
	PropertyItem(top, name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_min(min), m_max(max)
{
	setValue(m_value);
}

void IntegerProperty::setValue(int value)
{
	m_value = value;
	PropertyItem::setValue();
}

int IntegerProperty::getValue() const
{
	return m_value;
}

void IntegerProperty::setRange(int min, int max)
{
	m_min = min;
	m_max = max;
	PropertyItem::setValue();
}

QVariant IntegerProperty::getVariantValue() const
{
	return m_value;
}

void IntegerProperty::displayValue()
{
	setText(1, QString::number(m_value));
}

void IntegerProperty::validateValue()
{
	if (m_value < m_min)
		m_value = m_min;
	if (m_value > m_max)
		m_value = m_max;
}

QWidget *IntegerProperty::createEditor() const
{
	m_editor = new QSpinBox();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<IntegerProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void IntegerProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setRange(m_min, m_max);
		m_editor->setValue(m_value);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(valueChanged(int)), treeWidget(), SLOT(onValueChanged(int)));
	}
}

void IntegerProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		int value = m_editor->value();
		if (value != m_value)
			setValue(value);
	}
}

// ButtonsEditor --
IntegerProperty::ButtonsEditor::ButtonsEditor(QSpinBox *editor, IntegerProperty *property):
	TPropertyButton<QSpinBox>(editor)
{
	m_property = property;
}

void IntegerProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setValue(m_property->getDefaultValue());
	m_property->finishEdit();
}
