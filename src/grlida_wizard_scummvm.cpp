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

#include <QDesktopWidget>
#include <QMessageBox>

#include "grlida_wizard_scummvm.h"
#include "ui_wizard_scummvm.h"

frmWizardScummVM::frmWizardScummVM(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmWizardScummVM)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql          = m_sql;
	grlCfg       = m_cfg;
	categoria    = m_categoria;
	index_wizard = 0;

	grlDir.Home       = fGrl->homePath();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";
	grlDir.Temp       = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmWizardScummVM::~frmWizardScummVM()
{
	delete ui;
}

void frmWizardScummVM::cargarConfig()
{
	DatosJuego   = sql->getDefectDatos("scummvm");
	DatosScummVM = sql->getDefectScummVM();

	ui->twScummVM->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twScummVM->header()->setSectionsMovable(false);
	ui->twScummVM->header()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui->twScummVM->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui->twScummVM->header()->setSectionResizeMode(2, QHeaderView::Fixed);
#else
	ui->twScummVM->header()->setMovable(false);
	ui->twScummVM->header()->setResizeMode(0, QHeaderView::Fixed);
	ui->twScummVM->header()->setResizeMode(1, QHeaderView::Stretch);
	ui->twScummVM->header()->setResizeMode(2, QHeaderView::Fixed);
#endif
	ui->twScummVM->setColumnWidth(0, 30);
	ui->twScummVM->setColumnWidth(2, 80);

	fGrl->cargarDatosTwLista(ui->twScummVM, grlDir.Datos +"svm_lista.txt", TwListSvm, "|");

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

	cargarDatosScummVM(DatosScummVM);
}

void frmWizardScummVM::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/cat/scummvm.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
	ui->btnPrevious->setIcon(QIcon(fGrl->theme() +"img16/mp_rebobinar_atras.png"));
	ui->btnNext->setIcon(QIcon(fGrl->theme() +"img16/mp_rebobinar_adelante.png"));
// General
	ui->btnSvm_Path->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnSvm_Path_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSvm_PathSave->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnSvm_PathSave_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDescargarInfo->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDefectoSvm->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
}

void frmWizardScummVM::cargarDatosJuego(stDatosJuego datos)
{
	DatosJuego = datos;
	ui->txtDat_titulo->setText(datos.titulo);

	Thumbs      = datos.thumbs;
	CoverFront  = datos.cover_front;
	CoverBack   = datos.cover_back;
	CoverLeft   = datos.cover_left;
	CoverRight  = datos.cover_right;
	CoverTop    = datos.cover_top;
	CoverBottom = datos.cover_bottom;

// Thumbs
	file_thumbs = grlDir.Temp +"imagenes/"+ Thumbs;
	if (!QFile::exists(file_thumbs))
	{
		Thumbs = "";
		file_thumbs = "";
	}
// CoverFront
	file_cover_front = grlDir.Temp +"imagenes/"+ CoverFront;
	if (!QFile::exists(file_cover_front))
	{
		CoverFront = "";
		file_cover_front = "";
	}
// CoverBack
	file_cover_back = grlDir.Temp +"imagenes/"+ CoverBack;
	if (!QFile::exists(file_cover_back))
	{
		CoverBack = "";
		file_cover_back = "";
	}
// CoverLeft
	file_cover_left = grlDir.Temp +"imagenes/"+ CoverLeft;
	if (!QFile::exists(file_cover_left))
	{
		CoverLeft = "";
		file_cover_left = "";
	}
// CoverRight
	file_cover_right = grlDir.Temp +"imagenes/"+ CoverRight;
	if (!QFile::exists(file_cover_right))
	{
		CoverRight = "";
		file_cover_right = "";
	}
// CoverTop
	file_cover_top = grlDir.Temp +"imagenes/"+ CoverTop;
	if (!QFile::exists(file_cover_top))
	{
		CoverTop = "";
		file_cover_top = "";
	}
// CoverBottom
	file_cover_bottom = grlDir.Temp +"imagenes/"+ CoverBottom;
	if (!QFile::exists(file_cover_bottom))
	{
		CoverBottom = "";
		file_cover_bottom = "";
	}
}

