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

frmSvmAdd::frmSvmAdd(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir  = QDir::homePath()+"/.gr-lida/";		// directorio de trabajo del GR-lida
//	stHomeDir  = QDir::currentPath()+"/";	// directorio de trabajo del GR-lida
//	stHomeDir  = "./";								// directorio de trabajo del GR-lida
	stDatosDir = stHomeDir + "datos/";

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

	ui.twScummVM->header()->setStretchLastSection(true); 
	ui.twScummVM->header()->setMovable(false);
	ui.twScummVM->header()->resizeSection(0, 340); 
	ui.twScummVM->header()->resizeSection(1, 80); 
	ui.twScummVM->clear();

	fGrl.CargarDatosListaSvm(":/datos/svm_lista.txt"		, ui.twScummVM ); 			// Carga la lista de compatibilidad del ScummVM.
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"		, ui.cbxSvm_1, 2, true );	// Carga la lista de idiomas
	fGrl.CargarDatosComboBox(":/datos/svm_platform.txt"		, ui.cbxSvm_2, 2, false);	// Carga la lista de platform
	fGrl.CargarDatosComboBox(":/datos/svm_gfxmode.txt"		, ui.cbxSvm_3, 3, false);	// Carga la lista de gfxmode
	fGrl.CargarDatosComboBox(":/datos/svm_render_mode.txt"	, ui.cbxSvm_4, 1, false);	// Carga la lista de render_mode
	fGrl.CargarDatosComboBox(":/datos/svm_music_driver.txt"	, ui.cbxSvm_5, 1, false);	// Carga la lista de music_driver

	ui.cbxSvm_1->setCurrentIndex(0);
	ui.cbxSvm_2->setCurrentIndex(0);
	ui.cbxSvm_3->setCurrentIndex(0);
	ui.cbxSvm_4->setCurrentIndex(0);
	ui.cbxSvm_5->setCurrentIndex(0);

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmSvmAdd::~frmSvmAdd(){}

void frmSvmAdd::on_twScummVM_currentItemChanged(QTreeWidgetItem *item1,QTreeWidgetItem *item2){
	if( (item1)&&(item2) )
		emit on_twScummVM_clicked(  ui.twScummVM->currentItem() );
	else return;
}

void frmSvmAdd::on_twScummVM_clicked( QTreeWidgetItem *item )
{
	if( item && item->text(1)!=""){
		ui.txtSvm_1->setText( item->text(0) );
		ui.txtSvm_2->setText( item->text(1) );
	}else
		return;
}

void frmSvmAdd::on_twScummVM_Dblclicked( QTreeWidgetItem *item )
{
	if( item && item->text(1)!=""){
		ui.txtSvm_1->setText( item->text(0) );
		ui.txtSvm_2->setText( item->text(1) );
		ui.btnNext->click();
	}else
		return;
}

void frmSvmAdd::on_btnPrevious()
{
	ui.wizardSvm->setCurrentIndex(ui.wizardSvm->currentIndex()-1);
	ui.btnNext->setEnabled( true );
	if (ui.wizardSvm->currentIndex() < 1 )
		ui.btnPrevious->setEnabled(false) ;
}

void frmSvmAdd::on_btnNext()
{
	bool siguiente;
	if ((ui.twScummVM->isItemSelected(ui.twScummVM->currentItem())) &&
		(ui.twScummVM->currentItem()->text(1)!="")){
		siguiente = true;
		if (ui.wizardSvm->currentIndex() == 1) {
			if ((ui.txtSvm_1->text() != "") && (ui.txtSvm_2->text() != "")) {
				if ( ui.txtSvm_3->text() == ""){
					QMessageBox::information(this, stTituloSvm(), tr("Debes indicar el Directorio del juego"));
					siguiente = false;
				}else{ siguiente = true;}
			}else{
				QMessageBox::information(this, stTituloSvm(), tr("Debes poner un Titulo al juego"));
				siguiente = false;
			}
		}
		if (siguiente == true ){
			ui.btnPrevious->setEnabled(true);
			ui.wizardSvm->setCurrentIndex(ui.wizardSvm->currentIndex()+1);
		}
	    if(ui.wizardSvm->currentIndex() == 2) ui.cbxSvm_4->setFocus();
	    if(ui.wizardSvm->currentIndex() == 3) ui.cbxSvm_5->setFocus();
		if( ui.wizardSvm->currentIndex() >= 4) {
			ui.btnOk->setEnabled(true);
		  	ui.btnOk->setFocus();
			ui.btnNext->setEnabled(false);
		}
	}else {
		QMessageBox::information(this, stTituloSvm(), tr("Selecciona un juego de la lista"));
	}
}

void frmSvmAdd::on_btnOk()
{
	if ( ui.txtSvm_1->text().isEmpty() ) {
		QMessageBox::information( this, stTituloSvm(), tr("Debes poner por lo menos el titulo."));
	} else {
		DatosJuego.clear();
		DatosJuego["icono"]			= "scummvm"		;//icono
		DatosJuego["titulo"]		= ui.txtSvm_1->text();//titulo
		DatosJuego["subtitulo"] 	= ""			;//subtitulo
		DatosJuego["genero"] 		= ""			;//genero
		DatosJuego["compania"] 		= ""			;//compania
		DatosJuego["desarrollador"] = ""			;//desarrollador
		DatosJuego["tema"] 			= ""			;//tema
		DatosJuego["idioma"] 		= ""			;//idioma
		DatosJuego["formato"] 		= ""			;//formato
		DatosJuego["anno"] 			= ""			;//anno
		DatosJuego["numdisc"] 		= ""			;//numdisc
		DatosJuego["sistemaop"] 	= ""			;//sistemaop
		DatosJuego["tamano"] 		= ""			;//tamano
		DatosJuego["graficos"] 		= "1"			;//graficos
		DatosJuego["sonido"] 		= "1"			;//sonido
		DatosJuego["jugabilidad"] 	= "1"			;//jugabilidad
		DatosJuego["original"] 		= "false"		;//original
		DatosJuego["estado"] 		= ""			;//estado
		DatosJuego["thumbs"] 		= ""			;//thumbs
		DatosJuego["cover_front"] 	= ""			;//cover_front
		DatosJuego["cover_back"] 	= ""			;//cover_back
		DatosJuego["fecha"] 		= fGrl.HoraFechaActual();//fecha d/m/a h:m:s
		DatosJuego["tipo_emu"] 		= "scummvm"		;//tipo_emu
		DatosJuego["comentario"] 	= ""			;//comentario

		QString tmp_language, tmp_platform, tmp_gfx_mode;
		QStringList tmp_list;
		
		tmp_list.clear();
		tmp_list     = ui.cbxSvm_1->currentText().split(" - ");
		tmp_language = tmp_list[1];
		
		tmp_list.clear();
		tmp_list     = ui.cbxSvm_2->currentText().split(" - ");
		tmp_platform = tmp_list[1];

		tmp_list.clear();
		tmp_list     = ui.cbxSvm_3->currentText().split(" - ");
		tmp_gfx_mode = tmp_list[1];

		if ( tmp_language == "" ) tmp_language = "" ;
		if ( tmp_platform == "" ) tmp_platform = "" ;	
		if ( tmp_gfx_mode == "" ) tmp_gfx_mode = "" ;	

		DatosScummvm.clear();
		DatosScummvm["game"] = ui.txtSvm_2->text()		; //game
		DatosScummvm["language"] = tmp_language			; //language
		if( ui.chkSvm_1->isChecked() )
			DatosScummvm["subtitles"] = "true"			; //
		else DatosScummvm["subtitles"] = "false"		; //subtitles
		DatosScummvm["platform"] = tmp_platform			; //platform
		DatosScummvm["gfx_mode"] = tmp_gfx_mode			; //gfx_mode
		if ( ui.cbxSvm_4->currentText()!="" )
			DatosScummvm["render_mode"] = ui.cbxSvm_4->currentText(); 
		else DatosScummvm["render_mode"] = ""			; //render_mode
		if( ui.chkSvm_2->isChecked() )
			DatosScummvm["fullscreen"] = "true"			; //
		else DatosScummvm["fullscreen"] = "false"		; //fullscreen
		if( ui.chkSvm_3->isChecked() )
			DatosScummvm["aspect_ratio"] = "true"		; //
		else DatosScummvm["aspect_ratio"] = "false"		; //aspect_ratio
		DatosScummvm["path"] =	ui.txtSvm_3->text()		; //path
		DatosScummvm["path_setup"] = ""					; //path_setup
		DatosScummvm["path_extra"] = ""					; //path_extra
		DatosScummvm["path_save"] = ui.txtSvm_4->text()	; //path_save
		DatosScummvm["path_capturas"] = ""				; //path_capturas
		DatosScummvm["path_sonido"] = ""				; //path_sonido
		DatosScummvm["music_driver"] = ui.cbxSvm_5->currentText(); //music_driver
		if( ui.chkSvm_6->isChecked() )
			DatosScummvm["enable_gs"] = "true"			; //
		else DatosScummvm["enable_gs"] = "false"		; // enable_gs
		if( ui.chkSvm_4->isChecked() )
			DatosScummvm["multi_midi"] = "true"			; //
		else DatosScummvm["multi_midi"] = "false"		; //multi_midi
		if( ui.chkSvm_5->isChecked() )
			DatosScummvm["native_mt32"] = "true"		; //
		else DatosScummvm["native_mt32"] = "false"		; //native_mt32
		DatosScummvm["master_volume"] = "255"						; //master_volume
		DatosScummvm["music_volume"]  = ui.posSliderSvm_1->text() 	; //music_volume
		DatosScummvm["sfx_volume"]    = ui.posSliderSvm_2->text()	; //sfx_volume
		DatosScummvm["speech_volume"] = ui.posSliderSvm_3->text()	; //speech_volume
		DatosScummvm["tempo"] 		  = ui.posSliderSvm_4->text()	; //tempo
		DatosScummvm["talkspeed"] 	  = ui.posSliderSvm_5->text()	; //talkspeed
		DatosScummvm["debuglevel"]    = ui.posSliderSvm_6->text()	; //debuglevel
		DatosScummvm["cdrom"] 		  = "0"	; //cdrom ui.cbxSvm_6->currentIndex()
		DatosScummvm["joystick_num"]  = fGrl.IntToStr(ui.cbxSvm_7->currentIndex()) ;//joystick_num 

		QDialog::accept();
	}
}

void frmSvmAdd::on_btnDirGame()
{
	ui.txtSvm_3->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), stHomeDir , ui.txtSvm_3->text() ) ); 
}

void frmSvmAdd::on_btnDirSave()
{
	ui.txtSvm_4->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio del Juego"), stHomeDir , ui.txtSvm_4->text() ) ); 
}

void frmSvmAdd::on_btnDefecto()
{
	ui.h_SliderSvm_1->setValue(192);
	ui.h_SliderSvm_2->setValue(192);
	ui.h_SliderSvm_3->setValue(192);
	ui.h_SliderSvm_4->setValue(100);
	ui.h_SliderSvm_5->setValue(60);
	ui.h_SliderSvm_6->setValue(0);
}
