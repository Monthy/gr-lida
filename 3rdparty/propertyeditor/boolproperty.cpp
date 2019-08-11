#include "boolproperty.h"

BoolProperty::BoolProperty(const QString &name, bool value, bool defaultValue, const QString &textTrue, const QString &textFalse):
	PropertyItem(name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_textTrue(textTrue), m_textFalse(textFalse)
{
	setBgColor(Qt::white);
	setValue(value);
}

BoolProperty::BoolProperty(PropertyItem *top, const QString &name, bool value, bool defaultValue, const QString &textTrue, const QString &textFalse):
	PropertyItem(top, name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_textTrue(textTrue), m_textFalse(textFalse)
{
	setBgColor(Qt::white);
	setValue(value);
}

void BoolProperty::setValue(bool value)
{
	m_value = value;
	PropertyItem::setValue();
}

bool BoolProperty::getValue() const
{
	m_value = (checkState(1) == Qt::Checked);
	return m_value;
}

void BoolProperty::setBoolText(const QString &text)
{
	m_textTrue = text;
	m_textFalse = m_textTrue;
	setText(1, m_value ? m_textTrue : m_textFalse);
}

void BoolProperty::setBoolText(const QString &textTrue, const QString &textFalse)
{
	m_textTrue = textTrue;
	m_textFalse = textFalse;
	setText(1, m_value ? m_textTrue : m_textFalse);
}

void BoolProperty::setBgColor(const QColor &color)
{
	m_bgcolor = color;
}

QVariant BoolProperty::getVariantValue() const
{
	return getValue();
}

void BoolProperty::displayValue()
{
	setCheckState(1, m_value ? Qt::Checked : Qt::Unchecked);
	setText(1, m_value ? m_textTrue : m_textFalse);
}

QWidget *BoolProperty::createEditor() const
{
	m_editor = new QBoolEdit();
	m_editor->setBgColor(m_bgcolor);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<BoolProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void BoolProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setText(m_textTrue, m_textFalse);
		m_editor->setChecked(m_value);
		QObject::connect(m_editor, SIGNAL(toggled(bool)), treeWidget(), SLOT(onValueChanged(bool)));
	}
}

void BoolProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
		setValue(m_editor->isChecked());
}

bool BoolProperty::onKeyPressed(QKeyEvent *event, QWidget *)
{
	if (event->key() == Qt::Key_Return)
	{
		setValue(!m_value);
		return true;
	}
	return false;
}

// ButtonsEditor --
BoolProperty::ButtonsEditor::ButtonsEditor(QBoolEdit *editor, BoolProperty *property):
	TPropertyButton<QBoolEdit>(editor)
{
	m_property = property;
}

void BoolProperty::ButtonsEditor::onButtonDefaultClicked()
{
	m_property->setValue(m_property->getDefaultValue());
	m_property->finishEdit();
}
