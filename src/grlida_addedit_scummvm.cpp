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

#include "grlida_addedit_scummvm.h"

frmAddEditScummVM::frmAddEditScummVM(QWidget *parent)
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

frmAddEditScummVM::~frmAddEditScummVM()
{
	//
}

bool frmAddEditScummVM::isCorrectNext()
{
	bool siguiente = false;
	if( ui.txtSvm_game_label->text() == "" )
	{
		siguiente=false;
		QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner la etiqueta del juego de")+ " ScummVM" );
	} else {
		siguiente=true;
		if( ui.txtSvm_path->text() == "" )
		{
			siguiente=false;
			QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Directorio del juego de")+ " ScummVM" );
		} else {
			siguiente=true;
			QDir tmpdirsvm( ui.txtSvm_path->text() );
			if(!tmpdirsvm.exists())
			{
				siguiente=false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar un Directorio que exista") );
			} else
				siguiente=true;
		}
	}
	return siguiente;
}

void frmAddEditScummVM::setEditandoJuego(bool editando)
{
	EditandoJuego = editando;
}

void frmAddEditScummVM::createConnections()
{
	connect( ui.twScummVM    , SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_twScummVM_clicked( QTreeWidgetItem* )));
	connect( ui.btnDefectoSvm, SIGNAL( clicked() ), this, SLOT( on_btnDefecto() ) );
	connect( ui.btnDirSvm_1  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_path() ) );
	connect( ui.btnDirSvm_2  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_save() ) );
	connect( ui.btnDirSvm_3  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_extra() ) );
	connect( ui.btnDirSvm_4  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_capturas() ) );
	connect( ui.btnDirSvm_5  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_sonido() ) );
	connect( ui.btnDirSvm_6  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_sound_font() ) );
	connect( ui.btnDirSvm_7  , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_setup() ) );

	connect( ui.h_SliderSvm_music_volume , SIGNAL(valueChanged(int)), ui.posSliderSvm_1, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_sfx_volume   , SIGNAL(valueChanged(int)), ui.posSliderSvm_2, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_speech_volume, SIGNAL(valueChanged(int)), ui.posSliderSvm_3, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_tempo        , SIGNAL(valueChanged(int)), ui.posSliderSvm_4, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_talkspeed    , SIGNAL(valueChanged(int)), ui.posSliderSvm_5, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_debuglevel   , SIGNAL(valueChanged(int)), ui.posSliderSvm_6, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_midi_gain    , SIGNAL(valueChanged(int)), ui.posSliderSvm_7, SLOT(setNum(int)));
	connect( ui.h_SliderSvm_walkspeed    , SIGNAL(valueChanged(int)), ui.posSliderSvm_8, SLOT(setNum(int)));
}

void frmAddEditScummVM::setTheme()
{
	ui.btnDirSvm_1->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_2->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_3->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_4->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_5->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_6->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirSvm_7->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDirSvm_1_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_2_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_3_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_4_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_5_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_6_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_7_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDefectoSvm->setIcon( QIcon(stTheme+"img16/actualizar.png") );
	setTabIcon(0, QIcon(stTheme+"img16/scummvm.png") );
	setTabIcon(1, QIcon(stTheme+"img16/opciones.png") );
	setTabIcon(2, QIcon(stTheme+"img16/vdmsound.png") );
	ui.cbxSvm_joystick_num->setItemIcon(0, QIcon(stTheme+"img16/controller.png") );
	ui.cbxSvm_joystick_num->setItemIcon(1, QIcon(stTheme+"img16/controller.png") );

	if( GRLConfig["font_usar"].toBool() )
	{
		QFont m_font;
		m_font.setFamily( GRLConfig["font_family"].toString() );
		m_font.setPointSize( GRLConfig["font_size"].toInt() );
		setFont(m_font);
	}
}

