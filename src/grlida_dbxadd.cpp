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

#include "grlida_dbxadd.h"
#include "grlida_addedit_montajes.h"

frmDbxAdd::frmDbxAdd( QDialog *parent, Qt::WFlags flags )
        : QDialog( parent, flags ) 
{
	ui.setupUi(this);

	ui.wizardDbx->setCurrentIndex(0);

	stHomeDir = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir = "./";							// directorio de trabajo del GR-lida

// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		, SIGNAL( clicked() ), this, SLOT( on_btnOk()       ) );
	connect( ui.btnNext		, SIGNAL( clicked() ), this, SLOT( on_btnNext()     ) );
	connect( ui.btnPrevious	, SIGNAL( clicked() ), this, SLOT( on_btnPrevious() ) );

	connect( ui.btnFileConfg, SIGNAL( clicked() ), this, SLOT( on_btnFileConfg() ) );
	connect( ui.btnExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnExeJuego()  ) );
	connect( ui.btnExeSetup , SIGNAL( clicked() ), this, SLOT( on_btnExeSetup()   ) );

	connect( ui.btnMount_Add      , SIGNAL( clicked() ), this, SLOT( on_btnMount_Add()      ) );
	connect( ui.btnMount_Edit     , SIGNAL( clicked() ), this, SLOT( on_btnMount_Edit()     ) );
	connect( ui.btnMount_Delete   , SIGNAL( clicked() ), this, SLOT( on_btnMount_Delete()   ) );
	connect( ui.btnMount_Clear    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Clear()    ) );
	connect( ui.btnMount_Subir    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Subir()    ) );
	connect( ui.btnMount_Bajar    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Bajar()    ) );
	connect( ui.btnMount_AutoCrear, SIGNAL( clicked() ), this, SLOT( on_btnMount_AutoCrear()) );
	connect( ui.btnMount_Primario , SIGNAL( clicked() ), this, SLOT( on_btnMount_Primario() ) );

	ui.twMontajes->header()->setStretchLastSection(true); 
	ui.twMontajes->header()->setMovable(false);


	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt" , ui.cbxDbx_1 , 1, false); // Resolución pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"	  , ui.cbxDbx_2 , 1, false); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"	  , ui.cbxDbx_3 , 1, false); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"	  , ui.cbxDbx_4 , 1, false); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt" 	  , ui.cbxDbx_5 , 1, false); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"	  , ui.cbxDbx_6 , 1, false); // Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt" 	  , ui.cbxDbx_9 , 1, false); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt" 	  , ui.cbxDbx_16, 1, false); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt", ui.cbxDbx_12, 1, false); // MIDI Device

	// Ponemos los Combobox por defecto.
	ui.cbxDbx_1->setCurrentIndex( 0 );	// Resolución pantalla
	ui.cbxDbx_2->setCurrentIndex( 0 );	// Modo de Renderizado
	ui.cbxDbx_3->setCurrentIndex( 0 );	// Tarjeta de Video
	ui.cbxDbx_4->setCurrentIndex( 1 );	// Escalar y Filtros
	ui.cbxDbx_5->setCurrentIndex( 5 );	// Cantidad de memoria para DOSBox
	ui.cbxDbx_6->setCurrentIndex( 0 );	// Ciclos DOSBox
	ui.cbxDbx_9->setCurrentIndex( 5 );	// Tipo Sound Blaste
	ui.cbxDbx_16->setCurrentIndex( 0 ); // MPU-401
	ui.cbxDbx_12->setCurrentIndex( 0 ); // MIDI Device

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmDbxAdd::~frmDbxAdd(){}

void frmDbxAdd::on_btnPrevious()
{
	ui.wizardDbx->setCurrentIndex(ui.wizardDbx->currentIndex()-1);
	ui.btnNext->setEnabled( true );
	if (ui.wizardDbx->currentIndex() < 1 )
		ui.btnPrevious->setEnabled(false) ;
}

