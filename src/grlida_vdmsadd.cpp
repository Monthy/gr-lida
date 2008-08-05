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

#include "grlida_vdmsadd.h"
#include "grlida_importar_juego_info.h"

frmVdmsAdd::frmVdmsAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stConfgVdmSDir = stHomeDir + "confvdms/";	// directorio de configuracion para el VDMSound

	stTheme = fGrl.ThemeGrl();
	setTheme();

// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		     , SIGNAL( clicked() ), this, SLOT( on_btnOk()             ) );
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono	 , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );
	connect( ui.btnDescargarInfo , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo()  ) );

	CargaUltimosDirectorios();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmVdmsAdd::~frmVdmsAdd(){}

void frmVdmsAdd::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnVdms_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_Icono->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_FileConfg_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_ExeJuego_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_Icono_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
}

void frmVdmsAdd::CargaUltimosDirectorios()
{
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	UltimoPath.clear();
	lastdir.beginGroup("UltimoDirectorio");
		UltimoPath["Vdms_path_exe"]       = lastdir.value("Vdms_path_exe", "").toString();
		UltimoPath["Vdms_icon"]           = lastdir.value("Vdms_icon", "").toString();
	lastdir.endGroup();
}

void frmVdmsAdd::on_btnDescargarInfo()
{
	frmImportarJuegoInfo * ImportarJuegoInfo = new frmImportarJuegoInfo(0, Qt::Window);
	if( ImportarJuegoInfo->exec() == QDialog::Accepted )
	{
		QFile file_thumbs, file_cover_front, file_cover_back;

		TempDatosJuego.clear();
		ui.txtDatos_Titulo->setText( ImportarJuegoInfo->DatosJuego["titulo"] )			;// titulo
		TempDatosJuego["icono"]			= "vdmsound"									;//icono
		TempDatosJuego["titulo"]		= ui.txtDatos_Titulo->text()					;//titulo
		TempDatosJuego["subtitulo"]		= ImportarJuegoInfo->DatosJuego["subtitulo"]	;//subtitulo
		TempDatosJuego["genero"]		= ImportarJuegoInfo->DatosJuego["genero"]		;//genero
		TempDatosJuego["compania"]		= ImportarJuegoInfo->DatosJuego["compania"]		;//compania
		TempDatosJuego["desarrollador"]	= ImportarJuegoInfo->DatosJuego["desarrollador"];//desarrollador
		TempDatosJuego["tema"]			= ImportarJuegoInfo->DatosJuego["tema"] 		;//tema
		TempDatosJuego["idioma"]		= ImportarJuegoInfo->DatosJuego["idioma"]		;//idioma
		TempDatosJuego["formato"]		= ImportarJuegoInfo->DatosJuego["formato"]		;//formato
		TempDatosJuego["anno"]			= ImportarJuegoInfo->DatosJuego["anno"]			;//anno
		TempDatosJuego["numdisc"]		= ImportarJuegoInfo->DatosJuego["numdisc"]		;//numdisc
		TempDatosJuego["sistemaop"]		= ImportarJuegoInfo->DatosJuego["sistemaop"]	;//sistemaop
		TempDatosJuego["tamano"]		= ImportarJuegoInfo->DatosJuego["tamano"]		;//tamano
		TempDatosJuego["graficos"]		= ImportarJuegoInfo->DatosJuego["graficos"]		;//graficos
		TempDatosJuego["sonido"]		= ImportarJuegoInfo->DatosJuego["sonido"]		;//sonido
		TempDatosJuego["jugabilidad"]	= ImportarJuegoInfo->DatosJuego["jugabilidad"]	;//jugabilidad
		TempDatosJuego["original"]		= ImportarJuegoInfo->DatosJuego["original"]		;//original
		TempDatosJuego["estado"]		= ImportarJuegoInfo->DatosJuego["estado"]		;//estado
		TempDatosJuego["tipo_emu"]		= "vdmsound"									;//tipo_emu
		TempDatosJuego["comentario"]	= ImportarJuegoInfo->DatosJuego["comentario"]	;//comentario
		TempDatosJuego["favorito"]		= ImportarJuegoInfo->DatosJuego["favorito"]		;//favorito

		TempDatosJuego["thumbs"] = ImportarJuegoInfo->DatosJuego["thumbs"];
		if( !file_thumbs.exists( TempDatosJuego["thumbs"] ) )
			TempDatosJuego["thumbs"] = "";

		TempDatosJuego["cover_front"] = ImportarJuegoInfo->DatosJuego["cover_front"];
		if( !file_cover_front.exists( TempDatosJuego["cover_front"] ) )
			TempDatosJuego["cover_front"] = "";

		TempDatosJuego["cover_back"] = ImportarJuegoInfo->DatosJuego["cover_back"];
		if( !file_cover_back.exists( TempDatosJuego["cover_back"] ) )
			TempDatosJuego["cover_back"] = "";
	}

}

