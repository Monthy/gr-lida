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
 * Based on Qxt library and adapted by Monthy for GR-lida
 *
 *
**/

#include "qcheckcombobox.h"
#include <QLineEdit>
#include <QKeyEvent>

// INI QCheckComboModel -----------------------------------------------------------------------------------------
QCheckComboModel::QCheckComboModel(QObject *parent) : QStandardItemModel(0, 1, parent)
{

}

Qt::ItemFlags QCheckComboModel::flags(const QModelIndex &index) const
{
	return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant QCheckComboModel::data(const QModelIndex &index, int role) const
{
	QVariant value = QStandardItemModel::data(index, role);
	if (index.isValid() && role == Qt::CheckStateRole && !value.isValid())
		value = Qt::Unchecked;
	return value;
}

bool QCheckComboModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool ok = QStandardItemModel::setData(index, value, role);
	if (ok && role == Qt::CheckStateRole)
	{
		emit dataChanged(index, index);
		emit checkStateChanged();
	}
	return ok;
}
// FIN QQCheckComboModel -----------------------------------------------------------------------------------------

// INI QCheckComboBox -------------------------------------------------------------------------------------------
QCheckComboBox::QCheckComboBox(QWidget *parent) : QComboBox(parent)
{
	setModel(new QCheckComboModel(this));

	connect(this, SIGNAL(activated(int)), this, SLOT(toggleCheckState(int)));
	connect(model(), SIGNAL(checkStateChanged()), this, SLOT(updateText()));
//	connect(model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateText()));
//	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateText()));

	QLineEdit *lineEdit = new QLineEdit(this);
	setLineEdit(lineEdit);
	lineEdit->disconnect(this);
	setInsertPolicy(QComboBox::NoInsert);

	item_sep     = QLatin1String(";");
	default_text = "";
	containerMousePress = false;

	connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(updateCheckedItems()));
	connect(lineEdit, SIGNAL(textEdited(QString)), this, SIGNAL(textChanged(QString)));

	view()->installEventFilter(this);
	view()->window()->installEventFilter(this);
	view()->viewport()->installEventFilter(this);
	installEventFilter(this);
}

QCheckComboBox::~QCheckComboBox()
{
	//
}

bool QCheckComboBox::eventFilter(QObject *object, QEvent *event)
{
	switch (event->type())
	{
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if (object == this && (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down))
			{
				showPopup();
				return true;
			}
			else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Escape)
			{
				// it is important to call QComboBox implementation
				this->QComboBox::hidePopup();
				if (keyEvent->key() != Qt::Key_Escape)
					return true;
			}
		}
		break;
		case QEvent::MouseButtonPress:
			containerMousePress = (object == view()->window());
		break;
		case QEvent::MouseButtonRelease:
			containerMousePress = false;
		break;
		default:
		break;
	}
	return false;
}

void QCheckComboBox::updateText()
{
	QStringList items = checkedItems();
	QString text = "";
	if (items.isEmpty())
		text = default_text;
	else
		text = items.join(item_sep);

	setEditText(text);

	emit checkedItemsChanged(items);
}

void QCheckComboBox::updateCheckedItems()
{
	const QStringList items = lineEdit()->text().split(item_sep, QString::SkipEmptyParts);
	const int listItemSize = items.size();
	for (int i = 0; i < listItemSize; ++i)
	{
		int index = findText(items.at(i));
		if (index == -1)
			addItem(items.at(i));
	}
	setCheckedItems(items);
}

void QCheckComboBox::toggleCheckState(int index)
{
	QVariant value = itemData(index, Qt::CheckStateRole);
	if (value.isValid())
	{
		Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
		setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
		emit textChanged(lineEdit()->text());
	}
}

void QCheckComboBox::hidePopup()
{
	if (containerMousePress)
		this->QComboBox::hidePopup();
}

Qt::CheckState QCheckComboBox::itemCheckState(int index) const
{
	return static_cast<Qt::CheckState>(itemData(index, Qt::CheckStateRole).toInt());
}

void QCheckComboBox::setItemCheckState(int index, Qt::CheckState state)
{
	setItemData(index, state, Qt::CheckStateRole);
}

QStringList QCheckComboBox::checkedItems() const
{
	QStringList items;
	if (model())
	{
		QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
		QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
		const int listIndexesSize = indexes.size();
		for (int i = 0; i < listIndexesSize; ++i)
			items += indexes.at(i).data().toString();
	}
	return items;
}

QString QCheckComboBox::getCheckedItems() const
{
	return ""+ checkedItems().join(item_sep);
}

QString QCheckComboBox::getCheckedItemsUserData() const
{
	QStringList items;
	if (model())
	{
		QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
		QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
		const int listIndexesSize = indexes.size();
		for (int i = 0; i < listIndexesSize; ++i)
			items += itemData(indexes.at(i).row()).toString();
	}

	return ""+ items.join(item_sep);
}

QStringList QCheckComboBox::items() const
{
	QStringList items;
	if (model())
	{
		const int row_count = model()->rowCount();
		for (int i = 0; i < row_count; ++i)
		{
			QModelIndex idx = model()->index(i, modelColumn());
			items += idx.data().toString();
		}
	}
	return items;
}

void QCheckComboBox::emitTextChanged(const QString &text)
{
	emit textChanged(text);
}

void QCheckComboBox::setCheckedItems(const QStringList &items)
{
	const int row_count = model()->rowCount();
	for (int i = 0; i < row_count; ++i)
		setItemCheckState(i, items.contains(itemText(i)) ? Qt::Checked : Qt::Unchecked);
}

void QCheckComboBox::setCheckedItems(const QString &items)
{
	const QStringList items_list = items.split(item_sep, QString::SkipEmptyParts);
	const int listItemsSize = items_list.size();
	for (int i = 0; i < listItemsSize; ++i)
	{
		int index = findText(items_list.at(i));
		if (index == -1)
			addItem(items_list.at(i));
	}
	setCheckedItems(items_list);
}

void QCheckComboBox::setCheckedItemsUserData(const QString &items)
{
	const QStringList items_list = items.split(item_sep, QString::SkipEmptyParts);
	const int listItemsSize = items_list.size();
	for (int i = 0; i < listItemsSize; ++i)
	{
		int index = findData(items_list.at(i));
		if (index == -1)
			addItem(items_list.at(i));
	}
	const int row_count = model()->rowCount();
	for (int i = 0; i < row_count; ++i)
		setItemCheckState(i, items_list.contains(itemData(i).toString()) ? Qt::Checked : Qt::Unchecked);
}

QString QCheckComboBox::separator() const
{
	return item_sep;
}

void QCheckComboBox::setSeparator(const QString &sep)
{
	if (item_sep != sep)
	{
		item_sep = sep;
		updateText();
	}
}

QString QCheckComboBox::defaultText() const
{
	return default_text;
}

void QCheckComboBox::setDefaultText(const QString &text)
{
	if (default_text != text)
	{
		default_text = text;
		updateText();
	}
}
// FIN QCheckComboBox -------------------------------------------------------------------------------------------
