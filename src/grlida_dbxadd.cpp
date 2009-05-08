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
#include "grlida_importar_juego.h"

frmDbxAdd::frmDbxAdd( QDialog *parent, Qt::WFlags flags )
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	ui.wizardDbx->setCurrentIndex(0);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();

	createConnections();

	setTheme();

	CargarConfig();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmDbxAdd::~frmDbxAdd()
{
	//
}

void frmDbxAdd::createConnections()
{
// Conecta los distintos botones con las funciones.
	connect( ui.btnOk       , SIGNAL( clicked() ), this, SLOT( on_btnOk()       ) );
	connect( ui.btnNext     , SIGNAL( clicked() ), this, SLOT( on_btnNext()     ) );
	connect( ui.btnPrevious , SIGNAL( clicked() ), this, SLOT( on_btnPrevious() ) );

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
	connect( ui.cbxDbx_Profiles   , SIGNAL( activated(int) ), this, SLOT( on_setProfileGame(int) ) );
	connect( ui.btnDescargarInfo  , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo()  ) );
}

void frmDbxAdd::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/dosbox.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnPrevious->setIcon( QIcon(stTheme+"img16/mp_rebobinar_atras.png") );
	ui.btnNext->setIcon( QIcon(stTheme+"img16/mp_rebobinar_adelante.png") );
	ui.btnDbx_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_ExeSetup->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDescargarInfo->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.btnMount_Add->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.btnMount_Edit->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.btnMount_Delete->setIcon( QIcon(stTheme+"img16/eliminar.png") );
	ui.btnMount_Clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnMount_Subir->setIcon( QIcon(stTheme+"img16/go-up.png") );
	ui.btnMount_Bajar->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.btnMount_AutoCrear->setIcon( QIcon(stTheme+"img16/ejecutar_app.png") );
	ui.btnMount_Primario->setIcon( QIcon(stTheme+"img16/aplicar.png") );
}

void frmDbxAdd::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	ui.twMontajes->header()->setStretchLastSection(true);
	ui.twMontajes->header()->setMovable(false);

	QStringList sonido_frecuencias;
	sonido_frecuencias << "11050" << "22050" << "44100" << "48000";

	fGrl.Cargar_Profile_DFend_ComboBox(stHomeDir + "templates/", ui.cbxDbx_Profiles);
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt" , ui.cbxDbx_sdl_fullresolution); // Resolución pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"     , ui.cbxDbx_sdl_output        ); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"    , ui.cbxDbx_dosbox_machine    ); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"     , ui.cbxDbx_render_scaler     ); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt"    , ui.cbxDbx_dosbox_memsize    ); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"     , ui.cbxDbx_cpu_cycles        ); // Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt"     , ui.cbxDbx_sblaster_sbtype   ); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt"     , ui.cbxDbx_midi_mpu401       ); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt", ui.cbxDbx_midi_device       ); // MIDI Device
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_core.txt"   , ui.cbxDbx_cpu_core          ); // Ncleo de la CPU DOSBox

	ui.cbxDbx_dos_umb->clear();
	ui.cbxDbx_dos_umb->addItem("true");
	ui.cbxDbx_dos_umb->addItem("false");
	ui.cbxDbx_dos_umb->addItem("max");

	ui.cbxDbx_mixer_rate->clear();
	ui.cbxDbx_mixer_rate->addItems( sonido_frecuencias );
	ui.cbxDbx_sblaster_oplrate->clear();
	ui.cbxDbx_sblaster_oplrate->addItems( sonido_frecuencias );
	ui.cbxDbx_gus_gusrate->clear();
	ui.cbxDbx_gus_gusrate->addItems( sonido_frecuencias );
	ui.cbxDbx_midi_mt32rate->clear();
	ui.cbxDbx_midi_mt32rate->addItems( sonido_frecuencias );
	ui.cbxDbx_speaker_pcrate->clear();
	ui.cbxDbx_speaker_pcrate->addItems( sonido_frecuencias );
	ui.cbxDbx_speaker_tandyrate->clear();
	ui.cbxDbx_speaker_tandyrate->addItems( sonido_frecuencias );

	on_setProfileGame( 0 );

// Ponemos los Combobox por defecto.
	ui.cbxDbx_Profiles->setCurrentIndex( 0 );
	ui.cbxDbx_sdl_fullresolution->setCurrentIndex( 0 ); // Resolución pantalla
	ui.cbxDbx_sdl_output->setCurrentIndex( 0 );         // Modo de Renderizado
	ui.cbxDbx_dosbox_machine->setCurrentIndex( 0 );     // Tarjeta de Video
	ui.cbxDbx_render_scaler->setCurrentIndex( 1 );      // Escalar y Filtros
	ui.cbxDbx_dosbox_memsize->setCurrentIndex( 7 );     // Cantidad de memoria para DOSBox
	ui.cbxDbx_cpu_cycles->setCurrentIndex( 0 );         // Ciclos DOSBox
	ui.cbxDbx_sblaster_sbtype->setCurrentIndex( 5 );    // Tipo Sound Blaste
	ui.cbxDbx_midi_mpu401->setCurrentIndex( 0 );        // MPU-401
	ui.cbxDbx_midi_device->setCurrentIndex( 0 );        // MIDI Device
	ui.cbxDbx_midi_mt32rate->setCurrentIndex( 1 );      // mt32rate
	ui.cbxDbx_cpu_core->setCurrentIndex( 0 );           // Núcleo de la CPU DOSBox
	ui.cbxDbx_dos_umb->setCurrentIndex( 0 );            // Soporte para memoria UMB
	ui.cbxDbx_midi_mt32rate->setCurrentIndex( 1 );      //
	ui.cbxDbx_mixer_rate->setCurrentIndex( 1 );         //
	ui.cbxDbx_sblaster_oplrate->setCurrentIndex( 1 );   //
	ui.cbxDbx_gus_gusrate->setCurrentIndex( 1 );        //
	ui.cbxDbx_midi_mt32rate->setCurrentIndex( 1 );      //
	ui.cbxDbx_speaker_pcrate->setCurrentIndex( 1 );     //
	ui.cbxDbx_speaker_tandyrate->setCurrentIndex( 1 );  //

	ui.chkDbx_sdl_fullfixed->setVisible(false); // En desuso

	tempDatosJuego["Dat_favorito"] = "false";
	tempDatosJuego["Dat_rating"]   = "0";
}

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
	if( ui.wizardDbx->currentIndex() == 4 )
	{
		ui.btnMount_Clear->click();
		ui.btnMount_AutoCrear->click();
		ui.twMontajes->setFocus();
	}

	if( ui.wizardDbx->currentIndex() >= 5 )
	{
		ui.btnOk->setEnabled( true );
		ui.btnOk->setFocus();
		ui.btnNext->setEnabled( false );
	}
}

