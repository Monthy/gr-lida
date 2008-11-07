#ifndef STARDELEGATE_H
#define STARDELEGATE_H

#include <QtCore>
#include <QtGui>
#include <QItemDelegate>
#include <QSize>

#include "dbsql.h"

class StarDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	StarDelegate(QWidget *parent = 0);
	~StarDelegate();

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
	dbSql *sql;
	QSize mSize;
	QPixmap star_on, star_off;
	int num_column, col_Icono;
};

#endif // STARDELEGATE_H
