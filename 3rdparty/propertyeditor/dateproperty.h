#ifndef DateProperty_H
#define DateProperty_H

#include <QDateEdit>

#include "propertyitem.h"
#include "propertybutton.h"

class DateProperty : public PropertyItem
{
public:
	DateProperty(const QString &name, const QDate &value = QDate::currentDate(), const QDate &defaultValue = QDate::currentDate());
	DateProperty(PropertyItem *top, const QString &name, const QDate &value = QDate::currentDate(), const QDate &defaultValue = QDate::currentDate());

	void setDate(const QDate &value);
	QDate getDate() const;

	void setDefaultValue(const QDate &value) { m_defaultValue = value; }
	QDate getDefaultValue() const { return m_defaultValue; }

	void setMaximumDate(const QDate &value);
	QDate getMaximumDate() const;
	void setMinimumDate(const QDate &value);
	QDate getMinimumDate() const;
	void setDateRange(const QDate &min, const QDate &max);

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
	mutable QDate m_value;
	mutable QDateEdit *m_editor;
	QDate m_defaultValue, m_maxDate, m_minDate;
	QString m_format;

	class ButtonsEditor : public TPropertyButton<QDateEdit>
	{
	public:
		ButtonsEditor(QDateEdit *editor, DateProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		DateProperty *m_property;
	};
};

#endif // DateProperty_H
