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

#include "grlida_addedit_dosbox.h"
#include "grlida_addedit_montajes.h"

frmAddEditDosBox::frmAddEditDosBox(QWidget *parent)
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

frmAddEditDosBox::~frmAddEditDosBox()
{
	//
}

bool frmAddEditDosBox::isCorrectNext()
{
	bool siguiente = false;
	if( ui.txtDbx_path_conf->text() != "" )
	{
		siguiente = true;
		if( ui.txtDbx_path_exe->text() == "")
		{
			siguiente = false;
			QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " DOSBox");
		} else {
			siguiente = true;
			if( EditandoJuego != true )
			{
				QFile appConfg( stHomeDir + "confdbx/"+ ui.txtDbx_path_conf->text() );
				if( appConfg.exists() )
				{
					siguiente = false;
					ui.txtDbx_path_conf->setText("");
					QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el DOSBox ya esixte"));
				} else
					siguiente = true;
			}
		}
	} else {
		siguiente = false;
		QMessageBox::information( this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " DOSBox");
	}
	return siguiente;
}

void frmAddEditDosBox::setEditandoJuego(bool editando)
{
	EditandoJuego = editando;
}

void frmAddEditDosBox::createConnections()
{
	connect( ui.btnDbx_FileConfg      , SIGNAL( clicked() ), this, SLOT( on_btnDbx_FileConfg()       ) );
	connect( ui.btnDbx_ExeJuego       , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeJuego()        ) );
	connect( ui.btnDbx_ExeSetup       , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeSetup()        ) );
	connect( ui.btnDbx_mapperfile     , SIGNAL( clicked() ), this, SLOT( on_btnDbx_mapperfile()      ) );
	connect( ui.btnDbx_language       , SIGNAL( clicked() ), this, SLOT( on_btnDbx_language()        ) );
	connect( ui.btnDbx_capturas       , SIGNAL( clicked() ), this, SLOT( on_btnDbx_capturas()        ) );
	connect( ui.btnDbx_musica         , SIGNAL( clicked() ), this, SLOT( on_btnDbx_musica()          ) );
	connect( ui.btnDbx_AddSerial      , SIGNAL( clicked() ), this, SLOT( on_btnDbx_AddSerial()       ) );
	connect( ui.btnMount_Add          , SIGNAL( clicked() ), this, SLOT( on_btnMount_Add()           ) );
	connect( ui.btnMount_Edit         , SIGNAL( clicked() ), this, SLOT( on_btnMount_Edit()          ) );
	connect( ui.btnMount_Delete       , SIGNAL( clicked() ), this, SLOT( on_btnMount_Delete()        ) );
	connect( ui.btnMount_Clear        , SIGNAL( clicked() ), this, SLOT( on_btnMount_Clear()         ) );
	connect( ui.btnMount_Subir        , SIGNAL( clicked() ), this, SLOT( on_btnMount_Subir()         ) );
	connect( ui.btnMount_Bajar        , SIGNAL( clicked() ), this, SLOT( on_btnMount_Bajar()         ) );
	connect( ui.btnMount_AutoCrear    , SIGNAL( clicked() ), this, SLOT( on_btnMount_AutoCrear()     ) );
	connect( ui.btnMount_Primario     , SIGNAL( clicked() ), this, SLOT( on_btnMount_Primario()      ) );
	connect( ui.cbxDbx_Profiles       , SIGNAL( activated(int) ), this, SLOT( on_setProfileGame(int) ) );

// Menu Herramintas de Montaje en Autoexec
	connect( ui.btnToolDbx_mount_HD       , SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_HD()        ) );
	connect( ui.btnToolDbx_mount_CD       , SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_CD()        ) );
	connect( ui.btnToolDbx_mount_Floppy   , SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_Floppy()    ) );
	connect( ui.btnToolDbx_mount_ImgFloppy, SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_ImgFloppy() ) );
	connect( ui.btnToolDbx_mount_ImgCD    , SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_ImgCD()     ) );
	connect( ui.btnToolDbx_mount_ImgHD    , SIGNAL( clicked() ), this, SLOT( on_btnToolDbx_mount_ImgHD()     ) );
}

void frmAddEditDosBox::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	ui.btnDbx_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_ExeSetup->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_FileConfg_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_ExeJuego_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_ExeSetup_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_mapperfile->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_language->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_capturas->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_musica->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDbx_mapperfile_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_language_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_capturas_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_musica_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDbx_AddSerial->setIcon( QIcon(stTheme+"img16/applications.png") );
	ui.btnMount_Add->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.btnMount_Edit->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.btnMount_Delete->setIcon( QIcon(stTheme+"img16/eliminar.png") );
	ui.btnMount_Clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnMount_Subir->setIcon( QIcon(stTheme+"img16/go-up.png") );
	ui.btnMount_Bajar->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.btnMount_AutoCrear->setIcon( QIcon(stTheme+"img16/ejecutar_app.png") );
	ui.btnMount_Primario->setIcon( QIcon(stTheme+"img16/aplicar.png") );

