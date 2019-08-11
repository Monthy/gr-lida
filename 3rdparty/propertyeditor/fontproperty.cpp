#include <QFontDialog>

#include "fontproperty.h"

FontProperty::FontProperty(const QString &name, const QFont &font, const QFont &defaultFont, 
	const QFontComboBox::FontFilters filters, const QFontDatabase::WritingSystem writeSystem):
	PropertyItem(name),
	m_font(font),
	m_defaultFont(defaultFont),
	m_filters(filters),
	m_writeSystem(writeSystem),
	m_listFontsOnly(false)
{
	init();
}

FontProperty::FontProperty(PropertyItem *top, const QString &name, const QFont &font, const QFont &defaultFont,
	const QFontComboBox::FontFilters filters, const QFontDatabase::WritingSystem writeSystem):
	PropertyItem(top, name),
	m_font(font),
	m_defaultFont(defaultFont),
	m_filters(filters),
	m_writeSystem(writeSystem),
	m_listFontsOnly(false)
{
	init();
}

void FontProperty::init()
{
	setFont(m_font);
}

void FontProperty::setFont(const QFont &font)
{
	m_font = font;
	PropertyItem::setValue();
}

const QFont &FontProperty::getFont() const
{
	return m_font;
}

void FontProperty::allowListFontsOnly(bool on)
{
	m_listFontsOnly = on;
}

QVariant FontProperty::getVariantValue() const
{
	return getFont();
}

void FontProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	QString fontString = QString("%1, %2pt").arg(m_font.family()).arg(m_font.pointSizeF());

	if (m_font.bold()) fontString += ", bold";
	if (m_font.italic()) fontString += ", italic";
	if (m_font.underline()) fontString += ", underline";

	setText(1, fontString);
	setToolTip(1, fontString);

	QFont showFont(m_font);
	showFont.setPointSize(font(0).pointSize());
	QTreeWidgetItem::setFont(1, showFont);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

QWidget *FontProperty::createEditor() const
{
	m_editor = new QFontComboBox();
	m_editor->setWritingSystem(m_writeSystem);
	m_editor->setFontFilters(m_filters);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<FontProperty *>(this));
	hostEditor->setBtnVisible(!m_listFontsOnly);
	return hostEditor;
}

void FontProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setCurrentFont(m_font);
		QObject::connect(m_editor, SIGNAL(currentFontChanged(QFont)), treeWidget(), SLOT(onValueChanged(QFont)));
	}
}

void FontProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		if (m_editor->currentFont() != m_font)
			setFont(m_editor->currentFont());
	}
}

// ButtonsEditor --
FontProperty::ButtonsEditor::ButtonsEditor(QFontComboBox *editor, FontProperty *property):
	TPropertyButton<QFontComboBox>(editor)
{
	m_property = property;
}

void FontProperty::ButtonsEditor::onButtonClicked()
{
	QFontDialog fontDialog(getEditor()->currentFont());

	if (fontDialog.exec() == QDialog::Accepted)
	{
		getEditor()->setCurrentFont(fontDialog.currentFont());
		m_property->finishEdit();
	}
}

void FontProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setCurrentFont(m_property->getDefaultFont());
	m_property->finishEdit();
}
