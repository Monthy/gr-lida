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

#include "grlida_addedit_juego.h"
#include "grlida_addedit_montajes.h"
#include "grlida_addedit_url.h"
#include "grlida_img_viewer.h"

frmAddEditJuego::frmAddEditJuego(bool EditJuego, QString TipoEmu, QString stIDIndex, QDialog *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
	ui.setupUi(this);

	EditandoJuego = EditJuego;
	TipoEmulador  = TipoEmu;
	stItemIDGrl   = stIDIndex;

	stHomeDir     = QDir::homePath()+"/.gr-lida/";		// directorio de trabajo del GR-lida
//	stHomeDir     = QDir::currentPath()+"/";			// directorio de trabajo del GR-lida
//	stHomeDir     = "./";								// directorio de trabajo del GR-lida
	stIconDir     = stHomeDir + "iconos/";				// directorio de iconos para el GR-lida
	stDatosDir    = stHomeDir + "datos/";				// directorio para los distintos datos del GR-lida

// Conecta los distintos botones con las funciones.
	connect( ui.btnOk		, SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.cbxDatos_15	, SIGNAL( activated(const QString &) ), this, SLOT( on_cbxDatos_15_txtChanged(const QString &) ) );
	
	ui.cbxDatos_15->setCurrentIndex( ui.cbxDatos_15->findText( TipoEmulador ) );

	ui.tabWidget_Datos->setTabEnabled (3, false ); // tab DOSBox
	ui.tabWidget_Datos->setTabEnabled (4, false ); // tab ScummVM
	ui.tabWidget_Datos->setTabEnabled (5, false ); // tab vdmsound
	
	ui.twMontajes->header()->setStretchLastSection(true); 
	ui.twMontajes->header()->setMovable(false);

	ui.twDatosFiles->header()->setStretchLastSection(true); 
	ui.twDatosFiles->header()->setMovable(false);
	ui.twDatosFiles->header()->setResizeMode(QHeaderView::Interactive);
	ui.twDatosFiles->header()->resizeSection(0, 100 );//nombre
	ui.twDatosFiles->header()->resizeSection(1, 70  );//crc32
	ui.twDatosFiles->header()->resizeSection(2, 140 );//descripcion
	ui.twDatosFiles->header()->resizeSection(3, 70  );//size

	ui.twDatosURL->header()->setStretchLastSection(true); 
	ui.twDatosURL->header()->setMovable(false);
	ui.twDatosURL->header()->setResizeMode(QHeaderView::Interactive);
	ui.twDatosURL->header()->resizeSection(0, 100 );

// Referente al DatosJuego ---------------------------------------
	connect( ui.btnImgAbrir_1    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirImg_1()	) );
	connect( ui.btnImgAbrir_2    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirImg_2()	) );
	connect( ui.btnImgAbrir_3    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirImg_3()	) );
	connect( ui.btnImgVer_1      , SIGNAL( clicked() ), this, SLOT( on_btnVerImg_1()	) );
	connect( ui.btnImgVer_2      , SIGNAL( clicked() ), this, SLOT( on_btnVerImg_2()	) );
	connect( ui.btnImgVer_3      , SIGNAL( clicked() ), this, SLOT( on_btnVerImg_3()	) );
	connect( ui.btnImgEliminar_1 , SIGNAL( clicked() ), this, SLOT( on_btnElimnarImg_1()) );
	connect( ui.btnImgEliminar_2 , SIGNAL( clicked() ), this, SLOT( on_btnElimnarImg_2()) );
	connect( ui.btnImgEliminar_3 , SIGNAL( clicked() ), this, SLOT( on_btnElimnarImg_3()) );	
	connect( ui.twDatoSmile      , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twDatoSmile_Dblclicked(QTreeWidgetItem*) )); 

	// Conecta los botones para editar los comentarios de los juegos
	connect( ui.btnTool_1  , SIGNAL( clicked() ), this, SLOT( on_btnTool_1()  ) );
	connect( ui.btnTool_2  , SIGNAL( clicked() ), this, SLOT( on_btnTool_2()  ) );
	connect( ui.btnTool_3  , SIGNAL( clicked() ), this, SLOT( on_btnTool_3()  ) );
	connect( ui.btnTool_4  , SIGNAL( clicked() ), this, SLOT( on_btnTool_4()  ) );
	connect( ui.btnTool_5  , SIGNAL( clicked() ), this, SLOT( on_btnTool_5()  ) );
	connect( ui.btnTool_6  , SIGNAL( clicked() ), this, SLOT( on_btnTool_6()  ) );
	connect( ui.btnTool_7  , SIGNAL( clicked() ), this, SLOT( on_btnTool_7()  ) );
	connect( ui.btnTool_8  , SIGNAL( clicked() ), this, SLOT( on_btnTool_8()  ) );
	connect( ui.btnTool_9  , SIGNAL( clicked() ), this, SLOT( on_btnTool_9()  ) );
	connect( ui.btnTool_10 , SIGNAL( clicked() ), this, SLOT( on_btnTool_10() ) );
	connect( ui.btnTool_11 , SIGNAL( clicked() ), this, SLOT( on_btnTool_11() ) );
	connect( ui.btnTool_12 , SIGNAL( clicked() ), this, SLOT( on_btnTool_12() ) );
	connect( ui.btnTool_13 , SIGNAL( clicked() ), this, SLOT( on_btnTool_13() ) );
	connect( ui.btnTool_14 , SIGNAL( clicked() ), this, SLOT( on_btnTool_14() ) );
	connect( ui.btnTool_15 , SIGNAL( clicked() ), this, SLOT( on_btnTool_15() ) );
	connect( ui.btnTool_16 , SIGNAL( clicked() ), this, SLOT( on_btnTool_16() ) );
	
	connect( ui.btnNuevaUrl    , SIGNAL( clicked() ), this, SLOT( on_btnNuevaUrl()    ) );
	connect( ui.btnEditarUrl   , SIGNAL( clicked() ), this, SLOT( on_btnEditarUrl()   ) );
	connect( ui.btnEliminarUrl , SIGNAL( clicked() ), this, SLOT( on_btnEliminarUrl() ) );
	connect( ui.btnAbrirUrl    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirUrl()    ) );

	connect( ui.btnDatosFiles  , SIGNAL( clicked() ), this, SLOT( on_btnDatosFiles()  ) );
	connect( ui.btnAddFile     , SIGNAL( clicked() ), this, SLOT( on_btnAddFile()     ) );
	connect( ui.btnEditFile    , SIGNAL( clicked() ), this, SLOT( on_btnEditFile()    ) );
	connect( ui.btnUpdateFile  , SIGNAL( clicked() ), this, SLOT( on_btnUpdateFile()  ) );
	connect( ui.btnDeleteFile  , SIGNAL( clicked() ), this, SLOT( on_btnDeleteFile()  ) );
	//----
	filters.clear();
	filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas
	fGrl.CargarIconosComboBox(stIconDir, ui.cbxDatos_9, filters);

	fGrl.CargarDatosComboBox(stDatosDir + "generos.txt"  , ui.cbxDatos_1 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_2 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_3 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "tema.txt"     , ui.cbxDatos_4 , 1, false);
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"    , ui.cbxDatos_5 , 1, true );
	fGrl.CargarDatosComboBox(stDatosDir + "formatos.txt" , ui.cbxDatos_6 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "fechas.txt"   , ui.cbxDatos_7 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "numdisc.txt"  , ui.cbxDatos_8 , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "sistemaop.txt", ui.cbxDatos_10, 1, false);

	for(int n=1; n<11; n++)
		ui.cbxDatos_11->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));
	for(int n=1; n<11; n++)
		ui.cbxDatos_12->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));
	for(int n=1; n<11; n++)
		ui.cbxDatos_13->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));
	
	ui.html_preview->setVisible(false);

	if(EditandoJuego == true)
		CargarDatosJuego( stItemIDGrl );
	else {
		ui.lb_fechahora->setText( fGrl.HoraFechaActual() );
		stThumbs=""		;
		stCoverFront=""	;
		stCoverBack=""	;
		ui.lbImage_1->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.lbImage_2->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.lbImage_3->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_1->setEnabled( false );
		ui.btnImgEliminar_1->setEnabled( false );
		ui.btnImgVer_2->setEnabled( false );
		ui.btnImgEliminar_2->setEnabled( false );
		ui.btnImgVer_3->setEnabled( false );
		ui.btnImgEliminar_3->setEnabled( false );
		ui.cbxDatos_1->setCurrentIndex(0) ;
		ui.cbxDatos_2->setCurrentIndex(0) ;
		ui.cbxDatos_3->setCurrentIndex(0) ;
		ui.cbxDatos_4->setCurrentIndex(0) ;
		ui.cbxDatos_5->setCurrentIndex(0) ;
		ui.cbxDatos_6->setCurrentIndex(0) ;
		ui.cbxDatos_7->setCurrentIndex(0) ;
		ui.cbxDatos_8->setCurrentIndex(0) ;
		ui.cbxDatos_10->setCurrentIndex(0);
		ui.cbxDatos_11->setCurrentIndex(0);
		ui.cbxDatos_12->setCurrentIndex(0);
		ui.cbxDatos_13->setCurrentIndex(0);
		ui.cbxDatos_14->setCurrentIndex(0);
	}
	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles( stDatosDir + "smiles.txt", ui.twDatoSmile);

// Fin del DatosJuego --------------------------------------------
// Referente al ScummVM ------------------------------------------
	connect( ui.twScummVM     , SIGNAL( itemClicked( QTreeWidgetItem*, int )), this, SLOT( on_twScummVM_clicked( QTreeWidgetItem* )));
	connect( ui.btnDefectoSvm , SIGNAL( clicked() ), this, SLOT( on_btnDefecto() ) );
	connect( ui.btnDirSvm_1   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_path() ) );
	connect( ui.btnDirSvm_2   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_save() ) );
	connect( ui.btnDirSvm_3   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_extra() ) );
	connect( ui.btnDirSvm_4   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_capturas() ) );
	connect( ui.btnDirSvm_5   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_sonido() ) );
	connect( ui.btnDirSvm_6   , SIGNAL( clicked() ), this, SLOT( on_btnDirSvm_sound_font() ) );

	ui.twScummVM->header()->setStretchLastSection(true); 
	ui.twScummVM->header()->setMovable(false);
	ui.twScummVM->header()->resizeSection(0, 440); 
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
	ui.cbxSvm_8->setCurrentIndex(0);
	
	if( (EditandoJuego == true) && (TipoEmulador == "scummvm") )
	{
		ui.tabWidget_Datos->setTabEnabled (4, true ); // tab scummvm
		stItemIDSvm = sql->ItemIDIndex("dbgrl_emu_scummvm", stItemIDGrl); // Obtenemos el Index del ScummVM
		CargarDatosScummVM( stItemIDGrl );  // Carga los Datos del ScummVM.
	}

// Fin del ScummVM -----------------------------------------------
// Referente a DOSBox --------------------------------------------
	connect( ui.btnFileConfg      , SIGNAL( clicked() ), this, SLOT( on_btnFileConfg()      ) );
	connect( ui.btnExeJuego       , SIGNAL( clicked() ), this, SLOT( on_btnExeJuego()       ) );
	connect( ui.btnExeSetup       , SIGNAL( clicked() ), this, SLOT( on_btnExeSetup()       ) );
	connect( ui.btnDirGravisUltraSound, SIGNAL( clicked() ), this, SLOT( on_btnDirGravisUltraSound() ) );
	connect( ui.btnDbx_mapperfile , SIGNAL( clicked() ), this, SLOT( on_btnDbx_mapperfile() ) );
	connect( ui.btnDbx_language   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_language()   ) );
	connect( ui.btnDbx_capturas   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_capturas()   ) );
	connect( ui.btnDbx_musica     , SIGNAL( clicked() ), this, SLOT( on_btnDbx_musica()     ) );
	connect( ui.btnAddSerial      , SIGNAL( clicked() ), this, SLOT( on_btnAddSerial()       ) );
	connect( ui.btnMount_Add      , SIGNAL( clicked() ), this, SLOT( on_btnMount_Add()      ) );
	connect( ui.btnMount_Edit     , SIGNAL( clicked() ), this, SLOT( on_btnMount_Edit()     ) );
	connect( ui.btnMount_Delete   , SIGNAL( clicked() ), this, SLOT( on_btnMount_Delete()   ) );
	connect( ui.btnMount_Clear    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Clear()    ) );
	connect( ui.btnMount_Subir    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Subir()    ) );
	connect( ui.btnMount_Bajar    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Bajar()    ) );
	connect( ui.btnMount_AutoCrear, SIGNAL( clicked() ), this, SLOT( on_btnMount_AutoCrear()) );
	connect( ui.btnMount_Primario , SIGNAL( clicked() ), this, SLOT( on_btnMount_Primario() ) );

	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt" , ui.cbxDbx_1 , 1, false); // Resolucin pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt" , ui.cbxDbx_2 , 1, false); // windowresolution
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"	 , ui.cbxDbx_3 , 1, false); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"	 , ui.cbxDbx_4 , 1, false); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"	 , ui.cbxDbx_5 , 1, false); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_hwscale.txt"	 , ui.cbxDbx_6 , 1, false); // Escalar por hardware
	fGrl.CargarDatosComboBox(":/datos/dbx_sensitivity.txt", ui.cbxDbx_7 , 1, false); // Sensibilidad del ratn
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt" 	 , ui.cbxDbx_8 , 1, false); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_core.txt"	 , ui.cbxDbx_9 , 1, false);	// Ncleo de la CPU DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_frameskip.txt"	 , ui.cbxDbx_10, 1, false);	// Frameskip DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"	 , ui.cbxDbx_12, 1, false);	// Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_joystick.txt"	 , ui.cbxDbx_13, 1, false);	// Emulador de joystick
	fGrl.CargarDatosComboBox(":/datos/dbx_keyboardlayout.txt", ui.cbxDbx_42,1, false); // keyboardlayout
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt" 	 , ui.cbxDbx_15, 1, false); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_sb_oplmode.txt" , ui.cbxDbx_20, 1, false); // Sound Blaste Opl mode
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt" 	 , ui.cbxDbx_28, 1, false); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt", ui.cbxDbx_29, 1, false); // MIDI Device

	// Ponemos los Combobox por defecto.
	ui.cbxDbx_1->setCurrentIndex( 0 );	// Resolucin pantalla
	ui.cbxDbx_2->setCurrentIndex( 0 );	// windowresolution
	ui.cbxDbx_3->setCurrentIndex( 0 );	// Modo de Renderizado
	ui.cbxDbx_4->setCurrentIndex( 0 );	// Tarjeta de Video
	ui.cbxDbx_5->setCurrentIndex( 1 );	// Escalar y Filtros
	ui.cbxDbx_6->setCurrentIndex( 9 );	// Escalar por hardware
	ui.cbxDbx_7->setCurrentIndex( 9 );	// Sensibilidad del ratn
	ui.cbxDbx_8->setCurrentIndex( 5 );	// Cantidad de memoria para DOSBox
	ui.cbxDbx_9->setCurrentIndex( 0 );	// Ncleo de la CPU DOSBox
	ui.cbxDbx_10->setCurrentIndex( 0 );	// Frameskip DOSBox
	ui.cbxDbx_12->setCurrentIndex( 0 );	// Ciclos DOSBox
	ui.cbxDbx_13->setCurrentIndex( 0 );	// Emulador de joystick
	ui.cbxDbx_42->setCurrentIndex( 14 ); // keyboardlayout
	ui.cbxDbx_15->setCurrentIndex( 5 ); // Tipo Sound Blaste
	ui.cbxDbx_20->setCurrentIndex( 0 ); // Sound Blaste Opl mode
	ui.cbxDbx_28->setCurrentIndex( 0 ); // MPU-401
	ui.cbxDbx_29->setCurrentIndex( 0 ); // MIDI Device

	if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
	{
		ui.tabWidget_Datos->setTabEnabled (3, true ); // tab DOSBox
		stItemIDDbx = sql->ItemIDIndex("dbgrl_emu_dosbox", stItemIDGrl); // Obtenemos el Index del ScummVM
		CargarDatosDosBox( stItemIDGrl );
		CargarDatosDBxMontaje( stItemIDDbx );
	}

