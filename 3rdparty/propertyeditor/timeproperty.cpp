#include "timeproperty.h"

TimeProperty::TimeProperty(const QString &name, const QTime &value, const QTime &defaultValue):
	PropertyItem(name),
	m_defaultValue(defaultValue),
	m_format("HH:mm:ss")
{
	setTime(value);
}

TimeProperty::TimeProperty(PropertyItem *top, const QString &name, const QTime &value, const QTime &defaultValue):
	PropertyItem(top, name),
	m_defaultValue(defaultValue),
	m_format("HH:mm:ss")
{
	setTime(value);
}

void TimeProperty::setTime(const QTime &value)
{
	m_value = value;
	PropertyItem::setValue();
}

QTime TimeProperty::getTime() const
{
	return m_value;
}

void TimeProperty::setMaximumTime(const QTime &value)
{
	m_maxTime = value;
	PropertyItem::setValue();
}

QTime TimeProperty::getMaximumTime() const
{
	return m_maxTime;
}

void TimeProperty::setMinimumTime(const QTime &value)
{
	m_minTime = value;
	PropertyItem::setValue();
}

QTime TimeProperty::getMinimumTime() const
{
	return m_minTime;
}

void TimeProperty::setTimeRange(const QTime &min, const QTime &max)
{
	m_maxTime = max;
	m_minTime = min;
	PropertyItem::setValue();
}

void TimeProperty::setDisplayFormat(const QString &format)
{
	m_format = format;
	displayValue();
}

QString TimeProperty::displayFormat() const
{
	return m_format;
}

// reimp
QVariant TimeProperty::getVariantValue() const
{
	return m_value;
}

void TimeProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	QString timeString = m_format.isEmpty() ? m_value.toString(): m_value.toString(m_format);

	setText(1, timeString);
	setToolTip(1, timeString);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

void TimeProperty::validateValue()
{
	if (m_maxTime.isValid() && m_value > m_maxTime)
		m_value = m_maxTime;
	if (m_minTime.isValid() && m_value < m_minTime)
		m_value = m_minTime;
}

QWidget *TimeProperty::createEditor() const
{
	m_editor = new QTimeEdit();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<TimeProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void TimeProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setTime(m_value);
		m_editor->setTimeRange(m_minTime, m_maxTime);
		m_editor->setDisplayFormat(m_format);
		QObject::connect(m_editor, SIGNAL(timeChanged(QTime)), treeWidget(), SLOT(onValueChanged(QTime)));
	}
}

void TimeProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		QTime value = m_editor->time();
		if (value != m_value)
			setTime(value);
	}
}

// ButtonsEditor --
TimeProperty::ButtonsEditor::ButtonsEditor(QTimeEdit *editor, TimeProperty *property):
	TPropertyButton<QTimeEdit>(editor)
{
	m_property = property;
}

void TimeProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setTime(m_property->getDefaultValue());
	m_property->finishEdit();
}
