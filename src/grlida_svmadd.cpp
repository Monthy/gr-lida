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

#include "grlida_svmadd.h"
#include "grlida_importar_juego.h"

frmSvmAdd::frmSvmAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	ui.wizardSvm->setCurrentIndex(0);

	stHomeDir  = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stDatosDir = stHomeDir + "datos/";
	stTheme    = fGrl.ThemeGrl();

	createConnections();

	setTheme();

	CargarConfig();

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmSvmAdd::~frmSvmAdd()
{
	//
}

void frmSvmAdd::createConnections()
{
// Conecta los distintos botones con las funciones.
	connect( ui.btnOk            , SIGNAL( clicked() ), this, SLOT( on_btnOk()       ) );
	connect( ui.btnNext          , SIGNAL( clicked() ), this, SLOT( on_btnNext()     ) );
	connect( ui.btnPrevious      , SIGNAL( clicked() ), this, SLOT( on_btnPrevious() ) );
	connect( ui.btnDirSvm_1      , SIGNAL( clicked() ), this, SLOT( on_btnDirGame()  ) );
	connect( ui.btnDirSvm_2      , SIGNAL( clicked() ), this, SLOT( on_btnDirSave()  ) );
	connect( ui.btnDefectoSvm    , SIGNAL( clicked() ), this, SLOT( on_btnDefecto()  ) );
	connect( ui.twScummVM        , SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_twScummVM_clicked( QTreeWidgetItem* ) ) );
	connect( ui.twScummVM        , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twScummVM_Dblclicked(QTreeWidgetItem* ) ) );
	connect( ui.btnDescargarInfo , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo() ) );
}

void frmSvmAdd::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/scummvm.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnPrevious->setIcon( QIcon(stTheme+"img16/mp_rebobinar_atras.png") );
	ui.btnNext->setIcon( QIcon(stTheme+"img16/mp_rebobinar_adelante.png") );
	ui.btnDirSvm_1->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_2->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_1_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDirSvm_2_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDescargarInfo->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.btnDefectoSvm->setIcon( QIcon(stTheme+"img16/actualizar.png") );
	ui.cbxSvm_joystick_num->setItemIcon(0, QIcon(stTheme+"img16/controller.png") );
	ui.cbxSvm_joystick_num->setItemIcon(1, QIcon(stTheme+"img16/controller.png") );
}

void frmSvmAdd::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	ui.twScummVM->header()->setStretchLastSection(false);
	ui.twScummVM->header()->setMovable(false);
	ui.twScummVM->header()->setResizeMode(0, QHeaderView::Stretch);
	ui.twScummVM->header()->setResizeMode(1, QHeaderView::Fixed  );
	ui.twScummVM->setColumnWidth(1, 80);
	ui.twScummVM->clear();

	QRegExp regexp;
	regexp.setPatternSyntax( QRegExp::RegExp );
	regexp.setPattern("[A-Za-z-0-9]+");	// [A-Za-z_-]+([A-Za-z_-0-9]*)

	QValidator *validarTexto = new QRegExpValidator( regexp, this );
	ui.txtSvm_game_label->setValidator( validarTexto );

	fGrl.CargarDatosListaSvm(":/datos/svm_lista.txt"        , ui.twScummVM                    );	// Carga la lista de compatibilidad del ScummVM.
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"       , ui.cbxSvm_language    , 2, true );	// Carga la lista de idiomas
	fGrl.CargarDatosComboBox(":/datos/svm_platform.txt"     , ui.cbxSvm_platform    , 2, false);	// Carga la lista de platform
	fGrl.CargarDatosComboBox(":/datos/svm_gfxmode.txt"      , ui.cbxSvm_gfx_mode    , 3, false);	// Carga la lista de gfxmode
	fGrl.CargarDatosComboBox(":/datos/svm_render_mode.txt"  , ui.cbxSvm_render_mode , 1, false);	// Carga la lista de render_mode
	fGrl.CargarDatosComboBox(":/datos/svm_music_driver.txt" , ui.cbxSvm_music_driver, 1, false);	// Carga la lista de music_driver

	ui.cbxSvm_cdrom->clear();
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 0");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 1");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 2");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 3");
	ui.cbxSvm_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 4");

	ui.cbxSvm_language->setCurrentIndex(0);
	ui.cbxSvm_platform->setCurrentIndex(0);
	ui.cbxSvm_gfx_mode->setCurrentIndex(1);
	ui.cbxSvm_render_mode->setCurrentIndex(0);
	ui.cbxSvm_music_driver->setCurrentIndex(0);
	ui.cbxSvm_cdrom->setCurrentIndex(0);

	tempDatosJuego["Dat_favorito"] = "false";
	tempDatosJuego["Dat_rating"]   = "0";
}

void frmSvmAdd::on_twScummVM_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( item1 && item2 )
		emit on_twScummVM_clicked( ui.twScummVM->currentItem() );
	else
		return;
}

