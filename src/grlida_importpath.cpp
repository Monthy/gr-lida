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

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida

	stTheme = fGrl.ThemeGrl();
	setTheme();
	CargaUltimosDirectorios();

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

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportPath::~frmImportPath(){}

void frmImportPath::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );

	ui.btnDirPath_Datos_Thumbs->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Datos_CoverFront->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirPath_Datos_CoverBack->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );

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
}

void frmImportPath::CargaUltimosDirectorios()
{
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	UltimoPath.clear();
	lastdir.beginGroup("UltimoDirectorio");
	// Datos de Juegos
		UltimoPath["Img_Thumbs"]          = lastdir.value("Img_Thumbs", "").toString();
		UltimoPath["Img_CoverFront"]      = lastdir.value("Img_CoverFront", "").toString();
		UltimoPath["Img_CoverBack"]       = lastdir.value("Img_CoverBack", "").toString();
	// DOSBox
		UltimoPath["Dbx_path_exe"]        = lastdir.value("Dbx_path_exe", "").toString();
		UltimoPath["Dbx_path_setup"]      = lastdir.value("Dbx_path_setup", "").toString();
		UltimoPath["Dbx_gus_ultradir"]    = lastdir.value("Dbx_gus_ultradir", "").toString();
		UltimoPath["Dbx_sdl_mapperfile"]  = lastdir.value("Dbx_sdl_mapperfile", "").toString();
		UltimoPath["Dbx_dosbox_language"] = lastdir.value("Dbx_dosbox_language", "").toString();
		UltimoPath["Dbx_dosbox_captures"] = lastdir.value("Dbx_dosbox_captures", "").toString();
		UltimoPath["Dbx_path_sonido"]     = lastdir.value("Dbx_path_sonido", "").toString();
	// ScummVM
		UltimoPath["Svm_path"]            = lastdir.value("Svm_path", "").toString();
		UltimoPath["Svm_savepath"]        = lastdir.value("Svm_savepath", "").toString();
		UltimoPath["Svm_extrapath"]       = lastdir.value("Svm_extrapath", "").toString();
		UltimoPath["Svm_path_capturas"]   = lastdir.value("Svm_path_capturas", "").toString();
		UltimoPath["Svm_path_sonido"]     = lastdir.value("Svm_path_sonido", "").toString();
		UltimoPath["Svm_path_setup"]      = lastdir.value("Svm_path_setup", "").toString();
		UltimoPath["Svm_soundfont"]       = lastdir.value("Svm_soundfont", "").toString();
	// VDMSound
		UltimoPath["Vdms_path_exe"]       = lastdir.value("Vdms_path_exe", "").toString();
		UltimoPath["Vdms_icon"]           = lastdir.value("Vdms_icon", "").toString();
	lastdir.endGroup();
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

void frmImportPath::on_btnDirPath_Datos_Thumbs()
{
	ui.txtDatosPath_Thumbs->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Img_Thumbs"], ui.txtDatosPath_Thumbs->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDatosPath_Thumbs->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Img_Thumbs", fi.absolutePath()+"/" );
			UltimoPath["Img_Thumbs"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Img_Thumbs", "" );
			UltimoPath["Img_Thumbs"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnDirPath_Datos_CoverFront()
{
	ui.txtDatosPath_CoverFront->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Img_CoverFront"], ui.txtDatosPath_CoverFront->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDatosPath_CoverFront->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Img_CoverFront", fi.absolutePath()+"/" );
			UltimoPath["Img_CoverFront"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Img_CoverFront", "" );
			UltimoPath["Img_CoverFront"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnDirPath_Datos_CoverBack()
{
	ui.txtDatosPath_CoverBack->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Img_CoverBack"], ui.txtDatosPath_CoverBack->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDatosPath_CoverBack->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Img_CoverBack", fi.absolutePath()+"/" );
			UltimoPath["Img_CoverBack"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Img_CoverBack", "" );
			UltimoPath["Img_CoverBack"] = "";
		}
	lastdir.endGroup();
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
	ui.txtPath_Dbx_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Dbx_path_exe"], ui.txtPath_Dbx_2->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_2->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Dbx_path_exe", fi.absolutePath()+"/" );
			UltimoPath["Dbx_path_exe"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Dbx_path_exe", "" );
			UltimoPath["Dbx_path_exe"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnOpenDbxFile_3()
{
	ui.txtPath_Dbx_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Dbx_path_setup"], ui.txtPath_Dbx_3->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_3->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Dbx_path_setup", fi.absolutePath()+"/" );
			UltimoPath["Dbx_path_setup"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Dbx_path_setup", "" );
			UltimoPath["Dbx_path_setup"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnOpenDbxFile_4()
{
	ui.txtPath_Dbx_4->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Dbx_dosbox_language"], ui.txtPath_Dbx_4->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_4->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Dbx_dosbox_language", fi.absolutePath()+"/" );
			UltimoPath["Dbx_dosbox_language"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Dbx_dosbox_language", "" );
			UltimoPath["Dbx_dosbox_language"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnOpenDbxFile_5()
{
	ui.txtPath_Dbx_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Dbx_sdl_mapperfile"], ui.txtPath_Dbx_5->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Dbx_4->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Dbx_sdl_mapperfile", fi.absolutePath()+"/" );
			UltimoPath["Dbx_sdl_mapperfile"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Dbx_sdl_mapperfile", "" );
			UltimoPath["Dbx_sdl_mapperfile"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnOpenDbxDir_6()
{
	ui.txtPath_Dbx_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Dbx_gus_ultradir"], ui.txtPath_Dbx_6->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Dbx_gus_ultradir", ui.txtPath_Dbx_6->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Dbx_gus_ultradir"] = ui.txtPath_Dbx_6->text()+"/";
}

void frmImportPath::on_btnOpenDbxDir_7()
{
	ui.txtPath_Dbx_7->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Dbx_dosbox_captures"], ui.txtPath_Dbx_7->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Dbx_dosbox_captures", ui.txtPath_Dbx_7->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Dbx_dosbox_captures"] = ui.txtPath_Dbx_7->text()+"/";
}

void frmImportPath::on_btnOpenDbxDir_8()
{
	ui.txtPath_Dbx_8->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Dbx_path_sonido"], ui.txtPath_Dbx_8->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Dbx_path_sonido", ui.txtPath_Dbx_8->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Dbx_path_sonido"] = ui.txtPath_Dbx_8->text()+"/";
}

void frmImportPath::on_btnOpenSvmDir_1()
{
	ui.txtPath_Svm_1->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Svm_path"], ui.txtPath_Svm_1->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_path", ui.txtPath_Svm_1->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_path"] = ui.txtPath_Svm_1->text()+"/";
}

void frmImportPath::on_btnOpenSvmDir_2()
{
	ui.txtPath_Svm_2->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Svm_savepath"], ui.txtPath_Svm_2->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_savepath", ui.txtPath_Svm_2->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_savepath"] = ui.txtPath_Svm_2->text()+"/";
}

void frmImportPath::on_btnOpenSvmFile3()
{
	ui.txtPath_Svm_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Svm_path_setup"], ui.txtPath_Svm_3->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Svm_3->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Svm_path_setup", fi.absolutePath()+"/" );
			UltimoPath["Svm_path_setup"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Svm_path_setup", "" );
			UltimoPath["Svm_path_setup"] = "";
		}
	lastdir.endGroup();
}

void frmImportPath::on_btnOpenSvmDir_4()
{
	ui.txtPath_Svm_4->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Svm_extrapath"], ui.txtPath_Svm_4->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_extrapath", ui.txtPath_Svm_4->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_extrapath"] = ui.txtPath_Svm_4->text()+"/";
}

void frmImportPath::on_btnOpenSvmDir_5()
{
	ui.txtPath_Svm_5->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Svm_path_capturas"], ui.txtPath_Svm_5->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_path_capturas", ui.txtPath_Svm_5->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_path_capturas"] = ui.txtPath_Svm_5->text()+"/";
}

void frmImportPath::on_btnOpenSvmDir_6()
{
	ui.txtPath_Svm_6->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["Svm_path_sonido"], ui.txtPath_Svm_6->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_path_sonido", ui.txtPath_Svm_6->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_path_sonido"] = ui.txtPath_Svm_6->text()+"/";
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
	ui.txtPath_Vdms_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Vdms_path_exe"], ui.txtPath_Vdms_2->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtPath_Vdms_2->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Vdms_path_exe", fi.absolutePath()+"/" );
			UltimoPath["Vdms_path_exe"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Vdms_path_exe", "" );
			UltimoPath["Vdms_path_exe"] = "";
		}
	lastdir.endGroup();
}
