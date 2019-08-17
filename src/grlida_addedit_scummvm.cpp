/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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

#include <QMessageBox>
#include <QHeaderView>

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

	grlDir.Home  = fGrl->homePath();
	grlDir.Datos = grlDir.Home +"datos/";
}

frmAddEditScummVM::~frmAddEditScummVM()
{
	delete fGrl;
	delete ui;
}

void frmAddEditScummVM::cargarConfig()
{
	svm_model = new QStandardItemModel(ui->cbxScummVM);
	QTreeView *tvScummVM = new QTreeView(ui->cbxScummVM);
	tvScummVM->setFocusPolicy(Qt::ClickFocus);
	tvScummVM->setProperty("showDropIndicator", QVariant(false));
	tvScummVM->setIconSize(QSize(27, 24));
	tvScummVM->setTextElideMode(Qt::ElideNone);
	tvScummVM->setRootIsDecorated(false);
	tvScummVM->setUniformRowHeights(true);
	tvScummVM->setItemsExpandable(false);
	tvScummVM->setAllColumnsShowFocus(true);

	ui->cbxScummVM->setView(tvScummVM);
	ui->cbxScummVM->setModel(svm_model);

	QFont m_font;
	m_font.setBold(true);

	svm_model->clear();
	list_svm = fGrl->cargaListaDatos(grlDir.Datos +"svm_lista.txt", 4, "|");
	const int listSvmSize = list_svm.size();
	for (int row = 0; row < listSvmSize; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			QStandardItem *item_svm = new QStandardItem();
			switch (col)
			{
				case 0:
				{
					if (!QFile::exists(fGrl->theme() +"img_svm/"+ list_svm.at(row).icono))
						item_svm->setIcon(QIcon(fGrl->theme() +"img_svm/sinimg.png"));
					else
						item_svm->setIcon(QIcon(fGrl->theme() +"img_svm/"+ list_svm.at(row).icono));
					if (list_svm.at(row).key.isEmpty())
						item_svm->setFont(m_font);
					item_svm->setText(list_svm.at(row).titulo);
				}
				break;
				case 1: item_svm->setText(list_svm.at(row).key); break;
				case 2:
				{
					if (list_svm.at(row).extra.isEmpty())
						item_svm->setIcon(QIcon(fGrl->theme() +"img_svm/level/sinimg.png"));
					else
						item_svm->setIcon(QIcon(fGrl->theme() +"img_svm/level/"+ list_svm.at(row).extra +".png"));
				}
				break;
			}
			svm_model->setItem(row, col, item_svm);
		}
	}

	tvScummVM->header()->setStretchLastSection(false);
	tvScummVM->header()->hide();
#if QT_VERSION >= 0x050000
	tvScummVM->header()->setSectionsMovable(false);
	tvScummVM->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	tvScummVM->header()->setSectionResizeMode(1, QHeaderView::Fixed);
	tvScummVM->header()->setSectionResizeMode(2, QHeaderView::Fixed);
#else
	tvScummVM->header()->setMovable(false);
	tvScummVM->header()->setResizeMode(0, QHeaderView::Stretch);
	tvScummVM->header()->setResizeMode(1, QHeaderView::Fixed);
	tvScummVM->header()->setResizeMode(2, QHeaderView::Fixed);