void frmDbxAdd::on_setProfileGame(int row)
{
	if (row >= 0)
	{
		QString profileGame;
		profileGame = ui.cbxDbx_Profiles->itemData( row ).toString();

		if( !profileGame.isEmpty() || profileGame != "<defecto>" )
		{
			tempProfileDosBox.clear();
			tempProfileDosBox = fGrl.Importar_Profile_DFend( stHomeDir + "templates/" + profileGame );

			ui.chkDbx_sdl_fullscreen->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_sdl_fullscreen"] ) );										// sdl_fullscreen
			ui.chkDbx_sdl_fullfixed->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_sdl_fullfixed"] ) );										// sdl_fullfixed
			ui.cbxDbx_sdl_fullresolution->setCurrentIndex( ui.cbxDbx_sdl_fullresolution->findText( tempProfileDosBox["Dbx_sdl_fullresolution"] ) );	// sdl_fullresolution
			ui.cbxDbx_sdl_output->setCurrentIndex( ui.cbxDbx_sdl_output->findText( tempProfileDosBox["Dbx_sdl_output"] ) );							// sdl_output
			ui.chkDbx_sdl_autolock->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_sdl_autolock"] ) );											// sdl_autolock

			ui.cbxDbx_dosbox_machine->setCurrentIndex( ui.cbxDbx_dosbox_machine->findText( tempProfileDosBox["Dbx_dosbox_machine"] ) );			// dosbox_machine
			ui.cbxDbx_dosbox_memsize->setEditText(tempProfileDosBox["Dbx_dosbox_memsize"] );

			ui.chkDbx_render_aspect->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_render_aspect"] ) );									// render_aspect
			ui.cbxDbx_render_scaler->setCurrentIndex( ui.cbxDbx_render_scaler->findText( tempProfileDosBox["Dbx_render_scaler"] ) );			// render_scaler

			ui.cbxDbx_cpu_core->setCurrentIndex( ui.cbxDbx_cpu_core->findText( tempProfileDosBox["Dbx_cpu_core"] ) );							// cpu_core
			ui.cbxDbx_cpu_cycles->setCurrentIndex( ui.cbxDbx_cpu_cycles->findText( tempProfileDosBox["Dbx_cpu_cycles"] ) );						// cpu_cycles

			ui.chkDbx_mixer_nosound->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_mixer_nosound"] ) );									// mixer_nosound
			ui.cbxDbx_mixer_rate->setCurrentIndex( ui.cbxDbx_mixer_rate->findText( tempProfileDosBox["Dbx_mixer_rate"] ) );						// mixer_rate

			ui.cbxDbx_midi_mpu401->setCurrentIndex( ui.cbxDbx_midi_mpu401->findText( tempProfileDosBox["Dbx_midi_mpu401"] ) );					// midi_mpu401
			ui.chkDbx_midi_intelligent->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_midi_intelligent"] ) );								// midi_intelligent
			ui.cbxDbx_midi_device->setCurrentIndex( ui.cbxDbx_midi_device->findText( tempProfileDosBox["Dbx_midi_device"] ) );					// midi_device
			ui.cbxDbx_midi_mt32rate->setCurrentIndex( ui.cbxDbx_midi_mt32rate->findText( tempProfileDosBox["Dbx_midi_mt32rate"] ) );				// midi_mt32rate

			ui.cbxDbx_sblaster_sbtype->setCurrentIndex( ui.cbxDbx_sblaster_sbtype->findText( tempProfileDosBox["Dbx_sblaster_sbtype"] ) );		// sblaster_sbtype
			ui.cbxDbx_sblaster_oplrate->setCurrentIndex( ui.cbxDbx_sblaster_oplrate->findText( tempProfileDosBox["Dbx_sblaster_oplrate"] ) );	// sblaster_oplrate

			ui.chkDbx_gus_gus->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_gus_gus"] ) );												// gus_gus
			ui.cbxDbx_gus_gusrate->setCurrentIndex( ui.cbxDbx_gus_gusrate->findText( tempProfileDosBox["Dbx_gus_gusrate"] ) );					// gus_gusrate

			ui.chkDbx_speaker_pcspeaker->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_speaker_pcspeaker"] ) );							// speaker_pcspeaker
			ui.cbxDbx_speaker_pcrate->setCurrentIndex( ui.cbxDbx_speaker_pcrate->findText( tempProfileDosBox["Dbx_speaker_pcrate"] ) );			// speaker_pcrate
			ui.cbxDbx_speaker_tandyrate->setCurrentIndex( ui.cbxDbx_speaker_tandyrate->findText( tempProfileDosBox["Dbx_speaker_tandyrate"] ) );// speaker_tandyrate
			ui.chkDbx_speaker_disney->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_speaker_disney"] ) );									// speaker_disney

			ui.chkDbx_dos_xms->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_dos_xms"] ) );						// dos_xms
			ui.chkDbx_dos_ems->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_dos_ems"] ) );						// dos_ems
			ui.cbxDbx_dos_umb->setCurrentIndex( ui.cbxDbx_dos_umb->findText( tempProfileDosBox["Dbx_dos_umb"] ) );		// dos_umb

			ui.chkDbx_loadfix->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_opt_loadfix"] ) );					// opt_loadfix
			ui.chkDbx_consola_dbox->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_opt_consola_dbox"] ) );			// opt_consola_dbox
			ui.chkDbx_cerrar_dbox->setChecked( fGrl.StrToBool( tempProfileDosBox["Dbx_opt_cerrar_dbox"] ) );			// opt_cerrar_dbox

			if( !tempProfileDosBox["Dbx_path_exe"].isEmpty() )
				ui.txtDbx_path_exe->setText( tempProfileDosBox["Dbx_path_exe"] );					// path_exe

			if( !tempProfileDosBox["Dbx_path_setup"].isEmpty() )
				ui.txtDbx_path_setup->setText( tempProfileDosBox["Dbx_path_setup"] );				// path_setup

			ui.txtDbx_parametros_exe->setText( tempProfileDosBox["Dbx_parametros_exe"] );			// parametros_exe
			ui.txtDbx_parametros_setup->setText( tempProfileDosBox["Dbx_parametros_setup"] );		// parametros_setup

			ui.twMontajes->clear();
			ui.btnMount_AutoCrear->click();
		} else {
			tempProfileDosBox["Dbx_sdl_fulldouble"]        = "false";		// sdl_fulldouble
			tempProfileDosBox["Dbx_sdl_windowresolution"]  = "original";	// sdl_windowresolution
			tempProfileDosBox["Dbx_sdl_hwscale"]           = "1.00";		// sdl_hwscale
			tempProfileDosBox["Dbx_sdl_sensitivity"]       = "100";			// sdl_sensitivity
			tempProfileDosBox["Dbx_sdl_waitonerror"]       = "true";		// sdl_waitonerror
			tempProfileDosBox["Dbx_sdl_priority"]          = "higher,normal";// sdl_priority
			tempProfileDosBox["Dbx_sdl_mapperfile"]        = "mapper.txt";	// sdl_mapperfile
			tempProfileDosBox["Dbx_sdl_usescancodes"]      = "true";		// sdl_usescancodes
			tempProfileDosBox["Dbx_dosbox_language"]       = "";			// dosbox_language
			tempProfileDosBox["Dbx_dosbox_captures"]       = "capture";		// dosbox_captures
			tempProfileDosBox["Dbx_render_frameskip"]      = "0";			// render_frameskip
			tempProfileDosBox["Dbx_cpu_cputype"]           = "auto";		// cpu_cputype
			tempProfileDosBox["Dbx_cpu_cycleup"]           = "500";			// cpu_cycleup
			tempProfileDosBox["Dbx_cpu_cycledown"]         = "20";			// cpu_cycledown
			tempProfileDosBox["Dbx_mixer_blocksize"]       = "2048";		// mixer_blocksize
			tempProfileDosBox["Dbx_mixer_prebuffer"]       = "10";			// mixer_prebuffer
			tempProfileDosBox["Dbx_midi_config"]           = "";			// midi_config
			tempProfileDosBox["Dbx_sblaster_sbbase"]       = "220";			// sblaster_sbbase
			tempProfileDosBox["Dbx_sblaster_irq"]          = "7";			// sblaster_irq
			tempProfileDosBox["Dbx_sblaster_dma"]          = "1";			// sblaster_dma
			tempProfileDosBox["Dbx_sblaster_hdma"]         = "5";			// sblaster_hdma
			tempProfileDosBox["Dbx_sblaster_mixer"]        = "true";		// sblaster_mixer
			tempProfileDosBox["Dbx_sblaster_oplmode"]      = "auto";		// sblaster_oplmode
			tempProfileDosBox["Dbx_gus_gusbase"]           = "240";			// gus_gusbase
			tempProfileDosBox["Dbx_gus_irq1"]              = "5";			// gus_irq1
			tempProfileDosBox["Dbx_gus_irq2"]              = "5";			// gus_irq2
			tempProfileDosBox["Dbx_gus_dma1"]              = "3";			// gus_dma1
			tempProfileDosBox["Dbx_gus_dma2"]              = "3";			// gus_dma2
			tempProfileDosBox["Dbx_gus_ultradir"]          = "C:\\ULTRASND";// gus_ultradir
			tempProfileDosBox["Dbx_speaker_tandy"]         = "auto";		// speaker_tandy
			tempProfileDosBox["Dbx_joystick_type"]         = "auto";		// joystick_type
			tempProfileDosBox["Dbx_joystick_timed"]        = "true";		// joystick_timed
			tempProfileDosBox["Dbx_joystick_autofire"]     = "false";		// joystick_autofire
			tempProfileDosBox["Dbx_joystick_swap34"]       = "false";		// joystick_swap34
			tempProfileDosBox["Dbx_joystick_buttonwrap"]   = "true";		// joystick_buttonwrap
			tempProfileDosBox["Dbx_modem_modem"]           = "true";		// modem_modem
			tempProfileDosBox["Dbx_modem_comport"]         = "1";			// modem_comport
			tempProfileDosBox["Dbx_modem_listenport"]      = "23";			// modem_listenport
			tempProfileDosBox["Dbx_dserial_directserial"]  = "true";		// dserial_directserial
			tempProfileDosBox["Dbx_dserial_comport"]       = "1";			// dserial_comport
			tempProfileDosBox["Dbx_dserial_realport"]      = "COM1";		// serial_realport
			tempProfileDosBox["Dbx_dserial_defaultbps"]    = "1200";		// dserial_defaultbps
			tempProfileDosBox["Dbx_dserial_parity"]        = "N";			// dserial_parity
			tempProfileDosBox["Dbx_dserial_bytesize"]      = "8";			// dserial_bytesize
			tempProfileDosBox["Dbx_dserial_stopbit"]       = "1";			// dserial_stopbit
			tempProfileDosBox["Dbx_serial_1"]              = "dummy";		// serial_1
			tempProfileDosBox["Dbx_serial_2"]              = "dummy";		// serial_2
			tempProfileDosBox["Dbx_serial_3"]              = "disabled";	// serial_3
			tempProfileDosBox["Dbx_serial_4"]              = "disabled";	// serial_4
			tempProfileDosBox["Dbx_dos_keyboardlayout"]    = "none";		// dos_keyboardlayout
			tempProfileDosBox["Dbx_ipx_ipx"]               = "false";		// ipx_ipx
			tempProfileDosBox["Dbx_autoexec"]              = "";			// autoexec
			tempProfileDosBox["Dbx_opt_autoexec"]          = "false";		// opt_autoexec
			tempProfileDosBox["Dbx_opt_loadfix_mem"]       = "64";			// opt_loadfix_mem
			tempProfileDosBox["Dbx_opt_cycle_sincronizar"] = "false";		// opt_cycle_sincronizar
			tempProfileDosBox["Dbx_path_sonido"]           = "";			// path_sonido
		}
	}
}