void frmAddEditScummVM::CargarConfigScummVM()
{
	ui.twScummVM->header()->setStretchLastSection(false);
	ui.twScummVM->header()->setMovable(false);
	ui.twScummVM->header()->setResizeMode(0, QHeaderView::Fixed  );
	ui.twScummVM->header()->setResizeMode(1, QHeaderView::Stretch);
	ui.twScummVM->header()->setResizeMode(2, QHeaderView::Fixed  );
	ui.twScummVM->setColumnWidth(0, 35);
	ui.twScummVM->setColumnWidth(2, 80);

	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[A-Za-z-0-9]+");

	QValidator *validarTexto = new QRegExpValidator( regexp, this );
	ui.txtSvm_game_label->setValidator( validarTexto );

	fGrl.CargarDatosListaSvm(":/datos/svm_lista.txt"        , ui.twScummVM             );	// Carga la lista de compatibilidad del ScummVM.
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"       , ui.cbxSvm_language,2,true);	// Carga la lista de idiomas
	fGrl.CargarDatosComboBox(":/datos/svm_platform.txt"     , ui.cbxSvm_platform    , 2);	// Carga la lista de platform
	fGrl.CargarDatosComboBox(":/datos/svm_gfxmode.txt"      , ui.cbxSvm_gfx_mode    , 3);	// Carga la lista de gfxmode
	fGrl.CargarDatosComboBox(":/datos/svm_render_mode.txt"  , ui.cbxSvm_render_mode    );	// Carga la lista de render_mode
	fGrl.CargarDatosComboBox(":/datos/svm_music_driver.txt" , ui.cbxSvm_music_driver, 3);	// Carga la lista de music_driver

	ui.cbxSvm_opl_driver->clear();
	ui.cbxSvm_opl_driver->addItem(QIcon(stTheme+"img16/sinimg.png"), "<defecto>", "auto");
	ui.cbxSvm_opl_driver->addItem(QIcon(stTheme+"img16/img_advmame2x_3x.png"), "MAME OPL emulator", "mame");
	ui.cbxSvm_opl_driver->addItem(QIcon(stTheme+"img16/dosbox.png"), "DOSBox OPL emulator (experimental)", "db");

	ui.cbxSvm_output_rate->clear();
	ui.cbxSvm_output_rate->addItem("<defecto>");
	ui.cbxSvm_output_rate->addItem("11050");
	ui.cbxSvm_output_rate->addItem("22050");
	ui.cbxSvm_output_rate->addItem("44100");
	ui.cbxSvm_output_rate->addItem("48000");

	ui.cbxSvm_cdrom->clear();
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"),"CD Index 0");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"),"CD Index 1");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"),"CD Index 2");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"),"CD Index 3");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"),"CD Index 4");
}

void frmAddEditScummVM::setConfigDefecto()
{
	ui.cbxSvm_language->setCurrentIndex(0);
	ui.cbxSvm_platform->setCurrentIndex(0);
	ui.cbxSvm_gfx_mode->setCurrentIndex(1);
	ui.cbxSvm_render_mode->setCurrentIndex(0);
	ui.cbxSvm_music_driver->setCurrentIndex(0);
	ui.cbxSvm_output_rate->setCurrentIndex(0);
	ui.cbxSvm_cdrom->setCurrentIndex(0);
	ui.cbxSvm_opl_driver->setCurrentIndex(0);
}

void frmAddEditScummVM::CargarDatosScummVM(QString IDGrl)
{
	QHash<QString, QString> tempDatosScummVM;

	tempDatosScummVM.clear();
	tempDatosScummVM = sql->showConfg_ScummVM( IDGrl );
	stItemIDSvm      = sql->ItemIDIndex("dbgrl_emu_scummvm", IDGrl);	// Obtenemos el Index del ScummVM

	CargarDatosScummVM( tempDatosScummVM );
}

