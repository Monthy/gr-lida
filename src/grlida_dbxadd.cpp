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

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida

// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		, SIGNAL( clicked() ), this, SLOT( on_btnOk()       ) );
	connect( ui.btnNext		, SIGNAL( clicked() ), this, SLOT( on_btnNext()     ) );
	connect( ui.btnPrevious	, SIGNAL( clicked() ), this, SLOT( on_btnPrevious() ) );

	connect( ui.btnDbx_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnDbx_FileConfg() ) );
	connect( ui.btnDbx_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeJuego()  ) );
	connect( ui.btnDbx_ExeSetup , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeSetup()  ) );

	connect( ui.btnMount_Add      , SIGNAL( clicked() ), this, SLOT( on_btnMount_Add()      ) );
	connect( ui.btnMount_Edit     , SIGNAL( clicked() ), this, SLOT( on_btnMount_Edit()     ) );
	connect( ui.btnMount_Delete   , SIGNAL( clicked() ), this, SLOT( on_btnMount_Delete()   ) );
	connect( ui.btnMount_Clear    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Clear()    ) );
	connect( ui.btnMount_Subir    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Subir()    ) );
	connect( ui.btnMount_Bajar    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Bajar()    ) );
	connect( ui.btnMount_AutoCrear, SIGNAL( clicked() ), this, SLOT( on_btnMount_AutoCrear()) );
	connect( ui.btnMount_Primario , SIGNAL( clicked() ), this, SLOT( on_btnMount_Primario() ) );
	connect( ui.cbxDbx_Profiles   , SIGNAL( activated(const QString &) ), this, SLOT( on_setProfileGame(const QString &) ) );

	ui.twMontajes->header()->setStretchLastSection(true);
	ui.twMontajes->header()->setMovable(false);

	fGrl.Cargar_Profile_DFend_ComboBox(stHomeDir + "templates/", ui.cbxDbx_Profiles);
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt" , ui.cbxDbx_sdl_fullresolution , 1, false); // Resolución pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"     , ui.cbxDbx_sdl_output         , 1, false); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"    , ui.cbxDbx_dosbox_machine     , 1, false); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"     , ui.cbxDbx_render_scaler      , 1, false); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt"    , ui.cbxDbx_dosbox_memsize     , 1, false); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"     , ui.cbxDbx_cpu_cycles         , 1, false); // Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt"     , ui.cbxDbx_sblaster_sbtype    , 1, false); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt"     , ui.cbxDbx_midi_mpu401        , 1, false); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt", ui.cbxDbx_midi_device        , 1, false); // MIDI Device
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_core.txt"   , ui.cbxDbx_cpu_core           , 1, false); // Ncleo de la CPU DOSBox

	// Ponemos los Combobox por defecto.
	ui.cbxDbx_Profiles->setCurrentIndex( 0 );
	ui.cbxDbx_sdl_fullresolution->setCurrentIndex( 0 );	// Resolución pantalla
	ui.cbxDbx_sdl_output->setCurrentIndex( 0 );			// Modo de Renderizado
	ui.cbxDbx_dosbox_machine->setCurrentIndex( 0 );		// Tarjeta de Video
	ui.cbxDbx_render_scaler->setCurrentIndex( 1 );		// Escalar y Filtros
	ui.cbxDbx_dosbox_memsize->setCurrentIndex( 5 );		// Cantidad de memoria para DOSBox
	ui.cbxDbx_cpu_cycles->setCurrentIndex( 0 );			// Ciclos DOSBox
	ui.cbxDbx_sblaster_sbtype->setCurrentIndex( 5 );	// Tipo Sound Blaste
	ui.cbxDbx_midi_mpu401->setCurrentIndex( 0 );		// MPU-401
	ui.cbxDbx_midi_device->setCurrentIndex( 0 );		// MIDI Device

	on_setProfileGame( ui.cbxDbx_Profiles->currentText() );

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
	if(ui.wizardDbx->currentIndex() < 1 )
		ui.btnPrevious->setEnabled(false);
}

void frmDbxAdd::on_btnNext()
{
	bool siguiente;
	if( !ui.txtDatos_Titulo->text().isEmpty() )
	{
		if( !ui.txtDbx_path_conf->text().isEmpty() )
		{
			QFile appConfg( stHomeDir + "confdbx/"+ ui.txtDbx_path_conf->text() );
			if( appConfg.exists() )
			{
				ui.txtDbx_path_conf->setText("");
				siguiente = false;
				QMessageBox::information( this, stTituloDbx(), tr("El archivo de Configuración para el DOSBox ya esixte"));
				ui.txtDbx_path_conf->setFocus();
			} else {
				siguiente = true;
				if( ui.txtDbx_path_exe->text().isEmpty() )
				{
					QMessageBox::information(this, stTituloDbx(), tr("Debes indicar el Ejecutable del juego"));
					siguiente = false;
					ui.txtDbx_path_exe->setFocus();
				} else
					siguiente = true;
			}
		} else {
			QMessageBox::information(this, stTituloDbx(), tr("Debes indicar el archivo donde guardara la configuración del juego"));
			siguiente = false;
			ui.txtDbx_path_conf->setFocus();
		}
	} else {
		QMessageBox::information(this, stTituloDbx(), tr("Debes poner un Titulo al juego"));
		siguiente = false;
		ui.txtDatos_Titulo->setFocus();
	}
	if( siguiente == true )
	{
		ui.btnPrevious->setEnabled(true);
		ui.wizardDbx->setCurrentIndex(ui.wizardDbx->currentIndex()+1);
	}
	if( ui.wizardDbx->currentIndex() == 1 ) ui.chkDbx_sdl_fullscreen->setFocus();
	if( ui.wizardDbx->currentIndex() == 2 ) ui.cbxDbx_dosbox_memsize->setFocus();
	if( ui.wizardDbx->currentIndex() == 3 ) ui.chkDbx_mixer_nosound->setFocus();
	if( ui.wizardDbx->currentIndex() == 4 ) ui.twMontajes->setFocus();

	if( ui.wizardDbx->currentIndex() >= 5 )
	{
		ui.btnOk->setEnabled( true );
		ui.btnOk->setFocus();
		ui.btnNext->setEnabled( false );
	}
}

