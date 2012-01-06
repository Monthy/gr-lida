/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

class frmImportPath: public QDialog
{
    Q_OBJECT

public:
	frmImportPath(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmImportPath();

	Ui::ImportPathClass ui;

private:
	Funciones fGrl;

	QString stHomeDir, stTheme;

	QHash<QString, QVariant> GRLConfig;

	void createConnections();
	void setTheme();
	void CargaUltimosDirectorios();

private slots:
	void on_btnOk();
	void on_estado_cheket_items(bool estado);

// DOSBox
	void on_btnDbx_FileConfg();
	void on_btnDbx_ExeJuego();
	void on_btnDbx_ExeSetup();
	void on_btnDbx_mapperfile();
	void on_btnDbx_language();
	void on_btnDbx_capturas();
	void on_btnDbx_musica();
// ScummVM
	void on_btnSvm_Path();
	void on_btnSvm_PathSave();
	void on_btnSvm_PathSetup();
	void on_btnSvm_PathExtra();
	void on_btnSvm_PathCapturas();
	void on_btnSvm_PathSonido();
// VDMSound
	void on_btnVdms_PathConf();
	void on_btnVdms_PathExe();

public slots:
	void on_changeEnabled(bool estado);

};

#endif // GRLIDA_IMPORTPATH_H
