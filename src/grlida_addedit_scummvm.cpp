/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_addedit_scummvm.h"
#include "ui_addedit_scummvm.h"

frmAddEditScummVM::frmAddEditScummVM(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::frmAddEditScummVM)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	IdGame    = id_game;
	Editando  = m_editando;

	grlDir.Home  = fGrl->GRlidaHomePath();
	grlDir.Datos = grlDir.Home +"datos/";
}

frmAddEditScummVM::~frmAddEditScummVM()
{
	delete fGrl;
	delete ui;
}

void frmAddEditScummVM::cargarConfig()
{
	ui->twScummVM->header()->setStretchLastSection(false);
	ui->twScummVM->header()->setMovable(false);
	ui->twScummVM->header()->setResizeMode(0, QHeaderView::Fixed  );
	ui->twScummVM->header()->setResizeMode(1, QHeaderView::Stretch);
	ui->twScummVM->header()->setResizeMode(2, QHeaderView::Fixed  );
	ui->twScummVM->setColumnWidth(0, 30);
	ui->twScummVM->setColumnWidth(2, 80);

	fGrl->cargarDatosTwLista(ui->twScummVM, grlDir.Datos +"svm_lista.txt", TwListSvm, "|");

	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[A-Za-z-0-9]+");

	QValidator *validarTexto = new QRegExpValidator( regexp, this );
	ui->txtSvm_game_label->setValidator( validarTexto );

	fGrl->cargarDatosComboBox(ui->cbxSvm_language    , grlDir.Datos + fGrl->Idioma() +"/svm_idioma.txt", fGrl->Theme() +"img_lng/", "es"   , 3, "|", true, true);
	fGrl->cargarDatosComboBox(ui->cbxSvm_platform    , grlDir.Datos +"svm_platform.txt"                , fGrl->Theme() +"img16/"  , "pc"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_gfx_mode    , grlDir.Datos +"svm_gfxmode.txt"                 , fGrl->Theme() +"img16/"  , "2x"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_render_mode , grlDir.Datos +"svm_render_mode.txt"             , fGrl->Theme() +"img16/"  , ""     , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_music_driver, grlDir.Datos +"svm_music_driver.txt"            , fGrl->Theme() +"img16/"  , "adlib", 3, "|");

	ui->cbxSvm_opl_driver->clear();
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->Theme() +"img16/sinimg.png"), "<defecto>", "auto");
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->Theme() +"img16/img_advmame2x_3x.png"), "MAME OPL emulator", "mame");
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png"), "DOSBox OPL emulator (experimental)", "db");

	ui->cbxSvm_output_rate->clear();
	ui->cbxSvm_output_rate->addItem("<defecto>", "" );
	ui->cbxSvm_output_rate->addItem("8000" , "8000" );
	ui->cbxSvm_output_rate->addItem("11025", "11025");
	ui->cbxSvm_output_rate->addItem("22050", "22050");
	ui->cbxSvm_output_rate->addItem("44100", "44100");
	ui->cbxSvm_output_rate->addItem("48000", "48000");

	ui->cbxSvm_cdrom->clear();
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->Theme() +"img16/drive_cdrom.png"), "CD Index 0", "0");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->Theme() +"img16/drive_cdrom.png"), "CD Index 1", "1");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->Theme() +"img16/drive_cdrom.png"), "CD Index 2", "2");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->Theme() +"img16/drive_cdrom.png"), "CD Index 3", "3");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->Theme() +"img16/drive_cdrom.png"), "CD Index 4", "4");

	ui->cbxSvm_joystick_num->clear();
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->Theme() +"img16/controller.png"), tr("Sin Joystick"), "");
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->Theme() +"img16/controller.png"), "Joystick 1" , "0");
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->Theme() +"img16/controller.png"), "Joystick 2" , "1");
}