void frmAddEditScummVM::CargarDatosScummVM(QHash<QString, QString> datosSvm)
{
	ui.txtSvm_gameid->setText( datosSvm["Svm_game"] );				// game
	ui.txtSvm_game_label->setText( datosSvm["Svm_game_label"] );	// game_label

	QTreeWidgetItem *item;
	QList<QTreeWidgetItem *> found = ui.twScummVM->findItems( datosSvm["Svm_game"], Qt::MatchExactly, 2);
	foreach (item, found)
		ui.twScummVM->setItemSelected(item, true);

	ui.cbxSvm_language->setCurrentIndex( ui.cbxSvm_language->findData( datosSvm["Svm_language"] ) );				// language
	ui.chkSvm_subtitles->setChecked( fGrl.StrToBool( datosSvm["Svm_subtitles"] ) );									// subtitles
	ui.cbxSvm_platform->setCurrentIndex( ui.cbxSvm_platform->findData( datosSvm["Svm_platform"] ) );				// platform
	ui.cbxSvm_gfx_mode->setCurrentIndex( ui.cbxSvm_gfx_mode->findData( datosSvm["Svm_gfx_mode"] ) );				// gfx_mode
	ui.cbxSvm_render_mode->setCurrentIndex( ui.cbxSvm_render_mode->findText( datosSvm["Svm_render_mode"] ) );		// render_mode
	ui.chkSvm_fullscreen->setChecked( fGrl.StrToBool( datosSvm["Svm_fullscreen"] ) );								// fullscreen
	ui.chkSvm_aspect_ratio->setChecked( fGrl.StrToBool( datosSvm["Svm_aspect_ratio"] ) );							// aspect_ratio
	ui.txtSvm_path->setText( datosSvm["Svm_path"] );																// path
	ui.txtSvm_path_setup->setText( datosSvm["Svm_path_setup"] );													// path_setup
	ui.txtSvm_extrapath->setText( datosSvm["Svm_path_extra"] ); 													// path_extra
	ui.txtSvm_savepath->setText( datosSvm["Svm_path_save"] );														// path_save
	ui.txtSvm_path_capturas->setText( datosSvm["Svm_path_capturas"] );												// path_capturas
	ui.txtSvm_path_sonido->setText( datosSvm["Svm_path_sonido"] );													// path_sonido
	ui.cbxSvm_music_driver->setCurrentIndex( ui.cbxSvm_music_driver->findData( datosSvm["Svm_music_driver"] ) );	// music_driver
	ui.chkSvm_enable_gs->setChecked( fGrl.StrToBool( datosSvm["Svm_enable_gs"] ) );									// enable_gs
	ui.chkSvm_multi_midi->setChecked( fGrl.StrToBool( datosSvm["Svm_multi_midi"] ) );								// multi_midi
	ui.chkSvm_native_mt32->setChecked( fGrl.StrToBool( datosSvm["Svm_native_mt32"] ) );								// native_mt32
	//datosSvm["Svm_master_volume"];																				// master_volume
	ui.h_SliderSvm_music_volume->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_music_volume"] ) );				// music_volume
	ui.h_SliderSvm_sfx_volume->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_sfx_volume"] ) );					// sfx_volume
	ui.h_SliderSvm_speech_volume->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_speech_volume"] ) );				// speech_volume
	ui.h_SliderSvm_tempo->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_tempo"] ) );								// tempo
	ui.h_SliderSvm_talkspeed->setSliderPosition(fGrl.StrToInt(  datosSvm["Svm_talkspeed"] ) );						// talkspeed
	ui.h_SliderSvm_debuglevel->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_debuglevel"] ) );					// debuglevel
	ui.h_SliderSvm_walkspeed->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_walkspeed"] ) );						// walkspeed
	ui.cbxSvm_cdrom->setCurrentIndex( fGrl.StrToInt( datosSvm["Svm_cdrom"] ) );										// cdrom
	ui.cbxSvm_joystick_num->setCurrentIndex( fGrl.StrToInt( datosSvm["Svm_joystick_num"] ) );						// joystick_num

	if( datosSvm["Svm_output_rate"] != "" )
		ui.cbxSvm_output_rate->setCurrentIndex( ui.cbxSvm_output_rate->findText( datosSvm["Svm_output_rate"], Qt::MatchContains ) ); // output_rate
	else
		ui.cbxSvm_output_rate->setCurrentIndex( 0 );

	ui.h_SliderSvm_midi_gain->setSliderPosition( fGrl.StrToInt( datosSvm["Svm_midi_gain"] ) );	// midi_gain
	ui.chkSvm_copy_protection->setChecked( fGrl.StrToBool( datosSvm["Svm_copy_protection"] ) );	// copy_protection
	ui.txtSvm_soundfont->setText( datosSvm["Svm_sound_font"] );									// sound_font
	ui.cbxSvm_opl_driver->setCurrentIndex( ui.cbxSvm_opl_driver->findData( datosSvm["Svm_opl_driver"] ) ); // opl_driver
}