// Fin del DOSBox ------------------------------------------------
// Referente a VDMSound ------------------------------------------
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono	 , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );

	if( (EditandoJuego == true) && (TipoEmulador == "vdmsound") )
	{
		ui.tabWidget_Datos->setTabEnabled (5, true ); // tab VDMSound
		stItemIDVdms = sql->ItemIDIndex("dbgrl_emu_vdmsound", stItemIDGrl); // Obtenemos el Index del ScummVM
		CargarDatosVDMSound( stItemIDGrl );
	}
// Fin del VDMSound ----------------------------------------------

	if ( EditandoJuego == true )
	{
		ui.cbxDatos_15->setEnabled( false );
		setWindowTitle( tr("Editando a") + ": " + ui.txtDatos_1->text() );
	} else {
		ui.cbxDatos_15->setEnabled( true );
		setWindowTitle( tr("Añadiendo un nuevo juego") );
	}

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAddEditJuego::~frmAddEditJuego(){}

void frmAddEditJuego::on_btnOk()
{
	bool siguiente;
 
	//DOSBox minimo archivo conf, ejecutable
	if(ui.txtDatos_1->text()!="")
	{
		siguiente=true;
		if(ui.cbxDatos_15->currentText()=="dosbox")
		{
			if ( ui.txtDbx_1->text() != "" )
			{
				siguiente = true;
				if ( ui.txtDbx_3->text() == "")
				{
					siguiente = false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " DOSBox");
				} else {
					siguiente = true;				
					if(EditandoJuego != true)
					{
						QFile appConfg( stHomeDir + "confdbx/"+ ui.txtDbx_1->text() );
						if ( appConfg.exists() )
						{
							siguiente = false;
							ui.txtDbx_1->setText("");
							QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el DOSBox ya esixte"));
						} else
							siguiente = true;
					}
				}
			} else {
				siguiente = false;
				QMessageBox::information( this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " DOSBox");
			}
		}
		if(ui.cbxDatos_15->currentText()=="scummvm")
		{//IF de scummvm
			if(ui.txtSvm_1->text()=="")
			{
				siguiente=false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner la etiqueta del juego de")+ " ScummVM" );
			} else {
				siguiente=true;
				if(ui.txtSvm_2->text()=="")
				{
					siguiente=false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Directorio del juego de")+ " ScummVM" );
				}
				/*
				 else{
					siguiente=true;
					QDir tmpdirsvm(ui.txtSvm_2->text());
					if (!tmpdirsvm.exists()){
						siguiente=false;
						QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar un Directorio que exista"));
					}else siguiente=true;
				}*/
			}
		}//fin IF de scummvm
		if(ui.cbxDatos_15->currentText()=="vdmsound")
		{
			if(ui.txtVdms_1->text()!="")
			{
				siguiente = true;
				if(ui.txtVdms_2->text()=="")
				{
					siguiente=false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " VDMSound");
				} else {
					siguiente = true;				
					if(EditandoJuego != true)
					{
						QFile appConfg( stHomeDir + "confdbx/"+ ui.txtDbx_1->text() );
						if ( appConfg.exists() )
						{
							siguiente = false;
							ui.txtDbx_1->setText("");
							QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
						} else
							siguiente = true;
					}
				}
			} else {				
				siguiente=false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " VDMSound");
			}
		}
	} else {
		siguiente=false;
		QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner un Titulo al juego"));
	}
	if(siguiente==true)
	{
		setDatosJuegos();
		if(ui.cbxDatos_15->currentText()=="dosbox")
			setDatosDosBox();

		if(ui.cbxDatos_15->currentText()=="scummvm")
			setDatosScummVM();

		if(ui.cbxDatos_15->currentText()=="vdmsound")
			setDatosVDMSound();

		QDialog::accept();
	}
}

// Referente al DatosJuego ---------------------------------------
void frmAddEditJuego::CargarDatosJuego( QString stIDIndex )
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl WHERE idgrl="+stIDIndex+" LIMIT 0,1");
	if ( query.first() )
	{
		rec = query.record();
		ui.lb_fechahora->setText( query.value( rec.indexOf("fecha") ).toString())				; // fecha
		ui.txtDatos_1->setText( query.value( rec.indexOf("titulo") ).toString()   )				; // titulo 
		ui.txtDatos_2->setText( query.value( rec.indexOf("subtitulo") ).toString()   )			; // subtitulo 
		ui.txtDatos_3->setText( query.value( rec.indexOf("tamano") ).toString()  )				; // tamano 
		ui.txtDatos_4->setPlainText( query.value( rec.indexOf("comentario") ).toString()  )		; // comentario 
	
		ui.cbxDatos_1->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("genero") ).toString()		); // genero
		ui.cbxDatos_2->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("compania") ).toString()	); // compania
		ui.cbxDatos_3->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("desarrollador") ).toString()); // desarrollador
		ui.cbxDatos_4->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("tema") ).toString()		); // tema
		ui.cbxDatos_5->addItem( QIcon(":/img16/sinimg.png")  , query.value( rec.indexOf("idioma") ).toString()		); // idioma
		ui.cbxDatos_6->addItem( QIcon(":/img16/sinimg.png")  , query.value( rec.indexOf("formato") ).toString()		); // formato
		ui.cbxDatos_7->addItem( QIcon(":/img16/fecha.png")   , query.value( rec.indexOf("anno") ).toString()		); // anno
		ui.cbxDatos_8->addItem( QIcon(":/img16/sinimg.png")  , query.value( rec.indexOf("numdisc") ).toString()		); // numdisc
		ui.cbxDatos_10->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("sistemaop") ).toString()	); // sistemaop
		//ui.cbxDatos_11->addItem( QIcon(":/img16/datos_3.png"), query.value( rec.indexOf("graficos") ).toString()	); // graficos
		//ui.cbxDatos_12->addItem( QIcon(":/img16/datos_3.png"), query.value( rec.indexOf("sonido") ).toString()		); // sonido
		//ui.cbxDatos_13->addItem( QIcon(":/img16/datos_3.png"), query.value( rec.indexOf("jugabilidad") ).toString()	); // jugabilidad
		ui.cbxDatos_14->addItem( QIcon(":/img16/sinimg.png"), query.value( rec.indexOf("estado") ).toString()		); // estado
		
// Selecciona el dato correspondiente.	
		ui.cbxDatos_1->setCurrentIndex(ui.cbxDatos_1->findText(  query.value( rec.indexOf("genero") ).toString()  ))		; // genero
		ui.cbxDatos_2->setCurrentIndex(ui.cbxDatos_2->findText(  query.value( rec.indexOf("compania") ).toString()  ))		; // compania
		ui.cbxDatos_3->setCurrentIndex(ui.cbxDatos_3->findText(  query.value( rec.indexOf("desarrollador") ).toString()  ))	; // desarrollador
		ui.cbxDatos_4->setCurrentIndex(ui.cbxDatos_4->findText(  query.value( rec.indexOf("tema") ).toString()  ))			; // tema
		ui.cbxDatos_5->setCurrentIndex(ui.cbxDatos_5->findText(  query.value( rec.indexOf("idioma") ).toString()  ))		; // idioma
		ui.cbxDatos_6->setCurrentIndex(ui.cbxDatos_6->findText(  query.value( rec.indexOf("formato") ).toString()  ))		; // formato
		ui.cbxDatos_7->setCurrentIndex(ui.cbxDatos_7->findText(  query.value( rec.indexOf("anno") ).toString() ))			; // anno
		ui.cbxDatos_8->setCurrentIndex(ui.cbxDatos_8->findText(  query.value( rec.indexOf("numdisc") ).toString() ))		; // numdisc
		ui.cbxDatos_10->setCurrentIndex(ui.cbxDatos_10->findText(query.value( rec.indexOf("sistemaop") ).toString() ))		; // sistemaop
		ui.cbxDatos_11->setCurrentIndex(ui.cbxDatos_11->findText(query.value( rec.indexOf("graficos") ).toString() ))		; // graficos
		ui.cbxDatos_12->setCurrentIndex(ui.cbxDatos_12->findText(query.value( rec.indexOf("sonido") ).toString() ))			; // sonido
		ui.cbxDatos_13->setCurrentIndex(ui.cbxDatos_13->findText(query.value( rec.indexOf("jugabilidad") ).toString() ))	; // jugabilidad
		ui.cbxDatos_14->setCurrentIndex(ui.cbxDatos_14->findText(query.value( rec.indexOf("estado") ).toString() ))			; // estado
	
		ui.chkDatos_1->setChecked( query.value( rec.indexOf("original") ).toBool() )										; // original
		ui.chkDatos_2->setChecked( query.value( rec.indexOf("favorito") ).toBool() )										; // favorito

		QString stIcono;
		stIcono.clear();
		stIcono = query.value( rec.indexOf("icono") ).toString() ; // icono
		if ( stIcono == "datos" || stIcono == "" ) 
			ui.cbxDatos_9->setCurrentIndex(0) ;
		else if ( stIcono == "dosbox" )
			ui.cbxDatos_9->setCurrentIndex(1) ;
		else if ( stIcono == "scummvm" )
			ui.cbxDatos_9->setCurrentIndex(2) ;
		else if ( stIcono =="vdmsound" ) 
			ui.cbxDatos_9->setCurrentIndex(3) ;
		else 
			ui.cbxDatos_9->setCurrentIndex( ui.cbxDatos_9->findText( stIcono ) ); // icono
	
		stThumbs.clear()	; // thumbs
		stCoverFront.clear(); // cover_front
		stCoverBack.clear()	; // cover_back
		stThumbs     = query.value( rec.indexOf("thumbs")     ).toString()	; // thumbs
		stCoverFront = query.value( rec.indexOf("cover_front")).toString()	; // cover_front
		stCoverBack  = query.value( rec.indexOf("cover_back") ).toString()	; // cover_back

		if( file_thumbs.exists(stThumbs) )
		{
			ui.lbImage_1->setPixmap( QPixmap(stThumbs) );
			ui.btnImgVer_1->setEnabled( true );
			ui.btnImgEliminar_1->setEnabled( true );
		} else {
			ui.lbImage_1->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_1->setEnabled( false );
			ui.btnImgEliminar_1->setEnabled( false );
		}
		if( file_cover_front.exists(stCoverFront) )
		{
			ui.lbImage_2->setPixmap( QPixmap(stCoverFront) );
			ui.btnImgVer_2->setEnabled( true );
			ui.btnImgEliminar_2->setEnabled( true );
		} else {
			ui.lbImage_2->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_2->setEnabled( false );
			ui.btnImgEliminar_2->setEnabled( false );
		}
		if( file_cover_back.exists(stCoverBack) )
		{
			ui.lbImage_3->setPixmap( QPixmap(stCoverBack) );
			ui.btnImgVer_3->setEnabled( true );
			ui.btnImgEliminar_3->setEnabled( true );
		} else {
			ui.lbImage_3->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_3->setEnabled( false );
			ui.btnImgEliminar_3->setEnabled( false );
		}
	}
	
	ui.twDatosURL->clear();
	query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+stIDIndex);
	if (query.first())
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
			item_url->setText( 0 , query.value( rec.indexOf("url") ).toString()        ) ; // url
			item_url->setIcon( 0 , QIcon(":/img16/edit_enlace.png")                    ) ; // icono
			item_url->setText( 1 , query.value( rec.indexOf("descripcion") ).toString()) ; // descripcion
			item_url->setText( 2 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    ) ; // id
			item_url->setText( 3 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) ) ; // idgrl
		} while (query.next());
		ui.twDatosURL->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}	
	
	ui.twDatosFiles->clear();
	query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+stIDIndex);
	if (query.first())
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twDatosFiles );
			item_files->setText( 0 , query.value( rec.indexOf("nombre") ).toString()     ) ; // nombre
			item_files->setIcon( 0 , QIcon(":/img16/importar.png")                       ) ; // icono
			item_files->setText( 1 , query.value( rec.indexOf("crc") ).toString()        ) ; // crc
			item_files->setText( 2 , query.value( rec.indexOf("descripcion") ).toString()) ; // descripcion
			item_files->setText( 3 , query.value( rec.indexOf("size") ).toString()       ) ; // size
			item_files->setText( 4 , query.value( rec.indexOf("path") ).toString()       ) ; // path		
			item_files->setText( 5 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    ) ; // id
			item_files->setText( 6 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) ) ; // idgrl
		} while (query.next());
		ui.twDatosFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}	
}