void frmDbxAdd::on_btnDescargarInfo()
{
	frmImportarJuego * ImportarJuego = new frmImportarJuego(ui.txtDatos_Titulo->text(), 0, Qt::Window);
	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		QFile file_thumbs, file_cover_front, file_cover_back;

		tempDatosJuego.clear();
		ui.txtDatos_Titulo->setText( ImportarJuego->DatosJuego["Dat_titulo"] );			//  titulo
		tempDatosJuego["Dat_icono"]         = "dosbox";										// icono
		tempDatosJuego["Dat_titulo"]        = ui.txtDatos_Titulo->text();					// titulo
		tempDatosJuego["Dat_subtitulo"]     = ImportarJuego->DatosJuego["Dat_subtitulo"];	// subtitulo
		tempDatosJuego["Dat_genero"]        = ImportarJuego->DatosJuego["Dat_genero"];		// genero
		tempDatosJuego["Dat_compania"]      = ImportarJuego->DatosJuego["Dat_compania"];	// compania
		tempDatosJuego["Dat_desarrollador"] = ImportarJuego->DatosJuego["Dat_desarrollador"];//desarrollador
		tempDatosJuego["Dat_tema"]          = ImportarJuego->DatosJuego["Dat_tema"];		// tema
		tempDatosJuego["Dat_idioma"]        = ImportarJuego->DatosJuego["Dat_idioma"];		// idioma
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
		tempDatosJuego["Dat_tipo_emu"]      = "dosbox";										// tipo_emu
		tempDatosJuego["Dat_comentario"]    = ImportarJuego->DatosJuego["Dat_comentario"];	// comentario
		tempDatosJuego["Dat_favorito"]      = ImportarJuego->DatosJuego["Dat_favorito"];	// favorito
		tempDatosJuego["Dat_rating"]        = ImportarJuego->DatosJuego["Dat_rating"];		// rating
		tempDatosJuego["Dat_usuario"]       = ImportarJuego->DatosJuego["Dat_usuario"];		// usuario
	}
}

