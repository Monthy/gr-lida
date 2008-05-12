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

#include "grlida_exportar_juego.h"

frmExportarJuego::frmExportarJuego(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
// Conecta los distintos botones
	connect( ui.btnOk           , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.btnCheckedAll   , SIGNAL( clicked() ), this, SLOT( on_btnCheckedAll() ) );
	connect( ui.btnUnCheckedAll , SIGNAL( clicked() ), this, SLOT( on_btnUnCheckedAll() ) );
	connect( ui.btnDirExportPath, SIGNAL( clicked() ), this, SLOT( on_btnDirExportPath() ) );
	
	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stIconDir = stHomeDir + "iconos/";	// directorio de iconos para el GR-lida

	CargarListaJuegos("dosbox");

	ui.cbxExpotarComo->clear();
	ui.cbxExpotarComo->addItem("D-Fend Reloaded (*.prof)"); // Index 0
	ui.cbxExpotarComo->addItem("GR-lida (*.xml)");          // Index 1
	ui.cbxExpotarComo->setCurrentIndex( 1 );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmExportarJuego::~frmExportarJuego(){}

void frmExportarJuego::on_btnOk()
{
	QDir dir_export( ui.txtDirExportPath->text() );

	if( !ui.txtDirExportPath->text().isEmpty() && dir_export.exists() )
	{
		bool str_ok;
		QString archivo;

		int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );

		if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
		{
			ui.gBox_OptExportar->setEnabled( false );
			for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
			{
				QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
				if( item->checkState(0) == Qt::Checked )
				{
					CargarDatosExportar( item->text(1) );

					QFileInfo fi( item->text(0) );
					archivo = fi.completeBaseName();
					archivo = fGrl.eliminar_caracteres( archivo );
					
					switch ( ui.cbxExpotarComo->currentIndex() )
					{
						case 0: // D-Fend Reloaded
							str_ok = archivo.endsWith(".prof");
							if(str_ok == false)
								archivo.append(".prof");
		
							fGrl.Exportar_Profile_DFend( TempDatosJuego, TempDatosDosBox, twMontajes, ui.txtDirExportPath->text() + "/" + archivo);
						break;
						case 1: // GR-lida
							str_ok = archivo.endsWith(".xml");
							if(str_ok == false)
								archivo.append(".xml");

							fGrl.Exportar_Profile_GRLida( TempDatosJuego, TempDatosDosBox, twMontajes, ui.txtDirExportPath->text() + "/" + archivo);
						break;
					}
				}
			}
			ui.gBox_OptExportar->setEnabled( true );
		}
	}
}

void frmExportarJuego::on_btnCheckedAll()
{
	int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );
	if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
	{
		for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
		{
			QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
			item->setCheckState(0, Qt::Checked);
		}
	}
}

void frmExportarJuego::on_btnUnCheckedAll()
{
	int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );
	if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
	{
		for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
		{
			QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
			item->setCheckState(0, Qt::Unchecked);
		}
	}
}

void frmExportarJuego::on_btnDirExportPath()
{
	ui.txtDirExportPath->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), ui.txtDirExportPath->text(), ui.txtDirExportPath->text() ));
}