bool frmWizardScummVM::setDatosJuegos()
{
	bool isOk = false;

	DatosJuego.icono          = "cat/scummvm.png";
	DatosJuego.titulo         = ui->txtDat_titulo->text();
	DatosJuego.titulo_guiones = sql->removeAccents(DatosJuego.titulo);
	DatosJuego.tipo_emu       = "scummvm";
	DatosJuego.fecha          = sql->getTime();
	DatosJuego.idgrl          = sql->insertaDatos(categoria.tabla, DatosJuego);
	isOk = DatosJuego.idgrl.isEmpty() ? false : true;

	DatosJuego.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
	grlDir.DatosGame    = grlDir.DatosDbGrl + DatosJuego.game_dir +"/";

	if (isOk)
	{
		DatosJuego.thumbs       = "";
		DatosJuego.cover_front  = "";
		DatosJuego.cover_back   = "";
		DatosJuego.cover_left   = "";
		DatosJuego.cover_right  = "";
		DatosJuego.cover_top    = "";
		DatosJuego.cover_bottom = "";

		fGrl->comprobarDirectorio(grlDir.DatosGame);
		fGrl->comprobarDirectorio(grlDir.DatosGame +"archivos/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"caja/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"capturas/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/small/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"sonidos/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"videos/");

		if (!Thumbs.isEmpty() && QFile::exists(file_thumbs))
		{
			DatosJuego.thumbs = "thumbs."+ grlCfg.thumb_format.toLower();
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "thumbs", DatosJuego.thumbs);
			fGrl->saveThumbs(file_thumbs, grlDir.DatosGame + DatosJuego.thumbs, grlCfg.thumb_width, grlCfg.thumb_height, false, false, grlCfg.thumb_format, grlCfg.thumb_quality);
		}

		if (!CoverFront.isEmpty() && QFile::exists(file_cover_front))
		{
			DatosJuego.cover_front = "cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_front", DatosJuego.cover_front);
			fGrl->copiarArchivo(file_cover_front, grlDir.DatosGame +"caja/"+ DatosJuego.cover_front, false, true);
		}

		if (!CoverBack.isEmpty() && QFile::exists(file_cover_back))
		{
			DatosJuego.cover_back = "cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_back", DatosJuego.cover_back);
			fGrl->copiarArchivo(file_cover_back, grlDir.DatosGame +"caja/"+ DatosJuego.cover_back, false, true);
		}

		if (!CoverLeft.isEmpty() && QFile::exists(file_cover_left))
		{
			DatosJuego.cover_left = "cover_left"+ fGrl->getInfoFile(file_cover_left).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_left", DatosJuego.cover_left);
			fGrl->copiarArchivo(file_cover_left, grlDir.DatosGame +"caja/"+ DatosJuego.cover_left, false, true);
		}

		if (!CoverRight.isEmpty() && QFile::exists(file_cover_right))
		{
			DatosJuego.cover_right = "cover_right"+ fGrl->getInfoFile(file_cover_right).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_right", DatosJuego.cover_right);
			fGrl->copiarArchivo(file_cover_right, grlDir.DatosGame +"caja/"+ DatosJuego.cover_right, false, true);
		}

		if (!CoverTop.isEmpty() && QFile::exists(file_cover_top))
		{
			DatosJuego.cover_top = "cover_top"+ fGrl->getInfoFile(file_cover_top).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_top", DatosJuego.cover_top);
			fGrl->copiarArchivo(file_cover_top, grlDir.DatosGame +"caja/"+ DatosJuego.cover_top, false, true);
		}

		if (!CoverBottom.isEmpty() && QFile::exists(file_cover_bottom))
		{
			DatosJuego.cover_bottom = "cover_bottom"+ fGrl->getInfoFile(file_cover_bottom).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_bottom", DatosJuego.cover_bottom);
			fGrl->copiarArchivo(file_cover_bottom, grlDir.DatosGame +"caja/"+ DatosJuego.cover_bottom, false, true);
		}

		if (listImagenesImportadas.size() > 0)
		{
			stDatosImagenes imagen;
			QString dir_out = grlDir.DatosGame +"imagenes/";
			const int numImagenes = listImagenesImportadas.size();
			for (int i = 0; i < numImagenes; ++i)
			{
				imagen = listImagenesImportadas.at(i);

				if (imagen.isImport)
					fGrl->copiarArchivo(imagen.dir_in +"imagenes/"+ imagen.nombre, dir_out + imagen.nombre, false, true);
				else
					fGrl->copiarArchivo(imagen.dir_in + imagen.nombre, dir_out + imagen.nombre, false, true);

				fGrl->saveThumbs(dir_out + imagen.nombre, dir_out +"small/"+ imagen.nombre +".jpg", grlCfg.thumb_img_width, grlCfg.thumb_img_height, false, true, "JPG", grlCfg.thumb_img_quality, true);
			}
		}
	}

	return isOk;
}