void frmDbxAdd::on_btnOk()
{
	if( ui.txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information( this, stTituloDbx(), tr("Debes poner por lo menos el titulo."));
	else {
		DatosJuego.clear();
		DatosJuego["Dat_icono"]         = "dosbox";									// icono
		DatosJuego["Dat_titulo"]        = ui.txtDatos_Titulo->text();				// titulo
		DatosJuego["Dat_subtitulo"]     = ""+tempDatosJuego["Dat_subtitulo"];		// subtitulo
		DatosJuego["Dat_genero"]        = ""+tempDatosJuego["Dat_genero"];			// genero
		DatosJuego["Dat_compania"]      = ""+tempDatosJuego["Dat_compania"];		// compania
		DatosJuego["Dat_desarrollador"] = ""+tempDatosJuego["Dat_desarrollador"];	// desarrollador
		DatosJuego["Dat_tema"]          = ""+tempDatosJuego["Dat_tema"];			// tema
		DatosJuego["Dat_idioma"]        = ""+tempDatosJuego["Dat_idioma"];			// idioma
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
		DatosJuego["Dat_tipo_emu"]      = "dosbox";									// tipo_emu
		DatosJuego["Dat_comentario"]    = ""+tempDatosJuego["Dat_comentario"];		// comentario
		DatosJuego["Dat_favorito"]      = ""+tempDatosJuego["Dat_favorito"];		// favorito
		DatosJuego["Dat_rating"]        = ""+tempDatosJuego["Dat_rating"];			// rating
		DatosJuego["Dat_usuario"]       = ""+tempDatosJuego["Dat_usuario"];			// usuario

		DatosDosBox.clear();
	// sdl_fullscreen
		DatosDosBox["Dbx_sdl_fullscreen"] = fGrl.BoolToStr( ui.chkDbx_sdl_fullscreen->isChecked() );
	// sdl_fulldouble
		DatosDosBox["Dbx_sdl_fulldouble"] = tempProfileDosBox["Dbx_sdl_fulldouble"];
	// sdl_fullfixed
		DatosDosBox["Dbx_sdl_fullfixed"]  = fGrl.BoolToStr( ui.chkDbx_sdl_fullfixed->isChecked() );
	// sdl_fullresolution
		if( ui.cbxDbx_sdl_fullresolution->currentText() != "" )
			DatosDosBox["Dbx_sdl_fullresolution"] = ui.cbxDbx_sdl_fullresolution->currentText();
		else
			DatosDosBox["Dbx_sdl_fullresolution"] = "original";
	// sdl_windowresolution
		DatosDosBox["Dbx_sdl_windowresolution"] = tempProfileDosBox["Dbx_sdl_windowresolution"];
	// sdl_output
		if( ui.cbxDbx_sdl_output->currentText() != "" )
			DatosDosBox["Dbx_sdl_output"] = ui.cbxDbx_sdl_output->currentText();
		else
			DatosDosBox["Dbx_sdl_output"] = "surface";

		DatosDosBox["Dbx_sdl_hwscale"]      = tempProfileDosBox["Dbx_sdl_hwscale"];			// sdl_hwscale
		DatosDosBox["Dbx_sdl_autolock"]     = fGrl.BoolToStr( ui.chkDbx_sdl_autolock->isChecked() );	// sdl_autolock
		DatosDosBox["Dbx_sdl_sensitivity"]  = tempProfileDosBox["Dbx_sdl_sensitivity"];		// sdl_sensitivity
		DatosDosBox["Dbx_sdl_waitonerror"]  = tempProfileDosBox["Dbx_sdl_waitonerror"];		// sdl_waitonerror
		DatosDosBox["Dbx_sdl_priority"]     = tempProfileDosBox["Dbx_sdl_priority"];		// sdl_priority
		DatosDosBox["Dbx_sdl_mapperfile"]   = tempProfileDosBox["Dbx_sdl_mapperfile"];		// sdl_mapperfile
		DatosDosBox["Dbx_sdl_usescancodes"] = tempProfileDosBox["Dbx_sdl_usescancodes"];	// sdl_usescancodes
		DatosDosBox["Dbx_dosbox_language"]  = tempProfileDosBox["Dbx_dosbox_language"];		// dosbox_language

	// dosbox_machine
		if( ui.cbxDbx_dosbox_machine->currentText() != "" )
			DatosDosBox["Dbx_dosbox_machine"] = ui.cbxDbx_dosbox_machine->currentText();
		else
			DatosDosBox["Dbx_dosbox_machine"] = "vga";
	// dosbox_captures
		DatosDosBox["Dbx_dosbox_captures"] = tempProfileDosBox["Dbx_dosbox_captures"];
	// dosbox_memsize
		if( ui.cbxDbx_dosbox_memsize->currentText() != "" )
			DatosDosBox["Dbx_dosbox_memsize"] = ui.cbxDbx_dosbox_memsize->currentText();
		else
			DatosDosBox["Dbx_dosbox_memsize"] = "16";

		DatosDosBox["Dbx_render_frameskip"] = tempProfileDosBox["Dbx_render_frameskip"];					// render_frameskip
		DatosDosBox["Dbx_render_aspect"]    = fGrl.BoolToStr( ui.chkDbx_render_aspect->isChecked() );	// render_aspect
	// render_scaler
		if( ui.cbxDbx_render_scaler->currentText() != "" )
			DatosDosBox["Dbx_render_scaler"] = ui.cbxDbx_render_scaler->currentText();
		else
			DatosDosBox["Dbx_render_scaler"] = "normal2x";
	// cpu_core
		if( ui.cbxDbx_cpu_core->currentText() != "" )
			DatosDosBox["Dbx_cpu_core"] = ui.cbxDbx_cpu_core->currentText();
		else
			DatosDosBox["Dbx_cpu_core"] = "auto";
	// cpu_cycleup
		DatosDosBox["Dbx_cpu_cycleup"] = "auto";
	// cpu_cycles
		if( ui.cbxDbx_cpu_cycles->currentText() != "" )
			DatosDosBox["Dbx_cpu_cycles"] = ui.cbxDbx_cpu_cycles->currentText();
		else
			DatosDosBox["Dbx_cpu_cycles"] = "auto";

		DatosDosBox["Dbx_cpu_cycleup"]   = tempProfileDosBox["Dbx_cpu_cycleup"];						// cpu_cycleup
		DatosDosBox["Dbx_cpu_cycledown"] = tempProfileDosBox["Dbx_cpu_cycledown"];						// cpu_cycledown
		DatosDosBox["Dbx_mixer_nosound"] = fGrl.BoolToStr( ui.chkDbx_mixer_nosound->isChecked() );	// mixer_nosound
	// mixer_rate
		if( ui.cbxDbx_mixer_rate->currentText() != "" )
			DatosDosBox["Dbx_mixer_rate"] = ui.cbxDbx_mixer_rate->currentText();
		else
			DatosDosBox["Dbx_mixer_rate"] = "22050";

		DatosDosBox["Dbx_mixer_blocksize"] = tempProfileDosBox["Dbx_mixer_blocksize"];	// mixer_blocksize
		DatosDosBox["Dbx_mixer_prebuffer"] = tempProfileDosBox["Dbx_mixer_prebuffer"];	// mixer_prebuffer
	// midi_mpu401
		if( ui.cbxDbx_midi_mpu401->currentText() != "" )
			DatosDosBox["Dbx_midi_mpu401"] = ui.cbxDbx_midi_mpu401->currentText();
		else
			DatosDosBox["Dbx_midi_mpu401"] = "intelligent";
	// midi_intelligent <-DOSBox 0.63
		DatosDosBox["Dbx_midi_intelligent"] = fGrl.BoolToStr( ui.chkDbx_midi_intelligent->isChecked() );
	// midi_device
		if( ui.cbxDbx_midi_device->currentText() != "" )
			DatosDosBox["Dbx_midi_device"] = ui.cbxDbx_midi_device->currentText();
		else
			DatosDosBox["Dbx_midi_device"] = "default";

		DatosDosBox["Dbx_midi_config"] = tempProfileDosBox["Dbx_midi_config"];	// midi_config
	// midi_mt32rate
		if( ui.cbxDbx_midi_mt32rate->currentText() != "" )
			DatosDosBox["Dbx_midi_mt32rate"] = ui.cbxDbx_midi_mt32rate->currentText();
		else
			DatosDosBox["Dbx_midi_mt32rate"] = "22050";
	// sblaster_sbtype
		if( ui.cbxDbx_sblaster_sbtype->currentText() != "" )
			DatosDosBox["Dbx_sblaster_sbtype"] = ui.cbxDbx_sblaster_sbtype->currentText();
		else
			DatosDosBox["Dbx_sblaster_sbtype"] = "sb16";

		DatosDosBox["Dbx_sblaster_sbbase"]  = tempProfileDosBox["Dbx_sblaster_sbbase"];		// sblaster_sbbase
		DatosDosBox["Dbx_sblaster_irq"]     = tempProfileDosBox["Dbx_sblaster_irq"];		// sblaster_irq
		DatosDosBox["Dbx_sblaster_dma"]     = tempProfileDosBox["Dbx_sblaster_dma"];		// sblaster_dma
		DatosDosBox["Dbx_sblaster_hdma"]    = tempProfileDosBox["Dbx_sblaster_hdma"];		// sblaster_hdma
		DatosDosBox["Dbx_sblaster_mixer"]   = tempProfileDosBox["Dbx_sblaster_mixer"];		// sblaster_mixer
		DatosDosBox["Dbx_sblaster_oplmode"] = tempProfileDosBox["Dbx_sblaster_oplmode"];	// sblaster_oplmode

	// sblaster_oplrate
		if( ui.cbxDbx_sblaster_oplrate->currentText() != "" )
			DatosDosBox["Dbx_sblaster_oplrate"] = ui.cbxDbx_sblaster_oplrate->currentText();
		else
			DatosDosBox["Dbx_sblaster_oplrate"] = "22050";
	// gus_gus
		DatosDosBox["Dbx_gus_gus"] = fGrl.BoolToStr( ui.chkDbx_gus_gus->isChecked() );
	// gus_gusrate
		if( ui.cbxDbx_gus_gusrate->currentText() != "" )
			DatosDosBox["Dbx_gus_gusrate"] = ui.cbxDbx_gus_gusrate->currentText();
		else
			DatosDosBox["Dbx_gus_gusrate"] = "22050";

		DatosDosBox["Dbx_gus_gusbase"]       = tempProfileDosBox["Dbx_gus_gusbase"];	// gus_gusbase
		DatosDosBox["Dbx_gus_irq1"]          = tempProfileDosBox["Dbx_gus_irq1"];		// gus_irq1
		DatosDosBox["Dbx_gus_irq2"]          = tempProfileDosBox["Dbx_gus_irq2"];		// gus_irq2
		DatosDosBox["Dbx_gus_dma1"]          = tempProfileDosBox["Dbx_gus_dma1"];		// gus_dma1
		DatosDosBox["Dbx_gus_dma2"]          = tempProfileDosBox["Dbx_gus_dma2"];		// gus_dma2
		DatosDosBox["Dbx_gus_ultradir"]      = tempProfileDosBox["Dbx_gus_ultradir"];	// gus_ultradir
		DatosDosBox["Dbx_speaker_pcspeaker"] = fGrl.BoolToStr( ui.chkDbx_speaker_pcspeaker->isChecked() );// speaker_pcspeaker
	// speaker_pcrate
		if( ui.cbxDbx_speaker_pcrate->currentText() != "" )
			DatosDosBox["Dbx_speaker_pcrate"] = ui.cbxDbx_speaker_pcrate->currentText();
		else
			DatosDosBox["Dbx_speaker_pcrate"] = "22050";

		DatosDosBox["Dbx_speaker_tandy"] = tempProfileDosBox["Dbx_speaker_tandy"];	// speaker_tandy
	// speaker_tandyrate
		if( ui.cbxDbx_speaker_tandyrate->currentText() != "" )
			DatosDosBox["Dbx_speaker_tandyrate"] = ui.cbxDbx_speaker_tandyrate->currentText();
		else
			DatosDosBox["Dbx_speaker_tandyrate"] = "22050";

		DatosDosBox["Dbx_speaker_disney"]       = fGrl.BoolToStr( ui.chkDbx_speaker_disney->isChecked() );	// speaker_disney
		DatosDosBox["Dbx_joystick_type"]        = tempProfileDosBox["Dbx_joystick_type"];		// joystick_type
		DatosDosBox["Dbx_joystick_timed"]       = tempProfileDosBox["Dbx_joystick_timed"];		// joystick_timed
		DatosDosBox["Dbx_joystick_autofire"]    = tempProfileDosBox["Dbx_joystick_autofire"];	// joystick_autofire
		DatosDosBox["Dbx_joystick_swap34"]      = tempProfileDosBox["Dbx_joystick_swap34"];		// joystick_swap34
		DatosDosBox["Dbx_joystick_buttonwrap"]  = tempProfileDosBox["Dbx_joystick_buttonwrap"];	// joystick_buttonwrap
		DatosDosBox["Dbx_modem_modem"]          = tempProfileDosBox["Dbx_modem_modem"];			// modem_modem			<-DOSBox 0.63
		DatosDosBox["Dbx_modem_comport"]        = tempProfileDosBox["Dbx_modem_comport"];		// modem_comport		<-DOSBox 0.63
		DatosDosBox["Dbx_modem_listenport"]     = tempProfileDosBox["Dbx_modem_listenport"];	// modem_listenport		<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_directserial"] = tempProfileDosBox["Dbx_dserial_directserial"];// dserial_directserial	<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_comport"]      = tempProfileDosBox["Dbx_dserial_comport"];		// dserial_comport		<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_realport"]     = tempProfileDosBox["Dbx_dserial_realport"];	// dserial_realport		<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_defaultbps"]   = tempProfileDosBox["Dbx_dserial_defaultbps"];	// dserial_defaultbps	<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_parity"]       = tempProfileDosBox["Dbx_dserial_parity"];		// dserial_parity		<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_bytesize"]     = tempProfileDosBox["Dbx_dserial_bytesize"];	// dserial_bytesize		<-DOSBox 0.63
		DatosDosBox["Dbx_dserial_stopbit"]      = tempProfileDosBox["Dbx_dserial_stopbit"];		// dserial_stopbit		<-DOSBox 0.63
		DatosDosBox["Dbx_serial_1"]             = tempProfileDosBox["Dbx_serial_1"];			// serial_1
		DatosDosBox["Dbx_serial_2"]             = tempProfileDosBox["Dbx_serial_2"];			// serial_2
		DatosDosBox["Dbx_serial_3"]             = tempProfileDosBox["Dbx_serial_3"];			// serial_3
		DatosDosBox["Dbx_serial_4"]             = tempProfileDosBox["Dbx_serial_4"];			// serial_4
		DatosDosBox["Dbx_dos_xms"]              = fGrl.BoolToStr( ui.chkDbx_dos_xms->isChecked() );	// dos_xms
		DatosDosBox["Dbx_dos_ems"]              = fGrl.BoolToStr( ui.chkDbx_dos_ems->isChecked() );	// dos_ems
	// dos_umb
		if( ui.cbxDbx_dos_umb->currentText() != "" )
			DatosDosBox["Dbx_dos_umb"] = ui.cbxDbx_dos_umb->currentText();
		else
			DatosDosBox["Dbx_dos_umb"] = "true";

		DatosDosBox["Dbx_dos_keyboardlayout"]    = tempProfileDosBox["Dbx_dos_keyboardlayout"];					// dos_keyboardlayout
		DatosDosBox["Dbx_ipx_ipx"]               = tempProfileDosBox["Dbx_ipx_ipx"];							// ipx_ipx
		DatosDosBox["Dbx_autoexec"]              = tempProfileDosBox["Dbx_autoexec"];							// autoexec
		DatosDosBox["Dbx_opt_autoexec"]          = tempProfileDosBox["Dbx_opt_autoexec"];						// opt_autoexec
		DatosDosBox["Dbx_opt_loadfix"]           = fGrl.BoolToStr( ui.chkDbx_loadfix->isChecked() );		// opt_loadfix
		DatosDosBox["Dbx_opt_loadfix_mem"]       = tempProfileDosBox["Dbx_opt_loadfix_mem"];					// opt_loadfix_mem
		DatosDosBox["Dbx_opt_consola_dbox"]      = fGrl.BoolToStr( ui.chkDbx_consola_dbox->isChecked() );	// opt_consola_dbox
		DatosDosBox["Dbx_opt_cerrar_dbox"]       = fGrl.BoolToStr( ui.chkDbx_cerrar_dbox->isChecked() );	// opt_cerrar_dbox
		DatosDosBox["Dbx_opt_cycle_sincronizar"] = tempProfileDosBox["Dbx_opt_cycle_sincronizar"];				// opt_cycle_sincronizar
		DatosDosBox["Dbx_path_conf"]             = ui.txtDbx_path_conf->text();								// path_conf
		DatosDosBox["Dbx_path_sonido"]           = tempProfileDosBox["Dbx_path_sonido"];						// path_sonido
		DatosDosBox["Dbx_path_exe"]              = ui.txtDbx_path_exe->text();								// path_exe
		DatosDosBox["Dbx_path_setup"]            = ui.txtDbx_path_setup->text();							// path_setup
		DatosDosBox["Dbx_parametros_exe"]        = ui.txtDbx_parametros_exe->text();						// parametros_exe
		DatosDosBox["Dbx_parametros_setup"]      = ui.txtDbx_parametros_setup->text();						// parametros_setup

		QDialog::accept();
	}
}

void frmDbxAdd::on_txtDatos_Titulo_textChanged(const QString &text)
{
	bool str_ok;
	QString str;

	if( !text.isEmpty() )
	{
		str = fGrl.eliminar_caracteres( text );
		str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");
	} else
		str = "";

	ui.txtDbx_path_conf->setText( str );
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
	ui.txtDbx_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_exe"].toString(), ui.txtDbx_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_exe", GRLConfig["Dbx_path_exe"].toString() );
}

