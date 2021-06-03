/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2021 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRDAP_ACERCAD_H
#define GRDAP_ACERCAD_H

#include <QDialog>
#include <QDebug>

#include "funciones.h"

namespace Ui {
	class frmCompatibilidadExe;

}

class frmCompatibilidadExe : public QDialog
{
	Q_OBJECT

public:
	explicit frmCompatibilidadExe(QString m_path_exe, QString m_cfg_compatibilidad, QWidget *parent = 0);
	~frmCompatibilidadExe();

	QString getCfgCompatibilidad() { return cfg_compatibilidad; }

private:
	Ui::frmCompatibilidadExe *ui;

	Funciones *fGrl;
	QString path_exe, cfg_compatibilidad;

	void cargarConfig();
	void setTheme();

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();

	void on_chkDat_compatibilidad_color_reducido_clicked(bool checked);
};

#endif // GRDAP_ACERCAD_H
