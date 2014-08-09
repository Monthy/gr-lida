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

#ifndef GRLIDA_INSTALAR_JUEGO_H
#define GRLIDA_INSTALAR_JUEGO_H

#include <QDialog>

#include "funciones.h"

namespace Ui {
	class frmInstalarJuego;
}

class frmInstalarJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmInstalarJuego(stGrlCfg m_cfg, QWidget *parent = 0);
	~frmInstalarJuego();

private:
	Ui::frmInstalarJuego *ui;

	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	void cargarConfig();
	void setTheme();
	void ejecutar(const QString &bin, const QString &parametros = "", const QString &dirWorking = "");

private slots:
	void on_cbxMontaje_type_drive_activated(int index);
	void on_btnDirFile_clicked();
	void on_btnClearDirFile_clicked();
	void on_btnSubirIso_clicked();
	void on_btnBajarIso_clicked();
	void on_btnDeleteIso_clicked();
	void on_btnDestino_clicked();
	void on_btnDestinoClear_clicked();
	void on_btnCancel_clicked();
	void on_btnOk_clicked();
	void on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

};

#endif // GRLIDA_INSTALAR_JUEGO_H
