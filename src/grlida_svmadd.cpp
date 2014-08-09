/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_svmadd.h"
#include "grlida_importar_juego.h"
#include "ui_svmadd.h"

frmSvmAdd::frmSvmAdd(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmSvmAdd)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql          = m_sql;
	grlCfg       = m_cfg;
	categoria    = m_categoria;
	index_wizard = 0;

	grlDir.Home  = fGrl->GRlidaHomePath();
	grlDir.Datos = grlDir.Home +"datos/";

	cargarConfig();
	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmSvmAdd::~frmSvmAdd()
{
	delete ui;
}

void frmSvmAdd::cargarConfig()
{
	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[A-Za-z-0-9]+");

	fGrl->setIdioma(grlCfg.IdiomaSelect);

	DatosJuego   = fGrl->getDefectDatos("scummvm");
	DatosScummVM = fGrl->getDefectScummVM();

	ui->twScummVM->header()->setStretchLastSection(false);
	ui->twScummVM->header()->setMovable(false);
	ui->twScummVM->header()->setResizeMode(0, QHeaderView::Fixed  );
	ui->twScummVM->header()->setResizeMode(1, QHeaderView::Stretch);
	ui->twScummVM->header()->setResizeMode(2, QHeaderView::Fixed  );
	ui->twScummVM->setColumnWidth(0, 30);
	ui->twScummVM->setColumnWidth(2, 80);

	fGrl->cargarDatosTwLista(ui->twScummVM, grlDir.Datos +"svm_lista.txt", TwListSvm, "|");

	QValidator *validarTexto = new QRegExpValidator( regexp, this );
	ui->txtSvm_game_label->setValidator( validarTexto );

	fGrl->cargarDatosComboBox(ui->cbxSvm_language    , grlDir.Datos + fGrl->Idioma() +"/svm_idioma.txt", fGrl->Theme() +"img_lng/", "es"   , 3, "|", true, true);
	fGrl->cargarDatosComboBox(ui->cbxSvm_platform    , grlDir.Datos +"svm_platform.txt"                , fGrl->Theme() +"img16/"  , "pc"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_gfx_mode    , grlDir.Datos +"svm_gfxmode.txt"                 , fGrl->Theme() +"img16/"  , "2x"   , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_render_mode , grlDir.Datos +"svm_render_mode.txt"             , fGrl->Theme() +"img16/"  , ""     , 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxSvm_music_driver, grlDir.Datos +"svm_music_driver.txt"            , fGrl->Theme() +"img16/"  , "adlib", 3, "|");

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

	cargarDatosScummVM(DatosScummVM);
}

void frmSvmAdd::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/scummvm.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnPrevious->setIcon( QIcon(fGrl->Theme() +"img16/mp_rebobinar_atras.png") );
	ui->btnNext->setIcon( QIcon(fGrl->Theme() +"img16/mp_rebobinar_adelante.png") );

	ui->btnSvm_PathGame->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathGame_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSave->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathSave_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDescargarInfo->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnDefectoSvm->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
}

void frmSvmAdd::cargarDatosScummVM(stConfigScummVM cfgSvm)
{
	DatosScummVM = cfgSvm;

	ui->txtSvm_gameid->setText( cfgSvm.game );
	ui->txtSvm_game_label->setText( cfgSvm.game_label );
	ui->cbxSvm_language->setCurrentIndex( ui->cbxSvm_language->findData( cfgSvm.language ) );
	ui->chkSvm_subtitles->setChecked( fGrl->StrToBool( cfgSvm.subtitles ) );
	ui->cbxSvm_platform->setCurrentIndex( ui->cbxSvm_platform->findData( cfgSvm.platform ) );
	ui->cbxSvm_gfx_mode->setCurrentIndex( ui->cbxSvm_gfx_mode->findData( cfgSvm.gfx_mode ) );
	ui->cbxSvm_render_mode->setCurrentIndex( ui->cbxSvm_render_mode->findData( cfgSvm.render_mode ) );
	ui->chkSvm_fullscreen->setChecked( fGrl->StrToBool( cfgSvm.fullscreen ) );
	ui->chkSvm_aspect_ratio->setChecked( fGrl->StrToBool( cfgSvm.aspect_ratio ) );
	ui->txtSvm_path_game->setText( cfgSvm.path_game );
	ui->txtSvm_path_save->setText( cfgSvm.path_save );
	ui->cbxSvm_music_driver->setCurrentIndex( ui->cbxSvm_music_driver->findData( cfgSvm.music_driver ) );
	ui->chkSvm_enable_gs->setChecked( fGrl->StrToBool( cfgSvm.enable_gs ) );
	ui->chkSvm_multi_midi->setChecked( fGrl->StrToBool( cfgSvm.multi_midi ) );
	ui->chkSvm_native_mt32->setChecked( fGrl->StrToBool( cfgSvm.native_mt32 ) );
	ui->h_SliderSvm_music_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.music_volume ) );
	ui->h_SliderSvm_sfx_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.sfx_volume ) );
	ui->h_SliderSvm_speech_volume->setSliderPosition( fGrl->StrToInt( cfgSvm.speech_volume ) );
	ui->h_SliderSvm_tempo->setSliderPosition( fGrl->StrToInt( cfgSvm.tempo ) );
	ui->h_SliderSvm_talkspeed->setSliderPosition(fGrl->StrToInt(  cfgSvm.talkspeed ) );
	ui->h_SliderSvm_debuglevel->setSliderPosition( fGrl->StrToInt( cfgSvm.debuglevel ) );
	ui->cbxSvm_cdrom->setCurrentIndex( fGrl->StrToInt( cfgSvm.cdrom ) );
	ui->cbxSvm_joystick_num->setCurrentIndex( fGrl->StrToInt( cfgSvm.joystick_num ) );
	ui->h_SliderSvm_midi_gain->setSliderPosition( fGrl->StrToInt( cfgSvm.midi_gain ) );
	ui->h_SliderSvm_walkspeed->setSliderPosition( fGrl->StrToInt( cfgSvm.walkspeed ) );
}