void frmAddEditScummVM::setTheme()
{
	setTabIcon(0, QIcon(fGrl->Theme() +"img16/scummvm.png") );
	setTabIcon(1, QIcon(fGrl->Theme() +"img16/opciones.png") );
	setTabIcon(2, QIcon(fGrl->Theme() +"img16/vdmsound.png") );

	ui->btnSvm_Path->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_Path_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSave->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathSave_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathExtra->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathExtra_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathCapturas->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathCapturas_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSonido->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathSonido_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSetup->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnSvm_PathSetup_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_SoundFont->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnSvm_SoundFont_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->btnDefectoSvm->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
}

bool frmAddEditScummVM::isCorrectNext()
{
	bool siguiente = false;

	if( ui->txtSvm_gameid->text().isEmpty() )
	{
		siguiente = false;
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner el ID del juego de ScummVM") );
		ui->txtSvm_gameid->setFocus();
	} else {
		siguiente = true;
		if( ui->txtSvm_game_label->text().isEmpty() )
		{
			siguiente = false;
			QMessageBox::information(this, titulo_ventana(), tr("Debes poner la etiqueta del juego de ScummVM") );
			ui->txtSvm_game_label->setFocus();
		} else {
			siguiente = true;
			if( ui->txtSvm_path->text().isEmpty() )
			{
				siguiente = false;
				QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el directorio del juego de ScummVM") );
				ui->txtSvm_path->setFocus();
			} else {
				siguiente = true;
				QDir tmpdirsvm( ui->txtSvm_path->text() );
				if( !tmpdirsvm.exists() )
				{
					siguiente = false;
					QMessageBox::information(this, titulo_ventana(), tr("Debes indicar un directorio que exista") );
					ui->txtSvm_path->setFocus();
				} else
					siguiente = true;
			}
		}
	}

	return siguiente;
}

void frmAddEditScummVM::cargarDatosScummVM(stConfigScummVM cfgSvm)
{
	QTreeWidgetItem *item;
	if( Editando )
	{
		QList<QTreeWidgetItem *> found = ui->twScummVM->findItems(cfgSvm.game, Qt::MatchExactly, 2);
		foreach (item, found)
			ui->twScummVM->setItemSelected(item, true);
	}

	ui->txtSvm_gameid->setText( cfgSvm.game );
	ui->txtSvm_game_label->setText( cfgSvm.game_label );
	ui->cbxSvm_language->setCurrentIndex( ui->cbxSvm_language->findData( cfgSvm.language ) );
	ui->chkSvm_subtitles->setChecked( fGrl->StrToBool( cfgSvm.subtitles ) );
	ui->cbxSvm_platform->setCurrentIndex( ui->cbxSvm_platform->findData( cfgSvm.platform ) );
	ui->cbxSvm_gfx_mode->setCurrentIndex( ui->cbxSvm_gfx_mode->findData( cfgSvm.gfx_mode ) );
	ui->cbxSvm_render_mode->setCurrentIndex( ui->cbxSvm_render_mode->findData( cfgSvm.render_mode ) );
	ui->chkSvm_fullscreen->setChecked( fGrl->StrToBool( cfgSvm.fullscreen ) );
	ui->chkSvm_aspect_ratio->setChecked( fGrl->StrToBool( cfgSvm.aspect_ratio ) );
	ui->txtSvm_path->setText( cfgSvm.path );
	ui->txtSvm_path_setup->setText( cfgSvm.path_setup );
	ui->txtSvm_extrapath->setText( cfgSvm.path_extra );
	ui->txtSvm_savepath->setText( cfgSvm.path_save );
	ui->txtSvm_path_capturas->setText( cfgSvm.path_capturas );
	ui->txtSvm_path_sonido->setText( cfgSvm.path_sonido );
	ui->cbxSvm_music_driver->setCurrentIndex( ui->cbxSvm_music_driver->findData( cfgSvm.music_driver ) );
	ui->chkSvm_enable_gs->setChecked( fGrl->StrToBool( cfgSvm.enable_gs ) );
	ui->chkSvm_multi_midi->setChecked( fGrl->StrToBool( cfgSvm.multi_midi ) );
	ui->chkSvm_native_mt32->setChecked( fGrl->StrToBool( cfgSvm.native_mt32 ) );
	ui->chkSvm_mute->setChecked( fGrl->StrToBool( cfgSvm.mute ) );
// master_volume
	ui->h_SliderSvm_music_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.music_volume ) );
	ui->h_SliderSvm_sfx_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.sfx_volume ) );
	ui->h_SliderSvm_speech_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.speech_volume ) );
	ui->chkSvm_speech_mute->setChecked( fGrl->StrToBool( cfgSvm.speech_mute ) );
	ui->h_SliderSvm_tempo->setSliderPosition( fGrl->StrToInt( cfgSvm.tempo ) );
	ui->h_SliderSvm_talkspeed->setSliderPosition(fGrl->StrToInt(  cfgSvm.talkspeed ) );
	ui->h_SliderSvm_debuglevel->setSliderPosition( fGrl->StrToInt( cfgSvm.debuglevel ) );
	ui->cbxSvm_cdrom->setCurrentIndex( fGrl->StrToInt( cfgSvm.cdrom ) );
	ui->cbxSvm_joystick_num->setCurrentIndex( fGrl->StrToInt( cfgSvm.joystick_num ) );
	ui->cbxSvm_output_rate->setCurrentIndex( ui->cbxSvm_output_rate->findData( cfgSvm.output_rate ) );
	ui->h_SliderSvm_midi_gain->setSliderPosition( fGrl->StrToInt( cfgSvm.midi_gain ) );
	ui->chkSvm_copy_protection->setChecked( fGrl->StrToBool( cfgSvm.copy_protection ) );
	ui->txtSvm_soundfont->setText( cfgSvm.sound_font );
	ui->h_SliderSvm_walkspeed->setSliderPosition( fGrl->StrToInt( cfgSvm.walkspeed ) );
	ui->cbxSvm_opl_driver->setCurrentIndex( ui->cbxSvm_opl_driver->findData( cfgSvm.opl_driver ) );
	ui->chkSvm_disable_dithering->setChecked( fGrl->StrToBool( cfgSvm.disable_dithering ) );
	ui->chkSvm_alt_intro->setChecked( fGrl->StrToBool( cfgSvm.alt_intro ) );

	if( Editando && !IdGame.isEmpty() )
		IdSvm = cfgSvm.id;
	else
		IdSvm = "";
}