void frmDbxAdd::on_btnDbx_ExeSetup()
{
	ui.txtDbx_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_setup"].toString(), ui.txtDbx_path_setup->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_setup->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_setup", GRLConfig["Dbx_path_setup"].toString() );
}

void frmDbxAdd::PrevierMontajes()
{
	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["Dbx_path_exe"]        = ui.txtDbx_path_exe->text();
	datos_montaje["Dbx_parametros_exe"]  = ui.txtDbx_parametros_exe->text();
	datos_montaje["Dbx_opt_loadfix_mem"] = tempProfileDosBox["Dbx_opt_loadfix_mem"];
	datos_montaje["Dbx_opt_loadfix"]     = fGrl.BoolToStr( ui.chkDbx_loadfix->isChecked() );
	datos_montaje["Dbx_opt_cerrar_dbox"] = fGrl.BoolToStr( ui.chkDbx_cerrar_dbox->isChecked() );

	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmDbxAdd::on_btnMount_Add()
{
	QHash<QString, QString> DatosMontaje;
	DatosMontaje.clear();
	DatosMontaje["path"]      = "";      // directorio o iso
	DatosMontaje["label"]     = "";      // etiqueta
	DatosMontaje["tipo_as"]   = "drive"; // tipo de montaje
	DatosMontaje["letter"]    = "C";     // letra de montaje
	DatosMontaje["indx_cd"]   = "";      // index de la unidad de cd-rom
	DatosMontaje["opt_mount"] = "";      // opciones del cd-rom
	DatosMontaje["io_ctrl"]   = "";      // cd/dvd

	frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
	if( AddEditMontajes->exec() == QDialog::Accepted )
	{
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		QString tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];

		item->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive) )      );
		item->setText( 0, AddEditMontajes->DatosMontaje["path"]      );	// directorio o iso
		item->setText( 1, AddEditMontajes->DatosMontaje["label"]     );	// etiqueta
		item->setText( 2, AddEditMontajes->DatosMontaje["tipo_as"]   );	// tipo de montaje
		item->setText( 3, AddEditMontajes->DatosMontaje["letter"]    );	// letra de montaje
		item->setText( 4, AddEditMontajes->DatosMontaje["indx_cd"]   );	// index de la unidad de cd-rom
		item->setText( 5, AddEditMontajes->DatosMontaje["opt_mount"] );	// opciones del cd-rom
		item->setText( 6, AddEditMontajes->DatosMontaje["io_ctrl"]   );	// cd/dvd
		item->setText( 7, "x"                                        );	// primer montaje

		PrevierMontajes();
	}
}

