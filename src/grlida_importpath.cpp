/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_importpath.h"

frmImportPath::frmImportPath(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
	ui.frame_info->setVisible(false);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	createConnections();

	setTheme();

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportPath::~frmImportPath()
{
	//
}

void frmImportPath::on_changeEnabled(bool estado)
{
	ui.btnOk->setEnabled(estado);
	ui.frame_info->setVisible(!estado);
}

void frmImportPath::createConnections()
{
	connect( ui.btnOk, SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );

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
}

void frmImportPath::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/archivos.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );

	ui.btnDirPath_Dbx_1->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Dbx_2->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Dbx_3->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Dbx_4->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Dbx_5->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Dbx_6->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Dbx_7->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Dbx_8->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );

	ui.btnDirPath_Svm_1->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Svm_2->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Svm_3->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Svm_4->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Svm_5->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirPath_Svm_6->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );

	ui.btnDirPath_Vdms_1->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Vdms_2->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

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
	ui.txtPath_Dbx_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_exe"].toString(), ui.txtPath_Dbx_2->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_2->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_exe", GRLConfig["Dbx_path_exe"].toString() );
}

void frmImportPath::on_btnOpenDbxFile_3()
{
	ui.txtPath_Dbx_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_setup"].toString(), ui.txtPath_Dbx_3->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_3->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_setup", GRLConfig["Dbx_path_setup"].toString() );
}

void frmImportPath::on_btnOpenDbxFile_4()
{
	ui.txtPath_Dbx_4->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_dosbox_language"].toString(), ui.txtPath_Dbx_4->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_4->text() );
	if( fi.exists() )
		GRLConfig["Dbx_dosbox_language"] = fi.absolutePath();
	else
		GRLConfig["Dbx_dosbox_language"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_language", GRLConfig["Dbx_dosbox_language"].toString() );
}

void frmImportPath::on_btnOpenDbxFile_5()
{
	ui.txtPath_Dbx_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_sdl_mapperfile"].toString(), ui.txtPath_Dbx_5->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_4->text() );
	if( fi.exists() )
		GRLConfig["Dbx_sdl_mapperfile"] = fi.absolutePath();
	else
		GRLConfig["Dbx_sdl_mapperfile"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_sdl_mapperfile", GRLConfig["Dbx_sdl_mapperfile"].toString() );
}

void frmImportPath::on_btnOpenDbxDir_6()
{
	ui.txtPath_Dbx_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_gus_ultradir"].toString(), ui.txtPath_Dbx_6->text() ));

	QDir dir( ui.txtPath_Dbx_6->text() );
	if( dir.exists() )
		GRLConfig["Dbx_gus_ultradir"] = ui.txtPath_Dbx_6->text();
	else
		GRLConfig["Dbx_gus_ultradir"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_gus_ultradir", GRLConfig["Dbx_gus_ultradir"].toString() );
}

void frmImportPath::on_btnOpenDbxDir_7()
{
	ui.txtPath_Dbx_7->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_dosbox_captures"].toString(), ui.txtPath_Dbx_7->text() ));

	QDir dir( ui.txtPath_Dbx_7->text() );
	if( dir.exists() )
		GRLConfig["Dbx_dosbox_captures"] = ui.txtPath_Dbx_7->text();
	else
		GRLConfig["Dbx_dosbox_captures"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_captures", GRLConfig["Dbx_dosbox_captures"].toString() );
}

void frmImportPath::on_btnOpenDbxDir_8()
{
	ui.txtPath_Dbx_8->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_path_sonido"].toString(), ui.txtPath_Dbx_8->text() ));

	QDir dir( ui.txtPath_Dbx_8->text() );
	if( dir.exists() )
		GRLConfig["Dbx_path_sonido"] = ui.txtPath_Dbx_8->text();
	else
		GRLConfig["Dbx_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_sonido", GRLConfig["Dbx_path_sonido"].toString() );
}

void frmImportPath::on_btnOpenSvmDir_1()
{
	ui.txtPath_Svm_1->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path"].toString(), ui.txtPath_Svm_1->text() ));

	QDir dir( ui.txtPath_Svm_1->text() );
	if( dir.exists() )
		GRLConfig["Svm_path"] = ui.txtPath_Svm_1->text();
	else
		GRLConfig["Svm_path"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path", GRLConfig["Svm_path"].toString() );
}

void frmImportPath::on_btnOpenSvmDir_2()
{
	ui.txtPath_Svm_2->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_savepath"].toString(), ui.txtPath_Svm_2->text() ));

	QDir dir( ui.txtPath_Svm_2->text() );
	if( dir.exists() )
		GRLConfig["Svm_savepath"] = ui.txtPath_Svm_2->text();
	else
		GRLConfig["Svm_savepath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_savepath", GRLConfig["Svm_savepath"].toString() );
}

void frmImportPath::on_btnOpenSvmFile3()
{
	ui.txtPath_Svm_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Svm_path_setup"].toString(), ui.txtPath_Svm_3->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Svm_3->text() );
	if( fi.exists() )
		GRLConfig["Svm_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Svm_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_setup", GRLConfig["Svm_path_setup"].toString() );
}

void frmImportPath::on_btnOpenSvmDir_4()
{
	ui.txtPath_Svm_4->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_extrapath"].toString(), ui.txtPath_Svm_4->text() ));

	QDir dir( ui.txtPath_Svm_4->text() );
	if( dir.exists() )
		GRLConfig["Svm_extrapath"] = ui.txtPath_Svm_4->text();
	else
		GRLConfig["Svm_extrapath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_extrapath", GRLConfig["Svm_extrapath"].toString() );
}

void frmImportPath::on_btnOpenSvmDir_5()
{
	ui.txtPath_Svm_5->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path_capturas"].toString(), ui.txtPath_Svm_5->text() ));

	QDir dir( ui.txtPath_Svm_5->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_capturas"] = ui.txtPath_Svm_5->text();
	else
		GRLConfig["Svm_path_capturas"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_capturas", GRLConfig["Svm_path_capturas"].toString() );
}

void frmImportPath::on_btnOpenSvmDir_6()
{
	ui.txtPath_Svm_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path_sonido"].toString(), ui.txtPath_Svm_6->text() ));

	QDir dir( ui.txtPath_Svm_6->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_sonido"] = ui.txtPath_Svm_6->text();
	else
		GRLConfig["Svm_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_sonido", GRLConfig["Svm_path_sonido"].toString() );
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
	ui.txtPath_Vdms_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_path_exe"].toString(), ui.txtPath_Vdms_2->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Vdms_2->text() );
	if( fi.exists() )
		GRLConfig["Vdms_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Vdms_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_path_exe", GRLConfig["Vdms_path_exe"].toString() );
}
