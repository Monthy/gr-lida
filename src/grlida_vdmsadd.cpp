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
#include "grlida_importar_juego.h"

frmVdmsAdd::frmVdmsAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir      = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stConfgVdmSDir = stHomeDir + "confvdms/";	// directorio de configuracion para el VDMSound
	stTheme        = fGrl.ThemeGrl();

	createConnections();

	CargarConfig();

	setTheme();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmVdmsAdd::~frmVdmsAdd()
{
	//
}

void frmVdmsAdd::createConnections()
{
// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		     , SIGNAL( clicked() ), this, SLOT( on_btnOk()             ) );
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono	 , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );
	connect( ui.btnDescargarInfo , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo()  ) );
}

void frmVdmsAdd::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/vdmsound.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnVdms_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_Icono->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_FileConfg_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_ExeJuego_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_Icono_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDescargarInfo->setIcon( QIcon(stTheme+"img16/go-down.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

void frmVdmsAdd::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	tempDatosJuego["Dat_graficos"]        = "0";
	tempDatosJuego["Dat_sonido"]          = "0";
	tempDatosJuego["Dat_jugabilidad"]     = "0";
	tempDatosJuego["Dat_favorito"]        = "false";
	tempDatosJuego["Dat_rating"]          = "0";
	tempDatosJuego["Dat_edad_recomendada"]= "nd";
}

void frmVdmsAdd::on_btnDescargarInfo()
{
	frmImportarJuego * ImportarJuego = new frmImportarJuego(ui.txtDatos_Titulo->text(), 0, Qt::Window);
	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		tempDatosJuego.clear();
		ui.txtDatos_Titulo->setText( ImportarJuego->DatosJuego["Dat_titulo"] );				// titulo
		tempDatosJuego["Dat_icono"]         = "vdmsound";									// icono
		tempDatosJuego["Dat_titulo"]        = ui.txtDatos_Titulo->text();					// titulo
		tempDatosJuego["Dat_subtitulo"]     = ImportarJuego->DatosJuego["Dat_subtitulo"];	// subtitulo
		tempDatosJuego["Dat_genero"]        = ImportarJuego->DatosJuego["Dat_genero"];		// genero
		tempDatosJuego["Dat_compania"]      = ImportarJuego->DatosJuego["Dat_compania"];	// compania
		tempDatosJuego["Dat_desarrollador"] = ImportarJuego->DatosJuego["Dat_desarrollador"];//desarrollador
		tempDatosJuego["Dat_tema"]          = ImportarJuego->DatosJuego["Dat_tema"];		// tema
		tempDatosJuego["Dat_perspectiva"]   = ImportarJuego->DatosJuego["Dat_perspectiva"];	// perspectiva
		tempDatosJuego["Dat_idioma"]        = ImportarJuego->DatosJuego["Dat_idioma"];		// idioma
		tempDatosJuego["Dat_idioma_voces"]  = ImportarJuego->DatosJuego["Dat_idioma_voces"];// idioma_voces
		tempDatosJuego["Dat_formato"]       = ImportarJuego->DatosJuego["Dat_formato"];		// formato
		tempDatosJuego["Dat_anno"]          = ImportarJuego->DatosJuego["Dat_anno"];		// anno
		tempDatosJuego["Dat_numdisc"]       = ImportarJuego->DatosJuego["Dat_numdisc"];		// numdisc
		tempDatosJuego["Dat_sistemaop"]     = ImportarJuego->DatosJuego["Dat_sistemaop"];	// sistemaop
		tempDatosJuego["Dat_tamano"]        = ImportarJuego->DatosJuego["Dat_tamano"];		// tamano
		tempDatosJuego["Dat_graficos"]      = ImportarJuego->DatosJuego["Dat_graficos"];	// graficos
		tempDatosJuego["Dat_sonido"]        = ImportarJuego->DatosJuego["Dat_sonido"];		// sonido
		tempDatosJuego["Dat_jugabilidad"]   = ImportarJuego->DatosJuego["Dat_jugabilidad"];	// jugabilidad
		tempDatosJuego["Dat_original"]      = ImportarJuego->DatosJuego["Dat_original"];	// original
		tempDatosJuego["Dat_estado"]        = ImportarJuego->DatosJuego["Dat_estado"];		// estado
		tempDatosJuego["Dat_thumbs"]        = ImportarJuego->DatosJuego["Dat_thumbs"];		// thumbs
		tempDatosJuego["Dat_cover_front"]   = ImportarJuego->DatosJuego["Dat_cover_front"];	// cover_front
		tempDatosJuego["Dat_cover_back"]    = ImportarJuego->DatosJuego["Dat_cover_back"];	// cover_back
		tempDatosJuego["Dat_tipo_emu"]      = "vdmsound";									// tipo_emu
		tempDatosJuego["Dat_comentario"]    = ImportarJuego->DatosJuego["Dat_comentario"];	// comentario
		tempDatosJuego["Dat_favorito"]      = ImportarJuego->DatosJuego["Dat_favorito"];	// favorito
		tempDatosJuego["Dat_rating"]        = ImportarJuego->DatosJuego["Dat_rating"];		// rating
		tempDatosJuego["Dat_edad_recomendada"] = ImportarJuego->DatosJuego["Dat_edad_recomendada"];	// edad_recomendada
		tempDatosJuego["Dat_usuario"]          = ImportarJuego->DatosJuego["Dat_usuario"];			// usuario
		tempDatosJuego["Dat_path_exe"]         = ImportarJuego->DatosJuego["Dat_path_exe"];			// path_exe
		tempDatosJuego["Dat_parametros_exe"]   = ImportarJuego->DatosJuego["Dat_parametros_exe"];	// parametros_exe
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
		DatosJuego["Dat_icono"]         = "vdmsound";								// icono
		DatosJuego["Dat_titulo"]        = ui.txtDatos_Titulo->text();				// titulo
		DatosJuego["Dat_subtitulo"]     = ""+tempDatosJuego["Dat_subtitulo"];		// subtitulo
		DatosJuego["Dat_genero"]        = ""+tempDatosJuego["Dat_genero"];			// genero
		DatosJuego["Dat_compania"]      = ""+tempDatosJuego["Dat_compania"];		// compania
		DatosJuego["Dat_desarrollador"] = ""+tempDatosJuego["Dat_desarrollador"];	// desarrollador
		DatosJuego["Dat_tema"]          = ""+tempDatosJuego["Dat_tema"];			// tema
		DatosJuego["Dat_perspectiva"]   = ""+tempDatosJuego["Dat_perspectiva"];		// perspectiva
		DatosJuego["Dat_idioma"]        = ""+tempDatosJuego["Dat_idioma"];			// idioma
		DatosJuego["Dat_idioma_voces"]  = ""+tempDatosJuego["Dat_idioma_voces"];	// idioma_voces
		DatosJuego["Dat_formato"]       = ""+tempDatosJuego["Dat_formato"];			// formato
		DatosJuego["Dat_anno"]          = ""+tempDatosJuego["Dat_anno"];			// anno
		DatosJuego["Dat_numdisc"]       = ""+tempDatosJuego["Dat_numdisc"];			// numdisc
		DatosJuego["Dat_sistemaop"]     = ""+tempDatosJuego["Dat_sistemaop"];		// sistemaop
		DatosJuego["Dat_tamano"]        = ""+tempDatosJuego["Dat_tamano"];			// tamano
		DatosJuego["Dat_graficos"]      = ""+tempDatosJuego["Dat_graficos"];		// graficos
		DatosJuego["Dat_sonido"]        = ""+tempDatosJuego["Dat_sonido"];			// sonido
		DatosJuego["Dat_jugabilidad"]   = ""+tempDatosJuego["Dat_jugabilidad"];		// jugabilidad
		DatosJuego["Dat_original"]      = ""+tempDatosJuego["Dat_original"];		// original
		DatosJuego["Dat_estado"]        = ""+tempDatosJuego["Dat_estado"];			// estado
		DatosJuego["Dat_thumbs"]        = ""+tempDatosJuego["Dat_thumbs"];			// thumbs
		DatosJuego["Dat_cover_front"]   = ""+tempDatosJuego["Dat_cover_front"];		// cover_front
		DatosJuego["Dat_cover_back"]    = ""+tempDatosJuego["Dat_cover_back"];		// cover_back
		DatosJuego["Dat_fecha"]         = fGrl.HoraFechaActual(GRLConfig["FormatoFecha"].toString());// fecha d/m/a h:m:s
		DatosJuego["Dat_tipo_emu"]      = "vdmsound";								// tipo_emu
		DatosJuego["Dat_comentario"]    = ""+tempDatosJuego["Dat_comentario"];		// comentario
		DatosJuego["Dat_favorito"]      = ""+tempDatosJuego["Dat_favorito"];		// favorito
		DatosJuego["Dat_rating"]        = ""+tempDatosJuego["Dat_rating"];			// rating
		DatosJuego["Dat_edad_recomendada"] = ""+tempDatosJuego["Dat_edad_recomendada"];	// edad_recomendada
		DatosJuego["Dat_usuario"]          = ""+tempDatosJuego["Dat_usuario"];			// usuario
		DatosJuego["Dat_path_exe"]         = ""+tempDatosJuego["Dat_path_exe"];			// path_exe
		DatosJuego["Dat_parametros_exe"]   = ""+tempDatosJuego["Dat_parametros_exe"];	// parametros_exe

		DatosVDMSound.clear();
		DatosVDMSound["Vdms_path_conf"]       = ui.txtVdms_path_conf->text();
		DatosVDMSound["Vdms_path_exe"]        = ui.txtVdms_path_exe->text();
		DatosVDMSound["Vdms_program_1"]       = ui.txtVdms_params->text();
		DatosVDMSound["Vdms_program_2"]       = ui.txtVdms_icon->text();
		DatosVDMSound["Vdms_vdms_debug_1"]    = "no";
		DatosVDMSound["Vdms_vdms_debug_2"]    = "";
		DatosVDMSound["Vdms_winnt_dos_1"]     = "no";
		DatosVDMSound["Vdms_winnt_dos_2"]     = "";
		DatosVDMSound["Vdms_winnt_dosbox_1"]  = "yes";
		DatosVDMSound["Vdms_winnt_dosbox_2"]  = "no";
		DatosVDMSound["Vdms_winnt_dosbox_3"]  = "no";
		DatosVDMSound["Vdms_winnt_storage_1"] = "yes";
		DatosVDMSound["Vdms_winnt_storage_2"] = "no";

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
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_path_exe"].toString(), ui.txtVdms_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtVdms_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Vdms_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Vdms_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_path_exe", GRLConfig["Vdms_path_exe"].toString() );
}

void frmVdmsAdd::on_btnVdms_Icono()
{
	ui.txtVdms_icon->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_icon"].toString(), ui.txtVdms_icon->text(), tr("Todos los archivo") + " (*)", 0, false)  );//+ ",0"

	QFileInfo fi( ui.txtVdms_icon->text() );
	if( fi.exists() )
		GRLConfig["Vdms_icon"] = fi.absolutePath();
	else
		GRLConfig["Vdms_icon"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_icon", GRLConfig["Vdms_icon"].toString() );
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
