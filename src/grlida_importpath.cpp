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

#include "grlida_importpath.h"

frmImportPath::frmImportPath(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
	
	stHomeDir = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir = "./";							// directorio de trabajo del GR-lida
	
	connect( ui.btnOk, SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );

	connect( ui.btnDirPath_Datos_Thumbs     , SIGNAL( clicked() ), this, SLOT( on_btnDirPath_Datos_Thumbs() ) );
	connect( ui.btnDirPath_Datos_CoverFront , SIGNAL( clicked() ), this, SLOT( on_btnDirPath_Datos_CoverFront() ) );
	connect( ui.btnDirPath_Datos_CoverBack  , SIGNAL( clicked() ), this, SLOT( on_btnDirPath_Datos_CoverBack() ) );

	connect( ui.btnDirPath_Dbx_1 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxFile_1() ) );
	connect( ui.btnDirPath_Dbx_2 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxFile_2() ) );
	connect( ui.btnDirPath_Dbx_3 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxFile_3() ) );
	connect( ui.btnDirPath_Dbx_4 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxFile_4() ) );
	connect( ui.btnDirPath_Dbx_5 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxFile_5() ) );
	connect( ui.btnDirPath_Dbx_6 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxDir_6()  ) );
	connect( ui.btnDirPath_Dbx_7 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxDir_7()  ) );
	connect( ui.btnDirPath_Dbx_8 , SIGNAL( clicked() ), this, SLOT( on_btnOpenDbxDir_8()  ) );

	connect( ui.btnDirPath_Svm_1 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmDir_1()  ) );
	connect( ui.btnDirPath_Svm_2 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmDir_2()  ) );
	connect( ui.btnDirPath_Svm_3 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmFile3()  ) );
	connect( ui.btnDirPath_Svm_4 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmDir_4()  ) );
	connect( ui.btnDirPath_Svm_5 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmDir_5()  ) );
	connect( ui.btnDirPath_Svm_6 , SIGNAL( clicked() ), this, SLOT( on_btnOpenSvmDir_6()  ) );

	connect( ui.btnDirPath_Vdms_1, SIGNAL( clicked() ), this, SLOT( on_btnOpenVdmsFile_1()) );
	connect( ui.btnDirPath_Vdms_2, SIGNAL( clicked() ), this, SLOT( on_btnOpenVdmsFile_2()) );

	if( ui.gBox_path_emu->isEnabled() )
	{
		if( ui.wizardPath->currentIndex() == 0 )
			ui.gBox_path_emu->setTitle("Path - DOSBox");
		if( ui.wizardPath->currentIndex() == 1 )
			ui.gBox_path_emu->setTitle("Path - ScummVM");
		if( ui.wizardPath->currentIndex() == 2 )
			ui.gBox_path_emu->setTitle("Path - VDMSound");
	} else
			ui.gBox_path_emu->setTitle("Path");

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportPath::~frmImportPath(){}

void frmImportPath::on_btnOk()
{
	QFile appConfg;
	QString stExeJuego;	
	bool siguiente = true;

	if( ui.gBox_path_emu->isEnabled() )
	{
		siguiente = false;
		if( (ui.wizardPath->currentIndex() == 0) || (ui.wizardPath->currentIndex() == 2) )
		{
		// DOSBox
			if( ui.wizardPath->currentIndex() == 0 )
				appConfg.setFileName( stHomeDir + "confdbx/"+ ui.txtPath_Dbx_1->text() );
		// VDMSound
			if( ui.wizardPath->currentIndex() == 2 )
				appConfg.setFileName( stHomeDir + "confvdms/"+ ui.txtPath_Vdms_1->text() );

			if( appConfg.exists() )
			{
				siguiente = false;
				QMessageBox::information( this, "ImportPath", tr("El archivo de Configuración ya esixte")+":\n\n"+appConfg.fileName() );
			} else {
				siguiente = true;
				stExeJuego.clear();
				if( ui.wizardPath->currentIndex() == 0 )
					stExeJuego = ui.txtPath_Dbx_2->text();
				if( ui.wizardPath->currentIndex() == 2 )
					stExeJuego = ui.txtPath_Vdms_2->text();
				if( stExeJuego.isEmpty() )
				{
					siguiente = false;
					QMessageBox::information(this, "ImportPath", tr("Debes indicar el Ejecutable del juego") );
				} else
					siguiente = true;
			}
		} else 
			siguiente = true;
	}

	if( siguiente == true )
		QDialog::accept();
	else
		QDialog::rejected();
}

