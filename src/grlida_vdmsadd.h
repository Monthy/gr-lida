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

#ifndef GRLIDA_VDMSADD_H
#define GRLIDA_VDMSADD_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmVdmsAdd;
}

class frmVdmsAdd : public QDialog
{
    Q_OBJECT

public:
	frmVdmsAdd(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent = 0);
	~frmVdmsAdd();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stDatosJuego getDatosJuegos(){return DatosJuego;}

private:
	Ui::frmVdmsAdd *ui;

	QString titulo_ventana(){ return tr("Nuevo juego para el VDMSound"); }

	Funciones *fGrl;
	dbSql *sql;

	stDatosJuego DatosJuego;
	stConfigVDMSound DatosVDMSound;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	void cargarConfig();
	void setTheme();

	void setDatosVDMSound();
	void cargarDatosVDMSound(stConfigVDMSound cfgVdms);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();

	void on_btnVdms_FileConfg_clicked();
	void on_btnVdms_FileConfg_clear_clicked();
	void on_btnVdms_ExeJuego_clicked();
	void on_btnVdms_ExeJuego_clear_clicked();
	void on_btnVdms_params_clear_clicked();
	void on_btnVdms_Icono_clicked();
	void on_btnVdms_Icono_clear_clicked();
	void on_btnDescargarInfo_clicked();

	void on_txtDatos_Titulo_textEdited(const QString &arg1);

};

#endif // GRLIDA_VDMSADD_H
