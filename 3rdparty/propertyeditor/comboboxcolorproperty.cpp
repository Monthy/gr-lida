#include <QColorDialog>
#include <QLineEdit>

#include "comboboxcolorproperty.h"

ComboBoxColorProperty::ComboBoxColorProperty(const QString &name, const QColor &color, const QColor &defaultColor):
	PropertyItem(name),
	m_color(color),
	m_defaultColor(defaultColor),
	m_listColorsOnly(false)
{
	loadColorsList(QColor::colorNames());
	setColor(m_color);
}

ComboBoxColorProperty::ComboBoxColorProperty(PropertyItem *top, const QString &name, const QColor &color, const QColor &defaultColor):
	PropertyItem(top, name),
	m_color(color),
	m_defaultColor(defaultColor),
	m_listColorsOnly(false)
{
	loadColorsList(QColor::colorNames());
	setColor(m_color);
}

void ComboBoxColorProperty::loadColorsList(const QStringList &colorNames)
{
	m_listColorNames = colorNames;
	m_listColors.clear();
	for (int i = 0; i < m_listColorNames.size(); i++)
	{
		QColor color(m_listColorNames.at(i));
		m_listColors.insert(i, color);
	}
}

void ComboBoxColorProperty::setColor(const QColor &color)
{
	m_color = color;
	PropertyItem::setValue();
}

const QColor &ComboBoxColorProperty::getColor() const
{
	return m_color;
}

void ComboBoxColorProperty::setColorsList(const QStringList &colorNames)
{
	loadColorsList(colorNames);
	PropertyItem::setValue();
}

void ComboBoxColorProperty::allowListColorsOnly(bool on)
{
	m_listColorsOnly = on;
	PropertyItem::setValue();
}

QVariant ComboBoxColorProperty::getVariantValue() const
{
	return m_color;
}

void ComboBoxColorProperty::validateValue()
{
	if (!m_color.isValid())
		m_color = QColor(Qt::black);

	if (m_listColorsOnly && m_listColorNames.size())
	{
		if (m_listColors.indexOf(m_color) < 0)
			m_color = QColor(m_listColors.at(0));
	}
}

void ComboBoxColorProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	QString colorNameExt = QString("HEX: %1\nRGB: %2,%3,%4").arg(m_color.name()).arg(m_color.red()).arg(m_color.green()).arg(m_color.blue());

	if (m_listColorNames.isEmpty())
		setText(1, m_color.name());
	else {
		int index = m_listColors.indexOf(m_color);
		if(index > -1)
			setText(1, m_listColorNames.at(index));
		else
			setText(1, m_color.name());
	}
	setIcon(1, QColorComboBox::colorIcon(m_color));
	setToolTip(1, colorNameExt);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

QWidget *ComboBoxColorProperty::createEditor() const
{
	m_editor = new QColorComboBox();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<ComboBoxColorProperty *>(this));
	hostEditor->setBtnVisible(!m_listColorsOnly);
	return hostEditor;
}

void ComboBoxColorProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->allowListColorsOnly(m_listColorsOnly);
		m_editor->setCurrentColor(m_color);
		m_editor->lineEdit()->selectAll();
		QObject::connect(m_editor, SIGNAL(currentColorChanged(QColor)), treeWidget(), SLOT(onValueChanged(QColor)));
	}
}

void ComboBoxColorProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		QColor color = m_editor->currentColor();
		if (color.isValid() && color != m_color)
			setColor(color);
	}
}

// ButtonsEditor --
ComboBoxColorProperty::ButtonsEditor::ButtonsEditor(QColorComboBox *editor, ComboBoxColorProperty *property):
	TPropertyButton<QColorComboBox>(editor)
{
	m_property = property;
}

void ComboBoxColorProperty::ButtonsEditor::onButtonClicked()
{
	QColorDialog colorDialog(getEditor()->currentColor());
	colorDialog.setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);

	if (colorDialog.exec() == QDialog::Accepted)
	{
		m_property->setColor(colorDialog.selectedColor());
		m_property->finishEdit();
	}
}

void ComboBoxColorProperty::ButtonsEditor::onButtonDefaultClicked()
{
	m_property->setColor(m_property->getDefaultColor());
	m_property->finishEdit();
}
