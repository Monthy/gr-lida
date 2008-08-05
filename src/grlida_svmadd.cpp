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
#include "grlida_importar_juego_info.h"

frmSvmAdd::frmSvmAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir  = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stDatosDir = stHomeDir + "datos/";

	stTheme = fGrl.ThemeGrl();
	setTheme();

	ui.wizardSvm->setCurrentIndex(0);

// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		, SIGNAL( clicked() ), this, SLOT( on_btnOk()       ) );
	connect( ui.btnNext		, SIGNAL( clicked() ), this, SLOT( on_btnNext()     ) );
	connect( ui.btnPrevious	, SIGNAL( clicked() ), this, SLOT( on_btnPrevious() ) );
	connect( ui.btnDirSvm_1	, SIGNAL( clicked() ), this, SLOT( on_btnDirGame()  ) );
	connect( ui.btnDirSvm_2	, SIGNAL( clicked() ), this, SLOT( on_btnDirSave()  ) );
	connect( ui.btnDefectoSvm,SIGNAL( clicked() ), this, SLOT( on_btnDefecto()  ) );
	connect( ui.twScummVM	, SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_twScummVM_clicked( QTreeWidgetItem* )));
	connect( ui.twScummVM	, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twScummVM_Dblclicked(QTreeWidgetItem* )));
	connect( ui.btnDescargarInfo , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo()  ) );

	ui.twScummVM->header()->setStretchLastSection(true);
	ui.twScummVM->header()->setMovable(false);
	ui.twScummVM->header()->resizeSection(0, 340);
	ui.twScummVM->header()->resizeSection(1, 80);
	ui.twScummVM->clear();

	fGrl.CargarDatosListaSvm(":/datos/svm_lista.txt"		, ui.twScummVM ); 			// Carga la lista de compatibilidad del ScummVM.
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"		, ui.cbxSvm_language, 2, true );	// Carga la lista de idiomas
	fGrl.CargarDatosComboBox(":/datos/svm_platform.txt"		, ui.cbxSvm_platform, 2, false);	// Carga la lista de platform
	fGrl.CargarDatosComboBox(":/datos/svm_gfxmode.txt"		, ui.cbxSvm_gfx_mode, 3, false);	// Carga la lista de gfxmode
	fGrl.CargarDatosComboBox(":/datos/svm_render_mode.txt"	, ui.cbxSvm_render_mode, 1, false);	// Carga la lista de render_mode
	fGrl.CargarDatosComboBox(":/datos/svm_music_driver.txt"	, ui.cbxSvm_music_driver, 1, false);	// Carga la lista de music_driver

	ui.cbxSvm_language->setCurrentIndex(0);
	ui.cbxSvm_platform->setCurrentIndex(0);
	ui.cbxSvm_gfx_mode->setCurrentIndex(0);
	ui.cbxSvm_render_mode->setCurrentIndex(0);
	ui.cbxSvm_music_driver->setCurrentIndex(0);

	CargaUltimosDirectorios();

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmSvmAdd::~frmSvmAdd(){}

void frmSvmAdd::setTheme()
{
	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnPrevious->setIcon( QIcon(stTheme+"img16/mp_rebobinar_atras.png") );
	ui.btnNext->setIcon( QIcon(stTheme+"img16/mp_rebobinar_adelante.png") );
	ui.btnDirSvm_1->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDirSvm_2->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDefectoSvm->setIcon( QIcon(stTheme+"img16/actualizar.png") );
}

void frmSvmAdd::CargaUltimosDirectorios()
{
	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	UltimoPath.clear();
	lastdir.beginGroup("UltimoDirectorio");
		UltimoPath["Svm_path"]            = lastdir.value("Svm_path", "").toString();
		UltimoPath["Svm_savepath"]        = lastdir.value("Svm_savepath", "").toString();
		UltimoPath["Svm_extrapath"]       = lastdir.value("Svm_extrapath", "").toString();
		UltimoPath["Svm_path_capturas"]   = lastdir.value("Svm_path_capturas", "").toString();
		UltimoPath["Svm_path_sonido"]     = lastdir.value("Svm_path_sonido", "").toString();
		UltimoPath["Svm_path_setup"]      = lastdir.value("Svm_path_setup", "").toString();
		UltimoPath["Svm_soundfont"]       = lastdir.value("Svm_soundfont", "").toString();
	lastdir.endGroup();
}

void frmSvmAdd::on_twScummVM_currentItemChanged(QTreeWidgetItem *item1,QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
		emit on_twScummVM_clicked(  ui.twScummVM->currentItem() );
	else
		return;
}

