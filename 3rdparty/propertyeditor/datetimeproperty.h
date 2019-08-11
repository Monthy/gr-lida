#ifndef DATETIMEPROPERTY_H
#define DATETIMEPROPERTY_H

#include <QDateTimeEdit>

#include "propertyitem.h"
#include "propertybutton.h"

class DateTimeProperty : public PropertyItem
{
public:
	DateTimeProperty(const QString &name, const QDateTime &value = QDateTime::currentDateTime(), const QDateTime &defaultValue = QDateTime::currentDateTime());
	DateTimeProperty(PropertyItem *top, const QString &name, const QDateTime &value = QDateTime::currentDateTime(), const QDateTime &defaultValue = QDateTime::currentDateTime());

	void setDateTime(const QDateTime &value);
	QDateTime getDateTime() const;

	void setDefaultValue(const QDateTime &value) { m_defaultValue = value; }
	QDateTime getDefaultValue() const { return m_defaultValue; }

	void setMaximumDateTime(const QDateTime &value);
	QDateTime getMaximumDateTime() const;
	void setMinimumDateTime(const QDateTime &value);
	QDateTime getMinimumDateTime() const;
	void setDateTimeRange(const QDateTime &min, const QDateTime &max);

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
	mutable QDateTime m_value;
	mutable QDateTimeEdit *m_editor;
	QDateTime m_defaultValue, m_maxDateTime, m_minDateTime;
	QString m_format;

	class ButtonsEditor : public TPropertyButton<QDateTimeEdit>
	{
	public:
		ButtonsEditor(QDateTimeEdit *editor, DateTimeProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		DateTimeProperty *m_property;
	};
};

#endif // DATETIMEPROPERTY_H
