#include <QCalendarWidget>

#include "dateproperty.h"

DateProperty::DateProperty(const QString &name, const QDate &value, const QDate &defaultValue):
	PropertyItem(name),
	m_defaultValue(defaultValue)
{
	setDate(value);
}

DateProperty::DateProperty(PropertyItem *top, const QString &name, const QDate &value, const QDate &defaultValue):
	PropertyItem(top, name),
	m_defaultValue(defaultValue)
{
	setDate(value);
}

void DateProperty::setDate(const QDate &value)
{
	m_value = value;
	PropertyItem::setValue();
}

QDate DateProperty::getDate() const
{
	return m_value;
}

void DateProperty::setMaximumDate(const QDate &value)
{
	m_maxDate = value;
	PropertyItem::setValue();
}

QDate DateProperty::getMaximumDate() const
{
	return m_maxDate;
}

void DateProperty::setMinimumDate(const QDate &value)
{
	m_minDate = value;
	PropertyItem::setValue();
}

QDate DateProperty::getMinimumDate() const
{
	return m_minDate;
}

void DateProperty::setDateRange(const QDate &min, const QDate &max)
{
	m_maxDate = max;
	m_minDate = min;
	PropertyItem::setValue();
}

void DateProperty::setDisplayFormat(const QString &format)
{
	m_format = format;
	displayValue();
}

QString DateProperty::displayFormat() const
{
	return m_format;
}

// reimp
QVariant DateProperty::getVariantValue() const
{
	return m_value;
}

void DateProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	QString dateString = m_format.isEmpty() ? m_value.toString(): m_value.toString(m_format);

	setText(1, dateString);
	setToolTip(1, dateString);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

void DateProperty::validateValue()
{
	if (m_maxDate.isValid() && m_value > m_maxDate)
		m_value = m_maxDate;
	if (m_minDate.isValid() && m_value < m_minDate)
		m_value = m_minDate;
}

QWidget *DateProperty::createEditor() const
{
	m_editor = new QDateEdit();
	m_editor->setCalendarPopup(true);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<DateProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void DateProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setDate(m_value);
		if (m_minDate.isValid())
			m_editor->setMinimumDate(m_minDate);
		else
			m_editor->clearMinimumDate();
		if (m_maxDate.isValid())
			m_editor->setMaximumDate(m_maxDate);
		else
			m_editor->clearMaximumDate();
		m_editor->setDisplayFormat(m_format);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(dateChanged(QDate)), treeWidget(), SLOT(onValueChanged(QDate)));
	}
}

void DateProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		QDate value = m_editor->date();
		if (value != m_value)
			setDate(value);
	}
}

// ButtonsEditor --
DateProperty::ButtonsEditor::ButtonsEditor(QDateEdit *editor, DateProperty *property):
	TPropertyButton<QDateEdit>(editor)
{
	m_property = property;
}

void DateProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setDate(m_property->getDefaultValue());
	m_property->finishEdit();
}