QHash<QString, QString> frmAddEditScummVM::setDatosScummVM()
{
	QHash<QString, QString> tempDatosScummVM;

	tempDatosScummVM.clear();

	tempDatosScummVM["Svm_id"]         = stItemIDSvm;					// id del ScummVM
	tempDatosScummVM["Svm_game"]       = ui.txtSvm_gameid->text();		// game
	tempDatosScummVM["Svm_game_label"] = ui.txtSvm_game_label->text();	// game_label

	if( ui.cbxSvm_language->itemData( ui.cbxSvm_language->currentIndex() ).toString() != "" )
		tempDatosScummVM["Svm_language"] = ui.cbxSvm_language->itemData( ui.cbxSvm_language->currentIndex() ).toString();	// language
	else
		tempDatosScummVM["Svm_language"] = "es";

	tempDatosScummVM["Svm_subtitles"]  = fGrl.BoolToStr( ui.chkSvm_subtitles->isChecked() );								// subtitles

	if( ui.cbxSvm_platform->itemData( ui.cbxSvm_platform->currentIndex() ).toString() != "" )
		tempDatosScummVM["Svm_platform"] = ui.cbxSvm_platform->itemData( ui.cbxSvm_platform->currentIndex() ).toString();		// platform
	else
		tempDatosScummVM["Svm_platform"] = "pc";

	tempDatosScummVM["Svm_gfx_mode"]   = ui.cbxSvm_gfx_mode->itemData( ui.cbxSvm_gfx_mode->currentIndex() ).toString();		// gfx_mode

	if( ui.cbxSvm_render_mode->currentText() != "" )
		tempDatosScummVM["Svm_render_mode"] = ui.cbxSvm_render_mode->currentText();	// render_mode
	else
		tempDatosScummVM["Svm_render_mode"] = "";

	tempDatosScummVM["Svm_fullscreen"]    = fGrl.BoolToStr( ui.chkSvm_fullscreen->isChecked() );	// fullscreen
	tempDatosScummVM["Svm_aspect_ratio"]  = fGrl.BoolToStr( ui.chkSvm_aspect_ratio->isChecked() );	// aspect_ratio
	tempDatosScummVM["Svm_path"]          = ui.txtSvm_path->text();									// path
	tempDatosScummVM["Svm_path_setup"]    = ui.txtSvm_path_setup->text();							// path_setup
	tempDatosScummVM["Svm_path_extra"]    = ui.txtSvm_extrapath->text();							// path_extra
	tempDatosScummVM["Svm_path_save"]     = ui.txtSvm_savepath->text();								// path_save
	tempDatosScummVM["Svm_path_capturas"] = ui.txtSvm_path_capturas->text();						// path_capturas
	tempDatosScummVM["Svm_path_sonido"]   = ui.txtSvm_path_sonido->text();							// path_sonido
	tempDatosScummVM["Svm_music_driver"]  = ui.cbxSvm_music_driver->itemData( ui.cbxSvm_music_driver->currentIndex() ).toString();	// music_driver
	tempDatosScummVM["Svm_enable_gs"]     = fGrl.BoolToStr( ui.chkSvm_enable_gs->isChecked() );		// enable_gs
	tempDatosScummVM["Svm_multi_midi"]    = fGrl.BoolToStr( ui.chkSvm_multi_midi->isChecked() );	// multi_midi
	tempDatosScummVM["Svm_native_mt32"]   = fGrl.BoolToStr( ui.chkSvm_native_mt32->isChecked() );	// native_mt32
	tempDatosScummVM["Svm_master_volume"] = "255";													// master_volume
	tempDatosScummVM["Svm_music_volume"]  = ui.posSliderSvm_1->text();								// music_volume
	tempDatosScummVM["Svm_sfx_volume"]    = ui.posSliderSvm_2->text();								// sfx_volume
	tempDatosScummVM["Svm_speech_volume"] = ui.posSliderSvm_3->text();								// speech_volume
	tempDatosScummVM["Svm_tempo"]         = ui.posSliderSvm_4->text();								// tempo
	tempDatosScummVM["Svm_talkspeed"]     = ui.posSliderSvm_5->text();								// talkspeed
	tempDatosScummVM["Svm_debuglevel"]    = ui.posSliderSvm_6->text();								// debuglevel
	tempDatosScummVM["Svm_cdrom"]         = fGrl.IntToStr(ui.cbxSvm_cdrom->currentIndex());			// cdrom
	tempDatosScummVM["Svm_joystick_num"]  = fGrl.IntToStr(ui.cbxSvm_joystick_num->currentIndex());	// joystick_num

	if( ui.cbxSvm_output_rate->currentText() != "" && ui.cbxSvm_output_rate->currentIndex() != 0 )
		tempDatosScummVM["Svm_output_rate"] = ui.cbxSvm_output_rate->currentText();
	else
		tempDatosScummVM["Svm_output_rate"] = "";

	tempDatosScummVM["Svm_copy_protection"] = fGrl.BoolToStr( ui.chkSvm_copy_protection->isChecked() );	// copy_protection
	tempDatosScummVM["Svm_midi_gain"]       = ui.posSliderSvm_7->text();			// midi_gain
	tempDatosScummVM["Svm_sound_font"]      = ui.txtSvm_soundfont->text();			// sound_font
	tempDatosScummVM["Svm_walkspeed"]       = ui.posSliderSvm_8->text();			// walkspeed
	tempDatosScummVM["Svm_opl_driver"]      = ui.cbxSvm_opl_driver->itemData( ui.cbxSvm_opl_driver->currentIndex() ).toString();	// opl_driver

	return tempDatosScummVM;
}