void frmSvmAdd::on_twScummVM_clicked( QTreeWidgetItem *item )
{
	if( item && item->text(1)!="")
	{
		ui.txtDatos_Titulo->setText( item->text(0) );
		ui.txtSvm_game_label->setText( item->text(1) );
	} else
		return;
}

void frmSvmAdd::on_twScummVM_Dblclicked( QTreeWidgetItem *item )
{
	if( item && item->text(1)!="")
	{
		ui.txtDatos_Titulo->setText( item->text(0) );
		ui.txtSvm_game_label->setText( item->text(1) );
		ui.btnNext->click();
	} else
		return;
}

void frmSvmAdd::on_btnPrevious()
{
	ui.wizardSvm->setCurrentIndex(ui.wizardSvm->currentIndex()-1);
	ui.btnNext->setEnabled( true );
	if(ui.wizardSvm->currentIndex() < 1 )
		ui.btnPrevious->setEnabled(false) ;
}

void frmSvmAdd::on_btnNext()
{
	bool siguiente;
	if((ui.twScummVM->isItemSelected(ui.twScummVM->currentItem())) && (ui.twScummVM->currentItem()->text(1)!=""))
	{
		siguiente = true;
		if(ui.wizardSvm->currentIndex() == 1)
		{
			if((ui.txtDatos_Titulo->text() != "") && (ui.txtSvm_game_label->text() != ""))
			{
				if( ui.txtSvm_path->text() == "")
				{
					QMessageBox::information(this, stTituloSvm(), tr("Debes indicar el Directorio del juego"));
					siguiente = false;
				} else
					siguiente = true;
			} else {
				QMessageBox::information(this, stTituloSvm(), tr("Debes poner un Titulo al juego"));
				siguiente = false;
			}
		}
		if( siguiente == true )
		{
			ui.btnPrevious->setEnabled(true);
			ui.wizardSvm->setCurrentIndex(ui.wizardSvm->currentIndex()+1);
		}
		if(ui.wizardSvm->currentIndex() == 2) ui.cbxSvm_render_mode->setFocus();
		if(ui.wizardSvm->currentIndex() == 3) ui.cbxSvm_music_driver->setFocus();
		if( ui.wizardSvm->currentIndex() >= 4)
		{
			ui.btnOk->setEnabled(true);
			ui.btnOk->setFocus();
			ui.btnNext->setEnabled(false);
		}
	} else
		QMessageBox::information(this, stTituloSvm(), tr("Selecciona un juego de la lista"));
}

void frmSvmAdd::on_btnDescargarInfo()
{
	frmImportarJuegoInfo * ImportarJuegoInfo = new frmImportarJuegoInfo(0, Qt::Window);
	if( ImportarJuegoInfo->exec() == QDialog::Accepted )
	{
		QFile file_thumbs, file_cover_front, file_cover_back;

		TempDatosJuego.clear();
		ui.txtDatos_Titulo->setText( ImportarJuegoInfo->DatosJuego["titulo"] )			;// titulo
		TempDatosJuego["icono"]			= "scummvm"										;//icono
		TempDatosJuego["titulo"]		= ui.txtDatos_Titulo->text()					;//titulo
		TempDatosJuego["subtitulo"]		= ImportarJuegoInfo->DatosJuego["subtitulo"]	;//subtitulo
		TempDatosJuego["genero"]		= ImportarJuegoInfo->DatosJuego["genero"]		;//genero
		TempDatosJuego["compania"]		= ImportarJuegoInfo->DatosJuego["compania"]		;//compania
		TempDatosJuego["desarrollador"]	= ImportarJuegoInfo->DatosJuego["desarrollador"];//desarrollador
		TempDatosJuego["tema"]			= ImportarJuegoInfo->DatosJuego["tema"] 		;//tema
		TempDatosJuego["idioma"]		= ImportarJuegoInfo->DatosJuego["idioma"]		;//idioma
		TempDatosJuego["formato"]		= ImportarJuegoInfo->DatosJuego["formato"]		;//formato
		TempDatosJuego["anno"]			= ImportarJuegoInfo->DatosJuego["anno"]			;//anno
		TempDatosJuego["numdisc"]		= ImportarJuegoInfo->DatosJuego["numdisc"]		;//numdisc
		TempDatosJuego["sistemaop"]		= ImportarJuegoInfo->DatosJuego["sistemaop"]	;//sistemaop
		TempDatosJuego["tamano"]		= ImportarJuegoInfo->DatosJuego["tamano"]		;//tamano
		TempDatosJuego["graficos"]		= ImportarJuegoInfo->DatosJuego["graficos"]		;//graficos
		TempDatosJuego["sonido"]		= ImportarJuegoInfo->DatosJuego["sonido"]		;//sonido
		TempDatosJuego["jugabilidad"]	= ImportarJuegoInfo->DatosJuego["jugabilidad"]	;//jugabilidad
		TempDatosJuego["original"]		= ImportarJuegoInfo->DatosJuego["original"]		;//original
		TempDatosJuego["estado"]		= ImportarJuegoInfo->DatosJuego["estado"]		;//estado
		TempDatosJuego["tipo_emu"]		= "scummvm"										;//tipo_emu
		TempDatosJuego["comentario"]	= ImportarJuegoInfo->DatosJuego["comentario"]	;//comentario
		TempDatosJuego["favorito"]		= ImportarJuegoInfo->DatosJuego["favorito"]		;//favorito

		TempDatosJuego["thumbs"] = ImportarJuegoInfo->DatosJuego["thumbs"];
		if( !file_thumbs.exists( TempDatosJuego["thumbs"] ) )
			TempDatosJuego["thumbs"] = "";

		TempDatosJuego["cover_front"] = ImportarJuegoInfo->DatosJuego["cover_front"];
		if( !file_cover_front.exists( TempDatosJuego["cover_front"] ) )
			TempDatosJuego["cover_front"] = "";

		TempDatosJuego["cover_back"] = ImportarJuegoInfo->DatosJuego["cover_back"];
		if( !file_cover_back.exists( TempDatosJuego["cover_back"] ) )
			TempDatosJuego["cover_back"] = "";
	}

}

