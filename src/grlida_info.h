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

#ifndef GRLIDA_INFO_H
#define GRLIDA_INFO_H

#include <QDialog>

#include "dbsql.h"
#include "funciones.h"
#include "qtzip.h"

namespace Ui {
	class frmInfo;
}

class frmInfo : public QDialog
{
	Q_OBJECT

public:
	frmInfo(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent = 0);
	~frmInfo();

private:
	Ui::frmInfo *ui;

	Funciones *fGrl;
	dbSql *sql;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	unsigned int id_cat;
	QHash<int, stGrlCats> categoria;
	QTreeWidgetItem *twListInfo;

	void cargarConfig();
	void setTheme();

	void cargarListaCategorias();
	void menuNavAddCat(QString etiqueta, QString icono, QString sql_query = "", bool m_expanded = true, bool m_show_total = true);
	void menuNavAddSubCat(QString etiqueta, QString icono, QString sql_query = "", QString sql_col = "");
	void crearMenuNav();

private slots:
	void on_cbxCategorias_activated(int index);
	void on_btnOk_clicked();

};

#endif // GRLIDA_INFO_H
