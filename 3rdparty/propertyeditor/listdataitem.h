#ifndef LISTDATAITEM_H
#define LISTDATAITEM_H

#include <QList>
#include <QIcon>
#include <QVariant>

struct ListDataItem
{
	ListDataItem(const QString &text, const QVariant &userData = QVariant()):
		m_icon(QIcon()), m_text(text), m_userData(userData) {}
	ListDataItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant()):
		m_icon(icon), m_text(text), m_userData(userData) {}

	QIcon m_icon;
	QString m_text;
	QVariant m_userData;
};

typedef QList<ListDataItem> ListData;

#endif // LISTDATAITEM_H