void frmExportarJuego::CargarListaJuegos(QString TipoEmu, QString stdb_Orden_By, QString stdb_Orden)
{
	QString stIcono, stSqlWhere;
	QSqlQuery query;
	QSqlRecord rec;

	if( !TipoEmu.isEmpty() )
		stSqlWhere = " WHERE tipo_emu LIKE '%"+TipoEmu+"%'";
	else
		stSqlWhere.clear();

	query.exec("SELECT * FROM dbgrl "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+stdb_Orden);

	if( query.first() )
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item = new QTreeWidgetItem( ui.twListaJuegos );
			item->setText( 0 , query.value( rec.indexOf("titulo") ).toString() );
			item->setCheckState(0, Qt::Unchecked);
			item->setText( 1 , fGrl.IntToStr( query.value( rec.indexOf("idgrl") ).toInt() ) );	// titulo
			item->setText( 2 , query.value( rec.indexOf("tipo_emu") ).toString()            );	// tipo_emu

			stIcono = query.value( rec.indexOf("icono") ).toString();	// icono
			if( stIcono.isEmpty() )
				item->setIcon( 0, QIcon(":/img24/emu_sin_imagen.png") );
			else if( stIcono == "datos" ) 
				item->setIcon( 0, QIcon(":/img24/emu_datos.png") );
			else if( stIcono == "dosbox" )
				item->setIcon( 0, QIcon(":/img24/emu_dbx.png") );
			else if( stIcono == "scummvm" )
				item->setIcon( 0, QIcon(":/img24/emu_svm.png") );
			else if( stIcono == "vdmsound" ) 
				item->setIcon( 0, QIcon(":/img24/emu_vdms.png") );
			else {
				bool existeIcono;
				existeIcono = QFile::exists(stIconDir + stIcono);
				if( existeIcono )
					item->setIcon( 0, QIcon(stIconDir + stIcono) );
				else
					item->setIcon( 0, QIcon(":/img24/emu_sin_imagen.png") );
			}
		} while (query.next());
		ui.twListaJuegos->setCurrentItem( ui.twListaJuegos->itemAt(0,0) );
	}
}

