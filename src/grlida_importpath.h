/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006  Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#ifndef GRLIDA_IMPORTPATH_H
#define GRLIDA_IMPORTPATH_H

#include <QtCore>
#include <QtGui>
#include "funciones.h"
#include "ui_importpath.h"

class frmImportPath: public QDialog {
    Q_OBJECT
public:
	frmImportPath(QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmImportPath();

	Ui::ImportPathClass ui;

private:
	Funciones fGrl;
	QString stHomeDir; 

private slots:
	void on_btnOk();

	void on_btnDirPath_Datos_Thumbs();
	void on_btnDirPath_Datos_CoverFront();
	void on_btnDirPath_Datos_CoverBack();

	void on_btnOpenDbxFile_1();
	void on_btnOpenDbxFile_2();
	void on_btnOpenDbxFile_3();
	void on_btnOpenDbxFile_4();
	void on_btnOpenDbxFile_5();
	void on_btnOpenDbxDir_6();
	void on_btnOpenDbxDir_7();
	void on_btnOpenDbxDir_8();

	void on_btnOpenSvmDir_1();
	void on_btnOpenSvmDir_2();
	void on_btnOpenSvmFile3();
	void on_btnOpenSvmDir_4();
	void on_btnOpenSvmDir_5();
	void on_btnOpenSvmDir_6();

	void on_btnOpenVdmsFile_1();
	void on_btnOpenVdmsFile_2();

};

#endif // GRLIDA_IMPORTPATH_H
