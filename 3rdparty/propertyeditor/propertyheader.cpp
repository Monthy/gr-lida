#include "propertyheader.h"

PropertyHeader::PropertyHeader(const QString &name, const QBrush &bg, const QColor &color):
	PropertyItem(name),
	m_bg(bg),
	m_color(color)
{
}

PropertyHeader::PropertyHeader(PropertyItem *top, const QString &name, const QBrush &bg, const QColor &color):
	PropertyItem(top, name),
	m_bg(bg),
	m_color(color)
{
}

void PropertyHeader::onAdded()
{
	setBackground(m_bg);
	setTextColor(m_color);

	QFont font = this->font(0);
	font.setBold(true);

	setFont(0, font);
// important to call this AFTER adding
	setFirstColumnSpanned(true);
}