#endif
	tvScummVM->setColumnWidth(1, 80);
	tvScummVM->setColumnWidth(2, 30);

	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[A-Za-z-0-9]+");

	QValidator *validarTexto = new QRegExpValidator(regexp, this);
	ui->txtSvm_game_label->setValidator(validarTexto);

	fGrl->cargarDatosComboBox(ui->cbxSvm_language    , grlDir.Datos + fGrl->idioma() +"/svm_idioma.txt", "img16/", "es"   , 3, "|", true, true);
	fGrl->cargarDatosComboBox(ui->cbxSvm_platform    , grlDir.Datos +"svm_platform.txt"                , "img16/", "pc"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_gfx_mode    , grlDir.Datos +"svm_gfxmode.txt"                 , "img16/", "2x"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_render_mode , grlDir.Datos +"svm_render_mode.txt"             , "img16/", ""     , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_music_driver, grlDir.Datos +"svm_music_driver.txt"            , "img16/", "adlib", 3, "|");

	ui->cbxSvm_opl_driver->clear();
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->theme() +"img16/sinimg.png")          , tr("por defecto")                   , "auto");
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->theme() +"img16/img_advmame2x_3x.png"), "MAME OPL emulator"                 , "mame");
	ui->cbxSvm_opl_driver->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png")      , "DOSBox OPL emulator (experimental)", "db"  );

	ui->cbxSvm_output_rate->clear();
	ui->cbxSvm_output_rate->addItem(tr("por defecto"), "");
	ui->cbxSvm_output_rate->addItem("8000" , "8000");
	ui->cbxSvm_output_rate->addItem("11025", "11025");
	ui->cbxSvm_output_rate->addItem("22050", "22050");
	ui->cbxSvm_output_rate->addItem("44100", "44100");
	ui->cbxSvm_output_rate->addItem("48000", "48000");

	ui->cbxSvm_cdrom->clear();
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 0", "0");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 1", "1");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 2", "2");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 3", "3");
	ui->cbxSvm_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 4", "4");

	ui->cbxSvm_joystick_num->clear();
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->theme() +"img16/controller.png"), tr("Sin Joystick"), "");
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->theme() +"img16/controller.png"), "Joystick 1" , "0");
	ui->cbxSvm_joystick_num->addItem(QIcon(fGrl->theme() +"img16/controller.png"), "Joystick 2" , "1");

	ui->cbxSvm_autosave_period->clear();
	ui->cbxSvm_autosave_period->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), tr("Nunca"), "0");
	ui->cbxSvm_autosave_period->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 5 minutos"), "300");
	ui->cbxSvm_autosave_period->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 10 minutos"), "600");
	ui->cbxSvm_autosave_period->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 15 minutos"), "900");
	ui->cbxSvm_autosave_period->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 30 minutos"), "1800");

	setTheme();
}

void frmAddEditScummVM::setTheme()
{
	setTabIcon(0, QIcon(fGrl->theme() +"img16/opciones.png"));
	setTabIcon(1, QIcon(fGrl->theme() +"img16/img_audio.png"));
// General
	ui->btnSvm_Path->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnSvm_Path_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSvm_PathSave->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnSvm_PathSave_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSvm_PathExtra->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnSvm_PathExtra_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSvm_SoundFont->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnSvm_SoundFont_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));

	ui->btnDefectoSvm->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
}

