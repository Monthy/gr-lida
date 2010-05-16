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

#include "grlida_addedit_vdmsound.h"

frmAddEditVDMSound::frmAddEditVDMSound(QWidget *parent)
    : QTabWidget(parent)
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	GRLConfig = fGrl.CargarGRLConfig(stHomeDir+"GR-lida.conf");
	stTheme   = fGrl.ThemeGrl();
	setTheme();

// Conecta los distintos botones con las funciones.
	createConnections();
}

frmAddEditVDMSound::~frmAddEditVDMSound()
{
	//
}

bool frmAddEditVDMSound::isCorrectNext()
{
	bool siguiente = false;
	if( ui.txtVdms_path_conf->text() != "" )
	{
		siguiente = true;
		if( ui.txtVdms_path_exe->text() == "" )
		{
			siguiente=false;
			QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " VDMSound");
		} else {
			siguiente = true;
			if( EditandoJuego != true )
			{
				QFile appConfg( stHomeDir + "confvdms/"+ ui.txtVdms_path_conf->text() );
				if( appConfg.exists() )
				{
					siguiente = false;
					ui.txtVdms_path_conf->setText("");
					QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
				} else
					siguiente = true;
			}
		}
	} else {
		siguiente = false;
		QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " VDMSound");
	}
	return siguiente;
}

void frmAddEditVDMSound::setEditandoJuego(bool editando)
{
	EditandoJuego = editando;
}

void frmAddEditVDMSound::createConnections()
{
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono    , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );
}

void frmAddEditVDMSound::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	ui.btnVdms_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_Icono->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_FileConfg_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_ExeJuego_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_Icono_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

void frmAddEditVDMSound::setConfigDefecto()
{
	ui.txtVdms_path_conf->clear();
	ui.txtVdms_path_exe->clear();
	ui.txtVdms_params->clear();
	ui.txtVdms_icon->clear();
	ui.txtVdms_autoexec->clear();
	ui.txtVdms_customCfg->clear();
	ui.chkVdms_useAutoexec->setChecked(false);
	ui.chkVdms_useCustomCfg->setChecked(false);
	ui.chkVdms_exitclose->setChecked(false);
	ui.chkVdms_exitWarn->setChecked(false);
	ui.chkVdms_fastPaste->setChecked(false);
	ui.chkVdms_useCDROM->setChecked(false);
	ui.chkVdms_useNetware->setChecked(false);
}

void frmAddEditVDMSound::CargarDatosVDMSound(QString IDGrl)
{
	QHash<QString, QString> tempDatosVDMSound;

	tempDatosVDMSound.clear();
	tempDatosVDMSound = sql->showConfg_VDMSound(IDGrl);
	stItemIDVdms = sql->ItemIDIndex("dbgrl_emu_vdmsound", IDGrl);	// Obtenemos el Index del VDMSound
	CargarDatosVDMSound( tempDatosVDMSound );
}

void frmAddEditVDMSound::CargarDatosVDMSound(QHash<QString, QString> datosVdms)
{
	ui.txtVdms_path_conf->setText( datosVdms["Vdms_path_conf"] );
	ui.txtVdms_path_exe->setText( datosVdms["Vdms_path_exe"] );

	QStringList list_program       = datosVdms["Vdms_program"].split("|");
	QStringList list_winnt_dos     = datosVdms["Vdms_winnt_dos"].split("|");
	QStringList list_vdms_debug    = datosVdms["Vdms_vdms_debug"].split("|");
	QStringList list_winnt_dosbox  = datosVdms["Vdms_winnt_dosbox"].split("|");
	QStringList list_winnt_storage = datosVdms["Vdms_winnt_storage"].split("|");

	ui.txtVdms_params->setText( list_program[0] );
	ui.txtVdms_icon->setText( list_program[1] );

	ui.chkVdms_useAutoexec->setChecked( fGrl.StrToBool( list_winnt_dos[0] ) );
	ui.txtVdms_autoexec->setPlainText( list_winnt_dos[1].replace("%000d%000a", "\n") );

	ui.chkVdms_useCustomCfg->setChecked( fGrl.StrToBool( list_vdms_debug[0] ) );
	ui.txtVdms_customCfg->setPlainText( list_vdms_debug[1].replace("%000d%000a", "\n") );

	ui.chkVdms_exitclose->setChecked( fGrl.StrToBool( list_winnt_dosbox[0] ) );
	ui.chkVdms_exitWarn->setChecked( fGrl.StrToBool( list_winnt_dosbox[1] ) );
	ui.chkVdms_fastPaste->setChecked( fGrl.StrToBool( list_winnt_dosbox[2] ) );

	ui.chkVdms_useCDROM->setChecked( fGrl.StrToBool( list_winnt_storage[0] ) );
	ui.chkVdms_useNetware->setChecked( fGrl.StrToBool( list_winnt_storage[1] ) );
}

