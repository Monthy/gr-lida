#ifndef FONTPROPERTY_H
#define FONTPROPERTY_H

#include <QFontComboBox>

#include "propertyitem.h"
#include "propertybutton.h"

class FontProperty : public PropertyItem
{
public:
	FontProperty(const QString &name, const QFont &font = QFont(), const QFont &defaultFont = QFont(),
				const QFontComboBox::FontFilters filters = QFontComboBox::AllFonts,
				const QFontDatabase::WritingSystem writeSystem = QFontDatabase::Any);

	FontProperty(PropertyItem *top,const QString &name, const QFont &font = QFont(), const QFont &defaultFont = QFont(),
				const QFontComboBox::FontFilters filters = QFontComboBox::AllFonts,
				const QFontDatabase::WritingSystem writeSystem = QFontDatabase::Any);

	void setFont(const QFont &font);
	const QFont &getFont() const;

	void setDefaultFont(const QFont &font) { m_defaultFont = font; }
	const QFont &getDefaultFont() const { return m_defaultFont; }

	void allowListFontsOnly(bool on);

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	void init();
	mutable QFont m_font;
	mutable QFontComboBox *m_editor;
	QFont m_defaultFont;
	QFontComboBox::FontFilters m_filters;
	QFontDatabase::WritingSystem m_writeSystem;
	bool m_listFontsOnly;

	class ButtonsEditor : public TPropertyButton<QFontComboBox>
	{
	public:
		ButtonsEditor(QFontComboBox *editor, FontProperty *property);
	protected:
		virtual void onButtonClicked();
		virtual void onButtonDefaultClicked();
		FontProperty *m_property;
	};
};

#endif // FONTPROPERTY_H