void frmVdmsAdd::on_btnOk()
{
	bool siguiente;

	QFile appConfg( stHomeDir + "confvdms/"+ ui.txtVdms_path_conf->text() );

	if ( ui.txtDatos_Titulo->text().isEmpty() )
	{
		siguiente = false;
		QMessageBox::information( this, stTituloVdms(), tr("Debes poner por lo menos el titulo."));
	} else {
		siguiente = true;

		if( ui.txtVdms_path_conf->text().isEmpty() )
		{
			siguiente = false;
			QMessageBox::information(this, stTituloVdms(), tr("Debes indicar el archivo de Configuración para el VDMSound"));
		} else {
			siguiente = true;
			if( appConfg.exists() )
			{
				siguiente = false;
				QMessageBox::information( this, stTituloVdms(), tr("El archivo de Configuración para el VDMSound ya esixte"));
			} else {
				siguiente = true;
				if( ui.txtVdms_path_exe->text().isEmpty() )
				{
					siguiente = false;
					QMessageBox::information(this, stTituloVdms(), tr("Debes indicar el Ejecutable del juego"));
				} else
					siguiente = true;
			}
		}
	}

	if( siguiente == true )
	{
		DatosJuego.clear();
		DatosJuego["icono"]			= "vdmsound"						;//icono
		DatosJuego["titulo"]		= ui.txtDatos_Titulo->text()		;//titulo
		DatosJuego["subtitulo"]		= ""+TempDatosJuego["subtitulo"]	;//subtitulo
		DatosJuego["genero"]		= ""+TempDatosJuego["genero"]		;//genero
		DatosJuego["compania"]		= ""+TempDatosJuego["compania"]		;//compania
		DatosJuego["desarrollador"]	= ""+TempDatosJuego["desarrollador"];//desarrollador
		DatosJuego["tema"]			= ""+TempDatosJuego["tema"]			;//tema
		DatosJuego["idioma"]		= ""+TempDatosJuego["idioma"]		;//idioma
		DatosJuego["formato"]		= ""+TempDatosJuego["formato"]		;//formato
		DatosJuego["anno"]			= ""+TempDatosJuego["anno"]			;//anno
		DatosJuego["numdisc"]		= ""+TempDatosJuego["numdisc"]		;//numdisc
		DatosJuego["sistemaop"]		= ""+TempDatosJuego["sistemaop"]	;//sistemaop
		DatosJuego["tamano"]		= ""+TempDatosJuego["tamano"]		;//tamano
		DatosJuego["graficos"]		= ""+TempDatosJuego["graficos"]		;//graficos
		DatosJuego["sonido"]		= ""+TempDatosJuego["sonido"]		;//sonido
		DatosJuego["jugabilidad"]	= ""+TempDatosJuego["jugabilidad"]	;//jugabilidad
		DatosJuego["original"]		= ""+TempDatosJuego["original"]		;//original
		DatosJuego["estado"]		= ""+TempDatosJuego["estado"]		;//estado
		DatosJuego["thumbs"]		= ""+TempDatosJuego["thumbs"]		;//thumbs
		DatosJuego["cover_front"]	= ""+TempDatosJuego["cover_front"]	;//cover_front
		DatosJuego["cover_back"]	= ""+TempDatosJuego["cover_back"]	;//cover_back
		DatosJuego["fecha"]			= fGrl.HoraFechaActual()			;//fecha d/m/a h:m:s
		DatosJuego["tipo_emu"]		= "vdmsound"						;//tipo_emu
		DatosJuego["comentario"]	= ""+TempDatosJuego["comentario"]	;//comentario
		DatosJuego["favorito"]		= ""+TempDatosJuego["favorito"]		;//favorito

		DatosVDMSound.clear();
		DatosVDMSound["path_conf"]       = ui.txtVdms_path_conf->text()	; //
		DatosVDMSound["path_exe"]        = ui.txtVdms_path_exe->text()	; //
		DatosVDMSound["program_1"]       = ui.txtVdms_params->text() ; //
		DatosVDMSound["program_2"]       = ui.txtVdms_icon->text()	; //
		DatosVDMSound["vdms_debug_1"]    = "no"		; //
		DatosVDMSound["vdms_debug_2"]    = ""		; //
		DatosVDMSound["winnt_dos_1"]     = "no"		; //
		DatosVDMSound["winnt_dos_2"]     = ""		; //
		DatosVDMSound["winnt_dosbox_1"]  = "yes"	; //
		DatosVDMSound["winnt_dosbox_2"]  = "no"		; //
		DatosVDMSound["winnt_dosbox_3"]  = "no"		; //
		DatosVDMSound["winnt_storage_1"] = "yes"	; //
		DatosVDMSound["winnt_storage_2"] = "no"		; //

		QDialog::accept();
	}
}