void frmSvmAdd::on_twScummVM_clicked(QTreeWidgetItem *item)
{
	if( item && item->text(1) != "" )
	{
		ui.txtDatos_Titulo->setText( item->text(0) );
		ui.txtSvm_game->setText( item->text(1) );
		ui.txtSvm_game_label->setText( item->text(1) );
	} else
		return;
}

void frmSvmAdd::on_twScummVM_Dblclicked( QTreeWidgetItem *item )
{
	if( item && item->text(1) != "" )
	{
		ui.txtDatos_Titulo->setText( item->text(0) );
		ui.txtSvm_game->setText( item->text(1) );
		ui.txtSvm_game_label->setText( item->text(1) );
		ui.btnNext->click();
	} else
		return;
}

void frmSvmAdd::on_btnPrevious()
{
	ui.wizardSvm->setCurrentIndex( ui.wizardSvm->currentIndex() - 1);
	ui.btnNext->setEnabled( true );
	if(ui.wizardSvm->currentIndex() < 1 )
		ui.btnPrevious->setEnabled( false ) ;
}

void frmSvmAdd::on_btnNext()
{
	bool siguiente;
	if( ui.twScummVM->isItemSelected( ui.twScummVM->currentItem() ) && ui.twScummVM->currentItem()->text(1) != "" )
	{
		siguiente = true;
		if( ui.wizardSvm->currentIndex() == 1 )
		{
			if( ui.txtDatos_Titulo->text() != "" && ui.txtSvm_game->text() != "" )
			{
				if( ui.txtSvm_path->text() == "" )
				{
					QMessageBox::information(this, stTituloSvm(), tr("Debes indicar el Directorio del juego") );
					siguiente = false;
				} else
					siguiente = true;
			} else {
				QMessageBox::information(this, stTituloSvm(), tr("Debes poner un Titulo al juego") );
				siguiente = false;
			}
		}
		if( siguiente == true )
		{
			ui.btnPrevious->setEnabled( true );
			ui.wizardSvm->setCurrentIndex( ui.wizardSvm->currentIndex() + 1 );
		}
		if(ui.wizardSvm->currentIndex() == 2) ui.cbxSvm_render_mode->setFocus();
		if(ui.wizardSvm->currentIndex() == 3) ui.cbxSvm_music_driver->setFocus();
		if( ui.wizardSvm->currentIndex() >= 4)
		{
			ui.btnOk->setEnabled( true );
			ui.btnOk->setFocus();
			ui.btnNext->setEnabled( false );
		}
	} else
		QMessageBox::information(this, stTituloSvm(), tr("Selecciona un juego de la lista"));
}

void frmSvmAdd::on_btnDescargarInfo()
{
	frmImportarJuego * ImportarJuego = new frmImportarJuego(ui.txtDatos_Titulo->text(), 0, Qt::Window);
	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		QFile file_thumbs, file_cover_front, file_cover_back;

		tempDatosJuego.clear();
		ui.txtDatos_Titulo->setText( ImportarJuego->DatosJuego["Dat_titulo"] );			// titulo
		tempDatosJuego["Dat_icono"]         = "scummvm";									// icono
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
		tempDatosJuego["Dat_tipo_emu"]      = "scummvm";									// tipo_emu
		tempDatosJuego["Dat_comentario"]    = ImportarJuego->DatosJuego["Dat_comentario"];	// comentario
		tempDatosJuego["Dat_favorito"]      = ImportarJuego->DatosJuego["Dat_favorito"];	// favorito
		tempDatosJuego["Dat_rating"]        = ImportarJuego->DatosJuego["Dat_rating"];		// rating
		tempDatosJuego["Dat_usuario"]       = ImportarJuego->DatosJuego["Dat_usuario"];		// usuario
	}
}