bool frmAddEditScummVM::isCorrectNext()
{
	bool siguiente = false;

	if (ui->txtSvm_gameid->text().isEmpty())
	{
		siguiente = false;
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner el ID del juego de ScummVM"));
		ui->txtSvm_gameid->setFocus();
	} else {
		siguiente = true;
		if (ui->txtSvm_game_label->text().isEmpty())
		{
			siguiente = false;
			QMessageBox::information(this, titulo_ventana(), tr("Debes poner la etiqueta del juego de ScummVM"));
			ui->txtSvm_game_label->setFocus();
		} else {
			siguiente = true;
			if (ui->txtSvm_path->text().isEmpty())
			{
				siguiente = false;
				QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el directorio del juego de ScummVM"));
				ui->txtSvm_path->setFocus();
			} else {
				siguiente = true;
				if (!fGrl->comprobarDirectorio(fGrl->getDirRelative(ui->txtSvm_path->text(), "DosGames"), true))
				{
					siguiente = false;
					QMessageBox::information(this, titulo_ventana(), tr("Debes indicar un directorio que exista"));
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
	int select_svm = 0;
	if (Editando)
	{
		const int listSvmSize = list_svm.size();
		for (int row = 0; row < listSvmSize; ++row)
		{
			if (list_svm.at(row).key == cfgSvm.game)
			{
				select_svm = row;
			}
		}
	}
	ui->cbxScummVM->setCurrentIndex(select_svm);

	ui->txtSvm_gameid->setText(cfgSvm.game);
	ui->txtSvm_game_label->setText(cfgSvm.game_label);
	ui->cbxSvm_language->setCurrentIndex(ui->cbxSvm_language->findData(cfgSvm.language));
	ui->chkSvm_subtitles->setChecked(fGrl->strToBool(cfgSvm.subtitles));
	ui->cbxSvm_platform->setCurrentIndex(ui->cbxSvm_platform->findData(cfgSvm.platform));
	ui->cbxSvm_gfx_mode->setCurrentIndex(ui->cbxSvm_gfx_mode->findData(cfgSvm.gfx_mode));
	ui->cbxSvm_render_mode->setCurrentIndex(ui->cbxSvm_render_mode->findData(cfgSvm.render_mode));
	ui->chkSvm_fullscreen->setChecked(fGrl->strToBool(cfgSvm.fullscreen));
	ui->chkSvm_filtering->setChecked(fGrl->strToBool(cfgSvm.filtering));
	ui->chkSvm_aspect_ratio->setChecked(fGrl->strToBool(cfgSvm.aspect_ratio));
	ui->chkSvm_originalsaveload->setChecked(fGrl->strToBool(cfgSvm.originalsaveload));
	ui->chkSvm_bright_palette->setChecked(fGrl->strToBool(cfgSvm.bright_palette));
	ui->txtSvm_path->setText(cfgSvm.path);
	ui->txtSvm_path_extra->setText(cfgSvm.path_extra);
	ui->txtSvm_path_save->setText(cfgSvm.path_save);
	ui->cbxSvm_music_driver->setCurrentIndex(ui->cbxSvm_music_driver->findData(cfgSvm.music_driver));
	ui->chkSvm_enable_gs->setChecked(fGrl->strToBool(cfgSvm.enable_gs));
	ui->chkSvm_multi_midi->setChecked(fGrl->strToBool(cfgSvm.multi_midi));
	ui->chkSvm_native_mt32->setChecked(fGrl->strToBool(cfgSvm.native_mt32));
	ui->chkSvm_mute->setChecked(fGrl->strToBool(cfgSvm.mute));
// master_volume
	ui->h_SliderSvm_music_volume->setSliderPosition(fGrl->strToInt(cfgSvm.music_volume));
	ui->h_SliderSvm_sfx_volume->setSliderPosition(fGrl->strToInt(cfgSvm.sfx_volume));
	ui->h_SliderSvm_speech_volume->setSliderPosition(fGrl->strToInt(cfgSvm.speech_volume));
	ui->chkSvm_speech_mute->setChecked(fGrl->strToBool(cfgSvm.speech_mute));
	ui->h_SliderSvm_tempo->setSliderPosition(fGrl->strToInt(cfgSvm.tempo));
	ui->h_SliderSvm_talkspeed->setSliderPosition(fGrl->strToInt(cfgSvm.talkspeed));
	ui->h_SliderSvm_debuglevel->setSliderPosition(fGrl->strToInt(cfgSvm.debuglevel));
	ui->cbxSvm_cdrom->setCurrentIndex(fGrl->strToInt(cfgSvm.cdrom));
	ui->cbxSvm_joystick_num->setCurrentIndex(fGrl->strToInt(cfgSvm.joystick_num));
	ui->cbxSvm_autosave_period->setCurrentIndex(ui->cbxSvm_autosave_period->findData(cfgSvm.autosave_period));
	ui->cbxSvm_output_rate->setCurrentIndex(ui->cbxSvm_output_rate->findData(cfgSvm.output_rate));
	ui->h_SliderSvm_midi_gain->setSliderPosition(fGrl->strToInt(cfgSvm.midi_gain));
	ui->chkSvm_copy_protection->setChecked(fGrl->strToBool(cfgSvm.copy_protection));
	ui->txtSvm_soundfont->setText(cfgSvm.sound_font);
	ui->h_SliderSvm_walkspeed->setSliderPosition(fGrl->strToInt(cfgSvm.walkspeed));
	ui->cbxSvm_opl_driver->setCurrentIndex(ui->cbxSvm_opl_driver->findData(cfgSvm.opl_driver));
	ui->chkSvm_disable_dithering->setChecked(fGrl->strToBool(cfgSvm.disable_dithering));
	ui->chkSvm_alt_intro->setChecked(fGrl->strToBool(cfgSvm.alt_intro));
// boot_param

	if (Editando && !IdGame.isEmpty())
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

	DatosScummVM.game              = ui->txtSvm_gameid->text();
	DatosScummVM.game_label        = ui->txtSvm_game_label->text();
	str = ui->cbxSvm_language->itemData(ui->cbxSvm_language->currentIndex()).toString();
	DatosScummVM.language          = str.isEmpty() ? "es" : str;
	DatosScummVM.subtitles         = fGrl->boolToStr(ui->chkSvm_subtitles->isChecked());
	str = ui->cbxSvm_platform->itemData(ui->cbxSvm_platform->currentIndex()).toString();
	DatosScummVM.platform          = str.isEmpty() ? "pc" : str;
	str = ui->cbxSvm_gfx_mode->itemData(ui->cbxSvm_gfx_mode->currentIndex()).toString();
	DatosScummVM.gfx_mode          = str.isEmpty() ? "2x" : str;
	str = ui->cbxSvm_render_mode->itemData(ui->cbxSvm_render_mode->currentIndex()).toString();
	DatosScummVM.render_mode       = str.isEmpty() ? "" : str;
	DatosScummVM.fullscreen        = fGrl->boolToStr(ui->chkSvm_fullscreen->isChecked());
	DatosScummVM.filtering         = fGrl->boolToStr(ui->chkSvm_filtering->isChecked());
	DatosScummVM.aspect_ratio      = fGrl->boolToStr(ui->chkSvm_aspect_ratio->isChecked());
	DatosScummVM.path              = ui->txtSvm_path->text();
	DatosScummVM.path_extra        = ui->txtSvm_path_extra->text();
	DatosScummVM.path_save         = ui->txtSvm_path_save->text();
	str = ui->cbxSvm_music_driver->itemData(ui->cbxSvm_music_driver->currentIndex()).toString();
	DatosScummVM.music_driver      = str.isEmpty() ? "auto" : str;
	DatosScummVM.enable_gs         = fGrl->boolToStr(ui->chkSvm_enable_gs->isChecked());
	DatosScummVM.multi_midi        = fGrl->boolToStr(ui->chkSvm_multi_midi->isChecked());
	DatosScummVM.native_mt32       = fGrl->boolToStr(ui->chkSvm_native_mt32->isChecked());
	DatosScummVM.mute              = fGrl->boolToStr(ui->chkSvm_mute->isChecked());
	DatosScummVM.master_volume     = "192";
	DatosScummVM.music_volume      = fGrl->intToStr(ui->h_SliderSvm_music_volume->value());
	DatosScummVM.sfx_volume        = fGrl->intToStr(ui->h_SliderSvm_sfx_volume->value());
	DatosScummVM.speech_volume     = fGrl->intToStr(ui->h_SliderSvm_speech_volume->value());
	DatosScummVM.speech_mute       = fGrl->boolToStr(ui->chkSvm_speech_mute->isChecked());
	DatosScummVM.tempo             = fGrl->intToStr(ui->h_SliderSvm_tempo->value());
	DatosScummVM.talkspeed         = fGrl->intToStr(ui->h_SliderSvm_talkspeed->value());
	DatosScummVM.debuglevel        = fGrl->intToStr(ui->h_SliderSvm_debuglevel->value());
	str = ui->cbxSvm_cdrom->itemData(ui->cbxSvm_cdrom->currentIndex()).toString();
	DatosScummVM.cdrom             =  str.isEmpty() ? "" : str;
	str = ui->cbxSvm_joystick_num->itemData(ui->cbxSvm_joystick_num->currentIndex()).toString();
	DatosScummVM.joystick_num      = str.isEmpty() ? "" : str;
	str = ui->cbxSvm_autosave_period->itemData(ui->cbxSvm_autosave_period->currentIndex()).toString();
	DatosScummVM.autosave_period   = str.isEmpty() ? "300" : str;
	str = ui->cbxSvm_output_rate->itemData(ui->cbxSvm_output_rate->currentIndex()).toString();
	DatosScummVM.output_rate       = str.isEmpty() ? "44100" : str;
	DatosScummVM.midi_gain         = fGrl->intToStr(ui->h_SliderSvm_midi_gain->value());
	DatosScummVM.copy_protection   = fGrl->boolToStr(ui->chkSvm_copy_protection->isChecked());
	DatosScummVM.sound_font        = ui->txtSvm_soundfont->text();
	DatosScummVM.walkspeed         = fGrl->intToStr(ui->h_SliderSvm_walkspeed->value());
	str = ui->cbxSvm_opl_driver->itemData(ui->cbxSvm_opl_driver->currentIndex()).toString();
	DatosScummVM.opl_driver        = str.isEmpty() ? "auto" : str;
	DatosScummVM.disable_dithering = fGrl->boolToStr(ui->chkSvm_disable_dithering->isChecked());
	DatosScummVM.alt_intro         = fGrl->boolToStr(ui->chkSvm_alt_intro->isChecked());
	DatosScummVM.boot_param        = "";
	DatosScummVM.originalsaveload  = fGrl->boolToStr(ui->chkSvm_originalsaveload->isChecked());
	DatosScummVM.bright_palette    = fGrl->boolToStr(ui->chkSvm_bright_palette->isChecked());
//--
	DatosScummVM.description = "";
	DatosScummVM.emu_svmpath = "";
	DatosScummVM.guioptions  = "";
}

void frmAddEditScummVM::on_cbxScummVM_activated(int index)
{
	if (index > -1)
	{
		QString gameid = svm_model->index(index, 1).data().toString();
		if (!gameid.isEmpty())
		{
			ui->txtSvm_gameid->setText(gameid);
			ui->txtSvm_game_label->setText(gameid);
		}
	}
}

void frmAddEditScummVM::on_btnSvm_Path_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Svm_path, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtSvm_path->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.Svm_path = ui->txtSvm_path->text();
	}
}

void frmAddEditScummVM::on_btnSvm_Path_clear_clicked()
{
	ui->txtSvm_path->clear();
}

void frmAddEditScummVM::on_btnSvm_PathSave_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Svm_path_save, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtSvm_path_save->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.Svm_path_save = ui->txtSvm_path_save->text();
	}
}

