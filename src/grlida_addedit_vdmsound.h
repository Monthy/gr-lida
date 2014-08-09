/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_ADDEDIT_VDMSOUND_H
#define GRLIDA_ADDEDIT_VDMSOUND_H

#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmAddEditVDMSound;
}

class frmAddEditVDMSound : public QTabWidget
{
	Q_OBJECT

public:
	explicit frmAddEditVDMSound(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent = 0);
	~frmAddEditVDMSound();

	bool isCorrectNext();
	void cargarConfig();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stConfigVDMSound getDatosVDMSound(){return DatosVDMSound;}
	QString getPathConf();
	void setPathConf(QString config);

	void setDatosVDMSound();
	void cargarDatosVDMSound(stConfigVDMSound cfgVdms);

private:
	Ui::frmAddEditVDMSound *ui;

	Funciones *fGrl;
	dbSql *sql;

	QString titulo_ventana(){ return windowTitle(); }

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stConfigVDMSound DatosVDMSound;

	QString IdGame, IdVdms;

	bool Editando;

	void setTheme();

private slots:
	void on_btnVdms_FileConfg_clicked();
	void on_btnVdms_FileConfg_clear_clicked();
	void on_btnVdms_ExeJuego_clicked();
	void on_btnVdms_ExeJuego_clear_clicked();
	void on_btnVdms_params_clear_clicked();
	void on_btnVdms_Icono_clicked();
	void on_btnVdms_Icono_clear_clicked();

};

#endif // GRLIDA_ADDEDIT_VDMSOUND_H