QHash<QString, QString> frmAddEditVDMSound::setDatosVDMSound()
{
	QHash<QString, QString> tempDatosVDMSound;

	tempDatosVDMSound.clear();
	tempDatosVDMSound["Vdms_id"]        = stItemIDVdms;					// id del VDMSound
	tempDatosVDMSound["Vdms_path_conf"] = ui.txtVdms_path_conf->text();
	tempDatosVDMSound["Vdms_path_exe"]  = ui.txtVdms_path_exe->text();
	tempDatosVDMSound["Vdms_program_1"] = ui.txtVdms_params->text();
	tempDatosVDMSound["Vdms_program_2"] = ui.txtVdms_icon->text();

	tempDatosVDMSound["Vdms_winnt_dos_1"] = fGrl.BoolToStr( ui.chkVdms_useAutoexec->isChecked() ,true);
	tempDatosVDMSound["Vdms_winnt_dos_2"] = ui.txtVdms_autoexec->toPlainText().replace("\n", "%000d%000a");

	tempDatosVDMSound["Vdms_vdms_debug_1"] = fGrl.BoolToStr( ui.chkVdms_useCustomCfg->isChecked() ,true);
	tempDatosVDMSound["Vdms_vdms_debug_2"] = ui.txtVdms_customCfg->toPlainText().replace("\n", "%000d%000a");

	tempDatosVDMSound["Vdms_winnt_dosbox_1"] = fGrl.BoolToStr( ui.chkVdms_exitclose->isChecked() ,true);
	tempDatosVDMSound["Vdms_winnt_dosbox_2"] = fGrl.BoolToStr( ui.chkVdms_exitWarn->isChecked() ,true);
	tempDatosVDMSound["Vdms_winnt_dosbox_3"] = fGrl.BoolToStr( ui.chkVdms_fastPaste->isChecked() ,true);

	tempDatosVDMSound["Vdms_winnt_storage_1"] = fGrl.BoolToStr( ui.chkVdms_useCDROM->isChecked() ,true);
	tempDatosVDMSound["Vdms_winnt_storage_2"] = fGrl.BoolToStr( ui.chkVdms_useNetware->isChecked() ,true);

	return tempDatosVDMSound;
}

void frmAddEditVDMSound::on_btnVdms_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir + "confvdms/", ui.txtVdms_path_conf->text(), "Config VDMSound (*.vlp);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
		if( EditandoJuego != true )
		{
			if( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
		}
		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
		str_ok = str.endsWith(".vlp");
		if(str_ok == false)
			str.append(".vlp");
		ui.txtVdms_path_conf->setText( str );
	} else
		ui.txtVdms_path_conf->setText( "" );
}

void frmAddEditVDMSound::on_btnVdms_ExeJuego()
{
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_path_exe"].toString(), ui.txtVdms_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtVdms_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Vdms_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Vdms_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_path_exe", GRLConfig["Vdms_path_exe"].toString() );
}

void frmAddEditVDMSound::on_btnVdms_Icono()
{
	ui.txtVdms_icon->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_icon"].toString(), ui.txtVdms_icon->text(),
														tr("Iconos")+" (*.ico *.exe *.dll);;"+
														tr("Todos los archivo") + " (*)", 0, false)  );//+ ",0"

	QFileInfo fi( ui.txtVdms_icon->text() );
	if( fi.exists() )
		GRLConfig["Vdms_icon"] = fi.absolutePath();
	else
		GRLConfig["Vdms_icon"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_icon", GRLConfig["Vdms_icon"].toString() );
}