void frmWizardScummVM::cargarDatosScummVM(stConfigScummVM cfgSvm)
{
	DatosScummVM = cfgSvm;

	ui->txtSvm_gameid->setText(cfgSvm.game);
	ui->txtSvm_game_label->setText(cfgSvm.game_label);
	ui->cbxSvm_language->setCurrentIndex(ui->cbxSvm_language->findData(cfgSvm.language));
	ui->chkSvm_subtitles->setChecked(fGrl->strToBool(cfgSvm.subtitles));
	ui->cbxSvm_platform->setCurrentIndex(ui->cbxSvm_platform->findData(cfgSvm.platform));
	ui->cbxSvm_gfx_mode->setCurrentIndex(ui->cbxSvm_gfx_mode->findData(cfgSvm.gfx_mode));
	ui->cbxSvm_render_mode->setCurrentIndex(ui->cbxSvm_render_mode->findData(cfgSvm.render_mode));
	ui->chkSvm_fullscreen->setChecked(fGrl->strToBool(cfgSvm.fullscreen));
	ui->chkSvm_filtering->setChecked(fGrl->strToBool(cfgSvm.filtering));
	ui->chkSvm_originalsaveload->setChecked(fGrl->strToBool(cfgSvm.originalsaveload));
	ui->chkSvm_bright_palette->setChecked(fGrl->strToBool(cfgSvm.bright_palette));
	ui->chkSvm_aspect_ratio->setChecked(fGrl->strToBool(cfgSvm.aspect_ratio));
	ui->txtSvm_path->setText(cfgSvm.path);
	ui->txtSvm_path_save->setText(cfgSvm.path_save);
	ui->cbxSvm_music_driver->setCurrentIndex(ui->cbxSvm_music_driver->findData(cfgSvm.music_driver));
	ui->chkSvm_enable_gs->setChecked(fGrl->strToBool(cfgSvm.enable_gs));
	ui->chkSvm_multi_midi->setChecked(fGrl->strToBool(cfgSvm.multi_midi));
	ui->chkSvm_native_mt32->setChecked(fGrl->strToBool(cfgSvm.native_mt32));
	ui->h_SliderSvm_music_volume->setSliderPosition(fGrl->strToInt(cfgSvm.music_volume));
	ui->h_SliderSvm_sfx_volume->setSliderPosition(fGrl->strToInt(cfgSvm.sfx_volume));
	ui->h_SliderSvm_speech_volume->setSliderPosition(fGrl->strToInt(cfgSvm.speech_volume));
	ui->h_SliderSvm_tempo->setSliderPosition(fGrl->strToInt(cfgSvm.tempo));
	ui->h_SliderSvm_talkspeed->setSliderPosition(fGrl->strToInt(cfgSvm.talkspeed));
	ui->h_SliderSvm_debuglevel->setSliderPosition(fGrl->strToInt(cfgSvm.debuglevel));
	ui->cbxSvm_cdrom->setCurrentIndex(fGrl->strToInt(cfgSvm.cdrom));
	ui->cbxSvm_joystick_num->setCurrentIndex(fGrl->strToInt(cfgSvm.joystick_num));
	ui->h_SliderSvm_midi_gain->setSliderPosition(fGrl->strToInt(cfgSvm.midi_gain));
	ui->h_SliderSvm_walkspeed->setSliderPosition(fGrl->strToInt(cfgSvm.walkspeed));
}

