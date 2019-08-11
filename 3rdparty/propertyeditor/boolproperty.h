#ifndef BOOLPROPERTY_H
#define BOOLPROPERTY_H

#include <QKeyEvent>
#include <QCheckBox>

#include "propertyitem.h"
#include "propertybutton.h"
#include "qbooledit.h"

class BoolProperty : public PropertyItem
{
public:
	BoolProperty(const QString &name, bool value = false, bool defaultValue = false, const QString &textTrue = QObject::tr("True"), const QString &textFalse = QObject::tr("False"));
	BoolProperty(PropertyItem *top, const QString &name, bool value = false, bool defaultValue = false, const QString &textTrue = QObject::tr("True"), const QString &textFalse = QObject::tr("False"));

	void setValue(bool value);
	bool getValue() const;

	void setDefaultValue(bool value) { m_defaultValue = value; }
	bool getDefaultValue() const { return m_defaultValue; }

	void setBoolText(const QString &text);
	void setBoolText(const QString &textTrue, const QString &textFalse);
	void setBgColor(const QColor &color);

// handlers
	virtual bool onKeyPressed(QKeyEvent *event, QWidget */*editWidget*/);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable bool m_value;
	mutable QBoolEdit *m_editor;
	bool m_defaultValue;
	QString m_textTrue, m_textFalse;
	QColor m_bgcolor;

	class ButtonsEditor : public TPropertyButton<QBoolEdit>
	{
	public:
		ButtonsEditor(QBoolEdit *editor, BoolProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		BoolProperty *m_property;
	};
};

#endif // BOOLPROPERTY_H