void frmDbxAdd::on_btnNext()
{
	bool siguiente;
	if (ui.txtDbx_1->text() != "")
	{
		if ( ui.txtDbx_2->text() != "")
		{
			QFile appConfg( stHomeDir + "confdbx/"+ ui.txtDbx_2->text() );
			if ( appConfg.exists() )
			{
				ui.txtDbx_2->setText("");
				siguiente = false;
				QMessageBox::information( this, stTituloDbx(), tr("El archivo de Configuración para el DOSBox ya esixte"));
			} else {
				siguiente = true;
				if ( ui.txtDbx_3->text() == "")
				{
					QMessageBox::information(this, stTituloDbx(), tr("Debes indicar el Ejecutable del juego"));
					siguiente = false;
				} else
					siguiente = true;			
			}
		} else {
			QMessageBox::information(this, stTituloDbx(), tr("Debes indicar el archivo donde guardara la configuración del juego"));
			siguiente = false;
		}
	} else {
		QMessageBox::information(this, stTituloDbx(), tr("Debes poner un Titulo al juego"));
		siguiente = false;
	}
	if (siguiente == true )
	{
		ui.btnPrevious->setEnabled(true);
		ui.wizardDbx->setCurrentIndex(ui.wizardDbx->currentIndex()+1);
	}
	if( ui.wizardDbx->currentIndex() == 1) ui.chkDbx_4->setFocus();
	if( ui.wizardDbx->currentIndex() == 2) ui.cbxDbx_5->setFocus();
	if( ui.wizardDbx->currentIndex() == 3) ui.chkDbx_10->setFocus();
	if( ui.wizardDbx->currentIndex() == 4)
	{
		ui.btnMount_AutoCrear->click();
		ui.twMontajes->setFocus();		
	}
	if( ui.wizardDbx->currentIndex() >= 5)
	{
		ui.btnOk->setEnabled(true);
	  	ui.btnOk->setFocus();
		ui.btnNext->setEnabled(false);
	}
}