void frmSvmAdd::setDatosScummVM()
{
	QString str;
	DatosScummVM.game              = ui->txtSvm_gameid->text();
	DatosScummVM.game_label        = ui->txtSvm_game_label->text();
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
	DatosScummVM.path_game         = ui->txtSvm_path_game->text();
	DatosScummVM.path_save         = ui->txtSvm_path_save->text();
	str = ui->cbxSvm_music_driver->itemData( ui->cbxSvm_music_driver->currentIndex() ).toString();
	DatosScummVM.music_driver      = str.isEmpty() ? "auto" : str;
	DatosScummVM.enable_gs         = fGrl->BoolToStr( ui->chkSvm_enable_gs->isChecked() );
	DatosScummVM.multi_midi        = fGrl->BoolToStr( ui->chkSvm_multi_midi->isChecked() );
	DatosScummVM.native_mt32       = fGrl->BoolToStr( ui->chkSvm_native_mt32->isChecked() );
	DatosScummVM.master_volume     = "192";
	DatosScummVM.music_volume      = fGrl->IntToStr( ui->h_SliderSvm_music_volume->value() );
	DatosScummVM.sfx_volume        = fGrl->IntToStr( ui->h_SliderSvm_sfx_volume->value() );
	DatosScummVM.speech_volume     = fGrl->IntToStr( ui->h_SliderSvm_speech_volume->value() );
	DatosScummVM.tempo             = fGrl->IntToStr( ui->h_SliderSvm_tempo->value() );
	DatosScummVM.talkspeed         = fGrl->IntToStr( ui->h_SliderSvm_talkspeed->value() );
	DatosScummVM.debuglevel        = fGrl->IntToStr( ui->h_SliderSvm_debuglevel->value() );
	str = ui->cbxSvm_cdrom->itemData( ui->cbxSvm_cdrom->currentIndex() ).toString();
	DatosScummVM.cdrom             =  str.isEmpty() ? "" : str;
	str = ui->cbxSvm_joystick_num->itemData( ui->cbxSvm_joystick_num->currentIndex() ).toString();
	DatosScummVM.joystick_num      = str.isEmpty() ? "" : str;
	DatosScummVM.midi_gain         = fGrl->IntToStr( ui->h_SliderSvm_midi_gain->value() );
	DatosScummVM.walkspeed         = fGrl->IntToStr( ui->h_SliderSvm_walkspeed->value() );
}

