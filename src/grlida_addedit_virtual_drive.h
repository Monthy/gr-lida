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

#ifndef GRLIDA_ADDEDIT_VIRTUAL_DRIVE_H
#define GRLIDA_ADDEDIT_VIRTUAL_DRIVE_H

#include <QDebug>

#include <QDialog>
#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmAddEditVirtualDrive; 
}

class frmAddEditVirtualDrive : public QDialog
{
	Q_OBJECT

public:
	frmAddEditVirtualDrive(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent = 0);
	~frmAddEditVirtualDrive();

	stGrlCfg getGrlCfg() { return grlCfg; }

private:
	Ui::frmAddEditVirtualDrive *ui;

	Funciones *fGrl;
	dbSql *sql;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	QRegExp regexp;

	QHash<QString, stVirtualDrive> virtual_drive_list, virtual_drive_list_tpl;
	QString virtual_drive_etiqueta_old, tpl_info_old;

	void cargarConfig();
	void setTheme();
	void setDatosVirtualDrive(stVirtualDrive virtualDrive);
	void setDatosClear();
	void actualizaDatosVirtualDrive(QString old_etiqueta, QString new_etiqueta);
	void guardarDatosVirtualDrive();
	void addEditTwVirtualDrive(bool isAdd);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();

	void on_cbxVd_plantilla_cfg_activated(int index);
	void on_txtVd_titulo_textEdited(const QString &arg1);

	void on_btnVd_path_exe_clicked();
	void on_btnVd_path_exe_clear_clicked();
	void on_btnVd_info_clicked();

	void on_btnVdNew_clicked();
	void on_btnVdEdit_clicked();
	void on_btnVdAdd_clicked();
	void on_btnVdUpdate_clicked();
	void on_btnVdDelete_clicked();
	void on_btnVdCancel_clicked();

	void on_twVirtualDrive_itemClicked(QTreeWidgetItem *item, int column);
	void on_twVirtualDrive_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);


};

#endif // GRLIDA_ADDEDIT_VIRTUAL_DRIVE_H