void frmDbxAdd::on_btnOk()
{
	if ( ui.txtDbx_1->text().isEmpty() )
		QMessageBox::information( this, stTituloDbx(), tr("Debes poner por lo menos el titulo."));
	else {
		DatosJuego.clear();
		DatosJuego["icono"]			= "dosbox"		;//icono
		DatosJuego["titulo"]		= ui.txtDbx_1->text();//titulo
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
		DatosJuego["tipo_emu"] 		= "dosbox"		;//tipo_emu
		DatosJuego["comentario"] 	= ""			;//comentario	
		DatosJuego["favorito"]		= "false"		;//favorito

		DatosDosBox.clear();
		if(ui.chkDbx_4->isChecked() )
		  DatosDosBox["sdl_fullscreen"] = "true" ; else DatosDosBox["sdl_fullscreen"] = "false"	; // sdl_fullscreen
	
		DatosDosBox["sdl_fulldouble"] = "false"			; // sdl_fulldouble
	
		if(ui.chkDbx_5->isChecked() )
		  DatosDosBox["sdl_fullfixed"] = "true" ; else DatosDosBox["sdl_fullfixed"] = "false"	; // sdl_fullfixed
	
		if ( ui.cbxDbx_1->currentText()!="" )
		  DatosDosBox["sdl_fullresolution"] = ui.cbxDbx_1->currentText(); else DatosDosBox["sdl_fullresolution"] = "original" ; // sdl_fullresolution
	
		DatosDosBox["sdl_windowresolution"] = "original"; // sdl_windowresolution
	
		if ( ui.cbxDbx_2->currentText()!="" )
		  DatosDosBox["sdl_output"] = ui.cbxDbx_2->currentText(); else DatosDosBox["sdl_output"] = "surface" ; // sdl_output
	
		DatosDosBox["sdl_hwscale"] = "1.00"				; // sdl_hwscale
	
		if(ui.chkDbx_6->isChecked() )
		  DatosDosBox["sdl_autolock"] = "true" ; else DatosDosBox["sdl_autolock"] = "false"	; // sdl_autolock
	
		DatosDosBox["sdl_sensitivity"] = "100"			; // sdl_sensitivity
		DatosDosBox["sdl_waitonerror"] = "true"			; // sdl_waitonerror
		DatosDosBox["sdl_priority"] = "higher,normal"	; // sdl_priority
		DatosDosBox["sdl_mapperfile"] = "mapper.txt"	; // sdl_mapperfile
		DatosDosBox["sdl_usescancodes"] = "true"		; // sdl_usescancodes
		DatosDosBox["dosbox_language"] = ""				; // dosbox_language
		if ( ui.cbxDbx_3->currentText()!="" )
		  DatosDosBox["dosbox_machine"] = ui.cbxDbx_3->currentText(); else DatosDosBox["dosbox_machine"] = "vga"	; // dosbox_machine
		DatosDosBox["dosbox_captures"] = ""				; // dosbox_captures
		if ( ui.cbxDbx_5->currentText()!="" )
		  DatosDosBox["dosbox_memsize"] = ui.cbxDbx_5->currentText(); else DatosDosBox["dosbox_memsize"] = "16"		; // dosbox_memsize
		DatosDosBox["render_frameskip"] = "0"			; // render_frameskip
		if(ui.chkDbx_7->isChecked() )
		  DatosDosBox["render_aspect"] = "true" ; else DatosDosBox["render_aspect"] = "false"	; // render_aspect
		if ( ui.cbxDbx_4->currentText()!="" )
		  DatosDosBox["render_scaler"] = ui.cbxDbx_4->currentText(); else DatosDosBox["render_scaler"] = "normal2x" ; // render_scaler
		DatosDosBox["cpu_core"] = "auto"				; // cpu_core
		if ( ui.cbxDbx_6->currentText()!="" )		
		  DatosDosBox["cpu_cycles"] = ui.cbxDbx_6->currentText(); else DatosDosBox["cpu_cycles"] = "auto" ; // cpu_cycles
		DatosDosBox["cpu_cycleup"] = "500"				; // cpu_cycleup
		DatosDosBox["cpu_cycledown"] = "20"				; // cpu_cycledown
		if(ui.chkDbx_10->isChecked() )
		  DatosDosBox["mixer_nosound"] = "true" ; else DatosDosBox["mixer_nosound"] = "false"	; // mixer_nosound
		if ( ui.cbxDbx_8->currentText()!="" )
		  DatosDosBox["mixer_rate"] = ui.cbxDbx_8->currentText(); else DatosDosBox["mixer_rate"] = "22050" ; // mixer_rate
		DatosDosBox["mixer_blocksize"] = "2048"			; // mixer_blocksize
		DatosDosBox["mixer_prebuffer"] = "10"			; // mixer_prebuffer
		if ( ui.cbxDbx_16->currentText()!="" )
		  DatosDosBox["midi_mpu401"] = ui.cbxDbx_16->currentText(); else DatosDosBox["midi_mpu401"] = "intelligent"; // midi_mpu401
		if(ui.chkDbx_13->isChecked() )
		  DatosDosBox["midi_intelligent"] = "true" ; else DatosDosBox["midi_intelligent"] = "false"	; // midi_intelligent <-DOSBox 0.63
		if ( ui.cbxDbx_12->currentText()!="" )
		  DatosDosBox["midi_device"] = ui.cbxDbx_12->currentText(); else DatosDosBox["midi_device"] = "default"; // midi_device
		DatosDosBox["midi_config"] = ""					; // midi_config
		if ( ui.cbxDbx_13->currentText()!="" )
		  DatosDosBox["midi_mt32rate"] = ui.cbxDbx_13->currentText(); else DatosDosBox["midi_mt32rate"] = "22050"; // midi_mt32rate
		if ( ui.cbxDbx_9->currentText()!="" )
		  DatosDosBox["sblaster_sbtype"] = ui.cbxDbx_9->currentText(); else DatosDosBox["sblaster_sbtype"] = "sb16" ; // sblaster_sbtype
		DatosDosBox["sblaster_sbbase"] = "220"			; // sblaster_sbbase
		DatosDosBox["sblaster_irq"] = "7"				; // sblaster_irq
		DatosDosBox["sblaster_dma"] = "1"				; // sblaster_dma
		DatosDosBox["sblaster_hdma"] = "5"				; // sblaster_hdma
		DatosDosBox["sblaster_mixer"] = "true"			; // sblaster_mixer
		DatosDosBox["sblaster_oplmode"] = "auto"		; // sblaster_oplmode
		if ( ui.cbxDbx_10->currentText()!="" )
		  DatosDosBox["sblaster_oplrate"] = ui.cbxDbx_10->currentText(); else DatosDosBox["sblaster_oplrate"] = "22050" ; // sblaster_oplrate
		if(ui.chkDbx_11->isChecked() )
		  DatosDosBox["gus_gus"] = "true" ; else DatosDosBox["gus_gus"] = "false"	; // gus_gus
		if ( ui.cbxDbx_11->currentText()!="" )
		  DatosDosBox["gus_gusrate"] = ui.cbxDbx_11->currentText(); else DatosDosBox["gus_gusrate"] = "22050" ; // gus_gusrate
		DatosDosBox["gus_gusbase"] = "240"				; // gus_gusbase
		DatosDosBox["gus_irq1"] = "5"					; // gus_irq1
		DatosDosBox["gus_irq2"] = "5"					; // gus_irq2
		DatosDosBox["gus_dma1"] = "3"					; // gus_dma1
		DatosDosBox["gus_dma2"] = "3"					; // gus_dma2
		DatosDosBox["gus_ultradir"] = "C:/ULTRASND"		; // gus_ultradir
		if(ui.chkDbx_14->isChecked() )
		  DatosDosBox["speaker_pcspeaker"] = "true" ; else DatosDosBox["speaker_pcspeaker"] = "false"	; // speaker_pcspeaker
		if ( ui.cbxDbx_14->currentText()!="" )
		  DatosDosBox["speaker_pcrate"] = ui.cbxDbx_14->currentText(); else DatosDosBox["speaker_pcrate"] = "22050" ; // speaker_pcrate
		DatosDosBox["speaker_tandy"] = "auto"													; // speaker_tandy
		if ( ui.cbxDbx_15->currentText()!="" )
		  DatosDosBox["speaker_tandyrate"] = ui.cbxDbx_15->currentText(); else DatosDosBox["speaker_tandyrate"] = "22050" ; // speaker_tandyrate
		if(ui.chkDbx_15->isChecked() )
		  DatosDosBox["speaker_disney"] = "true" ; else DatosDosBox["speaker_disney"] = "false"	; // speaker_disney
		DatosDosBox["joystick_type"] = "auto"			; // joystick_type
		DatosDosBox["joystick_timed"] = "true"			; // joystick_timed
		DatosDosBox["joystick_autofire"] = "false"		; // joystick_autofire
		DatosDosBox["joystick_swap34"] = "false"		; // joystick_swap34
		DatosDosBox["joystick_buttonwrap"] = "true"		; // joystick_buttonwrap
		DatosDosBox["modem_modem"] = "false"			; // modem_modem			<-DOSBox 0.63
		DatosDosBox["modem_comport"] = "2"				; // modem_comport			<-DOSBox 0.63
		DatosDosBox["modem_listenport"] = "23"			; // modem_listenport		<-DOSBox 0.63
		DatosDosBox["dserial_directserial"] = "false"	; // dserial_directserial	<-DOSBox 0.63
		DatosDosBox["dserial_comport"] = "1"			; // dserial_comport		<-DOSBox 0.63
		DatosDosBox["dserial_realport"] = "COM1"		; // dserial_realport		<-DOSBox 0.63
		DatosDosBox["dserial_defaultbps"] = "1200"		; // dserial_defaultbps		<-DOSBox 0.63
		DatosDosBox["dserial_parity"] = "N"				; // dserial_parity			<-DOSBox 0.63
		DatosDosBox["dserial_bytesize"] = "8"			; // dserial_bytesize		<-DOSBox 0.63
		DatosDosBox["dserial_stopbit"] = "1"			; // dserial_stopbit		<-DOSBox 0.63
		DatosDosBox["serial_1"] = "dummy"				; // serial_1
		DatosDosBox["serial_2"] = "dummy"				; // serial_2
		DatosDosBox["serial_3"] = "disabled"			; // serial_3
		DatosDosBox["serial_4"] = "disabled"			; // serial_4
		if(ui.chkDbx_8->isChecked() )
		  DatosDosBox["dos_xms"] = "true" ; else DatosDosBox["dos_xms"] = "false"	; // dos_xms
		if(ui.chkDbx_9->isChecked() )
		  DatosDosBox["dos_ems"] = "true" ; else DatosDosBox["dos_ems"] = "false"	; // dos_ems
		if ( ui.cbxDbx_7->currentText()!="" )
		  DatosDosBox["dos_umb"] = ui.cbxDbx_7->currentText(); else DatosDosBox["dos_umb"] = "true" ; // dos_umb
		DatosDosBox["dos_keyboardlayout"] = "none"									; // dos_keyboardlayout
		DatosDosBox["ipx_ipx"] = "false"									; // ipx_ipx
		DatosDosBox["autoexec"] = ""										; // autoexec
		DatosDosBox["opt_autoexec"] = "false"									; // opt_autoexec
		if(ui.chkDbx_1->isChecked() )
		  DatosDosBox["opt_loadfix"] = "true" ; else DatosDosBox["opt_loadfix"] = "false"	; // opt_loadfix
		DatosDosBox["opt_loadfix_mem"] = "64"									; // opt_loadfix_mem
		if(ui.chkDbx_3->isChecked() )
		  DatosDosBox["opt_consola_dbox"] = "true" ; else DatosDosBox["opt_consola_dbox"] = "false"	; // opt_consola_dbox
		if(ui.chkDbx_2->isChecked() )
		  DatosDosBox["opt_cerrar_dbox"] = "true" ; else DatosDosBox["opt_cerrar_dbox"] = "false"	; // opt_cerrar_dbox
		DatosDosBox["opt_cycle_sincronizar"] = "false"						; // opt_cycle_sincronizar
		DatosDosBox["path_conf"] = ui.txtDbx_2->text()						; // path_conf
		DatosDosBox["path_sonido"] = ""										; // path_sonido
		DatosDosBox["path_exe"] = ui.txtDbx_3->text()						; // path_exe
		DatosDosBox["path_setup"] = ui.txtDbx_5->text()						; // path_setup
		DatosDosBox["parametros_exe"] = ui.txtDbx_4->text()					; // parametros_exe
		DatosDosBox["parametros_setup"] = ui.txtDbx_6->text()				; // parametros_setup
		
		QDialog::accept();
	}
}

