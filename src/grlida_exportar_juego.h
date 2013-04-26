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

#ifndef GRLIDA_EXPORTARJUEGO_H
#define GRLIDA_EXPORTARJUEGO_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmExportarJuego;
}

class frmExportarJuego : public QDialog
{
    Q_OBJECT

public:
	frmExportarJuego(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat, QWidget *parent = 0);
	~frmExportarJuego();

	stGrlCfg getGrlCfg(){return grlCfg;}

private:
	Ui::frmExportarJuego *ui;

	Funciones *fGrl;
	dbSql *sql;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	stDatosJuego DatosJuego;
	stConfigDOSBox DatosDosBox;
	stConfigScummVM DatosScummVM;
	stConfigVDMSound DatosVDMSound;

	unsigned int id_cat;
	QHash<int, stGrlCats> categoria;
	QHash<QString, stGrlDatos> emu_list;
	QString tipo_export;

	QTreeWidget *twMontajes;

	void cargarConfig();
	void setTheme();

	void cargarListaCategorias();
	void cargarListaJuegos(QString sql_where = "");
	void cargarDatosExportar(QString IdGame);

private slots:
	void on_btnOk_clicked();
	void on_btnCancelar_clicked();
	void on_cbxCategorias_activated(int index);
	void on_btnCheckedAll_clicked();
	void on_btnUnCheckedAll_clicked();
	void on_cbxExpotarComo_activated(int index);
	void on_btnDirExportPath_clicked();
	void on_btnDirExportPath_clear_clicked();

	void on_cbxTipoEmu_activated(int index);
};

#endif // GRLIDA_EXPORTARJUEGO_H
