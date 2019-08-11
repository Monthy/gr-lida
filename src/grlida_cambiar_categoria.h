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

#ifndef GRLIDA_CAMBIAR_CATEGORIA_H
#define GRLIDA_CAMBIAR_CATEGORIA_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmCambiarCategoria;
}

class frmCambiarCategoria : public QDialog
{
	Q_OBJECT

public:
	explicit frmCambiarCategoria(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, QWidget *parent = 0);
	~frmCambiarCategoria();

	int getNuevaCategoria(){return id_cat;}

private:
	Ui::frmCambiarCategoria *ui;

	Funciones *fGrl;
	dbSql *sql;

	stGrlCfg grlCfg;
	stGrlCats categoria;
	stDatosJuego DatosJuego;

	int id_cat;
	QString stDirApp, IdGame, IdCat;
	QHash<int, stGrlCats> categorias;

	void cargarConfig();
	void setTheme();
	QStringList actualizaIds(QString tabla, QString cat_id_new, QString game_id_new);

private slots:
	void on_cbxCategorias_activated(int index);
	void on_btnOk_clicked();
	void on_btnCancel_clicked();

};

#endif // GRLIDA_CAMBIAR_CATEGORIA_H
