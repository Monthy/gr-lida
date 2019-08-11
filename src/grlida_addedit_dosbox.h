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

#ifndef GRLIDA_ADDEDIT_DOSBOX_H
#define GRLIDA_ADDEDIT_DOSBOX_H

#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmAddEditDosBox;
}

class frmAddEditDosBox : public QTabWidget
{
	Q_OBJECT

public:
	explicit frmAddEditDosBox(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent = 0);
	~frmAddEditDosBox();

	bool isCorrectNext();
	void cargarConfig();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stConfigDOSBox getDatosDosBox(){return DatosDosBox;}
	QList<stConfigDOSBoxMount> getListMount();

	void setDatosDosBox();
	void cargarDatosDosBox(stConfigDOSBox cfgDbx);
	void cargarDatosDBxMontaje(QList<stConfigDOSBoxMount> listMount);

private:
	Ui::frmAddEditDosBox *ui;

	Funciones *fGrl;
	dbSql *sql;

	QString titulo_ventana(){ return windowTitle(); }

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;
	stConfigDOSBox DatosDosBox;

	QString IdGame, IdDbx;
	QString dos_keyboardlayout;
	QHash<QString, stGrlDatos> dbx_list;
	bool Editando, isDbxSVN;

	void setTheme();
	void previerMontajes();
	void cargarDatosDBxMontaje(QString IDdbx);
	QString setOpcionesSerial();
	void addMontajeAutoexec(QString tipo, QString dir_file);

private slots:
	void on_cbxDbx_EmuKey_activated(int index);
	void on_cbxDbx_Profiles_activated(int index);
	void on_chkDbx_loadfix_clicked();
	void on_spinDbx_loadfix_mem_valueChanged(int arg1);
	void on_cbxDbx_dos_keyboardlayout_activated(int index);
	void on_btnDbx_ExeJuego_clicked();
	void on_btnDbx_ExeJuego_clear_clicked();
	void on_btnDbx_ExeSetup_clicked();
	void on_btnDbx_ExeSetup_clear_clicked();
	void on_btnInstalarJuego_clicked();
	void on_btnMount_Add_clicked();
	void on_btnMount_Edit_clicked();
	void on_btnMount_Delete_clicked();
	void on_btnMount_Clear_clicked();
	void on_btnMount_Subir_clicked();
	void on_btnMount_Bajar_clicked();
	void on_btnMount_AutoCrear_clicked();
	void on_btnMount_Primario_clicked();
	void on_btnDbx_AddSerial_clicked();
	void on_btnToolDbx_mount_HD_clicked();
	void on_btnToolDbx_mount_CD_clicked();
	void on_btnToolDbx_mount_Floppy_clicked();
	void on_btnToolDbx_mount_ImgFloppy_clicked();
	void on_btnToolDbx_mount_ImgCD_clicked();
	void on_btnToolDbx_mount_ImgHD_clicked();
	void on_btnDbx_mapperfile_clicked();
	void on_btnDbx_mapperfile_clear_clicked();
	void on_btnDbx_language_clicked();
	void on_btnDbx_language_clear_clicked();

};

#endif // GRLIDA_ADDEDIT_DOSBOX_H