void frmDbxAdd::on_btnMount_Edit()
{
	int pos = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	if( ui.twMontajes->topLevelItemCount() > 0 && pos != -1 )
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

		frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
		if( AddEditMontajes->exec() == QDialog::Accepted )
		{
			QString tipoDrive, select_mount;

			tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];

			if( ui.twMontajes->currentItem()->text(7) == "v" )
				select_mount = "s_";
			else
				select_mount = "";

			ui.twMontajes->currentItem()->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive, select_mount) ) );
			ui.twMontajes->currentItem()->setText( 0, AddEditMontajes->DatosMontaje["path"]       );	// directorio o iso
			ui.twMontajes->currentItem()->setText( 1, AddEditMontajes->DatosMontaje["label"]      );	// etiqueta
			ui.twMontajes->currentItem()->setText( 2, AddEditMontajes->DatosMontaje["tipo_as"]    );	// tipo de montaje
			ui.twMontajes->currentItem()->setText( 3, AddEditMontajes->DatosMontaje["letter"]     );	// letra de montaje
			ui.twMontajes->currentItem()->setText( 4, AddEditMontajes->DatosMontaje["indx_cd"]    );	// index de la unidad de cd-rom
			ui.twMontajes->currentItem()->setText( 5, AddEditMontajes->DatosMontaje["opt_mount"]  );	// opciones del cd-rom
			ui.twMontajes->currentItem()->setText( 6, AddEditMontajes->DatosMontaje["io_ctrl"]    );	// cd/dvd
		//	ui.twMontajes->currentItem()->setText( 7, "x" );	// primer montaje
		//	ui.twMontajes->currentItem()->setText( 8, ""  );	// id
		//	ui.twMontajes->currentItem()->setText( 9, AddEditMontajes->DatosMontaje["io_ctrl"]    );	// id_lista

			PrevierMontajes();
		}
	}
}