void frmImportPath::on_btnDirPath_Datos_Thumbs()
{
	ui.txtDatosPath_Thumbs->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDatosPath_Thumbs->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnDirPath_Datos_CoverFront()
{
	ui.txtDatosPath_CoverFront->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDatosPath_CoverFront->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnDirPath_Datos_CoverBack()
{
	ui.txtDatosPath_CoverBack->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDatosPath_CoverBack->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenDbxFile_1()
{
	QString archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir+"/confdbx/", ui.txtPath_Dbx_1->text(), tr("Todos los archivo") + " (*)", 0, true);
	QFile appConfg( archivo );
	if( !appConfg.exists() )
	{
		QFileInfo fi( archivo );
		ui.txtPath_Dbx_1->setText( fi.fileName() );
	} else {
		QMessageBox::information( this, "GR-lida", tr("El archivo de Configuración para el DOSBox ya esixte"));
		ui.txtPath_Dbx_1->setText("");
	}
}

void frmImportPath::on_btnOpenDbxFile_2()
{
	ui.txtPath_Dbx_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Dbx_2->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenDbxFile_3()
{
	ui.txtPath_Dbx_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Dbx_3->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenDbxFile_4()
{
	ui.txtPath_Dbx_4->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Dbx_4->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenDbxFile_5()
{
	ui.txtPath_Dbx_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Dbx_5->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenDbxDir_6()
{
	ui.txtPath_Dbx_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Dbx_6->text() ));
}

void frmImportPath::on_btnOpenDbxDir_7()
{
	ui.txtPath_Dbx_7->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Dbx_7->text() ));
}

void frmImportPath::on_btnOpenDbxDir_8()
{
	ui.txtPath_Dbx_8->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Dbx_8->text() ));
}

void frmImportPath::on_btnOpenSvmDir_1()
{
	ui.txtPath_Svm_1->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Svm_1->text() ));
}

void frmImportPath::on_btnOpenSvmDir_2()
{
	ui.txtPath_Svm_2->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Svm_2->text() ));
}

void frmImportPath::on_btnOpenSvmFile3()
{
	ui.txtPath_Svm_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Svm_3->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmImportPath::on_btnOpenSvmDir_4()
{
	ui.txtPath_Svm_4->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Svm_4->text() ));
}

void frmImportPath::on_btnOpenSvmDir_5()
{
	ui.txtPath_Svm_5->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Svm_5->text() ));
}

void frmImportPath::on_btnOpenSvmDir_6()
{
	ui.txtPath_Svm_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtPath_Svm_6->text() ));
}

void frmImportPath::on_btnOpenVdmsFile_1()
{
	QString archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir+"/confvdms/", ui.txtPath_Vdms_1->text(), tr("Todos los archivo") + " (*)", 0, true);
	QFile appConfg( archivo );
	if( !appConfg.exists() )
	{
		QFileInfo fi( archivo );
		ui.txtPath_Vdms_1->setText( fi.fileName() );
	} else {
		QMessageBox::information( this, "GR-lida", tr("El archivo de Configuración para el VDMSound ya esixte"));
		ui.txtPath_Vdms_1->setText("");
	}
}

void frmImportPath::on_btnOpenVdmsFile_2()
{
	ui.txtPath_Vdms_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtPath_Vdms_2->text(), tr("Todos los archivo") + " (*)", 0, false) );
}