void frmSvmAdd::on_btnOk()
{
	if( ui.txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information( this, stTituloSvm(), tr("Debes poner por lo menos el titulo."));
	else {
		DatosJuego.clear();
		DatosJuego["Dat_icono"]         = "scummvm";								// icono
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
		DatosJuego["Dat_fecha"]         = fGrl.HoraFechaActual(GRLConfig["FormatoFecha"].toString());			// fecha d/m/a h:m:s
		DatosJuego["Dat_tipo_emu"]      = "scummvm";								// tipo_emu
		DatosJuego["Dat_comentario"]    = ""+tempDatosJuego["Dat_comentario"];		// comentario
		DatosJuego["Dat_favorito"]      = ""+tempDatosJuego["Dat_favorito"];		// favorito
		DatosJuego["Dat_rating"]        = ""+tempDatosJuego["Dat_rating"];			// rating
		DatosJuego["Dat_usuario"]       = ""+tempDatosJuego["Dat_usuario"];			// rating

		DatosScummVM.clear();
		DatosScummVM["Svm_game"]            = ui.txtSvm_game->text();															// game
		DatosScummVM["Svm_game_label"]      = ui.txtSvm_game_label->text();														// game_label
		DatosScummVM["Svm_language"]        = ui.cbxSvm_language->itemData( ui.cbxSvm_language->currentIndex() ).toString();	// language
		DatosScummVM["Svm_subtitles"]       = fGrl.BoolToStr( ui.chkSvm_subtitles->isChecked() );								// subtitles
		DatosScummVM["Svm_platform"]        = ui.cbxSvm_platform->itemData( ui.cbxSvm_platform->currentIndex() ).toString();	// platform
		DatosScummVM["Svm_gfx_mode"]        = ui.cbxSvm_gfx_mode->itemData( ui.cbxSvm_gfx_mode->currentIndex() ).toString();	// platform
		DatosScummVM["Svm_render_mode"]     = ui.cbxSvm_render_mode->currentText();						// render_mode
		DatosScummVM["Svm_fullscreen"]      = fGrl.BoolToStr( ui.chkSvm_fullscreen->isChecked() );		// fullscreen
		DatosScummVM["Svm_aspect_ratio"]    = fGrl.BoolToStr( ui.chkSvm_aspect_ratio->isChecked() );	// aspect_ratio
		DatosScummVM["Svm_path"]            = ui.txtSvm_path->text();									// path
		DatosScummVM["Svm_path_setup"]      = "";														// path_setup
		DatosScummVM["Svm_path_extra"]      = "";														// path_extra
		DatosScummVM["Svm_path_save"]       = ui.txtSvm_savepath->text();								// path_save
		DatosScummVM["Svm_path_capturas"]   = "";														// path_capturas
		DatosScummVM["Svm_path_sonido"]     = "";														// path_sonido
		DatosScummVM["Svm_music_driver"]    = ui.cbxSvm_music_driver->currentText();					// music_driver
		DatosScummVM["Svm_enable_gs"]       = fGrl.BoolToStr( ui.chkSvm_enable_gs->isChecked() );		// enable_gs
		DatosScummVM["Svm_multi_midi"]      = fGrl.BoolToStr( ui.chkSvm_multi_midi->isChecked() );		// multi_midi
		DatosScummVM["Svm_native_mt32"]     = fGrl.BoolToStr( ui.chkSvm_native_mt32->isChecked() );		// native_mt32
		DatosScummVM["Svm_master_volume"]   = "255";													// master_volume
		DatosScummVM["Svm_music_volume"]    = ui.posSliderSvm_1->text();								// music_volume
		DatosScummVM["Svm_sfx_volume"]      = ui.posSliderSvm_2->text();								// sfx_volume
		DatosScummVM["Svm_speech_volume"]   = ui.posSliderSvm_3->text();								// speech_volume
		DatosScummVM["Svm_tempo"]           = ui.posSliderSvm_4->text();								// tempo
		DatosScummVM["Svm_talkspeed"]       = ui.posSliderSvm_5->text();								// talkspeed
		DatosScummVM["Svm_debuglevel"]      = ui.posSliderSvm_6->text();								// debuglevel
		DatosScummVM["Svm_cdrom"]           = fGrl.IntToStr( ui.cbxSvm_cdrom->currentIndex() );			// cdrom
		DatosScummVM["Svm_joystick_num"]    = fGrl.IntToStr( ui.cbxSvm_joystick_num->currentIndex() );	//joystick_num
		DatosScummVM["Svm_output_rate"]     = "22050";													// output_rate
		DatosScummVM["Svm_midi_gain"]       = "100";													// midi_gain
		DatosScummVM["Svm_copy_protection"] = "false";													// copy_protection
		DatosScummVM["Svm_sound_font"]      = "";														// sound_font
		DatosScummVM["Svm_walkspeed"]       = "0";														// sound_font

		QDialog::accept();
	}
}

void frmSvmAdd::on_btnDirGame()
{
	ui.txtSvm_path->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), GRLConfig["Svm_path"].toString() , ui.txtSvm_path->text() ) );

	QDir dir( ui.txtSvm_path->text() );
	if( dir.exists() )
		GRLConfig["Svm_path"] = ui.txtSvm_path->text();
	else
		GRLConfig["Svm_path"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path", GRLConfig["Svm_path"].toString() );
}

void frmSvmAdd::on_btnDirSave()
{
	ui.txtSvm_savepath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), GRLConfig["Svm_savepath"].toString() , ui.txtSvm_savepath->text() ) );

	QDir dir( ui.txtSvm_savepath->text() );
	if( dir.exists() )
		GRLConfig["Svm_savepath"] = ui.txtSvm_savepath->text();
	else
		GRLConfig["Svm_savepath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_savepath", GRLConfig["Svm_savepath"].toString() );
}

void frmSvmAdd::on_btnDefecto()
{
	ui.h_SliderSvm_music_volume->setValue(192);
	ui.h_SliderSvm_sfx_volume->setValue(192);
	ui.h_SliderSvm_speech_volume->setValue(192);
	ui.h_SliderSvm_tempo->setValue(100);
	ui.h_SliderSvm_talkspeed->setValue(60);
	ui.h_SliderSvm_debuglevel->setValue(0);
}
