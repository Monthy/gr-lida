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
 *
**/

#ifndef GRLIDA_DELEGATE_H
#define GRLIDA_DELEGATE_H

#include <QApplication>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDateTime>
#include <QMouseEvent>

#include "funciones.h"

// INICIO GrlListViewDelegate -----------------------------------------------------------------------------------
class GrlListViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	GrlListViewDelegate(QWidget *parent = 0);
	virtual ~GrlListViewDelegate();

	void setdirApp(QString dir_app){stDirApp = dir_app;}
	void setTheme(stLwIconCfg m_lwConf, QString dir_theme);
	void setShowLinesPos(bool showLinesPos) { m_showLinesPos = showLinesPos; }

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
	void updateCol(const QModelIndex &index, int col);

private:
	Funciones *fGrl;

	QString stDirApp;
	QPixmap star_on, star_off;
	QHash<QString, QPixmap> ico_emu;
	stLwIconCfg lwConf;
	QPen pen;
	bool m_showLinesPos;

	void paintShadow(QPainter *painter, QPen m_pen, int x, int y, int w, int h, int a = 25)  const;

};
// FIN GrlListViewDelegate --------------------------------------------------------------------------------------

// INICIO GrlTreeViewModel --------------------------------------------------------------------------------------
class GrlTreeViewModel : public QStandardItemModel
{
	Q_OBJECT

public:
	explicit GrlTreeViewModel(QHash<QString, QString> list_emu, QObject *parent = 0, QString formato_fecha = "dd/MM/yyyy HH:mm:ss");
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setFormatoFecha(QString formato_fecha);

private:
	QString formatofecha;
	QHash<QString, QString> emu_list;

};
// FIN GrlTreeViewModel -----------------------------------------------------------------------------------------

// INICIO GrlTreeViewDelegate -----------------------------------------------------------------------------------
class GrlTreeViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	GrlTreeViewDelegate(QWidget *parent = 0);
	virtual ~GrlTreeViewDelegate();

	void setdirApp(QString dir_app){stDirApp = dir_app;}
	void setTheme(QSize m_size, QString m_fav, QString dir_theme, QString filter);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
	void updateCol(const QModelIndex &index, int col);

private:
	Funciones *fGrl;

	QString stDirApp;
	QSize size_icon;
	QPixmap star_on, star_off, sin_img, img_original, img_favorito;
	QHash<QString, QPixmap> icono;
	QHash<QString, QPixmap> edades;

};
// FIN GrlTreeViewDelegate --------------------------------------------------------------------------------------

#endif // GRLIDA_DELEGATE_H