// Menu Herramientas de Montaje en Autoexec
	ui.btnToolDbx_mount_HD->setIcon( QIcon(stTheme+"img16/drive_hd.png") );
	ui.btnToolDbx_mount_CD->setIcon( QIcon(stTheme+"img16/drive_cdrom.png") );
	ui.btnToolDbx_mount_Floppy->setIcon( QIcon(stTheme+"img16/drive_floppy.png") );
	ui.btnToolDbx_mount_ImgFloppy->setIcon( QIcon(stTheme+"img16/floppy_1.png") );
	ui.btnToolDbx_mount_ImgCD->setIcon( QIcon(stTheme+"img16/cd_iso.png") );
	ui.btnToolDbx_mount_ImgHD->setIcon( QIcon(stTheme+"img16/drive_hd.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

void frmAddEditDosBox::CargarConfigDosBox()
{
	fGrl.Cargar_Profile_DFend_ComboBox(stHomeDir+"templates/", ui.cbxDbx_Profiles                   );
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt"    , ui.cbxDbx_sdl_fullresolution         ); // Resolución pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt"    , ui.cbxDbx_sdl_windowresolution       ); // windowresolution
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"        , ui.cbxDbx_sdl_output                 ); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"       , ui.cbxDbx_dosbox_machine             ); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"        , ui.cbxDbx_render_scaler              ); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_hwscale.txt"       , ui.cbxDbx_sdl_hwscale                ); // Escalar por hardware
	fGrl.CargarDatosComboBox(":/datos/dbx_sensitivity.txt"   , ui.cbxDbx_sdl_sensitivity            ); // Sensibilidad del ratn
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt"       , ui.cbxDbx_dosbox_memsize             ); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_core.txt"      , ui.cbxDbx_cpu_core                   ); // Núcleo de la CPU DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_type.txt"      , ui.cbxDbx_cpu_cputype       , 2      ); // Tipo de CPU para usar en la emulación
	fGrl.CargarDatosComboBox(":/datos/dbx_frameskip.txt"     , ui.cbxDbx_render_frameskip           ); // Frameskip DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"        , ui.cbxDbx_cpu_cycles                 ); // Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"        , ui.cbxDbx_cpu_cycles_realmode        ); // Ciclos DOSBox realmode
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"        , ui.cbxDbx_cpu_cycles_limitmode       ); // Ciclos DOSBox limitmode
	fGrl.CargarDatosComboBox(":/datos/dbx_joystick.txt"      , ui.cbxDbx_joystick_type              ); // Emulador de joystick
	fGrl.CargarDatosComboBox(":/datos/dbx_keyboardlayout.txt", ui.cbxDbx_dos_keyboardlayout, 1, true); // keyboardlayout
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt"        , ui.cbxDbx_sblaster_sbtype            ); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_sb_oplmode.txt"    , ui.cbxDbx_sblaster_oplmode           ); // Sound Blaste Opl mode
	fGrl.CargarDatosComboBox(":/datos/dbx_sb_oplemu.txt"     , ui.cbxDbx_sblaster_oplemu            ); // Provider for the OPL emulation
	fGrl.CargarDatosComboBox(":/datos/dbx_sb_oplrate.txt"    , ui.cbxDbx_sblaster_oplrate           ); // Sample rate of OPL
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt"        , ui.cbxDbx_midi_mpu401                ); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt"   , ui.cbxDbx_midi_device                ); // MIDI Device

	ui.cbxDbx_sdl_fullresolution->addItem(QIcon(stTheme+"img16/sinimg.png"), "0x0");

	ui.cbxDbx_cpu_cycles_realmode->setItemText(0,"");
	ui.cbxDbx_cpu_cycles_realmode->removeItem(1);
	ui.cbxDbx_cpu_cycles_limitmode->setItemText(0,"");
	ui.cbxDbx_cpu_cycles_limitmode->removeItem(1);

	ui.cbxDbx_cpu_cycles_protmode->clear();
	ui.cbxDbx_cpu_cycles_protmode->addItem("");
	ui.cbxDbx_cpu_cycles_protmode->addItem("10%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("20%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("30%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("40%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("50%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("60%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("70%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("80%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("90%");
	ui.cbxDbx_cpu_cycles_protmode->addItem("100%");

	ui.cbxDbx_dos_umb->clear();
	ui.cbxDbx_dos_umb->addItem("true");
	ui.cbxDbx_dos_umb->addItem("false");
	ui.cbxDbx_dos_umb->addItem("max");

	QStringList sonido_frecuencias, sonido_Address,	sonido_IRQ, sonido_DMA, sonido_HDMA;
	sonido_frecuencias << "8000" << "11025" << "16000" << "22050" << "32000" << "44100" << "48000" << "49716";
	sonido_Address << "210" << "220" << "240" << "260" << "280" << "2a0" << "2c0" << "2e0" << "300";
	sonido_IRQ << "3" << "5" << "7" << "9" << "10" << "11" << "12";
	sonido_DMA << "0" << "1" << "3" << "5" << "6" << "7";
	sonido_HDMA << "0" << "1" << "3" << "5" << "6" << "7";

	ui.cbxDbx_mixer_rate->clear();
	ui.cbxDbx_mixer_rate->addItems( sonido_frecuencias );
	ui.cbxDbx_sblaster_sbbase->clear();
	ui.cbxDbx_sblaster_sbbase->addItems( sonido_Address );
	ui.cbxDbx_sblaster_irq->clear();
	ui.cbxDbx_sblaster_irq->addItems( sonido_IRQ );
	ui.cbxDbx_sblaster_dma->clear();
	ui.cbxDbx_sblaster_dma->addItems( sonido_DMA );
	ui.cbxDbx_sblaster_hdma->clear();
	ui.cbxDbx_sblaster_hdma->addItems( sonido_HDMA );

	ui.cbxDbx_gus_gusrate->clear();
	ui.cbxDbx_gus_gusrate->addItems( sonido_frecuencias );
	ui.cbxDbx_gus_gusbase->clear();
	ui.cbxDbx_gus_gusbase->addItems( sonido_Address );
	ui.cbxDbx_gus_irq1->clear();
	ui.cbxDbx_gus_irq1->addItems( sonido_IRQ );
	ui.cbxDbx_gus_irq2->clear();
	ui.cbxDbx_gus_irq2->addItems( sonido_IRQ );
	ui.cbxDbx_gus_dma1->clear();
	ui.cbxDbx_gus_dma1->addItems( sonido_DMA );
	ui.cbxDbx_gus_dma2->clear();
	ui.cbxDbx_gus_dma2->addItems( sonido_DMA );
	ui.txtDbx_gus_ultradir->setText("C:\\ULTRASND");

	ui.cbxDbx_midi_mt32rate->clear();
	ui.cbxDbx_midi_mt32rate->addItems( sonido_frecuencias );
	ui.cbxDbx_speaker_pcrate->clear();
	ui.cbxDbx_speaker_pcrate->addItems( sonido_frecuencias );
	ui.cbxDbx_speaker_tandyrate->clear();
	ui.cbxDbx_speaker_tandyrate->addItems( sonido_frecuencias );

	ui.cbxDbx_speaker_tandy->clear();
	ui.cbxDbx_speaker_tandy->addItem("auto");
	ui.cbxDbx_speaker_tandy->addItem("on");
	ui.cbxDbx_speaker_tandy->addItem("off");

	ui.cbxDbx_dserial_comport->clear();
	ui.cbxDbx_dserial_comport->addItem("1");
	ui.cbxDbx_dserial_comport->addItem("2");

	ui.cbxDbx_dserial_defaultbps->clear();
	ui.cbxDbx_dserial_defaultbps->addItem("1200");
	ui.cbxDbx_dserial_defaultbps->addItem("1800");
	ui.cbxDbx_dserial_defaultbps->addItem("2400");
	ui.cbxDbx_dserial_defaultbps->addItem("4800");
	ui.cbxDbx_dserial_defaultbps->addItem("7200");
	ui.cbxDbx_dserial_defaultbps->addItem("9600");
	ui.cbxDbx_dserial_defaultbps->addItem("14400");
	ui.cbxDbx_dserial_defaultbps->addItem("19200");
	ui.cbxDbx_dserial_defaultbps->addItem("38400");
	ui.cbxDbx_dserial_defaultbps->addItem("57600");
	ui.cbxDbx_dserial_defaultbps->addItem("115200");
	ui.cbxDbx_dserial_defaultbps->addItem("128000");

	ui.cbxDbx_dserial_realport->clear();
	ui.cbxDbx_dserial_realport->addItem("COM1");
	ui.cbxDbx_dserial_realport->addItem("COM2");
	ui.cbxDbx_dserial_realport->addItem("COM3");
	ui.cbxDbx_dserial_realport->addItem("COM4");

	ui.cbxDbx_modem_comport->clear();
	ui.cbxDbx_modem_comport->addItem("COM1");
	ui.cbxDbx_modem_comport->addItem("COM2");
	ui.cbxDbx_modem_comport->addItem("COM3");
	ui.cbxDbx_modem_comport->addItem("COM4");

	ui.cbxDbx_dserial_bytesize->clear();
	ui.cbxDbx_dserial_bytesize->addItem("5");
	ui.cbxDbx_dserial_bytesize->addItem("8");

	ui.cbxDbx_dserial_parity->clear();
	ui.cbxDbx_dserial_parity->addItem("N");
	ui.cbxDbx_dserial_parity->addItem("E");
	ui.cbxDbx_dserial_parity->addItem("O");

	ui.cbxDbx_dserial_stopbit->clear();
	ui.cbxDbx_dserial_stopbit->addItem("1");
	ui.cbxDbx_dserial_stopbit->addItem("2");

	ui.cbxDbx_select_serial->clear();
	ui.cbxDbx_select_serial->addItem("serial1");
	ui.cbxDbx_select_serial->addItem("serial2");
	ui.cbxDbx_select_serial->addItem("serial3");
	ui.cbxDbx_select_serial->addItem("serial4");

	ui.cbxDbx_type_serial->clear();
	ui.cbxDbx_type_serial->addItem("disabled");
	ui.cbxDbx_type_serial->addItem("dummy");
	ui.cbxDbx_type_serial->addItem("modem");
	ui.cbxDbx_type_serial->addItem("directserial");

	ui.chkDbx_sdl_fullfixed->setVisible(false); // En desuso

	ui.twMontajes->header()->setStretchLastSection(false);
	ui.twMontajes->header()->setMovable(false);
	ui.twMontajes->setColumnHidden(7, true);
	ui.twMontajes->header()->setResizeMode(6, QHeaderView::Stretch);

	setConfigDefecto();
}

void frmAddEditDosBox::setConfigDefecto()
{
	ui.txtDbx_loadfix_mem->setText("64");
	ui.cbxDbx_dos_keyboardlayout->setCurrentIndex( 1 );   // keyboardlayout
	ui.cbxDbx_sdl_fullresolution->setCurrentIndex( 0 );   // Resolucin pantalla
	ui.cbxDbx_sdl_windowresolution->setCurrentIndex( 0 ); // windowresolution
	ui.cbxDbx_sdl_hwscale->setCurrentIndex( 9 );          // Escalar por hardware
	ui.cbxDbx_sdl_sensitivity->setCurrentIndex( 9 );      // Sensibilidad del ratn
	ui.cbxDbx_sdl_output->setCurrentIndex( 0 );           // Modo de Renderizado
	ui.cbxDbx_dosbox_machine->setCurrentIndex( 8 );       // Tarjeta de Video
	ui.cbxDbx_render_scaler->setCurrentIndex( 1 );        // Escalar y Filtros
	ui.cbxDbx_dosbox_memsize->setCurrentIndex( 5 );       // Cantidad de memoria para DOSBox
	ui.cbxDbx_cpu_core->setCurrentIndex( 0 );             // Núcleo de la CPU DOSBox
	ui.cbxDbx_cpu_cputype->setCurrentIndex( 0 );          // Tipo de CPU para usar en la emulación
	ui.cbxDbx_render_frameskip->setCurrentIndex( 0 );     // Frameskip DOSBox
	ui.cbxDbx_dos_umb->setCurrentIndex( 0 );              // Soporte para memoria UMB
	ui.cbxDbx_cpu_cycles->setCurrentIndex( 0 );           // Ciclos DOSBox
	ui.cbxDbx_cpu_cycles_realmode->setCurrentIndex( 0 );  // Ciclos DOSBox realmode
	ui.cbxDbx_cpu_cycles_limitmode->setCurrentIndex( 0 ); // Ciclos DOSBox limitmode
	ui.cbxDbx_cpu_cycles_protmode->setCurrentIndex( 0 );  // Ciclos DOSBox protected
	ui.cbxDbx_joystick_type->setCurrentIndex( 0 );        // Emulador de joystick
	ui.cbxDbx_mixer_rate->setCurrentIndex( 5 );           // Frecuencia Sonido
	ui.txtDbx_mixer_blocksize->setText("1024");           //
	ui.txtDbx_mixer_prebuffer->setText("20");             //
	ui.cbxDbx_sblaster_sbtype->setCurrentIndex( 5 );      // Tipo Sound Blaste
	ui.cbxDbx_sblaster_oplmode->setCurrentIndex( 0 );     // Sound Blaste Opl mode
	ui.cbxDbx_sblaster_oplemu->setCurrentIndex( 0 );      //
	ui.cbxDbx_sblaster_oplrate->setCurrentIndex( 2 );     //
	ui.cbxDbx_sblaster_sbbase->setCurrentIndex( 1 );      //
	ui.cbxDbx_sblaster_irq->setCurrentIndex( 2 );         //
	ui.cbxDbx_sblaster_dma->setCurrentIndex( 1 );         //
	ui.cbxDbx_sblaster_hdma->setCurrentIndex( 3 );        //
	ui.cbxDbx_gus_gusrate->setCurrentIndex( 5 );          //
	ui.cbxDbx_gus_gusbase->setCurrentIndex( 2 );          //
	ui.cbxDbx_gus_irq1->setCurrentIndex( 1 );             //
	ui.cbxDbx_gus_irq2->setCurrentIndex( 1 );             //
	ui.cbxDbx_gus_dma1->setCurrentIndex( 2 );             //
	ui.cbxDbx_gus_dma2->setCurrentIndex( 2 );             //
	ui.txtDbx_gus_ultradir->setText("C:\\ULTRASND");      // MPU-401
	ui.cbxDbx_midi_device->setCurrentIndex( 0 );          // MIDI Device
	ui.cbxDbx_midi_mt32rate->setCurrentIndex( 5 );        //
	ui.cbxDbx_speaker_pcrate->setCurrentIndex( 5 );       //
	ui.cbxDbx_speaker_tandyrate->setCurrentIndex( 5 );    //
	ui.cbxDbx_speaker_tandy->setCurrentIndex( 0 );        //
	ui.cbxDbx_dserial_comport->setCurrentIndex(0);        //
	ui.cbxDbx_dserial_defaultbps->setCurrentIndex(0);     //
	ui.cbxDbx_dserial_bytesize->setCurrentIndex(1);       //
	ui.cbxDbx_dserial_parity->setCurrentIndex(0);         //
	ui.cbxDbx_dserial_stopbit->setCurrentIndex(0);        //
	ui.cbxDbx_dserial_realport->setCurrentIndex(0);       //
	ui.cbxDbx_modem_comport->setCurrentIndex(0);          //
	ui.txtDbx_modem_listenport->setText("23");            //
	ui.txtDbx_modem_irq->setText("1");                    //
	ui.cbxDbx_select_serial->setCurrentIndex( 0 );        //
	ui.cbxDbx_type_serial->setCurrentIndex(1);            //
	ui.txtDbx_serial_1->setText("dummy");                 //
	ui.txtDbx_serial_2->setText("dummy");                 //
	ui.txtDbx_serial_3->setText("disabled");              //
	ui.txtDbx_serial_4->setText("disabled");              //
	ui.txtDbx_sdl_mapperfile->setText("mapper.txt");      //
}

void frmAddEditDosBox::on_setProfileGame(int row)
{
	if (row >= 0)
	{
		QString profileGame = ui.cbxDbx_Profiles->itemData( row ).toString();
		CargarDatosDosBox("null", profileGame, true);
	}
}

void frmAddEditDosBox::CargarDatosDosBox(QString IDGrl, QString ProfileGame, bool isProfileGame)
{
	QHash<QString, QString> tempProfileDosBox;
	tempProfileDosBox.clear();

	if( isProfileGame )
	{
		if( !ProfileGame.isEmpty() || ProfileGame != "<defecto>" )
			tempProfileDosBox = fGrl.Importar_Profile_DFend(stHomeDir+"templates/"+ProfileGame+".prof");
		else
			setConfigDefecto();

		stItemIDDbx = "";
	} else	// Carga los Datos del DOSBox
		tempProfileDosBox = sql->showConfg_DOSBox( IDGrl );

	CargarDatosDosBox( tempProfileDosBox, IDGrl, isProfileGame);
}

void frmAddEditDosBox::CargarDatosDosBox(QHash<QString, QString> datosDbx, QString IDGrl, bool isProfileGame)
{
	QStringList sdl_priority_templist;

	ui.chkDbx_sdl_fullscreen->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_fullscreen"] ) );												// sdl_fullscreen
	ui.chkDbx_sdl_fulldouble->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_fulldouble"] ) );												// sdl_fulldouble
	ui.chkDbx_sdl_fullfixed->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_fullfixed"] ) );												// sdl_fullfixed
	ui.cbxDbx_sdl_fullresolution->setCurrentIndex( ui.cbxDbx_sdl_fullresolution->findText( datosDbx["Dbx_sdl_fullresolution"] ) );			// sdl_fullresolution
	ui.cbxDbx_sdl_windowresolution->setCurrentIndex( ui.cbxDbx_sdl_windowresolution->findText( datosDbx["Dbx_sdl_windowresolution"] ) );	// sdl_windowresolution
	ui.cbxDbx_sdl_output->setCurrentIndex( ui.cbxDbx_sdl_output->findText( datosDbx["Dbx_sdl_output"] ) );									// sdl_output
	ui.cbxDbx_sdl_hwscale->setCurrentIndex( ui.cbxDbx_sdl_hwscale->findText( datosDbx["Dbx_sdl_hwscale"] ) );								// sdl_hwscale
	ui.chkDbx_sdl_autolock->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_autolock"] ) );													// sdl_autolock
	ui.cbxDbx_sdl_sensitivity->setCurrentIndex( ui.cbxDbx_sdl_sensitivity->findText( datosDbx["Dbx_sdl_sensitivity"] ) );					// sdl_sensitivity
	ui.chkDbx_sdl_waitonerror->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_waitonerror"] ) );											// sdl_waitonerror

	sdl_priority_templist.clear();
	sdl_priority_templist = datosDbx["Dbx_sdl_priority"].split(",");
	if( sdl_priority_templist.value(0) == "lower"  )ui.rbtPrioridadBaja->setChecked(true);		// sdl_priority
	if( sdl_priority_templist.value(0) == "normal" )ui.rbtPrioridadNormal->setChecked(true);	// sdl_priority
	if( sdl_priority_templist.value(0) == "higher" )ui.rbtPrioridadAlta->setChecked(true);		// sdl_priority
	if( sdl_priority_templist.value(0) == "highest")ui.rbtPrioridadMuyAlt->setChecked(true);	// sdl_priority
	if( sdl_priority_templist.value(1) == "lower"  )ui.rbtNoPrioridadBaja->setChecked(true);	// sdl_priority
	if( sdl_priority_templist.value(1) == "normal" )ui.rbtNoPrioridadNormal->setChecked(true);	// sdl_priority
	if( sdl_priority_templist.value(1) == "higher" )ui.rbtNoPrioridadAlta->setChecked(true);	// sdl_priority
	if( sdl_priority_templist.value(1) == "highest")ui.rbtNoPrioridadMuyAlt->setChecked(true);	// sdl_priority

	ui.txtDbx_sdl_mapperfile->setText( datosDbx["Dbx_sdl_mapperfile"] );														// sdl_mapperfile
	ui.chkDbx_sdl_usescancodes->setChecked( fGrl.StrToBool( datosDbx["Dbx_sdl_usescancodes"] ) );								// sdl_usescancodes

	ui.txtDbx_dosbox_language->setText( datosDbx["Dbx_dosbox_language"] );														// dosbox_language
	ui.cbxDbx_dosbox_machine->setCurrentIndex( ui.cbxDbx_dosbox_machine->findText( datosDbx["Dbx_dosbox_machine"] ) );			// dosbox_machine
	ui.txtDbx_dosbox_captures->setText( datosDbx["Dbx_dosbox_captures"] );														// dosbox_captures
	ui.cbxDbx_dosbox_memsize->setEditText( datosDbx["Dbx_dosbox_memsize"] );

	ui.cbxDbx_render_frameskip->setCurrentIndex( ui.cbxDbx_render_frameskip->findText( datosDbx["Dbx_render_frameskip"] ) );	// render_frameskip
	ui.chkDbx_render_aspect->setChecked( fGrl.StrToBool( datosDbx["Dbx_render_aspect"] ) );										// render_aspect
	ui.cbxDbx_render_scaler->setCurrentIndex( ui.cbxDbx_render_scaler->findText( datosDbx["Dbx_render_scaler"] ) );				// render_scaler

	ui.cbxDbx_cpu_core->setCurrentIndex( ui.cbxDbx_cpu_core->findText( datosDbx["Dbx_cpu_core"] ) );							// cpu_core
	ui.cbxDbx_cpu_cputype->setCurrentIndex( ui.cbxDbx_cpu_cputype->findData( datosDbx["Dbx_cpu_cputype"] ) );					// cpu_core
	ui.cbxDbx_cpu_cycles->setCurrentIndex( ui.cbxDbx_cpu_cycles->findText( datosDbx["Dbx_cpu_cycles"] ) );						// cpu_cycles
	ui.cbxDbx_cpu_cycles_realmode->setCurrentIndex( ui.cbxDbx_cpu_cycles_realmode->findText( datosDbx["Dbx_cpu_cycles_realmode"] ) );	// cpu_cycles realmode
	ui.cbxDbx_cpu_cycles_protmode->setCurrentIndex( ui.cbxDbx_cpu_cycles_protmode->findText( datosDbx["Dbx_cpu_cycles_protmode"] ) );	// cpu_cycles protected mode
	ui.cbxDbx_cpu_cycles_limitmode->setCurrentIndex( ui.cbxDbx_cpu_cycles_limitmode->findText( datosDbx["Dbx_cpu_cycles_limitmode"] ) );// cpu_cycles limitmode
	ui.spinDbx_1->setValue( fGrl.StrToInt( datosDbx["Dbx_cpu_cycleup"] ) );														// cpu_cycleup
	ui.spinDbx_2->setValue( fGrl.StrToInt( datosDbx["Dbx_cpu_cycledown"] ) );													// cpu_cycledown

	ui.chkDbx_mixer_nosound->setChecked( fGrl.StrToBool( datosDbx["Dbx_mixer_nosound"] ) );										// mixer_nosound
	ui.cbxDbx_mixer_rate->setCurrentIndex( ui.cbxDbx_mixer_rate->findText( datosDbx["Dbx_mixer_rate"] ) );						// mixer_rate
	ui.txtDbx_mixer_blocksize->setText( datosDbx["Dbx_mixer_blocksize"] );														// mixer_blocksize
	ui.txtDbx_mixer_prebuffer->setText( datosDbx["Dbx_mixer_prebuffer"] );														// mixer_prebuffer

	ui.cbxDbx_midi_mpu401->setCurrentIndex( ui.cbxDbx_midi_mpu401->findText( datosDbx["Dbx_midi_mpu401"] ) );					// midi_mpu401
	ui.chkDbx_midi_intelligent->setChecked( fGrl.StrToBool( datosDbx["Dbx_midi_intelligent"] ) );								// midi_intelligent
	ui.cbxDbx_midi_device->setCurrentIndex( ui.cbxDbx_midi_device->findText( datosDbx["Dbx_midi_device"] ) );					// midi_device
	ui.txtDbx_midi_config->setText( datosDbx["Dbx_midi_config"] );																// midi_config
	ui.cbxDbx_midi_mt32rate->setCurrentIndex( ui.cbxDbx_midi_mt32rate->findText( datosDbx["Dbx_midi_mt32rate"] ) );				// midi_mt32rate

	ui.cbxDbx_sblaster_sbtype->setCurrentIndex( ui.cbxDbx_sblaster_sbtype->findText( datosDbx["Dbx_sblaster_sbtype"] ) );		// sblaster_sbtype
	ui.cbxDbx_sblaster_sbbase->setCurrentIndex( ui.cbxDbx_sblaster_sbbase->findText( datosDbx["Dbx_sblaster_sbbase"] ) );		// sblaster_sbbase
	ui.cbxDbx_sblaster_irq->setCurrentIndex( ui.cbxDbx_sblaster_irq->findText( datosDbx["Dbx_sblaster_irq"] ) );				// sblaster_irq
	ui.cbxDbx_sblaster_dma->setCurrentIndex( ui.cbxDbx_sblaster_dma->findText( datosDbx["Dbx_sblaster_dma"] ) );				// sblaster_dma
	ui.cbxDbx_sblaster_hdma->setCurrentIndex( ui.cbxDbx_sblaster_hdma->findText( datosDbx["Dbx_sblaster_hdma"] ) );				// sblaster_hdma
	ui.chkDbx_sblaster_mixer->setChecked( fGrl.StrToBool( datosDbx["Dbx_sblaster_mixer"] ) );									// sblaster_mixer
	ui.cbxDbx_sblaster_oplmode->setCurrentIndex( ui.cbxDbx_sblaster_oplmode->findText( datosDbx["Dbx_sblaster_oplmode"] ) );	// sblaster_oplmode
	ui.cbxDbx_sblaster_oplemu->setCurrentIndex( ui.cbxDbx_sblaster_oplemu->findText( datosDbx["Dbx_sblaster_oplemu"] ) );		// sblaster_oplemu
	ui.cbxDbx_sblaster_oplrate->setCurrentIndex( ui.cbxDbx_sblaster_oplrate->findText( datosDbx["Dbx_sblaster_oplrate"] ) );	// sblaster_oplrate

	ui.chkDbx_gus_gus->setChecked( fGrl.StrToBool( datosDbx["Dbx_gus_gus"] ) );													// gus_gus
	ui.cbxDbx_gus_gusrate->setCurrentIndex( ui.cbxDbx_gus_gusrate->findText( datosDbx["Dbx_gus_gusrate"] ) );					// gus_gusrate
	ui.cbxDbx_gus_gusbase->setCurrentIndex( ui.cbxDbx_gus_gusbase->findText( datosDbx["Dbx_gus_gusbase"] ) );					// gus_gusbase
	ui.cbxDbx_gus_irq1->setCurrentIndex( ui.cbxDbx_gus_irq1->findText( datosDbx["Dbx_gus_irq1"] ) );							// gus_irq1
	ui.cbxDbx_gus_irq2->setCurrentIndex( ui.cbxDbx_gus_irq2->findText( datosDbx["Dbx_gus_irq2"] ) );							// gus_irq2
	ui.cbxDbx_gus_dma1->setCurrentIndex( ui.cbxDbx_gus_dma1->findText( datosDbx["Dbx_gus_dma1"] ) );							// gus_dma1
	ui.cbxDbx_gus_dma2->setCurrentIndex( ui.cbxDbx_gus_dma2->findText( datosDbx["Dbx_gus_dma2"] ) );							// gus_dma2
	ui.txtDbx_gus_ultradir->setText( datosDbx["Dbx_gus_ultradir"] );															// gus_ultradir

	ui.chkDbx_speaker_pcspeaker->setChecked( fGrl.StrToBool( datosDbx["Dbx_speaker_pcspeaker"] ) );								// speaker_pcspeaker
	ui.cbxDbx_speaker_pcrate->setCurrentIndex( ui.cbxDbx_speaker_pcrate->findText( datosDbx["Dbx_speaker_pcrate"] ) );			// speaker_pcrate
	ui.cbxDbx_speaker_tandy->setCurrentIndex( ui.cbxDbx_speaker_tandy->findText( datosDbx["Dbx_speaker_tandy"] ) );				// speaker_tandy
	ui.cbxDbx_speaker_tandyrate->setCurrentIndex( ui.cbxDbx_speaker_tandyrate->findText( datosDbx["Dbx_speaker_tandyrate"] ) );// speaker_tandyrate
	ui.chkDbx_speaker_disney->setChecked( fGrl.StrToBool( datosDbx["Dbx_speaker_disney"] ) );									// speaker_disney

	ui.cbxDbx_joystick_type->setCurrentIndex( ui.cbxDbx_joystick_type->findText( datosDbx["Dbx_joystick_type"] ) );				// joystick_type
	ui.chkDbx_joystick_timed->setChecked( fGrl.StrToBool( datosDbx["Dbx_joystick_timed"] ) );									// joystick_timed
	ui.chkDbx_joystick_autofire->setChecked( fGrl.StrToBool( datosDbx["Dbx_joystick_autofire"] ) );								// joystick_autofire
	ui.chkDbx_joystick_swap34->setChecked( fGrl.StrToBool( datosDbx["Dbx_joystick_swap34"] ) );									// joystick_swap34
	ui.chkDbx_joystick_buttonwrap->setChecked( fGrl.StrToBool( datosDbx["Dbx_joystick_buttonwrap"] ) );							// joystick_buttonwrap

	ui.chkDbx_modem_modem->setChecked( fGrl.StrToBool( datosDbx["Dbx_modem_modem"] ) );											// modem_modem
	ui.cbxDbx_modem_comport->setCurrentIndex( fGrl.StrToInt( datosDbx["Dbx_modem_comport"] )-1 );								// modem_comport
	ui.txtDbx_modem_listenport->setText( datosDbx["Dbx_modem_listenport"] );													// modem_listenport

	ui.chkDbx_dserial_directserial->setChecked( fGrl.StrToBool( datosDbx["Dbx_dserial_directserial"] ) );						// dserial_directserial
	ui.cbxDbx_dserial_comport->setCurrentIndex( ui.cbxDbx_dserial_comport->findText( datosDbx["Dbx_dserial_comport"] ) );		// dserial_comport
	ui.cbxDbx_dserial_realport->setCurrentIndex( ui.cbxDbx_dserial_realport->findText( datosDbx["Dbx_dserial_realport"] ) );	// serial_realport
	ui.cbxDbx_dserial_defaultbps->setCurrentIndex( ui.cbxDbx_dserial_defaultbps->findText( datosDbx["Dbx_dserial_defaultbps"] ) );	// dserial_defaultbps
	ui.cbxDbx_dserial_parity->setCurrentIndex( ui.cbxDbx_dserial_parity->findText( datosDbx["Dbx_dserial_parity"] ) );			// dserial_parity
	ui.cbxDbx_dserial_bytesize->setCurrentIndex( ui.cbxDbx_dserial_bytesize->findText( datosDbx["Dbx_dserial_bytesize"] ) );	// dserial_bytesize
	ui.cbxDbx_dserial_stopbit->setCurrentIndex( ui.cbxDbx_dserial_stopbit->findText( datosDbx["Dbx_dserial_stopbit"] ) );		// dserial_stopbit
	ui.txtDbx_serial_1->setText( datosDbx["Dbx_serial_1"] );	// serial_1
	ui.txtDbx_serial_2->setText( datosDbx["Dbx_serial_2"] );	// serial_2
	ui.txtDbx_serial_3->setText( datosDbx["Dbx_serial_3"] );	// serial_3
	ui.txtDbx_serial_4->setText( datosDbx["Dbx_serial_4"] );	// serial_4

	ui.chkDbx_dos_xms->setChecked( fGrl.StrToBool( datosDbx["Dbx_dos_xms"] ) );						// dos_xms
	ui.chkDbx_dos_ems->setChecked( fGrl.StrToBool( datosDbx["Dbx_dos_ems"] ) );						// dos_ems
	ui.cbxDbx_dos_umb->setCurrentIndex( ui.cbxDbx_dos_umb->findText( datosDbx["Dbx_dos_umb"] ) );	// dos_umb
	ui.cbxDbx_dos_keyboardlayout->setCurrentIndex( ui.cbxDbx_dos_keyboardlayout->findText( datosDbx["Dbx_dos_keyboardlayout"] ) );	// dos_keyboardlayout

	ui.chkDbx_ipx_ipx->setChecked( fGrl.StrToBool( datosDbx["Dbx_ipx_ipx"] ) );							// ipx_ipx

	ui.txtDbx_Autoexec->setText(datosDbx["Dbx_autoexec"] );												// autoexec

	ui.chkDbx_autoexec->setChecked( fGrl.StrToBool( datosDbx["Dbx_opt_autoexec"] ) );					// opt_autoexec
	ui.chkDbx_loadfix->setChecked( fGrl.StrToBool( datosDbx["Dbx_opt_loadfix"] ) );						// opt_loadfix
	ui.txtDbx_loadfix_mem->setText( datosDbx["Dbx_opt_loadfix_mem"]  );									// opt_loadfix_mem
	ui.chkDbx_consola_dbox->setChecked( fGrl.StrToBool( datosDbx["Dbx_opt_consola_dbox"] ) );			// opt_consola_dbox
	ui.chkDbx_cerrar_dbox->setChecked( fGrl.StrToBool( datosDbx["Dbx_opt_cerrar_dbox"] ) );				// opt_cerrar_dbox
	ui.chkDbx_cycle_sincronizar->setChecked( fGrl.StrToBool( datosDbx["Dbx_opt_cycle_sincronizar"] ) );	// opt_cycle_sincronizar

	if( IDGrl != "null" && !isProfileGame )
		ui.txtDbx_path_conf->setText( datosDbx["Dbx_path_conf"] );				// path_conf

	ui.txtDbx_path_sonido->setText( datosDbx["Dbx_path_sonido"]);				// path_sonido

	if( !datosDbx["Dbx_path_exe"].isEmpty() )
		ui.txtDbx_path_exe->setText( datosDbx["Dbx_path_exe"] );				// path_exe

	if( !datosDbx["Dbx_path_setup"].isEmpty() )
		ui.txtDbx_path_setup->setText( datosDbx["Dbx_path_setup"] );			// path_setup

	ui.txtDbx_parametros_exe->setText( datosDbx["Dbx_parametros_exe"] );		// parametros_exe
	ui.txtDbx_parametros_setup->setText( datosDbx["Dbx_parametros_setup"] );	// parametros_setup

// Carga los Datos de Montaje Obteneendo el Index del DOSBox
	if( IDGrl != "null" && !isProfileGame )
	{
		stItemIDDbx = sql->ItemIDIndex("dbgrl_emu_dosbox", IDGrl);	// id del DOSBox
		CargarDatosDBxMontaje( stItemIDDbx );
	}
}

QHash<QString, QString> frmAddEditDosBox::setDatosDosBox()
{
	QHash<QString, QString> tempProfileDosBox;
	QString  prioritySelect, priorityNoSelect;

	if( ui.rbtPrioridadBaja->isChecked()    ) prioritySelect   = "lower";
	if( ui.rbtPrioridadNormal->isChecked()  ) prioritySelect   = "normal";
	if( ui.rbtPrioridadAlta->isChecked()    ) prioritySelect   = "higher";
	if( ui.rbtPrioridadMuyAlt->isChecked()  ) prioritySelect   = "highest";
	if( ui.rbtNoPrioridadBaja->isChecked()  ) priorityNoSelect = "lower";
	if( ui.rbtNoPrioridadNormal->isChecked()) priorityNoSelect = "normal";
	if( ui.rbtNoPrioridadAlta->isChecked()  ) priorityNoSelect = "higher";
	if( ui.rbtNoPrioridadMuyAlt->isChecked()) priorityNoSelect = "highest";

	tempProfileDosBox.clear();
	tempProfileDosBox["Dbx_id"] = stItemIDDbx;

// sdl_fullscreen
	tempProfileDosBox["Dbx_sdl_fullscreen"] = fGrl.BoolToStr( ui.chkDbx_sdl_fullscreen->isChecked() );
// sdl_fulldouble
	tempProfileDosBox["Dbx_sdl_fulldouble"] = fGrl.BoolToStr( ui.chkDbx_sdl_fulldouble->isChecked() );
// sdl_fullfixed
	tempProfileDosBox["Dbx_sdl_fullfixed"] = fGrl.BoolToStr( ui.chkDbx_sdl_fullfixed->isChecked() );
// sdl_fullresolution
	if( ui.cbxDbx_sdl_fullresolution->currentText() != "")
		tempProfileDosBox["Dbx_sdl_fullresolution"] = ui.cbxDbx_sdl_fullresolution->currentText(); else tempProfileDosBox["Dbx_sdl_fullresolution"] = "original";
// sdl_windowresolution
	if( ui.cbxDbx_sdl_windowresolution->currentText() != "" )
		tempProfileDosBox["Dbx_sdl_windowresolution"] = ui.cbxDbx_sdl_windowresolution->currentText(); else tempProfileDosBox["Dbx_sdl_windowresolution"] = "original";
// sdl_output
	if( ui.cbxDbx_sdl_output->currentText() != "" )
		tempProfileDosBox["Dbx_sdl_output"] = ui.cbxDbx_sdl_output->currentText(); else tempProfileDosBox["Dbx_sdl_output"] = "surface";
// sdl_hwscale
	if( ui.cbxDbx_sdl_hwscale->currentText() != "" )
		tempProfileDosBox["Dbx_sdl_hwscale"] = ui.cbxDbx_sdl_hwscale->currentText(); else tempProfileDosBox["Dbx_sdl_hwscale"] = "1.00";
// sdl_autolock
	tempProfileDosBox["Dbx_sdl_autolock"] = fGrl.BoolToStr( ui.chkDbx_sdl_autolock->isChecked() );
// sdl_sensitivity
	if( ui.cbxDbx_sdl_sensitivity->currentText() != "" )
		tempProfileDosBox["Dbx_sdl_sensitivity"] = ui.cbxDbx_sdl_sensitivity->currentText(); else tempProfileDosBox["Dbx_sdl_sensitivity"] = "100";
// sdl_waitonerror
	tempProfileDosBox["Dbx_sdl_waitonerror"] = fGrl.BoolToStr( ui.chkDbx_sdl_waitonerror->isChecked() );
// sdl_priority
	if(prioritySelect != ""  && priorityNoSelect != ""  )
		tempProfileDosBox["Dbx_sdl_priority"] = prioritySelect+","+priorityNoSelect; else tempProfileDosBox["Dbx_sdl_priority"] = "higher,normal";
// sdl_mapperfile
	if( ui.txtDbx_sdl_mapperfile->text() != "" )
		tempProfileDosBox["Dbx_sdl_mapperfile"] = ui.txtDbx_sdl_mapperfile->text(); else tempProfileDosBox["Dbx_sdl_mapperfile"] = "mapper.txt";
// sdl_usescancodes
	tempProfileDosBox["Dbx_sdl_usescancodes"] = fGrl.BoolToStr( ui.chkDbx_sdl_usescancodes->isChecked() );

// dosbox_language
	if( ui.txtDbx_dosbox_language->text() != "" )
		tempProfileDosBox["Dbx_dosbox_language"] = ui.txtDbx_dosbox_language->text(); else tempProfileDosBox["Dbx_dosbox_language"] = "";
// dosbox_machine
	if( ui.cbxDbx_dosbox_machine->currentText() != "" )
		tempProfileDosBox["Dbx_dosbox_machine"] = ui.cbxDbx_dosbox_machine->currentText(); else tempProfileDosBox["Dbx_dosbox_machine"] = "svga_s3";
// dosbox_captures
	if( ui.txtDbx_dosbox_captures->text() != "" )
		tempProfileDosBox["Dbx_dosbox_captures"] = ui.txtDbx_dosbox_captures->text(); else tempProfileDosBox["Dbx_dosbox_captures"] = "";
// dosbox_memsize
	if( ui.cbxDbx_dosbox_memsize->currentText() != "" )
		tempProfileDosBox["Dbx_dosbox_memsize"] = ui.cbxDbx_dosbox_memsize->currentText(); else tempProfileDosBox["Dbx_dosbox_memsize"] = "16";

// render_frameskip
	if( ui.cbxDbx_render_frameskip->currentText() != "" )
		tempProfileDosBox["Dbx_render_frameskip"] = ui.cbxDbx_render_frameskip->currentText(); else tempProfileDosBox["Dbx_render_frameskip"] = "0";
// render_aspect
	tempProfileDosBox["Dbx_render_aspect"] = fGrl.BoolToStr( ui.chkDbx_render_aspect->isChecked() );
// render_scaler
	if( ui.cbxDbx_render_scaler->currentText() != "" )
		tempProfileDosBox["Dbx_render_scaler"] = ui.cbxDbx_render_scaler->currentText(); else tempProfileDosBox["Dbx_render_scaler"] = "normal2x";

// cpu_core
	if( ui.cbxDbx_cpu_core->currentText() != "" )
		tempProfileDosBox["Dbx_cpu_core"] = ui.cbxDbx_cpu_core->currentText(); else tempProfileDosBox["Dbx_cpu_core"] = "auto";
// cpu_cputype
	if( ui.cbxDbx_cpu_cputype->itemData( ui.cbxDbx_cpu_cputype->currentIndex() ).toString() != "" )
		tempProfileDosBox["Dbx_cpu_cputype"] = ui.cbxDbx_cpu_cputype->itemData( ui.cbxDbx_cpu_cputype->currentIndex() ).toString(); else tempProfileDosBox["Dbx_cpu_cputype"] = "auto";
// cpu_cycles
	if( ui.cbxDbx_cpu_cycles->currentText() != "" )
		tempProfileDosBox["Dbx_cpu_cycles"] = ui.cbxDbx_cpu_cycles->currentText(); else tempProfileDosBox["Dbx_cpu_cycles"] = "auto";
// cpu_cycles_realmode
	if( ui.cbxDbx_cpu_cycles_realmode->currentText() != "" )
		tempProfileDosBox["Dbx_cpu_cycles_realmode"] = ui.cbxDbx_cpu_cycles_realmode->currentText(); else tempProfileDosBox["Dbx_cpu_cycles_realmode"] = "";
// cpu_cycles_protmode
	if( ui.cbxDbx_cpu_cycles_protmode->currentText() != "" )
		tempProfileDosBox["Dbx_cpu_cycles_protmode"] = ui.cbxDbx_cpu_cycles_protmode->currentText(); else tempProfileDosBox["Dbx_cpu_cycles_protmode"] = "";
// cpu_cycles_limitmode
	if( ui.cbxDbx_cpu_cycles_limitmode->currentText() != "" )
		tempProfileDosBox["Dbx_cpu_cycles_limitmode"] = ui.cbxDbx_cpu_cycles_limitmode->currentText(); else tempProfileDosBox["Dbx_cpu_cycles_limitmode"] = "";
// cpu_cycleup
	if( ui.spinDbx_1->value() >= 0)
		tempProfileDosBox["Dbx_cpu_cycleup"] = fGrl.IntToStr( ui.spinDbx_1->value() ); else tempProfileDosBox["Dbx_cpu_cycleup"] = "10";
// cpu_cycledown
	if( ui.spinDbx_2->value() >= 0)
		tempProfileDosBox["Dbx_cpu_cycledown"] = fGrl.IntToStr( ui.spinDbx_2->value() ); else tempProfileDosBox["Dbx_cpu_cycledown"] = "20";

// mixer_nosound
	if( ui.chkDbx_mixer_nosound->isChecked() )
		tempProfileDosBox["Dbx_mixer_nosound"] = "true"; else tempProfileDosBox["Dbx_mixer_nosound"] = "false";
// mixer_rate
	if( ui.cbxDbx_mixer_rate->currentText() != "" )
		tempProfileDosBox["Dbx_mixer_rate"] = ui.cbxDbx_mixer_rate->currentText(); else tempProfileDosBox["Dbx_mixer_rate"] = "44100";
// mixer_blocksize
	if( ui.txtDbx_mixer_blocksize->text() != "" )
		tempProfileDosBox["Dbx_mixer_blocksize"] = ui.txtDbx_mixer_blocksize->text(); else tempProfileDosBox["Dbx_mixer_blocksize"] = "1024";
// mixer_prebuffer
	if( ui.txtDbx_mixer_prebuffer->text() != "" )
		tempProfileDosBox["Dbx_mixer_prebuffer"] = ui.txtDbx_mixer_prebuffer->text(); else tempProfileDosBox["Dbx_mixer_prebuffer"] = "20";

// midi_mpu401
	if( ui.cbxDbx_midi_mpu401->currentText() != "" )
		tempProfileDosBox["Dbx_midi_mpu401"] = ui.cbxDbx_midi_mpu401->currentText(); else tempProfileDosBox["Dbx_midi_mpu401"] = "intelligent";
// midi_intelligent
	tempProfileDosBox["Dbx_midi_intelligent"] = fGrl.BoolToStr( ui.chkDbx_midi_intelligent->isChecked() );
// midi_device
	if( ui.cbxDbx_midi_device->currentText() != "" )
		tempProfileDosBox["Dbx_midi_device"] = ui.cbxDbx_midi_device->currentText(); else tempProfileDosBox["Dbx_midi_device"] = "default";
// midi_config
	if( ui.txtDbx_midi_config->text() != "" )
		tempProfileDosBox["Dbx_midi_config"] = ui.txtDbx_midi_config->text();else	tempProfileDosBox["Dbx_midi_config"] = "";
// midi_mt32rate
	if( ui.cbxDbx_midi_mt32rate->currentText() != "" )
		tempProfileDosBox["Dbx_midi_mt32rate"] = ui.cbxDbx_midi_mt32rate->currentText(); else tempProfileDosBox["Dbx_midi_mt32rate"] = "44100";

// sblaster_sbtype
	if( ui.cbxDbx_sblaster_sbtype->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_sbtype"] = ui.cbxDbx_sblaster_sbtype->currentText(); else tempProfileDosBox["Dbx_sblaster_sbtype"] = "sb16";
// sblaster_sbbase
	if( ui.cbxDbx_sblaster_sbbase->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_sbbase"] = ui.cbxDbx_sblaster_sbbase->currentText(); else tempProfileDosBox["Dbx_sblaster_sbbase"] = "220";
// sblaster_irq
	if( ui.cbxDbx_sblaster_irq->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_irq"] = ui.cbxDbx_sblaster_irq->currentText(); else tempProfileDosBox["Dbx_sblaster_irq"] = "7";
// sblaster_dma
	if( ui.cbxDbx_sblaster_dma->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_dma"] = ui.cbxDbx_sblaster_dma->currentText(); else tempProfileDosBox["Dbx_sblaster_dma"] = "1";
// sblaster_hdma
	if( ui.cbxDbx_sblaster_hdma->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_hdma"] = ui.cbxDbx_sblaster_hdma->currentText(); else tempProfileDosBox["Dbx_sblaster_hdma"] = "5";
// sblaster_mixer
	tempProfileDosBox["Dbx_sblaster_mixer"] = fGrl.BoolToStr( ui.chkDbx_sblaster_mixer->isChecked() );
// sblaster_oplmode
	if( ui.cbxDbx_sblaster_oplmode->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_oplmode"] = ui.cbxDbx_sblaster_oplmode->currentText(); else tempProfileDosBox["Dbx_sblaster_oplmode"] = "auto";
// sblaster_oplemu
	if( ui.cbxDbx_sblaster_oplemu->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_oplemu"] = ui.cbxDbx_sblaster_oplemu->currentText(); else tempProfileDosBox["Dbx_sblaster_oplemu"] = "default";
// sblaster_oplrate
	if( ui.cbxDbx_sblaster_oplrate->currentText() != "" )
		tempProfileDosBox["Dbx_sblaster_oplrate"] = ui.cbxDbx_sblaster_oplrate->currentText(); else tempProfileDosBox["Dbx_sblaster_oplrate"] = "44100";

// gus_gus
	tempProfileDosBox["Dbx_gus_gus"] = fGrl.BoolToStr( ui.chkDbx_gus_gus->isChecked() );
// gus_gusrate
	if( ui.cbxDbx_gus_gusrate->currentText() != "" )
		tempProfileDosBox["Dbx_gus_gusrate"] = ui.cbxDbx_gus_gusrate->currentText(); else tempProfileDosBox["Dbx_gus_gusrate"] = "44100";
// gus_gusbase
	if( ui.cbxDbx_gus_gusbase->currentText() != "" )
		tempProfileDosBox["Dbx_gus_gusbase"] = ui.cbxDbx_gus_gusbase->currentText(); else tempProfileDosBox["Dbx_gus_gusbase"] = "240";
// gus_irq1
	if( ui.cbxDbx_gus_irq1->currentText() != "" )
		tempProfileDosBox["Dbx_gus_irq1"] = ui.cbxDbx_gus_irq1->currentText(); else tempProfileDosBox["Dbx_gus_irq1"] = "5";
// gus_irq2
	if( ui.cbxDbx_gus_irq2->currentText() != "" )
		tempProfileDosBox["Dbx_gus_irq2"] = ui.cbxDbx_gus_irq2->currentText(); else tempProfileDosBox["Dbx_gus_irq2"] = "5";
// gus_dma1
	if( ui.cbxDbx_gus_dma1->currentText() != "" )
		tempProfileDosBox["Dbx_gus_dma1"] = ui.cbxDbx_gus_dma1->currentText(); else tempProfileDosBox["Dbx_gus_dma1"] = "3";
// gus_dma2
	if( ui.cbxDbx_gus_dma2->currentText() != "" )
		tempProfileDosBox["Dbx_gus_dma2"] = ui.cbxDbx_gus_dma2->currentText(); else tempProfileDosBox["Dbx_gus_dma2"] = "3";
// gus_ultradir
	if( ui.txtDbx_gus_ultradir->text() != "" )
		tempProfileDosBox["Dbx_gus_ultradir"] = ui.txtDbx_gus_ultradir->text(); else tempProfileDosBox["Dbx_gus_ultradir"] = "C:/ULTRASND";

// speaker_pcspeaker
	tempProfileDosBox["Dbx_speaker_pcspeaker"] = fGrl.BoolToStr( ui.chkDbx_speaker_pcspeaker->isChecked() );
// speaker_pcrate
	if( ui.cbxDbx_speaker_pcrate->currentText() != "" )
		tempProfileDosBox["Dbx_speaker_pcrate"] = ui.cbxDbx_speaker_pcrate->currentText(); else tempProfileDosBox["Dbx_speaker_pcrate"] = "44100";
// speaker_tandy
	if( ui.cbxDbx_speaker_tandy->currentText() != "" )
		tempProfileDosBox["Dbx_speaker_tandy"] = ui.cbxDbx_speaker_tandy->currentText(); else tempProfileDosBox["Dbx_speaker_tandy"] = "auto";
// speaker_tandyrate
	if( ui.cbxDbx_speaker_tandyrate->currentText() != "" )
		tempProfileDosBox["Dbx_speaker_tandyrate"] = ui.cbxDbx_speaker_tandyrate->currentText(); else tempProfileDosBox["Dbx_speaker_tandyrate"] = "44100";
// speaker_disney
	tempProfileDosBox["Dbx_speaker_disney"] = fGrl.BoolToStr( ui.chkDbx_speaker_disney->isChecked() );

// joystick_type
	if( ui.cbxDbx_joystick_type->currentText() != "" )
		tempProfileDosBox["Dbx_joystick_type"] = ui.cbxDbx_joystick_type->currentText(); else tempProfileDosBox["Dbx_joystick_type"] = "auto";
// joystick_timed
	tempProfileDosBox["Dbx_joystick_timed"] = fGrl.BoolToStr( ui.chkDbx_joystick_timed->isChecked() );
// joystick_autofire
	tempProfileDosBox["Dbx_joystick_autofire"] = fGrl.BoolToStr( ui.chkDbx_joystick_autofire->isChecked() );
// joystick_swap34
	tempProfileDosBox["Dbx_joystick_swap34"] = fGrl.BoolToStr( ui.chkDbx_joystick_swap34->isChecked() );
// joystick_buttonwrap
	tempProfileDosBox["Dbx_joystick_buttonwrap"] = fGrl.BoolToStr( ui.chkDbx_joystick_buttonwrap->isChecked() );

// modem_modem
	tempProfileDosBox["Dbx_modem_modem"] = fGrl.BoolToStr( ui.chkDbx_modem_modem->isChecked() );
// modem_comport
	if( ui.cbxDbx_modem_comport->currentText() != "" )
		tempProfileDosBox["Dbx_modem_comport"] = fGrl.IntToStr( ui.cbxDbx_modem_comport->currentIndex()+1); else tempProfileDosBox["Dbx_modem_comport"] = "2";
// modem_listenport
	if( ui.txtDbx_modem_listenport->text() != "" )
		tempProfileDosBox["Dbx_modem_listenport"] = ui.txtDbx_modem_listenport->text(); else tempProfileDosBox["Dbx_modem_listenport"] = "";

// dserial_directserial
	tempProfileDosBox["Dbx_dserial_directserial"] = fGrl.BoolToStr( ui.chkDbx_dserial_directserial->isChecked() );
// dserial_comport
	if( ui.cbxDbx_dserial_comport->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_comport"] = ui.cbxDbx_dserial_comport->currentText(); else tempProfileDosBox["Dbx_dserial_comport"] = "1";
// dserial_realport
	if( ui.cbxDbx_dserial_realport->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_realport"] = ui.cbxDbx_dserial_realport->currentText(); else tempProfileDosBox["Dbx_dserial_realport"] = "COM1";
// dserial_defaultbps
	if( ui.cbxDbx_dserial_defaultbps->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_defaultbps"] = ui.cbxDbx_dserial_defaultbps->currentText(); else tempProfileDosBox["Dbx_dserial_defaultbps"] = "1200";
// dserial_parity
	if( ui.cbxDbx_dserial_parity->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_parity"] = ui.cbxDbx_dserial_parity->currentText(); else tempProfileDosBox["Dbx_dserial_parity"] = "N";
// dserial_bytesize
	if( ui.cbxDbx_dserial_bytesize->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_bytesize"] = ui.cbxDbx_dserial_bytesize->currentText(); else tempProfileDosBox["Dbx_dserial_bytesize"] = "8";
// dserial_stopbit
	if( ui.cbxDbx_dserial_stopbit->currentText() != "" )
		tempProfileDosBox["Dbx_dserial_stopbit"] = ui.cbxDbx_dserial_stopbit->currentText(); else tempProfileDosBox["Dbx_dserial_stopbit"] = "1";

// serial_1
	if( ui.txtDbx_serial_1->text() != "" )
		tempProfileDosBox["Dbx_serial_1"] = ui.txtDbx_serial_1->text(); else tempProfileDosBox["Dbx_serial_1"] = "";
// serial_2
	if( ui.txtDbx_serial_2->text() != "" )
		tempProfileDosBox["Dbx_serial_2"] = ui.txtDbx_serial_2->text(); else tempProfileDosBox["Dbx_serial_2"] = "";
// serial_3
	if( ui.txtDbx_serial_3->text() != "" )
		tempProfileDosBox["Dbx_serial_3"] = ui.txtDbx_serial_3->text(); else tempProfileDosBox["Dbx_serial_3"] = "";
// serial_4
	if( ui.txtDbx_serial_4->text() != "" )
		tempProfileDosBox["Dbx_serial_4"] = ui.txtDbx_serial_4->text(); else tempProfileDosBox["Dbx_serial_4"] = "";

// dos_xms
	tempProfileDosBox["Dbx_dos_xms"] = fGrl.BoolToStr( ui.chkDbx_dos_xms->isChecked() );
// dos_ems
	tempProfileDosBox["Dbx_dos_ems"] = fGrl.BoolToStr( ui.chkDbx_dos_ems->isChecked() );
// dos_umb
	if( ui.cbxDbx_dos_umb->currentText() != "" )
		tempProfileDosBox["Dbx_dos_umb"] = ui.cbxDbx_dos_umb->currentText(); else tempProfileDosBox["Dbx_dos_umb"] = "true";
// dos_keyboardlayout
	if( ui.cbxDbx_dos_keyboardlayout->currentText() != "" )
		tempProfileDosBox["Dbx_dos_keyboardlayout"] = ui.cbxDbx_dos_keyboardlayout->currentText(); else tempProfileDosBox["Dbx_dos_keyboardlayout"] = "none";

// ipx_ipx
	tempProfileDosBox["Dbx_ipx_ipx"] = fGrl.BoolToStr( ui.chkDbx_ipx_ipx->isChecked() );
// autoexec
	if( ui.txtDbx_Autoexec->toPlainText() != "" )
		tempProfileDosBox["Dbx_autoexec"] = ui.txtDbx_Autoexec->toPlainText(); else tempProfileDosBox["Dbx_autoexec"] = "";

// opt_autoexec
	tempProfileDosBox["Dbx_opt_autoexec"] = fGrl.BoolToStr( ui.chkDbx_autoexec->isChecked() );
// opt_loadfix
	tempProfileDosBox["Dbx_opt_loadfix"] = fGrl.BoolToStr( ui.chkDbx_loadfix->isChecked() );
// opt_loadfix_mem
	if( ui.txtDbx_loadfix_mem->text() != "" )
		tempProfileDosBox["Dbx_opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text(); else  tempProfileDosBox["Dbx_opt_loadfix_mem"] = "";
// opt_consola_dbox
	tempProfileDosBox["Dbx_opt_consola_dbox"] = fGrl.BoolToStr( ui.chkDbx_consola_dbox->isChecked() );
// opt_cerrar_dbox
	tempProfileDosBox["Dbx_opt_cerrar_dbox"] = fGrl.BoolToStr( ui.chkDbx_cerrar_dbox->isChecked() );
// opt_cycle_sincronizar
	tempProfileDosBox["Dbx_opt_cycle_sincronizar"] = fGrl.BoolToStr( ui.chkDbx_cycle_sincronizar->isChecked() );

// path_conf
	if( ui.txtDbx_path_conf->text() != "" )
		tempProfileDosBox["Dbx_path_conf"] = ui.txtDbx_path_conf->text(); else tempProfileDosBox["Dbx_path_conf"] = "";
// path_sonido
	if( ui.txtDbx_path_sonido->text() != "" )
		tempProfileDosBox["Dbx_path_sonido"] = ui.txtDbx_path_sonido->text()	; else tempProfileDosBox["Dbx_path_sonido"] = "";
// path_exe
	if( ui.txtDbx_path_exe->text() != "" )
		tempProfileDosBox["Dbx_path_exe"] = ui.txtDbx_path_exe->text()	; else tempProfileDosBox["Dbx_path_exe"] = "";
// path_setup
	if( ui.txtDbx_path_setup->text() != "" )
		tempProfileDosBox["Dbx_path_setup"] = ui.txtDbx_path_setup->text(); else tempProfileDosBox["Dbx_path_setup"] = "";
// parametros_exe
	if( ui.txtDbx_parametros_exe->text() != "" )
		tempProfileDosBox["Dbx_parametros_exe"] = ui.txtDbx_parametros_exe->text(); else tempProfileDosBox["Dbx_parametros_exe"] = "";
// parametros_setup
	if( ui.txtDbx_parametros_setup->text() != "" )
		tempProfileDosBox["Dbx_parametros_setup"] = ui.txtDbx_parametros_setup->text(); else tempProfileDosBox["Dbx_parametros_setup"] = "";

	return tempProfileDosBox;
}

void frmAddEditDosBox::PrevierMontajes()
{
	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["Dbx_path_exe"]        = ui.txtDbx_path_exe->text();
	datos_montaje["Dbx_parametros_exe"]  = ui.txtDbx_parametros_exe->text();
	datos_montaje["Dbx_opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();
	datos_montaje["Dbx_opt_loadfix"]     = fGrl.BoolToStr( ui.chkDbx_loadfix->isChecked() );
	datos_montaje["Dbx_opt_cerrar_dbox"] = fGrl.BoolToStr( ui.chkDbx_cerrar_dbox->isChecked() );

	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmAddEditDosBox::CargarDatosDBxMontaje(QString IDdbx)
{
	QSqlQuery query;
	QString tipoDrive, select_mount;
	query.exec("SELECT * FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+IDdbx+" ORDER BY id_lista");
	if( query.first() )
	{
		ui.twMontajes->clear();
		do {
			QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
			tipoDrive.clear();
			tipoDrive = query.record().value("tipo_as").toString();

			if(query.record().value("select_mount").toString()=="v")
				select_mount = "s_";
			else
				select_mount = "";

			item->setIcon( 0, QIcon( fGrl.getIconMount(tipoDrive, select_mount) ) );
			item->setText( 0, query.record().value("path").toString()         );	// path			- directorio o iso
			item->setText( 1, query.record().value("label").toString()        );	// label		- etiqueta
			item->setText( 2, query.record().value("tipo_as").toString()      );	// tipo_as		- tipo de montaje
			item->setText( 3, query.record().value("letter").toString()       );	// letter		- letra de montaje
			item->setText( 4, query.record().value("indx_cd").toString()      );	// indx_cd		- index de la unidad de cd-rom
			item->setText( 5, query.record().value("opt_mount").toString()    );	// opt_mount	- opciones del cd-rom
			item->setText( 6, query.record().value("io_ctrl").toString()      );	// io_ctrl		- cd/dvd
			item->setText( 7, query.record().value("select_mount").toString() );	// select_mount	- primer montaje
			item->setText( 8, query.record().value("id").toString()           );	// id
			item->setText( 9, query.record().value("id_lista").toString()     );	// id_lista		- id_lista
		} while (query.next());
		PrevierMontajes();
	}
}

void frmAddEditDosBox::CargarDatosDBxMontaje(QTreeWidget *twMontajesDbx)
{
	ui.twMontajes->clear();
	for(int num_mount = 0; num_mount < twMontajesDbx->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *itemDbx = twMontajesDbx->topLevelItem( num_mount );

		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		item->setIcon( 0, itemDbx->icon(0) );
		item->setText( 0, itemDbx->text(0) );	// path			- directorio o iso
		item->setText( 1, itemDbx->text(1) );	// label		- etiqueta
		item->setText( 2, itemDbx->text(2) );	// tipo_as		- tipo de montaje
		item->setText( 3, itemDbx->text(3) );	// letter		- letra de montaje
		item->setText( 4, itemDbx->text(4) );	// indx_cd		- index de la unidad de cd-rom
		item->setText( 5, itemDbx->text(5) );	// opt_mount	- opciones del cd-rom
		item->setText( 6, itemDbx->text(6) );	// io_ctrl		- cd/dvd
		item->setText( 7, itemDbx->text(7) );	// select_mount	- primer montaje
		item->setText( 8, itemDbx->text(8) );	// id
		item->setText( 9, itemDbx->text(9) );	// id_lista		- id_lista
	}

	PrevierMontajes();
}

void frmAddEditDosBox::on_btnDbx_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."),  stHomeDir + "confdbx/", ui.txtDbx_path_conf->text(), "Config DOSBox (*.conf);;"+ tr("Todos los archivo") +" (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );

		if( EditandoJuego != true )
		{
			if( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el DOSBox ya esixte"));
		}

		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
		str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");
		ui.txtDbx_path_conf->setText( str );
	} else
		ui.txtDbx_path_conf->setText("");
}

void frmAddEditDosBox::on_btnDbx_ExeJuego()
{
	ui.txtDbx_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_exe"].toString(), ui.txtDbx_path_exe->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_exe", GRLConfig["Dbx_path_exe"].toString() );
}

void frmAddEditDosBox::on_btnDbx_ExeSetup()
{
	ui.txtDbx_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_setup"].toString(), ui.txtDbx_path_setup->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_setup->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_setup", GRLConfig["Dbx_path_setup"].toString() );
}

void frmAddEditDosBox::on_btnDbx_mapperfile()
{
	ui.txtDbx_sdl_mapperfile->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_sdl_mapperfile"].toString(), ui.txtDbx_sdl_mapperfile->text(), "KeyMapper (*.map *.txt);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_sdl_mapperfile->text() );
	if( fi.exists() )
		GRLConfig["Dbx_sdl_mapperfile"] = fi.absolutePath();
	else
		GRLConfig["Dbx_sdl_mapperfile"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_sdl_mapperfile", GRLConfig["Dbx_sdl_mapperfile"].toString() );
}

void frmAddEditDosBox::on_btnDbx_language()
{
	ui.txtDbx_dosbox_language->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_dosbox_language"].toString(), ui.txtDbx_dosbox_language->text(), tr("Idioma") +" (*.lng *.txt);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_dosbox_language->text() );
	if( fi.exists() )
		GRLConfig["Dbx_dosbox_language"] = fi.absolutePath();
	else
		GRLConfig["Dbx_dosbox_language"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_language", GRLConfig["Dbx_dosbox_language"].toString() );
}

void frmAddEditDosBox::on_btnDbx_capturas()
{
	ui.txtDbx_dosbox_captures->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Dbx_dosbox_captures"].toString(), ui.txtDbx_dosbox_captures->text() ) );

	QDir dir( ui.txtDbx_dosbox_captures->text() );
	if( dir.exists() )
		GRLConfig["Dbx_dosbox_captures"] = ui.txtDbx_dosbox_captures->text();
	else
		GRLConfig["Dbx_dosbox_captures"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_captures", GRLConfig["Dbx_dosbox_captures"].toString() );
}

void frmAddEditDosBox::on_btnDbx_musica()
{
	ui.txtDbx_path_sonido->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Dbx_path_sonido"].toString(), ui.txtDbx_path_sonido->text() ) );

	QDir dir( ui.txtDbx_path_sonido->text() );
	if( dir.exists() )
		GRLConfig["Dbx_path_sonido"] = ui.txtDbx_path_sonido->text();
	else
		GRLConfig["Dbx_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_sonido", GRLConfig["Dbx_path_sonido"].toString() );
}

void frmAddEditDosBox::on_btnDbx_AddSerial()
{
	switch ( ui.cbxDbx_select_serial->currentIndex() )
	{
		case 0:	// serial 1
			ui.txtDbx_serial_1->setText(setOpcionesSerial());
		break;
		case 1:	// serial 2
			ui.txtDbx_serial_2->setText(setOpcionesSerial());
		break;
		case 2:	// serial 3
			ui.txtDbx_serial_3->setText(setOpcionesSerial());
		break;
		case 3:	// serial 4
			ui.txtDbx_serial_4->setText(setOpcionesSerial());
		break;
	}
}

QString frmAddEditDosBox::setOpcionesSerial()
{
	QString Serial_Dbx;
	switch ( ui.cbxDbx_type_serial->currentIndex() )
	{
		case 0:	// diasabled
			Serial_Dbx = ui.cbxDbx_type_serial->currentText();
		break;
		case 1:	// dummy
			Serial_Dbx = ui.cbxDbx_type_serial->currentText();
		break;
		case 2:	// modem
			Serial_Dbx = ui.cbxDbx_type_serial->currentText() + " " +
						"listenport:" + ui.txtDbx_modem_listenport->text() + " " +
						"realport:" + ui.cbxDbx_dserial_realport->currentText() + " " +
						"startbps:" + ui.cbxDbx_dserial_defaultbps->currentText() + " " +
						"parity:" + ui.cbxDbx_dserial_parity->currentText() + " " +
						"bytesize:" + ui.cbxDbx_dserial_bytesize->currentText() + " " +
						"stopbits:" + ui.cbxDbx_dserial_stopbit->currentText() + " " +
						"irq:" + ui.txtDbx_modem_irq->text();
		break;
		case 3:			// directserial
			Serial_Dbx = ui.cbxDbx_type_serial->currentText() + " " +
						"realport:" + ui.cbxDbx_dserial_realport->currentText() + " " +
						"startbps:" + ui.cbxDbx_dserial_defaultbps->currentText() + " " +
						"parity:" + ui.cbxDbx_dserial_parity->currentText() + " " +
						"bytesize:" + ui.cbxDbx_dserial_bytesize->currentText() + " " +
						"stopbits:" + ui.cbxDbx_dserial_stopbit->currentText() + " " +
						"irq:" + ui.txtDbx_modem_irq->text();
			break;
	}
	return Serial_Dbx;
}

void frmAddEditDosBox::on_btnMount_Add()
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

		if( EditandoJuego == true )
		{
			QHash<QString, QString> datos_montaje_nuevo;
			datos_montaje_nuevo.clear();
			datos_montaje_nuevo["id_lista"]     = "";
			datos_montaje_nuevo["path"]         = AddEditMontajes->DatosMontaje["path"];
			datos_montaje_nuevo["label"]        = AddEditMontajes->DatosMontaje["label"];
			datos_montaje_nuevo["tipo_as"]      = AddEditMontajes->DatosMontaje["tipo_as"];
			datos_montaje_nuevo["letter"]       = AddEditMontajes->DatosMontaje["letter"];
			datos_montaje_nuevo["indx_cd"]      = AddEditMontajes->DatosMontaje["indx_cd"];
			datos_montaje_nuevo["opt_mount"]    = AddEditMontajes->DatosMontaje["opt_mount"];
			datos_montaje_nuevo["io_ctrl"]      = AddEditMontajes->DatosMontaje["io_ctrl"];
			datos_montaje_nuevo["select_mount"] = "x";

			item->setText( 8, sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
		} else
			item->setText( 8, "" );//

		item->setText( 9, fGrl.IntToStr( item->childCount() ) );

		PrevierMontajes();
	}
}

void frmAddEditDosBox::on_btnMount_Edit()
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

			if( ui.twMontajes->currentItem()->text(7)=="v")
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
		//	ui.twMontajes->currentItem()->setText( 7, "x");	// primer montaje
		//	ui.twMontajes->currentItem()->setText( 9, fGrl.IntToStr( ui.twMontajes->currentItem()->childCount() ) ); //

			PrevierMontajes();
		}
	}
}

void frmAddEditDosBox::on_btnMount_Delete()
{
	if( ui.twMontajes->topLevelItemCount() > 0 )
	{
		int respuesta = QMessageBox::question( this, tr("¿Eliminar Juego...?"), tr("¿Deseas eliminar este montaje?"), tr("Si"), tr("Cancelar"), 0, 1 );
		if( respuesta == 0 )
		{
			if( EditandoJuego == true )
				sql->ItemEliminarMontaje( ui.twMontajes->currentItem()->text(8) );

			fGrl.DeleteItemTree( ui.twMontajes->currentItem() );

			PrevierMontajes();
		}
	}
}

void frmAddEditDosBox::on_btnMount_Clear()
{
	if( ui.twMontajes->topLevelItemCount() > 0 )
	{
		if( EditandoJuego == true )
		{
			for(int num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = ui.twMontajes->topLevelItem( num_mount );
				sql->ItemEliminarMontaje( item->text(8) );
			}
		}
		ui.twMontajes->clear();
		ui.previer_mount->clear();
	}
}

void frmAddEditDosBox::on_btnMount_Subir()
{
	int index_1 = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	int index_2;
	if( index_1 > 0 && index_1 != -1 )
	{
		index_2 = index_1 - 1;

		QTreeWidgetItem *item = ui.twMontajes->topLevelItem( index_1 );

		item = ui.twMontajes->takeTopLevelItem( index_1 );
		ui.twMontajes->insertTopLevelItem(index_2, item);

		ui.twMontajes->setCurrentItem( item );
	}
}

void frmAddEditDosBox::on_btnMount_Bajar()
{
	int index_1 = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	int index_2;
	if( index_1 < ( ui.twMontajes->topLevelItemCount() - 1) && index_1 != -1 )
	{
		index_2 = index_1 + 1;

		QTreeWidgetItem *item = ui.twMontajes->topLevelItem( index_1 );

		item = ui.twMontajes->takeTopLevelItem( index_1 );
		ui.twMontajes->insertTopLevelItem(index_2, item);

		ui.twMontajes->setCurrentItem( item );
	}
}

void frmAddEditDosBox::on_btnMount_AutoCrear()
{
	QFileInfo fi( ui.txtDbx_path_exe->text() );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
	item->setIcon( 0, QIcon(stTheme+"img16/drive_hd.png")	);
	item->setText( 0, fi.absolutePath() );	// directorio o iso
	item->setText( 1, ""                );	// etiqueta
	item->setText( 2, "drive"           );	// tipo de montaje
	item->setText( 3, "C"               );	// letra de montaje
	item->setText( 4, ""                );	// index de la unidad de cd-rom
	item->setText( 5, ""                );	// opciones del cd-rom
	item->setText( 6, ""                );	// cd/dvd IOCtrl
	item->setText( 7, "x"               );	// primer montaje

	if( EditandoJuego == true )
	{
		QHash<QString, QString> datos_montaje_nuevo;
		datos_montaje_nuevo.clear();
		datos_montaje_nuevo["id_lista"]     = fGrl.IntToStr( item->childCount() );
		datos_montaje_nuevo["path"]         = fi.absolutePath();
		datos_montaje_nuevo["label"]        = "";
		datos_montaje_nuevo["tipo_as"]      = "drive";
		datos_montaje_nuevo["letter"]       = "C";
		datos_montaje_nuevo["indx_cd"]      = "";
		datos_montaje_nuevo["opt_mount"]    = "";
		datos_montaje_nuevo["io_ctrl"]      = "";
		datos_montaje_nuevo["select_mount"] = "x";

		item->setText( 8, sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
	} else
		item->setText( 8, "" );

	item->setText( 9, fGrl.IntToStr( item->childCount() ) );

	PrevierMontajes();
}

void frmAddEditDosBox::on_btnMount_Primario()
{
	int indx_mount=0, num_mount=0;
	QString tipoDrive, isPrimario;

	int pos = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
	if( ui.twMontajes->topLevelItemCount() > 0 && pos != -1 )
	{
		indx_mount = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );
		isPrimario = ui.twMontajes->topLevelItem( indx_mount )->text(7);

		for( num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
		{
			tipoDrive = ui.twMontajes->topLevelItem( num_mount )->text(2);

			ui.twMontajes->topLevelItem( num_mount )->setIcon(0, QIcon( fGrl.getIconMount(tipoDrive) ) );
			ui.twMontajes->topLevelItem( num_mount )->setText(7, "x");
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

// Menu Herramintas de Montaje en Autoexec
void frmAddEditDosBox::addMontajeAutoexec(QString tipo, QString dir_file)
{
// Montando Disquetera, Disco Duro, CDs, Imagenes de Discos, disquetes y CDs
	if( dir_file != "" )
	{
		if( tipo == "floppy")
			ui.txtDbx_Autoexec->insertPlainText("mount A \""+dir_file+"\" -t floppy");
		if( tipo == "driver" )
			ui.txtDbx_Autoexec->insertPlainText("mount C \""+dir_file+"\"");
		if( tipo == "cdrom" )
			ui.txtDbx_Autoexec->insertPlainText("mount D \""+dir_file+"\" -t cdrom");
		if( tipo == "IMG_floppy")
			ui.txtDbx_Autoexec->insertPlainText("mount A \""+dir_file+"\" -t floppy");
		if( tipo == "IMG_iso")
			ui.txtDbx_Autoexec->insertPlainText("mount D \""+dir_file+"\" -t iso");
		if( tipo == "IMG_hdd")
			ui.txtDbx_Autoexec->insertPlainText("mount C \""+dir_file+"\" -t hdd");
	}
}

void frmAddEditDosBox::on_btnToolDbx_mount_HD()
{
	QString m_dir = fGrl.VentanaDirectorios(tr("Selecciona el directorio"), "", "");
	addMontajeAutoexec("driver", m_dir);
}

void frmAddEditDosBox::on_btnToolDbx_mount_CD()
{
	QString m_dir = fGrl.VentanaDirectorios(tr("Selecciona el directorio"), "", "");
	addMontajeAutoexec("cdrom", m_dir);
}

void frmAddEditDosBox::on_btnToolDbx_mount_Floppy()
{
	QString m_dir = fGrl.VentanaDirectorios(tr("Selecciona el directorio"), "", "");
	addMontajeAutoexec("floppy", m_dir);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgFloppy()
{
		QString m_file = fGrl.VentanaAbrirArchivos(tr("Selecciona un archivo"), "", "", tr("Imagen") +" (*.ima *.img);;"+ tr("Todos los archivo") + " (*)", 0, false);
	addMontajeAutoexec("IMG_floppy", m_file);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgCD()
{
		QString m_file = fGrl.VentanaAbrirArchivos(tr("Selecciona un archivo"), "", "", tr("Imagen CD") +" (*.iso *.cue);;"+ tr("Todos los archivo") + " (*)", 0, false);
	addMontajeAutoexec("IMG_iso", m_file);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgHD()
{
		QString m_file = fGrl.VentanaAbrirArchivos(tr("Selecciona un archivo"), "", "", tr("Imagen HD") +" (*.ima *.img);;"+tr("Todos los archivo") + " (*)", 0, false);
	addMontajeAutoexec("IMG_hdd", m_file);
}
