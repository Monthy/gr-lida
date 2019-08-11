#include <QColorDialog>

#include "buttoncolorproperty.h"

ButtonColorProperty::ButtonColorProperty(const QString &name, const QColor &color, const QColor &defaultColor):
	PropertyItem(name),
	m_color(color),
	m_defaultColor(defaultColor)
{
	loadColorsList(QColor::colorNames());
	setColor(m_color);
}

ButtonColorProperty::ButtonColorProperty(PropertyItem *top, const QString &name, const QColor &color, const QColor &defaultColor):
	PropertyItem(top, name),
	m_color(color),
	m_defaultColor(defaultColor)
{
	loadColorsList(QColor::colorNames());
	setColor(m_color);
}

void ButtonColorProperty::loadColorsList(const QStringList &colorNames)
{
	m_listColorNames = colorNames;
	m_listColors.clear();
	for (int i = 0; i < m_listColorNames.size(); i++)
	{
		QColor color(m_listColorNames.at(i));
		m_listColors.insert(i, color);
	}
}

void ButtonColorProperty::setColor(const QColor &color)
{
	m_color = color;
	PropertyItem::setValue();
}

const QColor &ButtonColorProperty::getColor() const
{
	return m_color;
}

QVariant ButtonColorProperty::getVariantValue() const
{
	return m_color;
}

void ButtonColorProperty::validateValue()
{
	if (!m_color.isValid())
		m_color = QColor(Qt::black);
}

void ButtonColorProperty::displayValue()
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
	setIcon(1, QColorButton::colorIcon(m_color));
	setToolTip(1, colorNameExt);

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

QWidget *ButtonColorProperty::createEditor() const
{
	m_editor = new QColorButton();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<ButtonColorProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void ButtonColorProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setCurrentColor(m_color);
		QObject::connect(m_editor, SIGNAL(currentColorChanged(QColor)), treeWidget(), SLOT(onValueChanged(QColor)));
	}
}

void ButtonColorProperty::valueFromEditor()
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
ButtonColorProperty::ButtonsEditor::ButtonsEditor(QColorButton *editor, ButtonColorProperty *property):
	TPropertyButton<QColorButton>(editor)
{
	m_property = property;
}

void ButtonColorProperty::ButtonsEditor::onEditorClicked()
{
	QColorDialog colorDialog(getEditor()->currentColor());

	if (colorDialog.exec() == QDialog::Accepted)
	{
		m_property->setColor(colorDialog.selectedColor());
		m_property->finishEdit();
	}
}

void ButtonColorProperty::ButtonsEditor::onButtonDefaultClicked()
{
	m_property->setColor(m_property->getDefaultColor());
	m_property->finishEdit();
}
