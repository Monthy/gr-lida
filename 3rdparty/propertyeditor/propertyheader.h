#ifndef PROPERTYHEADER_H
#define PROPERTYHEADER_H

#include "propertyitem.h"

class PropertyHeader : public PropertyItem
{
public:
	PropertyHeader(const QString &name, const QBrush &bg = Qt::darkGray, const QColor &color = Qt::white);
	PropertyHeader(PropertyItem *top, const QString &name, const QBrush &bg = Qt::darkGray, const QColor &color = Qt::white);

// reimp
	virtual void onAdded();
	virtual QVariant getVariantValue() const { return QVariant(); }

private:
	QBrush m_bg;
	QColor m_color;
};

#endif // PROPERTYHEADER_H