void frmDbxAdd::on_txtDbx_1_textChanged(const QString &)
{
	bool str_ok;
	QString str = ui.txtDbx_1->text();
	if(str != "")
	{
		str = fGrl.eliminar_caracteres( str );
  		str_ok = str.endsWith(".conf");
		if(str_ok == false) str.append(".conf");
		ui.txtDbx_2->setText( str );
	} else
		ui.txtDbx_2->setText( "" );
}

void frmDbxAdd::on_btnFileConfg()
{
	bool str_ok;
	QString str, archivo;
	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."),  stHomeDir + "confdbx/", ui.txtDbx_2->text(), tr("Todos los archivo (*)"), 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
//		if ( !appConfg.exists() ) {
			QFileInfo fi( archivo );
			str = fi.fileName();
			str = fGrl.eliminar_caracteres( str );
	  		str_ok = str.endsWith(".conf");
			if(str_ok == false) str.append(".conf");
			ui.txtDbx_2->setText( str );
//		}else{
//			QMessageBox::information( this, stTituloDbx(), tr("El archivo de Configuración para el DOSBox ya esixte"));
//			ui.txtDbx_2->setText("");
//		}
	} else
		ui.txtDbx_2->setText( "" );
}

void frmDbxAdd::on_btnExeJuego()
{
	ui.txtDbx_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_3->text(), tr("Todos los archivo (*)"), 0, false) );
}

