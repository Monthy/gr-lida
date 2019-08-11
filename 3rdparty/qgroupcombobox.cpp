/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Based on the stackoverflow example:
 *     https://stackoverflow.com/questions/33012292/grouped-qcombobox
 * and adapted by Monthy for GR-lida
 *
 *
**/

#include "qgroupcombobox.h"
#include <QPainter>

// INI QGroupComboDelegate --------------------------------------------------------------------------------------
QGroupComboDelegate::QGroupComboDelegate(QObject *parent)
	:QItemDelegate(parent)
{
	//
}

void QGroupComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int type = index.data(Qt::AccessibleDescriptionRole).toInt();

	if(type == gCbxSeparate)
	{
		QItemDelegate::paint(painter, option, index);
		int y = (option.rect.top() + option.rect.bottom()) / 2;
		painter->setPen(option.palette.color(QPalette::Active, QPalette::Dark));
		painter->drawLine(option.rect.left() + 2, y, option.rect.right() - 2, y);
	}
	else if(type == gCbxParent)
	{
		QStyleOptionViewItem parentOption = option;
		parentOption.state |= QStyle::State_Enabled;
		QItemDelegate::paint(painter, parentOption, index);
	}
	else if (type == gCbxChild)
	{
		QStyleOptionViewItem childOption = option;
		int indent = option.fontMetrics.width(QString(4, QChar(' ')));
		childOption.rect.adjust(indent, 0, 0, 0);
		childOption.textElideMode = Qt::ElideNone;
		QItemDelegate::paint(painter, childOption, index);
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}

QSize QGroupComboDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int type = index.data(Qt::AccessibleDescriptionRole).toInt();
	if(type == gCbxSeparate)
		return QSize(0, 10);
	return QItemDelegate::sizeHint(option, index);
}
// FIN QGroupComboDelegate --------------------------------------------------------------------------------------

// INI QGroupComboBox -------------------------------------------------------------------------------------------
QGroupComboBox::QGroupComboBox(QWidget *parent) : QComboBox(parent)
{
	cbx_model = new QStandardItemModel(parent);

	setModel(cbx_model);
	setItemDelegate(new QGroupComboDelegate(this));
}

QGroupComboBox::~QGroupComboBox()
{
	//
}

void QGroupComboBox::addStandardItem(int type, const QString &text, const QVariant &userData, const QIcon &icon)
{
	QStandardItem *item = new QStandardItem();

	if (type == gCbxParent || type == gCbxChild)
		item->setText(text);

	if (type == gCbxParent || type == gCbxSeparate)
		item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));

	if (!userData.isNull())
		item->setData(userData, Qt::UserRole);

	if (!icon.isNull())
		item->setIcon(icon);

	item->setData(type, Qt::AccessibleDescriptionRole);

	if (type == gCbxParent)
	{
		QFont font = item->font();
		font.setBold( true );
		font.setItalic(true);
		item->setFont(font);
	}

	cbx_model->appendRow(item);
}

void QGroupComboBox::addItemParent(const QString &text)
{
	addStandardItem(gCbxParent, text);
}

void QGroupComboBox::addItemParent(const QIcon &icon, const QString &text)
{
	addStandardItem(gCbxParent, text, QVariant(), icon);
}

void QGroupComboBox::addItemChild(const QString &text, const QVariant &userData)
{
	addStandardItem(gCbxChild, text, userData);
}

void QGroupComboBox::addItemChild(const QIcon &icon, const QString &text, const QVariant &userData)
{
	addStandardItem(gCbxChild, text, userData, icon);
}

void QGroupComboBox::addItemSeparator()
{
	addStandardItem(gCbxSeparate);
}
// FIN QGroupComboBox -------------------------------------------------------------------------------------------
