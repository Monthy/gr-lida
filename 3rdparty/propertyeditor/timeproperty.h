#ifndef TIMEPROPERTY_H
#define TIMEPROPERTY_H

#include <QTimeEdit>

#include "propertyitem.h"
#include "propertybutton.h"

class TimeProperty : public PropertyItem
{
public:
	TimeProperty(const QString &name, const QTime &value = QTime::currentTime(), const QTime &defaultValue = QTime::currentTime());
	TimeProperty(PropertyItem *top, const QString &name, const QTime &value = QTime::currentTime(), const QTime &defaultValue = QTime::currentTime());

	void setTime(const QTime &value);
	QTime getTime() const;

	void setDefaultValue(const QTime &value) { m_defaultValue = value; }
	QTime getDefaultValue() const { return m_defaultValue; }

	void setMaximumTime(const QTime &value);
	QTime getMaximumTime() const;
	void setMinimumTime(const QTime &value);
	QTime getMinimumTime() const;
	void setTimeRange(const QTime &min, const QTime &max);

	void setDisplayFormat(const QString &format);
	QString	displayFormat() const;

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();
	virtual void validateValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable QTime m_value;
	mutable QTimeEdit *m_editor;
	QTime m_defaultValue, m_maxTime, m_minTime;
	QString m_format;

	class ButtonsEditor : public TPropertyButton<QTimeEdit>
	{
	public:
		ButtonsEditor(QTimeEdit *editor, TimeProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		TimeProperty *m_property;
	};
};

#endif // TIMEPROPERTY_H