void frmSvmAdd::on_btnOk_clicked()
{
	if( ui->txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el titulo."));
	else {
		setDatosScummVM();

		DatosJuego.titulo   = ui->txtDatos_Titulo->text();
		DatosJuego.tipo_emu = "scummvm";
		DatosJuego.fecha    = fGrl->getTime();
		DatosJuego.idgrl    = sql->insertaDatos(categoria.tabla, DatosJuego);

		DatosScummVM.idgrl = DatosJuego.idgrl;
		DatosScummVM.idcat = categoria.id;
		DatosScummVM.id    = sql->insertaSvm( DatosScummVM );

		QDialog::accept();
	}
}

void frmSvmAdd::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmSvmAdd::on_btnNext_clicked()
{
	bool siguiente = false;

	if( ui->twScummVM->isItemSelected( ui->twScummVM->currentItem() ) && !ui->twScummVM->currentItem()->text(2).isEmpty() )
	{
		siguiente = true;
		if( ui->wizardSvm->currentIndex() == 1 )
		{
			if( ui->txtDatos_Titulo->text().isEmpty() )
			{
				siguiente = false;
				QMessageBox::information(this, titulo_ventana(), tr("Debes poner un titulo al juego") );
				ui->txtDatos_Titulo->setFocus();
			} else {
				siguiente = true;
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
						if( ui->txtSvm_path_game->text().isEmpty() )
						{
							siguiente = false;
							QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el directorio del juego de ScummVM") );
							ui->txtSvm_path_game->setFocus();
						} else {
							siguiente = true;
							QDir tmpdirsvm( fGrl->getDirRelative(ui->txtSvm_path_game->text(), "DosGames") );
							if( !tmpdirsvm.exists() )
							{
								siguiente = false;
								QMessageBox::information(this, titulo_ventana(), tr("Debes indicar un directorio que exista") );
								ui->txtSvm_path_game->setFocus();
							} else
								siguiente = true;
						}
					}
				}
			}
		}

		if( siguiente )
		{
			ui->btnPrevious->setEnabled( true );
			ui->wizardSvm->setCurrentIndex( ui->wizardSvm->currentIndex()+1 );
		}
		if(ui->wizardSvm->currentIndex() == 2) ui->cbxSvm_render_mode->setFocus();
		if(ui->wizardSvm->currentIndex() == 3) ui->cbxSvm_music_driver->setFocus();
		if( ui->wizardSvm->currentIndex() > 3)
		{
			ui->btnOk->setEnabled( true );
			ui->btnOk->setFocus();
			ui->btnNext->setEnabled( false );
		}
	} else {
		QMessageBox::information(this, titulo_ventana(), tr("Selecciona un juego de la lista"));
		ui->twScummVM->setFocus();
	}
}

void frmSvmAdd::on_btnPrevious_clicked()
{
	ui->wizardSvm->setCurrentIndex( ui->wizardSvm->currentIndex() - 1);
	ui->btnNext->setEnabled( true );
	if( ui->wizardSvm->currentIndex() < 1 )
		ui->btnPrevious->setEnabled( false ) ;
}

void frmSvmAdd::on_twScummVM_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->txtDatos_Titulo->setText( item->text(1) );
		ui->txtSvm_gameid->setText( item->text(2) );
		ui->txtSvm_game_label->setText( item->text(2) );
	}
}

void frmSvmAdd::on_twScummVM_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		emit on_twScummVM_itemClicked(item, 0);
		emit on_btnNext_clicked();
	}
}

void frmSvmAdd::on_twScummVM_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twScummVM_itemClicked(current, 0);
}

void frmSvmAdd::on_btnSvm_PathGame_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_game, "DosGames");

	if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
	{
		ui->txtSvm_path_game->setText( fGrl->setDirRelative(directorio, "DosGames") );
		grlCfg.Svm_path_game = ui->txtSvm_path_game->text();

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_game", grlCfg.Svm_path_game);
	}
}

void frmSvmAdd::on_btnSvm_PathGame_clear_clicked()
{
	ui->txtSvm_path_game->clear();
}

void frmSvmAdd::on_btnSvm_PathSave_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_save, "DosGames");

	if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
	{
		ui->txtSvm_path_save->setText( fGrl->setDirRelative(directorio, "DosGames") );
		grlCfg.Svm_path_save = ui->txtSvm_path_save->text();

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_save", grlCfg.Svm_path_save);
	}
}

void frmSvmAdd::on_btnSvm_PathSave_clear_clicked()
{
	ui->txtSvm_path_save->clear();
}

void frmSvmAdd::on_btnDescargarInfo_clicked()
{
	stConfigDOSBox  DatosDosBox    = fGrl->getDefectDOSBox();
	stConfigVDMSound DatosVDMSound = fGrl->getDefectVDMSound();

	DatosJuego.titulo = ui->txtDatos_Titulo->text();

	frmImportarJuego *ImportarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, this);
	ImportarJuego->setWindowFlags(Qt::Window);

	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		DatosJuego = ImportarJuego->getDatosJuegos();
		ui->txtDatos_Titulo->setText( DatosJuego.titulo );

		if( DatosJuego.tipo_emu == "scummvm" && !ImportarJuego->isSoloDatos )
		{
			cargarDatosScummVM( ImportarJuego->getDatosScummVM() );
		}
	}

	delete ImportarJuego;
}

void frmSvmAdd::on_h_SliderSvm_music_volume_valueChanged(int value)
{
	ui->valueSliderSvm_music_volume->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_sfx_volume_valueChanged(int value)
{
	ui->valueSliderSvm_sfx_volume->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_speech_volume_valueChanged(int value)
{
	ui->valueSliderSvm_speech_volume->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_tempo_valueChanged(int value)
{
	ui->valueSliderSvm_tempo->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_talkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_talkspeed->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_debuglevel_valueChanged(int value)
{
	ui->valueSliderSvm_debuglevel->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_midi_gain_valueChanged(int value)
{
	ui->valueSliderSvm_midi_gain->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_h_SliderSvm_walkspeed_valueChanged(int value)
{
	ui->valueSliderSvm_walkspeed->setText( fGrl->IntToStr(value) );
}

void frmSvmAdd::on_btnDefectoSvm_clicked()
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
