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

frmVdmsAdd::frmVdmsAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir = "./";							// directorio de trabajo del GR-lida
	stConfgVdmSDir = stHomeDir + "confvdms/";	// directorio de configuracion para el VDMSound
	
	
// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		     , SIGNAL( clicked() ), this, SLOT( on_btnOk()             ) );
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono	 , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );
	
// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmVdmsAdd::~frmVdmsAdd(){}

void frmVdmsAdd::on_btnOk()
{
	bool siguiente;

	QFile appConfg( stHomeDir + "confvdms/"+ ui.txtVdms_1->text() );

	if ( ui.txtDatos_1->text().isEmpty() )
	{
		siguiente = false;
		QMessageBox::information( this, stTituloVdms(), tr("Debes poner por lo menos el titulo."));
	} else {
		siguiente = true;

		if( ui.txtVdms_1->text().isEmpty() )
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
				if( ui.txtVdms_2->text().isEmpty() )
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
		DatosJuego["icono"]			= "vdmsound"		;//icono
		DatosJuego["titulo"]		= ui.txtDatos_1->text();//titulo
		DatosJuego["subtitulo"] 	= ""			;//subtitulo
		DatosJuego["genero"] 		= ""			;//genero
		DatosJuego["compania"] 		= ""			;//compania
		DatosJuego["desarrollador"] = ""			;//desarrollador
		DatosJuego["tema"] 			= ""			;//tema
		DatosJuego["idioma"] 		= ""			;//idioma
		DatosJuego["formato"] 		= ""			;//formato
		DatosJuego["anno"] 			= ""			;//anno
		DatosJuego["numdisc"] 		= ""			;//numdisc
		DatosJuego["sistemaop"] 	= ""			;//sistemaop
		DatosJuego["tamano"] 		= ""			;//tamano
		DatosJuego["graficos"] 		= "1"			;//graficos
		DatosJuego["sonido"] 		= "1"			;//sonido
		DatosJuego["jugabilidad"] 	= "1"			;//jugabilidad
		DatosJuego["original"] 		= "false"		;//original
		DatosJuego["estado"] 		= ""			;//estado
		DatosJuego["thumbs"] 		= ""			;//thumbs
		DatosJuego["cover_front"] 	= ""			;//cover_front
		DatosJuego["cover_back"] 	= ""			;//cover_back
		DatosJuego["fecha"] 		= fGrl.HoraFechaActual();//fecha d/m/a h:m:s
		DatosJuego["tipo_emu"] 		= "vdmsound"	;//tipo_emu
		DatosJuego["comentario"] 	= ""			;//comentario
		DatosJuego["favorito"]		= "false"		;//favorito

		DatosVDMSound.clear();
		DatosVDMSound["path_conf"]       = ui.txtVdms_1->text()	; //
		DatosVDMSound["path_exe"]        = ui.txtVdms_2->text()	; //
		DatosVDMSound["program_1"]       = ui.txtVdms_3->text() ; //
		DatosVDMSound["program_2"]       = ui.txtVdms_4->text()	; //
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
	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stConfgVdmSDir, ui.txtVdms_1->text(), tr("Todos los archivo") + " (*.*)", 0, true);
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
			ui.txtVdms_1->setText( str );
		} else {
			QMessageBox::information( this, stTituloVdms(), tr("El archivo de Configuración para el VDMSound ya esixte"));
			ui.txtVdms_1->setText("");
		}
	}else ui.txtVdms_1->setText( "" );
}

void frmVdmsAdd::on_btnVdms_ExeJuego()
{
	ui.txtVdms_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stConfgVdmSDir, ui.txtVdms_2->text(), tr("Todos los archivo") + " (*.*)", 0, false) );
}

void frmVdmsAdd::on_btnVdms_Icono()
{
	ui.txtVdms_4->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stConfgVdmSDir, ui.txtVdms_4->text(), tr("Todos los archivo") + " (*.*)", 0, false)+ ",0");
}

void frmVdmsAdd::on_txtDatos_1_textChanged(const QString &)
{
	bool str_ok;
	QString str = ui.txtDatos_1->text();
	if(str != "")
	{
		str = fGrl.eliminar_caracteres( str );
  		str_ok = str.endsWith(".vlp");
		if(str_ok == false) str.append(".vlp");
		ui.txtVdms_1->setText( str );
	} else
		ui.txtVdms_1->setText( "" );
}