void frmAddEditJuego::setDatosJuegos()
{
	DatosJuego.clear();

	if ( ui.cbxDatos_9->currentText()!="" )
		DatosJuego["icono"] = ui.cbxDatos_9->currentText()		; // icono
	else DatosJuego["icono"] = "" ;

	DatosJuego["titulo"] = ui.txtDatos_1->text()				; // titulo
	DatosJuego["subtitulo"] = ui.txtDatos_2->text()				; // subtitulo
	
	if ( ui.cbxDatos_1->currentText()!="" )
		DatosJuego["genero"] = ui.cbxDatos_1->currentText()		; // genero
	else DatosJuego["genero"] = "" ;

	if ( ui.cbxDatos_2->currentText()!="" )
		DatosJuego["compania"] = ui.cbxDatos_2->currentText()	; // compania
	else DatosJuego["compania"] = "" ;

	if ( ui.cbxDatos_3->currentText()!="" )
		DatosJuego["desarrollador"] = ui.cbxDatos_3->currentText(); // desarrollador
	else DatosJuego["desarrollador"] = "" ;

	if ( ui.cbxDatos_4->currentText()!="" )
		DatosJuego["tema"] = ui.cbxDatos_4->currentText()		; // tema
	else DatosJuego["tema"] = "" ;
	
	if ( ui.cbxDatos_5->currentText()!="" )
		DatosJuego["idioma"] = ui.cbxDatos_5->currentText()		; // idioma
	else DatosJuego["idioma"] = "" ;

	if ( ui.cbxDatos_6->currentText()!="" )
		DatosJuego["formato"] = ui.cbxDatos_6->currentText()	; // formato
	else DatosJuego["formato"] = "" ;
	
	if ( ui.cbxDatos_7->currentText()!="" )
		DatosJuego["anno"] = ui.cbxDatos_7->currentText()		; // anno
	else DatosJuego["anno"] = "" ;
	
	if ( ui.cbxDatos_8->currentText()!="" )
		DatosJuego["numdisc"] = ui.cbxDatos_8->currentText()	; // numdisc
	else DatosJuego["numdisc"] = "" ;
	
	if ( ui.cbxDatos_10->currentText()!="" )
		DatosJuego["sistemaop"] = ui.cbxDatos_10->currentText()	; // sistemaop
	else DatosJuego["sistemaop"] = "" ;
	
	DatosJuego["tamano"] = ui.txtDatos_3->text()				; // tamano
	DatosJuego["graficos"] = ui.cbxDatos_11->currentText()		; // graficos
	DatosJuego["sonido"] = ui.cbxDatos_12->currentText()		; // sonido
	DatosJuego["jugabilidad"] = ui.cbxDatos_13->currentText()	; // jugabilidad

	if( ui.chkDatos_1->isChecked() ) DatosJuego["original"] = "true";
	else DatosJuego["original"] = "false"						; // original

	if( ui.chkDatos_2->isChecked() ) DatosJuego["favorito"] = "true";
	else DatosJuego["favorito"] = "false"						; // favorito	
	
	if ( ui.cbxDatos_14->currentText()!="" )
		DatosJuego["estado"] = ui.cbxDatos_14->currentText()	; // estado
	else DatosJuego["estado"] = "" ;
	DatosJuego["thumbs"] = stThumbs								; // thumbs
	DatosJuego["cover_front"] = stCoverFront					; // cover_front
	DatosJuego["cover_back"] = stCoverBack						; // cover_back
	DatosJuego["fecha"] = ui.lb_fechahora->text()				; // fecha d/m/a h:m:s
	DatosJuego["tipo_emu"] = ui.cbxDatos_15->currentText()		; // tipo_emu
	DatosJuego["comentario"] = ui.txtDatos_4->toPlainText()		; // comentario

}

void frmAddEditJuego::on_btnAbrirImg_1()
{
	stThumbs = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stThumbs, stThumbs, tr("Todos los archivo") + " (*)", 0, false) ;
	if( file_thumbs.exists(stThumbs) )
	{
		ui.lbImage_1->setPixmap( QPixmap(stThumbs) );
		ui.btnImgVer_1->setEnabled( true );
		ui.btnImgEliminar_1->setEnabled( true );
	} else {
		ui.lbImage_1->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_1->setEnabled( false );
		ui.btnImgEliminar_1->setEnabled( false );
		stThumbs = "";
	}
}

void frmAddEditJuego::on_btnAbrirImg_2()
{
	stCoverFront = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stCoverFront, stCoverFront, tr("Todos los archivo") + " (*)", 0, false);
	if( file_cover_front.exists(stCoverFront) )
	{
		ui.lbImage_2->setPixmap( QPixmap(stCoverFront) );
		ui.btnImgVer_2->setEnabled( true );
		ui.btnImgEliminar_2->setEnabled( true );
	} else {
		ui.lbImage_2->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_2->setEnabled( false );
		ui.btnImgEliminar_2->setEnabled( false );
		stCoverFront = "";
	}
}

void frmAddEditJuego::on_btnAbrirImg_3()
{
	stCoverBack = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stCoverBack, stCoverBack, tr("Todos los archivo") + " (*)", 0, false) ;
	if( file_cover_back.exists(stCoverBack) )
	{
		ui.lbImage_3->setPixmap( QPixmap(stCoverBack) );
		ui.btnImgVer_3->setEnabled( true );
		ui.btnImgEliminar_3->setEnabled( true );
	} else {
		ui.lbImage_3->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_3->setEnabled( false );
		ui.btnImgEliminar_3->setEnabled( false );
		stCoverBack = "";
	}
}

void frmAddEditJuego::on_btnVerImg_1()
{
	// Abre la imagen para verla en su tamaño original
	if( file_thumbs.exists(stThumbs) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stThumbs );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnVerImg_2()
{
	// Abre la imagen para verla en su tamaño original
	if( file_cover_front.exists(stCoverFront) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCoverFront );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnVerImg_3()
{
	// Abre la imagen para verla en su tamaño original
	if( file_cover_back.exists(stCoverBack) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCoverBack );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnElimnarImg_1()
{
	stThumbs="" ;
	ui.lbImage_1->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_1->setEnabled( false );
	ui.btnImgEliminar_1->setEnabled( false );
}

void frmAddEditJuego::on_btnElimnarImg_2()
{
	stCoverFront=""	;
	ui.lbImage_2->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_2->setEnabled( false );
	ui.btnImgEliminar_2->setEnabled( false );	
}

void frmAddEditJuego::on_btnElimnarImg_3()
{
	stCoverBack=""	;
	ui.lbImage_3->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_3->setEnabled( false );
	ui.btnImgEliminar_3->setEnabled( false );
}

void frmAddEditJuego::on_cbxDatos_15_txtChanged(const QString texto)
{
	if (texto == "datos")
		ui.cbxDatos_9->setCurrentIndex(0) ;

	if (texto == "dosbox")
	{
		ui.tabWidget_Datos->setTabEnabled (3, true )	; // tab dosbox
		ui.cbxDatos_9->setCurrentIndex(1) ;
	} else
		ui.tabWidget_Datos->setTabEnabled (3, false )	;

	if (texto == "scummvm")
	{
		ui.tabWidget_Datos->setTabEnabled (4, true )	; // tab scummvm
		ui.cbxDatos_9->setCurrentIndex(2) ;
	} else
		ui.tabWidget_Datos->setTabEnabled (4, false )	;

	if (texto == "vdmsound")
	{
		ui.tabWidget_Datos->setTabEnabled (5, true )	; // tab vdmsound
		ui.cbxDatos_9->setCurrentIndex(3) ;
	} else
		ui.tabWidget_Datos->setTabEnabled (5, false )	;
}

void frmAddEditJuego::on_twDatoSmile_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.txtDatos_4->textCursor().insertText( " " + item->text(0) + " " );
		// preview texto-html
		if( ui.btnTool_16->isChecked() )
			ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_4->toPlainText(), listSmailes ) );
	} else
		return;
}

void frmAddEditJuego::on_btnTool_1()
{
// cortar texto
	ui.txtDatos_4->cut();
}

void frmAddEditJuego::on_btnTool_2()
{
// copiar texto
	ui.txtDatos_4->copy();
}

void frmAddEditJuego::on_btnTool_3()
{
// pegar texto
	ui.txtDatos_4->paste();
}

void frmAddEditJuego::on_btnTool_4()
{
// seleccionar el texto
	ui.txtDatos_4->selectAll();
}

void frmAddEditJuego::on_btnTool_5()
{
// deshacer texto
	ui.txtDatos_4->undo();
}

void frmAddEditJuego::on_btnTool_6()
{
// rehacer texto
	ui.txtDatos_4->redo();
}

void frmAddEditJuego::on_btnTool_7()
{
// texto en negrita
	ui.txtDatos_4->textCursor().insertText("<strong>"+ui.txtDatos_4->textCursor().selectedText()+"</strong>");
}

void frmAddEditJuego::on_btnTool_8()
{
// texto en cursiva
	ui.txtDatos_4->textCursor().insertText("<em>"+ui.txtDatos_4->textCursor().selectedText()+"</em>");
}

void frmAddEditJuego::on_btnTool_9()
{
// texto subrayado
	ui.txtDatos_4->textCursor().insertText("<u>"+ui.txtDatos_4->textCursor().selectedText()+"</u>");
}

void frmAddEditJuego::on_btnTool_10()
{
// insertar una imagen en el texto
	ui.txtDatos_4->textCursor().insertText("<img src=\"direccion_imagen\" />");
}

void frmAddEditJuego::on_btnTool_11()
{
// insertar una url en el texto
	ui.txtDatos_4->textCursor().insertText("<a href=\""+ui.txtDatos_4->textCursor().selectedText()+"\">"+ui.txtDatos_4->textCursor().selectedText()+"</a>");
}

void frmAddEditJuego::on_btnTool_12()
{
// buscar texto

}

void frmAddEditJuego::on_btnTool_13()
{
// buscar texto anterior

}

void frmAddEditJuego::on_btnTool_14()
{
// buscar texto siguiente

}

void frmAddEditJuego::on_btnTool_15()
{
// reemlazar texto

}

void frmAddEditJuego::on_btnTool_16()
{
// preview texto-html
	if( ui.btnTool_16->isChecked() )
	{
		ui.txtDatos_4->setVisible(false);
		ui.html_preview->setVisible(true);
		ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_4->toPlainText(), listSmailes ) );
	} else {	
		ui.txtDatos_4->setVisible(true);
		ui.html_preview->setVisible(false);
	}
}