void frmVdmsAdd::on_btnVdms_FileConfg()
{
	bool str_ok;
	QString str, archivo;
	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stConfgVdmSDir, ui.txtVdms_path_conf->text(), "Config VDMSound (*.vlp);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
		if ( !appConfg.exists() )
		{
			QFileInfo fi( archivo );
			str = fi.fileName();
			str = fGrl.eliminar_caracteres( str );
	  		str_ok = str.endsWith(".vlp");
			if(str_ok == false) str.append(".vlp");
			ui.txtVdms_path_conf->setText( str );
		} else {
			QMessageBox::information( this, stTituloVdms(), tr("El archivo de Configuración para el VDMSound ya esixte"));
			ui.txtVdms_path_conf->setText("");
		}
	}else ui.txtVdms_path_conf->setText( "" );
}

void frmVdmsAdd::on_btnVdms_ExeJuego()
{
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Vdms_path_exe"], ui.txtVdms_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtVdms_path_exe->text() );
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

void frmVdmsAdd::on_btnVdms_Icono()
{
	ui.txtVdms_icon->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), UltimoPath["Vdms_icon"], ui.txtVdms_icon->text(), tr("Todos los archivo") + " (*)", 0, false)  );//+ ",0"

	QFileInfo fi( ui.txtVdms_icon->text() );
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		if( fi.exists() )
		{
			lastdir.setValue("Vdms_icon", fi.absolutePath()+"/" );
			UltimoPath["Vdms_icon"] = fi.absolutePath()+"/";
		} else {
			lastdir.setValue("Vdms_icon", "" );
			UltimoPath["Vdms_icon"] = "";
		}
	lastdir.endGroup();
}

void frmVdmsAdd::on_txtDatos_Titulo_textChanged(const QString &)
{
	bool str_ok;
	QString str = ui.txtDatos_Titulo->text();
	if(str != "")
	{
		str = fGrl.eliminar_caracteres( str );
  		str_ok = str.endsWith(".vlp");
		if(str_ok == false) str.append(".vlp");
		ui.txtVdms_path_conf->setText( str );
	} else
		ui.txtVdms_path_conf->setText( "" );
}
