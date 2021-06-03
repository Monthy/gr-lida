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

#ifndef GRLIDA_INSTALAR_JUEGO_H
#define GRLIDA_INSTALAR_JUEGO_H

#include <QDialog>
#include <QCloseEvent>
#include <QProcess>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmInstalarJuego;
}

class frmInstalarJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmInstalarJuego(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent = 0);
	~frmInstalarJuego();

	QList<stConfigDOSBoxMount> getDatosMontajes(){return list_mount;}

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmInstalarJuego *ui;

	Funciones *fGrl;
	dbSql *sql;
	QProcess *grlProcess, *grlProcessMount;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stConfigDOSBoxMount DatosMontaje;
	QList<stConfigDOSBoxMount> list_mount;
	QHash<QString, stGrlDatos> dbx_list;
	QHash<QString, stVirtualDrive> virtual_drive_list;
	QStringList img_floppy, img_iso, img_hdd;
	bool isCloseEvent, isMountImage, isCancel;
	int idx_virtual_drive;

	void cargarConfig();
	void setTheme();

	void ejecutar(const QString &bin, const QString &parametros = "", const QString &dirWorking = "");
	void montaje(QString bin, QString parametros = "", QString working_dir = "");

	void error_msg_Proceso(QProcess::ProcessError error);

	void cargarVirtualDriveList();
	void setConfigDatosMontaje();

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();

// DOSBox
	void on_cbxMontaje_type_drive_activated(int index);
	void on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_btnDirFile_clicked();
	void on_btnDirFile_clear_clicked();
	void on_btnSubirIso_clicked();
	void on_btnBajarIso_clicked();
	void on_btnDeleteIso_clicked();
	void on_btnDestino_clicked();
	void on_btnDestinoClear_clicked();
	void on_btnEjecutarDbx_clicked();

// Montaje ISO, CUE/BIN
	void on_lw_vd_images_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_btn_virtual_drive_edit_clicked();
	void on_btn_vd_image_list_add_clicked();
	void on_btn_vd_image_list_clear_clicked();
	void on_btn_vd_image_list_subir_clicked();
	void on_btn_vd_image_list_bajar_clicked();
	void on_btn_vd_image_list_delete_clicked();
	void on_btn_virtual_drive_montar_clicked();

// Procesos
	void fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus);
	void fin_ProcesoMount(int exitCode, QProcess::ExitStatus exitStatus);
	void error_Proceso(QProcess::ProcessError error);
	void error_ProcesoMount(QProcess::ProcessError error);

};

#endif // GRLIDA_INSTALAR_JUEGO_H
