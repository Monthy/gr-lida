#ifndef BUTTONCOLORPROPERTY_H
#define BUTTONCOLORPROPERTY_H

#include "propertyitem.h"
#include "propertybutton.h"
#include "qcolorbutton.h"

class ButtonColorProperty : public PropertyItem
{
public:
	ButtonColorProperty(const QString &name, const QColor &color = QColor(), const QColor &defaultColor = QColor());
	ButtonColorProperty(PropertyItem *top, const QString &name, const QColor &color = QColor(), const QColor &defaultColor = QColor());

	void setColor(const QColor &color);
	const QColor &getColor() const;

	void setDefaultColor(const QColor &color) { m_defaultColor = color; }
	const QColor &getDefaultColor() const { return m_defaultColor; }

// reimp
	virtual QVariant getVariantValue() const;
	virtual void validateValue();
	virtual void displayValue();

	virtual QWidget* createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable QColor m_color;
	mutable QColorButton *m_editor;
	QColor m_defaultColor;
	QStringList m_listColorNames;
	QList<QColor> m_listColors;

	class ButtonsEditor : public TPropertyButton<QColorButton>
	{
	public:
		ButtonsEditor(QColorButton *editor, ButtonColorProperty *property);
	protected:
		virtual void onButtonDefaultClicked();
		virtual void onEditorClicked();
		ButtonColorProperty *m_property;
	};

private:
	void loadColorsList(const QStringList &colorNames);
};

#endif // BUTTONCOLORPROPERTY_H