void frmAddEditScummVM::setDatosScummVM()
{
	QString str;

	DatosScummVM.id    = IdSvm;
	DatosScummVM.idgrl = IdGame;
	DatosScummVM.idcat = categoria.id;

	DatosScummVM.game       = ui->txtSvm_gameid->text();
	DatosScummVM.game_label = ui->txtSvm_game_label->text();
	str = ui->cbxSvm_language->itemData( ui->cbxSvm_language->currentIndex() ).toString();
	DatosScummVM.language          = str.isEmpty() ? "es" : str;
	DatosScummVM.subtitles         = fGrl->BoolToStr( ui->chkSvm_subtitles->isChecked() );
	str = ui->cbxSvm_platform->itemData( ui->cbxSvm_platform->currentIndex() ).toString();
	DatosScummVM.platform          = str.isEmpty() ? "pc" : str;
	str = ui->cbxSvm_gfx_mode->itemData( ui->cbxSvm_gfx_mode->currentIndex() ).toString();
	DatosScummVM.gfx_mode          = str.isEmpty() ? "2x" : str;
	str = ui->cbxSvm_render_mode->itemData( ui->cbxSvm_render_mode->currentIndex() ).toString();
	DatosScummVM.render_mode       = str.isEmpty() ? "" : str;
	DatosScummVM.fullscreen        = fGrl->BoolToStr( ui->chkSvm_fullscreen->isChecked() );
	DatosScummVM.aspect_ratio      = fGrl->BoolToStr( ui->chkSvm_aspect_ratio->isChecked() );
	DatosScummVM.path              = ui->txtSvm_path->text();
	DatosScummVM.path_setup        = ui->txtSvm_path_setup->text();
	DatosScummVM.path_extra        = ui->txtSvm_extrapath->text();
	DatosScummVM.path_save         = ui->txtSvm_savepath->text();
	DatosScummVM.path_capturas     = ui->txtSvm_path_capturas->text();
	DatosScummVM.path_sonido       = ui->txtSvm_path_sonido->text();
	str = ui->cbxSvm_music_driver->itemData( ui->cbxSvm_music_driver->currentIndex() ).toString();
	DatosScummVM.music_driver      = str.isEmpty() ? "auto" : str;
	DatosScummVM.enable_gs         = fGrl->BoolToStr( ui->chkSvm_enable_gs->isChecked() );
	DatosScummVM.multi_midi        = fGrl->BoolToStr( ui->chkSvm_multi_midi->isChecked() );
	DatosScummVM.native_mt32       = fGrl->BoolToStr( ui->chkSvm_native_mt32->isChecked() );
	DatosScummVM.mute              = fGrl->BoolToStr( ui->chkSvm_mute->isChecked() );
	DatosScummVM.master_volume     = "192";
	DatosScummVM.music_volume      = fGrl->IntToStr( ui->h_SliderSvm_music_volume->value() );
	DatosScummVM.sfx_volume        = fGrl->IntToStr( ui->h_SliderSvm_sfx_volume->value() );
	DatosScummVM.speech_volume     = fGrl->IntToStr( ui->h_SliderSvm_speech_volume->value() );
	DatosScummVM.speech_mute       = fGrl->BoolToStr( ui->chkSvm_speech_mute->isChecked() );
	DatosScummVM.tempo             = fGrl->IntToStr( ui->h_SliderSvm_tempo->value() );
	DatosScummVM.talkspeed         = fGrl->IntToStr( ui->h_SliderSvm_talkspeed->value() );
	DatosScummVM.debuglevel        = fGrl->IntToStr( ui->h_SliderSvm_debuglevel->value() );
	str = ui->cbxSvm_cdrom->itemData( ui->cbxSvm_cdrom->currentIndex() ).toString();
	DatosScummVM.cdrom             =  str.isEmpty() ? "" : str;
	str = ui->cbxSvm_joystick_num->itemData( ui->cbxSvm_joystick_num->currentIndex() ).toString();
	DatosScummVM.joystick_num      = str.isEmpty() ? "" : str;
	str = ui->cbxSvm_output_rate->itemData( ui->cbxSvm_output_rate->currentIndex() ).toString();
	DatosScummVM.output_rate       = str.isEmpty() ? "44100" : str;
	DatosScummVM.midi_gain         = fGrl->IntToStr( ui->h_SliderSvm_midi_gain->value() );
	DatosScummVM.copy_protection   = fGrl->BoolToStr( ui->chkSvm_copy_protection->isChecked() );
	DatosScummVM.sound_font        = ui->txtSvm_soundfont->text();
	DatosScummVM.walkspeed         = fGrl->IntToStr( ui->h_SliderSvm_walkspeed->value() );
	str = ui->cbxSvm_opl_driver->itemData( ui->cbxSvm_opl_driver->currentIndex() ).toString();
	DatosScummVM.opl_driver        = str.isEmpty() ? "auto" : str;
	DatosScummVM.disable_dithering = fGrl->BoolToStr( ui->chkSvm_disable_dithering->isChecked() );
	DatosScummVM.alt_intro         = fGrl->BoolToStr( ui->chkSvm_alt_intro->isChecked() );
	DatosScummVM.boot_param        = "";
//--
	DatosScummVM.description       = "";
	DatosScummVM.emu_svmpath       = "";
}