void frmAddEditScummVM::on_btnDirSvm_path()
{
	ui.txtSvm_path->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Svm_path"].toString(), ui.txtSvm_path->text() ) );

	QDir dir( ui.txtSvm_path->text() );
	if( dir.exists() )
		GRLConfig["Svm_path"] = ui.txtSvm_path->text();
	else
		GRLConfig["Svm_path"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path", GRLConfig["Svm_path"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_save()
{
	ui.txtSvm_savepath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Svm_savepath"].toString(), ui.txtSvm_savepath->text() ) );

	QDir dir( ui.txtSvm_savepath->text() );
	if( dir.exists() )
		GRLConfig["Svm_savepath"] = ui.txtSvm_savepath->text();
	else
		GRLConfig["Svm_savepath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_savepath", GRLConfig["Svm_savepath"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_extra()
{
	ui.txtSvm_extrapath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Svm_extrapath"].toString(), ui.txtSvm_extrapath->text() ) );

	QDir dir( ui.txtSvm_extrapath->text() );
	if( dir.exists() )
		GRLConfig["Svm_extrapath"] = ui.txtSvm_extrapath->text();
	else
		GRLConfig["Svm_extrapath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_extrapath", GRLConfig["Svm_extrapath"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_capturas()
{
	ui.txtSvm_path_capturas->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Svm_path_capturas"].toString(), ui.txtSvm_path_capturas->text() ) );

	QDir dir( ui.txtSvm_path_capturas->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_capturas"] = ui.txtSvm_path_capturas->text();
	else
		GRLConfig["Svm_path_capturas"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_capturas", GRLConfig["Svm_path_capturas"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_sonido()
{
	ui.txtSvm_path_sonido->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), GRLConfig["Svm_path_sonido"].toString(), ui.txtSvm_path_sonido->text() ) );

	QDir dir( ui.txtSvm_path_sonido->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_sonido"] = ui.txtSvm_path_sonido->text();
	else
		GRLConfig["Svm_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_sonido", GRLConfig["Svm_path_sonido"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_setup()
{
	ui.txtSvm_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Svm_path_setup"].toString(), ui.txtSvm_path_setup->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtSvm_path_setup->text() );
	if( fi.exists() )
		GRLConfig["Svm_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Svm_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_setup", GRLConfig["Svm_path_setup"].toString() );
}

void frmAddEditScummVM::on_btnDirSvm_sound_font()
{
	ui.txtSvm_soundfont->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Svm_soundfont"].toString(), ui.txtSvm_soundfont->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtSvm_soundfont->text() );
	if( fi.exists() )
		GRLConfig["Svm_soundfont"] = fi.absolutePath();
	else
		GRLConfig["Svm_soundfont"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_soundfont", GRLConfig["Svm_soundfont"].toString() );

}

void frmAddEditScummVM::on_twScummVM_clicked( QTreeWidgetItem *item )
{
	ui.txtSvm_gameid->setText( item->text(2) );
	ui.txtSvm_game_label->setText( item->text(2) );
}

void frmAddEditScummVM::on_btnDefecto()
{
	ui.h_SliderSvm_music_volume->setValue(192);
	ui.h_SliderSvm_sfx_volume->setValue(192);
	ui.h_SliderSvm_speech_volume->setValue(192);
	ui.h_SliderSvm_tempo->setValue(100);
	ui.h_SliderSvm_talkspeed->setValue(60);
	ui.h_SliderSvm_debuglevel->setValue(0);
	ui.h_SliderSvm_midi_gain->setValue(100);
	ui.h_SliderSvm_walkspeed->setValue(0);
}