void frmDbxAdd::on_setProfileGame(const QString ProfileGame)
{
	if(!ProfileGame.isEmpty() || ProfileGame!="<defecto>")
	{
		QStringList str_Lista;
		QString strProfile, tipoDrive, temp_opt_mount;

		strProfile.clear();
		strProfile = stHomeDir + "templates/" + ProfileGame + ".prof";

		TempProfileDosBox.clear();
		TempProfileDosBox = fGrl.Importar_Profile_DFend( strProfile );

		ui.chkDbx_sdl_fullscreen->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_fullscreen"] ) );										// sdl_fullscreen
		ui.chkDbx_sdl_fullfixed->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_fullfixed"] ) );										// sdl_fullfixed
		ui.cbxDbx_sdl_fullresolution->setCurrentIndex( ui.cbxDbx_sdl_fullresolution->findText( TempProfileDosBox["sdl_fullresolution"] ) );	// sdl_fullresolution
		ui.cbxDbx_sdl_output->setCurrentIndex( ui.cbxDbx_sdl_output->findText( TempProfileDosBox["sdl_output"] ) );							// sdl_output
		ui.chkDbx_sdl_autolock->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_autolock"] ) );											// sdl_autolock

		ui.cbxDbx_dosbox_machine->setCurrentIndex( ui.cbxDbx_dosbox_machine->findText( TempProfileDosBox["dosbox_machine"] ) );			// dosbox_machine
		ui.cbxDbx_dosbox_memsize->setEditText(TempProfileDosBox["dosbox_memsize"] );

		ui.chkDbx_render_aspect->setChecked( fGrl.StrToBool( TempProfileDosBox["render_aspect"] ) );									// render_aspect
		ui.cbxDbx_render_scaler->setCurrentIndex( ui.cbxDbx_render_scaler->findText( TempProfileDosBox["render_scaler"] ) );			// render_scaler

		ui.cbxDbx_cpu_core->setCurrentIndex( ui.cbxDbx_cpu_core->findText( TempProfileDosBox["cpu_core"] ) );							// cpu_core
		ui.cbxDbx_cpu_cycles->setCurrentIndex( ui.cbxDbx_cpu_cycles->findText( TempProfileDosBox["cpu_cycles"] ) );						// cpu_cycles

		ui.chkDbx_mixer_nosound->setChecked( fGrl.StrToBool( TempProfileDosBox["mixer_nosound"] ) );									// mixer_nosound
		ui.cbxDbx_mixer_rate->setCurrentIndex( ui.cbxDbx_mixer_rate->findText( TempProfileDosBox["mixer_rate"] ) );						// mixer_rate

		ui.cbxDbx_midi_mpu401->setCurrentIndex( ui.cbxDbx_midi_mpu401->findText( TempProfileDosBox["midi_mpu401"] ) );					// midi_mpu401
		ui.chkDbx_midi_intelligent->setChecked( fGrl.StrToBool( TempProfileDosBox["midi_intelligent"] ) );								// midi_intelligent
		ui.cbxDbx_midi_device->setCurrentIndex( ui.cbxDbx_midi_device->findText( TempProfileDosBox["midi_device"] ) );					// midi_device
		ui.cbxDbx_midi_mt32rate->setCurrentIndex( ui.cbxDbx_midi_mt32rate->findText( TempProfileDosBox["midi_mt32rate"] ) );				// midi_mt32rate

		ui.cbxDbx_sblaster_sbtype->setCurrentIndex( ui.cbxDbx_sblaster_sbtype->findText( TempProfileDosBox["sblaster_sbtype"] ) );		// sblaster_sbtype
		ui.cbxDbx_sblaster_oplrate->setCurrentIndex( ui.cbxDbx_sblaster_oplrate->findText( TempProfileDosBox["sblaster_oplrate"] ) );	// sblaster_oplrate

		ui.chkDbx_gus_gus->setChecked( fGrl.StrToBool( TempProfileDosBox["gus_gus"] ) );												// gus_gus
		ui.cbxDbx_gus_gusrate->setCurrentIndex( ui.cbxDbx_gus_gusrate->findText( TempProfileDosBox["gus_gusrate"] ) );					// gus_gusrate

		ui.chkDbx_speaker_pcspeaker->setChecked( fGrl.StrToBool( TempProfileDosBox["speaker_pcspeaker"] ) );							// speaker_pcspeaker
		ui.cbxDbx_speaker_pcrate->setCurrentIndex( ui.cbxDbx_speaker_pcrate->findText( TempProfileDosBox["speaker_pcrate"] ) );			// speaker_pcrate
		ui.cbxDbx_speaker_tandyrate->setCurrentIndex( ui.cbxDbx_speaker_tandyrate->findText( TempProfileDosBox["speaker_tandyrate"] ) );// speaker_tandyrate
		ui.chkDbx_speaker_disney->setChecked( fGrl.StrToBool( TempProfileDosBox["speaker_disney"] ) );									// speaker_disney

		ui.chkDbx_dos_xms->setChecked( fGrl.StrToBool( TempProfileDosBox["dos_xms"] ) );						// dos_xms
		ui.chkDbx_dos_ems->setChecked( fGrl.StrToBool( TempProfileDosBox["dos_ems"] ) );						// dos_ems
		ui.cbxDbx_dos_umb->setCurrentIndex( ui.cbxDbx_dos_umb->findText( TempProfileDosBox["dos_umb"] ) );		// dos_umb

		ui.chkDbx_loadfix->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_loadfix"] ) );					// opt_loadfix
		ui.chkDbx_consola_dbox->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_consola_dbox"] ) );			// opt_consola_dbox
		ui.chkDbx_cerrar_dbox->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_cerrar_dbox"] ) );			// opt_cerrar_dbox

		if( !TempProfileDosBox["path_exe"].isEmpty() )
			ui.txtDbx_path_exe->setText( TempProfileDosBox["path_exe"] );					// path_exe

		if( !TempProfileDosBox["path_setup"].isEmpty() )
			ui.txtDbx_path_setup->setText( TempProfileDosBox["path_setup"] );				// path_setup

		ui.txtDbx_parametros_exe->setText( TempProfileDosBox["parametros_exe"] );			// parametros_exe
		ui.txtDbx_parametros_setup->setText( TempProfileDosBox["parametros_setup"] );		// parametros_setup

		ui.twMontajes->clear();		
		ui.btnMount_AutoCrear->click();
/*
		int num_mounts = fGrl.StrToInt( TempProfileDosBox["NrOfMounts"] );
		if( num_mounts > 0 )
		{
			for(int i=0; i < num_mounts ; i++)
			{
				QTreeWidgetItem *twItemDfend = new QTreeWidgetItem(ui.twMontajes);

				str_Lista.clear();
				str_Lista = TempProfileDosBox[fGrl.IntToStr(i)].split("|");

				tipoDrive.clear();
				tipoDrive = str_Lista.value(1).toLower();// tipo_as

				if(tipoDrive=="drive")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_hd.png") );
					tipoDrive = "drive";
				}
				if(tipoDrive=="cdrom")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
					tipoDrive = "cdrom";
				}
				if(tipoDrive=="floppy")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
					tipoDrive = "floppy";
				}
				if(tipoDrive=="floppyimage")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/floppy_1.png") );
					tipoDrive = "IMG_floppy";
				}
				if(tipoDrive=="cdromimage")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/cd_iso.png") );
					tipoDrive = "IMG_iso";
				}
				if(tipoDrive=="image")
				{
					twItemDfend->setIcon(0, QIcon(":/img16/drive_hd.png") );
					tipoDrive = "IMG_hdd";
				}
				temp_opt_mount.clear();
				temp_opt_mount = str_Lista.value(5);

				twItemDfend->setText(0, ""+str_Lista.value(0) ); // path
				twItemDfend->setText(1, ""+str_Lista.value(4) ); // label
				twItemDfend->setText(2, ""+tipoDrive          ); // tipo_as
				twItemDfend->setText(3, ""+str_Lista.value(2) ); // letter
				twItemDfend->setText(4, ""+fGrl.IntToStr(i)   ); // indx_cd
				twItemDfend->setText(5, ""+temp_opt_mount     ); // opt_mount
				twItemDfend->setText(6, ""+str_Lista.value(3) ); // io_ctrl
				twItemDfend->setText(7, "x"                   ); // select_mount
			}
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
			datos_montaje["opt_loadfix_mem"] = "64";
			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_cerrar_dbox->isChecked())
				datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";

			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
*/
	} else {
		TempProfileDosBox["sdl_fulldouble"]        = "false";		// sdl_fulldouble
		TempProfileDosBox["sdl_windowresolution"]  = "original";	// sdl_windowresolution
		TempProfileDosBox["sdl_hwscale"]           = "1.00";		// sdl_hwscale
		TempProfileDosBox["sdl_sensitivity"]       = "100";			// sdl_sensitivity
		TempProfileDosBox["sdl_waitonerror"]       = "true";		// sdl_waitonerror
		TempProfileDosBox["sdl_priority"]          = "higher,normal";// sdl_priority
		TempProfileDosBox["sdl_mapperfile"]        = "mapper.txt";	// sdl_mapperfile
		TempProfileDosBox["sdl_usescancodes"]      = "true";		// sdl_usescancodes
		TempProfileDosBox["dosbox_language"]       = "";			// dosbox_language
		TempProfileDosBox["dosbox_captures"]       = "capture";		// dosbox_captures
		TempProfileDosBox["render_frameskip"]      = "0";			// render_frameskip
		TempProfileDosBox["cpu_cycleup"]           = "500";			// cpu_cycleup
		TempProfileDosBox["cpu_cycledown"]         = "20";			// cpu_cycledown
		TempProfileDosBox["mixer_blocksize"]       = "2048";		// mixer_blocksize
		TempProfileDosBox["mixer_prebuffer"]       = "10";			// mixer_prebuffer
		TempProfileDosBox["midi_config"]           = "";			// midi_config
		TempProfileDosBox["sblaster_sbbase"]       = "220";			// sblaster_sbbase
		TempProfileDosBox["sblaster_irq"]          = "7";			// sblaster_irq
		TempProfileDosBox["sblaster_dma"]          = "1";			// sblaster_dma
		TempProfileDosBox["sblaster_hdma"]         = "5";			// sblaster_hdma
		TempProfileDosBox["sblaster_mixer"]        = "true";		// sblaster_mixer
		TempProfileDosBox["sblaster_oplmode"]      = "auto";		// sblaster_oplmode
		TempProfileDosBox["gus_gusbase"]           = "240";			// gus_gusbase
		TempProfileDosBox["gus_irq1"]              = "5";			// gus_irq1
		TempProfileDosBox["gus_irq2"]              = "5";			// gus_irq2
		TempProfileDosBox["gus_dma1"]              = "3";			// gus_dma1
		TempProfileDosBox["gus_dma2"]              = "3";			// gus_dma2
		TempProfileDosBox["gus_ultradir"]          = "C:\\ULTRASND";// gus_ultradir
		TempProfileDosBox["speaker_tandy"]         = "auto";		// speaker_tandy
		TempProfileDosBox["joystick_type"]         = "auto";		// joystick_type
		TempProfileDosBox["joystick_timed"]        = "true";		// joystick_timed
		TempProfileDosBox["joystick_autofire"]     = "false";		// joystick_autofire
		TempProfileDosBox["joystick_swap34"]       = "false";		// joystick_swap34
		TempProfileDosBox["joystick_buttonwrap"]   = "true";		// joystick_buttonwrap
		TempProfileDosBox["modem_modem"]           = "true";		// modem_modem
		TempProfileDosBox["modem_comport"]         = "1";			// modem_comport
		TempProfileDosBox["modem_listenport"]      = "23";			// modem_listenport
		TempProfileDosBox["dserial_directserial"]  = "true";		// dserial_directserial
		TempProfileDosBox["dserial_comport"]       = "1";			// dserial_comport
		TempProfileDosBox["dserial_realport"]      = "COM1";		// serial_realport
		TempProfileDosBox["dserial_defaultbps"]    = "1200";		// dserial_defaultbps
		TempProfileDosBox["dserial_parity"]        = "N";			// dserial_parity
		TempProfileDosBox["dserial_bytesize"]      = "8";			// dserial_bytesize
		TempProfileDosBox["dserial_stopbit"]       = "1";			// dserial_stopbit
		TempProfileDosBox["serial_1"]              = "dummy";		// serial_1
		TempProfileDosBox["serial_2"]              = "dummy";		// serial_2
		TempProfileDosBox["serial_3"]              = "disabled";	// serial_3
		TempProfileDosBox["serial_4"]              = "disabled";	// serial_4
		TempProfileDosBox["dos_keyboardlayout"]    = "none";		// dos_keyboardlayout
		TempProfileDosBox["ipx_ipx"]               = "false";		// ipx_ipx
		TempProfileDosBox["autoexec"]              = "";			// autoexec
		TempProfileDosBox["opt_autoexec"]          = "false";		// opt_autoexec
		TempProfileDosBox["opt_loadfix_mem"]       = "64";			// opt_loadfix_mem
		TempProfileDosBox["opt_cycle_sincronizar"] = "false";		// opt_cycle_sincronizar
		TempProfileDosBox["path_sonido"]           = "";			// path_sonido
	}
}