void frmAddEditJuego::on_btnNuevaUrl()
{
	QString id_url;
	frmAddEditURL * AddEditURL = new frmAddEditURL();
    if ( AddEditURL->exec() == QDialog::Accepted )
    {
		QHash<QString, QString> datos_url;
		datos_url.clear();
		datos_url["url"] = AddEditURL->ui.txt_addedit_url_1->currentText(); // 0
		datos_url["descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText(); // 1
		
		if(stItemIDGrl!="")
			id_url = sql->ItemInsertaUnURL( datos_url , stItemIDGrl );
		else
			id_url = "" ;
		
		QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
		item_url->setText( 0 , datos_url["url"] ) ; // url
		item_url->setIcon( 0 , QIcon(":/img16/edit_enlace.png") ) ; // icono
		item_url->setText( 1 , datos_url["descripcion"] ) ; // descripcion
		item_url->setText( 2 , id_url		) ; // id
		item_url->setText( 3 , stItemIDGrl  ) ; // idgrl
	}
}

void frmAddEditJuego::on_btnEditarUrl()
{
	if( (ui.twDatosURL->topLevelItemCount() > 0) && (ui.twDatosURL->isItemSelected(ui.twDatosURL->currentItem())) )
	{
		QString id_url;
		frmAddEditURL * AddEditURL = new frmAddEditURL();
			
		AddEditURL->ui.txt_addedit_url_1->setEditText(  ui.twDatosURL->currentItem()->text(0) ); // 0
		AddEditURL->ui.txt_addedit_url_2->setPlainText( ui.twDatosURL->currentItem()->text(1) ); // 1		
		id_url    = ui.twDatosURL->currentItem()->text(2); // id

	    if ( AddEditURL->exec() == QDialog::Accepted )
	    {
			QHash<QString, QString> datos_url;
			datos_url.clear();
			datos_url["url"] = AddEditURL->ui.txt_addedit_url_1->currentText(); // 0
			datos_url["descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText(); // 1
			
			ui.twDatosURL->currentItem()->setText( 0 , datos_url["url"] ); // url
			ui.twDatosURL->currentItem()->setIcon( 0, QIcon(":/img16/edit_enlace.png") ); // icono
			ui.twDatosURL->currentItem()->setText( 1 , datos_url["descripcion"] ); // descripcion
			ui.twDatosURL->currentItem()->setText( 2 , id_url		); // id
			ui.twDatosURL->currentItem()->setText( 3 , stItemIDGrl ); // idgrl
			
			if(id_url!="")
				sql->ItemActualizaURL( datos_url, id_url);
		}
    }
}

void frmAddEditJuego::on_btnEliminarUrl()
{
	QString id_url;
	if( ui.twDatosURL->topLevelItemCount()>0 )
	{
		int EliminarSiNO;
		id_url = ui.twDatosURL->currentItem()->text(2); // id
		EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar url...?"), tr("¿Deseas eliminar esta url?"), tr( "Si" ), tr( "No" ), 0, 1 );
		if( EliminarSiNO == 0 )
		{
			fGrl.DeleteItemTree( ui.twDatosURL->currentItem() ) ;
			if(id_url!="")
				sql->ItemEliminarURL( id_url );
		}
	}
}

void frmAddEditJuego::on_btnAbrirUrl()
{
// Abre la URL con el navegador por defecto
	QTreeWidgetItem * item = 0;
	item = ui.twDatosURL->currentItem();
	if ( item )
	{
		my_url.clear();
		my_url = ui.twDatosURL->currentItem()->text(0);
	    QDesktopServices::openUrl( my_url );
	}
}

void frmAddEditJuego::on_btnDatosFiles()
{
	ui.txtDatos_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDatos_5->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnAddFile()
{
	QString id_file;
	
	if(ui.txtDatos_5->text()!="")
	{
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["nombre"] = ui.txtDatos_6->text()			; // 0 nombre
		datos_file["crc"] = ui.txtDatos_8->text()				; // 1 crc32
		datos_file["descripcion"] = ui.txtDatos_7->toPlainText(); // 2 descripcion
		datos_file["path"] = ui.txtDatos_5->text()				; // 3 path
		datos_file["size"] = ui.txtDatos_9->text()				; // 4 size
 
		if(stItemIDGrl!="")
			id_file = sql->ItemInsertaUnFiles( datos_file , stItemIDGrl );
		else 
			id_file = "";
			
		QTreeWidgetItem *item_file = new QTreeWidgetItem( ui.twDatosFiles );
		item_file->setText( 0 , datos_file["nombre"]		) ; // nombre
		item_file->setIcon( 0 , QIcon(":/img16/archivos.png")); // icono
		item_file->setText( 1 , datos_file["crc"]			) ; // crc32
		item_file->setText( 2 , datos_file["descripcion"]	) ; // descripcion
		item_file->setText( 3 , datos_file["size"]			) ; // size
		item_file->setText( 4 , datos_file["path"]			) ; // path
		item_file->setText( 5 , id_file				) ; // id
		item_file->setText( 6 , stItemIDGrl			) ; // idgrl
	}
}

void frmAddEditJuego:: on_btnEditFile()
{
	if( (ui.twDatosFiles->topLevelItemCount() > 0) && (ui.twDatosFiles->isItemSelected(ui.twDatosFiles->currentItem())) )
	{
		ui.btnAddFile->setEnabled( false );
		ui.btnEditFile->setEnabled( false );
		ui.btnUpdateFile->setEnabled( true );
		ui.btnDeleteFile->setEnabled( false );
		ui.twDatosFiles->setEnabled( false );
		ui.txtDatos_6->setText( ui.twDatosFiles->currentItem()->text(0) ) ; // 0 nombre
		ui.txtDatos_8->setText( ui.twDatosFiles->currentItem()->text(1) ) ; // 1 crc32
		ui.txtDatos_7->setText( ui.twDatosFiles->currentItem()->text(2) ) ; // 2 descripcion
		ui.txtDatos_5->setText( ui.twDatosFiles->currentItem()->text(4) ) ; // 3 path
		ui.txtDatos_9->setText( ui.twDatosFiles->currentItem()->text(3)	) ;	// 4 size
    }
}

void frmAddEditJuego::on_btnUpdateFile()
{
	if( ui.twDatosFiles->isItemSelected( ui.twDatosFiles->currentItem() ) )
	{
		QString id_file;
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["nombre"] = ui.txtDatos_6->text()			; // 0 nombre
		datos_file["crc"] = ui.txtDatos_8->text()				; // 1 crc32
		datos_file["descripcion"] = ui.txtDatos_7->toPlainText(); // 2 descripcion
		datos_file["path"] = ui.txtDatos_5->text()				; // 3 path
		datos_file["size"] = ui.txtDatos_9->text()				; // 4 size

      	id_file = ui.twDatosFiles->currentItem()->text(5) ; // id 
 		
 		if(id_file!="")
 			sql->ItemActualizaFiles( datos_file, id_file);
	
		ui.twDatosFiles->currentItem()->setText(0, datos_file["nombre"] 	) ; // 0 nombre
		ui.twDatosFiles->currentItem()->setText(1, datos_file["crc"] 		) ; // 1 crc32
		ui.twDatosFiles->currentItem()->setText(2, datos_file["descripcion"]) ; // 2 descripcion
		ui.twDatosFiles->currentItem()->setText(3, datos_file["size"] 		) ; // 4 size
		ui.twDatosFiles->currentItem()->setText(4, datos_file["path"] 		) ; // 3 path
			
		ui.btnAddFile->setEnabled( true );
		ui.btnEditFile->setEnabled( true );
		ui.btnUpdateFile->setEnabled( false );
		ui.btnDeleteFile->setEnabled( true );
		ui.twDatosFiles->setEnabled( true );
	}
}

void frmAddEditJuego::on_btnDeleteFile()
{
	if( ui.twDatosFiles->topLevelItemCount()>0 )
	{
		QString id_file;	
		int EliminarSiNO;
		id_file = ui.twDatosFiles->currentItem()->text(5); // id
		EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar Archivo...?"), tr("¿Deseas eliminar esta Archivo?"), tr("Si"), tr("No"), 0, 1 );
		if( EliminarSiNO == 0 )
		{
			fGrl.DeleteItemTree( ui.twDatosFiles->currentItem() ) ;
			if(id_file!="")
				sql->ItemEliminarFiles( id_file );
		}
	}
}

// ---------------------------------------------------------------
// Referente al ScummVM ------------------------------------------
void frmAddEditJuego::on_btnDirSvm_path()
{
	ui.txtSvm_2->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_2->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_save()
{
	ui.txtSvm_3->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_3->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_extra()
{
	ui.txtSvm_4->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_4->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_capturas()
{
	ui.txtSvm_5->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_5->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_sonido()
{
	ui.txtSvm_6->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_6->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_sound_font()
{
	ui.txtSvm_7->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtSvm_7->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::CargarDatosScummVM( QString stIDsvm )
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+stIDsvm+" LIMIT 0,1");
	if ( query.first() )
	{
		rec = query.record();
		ui.txtSvm_1->setText( query.value( rec.indexOf("game") ).toString() )					; // game
		
		QTreeWidgetItem *item;
        QList<QTreeWidgetItem *> found = ui.twScummVM->findItems(ui.txtSvm_1->text() , Qt::MatchExactly, 1);
        foreach (item, found) ui.twScummVM->setItemSelected(item, true);
	
		ui.cbxSvm_1->setCurrentIndex( ui.cbxSvm_1->findText( " - " + query.value( rec.indexOf("language") ).toString(), Qt::MatchContains ) )	; // language
		ui.chkSvm_1->setChecked( query.value( rec.indexOf("subtitles") ).toBool() )				; // subtitles
		ui.cbxSvm_2->setCurrentIndex( ui.cbxSvm_2->findText( " - " + query.value( rec.indexOf("platform") ).toString(), Qt::MatchContains ) )	; // platform
		ui.cbxSvm_3->setCurrentIndex( ui.cbxSvm_3->findText( query.value( rec.indexOf("gfx_mode") ).toString(), Qt::MatchContains ) )		 	; // gfx_mode
		ui.cbxSvm_4->setCurrentIndex( ui.cbxSvm_4->findText( query.value( rec.indexOf("render_mode") ).toString() ) )							; // render_mode
		ui.chkSvm_2->setChecked( query.value( rec.indexOf("fullscreen") ).toBool() )			; // fullscreen
		ui.chkSvm_3->setChecked( query.value( rec.indexOf("aspect_ratio") ).toBool() )			; // aspect_ratio
		ui.txtSvm_2->setText( query.value( rec.indexOf("path") ).toString() )					; // path
		//""query.value( rec.indexOf("path_setup") ).toString()									; // path_setup
		ui.txtSvm_4->setText( query.value( rec.indexOf("path_extra") ).toString() )				; // path_extra
		ui.txtSvm_3->setText( query.value( rec.indexOf("path_save") ).toString() )				; // path_save
		ui.txtSvm_5->setText( query.value( rec.indexOf("path_capturas") ).toString() )			; // path_capturas
		ui.txtSvm_6->setText( query.value( rec.indexOf("path_sonido") ).toString() )			; // path_sonido
		ui.cbxSvm_5->setCurrentIndex( ui.cbxSvm_5->findText( query.value( rec.indexOf("music_driver") ).toString()) )	; // music_driver
		ui.chkSvm_6->setChecked( query.value( rec.indexOf("enable_gs") ).toBool() )				; // enable_gs
		ui.chkSvm_4->setChecked( query.value( rec.indexOf("multi_midi") ).toBool() )			; // multi_midi
		ui.chkSvm_5->setChecked( query.value( rec.indexOf("native_mt32") ).toBool() )			; // native_mt32
		//"255"query.value( rec.indexOf("master_volume") ).toString()							; // master_volume
		ui.h_SliderSvm_1->setSliderPosition(query.value( rec.indexOf("music_volume") ).toInt())	; // music_volume
		ui.h_SliderSvm_2->setSliderPosition(query.value( rec.indexOf("sfx_volume") ).toInt())	; // sfx_volume
		ui.h_SliderSvm_3->setSliderPosition(query.value( rec.indexOf("speech_volume") ).toInt()); // speech_volume
		ui.h_SliderSvm_4->setSliderPosition(query.value( rec.indexOf("tempo") ).toInt())		; // tempo
		ui.h_SliderSvm_5->setSliderPosition(query.value( rec.indexOf("talkspeed") ).toInt())	; // talkspeed
		ui.h_SliderSvm_6->setSliderPosition(query.value( rec.indexOf("debuglevel") ).toInt())	; // debuglevel
		//query.value( rec.indexOf("cdrom") ).toString()"0"										; // cdrom ui.cbxSvm_6->currentText()
		ui.cbxSvm_7->setCurrentIndex( query.value( rec.indexOf("joystick_num") ).toInt() )		; // joystick_num 

		if(query.value( rec.indexOf("output_rate") ).toString()!="")
			ui.cbxSvm_8->setCurrentIndex( ui.cbxSvm_8->findText( query.value( rec.indexOf("output_rate") ).toString(), Qt::MatchContains ) ); // output_rate
		else
			ui.cbxSvm_8->setCurrentIndex( 0 ); // output_rate

		ui.h_SliderSvm_7->setSliderPosition(query.value( rec.indexOf("midi_gain") ).toInt())	; // midi_gain
		ui.chkSvm_7->setChecked( query.value( rec.indexOf("copy_protection") ).toBool() )		; // copy_protection
		ui.txtSvm_7->setText( query.value( rec.indexOf("sound_font") ).toString() )				; // sound_font 
	}
}

void frmAddEditJuego::setDatosScummVM()
{
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
	
	if ( tmp_language == "" ) tmp_language ="" ;
	if ( tmp_platform == "" ) tmp_platform ="" ;	
	if ( tmp_gfx_mode == "" ) tmp_gfx_mode ="" ;	
		
	DatosScummvm.clear();
	DatosScummvm["game"] = ui.txtSvm_1->text()			; // game
	DatosScummvm["language"] = tmp_language 			; // language

	if( ui.chkSvm_1->isChecked() ) DatosScummvm["subtitles"] = "true";
	else DatosScummvm["subtitles"] = "false"			; // subtitles
	
	DatosScummvm["platform"] = tmp_platform				; // platform
	DatosScummvm["gfx_mode"] = tmp_gfx_mode				; // gfx_mode
	
	if ( ui.cbxSvm_4->currentText()!="" ) DatosScummvm["render_mode"] = ui.cbxSvm_4->currentText(); 
	else DatosScummvm["render_mode"] = ""				; // render_mode

	if( ui.chkSvm_2->isChecked() ) DatosScummvm["fullscreen"] = "true"; //
	else DatosScummvm["fullscreen"] = "false"			; // fullscreen

	if( ui.chkSvm_3->isChecked() )DatosScummvm["aspect_ratio"] = "true";
	else DatosScummvm["aspect_ratio"] = "false"					; // aspect_ratio

	DatosScummvm["path"] =	ui.txtSvm_2->text()					; // path
	DatosScummvm["path_setup"] = ""								; // path_setup
	DatosScummvm["path_extra"] = ui.txtSvm_4->text()			; // path_extra
	DatosScummvm["path_save"] = ui.txtSvm_3->text()				; // path_save
	DatosScummvm["path_capturas"] = ui.txtSvm_5->text()			; // path_capturas
	DatosScummvm["path_sonido"] = ui.txtSvm_6->text()			; // path_sonido
	DatosScummvm["music_driver"] = ui.cbxSvm_5->currentText()	; // music_driver

	if( ui.chkSvm_6->isChecked() ) DatosScummvm["enable_gs"] = "true"; 
	else DatosScummvm["enable_gs"] = "false"					; // enable_gs

	if( ui.chkSvm_4->isChecked() ) DatosScummvm["multi_midi"] = "true";
	else DatosScummvm["multi_midi"] = "false"					; // multi_midi

	if( ui.chkSvm_5->isChecked() ) DatosScummvm["native_mt32"] = "true";
	else DatosScummvm["native_mt32"] = "false"					; // native_mt32
	
	DatosScummvm["master_volume"] = "255"						; // master_volume
	DatosScummvm["music_volume"]  = ui.posSliderSvm_1->text()	; // music_volume
	DatosScummvm["sfx_volume"]    = ui.posSliderSvm_2->text()	; // sfx_volume
	DatosScummvm["speech_volume"] = ui.posSliderSvm_3->text()	; // speech_volume
	DatosScummvm["tempo"]         = ui.posSliderSvm_4->text()	; // tempo
	DatosScummvm["talkspeed"]     = ui.posSliderSvm_5->text()	; // talkspeed
	DatosScummvm["debuglevel"]    = ui.posSliderSvm_6->text()	; // debuglevel
	DatosScummvm["cdrom"]         = "0"							; // cdrom ui.cbxSvm_6->currentIndex()
	DatosScummvm["joystick_num"]  = fGrl.IntToStr(ui.cbxSvm_7->currentIndex()) ; // joystick_num 

	if ( ui.cbxSvm_8->currentText()!="" && ui.cbxSvm_8->currentIndex()!= 0 )
		DatosScummvm["output_rate"] = ui.cbxSvm_8->currentText();
	else
		DatosScummvm["output_rate"] = "";

	if( ui.chkSvm_7->isChecked() ) DatosScummvm["copy_protection"] = "true";
	else DatosScummvm["copy_protection"] = "false"					; // copy_protection

	DatosScummvm["midi_gain"]     = ui.posSliderSvm_7->text()		; // midi_gain 

	if( ui.chkSvm_7->isChecked() ) DatosScummvm["copy_protection"] = "true";
	else DatosScummvm["copy_protection"] = "false"					; // copy_protection

	DatosScummvm["sound_font"]     = ui.txtSvm_7->text()			; // sound_font 
}

void frmAddEditJuego::on_twScummVM_clicked( QTreeWidgetItem *item )
{
	ui.txtSvm_1->setText( item->text(1) );
}

void frmAddEditJuego::on_btnDefecto()
{
	ui.h_SliderSvm_1->setValue(192);
	ui.h_SliderSvm_2->setValue(192);
	ui.h_SliderSvm_3->setValue(192);
	ui.h_SliderSvm_4->setValue(100);
	ui.h_SliderSvm_5->setValue(60);
	ui.h_SliderSvm_6->setValue(0);
	ui.h_SliderSvm_7->setValue(100);
}
// ---------------------------------------------------------------
// Referente al DOSBox -------------------------------------------
void frmAddEditJuego::CargarDatosDosBox( QString stIDdbx )
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stIDdbx+" LIMIT 0,1");
	if ( query.first() )
	{
		rec = query.record();
//0 id
//1 idgrl
		ui.chkDbx_4->setChecked( query.value( rec.indexOf("sdl_fullscreen") ).toBool() )										; // sdl_fullscreen
		ui.chkDbx_22->setChecked(query.value( rec.indexOf("sdl_fulldouble") ).toBool() )										; // sdl_fulldouble
		ui.chkDbx_5->setChecked( query.value( rec.indexOf("sdl_fullfixed") ).toBool() )											; // sdl_fullfixed
		ui.cbxDbx_1->setCurrentIndex( ui.cbxDbx_1->findText( query.value( rec.indexOf("sdl_fullresolution" )).toString() ) )	; // sdl_fullresolution
		ui.cbxDbx_2->setCurrentIndex( ui.cbxDbx_2->findText( query.value( rec.indexOf("sdl_windowresolution") ).toString() ) )	; // sdl_windowresolution
		ui.cbxDbx_3->setCurrentIndex( ui.cbxDbx_3->findText( query.value( rec.indexOf("sdl_output") ).toString() ) )			; // sdl_output
		ui.cbxDbx_6->setCurrentIndex( ui.cbxDbx_6->findText( query.value( rec.indexOf("sdl_hwscale") ).toString() ) )			; // sdl_hwscale
		ui.chkDbx_6->setChecked( query.value( rec.indexOf("sdl_autolock") ).toBool() )											; // sdl_autolock
		ui.cbxDbx_7->setCurrentIndex( ui.cbxDbx_7->findText( query.value( rec.indexOf("sdl_sensitivity") ).toString() ) )		; // sdl_sensitivity
		ui.chkDbx_8->setChecked( query.value(rec.indexOf("sdl_waitonerror")).toBool() )											; // sdl_waitonerror
	QString sdl_priority_temp;
	QStringList sdl_priority_templist;
		sdl_priority_temp     = query.value( rec.indexOf("sdl_priority") ).toString();
		sdl_priority_templist = sdl_priority_temp.split(",");
		if( sdl_priority_templist.value(0) == "lower"  ) ui.rbtPrioridadBaja->setChecked(true)		; // sdl_priority
		if( sdl_priority_templist.value(0) == "normal" ) ui.rbtPrioridadNormal->setChecked(true)	; // sdl_priority
		if( sdl_priority_templist.value(0) == "higher" ) ui.rbtPrioridadAlta->setChecked(true)		; // sdl_priority
		if( sdl_priority_templist.value(0) == "highest") ui.rbtPrioridadMuyAlt->setChecked(true)	; // sdl_priority
		if( sdl_priority_templist.value(1) == "lower"  ) ui.rbtNoPrioridadBaja->setChecked(true)	; // sdl_priority
		if( sdl_priority_templist.value(1) == "normal" ) ui.rbtNoPrioridadNormal->setChecked(true)	; // sdl_priority
		if( sdl_priority_templist.value(1) == "higher" ) ui.rbtNoPrioridadAlta->setChecked(true)	; // sdl_priority
		if( sdl_priority_templist.value(1) == "highest") ui.rbtNoPrioridadMuyAlt->setChecked(true)	; // sdl_priority
		
		ui.txtDbx_13->setText( query.value( rec.indexOf("sdl_mapperfile") ).toString() )										; // sdl_mapperfile
		ui.chkDbx_9->setChecked( query.value( rec.indexOf("sdl_usescancodes") ).toBool() )										; // sdl_usescancodes
		ui.txtDbx_14->setText( query.value( rec.indexOf("dosbox_language") ).toString() )										; // dosbox_language
		ui.cbxDbx_4->setCurrentIndex( ui.cbxDbx_4->findText( query.value( rec.indexOf("dosbox_machine") ).toString() ) )		; // dosbox_machine
		ui.txtDbx_15->setText( query.value( rec.indexOf("dosbox_captures") ).toString() )										; // dosbox_captures
		ui.cbxDbx_8->setCurrentIndex( ui.cbxDbx_8->findText( query.value( rec.indexOf("dosbox_memsize") ).toString() ) )		; // dosbox_memsize
		ui.cbxDbx_10->setCurrentIndex( ui.cbxDbx_10->findText( query.value( rec.indexOf("render_frameskip") ).toString() ))		; // render_frameskip
		ui.chkDbx_7->setChecked( query.value( rec.indexOf("render_aspect") ).toBool() )											; // render_aspect
		ui.cbxDbx_5->setCurrentIndex( ui.cbxDbx_5->findText( query.value( rec.indexOf("render_scaler") ).toString() ) )			; // render_scaler
		ui.cbxDbx_9->setCurrentIndex( ui.cbxDbx_9->findText( query.value( rec.indexOf("cpu_core") ).toString() ) )				; // cpu_core
		ui.cbxDbx_12->setCurrentIndex( ui.cbxDbx_12->findText( query.value( rec.indexOf("cpu_cycles") ).toString() ))			; // cpu_cycles
		ui.spinDbx_1->setValue( query.value( rec.indexOf("cpu_cycleup") ).toInt() )												; // cpu_cycleup
		ui.spinDbx_2->setValue( query.value( rec.indexOf("cpu_cycledown") ).toInt() )											; // cpu_cycledown
		ui.chkDbx_13->setChecked( query.value( rec.indexOf("mixer_nosound") ).toBool() )										; // mixer_nosound
		ui.cbxDbx_14->setCurrentIndex( ui.cbxDbx_14->findText( query.value( rec.indexOf("mixer_rate") ).toString() ))			; // mixer_rate
		ui.txtDbx_7->setText( query.value( rec.indexOf("mixer_blocksize") ).toString() )										; // mixer_blocksize
		ui.txtDbx_8->setText( query.value( rec.indexOf("mixer_prebuffer") ).toString() )										; // mixer_prebuffer
		ui.cbxDbx_28->setCurrentIndex( ui.cbxDbx_28->findText( query.value( rec.indexOf("midi_mpu401") ).toString() ))			; // midi_mpu401
		ui.chkDbx_17->setChecked( query.value( rec.indexOf("midi_intelligent") ).toBool() )										; // midi_intelligent
		ui.cbxDbx_29->setCurrentIndex( ui.cbxDbx_29->findText( query.value( rec.indexOf("midi_device") ).toString() ))			; // midi_device
		ui.txtDbx_10->setText( query.value( rec.indexOf("midi_config") ).toString() )											; // midi_config
		ui.cbxDbx_30->setCurrentIndex( ui.cbxDbx_30->findText( query.value( rec.indexOf("midi_mt32rate") ).toString() ))		; // midi_mt32rate
		ui.cbxDbx_15->setCurrentIndex( ui.cbxDbx_15->findText( query.value( rec.indexOf("sblaster_sbtype") ).toString() ))		; // sblaster_sbtype
		ui.cbxDbx_16->setCurrentIndex( ui.cbxDbx_16->findText( query.value( rec.indexOf("sblaster_sbbase") ).toString() ))		; // sblaster_sbbase
		ui.cbxDbx_17->setCurrentIndex( ui.cbxDbx_17->findText( query.value( rec.indexOf("sblaster_irq") ).toString() ))			; // sblaster_irq
		ui.cbxDbx_18->setCurrentIndex( ui.cbxDbx_18->findText( query.value( rec.indexOf("sblaster_dma") ).toString() ))			; // sblaster_dma
		ui.cbxDbx_19->setCurrentIndex( ui.cbxDbx_19->findText( query.value( rec.indexOf("sblaster_hdma") ).toString() ))		; // sblaster_hdma
		ui.chkDbx_14->setChecked( query.value( rec.indexOf("sblaster_mixer") ).toBool() )										; // sblaster_mixer
		ui.cbxDbx_20->setCurrentIndex( ui.cbxDbx_20->findText( query.value( rec.indexOf("sblaster_oplmode") ).toString() ))		; // sblaster_oplmode
		ui.cbxDbx_21->setCurrentIndex( ui.cbxDbx_21->findText( query.value( rec.indexOf("sblaster_oplrate") ).toString() ))		; // sblaster_oplrate
		ui.chkDbx_15->setChecked( query.value( rec.indexOf("gus_gus") ).toBool() )												; // gus_gus
		ui.cbxDbx_25->setCurrentIndex( ui.cbxDbx_25->findText( query.value( rec.indexOf("gus_gusrate") ).toString() ))			; // gus_gusrate
		ui.cbxDbx_22->setCurrentIndex( ui.cbxDbx_22->findText( query.value( rec.indexOf("gus_gusbase") ).toString() ))			; // gus_gusbase
		ui.cbxDbx_23->setCurrentIndex( ui.cbxDbx_23->findText( query.value( rec.indexOf("gus_irq1") ).toString() ))				; // gus_irq1
		ui.cbxDbx_24->setCurrentIndex( ui.cbxDbx_24->findText( query.value( rec.indexOf("gus_irq2") ).toString() ))				; // gus_irq2
		ui.cbxDbx_26->setCurrentIndex( ui.cbxDbx_26->findText( query.value( rec.indexOf("gus_dma1") ).toString() ))				; // gus_dma1
		ui.cbxDbx_27->setCurrentIndex( ui.cbxDbx_27->findText( query.value( rec.indexOf("gus_dma2") ).toString() ))				; // gus_dma2
		ui.txtDbx_9->setText( query.value( rec.indexOf("gus_ultradir") ).toString() )											; // gus_ultradir
		ui.chkDbx_18->setChecked( query.value( rec.indexOf("speaker_pcspeaker") ).toBool() )									; // speaker_pcspeaker
		ui.cbxDbx_31->setCurrentIndex( ui.cbxDbx_31->findText( query.value( rec.indexOf("speaker_pcrate") ).toString() ))		; // speaker_pcrate
		ui.cbxDbx_33->setCurrentIndex( ui.cbxDbx_33->findText( query.value( rec.indexOf("speaker_tandy") ).toString() ))		; // speaker_tandy
		ui.cbxDbx_32->setCurrentIndex( ui.cbxDbx_32->findText( query.value( rec.indexOf("speaker_tandyrate") ).toString() ))	; // speaker_tandyrate
		ui.chkDbx_19->setChecked( query.value( rec.indexOf("speaker_disney") ).toBool() )										; // speaker_disney
		
		ui.cbxDbx_13->setCurrentIndex( ui.cbxDbx_13->findText( query.value( rec.indexOf("joystick_type") ).toString() ))		; // joystick_type
	
		ui.chkDbx_25->setChecked( query.value( rec.indexOf("joystick_timed") ).toBool() )										; // joystick_timed 
		ui.chkDbx_26->setChecked( query.value( rec.indexOf("joystick_autofire") ).toBool() )									; // joystick_autofire 
		ui.chkDbx_27->setChecked( query.value( rec.indexOf("joystick_swap34") ).toBool() )										; // joystick_swap34
		ui.chkDbx_28->setChecked( query.value( rec.indexOf("joystick_buttonwrap") ).toBool() )									; // joystick_buttonwrap
		ui.chkDbx_23->setChecked( query.value( rec.indexOf("modem_modem") ).toBool() )											; // modem_modem
		
		ui.cbxDbx_43->setCurrentIndex( query.value( rec.indexOf("modem_comport") ).toInt()-1 )			; // modem_comport
		
		ui.txtDbx_12->setText( query.value( rec.indexOf("modem_listenport") ).toString() )										; // modem_listenport
		ui.chkDbx_24->setChecked( query.value( rec.indexOf("dserial_directserial") ).toBool() )									; // dserial_directserial
	
		ui.cbxDbx_35->setCurrentIndex( ui.cbxDbx_35->findText( query.value( rec.indexOf("dserial_comport") ).toString() ))		; // dserial_comport
		
		ui.cbxDbx_36->setCurrentIndex( ui.cbxDbx_36->findText( query.value( rec.indexOf("dserial_realport") ).toString() ))		; // serial_realport
		
		ui.cbxDbx_39->setCurrentIndex( ui.cbxDbx_39->findText( query.value( rec.indexOf("dserial_defaultbps") ).toString() ))	; // dserial_defaultbps
		ui.cbxDbx_37->setCurrentIndex( ui.cbxDbx_37->findText( query.value( rec.indexOf("dserial_parity") ).toString() ))		; // dserial_parity
		ui.cbxDbx_40->setCurrentIndex( ui.cbxDbx_40->findText( query.value( rec.indexOf("dserial_bytesize") ).toString() ))		; // dserial_bytesize
		ui.cbxDbx_41->setCurrentIndex( ui.cbxDbx_41->findText( query.value( rec.indexOf("dserial_stopbit") ).toString() ))		; // dserial_stopbit
		ui.txtDbx_17->setText( query.value( rec.indexOf("serial_1") ).toString() )												; // serial_1
		ui.txtDbx_18->setText( query.value( rec.indexOf("serial_2") ).toString() )												; // serial_2
		ui.txtDbx_19->setText( query.value( rec.indexOf("serial_3") ).toString() )												; // serial_3
		ui.txtDbx_20->setText( query.value( rec.indexOf("serial_4") ).toString() )												; // serial_4
		ui.chkDbx_10->setChecked( query.value( rec.indexOf("dos_xms") ).toBool() )												; // dos_xms
		ui.chkDbx_11->setChecked( query.value( rec.indexOf("dos_ems") ).toBool() )												; // dos_ems
		ui.cbxDbx_11->setCurrentIndex( ui.cbxDbx_11->findText( query.value( rec.indexOf("dos_umb") ).toString() ))				; // dos_umb
		ui.cbxDbx_42->setCurrentIndex( ui.cbxDbx_42->findText( query.value( rec.indexOf("dos_keyboardlayout") ).toString() ))	; // dos_keyboardlayout
		ui.chkDbx_20->setChecked( query.value( rec.indexOf("ipx_ipx") ).toBool() )												; // ipx_ipx
		ui.txtDbx_Autoexec->setText(query.value( rec.indexOf("autoexec") ).toString() )											; // autoexec
		ui.chkDbx_21->setChecked( query.value( rec.indexOf("opt_autoexec") ).toBool() )											; // opt_autoexec
		ui.chkDbx_1->setChecked( query.value( rec.indexOf("opt_loadfix") ).toBool() )											; // opt_loadfix
		ui.txtDbx_2->setText( query.value( rec.indexOf("opt_loadfix_mem") ).toString()  )										; // opt_loadfix_mem
		ui.chkDbx_3->setChecked( query.value( rec.indexOf("opt_consola_dbox") ).toBool() )										; // opt_consola_dbox
		ui.chkDbx_2->setChecked( query.value( rec.indexOf("opt_cerrar_dbox") ).toBool() )										; // opt_cerrar_dbox
		ui.chkDbx_12->setChecked( query.value( rec.indexOf("opt_cycle_sincronizar") ).toBool() )								; // opt_cycle_sincronizar
		ui.txtDbx_1->setText( query.value( rec.indexOf("path_conf") ).toString() )												; // path_conf
		ui.txtDbx_16->setText( query.value( rec.indexOf("path_sonido") ).toString())											; // path_sonido
		ui.txtDbx_3->setText( query.value( rec.indexOf("path_exe") ).toString() )												; // path_exe
		ui.txtDbx_5->setText( query.value( rec.indexOf("path_setup") ).toString() )												; // path_setup
		ui.txtDbx_4->setText( query.value( rec.indexOf("parametros_exe") ).toString() )											; // parametros_exe
		ui.txtDbx_6->setText( query.value( rec.indexOf("parametros_setup") ).toString() )										; // parametros_setup
	}
}

void frmAddEditJuego::CargarDatosDBxMontaje( QString stIDdbx )
{
	QSqlQuery query;
	QSqlRecord rec;
	QString tipoDrive;
	query.exec("SELECT * FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+stIDdbx+" ORDER BY id_lista");
	if ( query.first() )
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
			tipoDrive.clear();
			tipoDrive = query.value(rec.indexOf("tipo_as")).toString();
			if(tipoDrive=="drive")
				item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
			if(tipoDrive=="cdrom")
				item->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
			if(tipoDrive=="floppy")
				item->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
			if(tipoDrive=="IMG_floppy")
				item->setIcon( 0, QIcon(":/img16/floppy_1.png") );
			if(tipoDrive=="IMG_iso")
				item->setIcon( 0, QIcon(":/img16/cd_iso.png") );
			if(tipoDrive=="IMG_hdd")
				item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
			
			item->setText( 0 , query.value(rec.indexOf("path")).toString() )		;	// path			- directorio o iso
			item->setText( 1 , query.value(rec.indexOf("label")).toString() )		;	// label		- etiqueta
			item->setText( 2 , query.value(rec.indexOf("tipo_as")).toString() )		;	// tipo_as		- tipo de montaje
			item->setText( 3 , query.value(rec.indexOf("letter")).toString() )		;	// letter		- letra de montaje
			item->setText( 4 , query.value(rec.indexOf("indx_cd")).toString() )		;	// indx_cd		- index de la unidad de cd-rom
			item->setText( 5 , query.value(rec.indexOf("opt_mount")).toString() )	;	// opt_mount	- opciones del cd-rom
			item->setText( 6 , query.value(rec.indexOf("io_ctrl")).toString() )		;	// io_ctrl		- cd/dvd
			item->setText( 7 , query.value(rec.indexOf("select_mount")).toString())	;	// select_mount	- primer montaje
			item->setText( 8 , query.value(rec.indexOf("id")).toString() )			;	// id
			item->setText( 9 , query.value(rec.indexOf("id_lista")).toString() )	;	// id_lista		- id_lista
		} while (query.next());
	}
	  
	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"] = ui.txtDbx_3->text();
	datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
	datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();

	if(ui.chkDbx_1->isChecked())
		datos_montaje["opt_loadfix"] = "true";
	else
		datos_montaje["opt_loadfix"] = "false";

	if(ui.chkDbx_2->isChecked())
		datos_montaje["opt_cerrar_dbox"] = "true";
	else
		datos_montaje["opt_cerrar_dbox"] = "false";
  
	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmAddEditJuego::setDatosDosBox()
{
	QString  prioritySelect, priorityNoSelect;

		if( ui.rbtPrioridadBaja->isChecked()    ) prioritySelect   = "lower"  ;
		if( ui.rbtPrioridadNormal->isChecked()  ) prioritySelect   = "normal" ;
		if( ui.rbtPrioridadAlta->isChecked()    ) prioritySelect   = "higher" ;
		if( ui.rbtPrioridadMuyAlt->isChecked()  ) prioritySelect   = "highest";
		if( ui.rbtNoPrioridadBaja->isChecked()  ) priorityNoSelect = "lower"  ;
		if( ui.rbtNoPrioridadNormal->isChecked()) priorityNoSelect = "normal" ;
		if( ui.rbtNoPrioridadAlta->isChecked()  ) priorityNoSelect = "higher" ;
		if( ui.rbtNoPrioridadMuyAlt->isChecked()) priorityNoSelect = "highest";

		DatosDosBox.clear();
		if(ui.chkDbx_4->isChecked() )
		  DatosDosBox["sdl_fullscreen"] = "true"; else DatosDosBox["sdl_fullscreen"] = "false"							; // sdl_fullscreen
		if(ui.chkDbx_22->isChecked() )
		  DatosDosBox["sdl_fulldouble"] = "true"; else DatosDosBox["sdl_fulldouble"] = "false"							; // sdl_fulldouble
		if(ui.chkDbx_5->isChecked() )
		  DatosDosBox["sdl_fullfixed"] = "true"; else DatosDosBox["sdl_fullfixed"] = "false"							; // sdl_fullfixed
		if(ui.cbxDbx_1->currentText()!="")
		  DatosDosBox["sdl_fullresolution"] = ui.cbxDbx_1->currentText(); else DatosDosBox["sdl_fullresolution"] = "original"; // sdl_fullresolution
		if(ui.cbxDbx_2->currentText()!="")
		  DatosDosBox["sdl_windowresolution"] = ui.cbxDbx_2->currentText(); else DatosDosBox["sdl_windowresolution"] = "original"; // sdl_windowresolution
		if(ui.cbxDbx_3->currentText()!="")
		  DatosDosBox["sdl_output"] = ui.cbxDbx_3->currentText(); else DatosDosBox["sdl_output"] = "surface"			; // sdl_output
		if(ui.cbxDbx_6->currentText()!="")
		  DatosDosBox["sdl_hwscale"] = ui.cbxDbx_6->currentText(); else DatosDosBox["sdl_hwscale"] = "1.00"				; // sdl_hwscale
		if(ui.chkDbx_6->isChecked() )
		  DatosDosBox["sdl_autolock"] = "true"; else DatosDosBox["sdl_autolock"] = "false"								; // sdl_autolock
		if(ui.cbxDbx_7->currentText()!="")
		  DatosDosBox["sdl_sensitivity"] = ui.cbxDbx_7->currentText(); else DatosDosBox["sdl_sensitivity"] = "100"		; // sdl_sensitivity
		if(ui.chkDbx_8->isChecked() )
		  DatosDosBox["sdl_waitonerror"] = "true"; else DatosDosBox["sdl_waitonerror"] = "false"						; // sdl_waitonerror											
		if(prioritySelect!="" && priorityNoSelect!="" )
		  DatosDosBox["sdl_priority"] = prioritySelect+","+priorityNoSelect ; else DatosDosBox["sdl_priority"] = "higher,normal"; // sdl_priority
		if(ui.txtDbx_13->text()!="")
		  DatosDosBox["sdl_mapperfile"] = ui.txtDbx_13->text(); else DatosDosBox["sdl_mapperfile"] = "mapper.txt"					; // sdl_mapperfile
		if(ui.chkDbx_9->isChecked()	)
		  DatosDosBox["sdl_usescancodes"] = "true"; else DatosDosBox["sdl_usescancodes"] = "true"						; // sdl_usescancodes
		
		
		if(ui.txtDbx_14->text()!="")
		  DatosDosBox["dosbox_language"] = ui.txtDbx_14->text(); DatosDosBox["dosbox_language"] = ""					; // dosbox_language
		if(ui.cbxDbx_4->currentText()!="")
		  DatosDosBox["dosbox_machine"] = ui.cbxDbx_4->currentText(); else DatosDosBox["dosbox_machine"] = "vga"		; // dosbox_machine
		if(ui.txtDbx_15->text()!="")
		  DatosDosBox["dosbox_captures"] = ui.txtDbx_15->text(); else DatosDosBox["dosbox_captures"] = ""				; // dosbox_captures
		if(ui.cbxDbx_8->currentText()!="")
		  DatosDosBox["dosbox_memsize"] = ui.cbxDbx_8->currentText(); else DatosDosBox["dosbox_memsize"] = "16"			; // dosbox_memsize
	
	
		if(ui.cbxDbx_10->currentText()!="")
		  DatosDosBox["render_frameskip"] = ui.cbxDbx_10->currentText(); else DatosDosBox["render_frameskip"] = "0"		; // render_frameskip
		if(ui.chkDbx_7->isChecked() )
		  DatosDosBox["render_aspect"] = "true"; else DatosDosBox["render_aspect"] = "false"							; // render_aspect
		if(ui.cbxDbx_5->currentText()!="")
		  DatosDosBox["render_scaler"] = ui.cbxDbx_5->currentText(); else DatosDosBox["render_scaler"] = "normal2x"		; // render_scaler
	

		if(ui.cbxDbx_9->currentText()!="")
		  DatosDosBox["cpu_core"] = ui.cbxDbx_9->currentText(); else DatosDosBox["cpu_core"] = "auto"					; // cpu_core
		if(ui.cbxDbx_12->currentText()!="")
		  DatosDosBox["cpu_cycles"] = ui.cbxDbx_12->currentText(); else DatosDosBox["cpu_cycles"] = "auto"				; // cpu_cycles
		if(ui.spinDbx_1->value() >= 0)
		  DatosDosBox["cpu_cycleup"] = fGrl.IntToStr( ui.spinDbx_1->value() ) ; else DatosDosBox["cpu_cycleup"] = "500"			; // cpu_cycleup
		if(ui.spinDbx_2->value() >= 0)
		  DatosDosBox["cpu_cycledown"] = fGrl.IntToStr( ui.spinDbx_2->value() ); else DatosDosBox["cpu_cycledown"] = "20"		; // cpu_cycledown


		if(ui.chkDbx_13->isChecked() )
		  DatosDosBox["mixer_nosound"] = "true"; else DatosDosBox["mixer_nosound"] = "false"							; // mixer_nosound
		if(ui.cbxDbx_14->currentText()!="")
		  DatosDosBox["mixer_rate"] = ui.cbxDbx_14->currentText(); else DatosDosBox["mixer_rate"] = "22050"				; // mixer_rate
		if(ui.txtDbx_7->text()!="")
		  DatosDosBox["mixer_blocksize"] = ui.txtDbx_7->text(); else DatosDosBox["mixer_blocksize"] = "2048" 				; // mixer_blocksize
		if(ui.txtDbx_8->text()!="")
		  DatosDosBox["mixer_prebuffer"] = ui.txtDbx_8->text(); else DatosDosBox["mixer_prebuffer"] = "10"				; // mixer_prebuffer
		if(ui.cbxDbx_28->currentText()!="")
		  DatosDosBox["midi_mpu401"] = ui.cbxDbx_28->currentText(); else DatosDosBox["midi_mpu401"] = "intelligent"		; // midi_mpu401
		if(ui.chkDbx_17->isChecked() )
		  DatosDosBox["midi_intelligent"] = "true"; else DatosDosBox["midi_intelligent"] = "false"						; // midi_intelligent
		if(ui.cbxDbx_29->currentText()!="")
		  DatosDosBox["midi_device"] = ui.cbxDbx_29->currentText(); else DatosDosBox["midi_device"] = "default"			; // midi_device
		if(ui.txtDbx_10->text()!="")
		  DatosDosBox["midi_config"] = ui.txtDbx_10->text();else	DatosDosBox["midi_config"] = ""						; // midi_config
		if(ui.cbxDbx_30->currentText()!="")
		  DatosDosBox["midi_mt32rate"] = ui.cbxDbx_30->currentText(); else DatosDosBox["midi_mt32rate"] = "22050"		; // midi_mt32rate
	
	
		if(ui.cbxDbx_15->currentText()!="")
		  DatosDosBox["sblaster_sbtype"] = ui.cbxDbx_15->currentText(); else DatosDosBox["sblaster_sbtype"] = "sb16"	; // sblaster_sbtype
		if(ui.cbxDbx_16->currentText()!="")
		  DatosDosBox["sblaster_sbbase"] = ui.cbxDbx_16->currentText(); else DatosDosBox["sblaster_sbbase"] = "220"		; // sblaster_sbbase
		if(ui.cbxDbx_17->currentText()!="")
		  DatosDosBox["sblaster_irq"] = ui.cbxDbx_17->currentText(); else DatosDosBox["sblaster_irq"] = "7"				; // sblaster_irq
		if(ui.cbxDbx_18->currentText()!="")
		  DatosDosBox["sblaster_dma"] = ui.cbxDbx_18->currentText(); else DatosDosBox["sblaster_dma"] = "1"				; // sblaster_dma
		if(ui.cbxDbx_19->currentText()!="")
		  DatosDosBox["sblaster_hdma"] = ui.cbxDbx_19->currentText(); else DatosDosBox["sblaster_hdma"] = "5"			; // sblaster_hdma
		if(ui.chkDbx_14->isChecked() )
		  DatosDosBox["sblaster_mixer"] = "true"; else DatosDosBox["sblaster_mixer"] = "false"							; // sblaster_mixer
		if(ui.cbxDbx_20->currentText()!="")
		  DatosDosBox["sblaster_oplmode"] = ui.cbxDbx_20->currentText(); else DatosDosBox["sblaster_oplmode"] = "auto"	; // sblaster_oplmode
		if(ui.cbxDbx_21->currentText()!="")
		  DatosDosBox["sblaster_oplrate"] = ui.cbxDbx_21->currentText(); else DatosDosBox["sblaster_oplrate"] = "22050"	; // sblaster_oplrate
	
	
		if(ui.chkDbx_15->isChecked() )
		  DatosDosBox["gus_gus"] = "true"; else DatosDosBox["gus_gus"] = "false"										; // gus_gus
		if(ui.cbxDbx_25->currentText()!="")
		  DatosDosBox["gus_gusrate"] = ui.cbxDbx_25->currentText(); else DatosDosBox["gus_gusrate"] = "22050"			; // gus_gusrate
		if(ui.cbxDbx_22->currentText()!="")
		  DatosDosBox["gus_gusbase"] = ui.cbxDbx_22->currentText(); else DatosDosBox["gus_gusbase"] = "240"				; // gus_gusbase
		if(ui.cbxDbx_23->currentText()!="")
		  DatosDosBox["gus_irq1"] = ui.cbxDbx_23->currentText(); else DatosDosBox["gus_irq1"] = "5"						; // gus_irq1
		if(ui.cbxDbx_24->currentText()!="")
		  DatosDosBox["gus_irq2"] = ui.cbxDbx_24->currentText(); else DatosDosBox["gus_irq2"] = "5"						; // gus_irq2
		if(ui.cbxDbx_26->currentText()!="")
		  DatosDosBox["gus_dma1"] = ui.cbxDbx_26->currentText(); else DatosDosBox["gus_dma1"] = "3"						; // gus_dma1
		if(ui.cbxDbx_27->currentText()!="")
		  DatosDosBox["gus_dma2"] = ui.cbxDbx_27->currentText(); else DatosDosBox["gus_dma2"] = "3"						; // gus_dma2
		if(ui.txtDbx_9->text()!="")
		  DatosDosBox["gus_ultradir"] = ui.txtDbx_9->text(); else DatosDosBox["gus_ultradir"] = "C:/ULTRASND"						; // gus_ultradir
		
		
		if(ui.chkDbx_18->isChecked() )
		  DatosDosBox["speaker_pcspeaker"] = "true"; else DatosDosBox["speaker_pcspeaker"] = "false"					; // speaker_pcspeaker
		if(ui.cbxDbx_31->currentText()!="")
		  DatosDosBox["speaker_pcrate"] = ui.cbxDbx_31->currentText(); else DatosDosBox["speaker_pcrate"] = "22050"		; // speaker_pcrate
		if(ui.cbxDbx_33->currentText()!="")
		  DatosDosBox["speaker_tandy"] = ui.cbxDbx_33->currentText(); else DatosDosBox["speaker_tandy"] = "auto"		; // speaker_tandy
		if(ui.cbxDbx_32->currentText()!="")
		  DatosDosBox["speaker_tandyrate"] = ui.cbxDbx_32->currentText(); else DatosDosBox["speaker_tandyrate"] = "22050"; // speaker_tandyrate
		if(ui.chkDbx_19->isChecked() )
		  DatosDosBox["speaker_disney"] = "true"; else DatosDosBox["speaker_disney"] = "false"							; // speaker_disney
	
	
		if(ui.cbxDbx_13->currentText()!="")
		  DatosDosBox["joystick_type"] = ui.cbxDbx_13->currentText(); else DatosDosBox["joystick_type"] = "auto"		; // joystick_type
		if(ui.chkDbx_25->isChecked() )
		  DatosDosBox["joystick_timed"] = "true"; else DatosDosBox["joystick_timed"] = "false"			; // joystick_timed
		if(ui.chkDbx_26->isChecked() )
		  DatosDosBox["joystick_autofire"] = "true"; else DatosDosBox["joystick_autofire"] = "false"	; // joystick_autofire
		if(ui.chkDbx_27->isChecked() )
		  DatosDosBox["joystick_swap34"] = "true"; else DatosDosBox["joystick_swap34"] = "false"		; // joystick_swap34
		if(ui.chkDbx_28->isChecked() )
		  DatosDosBox["joystick_buttonwrap"] = "true"; else DatosDosBox["joystick_buttonwrap"] = "false"						; // joystick_buttonwrap
	
	
		if(ui.chkDbx_23->isChecked() )
		  DatosDosBox["modem_modem"] = "true"; else DatosDosBox["modem_modem"] = "false"								; // modem_modem
		if(ui.cbxDbx_43->currentText()!="")
		  DatosDosBox["modem_comport"] = fGrl.IntToStr(ui.cbxDbx_43->currentIndex()+1); else DatosDosBox["modem_comport"] = "2"	; // modem_comport
		if(ui.txtDbx_12->text()!="")
		  DatosDosBox["modem_listenport"] = ui.txtDbx_12->text() ; else DatosDosBox["modem_listenport"] = ""			; // modem_listenport
	
	
		if(ui.chkDbx_24->isChecked() )
		  DatosDosBox["dserial_directserial"] = "true"; else DatosDosBox["dserial_directserial"] = "false"				; // dserial_directserial
		if(ui.cbxDbx_35->currentText()!="")
		  DatosDosBox["dserial_comport"] = ui.cbxDbx_35->currentText(); else DatosDosBox["dserial_comport"] = "1"		; // dserial_comport
		if(ui.cbxDbx_36->currentText()!="")
		  DatosDosBox["dserial_realport"] = ui.cbxDbx_36->currentText(); else DatosDosBox["dserial_realport"] = "COM1"	; // dserial_realport
		if(ui.cbxDbx_39->currentText()!="")
		  DatosDosBox["dserial_defaultbps"] = ui.cbxDbx_39->currentText(); else DatosDosBox["dserial_defaultbps"] = "1200"	; // dserial_defaultbps
		if(ui.cbxDbx_37->currentText()!="")
		  DatosDosBox["dserial_parity"] = ui.cbxDbx_37->currentText(); else DatosDosBox["dserial_parity"] = "N"			; // dserial_parity
		if(ui.cbxDbx_40->currentText()!="")
		  DatosDosBox["dserial_bytesize"] = ui.cbxDbx_40->currentText(); else DatosDosBox["dserial_bytesize"] = "8"		; // dserial_bytesize
		if(ui.cbxDbx_41->currentText()!="")
		  DatosDosBox["dserial_stopbit"] = ui.cbxDbx_41->currentText(); else DatosDosBox["dserial_stopbit"] = "1"		; // dserial_stopbit
	
	
		if(ui.txtDbx_17->text()!="")
		  DatosDosBox["serial_1"] = ui.txtDbx_17->text(); else DatosDosBox["serial_1"] = ""		      ; // serial_1
		if(ui.txtDbx_18->text()!="")
		  DatosDosBox["serial_2"] = ui.txtDbx_18->text(); else DatosDosBox["serial_2"] = ""		      ; // serial_2
		if(ui.txtDbx_19->text()!="")
		  DatosDosBox["serial_3"] = ui.txtDbx_19->text(); else DatosDosBox["serial_3"] = ""		      ; // serial_3
		if(ui.txtDbx_20->text()!="")
		  DatosDosBox["serial_4"] = ui.txtDbx_20->text(); else DatosDosBox["serial_4"] = ""		      ; // serial_4
	
	
		if(ui.chkDbx_10->isChecked() )
		  DatosDosBox["dos_xms"] = "true"; else DatosDosBox["dos_xms"] = "false"						          ; // dos_xms
		if(ui.chkDbx_11->isChecked() )
		  DatosDosBox["dos_ems"] = "true"; else DatosDosBox["dos_ems"] = "false"						          ; // dos_ems
		if(ui.cbxDbx_11->currentText()!="")
		  DatosDosBox["dos_umb"] = ui.cbxDbx_11->currentText(); else DatosDosBox["dos_umb"] = "true"	; // dos_umb
		if(ui.cbxDbx_42->currentText()!="")
		  DatosDosBox["dos_keyboardlayout"] = ui.cbxDbx_42->currentText(); else DatosDosBox["dos_keyboardlayout"] = "none"	; // dos_keyboardlayout
		
		
		if(ui.chkDbx_20->isChecked() )
		  DatosDosBox["ipx_ipx"] = "true"; else DatosDosBox["ipx_ipx"] = "false"						; // ipx_ipx
		
		
		if(ui.txtDbx_Autoexec->toPlainText()!="")
		  DatosDosBox["autoexec"] = ui.txtDbx_Autoexec->toPlainText(); else DatosDosBox["autoexec"] = ""; // autoexec
		
		
		if(ui.chkDbx_21->isChecked() )
		  DatosDosBox["opt_autoexec"] = "true"; else DatosDosBox["opt_autoexec"] = "false"				; // opt_autoexec
		if(ui.chkDbx_1->isChecked() )
		  DatosDosBox["opt_loadfix"] = "true"; else DatosDosBox["opt_loadfix"] = "false"				; // opt_loadfix
		if(ui.txtDbx_2->text()!="")
		  DatosDosBox["opt_loadfix_mem"] = ui.txtDbx_2->text(); else  DatosDosBox["opt_loadfix_mem"] = ""	; // opt_loadfix_mem
		if(ui.chkDbx_3->isChecked() )
		  DatosDosBox["opt_consola_dbox"] = "true"; else DatosDosBox["opt_consola_dbox"] = "false"		; // opt_consola_dbox
		if(ui.chkDbx_2->isChecked() )
		  DatosDosBox["opt_cerrar_dbox"] = "true"; else DatosDosBox["opt_cerrar_dbox"] = "false"		; // opt_cerrar_dbox
		if(ui.chkDbx_12->isChecked() )
		  DatosDosBox["opt_cycle_sincronizar"] = "true"; else DatosDosBox["opt_cycle_sincronizar"] = "false"			; // opt_cycle_sincronizar
		
		
		if(ui.txtDbx_1->text()!="")
		  DatosDosBox["path_conf"] = ui.txtDbx_1->text() ; else DatosDosBox["path_conf"] = ""													; // path_conf
		if(ui.txtDbx_16->text()!="")
		  DatosDosBox["path_sonido"] = ui.txtDbx_16->text()	; else DatosDosBox["path_sonido"] = ""											; // path_sonido
		if(ui.txtDbx_3->text()!="")
		  DatosDosBox["path_exe"] = ui.txtDbx_3->text()	; else DatosDosBox["path_exe"] = ""												; // path_exe
		if(ui.txtDbx_5->text()!="")
		  DatosDosBox["path_setup"] = ui.txtDbx_5->text(); else DatosDosBox["path_setup"] = ""													; // path_setup
		if(ui.txtDbx_4->text()!="")
		  DatosDosBox["parametros_exe"] = ui.txtDbx_4->text(); else DatosDosBox["parametros_exe"] = ""												; // parametros_exe
		if(ui.txtDbx_6->text()!="")
		  DatosDosBox["parametros_setup"] = ui.txtDbx_6->text(); else DatosDosBox["parametros_setup"] = ""	; // parametros_setup
}

void frmAddEditJuego::on_txtDbx_1_textChanged(const QString &)
{
	QRegExp rx("/([a-zA-Z0-9]+)");
	QValidator *validator = new QRegExpValidator(rx, this);

	ui.txtDbx_1->setValidator(validator);
	
	bool str_ok;
	QString str = ui.txtDbx_1->text();
	if(str != "")
	{
		str = fGrl.eliminar_caracteres( str );
  		str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");
		ui.txtDbx_1->setText( str );
	} else
		ui.txtDbx_1->setText( "" );
}

void frmAddEditJuego::on_btnFileConfg()
{
	bool str_ok;
	QString str, archivo;
	
	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."),  stHomeDir + "confdbx/", ui.txtDbx_1->text(), tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
	
		if(EditandoJuego != true)
		{
			if ( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el DOSBox ya esixte"));
		}
		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
	  	str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");
		ui.txtDbx_1->setText( str );
	} else 
		ui.txtDbx_1->setText( "" );
}

void frmAddEditJuego::on_btnExeJuego()
{
	ui.txtDbx_3->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_3->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnExeSetup()
{
	ui.txtDbx_5->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_5->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDirGravisUltraSound()
{
	ui.txtDbx_9->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_9->text() ) );
}

void frmAddEditJuego::on_btnDbx_mapperfile()
{
	ui.txtDbx_13->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_13->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDbx_language()
{
	ui.txtDbx_14->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_14->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDbx_capturas()
{
	ui.txtDbx_15->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_15->text() ) );
}

void frmAddEditJuego::on_btnDbx_musica()
{
	ui.txtDbx_16->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_16->text() ) );
}

void frmAddEditJuego::on_btnAddSerial()
{
	switch ( ui.cbxDbx_34->currentIndex() )
	{
		case 0:       // serial 1
			ui.txtDbx_17->setText(setOpcionesSerial());		
		break;
		case 1:       // serial 2
			ui.txtDbx_18->setText(setOpcionesSerial());		
		break;
		case 2:       // serial 3
			ui.txtDbx_19->setText(setOpcionesSerial());		
		break;
		case 3:       // serial 4
			ui.txtDbx_20->setText(setOpcionesSerial());
		break;
    }
}

QString frmAddEditJuego::setOpcionesSerial()
{
	QString Serial_Dbx;
	switch ( ui.cbxDbx_38->currentIndex() )
	{
		case 0:       // diasabled
			Serial_Dbx = ui.cbxDbx_38->currentText() ;
		break;
        case 1:       // dummy
            Serial_Dbx = ui.cbxDbx_38->currentText();
    	break;
        case 2:       // modem
            Serial_Dbx = ui.cbxDbx_38->currentText() + " " +
						"listenport:" + ui.txtDbx_12->text() + " " +
						"realport:" + ui.cbxDbx_36->currentText() + " " +
						"startbps:" + ui.cbxDbx_39->currentText() + " " +
						"parity:" + ui.cbxDbx_37->currentText() + " " +
						"bytesize:" + ui.cbxDbx_40->currentText() + " " +
						"stopbits:" + ui.cbxDbx_41->currentText() + " " +
						"irq:" + ui.txtDbx_11->text();
    	break;
        case 3:       // directserial
            Serial_Dbx = ui.cbxDbx_38->currentText() + " " +
						"realport:" + ui.cbxDbx_36->currentText() + " " +
						"startbps:" + ui.cbxDbx_39->currentText() + " " +
						"parity:" + ui.cbxDbx_37->currentText() + " " +
						"bytesize:" + ui.cbxDbx_40->currentText() + " " +
						"stopbits:" + ui.cbxDbx_41->currentText() + " " +
						"irq:" + ui.txtDbx_11->text();
    	break;
    }
	return Serial_Dbx;
}

void frmAddEditJuego::on_btnMount_Add()
{	
	QVector<QString> datos_montaje_edit;
	datos_montaje_edit.clear();
	datos_montaje_edit << ""; // directorio o iso
	datos_montaje_edit << ""; // etiqueta
	datos_montaje_edit << ""; // tipo de montaje
	datos_montaje_edit << ""; // letra de montaje
	datos_montaje_edit << ""; // index de la unidad de cd-rom
	datos_montaje_edit << ""; // opciones del cd-rom
	datos_montaje_edit << ""; // cd/dvd

	frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes(false,datos_montaje_edit);
    if ( AddEditMontajes->exec() == QDialog::Accepted )
    {
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		QString tipoDrive = AddEditMontajes->DatosMontaje[2];
		if(tipoDrive=="drive")
			item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
		if(tipoDrive=="cdrom")
			item->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
		if(tipoDrive=="floppy")
			item->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
		if(tipoDrive=="IMG_floppy")
			item->setIcon( 0, QIcon(":/img16/floppy_1.png") );
		if(tipoDrive=="IMG_iso")
			item->setIcon( 0, QIcon(":/img16/cd_iso.png") );
		if(tipoDrive=="IMG_hdd")
			item->setIcon( 0, QIcon(":/img16/drive_hd.png") );

		item->setText( 0 , AddEditMontajes->DatosMontaje[0] );	// directorio o iso
		item->setText( 1 , AddEditMontajes->DatosMontaje[1] );	// etiqueta
		item->setText( 2 , AddEditMontajes->DatosMontaje[2] );	// tipo de montaje
		item->setText( 3 , AddEditMontajes->DatosMontaje[3] );	// letra de montaje
		item->setText( 4 , AddEditMontajes->DatosMontaje[4] );	// index de la unidad de cd-rom
		item->setText( 5 , AddEditMontajes->DatosMontaje[5] );	// opciones del cd-rom
		item->setText( 6 , AddEditMontajes->DatosMontaje[6] );	// cd/dvd
		item->setText( 7 , "x"								);	// primer montaje

		if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
		{
			QHash<QString, QString> datos_montaje_nuevo;
			datos_montaje_nuevo.clear();
			datos_montaje_nuevo["id_lista"] = "";
			datos_montaje_nuevo["path"] = AddEditMontajes->DatosMontaje[0];
			datos_montaje_nuevo["label"] = AddEditMontajes->DatosMontaje[1];
			datos_montaje_nuevo["tipo_as"] = AddEditMontajes->DatosMontaje[2];
			datos_montaje_nuevo["letter"] = AddEditMontajes->DatosMontaje[3];
			datos_montaje_nuevo["indx_cd"] = AddEditMontajes->DatosMontaje[4];
			datos_montaje_nuevo["opt_mount"] = AddEditMontajes->DatosMontaje[5];
			datos_montaje_nuevo["io_ctrl"] = AddEditMontajes->DatosMontaje[6];
			datos_montaje_nuevo["select_mount"] = "x";			
			
			item->setText( 8 , sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
		} else
			item->setText( 8 , "" );//
		item->setText( 9 , fGrl.IntToStr( item->childCount() ) ); //

		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_3->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
		datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();
		if(ui.chkDbx_1->isChecked())
		  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_2->isChecked())
		  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";

		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}

void frmAddEditJuego::on_btnMount_Edit()
{
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		QVector<QString> datos_montaje_edit;
		datos_montaje_edit.clear();
		datos_montaje_edit << ui.twMontajes->currentItem()->text(0); // directorio o iso
		datos_montaje_edit << ui.twMontajes->currentItem()->text(1); // etiqueta
		datos_montaje_edit << ui.twMontajes->currentItem()->text(2); // tipo de montaje
		datos_montaje_edit << ui.twMontajes->currentItem()->text(3); // letra de montaje
		datos_montaje_edit << ui.twMontajes->currentItem()->text(4); // index de la unidad de cd-rom
		datos_montaje_edit << ui.twMontajes->currentItem()->text(5); // opciones del cd-rom
		datos_montaje_edit << ui.twMontajes->currentItem()->text(6); // cd/dvd
	
		frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes(true,datos_montaje_edit);
	    if ( AddEditMontajes->exec() == QDialog::Accepted )
	    {
			QString tipoDrive = AddEditMontajes->DatosMontaje[2];
			if(tipoDrive=="drive")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_hd.png") );
			if(tipoDrive=="cdrom")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
			if(tipoDrive=="floppy")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
			if(tipoDrive=="IMG_floppy")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/floppy_1.png") );
			if(tipoDrive=="IMG_iso")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/cd_iso.png") );
			if(tipoDrive=="IMG_hdd")
				ui.twMontajes->currentItem()->setIcon( 0, QIcon(":/img16/drive_hd.png") );

			ui.twMontajes->currentItem()->setText( 0 , AddEditMontajes->DatosMontaje[0] );	// directorio o iso
			ui.twMontajes->currentItem()->setText( 1 , AddEditMontajes->DatosMontaje[1] );	// etiqueta
			ui.twMontajes->currentItem()->setText( 2 , AddEditMontajes->DatosMontaje[2] );	// tipo de montaje
			ui.twMontajes->currentItem()->setText( 3 , AddEditMontajes->DatosMontaje[3] );	// letra de montaje
			ui.twMontajes->currentItem()->setText( 4 , AddEditMontajes->DatosMontaje[4] );	// index de la unidad de cd-rom
			ui.twMontajes->currentItem()->setText( 5 , AddEditMontajes->DatosMontaje[5] );	// opciones del cd-rom
			ui.twMontajes->currentItem()->setText( 6 , AddEditMontajes->DatosMontaje[6] );	// cd/dvd
			ui.twMontajes->currentItem()->setText( 7 , "x"								);	// primer montaje
	/*
			if( (EditandoJuego == true) && (TipoEmulador == "dosbox") ){
				QHash<QString, QString> datos_montaje_nuevo;
				datos_montaje_nuevo.clear();
				datos_montaje_nuevo["id_lista"] = "";
				datos_montaje_nuevo["path"] = AddEditMontajes->DatosMontaje[0];
				datos_montaje_nuevo["label"] = AddEditMontajes->DatosMontaje[1];
				datos_montaje_nuevo["tipo_as"] = AddEditMontajes->DatosMontaje[2];
				datos_montaje_nuevo["letter"] = AddEditMontajes->DatosMontaje[3];
				datos_montaje_nuevo["indx_cd"] = AddEditMontajes->DatosMontaje[4];
				datos_montaje_nuevo["opt_mount"] = AddEditMontajes->DatosMontaje[5];
				datos_montaje_nuevo["io_ctrl"] = AddEditMontajes->DatosMontaje[6];
				datos_montaje_nuevo["select_mount"] = "x";		
							
				ui.twMontajes->currentItem()->setText( 8 , sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
			}else ui.twMontajes->currentItem()->setText( 8 , "" );//
	*/
			ui.twMontajes->currentItem()->setText( 9 , fGrl.IntToStr( ui.twMontajes->currentItem()->childCount() ) ); //
	
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"] = ui.txtDbx_3->text();
			datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
			datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();
			if(ui.chkDbx_1->isChecked())
			  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_2->isChecked())
			  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
	
			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
	}
}

void frmAddEditJuego::on_btnMount_Delete()
{
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		int EliminarSiNO;
		EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar Juego...?"), tr( "¿Deseas eliminar este montaje?" ), tr( "Si" ), tr( "Cancelar" ), 0, 1 );
		if( EliminarSiNO == 0 )
		{
			if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
				sql->ItemEliminarMontaje( ui.twMontajes->currentItem()->text(8) );

			fGrl.DeleteItemTree( ui.twMontajes->currentItem() ) ;
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"] = ui.txtDbx_3->text();
			datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
			datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();
			if(ui.chkDbx_1->isChecked())
			  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_2->isChecked())
			  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
			  
			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
	}
}

void frmAddEditJuego::on_btnMount_Clear()
{
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
		{
			for (int num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = ui.twMontajes->topLevelItem( num_mount );
				sql->ItemEliminarMontaje( item->text(8) );
			}
		}
		ui.twMontajes->clear();
		ui.previer_mount->clear();
	}
}

void frmAddEditJuego::on_btnMount_Subir()
{
	//
}

void frmAddEditJuego::on_btnMount_Bajar()
{
	//
}

void frmAddEditJuego::on_btnMount_AutoCrear()
{
	QFileInfo fi( ui.txtDbx_3->text() );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
	item->setIcon( 0, QIcon(":/img16/drive_hd.png") );
	item->setText( 0 , fi.absolutePath() );	// directorio o iso
	item->setText( 1 , ""      );	// etiqueta
	item->setText( 2 , "drive" );	// tipo de montaje
	item->setText( 3 , "C"     );	// letra de montaje
	item->setText( 4 , ""      );	// index de la unidad de cd-rom
	item->setText( 5 , "-usecd 0");	// opciones del cd-rom
	item->setText( 6 , "-aspi"      );	// cd/dvd IOCtrl
	item->setText( 7 , "x"     );	// primer montaje

	if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
	{
		QHash<QString, QString> datos_montaje_nuevo;
		datos_montaje_nuevo.clear();
		datos_montaje_nuevo["id_lista"] = fGrl.IntToStr( item->childCount() );
		datos_montaje_nuevo["path"] = fi.absolutePath();
		datos_montaje_nuevo["label"] = "";
		datos_montaje_nuevo["tipo_as"] = "drive";
		datos_montaje_nuevo["letter"] = "C";
		datos_montaje_nuevo["indx_cd"] = "";
		datos_montaje_nuevo["opt_mount"] = "-usecd 0";
		datos_montaje_nuevo["io_ctrl"] = "-aspi";
		datos_montaje_nuevo["select_mount"] = "x";		
				
		item->setText( 8 , sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
	} else
		item->setText( 8 , "" );//
	item->setText( 9 , fGrl.IntToStr( item->childCount() )  ); //

	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"] = ui.txtDbx_3->text();
	datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
	datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();
	if(ui.chkDbx_1->isChecked())
	  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
	if(ui.chkDbx_2->isChecked())
	  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";

	ui.previer_mount->clear();
	ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
}

void frmAddEditJuego::on_btnMount_Primario()
{
	int indx_mount=0, num_mount=0;
	if( ui.twMontajes->topLevelItemCount()>0 )
	{
		indx_mount = ui.twMontajes->indexOfTopLevelItem(ui.twMontajes->currentItem());
		for ( num_mount = 0; num_mount < ui.twMontajes->topLevelItemCount(); num_mount++ )
			ui.twMontajes->topLevelItem( num_mount )->setText(7 , "x");

		ui.twMontajes->topLevelItem( indx_mount )->setText(7 , "v");
		
		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_3->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_4->text();
		datos_montaje["opt_loadfix_mem"] = ui.txtDbx_2->text();
		if(ui.chkDbx_1->isChecked())
		  datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_2->isChecked())
		  datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";
		  
		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}
// ---------------------------------------------------------------
// Referente al VDMSound -----------------------------------------
void frmAddEditJuego::CargarDatosVDMSound( QString stIDvdms )
{
	QString strTemp, str_program, str_vdms_debug, str_winnt_dos, str_winnt_dosbox, str_winnt_storage;
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+stIDvdms+" LIMIT 0,1");
	if ( query.first() )
	{
		rec = query.record();

		ui.txtVdms_1->setText( query.value(rec.indexOf("path_conf")).toString() )	; // 
		ui.txtVdms_2->setText( query.value(rec.indexOf("path_exe")).toString() )	; // 
		str_program       = query.value(rec.indexOf("program")).toString()			; // 
		str_vdms_debug    = query.value(rec.indexOf("vdms_debug")).toString()		; // 
		str_winnt_dos     = query.value(rec.indexOf("winnt_dos")).toString()		; // 
		str_winnt_dosbox  = query.value(rec.indexOf("winnt_dosbox")).toString()		; // 
		str_winnt_storage = query.value(rec.indexOf("winnt_storage")).toString()	; // 
	
		QStringList list_program = str_program.split("|");
		QStringList list_winnt_dos = str_winnt_dos.split("|");
		QStringList list_vdms_debug = str_vdms_debug.split("|");
		QStringList list_winnt_dosbox = str_winnt_dosbox.split("|");
		QStringList list_winnt_storage = str_winnt_storage.split("|");
		
		ui.txtVdms_3->setText( list_program[0] ); 
	 	ui.txtVdms_4->setText( list_program[1] ); 
	
		if(list_winnt_dos[0]=="yes")
			ui.chkVdms_1->setChecked(true); else ui.chkVdms_1->setChecked(false); 
		
		strTemp.clear();
		strTemp = list_winnt_dos[1];
		ui.txtVdms_5->setPlainText( strTemp.replace("%000d%000a", "\n") );
		
		if(list_vdms_debug[0]=="yes")
			ui.chkVdms_2->setChecked(true); else ui.chkVdms_2->setChecked(false); 
		
		strTemp.clear();
		strTemp = list_vdms_debug[1];
		ui.txtVdms_6->setPlainText( strTemp.replace("%000d%000a", "\n") );
				
		if(list_winnt_dosbox[0]=="yes")
			ui.chkVdms_3->setChecked(true); else ui.chkVdms_3->setChecked(false); 
		if(list_winnt_dosbox[1]=="yes")
			ui.chkVdms_4->setChecked(true); else ui.chkVdms_4->setChecked(false); 
		if(list_winnt_dosbox[2]=="yes")
			ui.chkVdms_5->setChecked(true); else ui.chkVdms_5->setChecked(false); 
	
		if(list_winnt_storage[0]=="yes")
			ui.chkVdms_6->setChecked(true); else ui.chkVdms_6->setChecked(false); 
		if(list_winnt_storage[1]=="yes")
			ui.chkVdms_7->setChecked(true); else ui.chkVdms_7->setChecked(false); 
	}	
}

void frmAddEditJuego::setDatosVDMSound()
{
	QString strTemp;
	DatosVDMSound.clear();
	DatosVDMSound["path_conf"] = ui.txtVdms_1->text()	;
	DatosVDMSound["path_exe"]  = ui.txtVdms_2->text()	;
	DatosVDMSound["program_1"] = ui.txtVdms_3->text() 	;
	DatosVDMSound["program_2"] = ui.txtVdms_4->text()	;
	if(ui.chkVdms_1->isChecked())
	  DatosVDMSound["winnt_dos_1"] = "yes"; else DatosVDMSound["winnt_dos_1"] = "no";
	
	strTemp.clear();
	strTemp = ui.txtVdms_5->toPlainText();
	DatosVDMSound["winnt_dos_2"] = strTemp.replace("\n", "%000d%000a");
	
	if(ui.chkVdms_2->isChecked())
	  DatosVDMSound["vdms_debug_1"] = "yes"; else DatosVDMSound["vdms_debug_1"] = "no";

	strTemp.clear();
	strTemp = ui.txtVdms_6->toPlainText();
	DatosVDMSound["vdms_debug_2"] = strTemp.replace("\n", "%000d%000a"); 

	if(ui.chkVdms_3->isChecked())
	  DatosVDMSound["winnt_dosbox_1"] = "yes"; else DatosVDMSound["winnt_dosbox_1"] = "no";
	if(ui.chkVdms_4->isChecked())
	  DatosVDMSound["winnt_dosbox_2"] = "yes"; else DatosVDMSound["winnt_dosbox_2"] = "no";
	if(ui.chkVdms_5->isChecked())
	  DatosVDMSound["winnt_dosbox_3"] = "yes"; else DatosVDMSound["winnt_dosbox_3"] = "no";
	if(ui.chkVdms_6->isChecked())
	  DatosVDMSound["winnt_storage_1"] = "yes"; else DatosVDMSound["winnt_storage_1"] = "no";
	if(ui.chkVdms_7->isChecked()) 
	  DatosVDMSound["winnt_storage_2"] = "yes"; else DatosVDMSound["winnt_storage_2"] = "no";
	
}

void frmAddEditJuego::on_btnVdms_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir + "confvdms/", ui.txtVdms_1->text(), tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
		if(EditandoJuego != true)
		{
			if ( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
		}
		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
	  	str_ok = str.endsWith(".vlp");
		if(str_ok == false)
			str.append(".vlp");
		ui.txtVdms_1->setText( str );
	} else 
		ui.txtVdms_1->setText( "" );
}

void frmAddEditJuego::on_btnVdms_ExeJuego()
{
	ui.txtVdms_2->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtVdms_2->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnVdms_Icono()
{
	ui.txtVdms_4->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtVdms_4->text(), tr("Todos los archivo") + " (*)", 0, false) + ",0" );
}
// ---------------------------------------------------------------