void frmSvmAdd::on_btnOk()
{
	if( ui.txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information( this, stTituloSvm(), tr("Debes poner por lo menos el titulo."));
	else {
		DatosJuego.clear();
		DatosJuego["icono"]			= "scummvm"							;//icono
		DatosJuego["titulo"]		= ui.txtDatos_Titulo->text()		;//titulo
		DatosJuego["subtitulo"]		= ""+TempDatosJuego["subtitulo"]	;//subtitulo
		DatosJuego["genero"]		= ""+TempDatosJuego["genero"]		;//genero
		DatosJuego["compania"]		= ""+TempDatosJuego["compania"]		;//compania
		DatosJuego["desarrollador"]	= ""+TempDatosJuego["desarrollador"];//desarrollador
		DatosJuego["tema"]			= ""+TempDatosJuego["tema"]			;//tema
		DatosJuego["idioma"]		= ""+TempDatosJuego["idioma"]		;//idioma
		DatosJuego["formato"]		= ""+TempDatosJuego["formato"]		;//formato
		DatosJuego["anno"]			= ""+TempDatosJuego["anno"]			;//anno
		DatosJuego["numdisc"]		= ""+TempDatosJuego["numdisc"]		;//numdisc
		DatosJuego["sistemaop"]		= ""+TempDatosJuego["sistemaop"]	;//sistemaop
		DatosJuego["tamano"]		= ""+TempDatosJuego["tamano"]		;//tamano
		DatosJuego["graficos"]		= ""+TempDatosJuego["graficos"]		;//graficos
		DatosJuego["sonido"]		= ""+TempDatosJuego["sonido"]		;//sonido
		DatosJuego["jugabilidad"]	= ""+TempDatosJuego["jugabilidad"]	;//jugabilidad
		DatosJuego["original"]		= ""+TempDatosJuego["original"]		;//original
		DatosJuego["estado"]		= ""+TempDatosJuego["estado"]		;//estado
		DatosJuego["thumbs"]		= ""+TempDatosJuego["thumbs"]		;//thumbs
		DatosJuego["cover_front"]	= ""+TempDatosJuego["cover_front"]	;//cover_front
		DatosJuego["cover_back"]	= ""+TempDatosJuego["cover_back"]	;//cover_back
		DatosJuego["fecha"]			= fGrl.HoraFechaActual()			;//fecha d/m/a h:m:s
		DatosJuego["tipo_emu"]		= "scummvm"							;//tipo_emu
		DatosJuego["comentario"]	= ""+TempDatosJuego["comentario"]	;//comentario
		DatosJuego["favorito"]		= ""+TempDatosJuego["favorito"]		;//favorito

		QString tmp_language, tmp_platform, tmp_gfx_mode;
		QStringList tmp_list;

		tmp_list.clear();
		tmp_list     = ui.cbxSvm_language->currentText().split(" - ");
		tmp_language = tmp_list[1];

		tmp_list.clear();
		tmp_list     = ui.cbxSvm_platform->currentText().split(" - ");
		tmp_platform = tmp_list[1];

		tmp_list.clear();
		tmp_list     = ui.cbxSvm_gfx_mode->currentText().split(" - ");
		tmp_gfx_mode = tmp_list[1];

		if( tmp_language == "" ) tmp_language = "" ;
		if( tmp_platform == "" ) tmp_platform = "" ;
		if( tmp_gfx_mode == "" ) tmp_gfx_mode = "" ;

		DatosScummvm.clear();
		DatosScummvm["game"] = ui.txtSvm_game_label->text()	; //game
		DatosScummvm["language"] = tmp_language		; //language
	// subtitles
		if( ui.chkSvm_subtitles->isChecked() )
			DatosScummvm["subtitles"] = "true";
		else DatosScummvm["subtitles"] = "false";

		DatosScummvm["platform"] = tmp_platform; // platform
		DatosScummvm["gfx_mode"] = tmp_gfx_mode; // gfx_mode
	// render_mode
		if( ui.cbxSvm_render_mode->currentText()!="" )
			DatosScummvm["render_mode"] = ui.cbxSvm_render_mode->currentText();
		else DatosScummvm["render_mode"] = "";
	// fullscreen
		if( ui.chkSvm_fullscreen->isChecked() )
			DatosScummvm["fullscreen"] = "true";
		else DatosScummvm["fullscreen"] = "false";
	// aspect_ratio
		if( ui.chkSvm_aspect_ratio->isChecked() )
			DatosScummvm["aspect_ratio"] = "true";
		else DatosScummvm["aspect_ratio"] = "false";

		DatosScummvm["path"]			= ui.txtSvm_path->text()				; // path
		DatosScummvm["path_setup"]		= ""									; // path_setup
		DatosScummvm["path_extra"]		= ""									; // path_extra
		DatosScummvm["path_save"]		= ui.txtSvm_savepath->text()			; // path_save
		DatosScummvm["path_capturas"]	= ""									; // path_capturas
		DatosScummvm["path_sonido"]		= ""									; // path_sonido
		DatosScummvm["music_driver"]	= ui.cbxSvm_music_driver->currentText()	; // music_driver
	// enable_gs
		if( ui.chkSvm_enable_gs->isChecked() )
			DatosScummvm["enable_gs"] = "true";
		else DatosScummvm["enable_gs"] = "false";
	// multi_midi
		if( ui.chkSvm_multi_midi->isChecked() )
			DatosScummvm["multi_midi"] = "true";
		else DatosScummvm["multi_midi"] = "false";
	// native_mt32
		if( ui.chkSvm_native_mt32->isChecked() )
			DatosScummvm["native_mt32"] = "true";
		else DatosScummvm["native_mt32"] = "false";

		DatosScummvm["master_volume"]	= "255"						; // master_volume
		DatosScummvm["music_volume"]	= ui.posSliderSvm_1->text()	; // music_volume
		DatosScummvm["sfx_volume"]		= ui.posSliderSvm_2->text()	; // sfx_volume
		DatosScummvm["speech_volume"]	= ui.posSliderSvm_3->text()	; // speech_volume
		DatosScummvm["tempo"]			= ui.posSliderSvm_4->text()	; // tempo
		DatosScummvm["talkspeed"]		= ui.posSliderSvm_5->text()	; // talkspeed
		DatosScummvm["debuglevel"]		= ui.posSliderSvm_6->text()	; // debuglevel
		DatosScummvm["cdrom"]			= "0"						; // cdrom ui.cbxSvm_cdrom->currentIndex()
		DatosScummvm["joystick_num"]	= fGrl.IntToStr(ui.cbxSvm_joystick_num->currentIndex()); //joystick_num
		DatosScummvm["output_rate"]		= ""						; // output_rate
		DatosScummvm["midi_gain"]		= "100"						; // midi_gain
		DatosScummvm["copy_protection"]	= "false"					; // copy_protection
		DatosScummvm["sound_font"]		= ""						; // sound_font
		QDialog::accept();
	}
}

void frmSvmAdd::on_btnDirGame()
{
	ui.txtSvm_path->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), UltimoPath["Svm_path"] , ui.txtSvm_path->text() ) );

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_path", ui.txtSvm_path->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_path"] = ui.txtSvm_path->text()+"/";
}

void frmSvmAdd::on_btnDirSave()
{
	ui.txtSvm_savepath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), UltimoPath["Svm_savepath"] , ui.txtSvm_savepath->text() ) );

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("Svm_savepath", ui.txtSvm_savepath->text()+"/" );
	lastdir.endGroup();
	UltimoPath["Svm_savepath"] = ui.txtSvm_savepath->text()+"/";
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