void frmDbxAdd::on_btnOk()
{
	if( ui.txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information( this, stTituloDbx(), tr("Debes poner por lo menos el titulo."));
	else {
		DatosJuego.clear();
		DatosJuego["icono"]			= "dosbox"		;//icono
		DatosJuego["titulo"]		= ui.txtDatos_Titulo->text();//titulo
		DatosJuego["subtitulo"]		= ""			;//subtitulo
		DatosJuego["genero"]		= ""			;//genero
		DatosJuego["compania"]		= ""			;//compania
		DatosJuego["desarrollador"]	= ""			;//desarrollador
		DatosJuego["tema"]			= ""			;//tema
		DatosJuego["idioma"]		= ""			;//idioma
		DatosJuego["formato"]		= ""			;//formato
		DatosJuego["anno"]			= ""			;//anno
		DatosJuego["numdisc"]		= ""			;//numdisc
		DatosJuego["sistemaop"]		= ""			;//sistemaop
		DatosJuego["tamano"]		= ""			;//tamano
		DatosJuego["graficos"]		= "1"			;//graficos
		DatosJuego["sonido"]		= "1"			;//sonido
		DatosJuego["jugabilidad"]	= "1"			;//jugabilidad
		DatosJuego["original"]		= "false"		;//original
		DatosJuego["estado"]		= ""			;//estado
		DatosJuego["thumbs"]		= ""			;//thumbs
		DatosJuego["cover_front"]	= ""			;//cover_front
		DatosJuego["cover_back"]	= ""			;//cover_back
		DatosJuego["fecha"]			= fGrl.HoraFechaActual();//fecha d/m/a h:m:s
		DatosJuego["tipo_emu"]		= "dosbox"		;//tipo_emu
		DatosJuego["comentario"]	= ""			;//comentario
		DatosJuego["favorito"]		= "false"		;//favorito

		DatosDosBox.clear();
	// sdl_fullscreen
		if(ui.chkDbx_sdl_fullscreen->isChecked() )
			DatosDosBox["sdl_fullscreen"] = "true" ; else DatosDosBox["sdl_fullscreen"] = "false";
	// sdl_fulldouble
		DatosDosBox["sdl_fulldouble"] = TempProfileDosBox["sdl_fulldouble"];
	// sdl_fullfixed
		if(ui.chkDbx_sdl_fullfixed->isChecked() )
			DatosDosBox["sdl_fullfixed"] = "true" ; else DatosDosBox["sdl_fullfixed"] = "false";
	// sdl_fullresolution
		if( ui.cbxDbx_sdl_fullresolution->currentText()!="" )
			DatosDosBox["sdl_fullresolution"] = ui.cbxDbx_sdl_fullresolution->currentText(); else DatosDosBox["sdl_fullresolution"] = "original";
	// sdl_windowresolution
		DatosDosBox["sdl_windowresolution"] = TempProfileDosBox["sdl_windowresolution"];
	// sdl_output
		if( ui.cbxDbx_sdl_output->currentText()!="" )
			DatosDosBox["sdl_output"] = ui.cbxDbx_sdl_output->currentText(); else DatosDosBox["sdl_output"] = "surface";

		DatosDosBox["sdl_hwscale"] = TempProfileDosBox["sdl_hwscale"];	// sdl_hwscale

	// sdl_autolock
		if(ui.chkDbx_sdl_autolock->isChecked() )
			DatosDosBox["sdl_autolock"] = "true" ; else DatosDosBox["sdl_autolock"] = "false";

		DatosDosBox["sdl_sensitivity"]  = TempProfileDosBox["sdl_sensitivity"];			// sdl_sensitivity
		DatosDosBox["sdl_waitonerror"]  = TempProfileDosBox["sdl_waitonerror"];			// sdl_waitonerror
		DatosDosBox["sdl_priority"]     = TempProfileDosBox["sdl_priority"];	// sdl_priority
		DatosDosBox["sdl_mapperfile"]   = TempProfileDosBox["sdl_mapperfile"];		// sdl_mapperfile
		DatosDosBox["sdl_usescancodes"] = TempProfileDosBox["sdl_usescancodes"];			// sdl_usescancodes
		DatosDosBox["dosbox_language"]  = TempProfileDosBox["dosbox_language"];				// dosbox_language

	// dosbox_machine
		if( ui.cbxDbx_dosbox_machine->currentText()!="" )
			DatosDosBox["dosbox_machine"] = ui.cbxDbx_dosbox_machine->currentText(); else DatosDosBox["dosbox_machine"] = "vga";
	// dosbox_captures
		DatosDosBox["dosbox_captures"] = TempProfileDosBox["dosbox_captures"];
	// dosbox_memsize
		if( ui.cbxDbx_dosbox_memsize->currentText()!="" )
			DatosDosBox["dosbox_memsize"] = ui.cbxDbx_dosbox_memsize->currentText(); else DatosDosBox["dosbox_memsize"] = "16";

		DatosDosBox["render_frameskip"] = TempProfileDosBox["render_frameskip"];	// render_frameskip

	// render_aspect
		if(ui.chkDbx_render_aspect->isChecked() )
			DatosDosBox["render_aspect"] = "true" ; else DatosDosBox["render_aspect"] = "false";
	// render_scaler
		if( ui.cbxDbx_render_scaler->currentText()!="" )
			DatosDosBox["render_scaler"] = ui.cbxDbx_render_scaler->currentText(); else DatosDosBox["render_scaler"] = "normal2x";
	// cpu_core
		if( ui.cbxDbx_cpu_core->currentText()!="" )
			DatosDosBox["cpu_core"] = ui.cbxDbx_cpu_core->currentText(); else DatosDosBox["cpu_core"] = "auto";
	// cpu_cycles
		if( ui.cbxDbx_cpu_cycles->currentText()!="" )
			DatosDosBox["cpu_cycles"] = ui.cbxDbx_cpu_cycles->currentText(); else DatosDosBox["cpu_cycles"] = "auto";

		DatosDosBox["cpu_cycleup"]   = TempProfileDosBox["cpu_cycleup"];	// cpu_cycleup
		DatosDosBox["cpu_cycledown"] = TempProfileDosBox["cpu_cycledown"];	// cpu_cycledown

	// mixer_nosound
		if(ui.chkDbx_mixer_nosound->isChecked() )
			DatosDosBox["mixer_nosound"] = "true" ; else DatosDosBox["mixer_nosound"] = "false";
	// mixer_rate
		if( ui.cbxDbx_mixer_rate->currentText()!="" )
			DatosDosBox["mixer_rate"] = ui.cbxDbx_mixer_rate->currentText(); else DatosDosBox["mixer_rate"] = "22050";

		DatosDosBox["mixer_blocksize"] = TempProfileDosBox["mixer_blocksize"];	// mixer_blocksize
		DatosDosBox["mixer_prebuffer"] = TempProfileDosBox["mixer_prebuffer"];		// mixer_prebuffer

	// midi_mpu401
		if( ui.cbxDbx_midi_mpu401->currentText()!="" )
			DatosDosBox["midi_mpu401"] = ui.cbxDbx_midi_mpu401->currentText(); else DatosDosBox["midi_mpu401"] = "intelligent";
	// midi_intelligent <-DOSBox 0.63
		if(ui.chkDbx_midi_intelligent->isChecked() )
			DatosDosBox["midi_intelligent"] = "true" ; else DatosDosBox["midi_intelligent"] = "false";
	// midi_device
		if( ui.cbxDbx_midi_device->currentText()!="" )
			DatosDosBox["midi_device"] = ui.cbxDbx_midi_device->currentText(); else DatosDosBox["midi_device"] = "default";

		DatosDosBox["midi_config"] = TempProfileDosBox["midi_config"];	// midi_config

	// midi_mt32rate
		if( ui.cbxDbx_midi_mt32rate->currentText()!="" )
			DatosDosBox["midi_mt32rate"] = ui.cbxDbx_midi_mt32rate->currentText(); else DatosDosBox["midi_mt32rate"] = "22050";
	// sblaster_sbtype
		if( ui.cbxDbx_sblaster_sbtype->currentText()!="" )
			DatosDosBox["sblaster_sbtype"] = ui.cbxDbx_sblaster_sbtype->currentText(); else DatosDosBox["sblaster_sbtype"] = "sb16";

		DatosDosBox["sblaster_sbbase"]  = TempProfileDosBox["sblaster_sbbase"];		// sblaster_sbbase
		DatosDosBox["sblaster_irq"]     = TempProfileDosBox["sblaster_irq"];		// sblaster_irq
		DatosDosBox["sblaster_dma"]     = TempProfileDosBox["sblaster_dma"];		// sblaster_dma
		DatosDosBox["sblaster_hdma"]    = TempProfileDosBox["sblaster_hdma"];		// sblaster_hdma
		DatosDosBox["sblaster_mixer"]   = TempProfileDosBox["sblaster_mixer"];		// sblaster_mixer
		DatosDosBox["sblaster_oplmode"] = TempProfileDosBox["sblaster_oplmode"];	// sblaster_oplmode

	// sblaster_oplrate
		if( ui.cbxDbx_sblaster_oplrate->currentText()!="" )
			DatosDosBox["sblaster_oplrate"] = ui.cbxDbx_sblaster_oplrate->currentText(); else DatosDosBox["sblaster_oplrate"] = "22050";
	// gus_gus
		if(ui.chkDbx_gus_gus->isChecked() )
			DatosDosBox["gus_gus"] = "true" ; else DatosDosBox["gus_gus"] = "false";
	// gus_gusrate
		if( ui.cbxDbx_gus_gusrate->currentText()!="" )
			DatosDosBox["gus_gusrate"] = ui.cbxDbx_gus_gusrate->currentText(); else DatosDosBox["gus_gusrate"] = "22050";

		DatosDosBox["gus_gusbase"]  = TempProfileDosBox["gus_gusbase"];	// gus_gusbase
		DatosDosBox["gus_irq1"]     = TempProfileDosBox["gus_irq1"];	// gus_irq1
		DatosDosBox["gus_irq2"]     = TempProfileDosBox["gus_irq2"];	// gus_irq2
		DatosDosBox["gus_dma1"]     = TempProfileDosBox["gus_dma1"];	// gus_dma1
		DatosDosBox["gus_dma2"]     = TempProfileDosBox["gus_dma2"];	// gus_dma2
		DatosDosBox["gus_ultradir"] = TempProfileDosBox["gus_ultradir"];// gus_ultradir

	// speaker_pcspeaker
		if(ui.chkDbx_speaker_pcspeaker->isChecked() )
			DatosDosBox["speaker_pcspeaker"] = "true" ; else DatosDosBox["speaker_pcspeaker"] = "false";
	// speaker_pcrate
		if( ui.cbxDbx_speaker_pcrate->currentText()!="" )
			DatosDosBox["speaker_pcrate"] = ui.cbxDbx_speaker_pcrate->currentText(); else DatosDosBox["speaker_pcrate"] = "22050";

		DatosDosBox["speaker_tandy"] = TempProfileDosBox["speaker_tandy"];	// speaker_tandy

	// speaker_tandyrate
		if( ui.cbxDbx_speaker_tandyrate->currentText()!="" )
			DatosDosBox["speaker_tandyrate"] = ui.cbxDbx_speaker_tandyrate->currentText(); else DatosDosBox["speaker_tandyrate"] = "22050";
	// speaker_disney
		if(ui.chkDbx_speaker_disney->isChecked() )
			DatosDosBox["speaker_disney"] = "true" ; else DatosDosBox["speaker_disney"] = "false";

		DatosDosBox["joystick_type"]        = TempProfileDosBox["joystick_type"];		// joystick_type
		DatosDosBox["joystick_timed"]       = TempProfileDosBox["joystick_timed"];		// joystick_timed
		DatosDosBox["joystick_autofire"]    = TempProfileDosBox["joystick_autofire"];	// joystick_autofire
		DatosDosBox["joystick_swap34"]      = TempProfileDosBox["joystick_swap34"];		// joystick_swap34
		DatosDosBox["joystick_buttonwrap"]  = TempProfileDosBox["joystick_buttonwrap"];	// joystick_buttonwrap
		DatosDosBox["modem_modem"]          = TempProfileDosBox["modem_modem"];			// modem_modem			<-DOSBox 0.63
		DatosDosBox["modem_comport"]        = TempProfileDosBox["modem_comport"];		// modem_comport		<-DOSBox 0.63
		DatosDosBox["modem_listenport"]     = TempProfileDosBox["modem_listenport"];	// modem_listenport		<-DOSBox 0.63
		DatosDosBox["dserial_directserial"] = TempProfileDosBox["dserial_directserial"];// dserial_directserial	<-DOSBox 0.63
		DatosDosBox["dserial_comport"]      = TempProfileDosBox["dserial_comport"];		// dserial_comport		<-DOSBox 0.63
		DatosDosBox["dserial_realport"]     = TempProfileDosBox["dserial_realport"];	// dserial_realport		<-DOSBox 0.63
		DatosDosBox["dserial_defaultbps"]   = TempProfileDosBox["dserial_defaultbps"];	// dserial_defaultbps	<-DOSBox 0.63
		DatosDosBox["dserial_parity"]       = TempProfileDosBox["dserial_parity"];		// dserial_parity		<-DOSBox 0.63
		DatosDosBox["dserial_bytesize"]     = TempProfileDosBox["dserial_bytesize"];	// dserial_bytesize		<-DOSBox 0.63
		DatosDosBox["dserial_stopbit"]      = TempProfileDosBox["dserial_stopbit"];		// dserial_stopbit		<-DOSBox 0.63
		DatosDosBox["serial_1"]             = TempProfileDosBox["serial_1"];			// serial_1
		DatosDosBox["serial_2"]             = TempProfileDosBox["serial_2"];			// serial_2
		DatosDosBox["serial_3"]             = TempProfileDosBox["serial_3"];			// serial_3
		DatosDosBox["serial_4"]             = TempProfileDosBox["serial_4"];			// serial_4

	// dos_xms
		if(ui.chkDbx_dos_xms->isChecked() )
			DatosDosBox["dos_xms"] = "true" ; else DatosDosBox["dos_xms"] = "false";
	// dos_ems
		if(ui.chkDbx_dos_ems->isChecked() )
			DatosDosBox["dos_ems"] = "true" ; else DatosDosBox["dos_ems"] = "false";
	// dos_umb
		if( ui.cbxDbx_dos_umb->currentText()!="" )
			DatosDosBox["dos_umb"] = ui.cbxDbx_dos_umb->currentText(); else DatosDosBox["dos_umb"] = "true";

		DatosDosBox["dos_keyboardlayout"] = TempProfileDosBox["dos_keyboardlayout"];// dos_keyboardlayout
		DatosDosBox["ipx_ipx"]            = TempProfileDosBox["ipx_ipx"];			// ipx_ipx
		DatosDosBox["autoexec"]           = TempProfileDosBox["autoexec"];			// autoexec
		DatosDosBox["opt_autoexec"]       = TempProfileDosBox["opt_autoexec"];		// opt_autoexec

	// opt_loadfix
		if(ui.chkDbx_loadfix->isChecked() )
			DatosDosBox["opt_loadfix"] = "true" ; else DatosDosBox["opt_loadfix"] = "false";

		DatosDosBox["opt_loadfix_mem"] = TempProfileDosBox["opt_loadfix_mem"];	// opt_loadfix_mem

	// opt_consola_dbox
		if(ui.chkDbx_consola_dbox->isChecked() )
			DatosDosBox["opt_consola_dbox"] = "true" ; else DatosDosBox["opt_consola_dbox"] = "false";
	// opt_cerrar_dbox
		if(ui.chkDbx_cerrar_dbox->isChecked() )
			DatosDosBox["opt_cerrar_dbox"] = "true" ; else DatosDosBox["opt_cerrar_dbox"] = "false";

		DatosDosBox["opt_cycle_sincronizar"] = TempProfileDosBox["opt_cycle_sincronizar"];	// opt_cycle_sincronizar
		DatosDosBox["path_conf"]             = ui.txtDbx_path_conf->text();					// path_conf
		DatosDosBox["path_sonido"]           = TempProfileDosBox["path_sonido"];			// path_sonido
		DatosDosBox["path_exe"]              = ui.txtDbx_path_exe->text();					// path_exe
		DatosDosBox["path_setup"]            = ui.txtDbx_path_setup->text();				// path_setup
		DatosDosBox["parametros_exe"]        = ui.txtDbx_parametros_exe->text();			// parametros_exe
		DatosDosBox["parametros_setup"]      = ui.txtDbx_parametros_setup->text();			// parametros_setup

		QDialog::accept();
	}
}

void frmDbxAdd::on_txtDatos_Titulo_textChanged(const QString &)
{
	bool str_ok;
	QString str = ui.txtDatos_Titulo->text();
	if(str != "")
	{
		str = fGrl.eliminar_caracteres( str );
		str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");
		ui.txtDbx_path_conf->setText( str );
	} else
		ui.txtDbx_path_conf->setText( "" );
}

void frmDbxAdd::on_btnDbx_FileConfg()
{
	bool str_ok;
	QString str, archivo;
	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."),  stHomeDir + "confdbx/", ui.txtDbx_path_conf->text(), "Config DOSBox (*.conf);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
//		if( !appConfg.exists() ) {
			QFileInfo fi( archivo );
			str = fi.fileName();
			str = fGrl.eliminar_caracteres( str );
			str_ok = str.endsWith(".conf");
			if(str_ok == false) str.append(".conf");
			ui.txtDbx_path_conf->setText( str );
//		}else{
//			QMessageBox::information( this, stTituloDbx(), tr("El archivo de Configuración para el DOSBox ya esixte"));
//			ui.txtDbx_path_conf->setText("");
//		}
	} else
		ui.txtDbx_path_conf->setText( "" );
}