void frmAddEditScummVM::on_btnSvm_PathSave_clear_clicked()
{
	ui->txtSvm_path_save->clear();
}

void frmAddEditScummVM::on_btnSvm_PathExtra_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Svm_path_extra, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtSvm_path_extra->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.Svm_path_extra = ui->txtSvm_path_extra->text();
	}
}

void frmAddEditScummVM::on_btnSvm_PathExtra_clear_clicked()
{
	 ui->txtSvm_path_extra->clear();
}

void frmAddEditScummVM::on_btnSvm_SoundFont_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Svm_soundfont, "", tr("Todos los archivos") +" (*)", 0, false);

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtSvm_soundfont->setText(fGrl->setDirRelative(archivo));
			grlCfg.Svm_soundfont = ui->txtSvm_soundfont->text();
		}
	}
}

void frmAddEditScummVM::on_btnSvm_SoundFont_clear_clicked()
{
	ui->txtSvm_soundfont->clear();
}

void frmAddEditScummVM::on_h_SliderSvm_music_volume_valueChanged(int value)
{
	ui->valueSliderSvm_music_volume->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_sfx_volume_valueChanged(int value)
{
	ui->valueSliderSvm_sfx_volume->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_speech_volume_valueChanged(int value)
{
	ui->valueSliderSvm_speech_volume->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_tempo_valueChanged(int value)
{
	ui->valueSliderSvm_tempo->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_talkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_talkspeed->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_debuglevel_valueChanged(int value)
{
	ui->valueSliderSvm_debuglevel->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_midi_gain_valueChanged(int value)
{
	ui->valueSliderSvm_midi_gain->setText(fGrl->intToStr(value));
}

void frmAddEditScummVM::on_h_SliderSvm_walkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_walkspeed->setText(fGrl->intToStr(value));
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