void frmDbxAdd::on_btnMount_Delete()
{
	if( ui.twMontajes->topLevelItemCount() > 0 )
	{
		int respuesta = QMessageBox::question( this, tr("¿Eliminar Juego...?"), tr( "¿Deseas eliminar este montaje?" ), tr( "Si" ), tr( "Cancelar" ), 0, 1 );
		if( respuesta == 0 )
		{
			fGrl.DeleteItemTree( ui.twMontajes->currentItem() ) ;

			PrevierMontajes();
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
	if( index_1 > 0 && index_1 != -1)
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
	if( index_1 < (ui.twMontajes->topLevelItemCount() - 1) && index_1 != -1 )
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
	item->setIcon( 0 , QIcon(stTheme+"img16/drive_hd.png") );
	item->setText( 0 , fi.absolutePath() );	// directorio o iso
	item->setText( 1 , ""                );	// etiqueta
	item->setText( 2 , "drive"           );	// tipo de montaje
	item->setText( 3 , "C"               );	// letra de montaje
	item->setText( 4 , ""                );	// index de la unidad de cd-rom
	item->setText( 5 , ""                );	// opciones del cd-rom
	item->setText( 6 , ""                );	// cd/dvd IOCtrl
	item->setText( 7 , "x"               );	// primer montaje

	PrevierMontajes();
}

void frmDbxAdd::on_btnMount_Primario()
{
	int indx_mount = 0, num_mount = 0;
	QString tipoDrive, isPrimario;

	int pos = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	if( ui.twMontajes->topLevelItemCount() > 0 && pos != -1 )
	{
		indx_mount = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
		isPrimario = ui.twMontajes->topLevelItem( indx_mount )->text(7);

		for( num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
		{
			tipoDrive = ui.twMontajes->topLevelItem( num_mount )->text(2);

			ui.twMontajes->topLevelItem( num_mount )->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive) ) );
			ui.twMontajes->topLevelItem( num_mount )->setText( 7, "x");
		}

		tipoDrive = ui.twMontajes->topLevelItem( indx_mount )->text(2);

		if( isPrimario == "v")
		{
			ui.twMontajes->topLevelItem( indx_mount )->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive) ) );
			ui.twMontajes->topLevelItem( indx_mount )->setText( 7, "x");
		} else {
			ui.twMontajes->topLevelItem( indx_mount )->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive, "s_") ) );
			ui.twMontajes->topLevelItem( indx_mount )->setText( 7, "v");
		}

		PrevierMontajes();
	}
}