void frmDbxAdd::on_btnDbx_ExeJuego()
{
	ui.txtDbx_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), ui.txtDbx_path_exe->text(), ui.txtDbx_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmDbxAdd::on_btnDbx_ExeSetup()
{
	ui.txtDbx_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), ui.txtDbx_path_setup->text(), ui.txtDbx_path_setup->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmDbxAdd::on_btnMount_Add()
{
	QHash<QString, QString> DatosMontaje;
	DatosMontaje.clear();
	DatosMontaje["path"]      = ""; // directorio o iso
	DatosMontaje["label"]     = ""; // etiqueta
	DatosMontaje["tipo_as"]   = "drive"; // tipo de montaje
	DatosMontaje["letter"]    = "C"; // letra de montaje
	DatosMontaje["indx_cd"]   = ""; // index de la unidad de cd-rom
	DatosMontaje["opt_mount"] = ""; // opciones del cd-rom
	DatosMontaje["io_ctrl"]   = "-aspi"; // cd/dvd

	frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
	if( AddEditMontajes->exec() == QDialog::Accepted )
	{
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		QString tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];
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

		item->setText( 0 , AddEditMontajes->DatosMontaje["path"]		);	// directorio o iso
		item->setText( 1 , AddEditMontajes->DatosMontaje["label"]		);	// etiqueta
		item->setText( 2 , AddEditMontajes->DatosMontaje["tipo_as"]		);	// tipo de montaje
		item->setText( 3 , AddEditMontajes->DatosMontaje["letter"]		);	// letra de montaje
		item->setText( 4 , AddEditMontajes->DatosMontaje["indx_cd"]		);	// index de la unidad de cd-rom
		item->setText( 5 , AddEditMontajes->DatosMontaje["opt_mount"]	);	// opciones del cd-rom
		item->setText( 6 , AddEditMontajes->DatosMontaje["io_ctrl"]		);	// cd/dvd
		item->setText( 7 , "x"											);	// primer montaje

		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
		datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
		datos_montaje["opt_loadfix_mem"] = "64";

		if(ui.chkDbx_loadfix->isChecked())
			datos_montaje["opt_loadfix"] = "true";
		else datos_montaje["opt_loadfix"] = "false";

		if(ui.chkDbx_cerrar_dbox->isChecked())
			datos_montaje["opt_cerrar_dbox"] = "true";
		else datos_montaje["opt_cerrar_dbox"] = "false";

		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}

