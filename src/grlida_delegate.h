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
 *
**/

#ifndef GRLIDA_DELEGATE_H
#define GRLIDA_DELEGATE_H

#include <QSqlTableModel>
#include <QItemDelegate>
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QHash>

#include "funciones.h"
#include "dbsql.h"

// INICIO GrlListViewDelegate -----------------------------------------------------------------------------------
class GrlListViewDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	GrlListViewDelegate(dbSql *m_sql, QString dir_app, QString dir_theme, QString dir_theme_app, QString filter, QWidget *parent = 0);
	virtual ~GrlListViewDelegate();

	enum datarole {
		IdGrlRole   = Qt::UserRole +100,
		TituloRole  = Qt::UserRole +101,
		IconRole    = Qt::UserRole +102,
		TipoEmuRole = Qt::UserRole +103,
		RatingRole  = Qt::UserRole +104
	};

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

	void setDirApp(QString dir_app){stDirApp = dir_app;}
	void setTheme(QString dir_theme){stTheme = dir_theme;}
	void setThemeCat(QString dir_theme_app){stThemeApp = dir_theme_app;}
	void setTable(QString m_tabla){stTabla = m_tabla;}
	void setIdCol(QHash<int, int> m_col){col = m_col;}
	void setLwIconCfg(stLwIconCfg m_lwConf);

private:
	Funciones fGrl;
	dbSql *sql;

	QString stDirApp, stTheme, stThemeApp, stTabla, stFilter;
	QPixmap star_on, star_off, title_bg, cover_top, cover_top_select;
	QHash<QString, QPixmap> ico_emu;
	QHash<int, int> col;
	stLwIconCfg lwConf;
};
// FIN GrlListViewDelegate --------------------------------------------------------------------------------------

// INICIO GrlTreeViewModel --------------------------------------------------------------------------------------
class GrlTreeViewModel : public QSqlTableModel
{
	Q_OBJECT

public:
	GrlTreeViewModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase(), QString formato_fecha = "dd/MM/yyyy HH:mm:ss");
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setFormatoFecha(QString formato_fecha){formatofecha = (formato_fecha.isEmpty())?"dd/MM/yyyy HH:mm:ss":formato_fecha;}

private:
	QString formatofecha;

};
// FIN GrlTreeViewModel -----------------------------------------------------------------------------------------

// INICIO GrlTreeViewDelegate -----------------------------------------------------------------------------------
class GrlTreeViewDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	GrlTreeViewDelegate(dbSql *m_sql, QString dir_app, QString dir_theme, QString dir_theme_app, QString filter, QTreeWidget *m_twNav, QWidget *parent = 0);
	virtual ~GrlTreeViewDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

	void setDirApp(QString dir_app){stDirApp = dir_app;}
	void setTheme(QString dir_theme){stTheme = dir_theme;}
	void setThemeCat(QString dir_theme_app){stThemeApp = dir_theme_app;}
	void setTable(QString m_tabla){stTabla = m_tabla;}
	void setIdCol(QHash<int, int> m_col){col = m_col;}
	void setIconSize(QSize m_size){size_icon = m_size;}
	void setIconFav(QString m_fav)
	{
		if( !img_favorito.load(stTheme +"img16/"+ m_fav) )
			img_favorito.load(":/img16/original.png");
	}

private:
	Funciones fGrl;
	dbSql *sql;

	QString stDirApp, stTheme, stThemeApp, stTabla;
	QSize size_icon;
	QPixmap star_on, star_off, sin_img, img_original, img_favorito;
	QHash<QString, QPixmap> icono;
	QHash<QString, QPixmap> edades;
	QHash<int, int> col;
	QTreeWidget *twNav;

};
// FIN GrlTreeViewDelegate --------------------------------------------------------------------------------------

#endif // GRLIDA_DELEGATE_H