void frmDbxAdd::on_btnExeSetup()
{
	ui.txtDbx_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_5->text(), tr("Todos los archivo (*)"), 0, false) );
}

void frmDbxAdd::on_btnMount_Add()
{
	QVector<QString> datos_montaje_edit;

	datos_montaje_edit.clear();
	datos_montaje_edit << ""; // directorio o iso
	datos_montaje_edit << ""; // etiqueta
	datos_montaje_edit << ""; // tipo de montaje
	datos_montaje_edit << ""; // letra de montaje
	datos_montaje_edit << ""; // index de la unidad de cd-rom
	datos_montaje_edit << ""; // opciones del cd-rom
	datos_montaje_edit << ""; // cd/dvd

	frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes(false,datos_montaje_edit);
    if ( AddEditMontajes->exec() == QDialog::Accepted )
    {
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		QString tipoDrive = AddEditMontajes->DatosMontaje[2];
		if(tipoDrive=="drive")
			item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
		if(tipoDrive=="cdrom")
			item->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
		if(tipoDrive=="floppy")
			item->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
		if(tipoDrive=="IMG_floppy")
			item->setIcon( 0, QIcon(":/img16/floppy_1.png") );
		if(tipoDrive=="IMG_iso")
			item->setIcon( 0, QIcon(":/img16/cd_iso.png") );
		if(tipoDrive=="IMG_hdd")
			item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
	
		item->setText( 0 , AddEditMontajes->DatosMontaje[0] );	// directorio o iso
		item->setText( 1 , AddEditMontajes->DatosMontaje[1] );	// etiqueta
		item->setText( 2 , AddEditMontajes->DatosMontaje[2] );	// tipo de montaje
		item->setText( 3 , AddEditMontajes->DatosMontaje[3] );	// letra de montaje
		item->setText( 4 , AddEditMontajes->DatosMontaje[4] );	// index de la unidad de cd-rom
		item->setText( 5 , AddEditMontajes->DatosMontaje[5] );	// opciones del cd-rom
		item->setText( 6 , AddEditMontajes->DatosMontaje[6] );	// cd/dvd
		item->setText( 7 , "x"								);	// primer montaje
		  
		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_3->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
		datos_montaje["opt_loadfix_mem"]= "64";
		if(ui.chkDbx_1->isChecked())
		  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_2->isChecked())
		  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
	  
		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}