void frmDbxAdd::on_btnMount_Edit()
{
	int pos = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );

	if( ui.twMontajes->topLevelItemCount()>0 && pos!=-1 )
	{
		QHash<QString, QString> DatosMontaje;
		DatosMontaje.clear();
		DatosMontaje["path"]      = ui.twMontajes->currentItem()->text(0); // directorio o iso
		DatosMontaje["label"]     = ui.twMontajes->currentItem()->text(1); // etiqueta
		DatosMontaje["tipo_as"]   = ui.twMontajes->currentItem()->text(2); // tipo de montaje
		DatosMontaje["letter"]    = ui.twMontajes->currentItem()->text(3); // letra de montaje
		DatosMontaje["indx_cd"]   = ui.twMontajes->currentItem()->text(4); // index de la unidad de cd-rom
		DatosMontaje["opt_mount"] = ui.twMontajes->currentItem()->text(5); // opciones del cd-rom
		DatosMontaje["io_ctrl"]   = ui.twMontajes->currentItem()->text(6); // cd/dvd
	
		frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
		if( AddEditMontajes->exec() == QDialog::Accepted )
		{
			QString tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];
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
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_hd.png")	);

			ui.twMontajes->currentItem()->setText( 0 , AddEditMontajes->DatosMontaje["path"]		);	// directorio o iso
			ui.twMontajes->currentItem()->setText( 1 , AddEditMontajes->DatosMontaje["label"]		);	// etiqueta
			ui.twMontajes->currentItem()->setText( 2 , AddEditMontajes->DatosMontaje["tipo_as"]		);	// tipo de montaje
			ui.twMontajes->currentItem()->setText( 3 , AddEditMontajes->DatosMontaje["letter"]		);	// letra de montaje
			ui.twMontajes->currentItem()->setText( 4 , AddEditMontajes->DatosMontaje["indx_cd"]		);	// index de la unidad de cd-rom
			ui.twMontajes->currentItem()->setText( 5 , AddEditMontajes->DatosMontaje["opt_mount"]	);	// opciones del cd-rom
			ui.twMontajes->currentItem()->setText( 6 , AddEditMontajes->DatosMontaje["io_ctrl"]		);	// cd/dvd
			//ui.twMontajes->currentItem()->setText( 7 , );	// select_mount
			//ui.twMontajes->currentItem()->setText( 8 , );	// id
			//ui.twMontajes->currentItem()->setText( 9 , AddEditMontajes->DatosMontaje["io_ctrl"]		);	// id_lista
	
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"] = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"] = ui.txtDbx_parametros_exe->text();
			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true";
			else datos_montaje["opt_loadfix"] = "false";
	
			if(ui.chkDbx_cerrar_dbox->isChecked())
				datos_montaje["opt_cerrar_dbox"] = "true";
			else datos_montaje["opt_cerrar_dbox"] = "false";
	
			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
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
			datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
			datos_montaje["opt_loadfix_mem"] = "64";
			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true";
			else datos_montaje["opt_loadfix"] = "false";

			if(ui.chkDbx_cerrar_dbox->isChecked())
				datos_montaje["opt_cerrar_dbox"] = "true";
			else datos_montaje["opt_cerrar_dbox"] = "false";

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
	int index_1 = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	int index_2;
	if( index_1 > 0 && index_1!=-1)
	{
		index_2 = index_1 - 1;

		QTreeWidgetItem *item = ui.twMontajes->topLevelItem( index_1 );

		item = ui.twMontajes->takeTopLevelItem( index_1 );
		ui.twMontajes->insertTopLevelItem(index_2, item);

		ui.twMontajes->setCurrentItem( item );
	}
}

