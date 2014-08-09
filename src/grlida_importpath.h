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

#ifndef GRLIDA_IMPORTPATH_H
#define GRLIDA_IMPORTPATH_H

#include <QDialog>

#include "funciones.h"
#include "ui_importpath.h"

namespace Ui {
	class frmImportPath;
}

class frmImportPath : public QDialog
{
	Q_OBJECT

public:
	explicit frmImportPath(stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent = 0);
	~frmImportPath();

	stGrlCfg getGrlCfg(){return grlCfg;}

	Ui::frmImportPath *ui;
private:
	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	void setTheme();

public slots:
	void statusBtnEnabled(bool estado);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
//-- Datos
	void on_twDatosJuego_itemClicked(QTreeWidgetItem *item, int column);
	void on_chk_selectDatos_clicked(bool checked);
//-- DOSBox
	void on_btnDbx_FileConfg_clicked();
	void on_btnDbx_ExeJuego_clicked();
	void on_btnDbx_FileConfg_clear_clicked();
	void on_btnDbx_ExeJuego_clear_clicked();
	void on_btnDbx_ExeSetup_clicked();
	void on_btnDbx_ExeSetup_clear_clicked();
	void on_btnDbx_language_clicked();
	void on_btnDbx_language_clear_clicked();
	void on_btnDbx_mapperfile_clicked();
	void on_btnDbx_mapperfile_clear_clicked();
	void on_btnDbx_gus_ultradir_clear_clicked();
	void on_btnDbx_capturas_clicked();
	void on_btnDbx_capturas_clear_clicked();
	void on_btnDbx_musica_clicked();
	void on_btnDbx_musica_clear_clicked();
//-- ScummVM
	void on_btnSvm_PathGame_clicked();
	void on_btnSvm_PathGame_clear_clicked();
	void on_btnSvm_PathSave_clicked();
	void on_btnSvm_PathSave_clear_clicked();
	void on_btnSvm_PathExtra_clicked();
	void on_btnSvm_PathExtra_clear_clicked();
	void on_btnSvm_PathCapturas_clicked();
	void on_btnSvm_PathCapturas_clear_clicked();
	void on_btnSvm_PathSonido_clicked();
	void on_btnSvm_PathSonido_clear_clicked();
//-- VDMSound
	void on_btnVdms_PathConf_clicked();
	void on_btnVdms_PathConf_clear_clicked();
	void on_btnVdms_PathExe_clicked();
	void on_btnVdms_PathExe_clear_clicked();

};

#endif // GRLIDA_IMPORTPATH_H