void frmDbxAdd::on_btnMount_Edit()
{

	QVector<QString> datos_montaje_edit;
	datos_montaje_edit.clear();
	datos_montaje_edit << ui.twMontajes->currentItem()->text(0); // directorio o iso
	datos_montaje_edit << ui.twMontajes->currentItem()->text(1); // etiqueta
	datos_montaje_edit << ui.twMontajes->currentItem()->text(2); // tipo de montaje
	datos_montaje_edit << ui.twMontajes->currentItem()->text(3); // letra de montaje
	datos_montaje_edit << ui.twMontajes->currentItem()->text(4); // index de la unidad de cd-rom
	datos_montaje_edit << ui.twMontajes->currentItem()->text(5); // opciones del cd-rom
	datos_montaje_edit << ui.twMontajes->currentItem()->text(6); // cd/dvd

	frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes(true, datos_montaje_edit);
    if ( AddEditMontajes->exec() == QDialog::Accepted ) {
		QString tipoDrive = AddEditMontajes->DatosMontaje[2];
		if(tipoDrive=="drive")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_hd.png") );
		if(tipoDrive=="cdrom")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
		if(tipoDrive=="floppy")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
		if(tipoDrive=="IMG_floppy")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/floppy_1.png") );
		if(tipoDrive=="IMG_iso")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/cd_iso.png") );
		if(tipoDrive=="IMG_hdd")
			ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_hd.png") );
		ui.twMontajes->currentItem()->setText( 0 , AddEditMontajes->DatosMontaje[0] );	// directorio o iso
		ui.twMontajes->currentItem()->setText( 1 , AddEditMontajes->DatosMontaje[1] );	// etiqueta
		ui.twMontajes->currentItem()->setText( 2 , AddEditMontajes->DatosMontaje[2] );	// tipo de montaje
		ui.twMontajes->currentItem()->setText( 3 , AddEditMontajes->DatosMontaje[3] );	// letra de montaje
		ui.twMontajes->currentItem()->setText( 4 , AddEditMontajes->DatosMontaje[4] );	// index de la unidad de cd-rom
		ui.twMontajes->currentItem()->setText( 5 , AddEditMontajes->DatosMontaje[5] );	// opciones del cd-rom
		ui.twMontajes->currentItem()->setText( 6 , AddEditMontajes->DatosMontaje[6] );	// cd/dvd
		ui.twMontajes->currentItem()->setText( 7 , "x"								);	// primer montaje
		ui.twMontajes->currentItem()->setText( 8 , "" ); //
		ui.twMontajes->currentItem()->setText( 9 , "" ); //
		  
		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_3->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
		if(ui.chkDbx_1->isChecked())
		  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_2->isChecked())
		  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
		  
		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}

