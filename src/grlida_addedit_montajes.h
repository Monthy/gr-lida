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

#ifndef GRLIDA_ADDEDIT_MONTAJES_H
#define GRLIDA_ADDEDIT_MONTAJES_H

#include <QDialog>

#include "funciones.h"

namespace Ui {
    class frmAddEditMontajes;
}

class frmAddEditMontajes : public QDialog
{
	Q_OBJECT

public:
	frmAddEditMontajes(stGrlCfg m_cfg, bool m_editando, QWidget *parent = 0);
	~frmAddEditMontajes();

	stGrlCfg getGrlCfg(){return grlCfg;}
	void setDatosMontaje(stConfigDOSBoxMount montaje);
	stConfigDOSBoxMount getDatosMontaje(){ return DatosMontaje;}

private:
	Ui::frmAddEditMontajes *ui;

	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stConfigDOSBoxMount DatosMontaje;

	bool Editando;
	QStringList list_letter;

	void cargarConfig();
	void setTheme();

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_cbxMontaje_type_drive_activated(int index);
	void on_btnDirFile_clicked();
	void on_btnDirFile_clear_clicked();
	void on_btnSubirIso_clicked();
	void on_btnBajarIso_clicked();
	void on_btnDeleteIso_clicked();
	void on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_chkMontaje_opt_freesize_clicked(bool checked);
	void on_h_SliderMontaje_freesize_valueChanged(int value);

	void on_btnMontaje_opt_size_mount_clear_clicked();

};

#endif // GRLIDA_ADDEDIT_MONTAJES_H