void frmWizardScummVM::setDatosScummVM()
{
	QString str;

	DatosScummVM.game             = ui->txtSvm_gameid->text();
	DatosScummVM.game_label       = ui->txtSvm_game_label->text();
	str = ui->cbxSvm_language->itemData(ui->cbxSvm_language->currentIndex()).toString();
	DatosScummVM.language         = str.isEmpty() ? "es" : str;
	DatosScummVM.subtitles        = fGrl->boolToStr(ui->chkSvm_subtitles->isChecked());
	str = ui->cbxSvm_platform->itemData(ui->cbxSvm_platform->currentIndex()).toString();
	DatosScummVM.platform         = str.isEmpty() ? "pc" : str;
	str = ui->cbxSvm_gfx_mode->itemData(ui->cbxSvm_gfx_mode->currentIndex()).toString();
	DatosScummVM.gfx_mode         = str.isEmpty() ? "2x" : str;
	str = ui->cbxSvm_render_mode->itemData(ui->cbxSvm_render_mode->currentIndex()).toString();
	DatosScummVM.render_mode      = str.isEmpty() ? "" : str;
	DatosScummVM.fullscreen       = fGrl->boolToStr(ui->chkSvm_fullscreen->isChecked());
	DatosScummVM.filtering        = fGrl->boolToStr(ui->chkSvm_filtering->isChecked());
	DatosScummVM.aspect_ratio     = fGrl->boolToStr(ui->chkSvm_aspect_ratio->isChecked());
	DatosScummVM.path             = ui->txtSvm_path->text();
	DatosScummVM.path_save        = ui->txtSvm_path_save->text();
	str = ui->cbxSvm_music_driver->itemData(ui->cbxSvm_music_driver->currentIndex()).toString();
	DatosScummVM.music_driver     = str.isEmpty() ? "auto" : str;
	DatosScummVM.enable_gs        = fGrl->boolToStr(ui->chkSvm_enable_gs->isChecked());
	DatosScummVM.multi_midi       = fGrl->boolToStr(ui->chkSvm_multi_midi->isChecked());
	DatosScummVM.native_mt32      = fGrl->boolToStr(ui->chkSvm_native_mt32->isChecked());
	DatosScummVM.master_volume    = "192";
	DatosScummVM.music_volume     = fGrl->intToStr(ui->h_SliderSvm_music_volume->value());
	DatosScummVM.sfx_volume       = fGrl->intToStr(ui->h_SliderSvm_sfx_volume->value());
	DatosScummVM.speech_volume    = fGrl->intToStr(ui->h_SliderSvm_speech_volume->value());
	DatosScummVM.tempo            = fGrl->intToStr(ui->h_SliderSvm_tempo->value());
	DatosScummVM.talkspeed        = fGrl->intToStr(ui->h_SliderSvm_talkspeed->value());
	DatosScummVM.debuglevel       = fGrl->intToStr(ui->h_SliderSvm_debuglevel->value());
	str = ui->cbxSvm_cdrom->itemData(ui->cbxSvm_cdrom->currentIndex()).toString();
	DatosScummVM.cdrom            =  str.isEmpty() ? "" : str;
	str = ui->cbxSvm_joystick_num->itemData(ui->cbxSvm_joystick_num->currentIndex()).toString();
	DatosScummVM.joystick_num     = str.isEmpty() ? "" : str;
	DatosScummVM.midi_gain        = fGrl->intToStr(ui->h_SliderSvm_midi_gain->value());
	DatosScummVM.walkspeed        = fGrl->intToStr(ui->h_SliderSvm_walkspeed->value());
	DatosScummVM.originalsaveload = fGrl->boolToStr(ui->chkSvm_originalsaveload->isChecked());
	DatosScummVM.bright_palette   = fGrl->boolToStr(ui->chkSvm_bright_palette->isChecked());
}

void frmWizardScummVM::on_btnOk_clicked()
{
	if (ui->txtDat_titulo->text().isEmpty())
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el título."));
	else {
		if (setDatosJuegos())
		{
			setDatosScummVM();

			DatosScummVM.idgrl = DatosJuego.idgrl;
			DatosScummVM.idcat = categoria.id;
			DatosScummVM.id    = sql->insertaSvm(DatosScummVM);

			if (grlCfg.ScummVMSaveConfFile)
			{
				DatosScummVM.emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;
				fGrl->creaIniScummVM(DatosScummVM, grlDir.DatosGame +"scummvm.ini");
			}

		// Guardamos los cambios en la base de datos.
			sql->comit();
			QDialog::accept();
		}
	}
}