void frmExportarJuego::CargarDatosExportar( QString stIDx )
{
	if( !stIDx.isEmpty() )
	{
		TempDatosDosBox.clear();
		TempDatosJuego.clear();
		QSqlQuery query;

		TempDatosJuego  = sql->show_Datos( stIDx );
		TempDatosDosBox = sql->showConfg_DOSBox( stIDx );
/*
		query.exec("SELECT * FROM dbgrl WHERE idgrl="+stIDx+" LIMIT 0,1");
		if( query.first() )
		{
			rec = query.record();

			TempDatosDosBox["Icon"]        = query.value( rec.indexOf("icono") ).toString();		// icono
			TempDatosDosBox["Name"]        = query.value( rec.indexOf("titulo") ).toString();		// titulo
			TempDatosDosBox["subtitulo"]   = query.value( rec.indexOf("subtitulo") ).toString();	// subtitulo
			TempDatosDosBox["Genre"]       = query.value( rec.indexOf("genero") ).toString();		// genero
			TempDatosDosBox["Publisher"]   = query.value( rec.indexOf("compania") ).toString();		// compania
			TempDatosDosBox["Developer"]   = query.value( rec.indexOf("desarrollador") ).toString();// desarrollador
			TempDatosDosBox["tema"]        = query.value( rec.indexOf("tema") ).toString();			// tema
			TempDatosDosBox["Language"]    = query.value( rec.indexOf("idioma") ).toString();		// idioma
			TempDatosDosBox["formato"]     = query.value( rec.indexOf("formato") ).toString();		// formato
			TempDatosDosBox["Year"]        = query.value( rec.indexOf("anno") ).toString();			// anno
			TempDatosDosBox["numdisc"]     = query.value( rec.indexOf("numdisc") ).toString();		// numdisc
			TempDatosDosBox["sistemaop"]   = query.value( rec.indexOf("sistemaop") ).toString();	// sistemaop
			TempDatosDosBox["tamano"]      = query.value( rec.indexOf("tamano") ).toString();		// tamano
			TempDatosDosBox["graficos"]    = query.value( rec.indexOf("graficos") ).toString();		// graficos
			TempDatosDosBox["sonido"]      = query.value( rec.indexOf("sonido") ).toString();		// sonido
			TempDatosDosBox["jugabilidad"] = query.value( rec.indexOf("jugabilidad") ).toString();	// jugabilidad
			TempDatosDosBox["original"]    = query.value( rec.indexOf("original") ).toString();		// original
			TempDatosDosBox["estado"]      = query.value( rec.indexOf("estado") ).toString();		// estado
			TempDatosDosBox["thumbs"]      = query.value( rec.indexOf("thumbs") ).toString();		// thumbs
			TempDatosDosBox["cover_front"] = query.value( rec.indexOf("cover_front") ).toString();	// cover_front
			TempDatosDosBox["cover_back"]  = query.value( rec.indexOf("cover_back") ).toString();	// cover_back
			TempDatosDosBox["tipo_emu"]    = query.value( rec.indexOf("tipo_emu") ).toString();		// tipo_emu
			TempDatosDosBox["Notes"]       = query.value( rec.indexOf("comentario") ).toString();	// comentario
			TempDatosDosBox["Favorite"]    = query.value( rec.indexOf("favorito") ).toString();		// favorito
			//ProfileDFend["WWW"]       = query.value( rec.indexOf("WWW") ).toString();
			//ProfileDFend["UserInfo"]  = query.value( rec.indexOf("UserInfo") ).toString();
		}
		
		query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stIDx+" LIMIT 0,1");
		if( query.first() )
		{
			rec = query.record();

			TempDatosDosBox["sdl_fullscreen"]       = query.value( rec.indexOf("sdl_fullscreen") ).toString();			// sdl_fullscreen
			TempDatosDosBox["sdl_fulldouble"]       = query.value( rec.indexOf("sdl_fulldouble") ).toString();			// sdl_fulldouble
			TempDatosDosBox["sdl_fullfixed"]        = query.value( rec.indexOf("sdl_fullfixed") ).toString();			// sdl_fullfixed
			TempDatosDosBox["sdl_fullresolution"]   = query.value( rec.indexOf("sdl_fullresolution" )).toString();		// sdl_fullresolution
			TempDatosDosBox["sdl_windowresolution"] = query.value( rec.indexOf("sdl_windowresolution") ).toString();	// sdl_windowresolution
			TempDatosDosBox["sdl_output"]           = query.value( rec.indexOf("sdl_output") ).toString();				// sdl_output
			TempDatosDosBox["sdl_hwscale"]          = query.value( rec.indexOf("sdl_hwscale") ).toString();				// sdl_hwscale
			TempDatosDosBox["sdl_autolock"]         = query.value( rec.indexOf("sdl_autolock") ).toString();			// sdl_autolock
			TempDatosDosBox["sdl_sensitivity"]      = query.value( rec.indexOf("sdl_sensitivity") ).toString();			// sdl_sensitivity
			TempDatosDosBox["sdl_waitonerror"]      = query.value( rec.indexOf("sdl_waitonerror") ).toString();			// sdl_waitonerror
			TempDatosDosBox["sdl_priority"]         = query.value( rec.indexOf("sdl_priority") ).toString();			// sdl_priority

			TempDatosDosBox["sdl_mapperfile"]   = query.value( rec.indexOf("sdl_mapperfile") ).toString();			// sdl_mapperfile
			TempDatosDosBox["sdl_usescancodes"] = query.value( rec.indexOf("sdl_usescancodes") ).toString();		// sdl_usescancodes

			TempDatosDosBox["dosbox_language"] = query.value( rec.indexOf("dosbox_language") ).toString();			// dosbox_language
			TempDatosDosBox["dosbox_machine"]  = query.value( rec.indexOf("dosbox_machine") ).toString();			// dosbox_machine
			TempDatosDosBox["dosbox_captures"] = query.value( rec.indexOf("dosbox_captures") ).toString();			// dosbox_captures
			TempDatosDosBox["dosbox_memsize"]  = query.value( rec.indexOf("dosbox_memsize") ).toString();			// dosbox_memsize

			TempDatosDosBox["render_frameskip"] = query.value( rec.indexOf("render_frameskip") ).toString();		// render_frameskip
			TempDatosDosBox["render_aspect"]    = query.value( rec.indexOf("render_aspect") ).toString();			// render_aspect
			TempDatosDosBox["render_scaler"]    = query.value( rec.indexOf("render_scaler") ).toString();			// render_scaler

			TempDatosDosBox["cpu_core"]      = query.value( rec.indexOf("cpu_core") ).toString();					// cpu_core
			TempDatosDosBox["cpu_cycles"]    = query.value( rec.indexOf("cpu_cycles") ).toString();					// cpu_cycles
			TempDatosDosBox["cpu_cycleup"]   = query.value( rec.indexOf("cpu_cycleup") ).toString() ;				// cpu_cycleup
			TempDatosDosBox["cpu_cycledown"] = query.value( rec.indexOf("cpu_cycledown") ).toString();				// cpu_cycledown

			TempDatosDosBox["mixer_nosound"]   = query.value( rec.indexOf("mixer_nosound") ).toString();			// mixer_nosound
			TempDatosDosBox["mixer_rate"]      = query.value( rec.indexOf("mixer_rate") ).toString();				// mixer_rate
			TempDatosDosBox["mixer_blocksize"] = query.value( rec.indexOf("mixer_blocksize") ).toString();			// mixer_blocksize
			TempDatosDosBox["mixer_prebuffer"] = query.value( rec.indexOf("mixer_prebuffer") ).toString();			// mixer_prebuffer

			TempDatosDosBox["midi_mpu401"]      = query.value( rec.indexOf("midi_mpu401") ).toString();				// midi_mpu401
			TempDatosDosBox["midi_intelligent"] = query.value( rec.indexOf("midi_intelligent") ).toString();		// midi_intelligent
			TempDatosDosBox["midi_device"]      = query.value( rec.indexOf("midi_device") ).toString();				// midi_device
			TempDatosDosBox["midi_config"]      = query.value( rec.indexOf("midi_config") ).toString();				// midi_config
			TempDatosDosBox["midi_mt32rate"]    = query.value( rec.indexOf("midi_mt32rate") ).toString();			// midi_mt32rate

			TempDatosDosBox["sblaster_sbtype"]  = query.value( rec.indexOf("sblaster_sbtype") ).toString();			// sblaster_sbtype
			TempDatosDosBox["sblaster_sbbase"]  = query.value( rec.indexOf("sblaster_sbbase") ).toString();			// sblaster_sbbase
			TempDatosDosBox["sblaster_irq"]     = query.value( rec.indexOf("sblaster_irq") ).toString();			// sblaster_irq
			TempDatosDosBox["sblaster_dma"]     = query.value( rec.indexOf("sblaster_dma") ).toString();			// sblaster_dma
			TempDatosDosBox["sblaster_hdma"]    = query.value( rec.indexOf("sblaster_hdma") ).toString();			// sblaster_hdma
			TempDatosDosBox["sblaster_mixer"]   = query.value( rec.indexOf("sblaster_mixer") ).toString();			// sblaster_mixer
			TempDatosDosBox["sblaster_oplmode"] = query.value( rec.indexOf("sblaster_oplmode") ).toString();		// sblaster_oplmode
			TempDatosDosBox["sblaster_oplrate"] = query.value( rec.indexOf("sblaster_oplrate") ).toString();		// sblaster_oplrate

			TempDatosDosBox["gus_gus"]      = query.value( rec.indexOf("gus_gus") ).toString();						// gus_gus
			TempDatosDosBox["gus_gusrate"]  = query.value( rec.indexOf("gus_gusrate") ).toString();					// gus_gusrate
			TempDatosDosBox["gus_gusbase"]  = query.value( rec.indexOf("gus_gusbase") ).toString();					// gus_gusbase
			TempDatosDosBox["gus_irq1"]     = query.value( rec.indexOf("gus_irq1") ).toString();					// gus_irq1
			TempDatosDosBox["gus_irq2"]     = query.value( rec.indexOf("gus_irq2") ).toString();					// gus_irq2
			TempDatosDosBox["gus_dma1"]     = query.value( rec.indexOf("gus_dma1") ).toString();					// gus_dma1
			TempDatosDosBox["gus_dma2"]     = query.value( rec.indexOf("gus_dma2") ).toString();					// gus_dma2
			TempDatosDosBox["gus_ultradir"] = query.value( rec.indexOf("gus_ultradir") ).toString();				// gus_ultradir

			TempDatosDosBox["speaker_pcspeaker"] = query.value( rec.indexOf("speaker_pcspeaker") ).toString();		// speaker_pcspeaker
			TempDatosDosBox["speaker_pcrate"]    = query.value( rec.indexOf("speaker_pcrate") ).toString();			// speaker_pcrate
			TempDatosDosBox["speaker_tandy"]     = query.value( rec.indexOf("speaker_tandy") ).toString();			// speaker_tandy
			TempDatosDosBox["speaker_tandyrate"] = query.value( rec.indexOf("speaker_tandyrate") ).toString();		// speaker_tandyrate
			TempDatosDosBox["speaker_disney"]    = query.value( rec.indexOf("speaker_disney") ).toString();			// speaker_disney

			TempDatosDosBox["joystick_type"]       = query.value( rec.indexOf("joystick_type") ).toString();		// joystick_type
			TempDatosDosBox["joystick_timed"]      = query.value( rec.indexOf("joystick_timed") ).toString();		// joystick_timed
			TempDatosDosBox["joystick_autofire"]   = query.value( rec.indexOf("joystick_autofire") ).toString();	// joystick_autofire
			TempDatosDosBox["joystick_swap34"]     = query.value( rec.indexOf("joystick_swap34") ).toString();		// joystick_swap34
			TempDatosDosBox["joystick_buttonwrap"] = query.value( rec.indexOf("joystick_buttonwrap") ).toString();	// joystick_buttonwrap

			TempDatosDosBox["modem_modem"]      = query.value( rec.indexOf("modem_modem") ).toString();				// modem_modem
			TempDatosDosBox["modem_comport"]    = query.value( rec.indexOf("modem_comport") ).toString();			// modem_comport
			TempDatosDosBox["modem_listenport"] = query.value( rec.indexOf("modem_listenport") ).toString();		// modem_listenport

			TempDatosDosBox["dserial_directserial"] = query.value( rec.indexOf("dserial_directserial") ).toString();// dserial_directserial
			TempDatosDosBox["dserial_comport"]      = query.value( rec.indexOf("dserial_comport") ).toString();		// dserial_comport
			TempDatosDosBox["dserial_realport"]     = query.value( rec.indexOf("dserial_realport") ).toString();	// serial_realport
			TempDatosDosBox["dserial_defaultbps"]   = query.value( rec.indexOf("dserial_defaultbps") ).toString();	// dserial_defaultbps
			TempDatosDosBox["dserial_parity"]       = query.value( rec.indexOf("dserial_parity") ).toString();		// dserial_parity
			TempDatosDosBox["dserial_bytesize"]     = query.value( rec.indexOf("dserial_bytesize") ).toString();	// dserial_bytesize
			TempDatosDosBox["dserial_stopbit"]      = query.value( rec.indexOf("dserial_stopbit") ).toString();		// dserial_stopbit
			TempDatosDosBox["serial_1"]             = query.value( rec.indexOf("serial_1") ).toString();			// serial_1
			TempDatosDosBox["serial_2"]             = query.value( rec.indexOf("serial_2") ).toString();			// serial_2
			TempDatosDosBox["serial_3"]             = query.value( rec.indexOf("serial_3") ).toString();			// serial_3
			TempDatosDosBox["serial_4"]             = query.value( rec.indexOf("serial_4") ).toString();			// serial_4

			TempDatosDosBox["dos_xms"] = query.value( rec.indexOf("dos_xms") ).toString();							// dos_xms
			TempDatosDosBox["dos_ems"] = query.value( rec.indexOf("dos_ems") ).toString();							// dos_ems
			TempDatosDosBox["dos_umb"] = query.value( rec.indexOf("dos_umb") ).toString();							// dos_umb
			TempDatosDosBox["dos_keyboardlayout"] = query.value( rec.indexOf("dos_keyboardlayout") ).toString();	// dos_keyboardlayout

			TempDatosDosBox["ipx_ipx"] = query.value( rec.indexOf("ipx_ipx") ).toString();								// ipx_ipx

			TempDatosDosBox["autoexec"]     = query.value( rec.indexOf("autoexec") ).toString();						// autoexec
			TempDatosDosBox["opt_autoexec"] = query.value( rec.indexOf("opt_autoexec") ).toString();					// opt_autoexec

			TempDatosDosBox["opt_loadfix"]           = query.value( rec.indexOf("opt_loadfix") ).toString();			// opt_loadfix
			TempDatosDosBox["opt_loadfix_mem"]       = query.value( rec.indexOf("opt_loadfix_mem") ).toString();		// opt_loadfix_mem
			TempDatosDosBox["opt_consola_dbox"]      = query.value( rec.indexOf("opt_consola_dbox") ).toString();		// opt_consola_dbox
			TempDatosDosBox["opt_cerrar_dbox"]       = query.value( rec.indexOf("opt_cerrar_dbox") ).toString();		// opt_cerrar_dbox
			TempDatosDosBox["opt_cycle_sincronizar"] = query.value( rec.indexOf("opt_cycle_sincronizar") ).toString();	// opt_cycle_sincronizar

			TempDatosDosBox["path_conf"]        = query.value( rec.indexOf("path_conf") ).toString();					// path_conf
			TempDatosDosBox["path_sonido"]      = query.value( rec.indexOf("path_sonido") ).toString();					// path_sonido
			TempDatosDosBox["path_exe"]         = query.value( rec.indexOf("path_exe") ).toString();					// path_exe
			TempDatosDosBox["path_setup"]       = query.value( rec.indexOf("path_setup") ).toString();					// path_setup
			TempDatosDosBox["parametros_exe"]   = query.value( rec.indexOf("parametros_exe") ).toString();				// parametros_exe
			TempDatosDosBox["parametros_setup"] = query.value( rec.indexOf("parametros_setup") ).toString();			// parametros_setup
		}
*/
		twMontajes = new QTreeWidget();
		twMontajes->clear();		
		
		query.exec("SELECT * FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+stIDx+" ORDER BY id_lista");
		if( query.first() )
		{
			do {
				QTreeWidgetItem *item_mount = new QTreeWidgetItem( twMontajes );

				item_mount->setText( 0 , query.record().value("path").toString() );			// path			- directorio o iso
				item_mount->setText( 1 , query.record().value("label").toString() );		// label		- etiqueta
				item_mount->setText( 2 , query.record().value("tipo_as").toString() );		// tipo_as		- tipo de montaje
				item_mount->setText( 3 , query.record().value("letter").toString() );		// letter		- letra de montaje
				item_mount->setText( 4 , query.record().value("indx_cd").toString() );		// indx_cd		- index de la unidad de cd-rom
				item_mount->setText( 5 , query.record().value("opt_mount").toString() );	// opt_mount	- opciones del cd-rom
				item_mount->setText( 6 , query.record().value("io_ctrl").toString() );		// io_ctrl		- cd/dvd
				item_mount->setText( 7 , query.record().value("select_mount").toString());	// select_mount	- primer montaje
				item_mount->setText( 8 , query.record().value("id").toString() );			// id
				item_mount->setText( 9 , query.record().value("id_lista").toString() );		// id_lista		- id_lista

			} while (query.next());
		}
	}
}
