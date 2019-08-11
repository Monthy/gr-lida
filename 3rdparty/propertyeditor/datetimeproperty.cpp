#include <QCalendarWidget>

#include "datetimeproperty.h"

DateTimeProperty::DateTimeProperty(const QString &name, const QDateTime &value, const QDateTime &defaultValue):
	PropertyItem(name),
	m_defaultValue(defaultValue)
{
	setDateTime(value);
}

DateTimeProperty::DateTimeProperty(PropertyItem *top, const QString &name, const QDateTime &value, const QDateTime &defaultValue):
	PropertyItem(top, name),
	m_defaultValue(defaultValue)
{
	setDateTime(value);
}

void DateTimeProperty::setDateTime(const QDateTime &value)
{
	m_value = value;
	PropertyItem::setValue();
}

QDateTime DateTimeProperty::getDateTime() const
{
	return m_value;
}

void DateTimeProperty::setMaximumDateTime(const QDateTime &value)
{
	m_maxDateTime = value;
	PropertyItem::setValue();
}

QDateTime DateTimeProperty::getMaximumDateTime() const
{
	return m_maxDateTime;
}

void DateTimeProperty::setMinimumDateTime(const QDateTime &value)
{
	m_minDateTime = value;
	PropertyItem::setValue();
}

QDateTime DateTimeProperty::getMinimumDateTime() const
{
	return m_minDateTime;
}

void DateTimeProperty::setDateTimeRange(const QDateTime &min, const QDateTime &max)
{
	m_maxDateTime = max;
	m_minDateTime = min;
	PropertyItem::setValue();
}

void DateTimeProperty::setDisplayFormat(const QString &format)
{
	m_format = format;
	displayValue();
}

QString DateTimeProperty::displayFormat() const
{
	return m_format;
}

// reimp
QVariant DateTimeProperty::getVariantValue() const
{
	return m_value;
}

void DateTimeProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	QString dateString = m_format.isEmpty() ? m_value.toString(): m_value.toString(m_format);

	setText(1, dateString);
	setToolTip(1, dateString);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

void DateTimeProperty::validateValue()
{
	if (m_maxDateTime.isValid() && m_value > m_maxDateTime)
		m_value = m_maxDateTime;
	if (m_minDateTime.isValid() && m_value < m_minDateTime)
		m_value = m_minDateTime;
}

QWidget *DateTimeProperty::createEditor() const
{
	m_editor = new QDateTimeEdit();
	m_editor->setCalendarPopup(true);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<DateTimeProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void DateTimeProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setDateTime(m_value);
		if (m_minDateTime.isValid())
			m_editor->setMinimumDateTime(m_minDateTime);
		else
			m_editor->clearMinimumDateTime();
		if (m_maxDateTime.isValid())
			m_editor->setMaximumDateTime(m_maxDateTime);
		else
			m_editor->clearMaximumDateTime();
		m_editor->setDisplayFormat(m_format);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(dateTimeChanged(QDateTime)), treeWidget(), SLOT(onValueChanged(QDateTime)));
	}
}

void DateTimeProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		QDateTime value = m_editor->dateTime();
		if (value != m_value)
			setDateTime(value);
	}
}

// ButtonsEditor --
DateTimeProperty::ButtonsEditor::ButtonsEditor(QDateTimeEdit *editor, DateTimeProperty *property):
	TPropertyButton<QDateTimeEdit>(editor)
{
	m_property = property;
}

void DateTimeProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setDateTime(m_property->getDefaultValue());
	m_property->finishEdit();
}