void frmDbxAdd::on_btnMount_Delete()
{
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		int EliminarSiNO;
		EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar Juego...?"), tr( "¿Deseas eliminar este montaje?" ), tr( "Si" ), tr( "Cancelar" ), 0, 1 );
		if( EliminarSiNO == 0 )
		{
			fGrl.DeleteItemTree( ui.twMontajes->currentItem() ) ;
		
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"] = ui.txtDbx_3->text();
			datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
			datos_montaje["opt_loadfix_mem"]= "64";
			if(ui.chkDbx_1->isChecked())
			  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_2->isChecked())
			  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
		
			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
	}
}

void frmDbxAdd::on_btnMount_Clear()
{
	ui.twMontajes->clear();
	ui.previer_mount->clear();
}

void frmDbxAdd::on_btnMount_Subir()
{
	
}

void frmDbxAdd::on_btnMount_Bajar()
{
	
}

void frmDbxAdd::on_btnMount_AutoCrear()
{
	QFileInfo fi( ui.txtDbx_3->text() );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
	item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
	item->setText( 0 , fi.absolutePath());	// directorio o iso
	item->setText( 1 , ""				);	// etiqueta
	item->setText( 2 , "drive"			);	// tipo de montaje
	item->setText( 3 , "C"				);	// letra de montaje
	item->setText( 4 , ""				);	// index de la unidad de cd-rom
	item->setText( 5 , "-usecd 0"		);	// opciones del cd-rom
	item->setText( 6 , "-aspi"			);	// cd/dvd IOCtrl
	item->setText( 7 , "x"				);	// primer montaje

	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"] = ui.txtDbx_3->text();
	datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
	datos_montaje["opt_loadfix_mem"]= "64";
	if(ui.chkDbx_1->isChecked())
	  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
	if(ui.chkDbx_2->isChecked())
	  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
	  
	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmDbxAdd::on_btnMount_Primario()
{
	int indx_mount=0, num_mount=0;
	indx_mount = ui.twMontajes->indexOfTopLevelItem(ui.twMontajes->currentItem());
	for ( num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
	{
		ui.twMontajes->topLevelItem( num_mount )->setText(7 , "x");
	}
	ui.twMontajes->topLevelItem( indx_mount )->setText(7 , "v");
	
	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"] = ui.txtDbx_3->text();
	datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
	datos_montaje["opt_loadfix_mem"] = "64";
	if(ui.chkDbx_1->isChecked())
	  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
	if(ui.chkDbx_2->isChecked())
	  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}
