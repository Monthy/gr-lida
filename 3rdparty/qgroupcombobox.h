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

#ifndef QGROUPCOMBOBOX_H
#define QGROUPCOMBOBOX_H

#include <QComboBox>
#include <QItemDelegate>
#include <QStandardItemModel>

enum GroupComboBoxRole {
	gCbxParent   = 0,
	gCbxChild    = 1,
	gCbxSeparate = 2
};

// INI QGroupComboDelegate --------------------------------------------------------------------------------------
class QGroupComboDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit QGroupComboDelegate(QObject *parent = 0);

protected:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};
// FIN QGroupComboDelegate --------------------------------------------------------------------------------------

// INI QGroupComboBox -------------------------------------------------------------------------------------------
class QGroupComboBox : public QComboBox
{
	Q_OBJECT

public:
	explicit QGroupComboBox(QWidget *parent = 0);
	virtual ~QGroupComboBox();

	void addItemParent(const QString &text);
	void addItemParent(const QIcon &icon, const QString &text);

	void addItemChild(const QString &text, const QVariant &userData = QVariant());
	void addItemChild(const QIcon &icon, const QString &text, const QVariant &userData = QVariant());

	void addItemSeparator();

private:
	QStandardItemModel *cbx_model;

	void addStandardItem(int type, const QString &text = "", const QVariant &userData = QVariant(), const QIcon &icon = QIcon());


};
// FIN QGroupComboBox -------------------------------------------------------------------------------------------

#endif // QGROUPCOMBOBOX_H