void frmAddEditScummVM::on_twScummVM_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->txtSvm_gameid->setText( item->text(2) );
		ui->txtSvm_game_label->setText( item->text(2) );
	}
}

void frmAddEditScummVM::on_twScummVM_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twScummVM_itemClicked(current, 0);
}

void frmAddEditScummVM::on_btnSvm_Path_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path, fGrl->getDirRelative(ui->txtSvm_path->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_path = ui->txtSvm_path->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path", grlCfg.Svm_path);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_Path_clear_clicked()
{
	ui->txtSvm_path->clear();
}

void frmAddEditScummVM::on_btnSvm_PathSave_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_savepath, fGrl->getDirRelative(ui->txtSvm_savepath->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_savepath->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_savepath = ui->txtSvm_savepath->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_savepath", grlCfg.Svm_savepath);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_PathSave_clear_clicked()
{
	ui->txtSvm_savepath->clear();
}

void frmAddEditScummVM::on_btnSvm_PathExtra_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_extrapath, fGrl->getDirRelative(ui->txtSvm_extrapath->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_extrapath->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_extrapath = ui->txtSvm_extrapath->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_extrapath", grlCfg.Svm_extrapath);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_PathExtra_clear_clicked()
{
	 ui->txtSvm_extrapath->clear();
}

void frmAddEditScummVM::on_btnSvm_PathCapturas_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_capturas, fGrl->getDirRelative(ui->txtSvm_path_capturas->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_capturas->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Svm_path_capturas = ui->txtSvm_path_capturas->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_capturas", grlCfg.Svm_path_capturas);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_PathCapturas_clear_clicked()
{
	ui->txtSvm_path_capturas->clear();
}

void frmAddEditScummVM::on_btnSvm_PathSonido_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_sonido, fGrl->getDirRelative(ui->txtSvm_path_sonido->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_sonido->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Svm_path_sonido = ui->txtSvm_path_sonido->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_sonido", grlCfg.Svm_path_sonido);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_PathSonido_clear_clicked()
{
	ui->txtSvm_path_sonido->clear();
}

void frmAddEditScummVM::on_btnSvm_PathSetup_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_setup, fGrl->getDirRelative(ui->txtSvm_path_setup->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_setup->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_path_setup = ui->txtSvm_path_setup->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_setup", grlCfg.Svm_path_setup);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_PathSetup_clear_clicked()
{
	ui->txtSvm_path_setup->clear();
}

void frmAddEditScummVM::on_btnSvm_SoundFont_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Svm_soundfont, fGrl->getDirRelative(ui->txtSvm_soundfont->text()), tr("Todos los archivo") +" (*)", 0, false);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtSvm_soundfont->setText( fGrl->setDirRelative(archivo) );
		grlCfg.Svm_soundfont = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_soundfont", grlCfg.Svm_soundfont);
	}
}

void frmAddEditScummVM::on_btnSvm_SoundFont_clear_clicked()
{
	ui->txtSvm_soundfont->clear();
}

void frmAddEditScummVM::on_h_SliderSvm_music_volume_valueChanged(int value)
{
	ui->valueSliderSvm_music_volume->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_sfx_volume_valueChanged(int value)
{
	ui->valueSliderSvm_sfx_volume->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_speech_volume_valueChanged(int value)
{
	ui->valueSliderSvm_speech_volume->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_tempo_valueChanged(int value)
{
	ui->valueSliderSvm_tempo->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_talkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_talkspeed->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_debuglevel_valueChanged(int value)
{
	ui->valueSliderSvm_debuglevel->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_midi_gain_valueChanged(int value)
{
	ui->valueSliderSvm_midi_gain->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_h_SliderSvm_walkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_walkspeed->setText( fGrl->IntToStr(value) );
}

void frmAddEditScummVM::on_btnDefectoSvm_clicked()
{
	ui->h_SliderSvm_music_volume->setValue(192);
	ui->h_SliderSvm_sfx_volume->setValue(192);
	ui->h_SliderSvm_speech_volume->setValue(192);
	ui->h_SliderSvm_tempo->setValue(100);
	ui->h_SliderSvm_talkspeed->setValue(60);
	ui->h_SliderSvm_debuglevel->setValue(0);
	ui->h_SliderSvm_midi_gain->setValue(100);
	ui->h_SliderSvm_walkspeed->setValue(0);
}