void frmDbxAdd::on_btnMount_Bajar()
{
	int index_1 = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	int index_2;
	if( index_1 < (ui.twMontajes->topLevelItemCount() - 1) && index_1!=-1 )
	{
		index_2 = index_1 + 1;

		QTreeWidgetItem *item = ui.twMontajes->topLevelItem( index_1 );

		item = ui.twMontajes->takeTopLevelItem( index_1 );
		ui.twMontajes->insertTopLevelItem(index_2, item);

		ui.twMontajes->setCurrentItem( item );
	}
}

void frmDbxAdd::on_btnMount_AutoCrear()
{
	QFileInfo fi( ui.txtDbx_path_exe->text() );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
	item->setIcon( 0 , QIcon(":/img16/drive_hd.png") );
	item->setText( 0 , fi.absolutePath()	);	// directorio o iso
	item->setText( 1 , ""					);	// etiqueta
	item->setText( 2 , "drive"				);	// tipo de montaje
	item->setText( 3 , "C"					);	// letra de montaje
	item->setText( 4 , ""					);	// index de la unidad de cd-rom
	item->setText( 5 , ""					);	// opciones del cd-rom
	item->setText( 6 , "-aspi"				);	// cd/dvd IOCtrl
	item->setText( 7 , "x"					);	// primer montaje

	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
	datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
	datos_montaje["opt_loadfix_mem"] = "64";

	if(ui.chkDbx_loadfix->isChecked())
		datos_montaje["opt_loadfix"] = "true";
	else datos_montaje["opt_loadfix"] = "false";

	if(ui.chkDbx_cerrar_dbox->isChecked())
		datos_montaje["opt_cerrar_dbox"] = "true";
	else datos_montaje["opt_cerrar_dbox"] = "false";

	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmDbxAdd::on_btnMount_Primario()
{
	int indx_mount=0, num_mount=0;	
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		indx_mount = ui.twMontajes->indexOfTopLevelItem(ui.twMontajes->currentItem());
		for ( num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
			ui.twMontajes->topLevelItem( num_mount )->setText(7 , "x");

		ui.twMontajes->topLevelItem( indx_mount )->setText(7 , "v");

		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_path_exe->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_parametros_exe->text();
		datos_montaje["opt_loadfix_mem"] = "64";
		if(ui.chkDbx_loadfix->isChecked())
			datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_cerrar_dbox->isChecked())
			datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";

		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}
