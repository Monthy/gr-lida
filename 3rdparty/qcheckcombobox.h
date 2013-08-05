/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
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
 * Based on Qxt library and adapted by Monthy for GR-lida
 *
 *
**/

#ifndef QCHECKCOMBOBOX_H
#define QCHECKCOMBOBOX_H

#include <QComboBox>
#include <QListView>
#include <QStandardItemModel>

// INI QCheckComboModel -----------------------------------------------------------------------------------------
class QCheckComboModel : public QStandardItemModel
{
	Q_OBJECT

public:
	explicit QCheckComboModel(QObject *parent = 0);

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

signals:
	void checkStateChanged();

};
// FIN QCheckComboModel -----------------------------------------------------------------------------------------

// INI QCheckComboBox -------------------------------------------------------------------------------------------
class QCheckComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QString separator READ separator WRITE setSeparator)
	Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
	Q_PROPERTY(QStringList checkedItems READ checkedItems WRITE setCheckedItems)

public:
	explicit QCheckComboBox(QWidget *parent = 0);
	virtual ~QCheckComboBox();

	virtual void hidePopup();

	Qt::CheckState itemCheckState(int index) const;
	void setItemCheckState(int index, Qt::CheckState state);

	QString separator() const;
	void setSeparator(const QString &separator);

	QString defaultText() const;
	void setDefaultText(const QString &text);

	QStringList checkedItems() const;
	QString getCheckedItems() const;
	QString getCheckedItemsUserData() const;
	QStringList items() const;

public slots:
	void setCheckedItems(const QStringList &items);
	void setCheckedItems(const QString &items);
	void setCheckedItemsUserData(const QString &items);

signals:
	void checkedItemsChanged(const QStringList &items);

private:
	bool eventFilter(QObject *object, QEvent *event);

	QString item_sep;
	QString default_text;
	bool containerMousePress;

private slots:
	void updateText();
	void updateCheckedItems();
	void toggleCheckState(int index);

};
// FIN QCheckComboBox -------------------------------------------------------------------------------------------

#endif // QCHECKCOMBOBOX_H