void frmWizardScummVM::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmWizardScummVM::on_btnNext_clicked()
{
	bool siguiente = false;

	if (ui->twScummVM->isItemSelected(ui->twScummVM->currentItem()) && !ui->twScummVM->currentItem()->text(2).isEmpty())
	{
		siguiente = true;
		if (ui->wizardSvm->currentIndex() == 1)
		{
			if (ui->txtDat_titulo->text().isEmpty())
			{
				siguiente = false;
				QMessageBox::information(this, titulo_ventana(), tr("Debes poner un título al juego"));
				ui->txtDat_titulo->setFocus();
			} else {
				siguiente = true;
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
							QDir tmpdirsvm(fGrl->getDirRelative(ui->txtSvm_path->text(), "DosGames"));
							if (!tmpdirsvm.exists())
							{
								siguiente = false;
								QMessageBox::information(this, titulo_ventana(), tr("Debes indicar un directorio que exista"));
								ui->txtSvm_path->setFocus();
							} else
								siguiente = true;
						}
					}
				}
			}
		}

		if (siguiente)
		{
			ui->btnPrevious->setEnabled(true);
			ui->wizardSvm->setCurrentIndex(ui->wizardSvm->currentIndex()+1);
		}
		if (ui->wizardSvm->currentIndex() == 2) ui->cbxSvm_render_mode->setFocus();
		if (ui->wizardSvm->currentIndex() == 3) ui->cbxSvm_music_driver->setFocus();
		if (ui->wizardSvm->currentIndex() > 3)
		{
			ui->btnOk->setEnabled(true);
			ui->btnOk->setFocus();
			ui->btnNext->setEnabled(false);
		}
	} else {
		QMessageBox::information(this, titulo_ventana(), tr("Selecciona un juego de la lista"));
		ui->twScummVM->setFocus();
	}
}

void frmWizardScummVM::on_btnPrevious_clicked()
{
	ui->wizardSvm->setCurrentIndex(ui->wizardSvm->currentIndex() - 1);
	ui->btnNext->setEnabled(true);
	if (ui->wizardSvm->currentIndex() < 1)
		ui->btnPrevious->setEnabled(false) ;
}

void frmWizardScummVM::on_twScummVM_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->txtDat_titulo->setText(item->text(1));
		ui->txtSvm_gameid->setText(item->text(2));
		ui->txtSvm_game_label->setText(item->text(2));
	}
}

void frmWizardScummVM::on_twScummVM_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		on_twScummVM_itemClicked(item, 0);
		on_btnNext_clicked();
	}
}

void frmWizardScummVM::on_twScummVM_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		on_twScummVM_itemClicked(current, 0);
}

void frmWizardScummVM::on_btnSvm_Path_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Svm_path, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtSvm_path->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.Svm_path = ui->txtSvm_path->text();
	}
}

void frmWizardScummVM::on_btnSvm_Path_clear_clicked()
{
	ui->txtSvm_path->clear();
}

void frmWizardScummVM::on_btnSvm_PathSave_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Svm_path_save, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtSvm_path_save->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.Svm_path_save = ui->txtSvm_path_save->text();
	}
}

void frmWizardScummVM::on_btnSvm_PathSave_clear_clicked()
{
	ui->txtSvm_path_save->clear();
}

void frmWizardScummVM::on_btnDescargarInfo_clicked()
{
	stConfigDOSBox  DatosDosBox    = sql->getDefectDOSBox();
	stConfigVDMSound DatosVDMSound = sql->getDefectVDMSound();

	DatosJuego.titulo = ui->txtDat_titulo->text();

	frmImportarJuego *importarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, false, this);
	importarJuego->setWindowFlags(Qt::Window);

	if (importarJuego->exec() == QDialog::Accepted)
	{
		grlCfg = importarJuego->getGrlCfg();

		if (importarJuego->getDatosImport().size() > 0)
		{
			stImport import = importarJuego->getDatosImport().at(0);
			DatosJuego = import.datos;
			listImagenesImportadas = import.imagenes;

			cargarDatosJuego(DatosJuego);

			if (!import.solo_datos && DatosJuego.tipo_emu == "scummvm")
			{
				cargarDatosScummVM(import.scummvm);
			}
		}
	} else {
		grlCfg = importarJuego->getGrlCfg();
	}

	delete importarJuego;
}

void frmWizardScummVM::on_h_SliderSvm_music_volume_valueChanged(int value)
{
	ui->valueSliderSvm_music_volume->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_sfx_volume_valueChanged(int value)
{
	ui->valueSliderSvm_sfx_volume->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_speech_volume_valueChanged(int value)
{
	ui->valueSliderSvm_speech_volume->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_tempo_valueChanged(int value)
{
	ui->valueSliderSvm_tempo->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_talkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_talkspeed->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_debuglevel_valueChanged(int value)
{
	ui->valueSliderSvm_debuglevel->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_midi_gain_valueChanged(int value)
{
	ui->valueSliderSvm_midi_gain->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_h_SliderSvm_walkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_walkspeed->setText(fGrl->intToStr(value));
}

void frmWizardScummVM::on_btnDefectoSvm_clicked()
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
