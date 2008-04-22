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
	connect( ui.cbxDatos_TipoEmu	, SIGNAL( activated(const QString &) ), this, SLOT( on_cbxDatos_TipoEmu_txtChanged(const QString &) ) );

	ui.cbxDatos_TipoEmu->setCurrentIndex( ui.cbxDatos_TipoEmu->findText( TipoEmulador ) );

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
	connect( ui.btnImgAbrir_Thumbs        , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_Thumbs() ) );
	connect( ui.btnImgAbrir_CoverFront    , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_CoverFront() ) );
	connect( ui.btnImgAbrir_CoverBack     , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_CoverBack() ) );
	connect( ui.btnImgVer_Thumbs          , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_Thumbs() ) );
	connect( ui.btnImgVer_CoverFront      , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_CoverFront() ) );
	connect( ui.btnImgVer_CoverBack       , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_CoverBack() ) );
	connect( ui.btnImgEliminar_Thumbs     , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_Thumbs() ) );
	connect( ui.btnImgEliminar_CoverFront , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_CoverFront() ) );
	connect( ui.btnImgEliminar_CoverBack  , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_CoverBack() ) );
	connect( ui.twDatoSmile , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twDatoSmile_Dblclicked(QTreeWidgetItem*) ));

	// Conecta los botones para editar los comentarios de los juegos
	connect( ui.btnTool_Cortar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Cortar() ) );
	connect( ui.btnTool_Copiar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Copiar() ) );
	connect( ui.btnTool_Pegar           , SIGNAL( clicked() ), this, SLOT( on_btnTool_Pegar() ) );
	connect( ui.btnTool_SelectAll       , SIGNAL( clicked() ), this, SLOT( on_btnTool_SelectAll() ) );
	connect( ui.btnTool_Deshacer        , SIGNAL( clicked() ), this, SLOT( on_btnTool_Deshacer() ) );
	connect( ui.btnTool_Rehacer         , SIGNAL( clicked() ), this, SLOT( on_btnTool_Rehacer() ) );
	connect( ui.btnTool_TextoNegrita    , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoNegrita() ) );
	connect( ui.btnTool_TextoCursiva    , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoCursiva() ) );
	connect( ui.btnTool_TextoSubrayado  , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoSubrayado() ) );
	connect( ui.btnTool_InsertarImg     , SIGNAL( clicked() ), this, SLOT( on_btnTool_InsertarImg() ) );
	connect( ui.btnTool_InsertaUrl      , SIGNAL( clicked() ), this, SLOT( on_btnTool_InsertaUrl() ) );
	connect( ui.btnTool_Buscar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Buscar() ) );
	connect( ui.btnTool_BuscarAnterior  , SIGNAL( clicked() ), this, SLOT( on_btnTool_BuscarAnterior() ) );
	connect( ui.btnTool_BuscarSiguiente , SIGNAL( clicked() ), this, SLOT( on_btnTool_BuscarSiguiente() ) );
	connect( ui.btnTool_Reemplazar      , SIGNAL( clicked() ), this, SLOT( on_btnTool_Reemplazar() ) );
	connect( ui.btnTool_Preview         , SIGNAL( clicked() ), this, SLOT( on_btnTool_Preview() ) );

	connect( ui.btnNuevaUrl    , SIGNAL( clicked() ), this, SLOT( on_btnNuevaUrl()    ) );
	connect( ui.btnEditarUrl   , SIGNAL( clicked() ), this, SLOT( on_btnEditarUrl()   ) );
	connect( ui.btnEliminarUrl , SIGNAL( clicked() ), this, SLOT( on_btnEliminarUrl() ) );
	connect( ui.btnAbrirUrl    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirUrl()    ) );

	connect( ui.btnDatosFiles_PathFile , SIGNAL( clicked() ), this, SLOT( on_btnDatosFiles_PathFile() ) );
	connect( ui.btnAddFile     , SIGNAL( clicked() ), this, SLOT( on_btnAddFile()     ) );
	connect( ui.btnEditFile    , SIGNAL( clicked() ), this, SLOT( on_btnEditFile()    ) );
	connect( ui.btnUpdateFile  , SIGNAL( clicked() ), this, SLOT( on_btnUpdateFile()  ) );
	connect( ui.btnDeleteFile  , SIGNAL( clicked() ), this, SLOT( on_btnDeleteFile()  ) );
	//----
	filters.clear();
	filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas
	fGrl.CargarIconosComboBox(stIconDir, ui.cbxDatos_Icono, filters);

	fGrl.CargarDatosComboBox(stDatosDir + "generos.txt"  , ui.cbxDatos_Genero , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_Compania , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_Desarrollador , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "tema.txt"     , ui.cbxDatos_Tema , 1, false);
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"    , ui.cbxDatos_Idioma , 1, true );
	fGrl.CargarDatosComboBox(stDatosDir + "formatos.txt" , ui.cbxDatos_Formato , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "fechas.txt"   , ui.cbxDatos_Anno , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "numdisc.txt"  , ui.cbxDatos_NumDisc , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "sistemaop.txt", ui.cbxDatos_SistemaOp, 1, false);

	for(int n=1; n<11; n++)
		ui.cbxDatos_Graficos->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));
	for(int n=1; n<11; n++)
		ui.cbxDatos_Sonido->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));
	for(int n=1; n<11; n++)
		ui.cbxDatos_Jugabilidad->addItem(QIcon(":/img16/grafica.png"),fGrl.IntToStr(n));

	ui.html_preview->setVisible(false);

	if(EditandoJuego == true)
		CargarDatosJuego( stItemIDGrl );
	else {
		ui.lb_fechahora->setText( fGrl.HoraFechaActual() );
		stThumbs = "";
		stCoverFront = "";
		stCoverBack = "";
		ui.lbImg_Thumbs->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.lbImg_CoverFront->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.lbImg_CoverBack->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_Thumbs->setEnabled( false );
		ui.btnImgVer_CoverFront->setEnabled( false );
		ui.btnImgVer_CoverBack->setEnabled( false );
		ui.btnImgEliminar_Thumbs->setEnabled( false );
		ui.btnImgEliminar_CoverFront->setEnabled( false );
		ui.btnImgEliminar_CoverBack->setEnabled( false );
		ui.cbxDatos_Genero->setCurrentIndex(0) ;
		ui.cbxDatos_Compania->setCurrentIndex(0) ;
		ui.cbxDatos_Desarrollador->setCurrentIndex(0) ;
		ui.cbxDatos_Tema->setCurrentIndex(0) ;
		ui.cbxDatos_Idioma->setCurrentIndex(0) ;
		ui.cbxDatos_Formato->setCurrentIndex(0) ;
		ui.cbxDatos_Anno->setCurrentIndex(0) ;
		ui.cbxDatos_NumDisc->setCurrentIndex(0) ;
		ui.cbxDatos_SistemaOp->setCurrentIndex(0);
		ui.cbxDatos_Graficos->setCurrentIndex(0);
		ui.cbxDatos_Sonido->setCurrentIndex(0);
		ui.cbxDatos_Jugabilidad->setCurrentIndex(0);
		ui.cbxDatos_Estado->setCurrentIndex(0);
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

	fGrl.CargarDatosListaSvm(":/datos/svm_lista.txt"		, ui.twScummVM						);	// Carga la lista de compatibilidad del ScummVM.
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"		, ui.cbxSvm_language	, 2, true	);	// Carga la lista de idiomas
	fGrl.CargarDatosComboBox(":/datos/svm_platform.txt"		, ui.cbxSvm_platform	, 2, false	);	// Carga la lista de platform
	fGrl.CargarDatosComboBox(":/datos/svm_gfxmode.txt"		, ui.cbxSvm_gfx_mode	, 3, false	);	// Carga la lista de gfxmode
	fGrl.CargarDatosComboBox(":/datos/svm_render_mode.txt"	, ui.cbxSvm_render_mode	, 1, false	);	// Carga la lista de render_mode
	fGrl.CargarDatosComboBox(":/datos/svm_music_driver.txt"	, ui.cbxSvm_music_driver, 1, false	);	// Carga la lista de music_driver

	ui.cbxSvm_language->setCurrentIndex(0);
	ui.cbxSvm_platform->setCurrentIndex(0);
	ui.cbxSvm_gfx_mode->setCurrentIndex(0);
	ui.cbxSvm_render_mode->setCurrentIndex(0);
	ui.cbxSvm_music_driver->setCurrentIndex(0);
	ui.cbxSvm_output_rate->setCurrentIndex(0);

	if( (EditandoJuego == true) && (TipoEmulador == "scummvm") )
	{
		ui.tabWidget_Datos->setTabEnabled (4, true ); // tab scummvm
		stItemIDSvm = sql->ItemIDIndex("dbgrl_emu_scummvm", stItemIDGrl); // Obtenemos el Index del ScummVM
		CargarDatosScummVM( stItemIDGrl );  // Carga los Datos del ScummVM.
	}
// Fin del ScummVM -----------------------------------------------

// Referente a DOSBox --------------------------------------------
	connect( ui.btnDbx_FileConfg  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_FileConfg()  ) );
	connect( ui.btnDbx_ExeJuego   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeJuego()   ) );
	connect( ui.btnDbx_ExeSetup   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeSetup()   ) );
	connect( ui.btnDirGravisUltraSound, SIGNAL( clicked() ), this, SLOT( on_btnDirGravisUltraSound() ) );
	connect( ui.btnDbx_mapperfile , SIGNAL( clicked() ), this, SLOT( on_btnDbx_mapperfile() ) );
	connect( ui.btnDbx_language   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_language()   ) );
	connect( ui.btnDbx_capturas   , SIGNAL( clicked() ), this, SLOT( on_btnDbx_capturas()   ) );
	connect( ui.btnDbx_musica     , SIGNAL( clicked() ), this, SLOT( on_btnDbx_musica()     ) );
	connect( ui.btnDbx_AddSerial  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_AddSerial()  ) );
	connect( ui.btnMount_Add      , SIGNAL( clicked() ), this, SLOT( on_btnMount_Add()      ) );
	connect( ui.btnMount_Edit     , SIGNAL( clicked() ), this, SLOT( on_btnMount_Edit()     ) );
	connect( ui.btnMount_Delete   , SIGNAL( clicked() ), this, SLOT( on_btnMount_Delete()   ) );
	connect( ui.btnMount_Clear    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Clear()    ) );
	connect( ui.btnMount_Subir    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Subir()    ) );
	connect( ui.btnMount_Bajar    , SIGNAL( clicked() ), this, SLOT( on_btnMount_Bajar()    ) );
	connect( ui.btnMount_AutoCrear, SIGNAL( clicked() ), this, SLOT( on_btnMount_AutoCrear()) );
	connect( ui.btnMount_Primario , SIGNAL( clicked() ), this, SLOT( on_btnMount_Primario() ) );
	connect( ui.cbxDbx_Profiles   , SIGNAL( activated(const QString &) ), this, SLOT( on_setProfileGame(const QString &) ) );

	fGrl.Cargar_Profile_DFend_ComboBox(stHomeDir + "templates/", ui.cbxDbx_Profiles);
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt"  , ui.cbxDbx_sdl_fullresolution , 1, false); // Resolucin pantalla
	fGrl.CargarDatosComboBox(":/datos/dbx_resolution.txt"  , ui.cbxDbx_sdl_windowresolution , 1, false); // windowresolution
	fGrl.CargarDatosComboBox(":/datos/dbx_output.txt"      , ui.cbxDbx_sdl_output , 1, false); // Modo de Renderizado
	fGrl.CargarDatosComboBox(":/datos/dbx_machine.txt"     , ui.cbxDbx_dosbox_machine , 1, false); // Tarjeta de Video
	fGrl.CargarDatosComboBox(":/datos/dbx_scaler.txt"      , ui.cbxDbx_render_scaler , 1, false); // Escalar y Filtros
	fGrl.CargarDatosComboBox(":/datos/dbx_hwscale.txt"     , ui.cbxDbx_sdl_hwscale , 1, false); // Escalar por hardware
	fGrl.CargarDatosComboBox(":/datos/dbx_sensitivity.txt" , ui.cbxDbx_sdl_sensitivity , 1, false); // Sensibilidad del ratn
	fGrl.CargarDatosComboBox(":/datos/dbx_memsize.txt"     , ui.cbxDbx_dosbox_memsize , 1, false); // Cantidad de memoria para DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cpu_core.txt"    , ui.cbxDbx_cpu_core , 1, false); // Ncleo de la CPU DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_frameskip.txt"   , ui.cbxDbx_render_frameskip, 1, false); // Frameskip DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_cycles.txt"      , ui.cbxDbx_cpu_cycles, 1, false); // Ciclos DOSBox
	fGrl.CargarDatosComboBox(":/datos/dbx_joystick.txt"    , ui.cbxDbx_joystick_type, 1, false); // Emulador de joystick
	fGrl.CargarDatosComboBox(":/datos/dbx_keyboardlayout.txt", ui.cbxDbx_dos_keyboardlayout,1, false);// keyboardlayout
	fGrl.CargarDatosComboBox(":/datos/dbx_sbtype.txt"      , ui.cbxDbx_sblaster_sbtype, 1, false); // Tipo Sound Blaste
	fGrl.CargarDatosComboBox(":/datos/dbx_sb_oplmode.txt"  , ui.cbxDbx_sblaster_oplmode, 1, false); // Sound Blaste Opl mode
	fGrl.CargarDatosComboBox(":/datos/dbx_mpu401.txt"      , ui.cbxDbx_midi_mpu401, 1, false); // MPU-401
	fGrl.CargarDatosComboBox(":/datos/dbx_midi_device.txt" , ui.cbxDbx_midi_device, 1, false); // MIDI Device

	// Ponemos los Combobox por defecto.
	ui.cbxDbx_sdl_fullresolution->setCurrentIndex( 0 );	// Resolucin pantalla
	ui.cbxDbx_sdl_windowresolution->setCurrentIndex( 0 );	// windowresolution
	ui.cbxDbx_sdl_output->setCurrentIndex( 0 );	// Modo de Renderizado
	ui.cbxDbx_dosbox_machine->setCurrentIndex( 0 );	// Tarjeta de Video
	ui.cbxDbx_render_scaler->setCurrentIndex( 1 );	// Escalar y Filtros
	ui.cbxDbx_sdl_hwscale->setCurrentIndex( 9 );	// Escalar por hardware
	ui.cbxDbx_sdl_sensitivity->setCurrentIndex( 9 );	// Sensibilidad del ratn
	ui.cbxDbx_dosbox_memsize->setCurrentIndex( 5 );	// Cantidad de memoria para DOSBox
	ui.cbxDbx_cpu_core->setCurrentIndex( 0 );	// Ncleo de la CPU DOSBox
	ui.cbxDbx_render_frameskip->setCurrentIndex( 0 );	// Frameskip DOSBox
	ui.cbxDbx_cpu_cycles->setCurrentIndex( 0 );	// Ciclos DOSBox
	ui.cbxDbx_joystick_type->setCurrentIndex( 0 );	// Emulador de joystick
	ui.cbxDbx_dos_keyboardlayout->setCurrentIndex( 14 ); // keyboardlayout
	ui.cbxDbx_sblaster_sbtype->setCurrentIndex( 5 ); // Tipo Sound Blaste
	ui.cbxDbx_sblaster_oplmode->setCurrentIndex( 0 ); // Sound Blaste Opl mode
	ui.cbxDbx_midi_mpu401->setCurrentIndex( 0 ); // MPU-401
	ui.cbxDbx_midi_device->setCurrentIndex( 0 ); // MIDI Device

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
	connect( ui.btnVdms_Icono    , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );

	if( (EditandoJuego == true) && (TipoEmulador == "vdmsound") )
	{
		ui.tabWidget_Datos->setTabEnabled (5, true ); // tab VDMSound
		stItemIDVdms = sql->ItemIDIndex("dbgrl_emu_vdmsound", stItemIDGrl); // Obtenemos el Index del ScummVM
		CargarDatosVDMSound( stItemIDGrl );
	}
// Fin del VDMSound ----------------------------------------------

	if( EditandoJuego == true )
	{
		ui.cbxDatos_TipoEmu->setEnabled( false );
		setWindowTitle( tr("Editando a") + ": " + ui.txtDatos_Titulo->text() );
	} else {
		ui.cbxDatos_TipoEmu->setEnabled( true );
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
	if(ui.txtDatos_Titulo->text()!="")
	{
		siguiente=true;
		if(ui.cbxDatos_TipoEmu->currentText()=="dosbox")
		{
			if( ui.txtDbx_path_conf->text() != "" )
			{
				siguiente = true;
				if( ui.txtDbx_path_exe->text() == "")
				{
					siguiente = false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " DOSBox");
				} else {
					siguiente = true;
					if(EditandoJuego != true)
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
		}
		if(ui.cbxDatos_TipoEmu->currentText()=="scummvm")
		{//IF de scummvm
			if(ui.txtSvm_game_label->text()=="")
			{
				siguiente=false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner la etiqueta del juego de")+ " ScummVM" );
			} else {
				siguiente=true;
				if(ui.txtSvm_path->text()=="")
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
		}//fin IF de scummvm
		if(ui.cbxDatos_TipoEmu->currentText()=="vdmsound")
		{
			if(ui.txtVdms_path_conf->text()!="")
			{
				siguiente = true;
				if(ui.txtVdms_path_exe->text()=="")
				{
					siguiente=false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " VDMSound");
				} else {
					siguiente = true;
					if(EditandoJuego != true)
					{
						QFile appConfg( stHomeDir + "confvdms/"+ ui.txtVdms_path_conf->text() );
						if( appConfg.exists() )
						{
							siguiente = false;
							ui.txtVdms_path_conf->setText("");
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
		if(ui.cbxDatos_TipoEmu->currentText()=="dosbox")
			setDatosDosBox();

		if(ui.cbxDatos_TipoEmu->currentText()=="scummvm")
			setDatosScummVM();

		if(ui.cbxDatos_TipoEmu->currentText()=="vdmsound")
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
	if( query.first() )
	{
		rec = query.record();
		ui.lb_fechahora->setText( query.value( rec.indexOf("fecha") ).toString() );						// fecha
		ui.txtDatos_Titulo->setText( query.value( rec.indexOf("titulo") ).toString() );					// titulo
		ui.txtDatos_Subtitulo->setText( query.value( rec.indexOf("subtitulo") ).toString() );			// subtitulo
		ui.txtDatos_Tamano->setText( query.value( rec.indexOf("tamano") ).toString() );					// tamano
		ui.txtDatos_Comentario->setPlainText( query.value( rec.indexOf("comentario") ).toString() );	// comentario

		ui.cbxDatos_Genero->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("genero") ).toString() );				// genero
		ui.cbxDatos_Compania->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("compania") ).toString() );			// compania
		ui.cbxDatos_Desarrollador->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("desarrollador") ).toString());	// desarrollador
		ui.cbxDatos_Tema->addItem( QIcon(":/img16/datos_3.png") , query.value( rec.indexOf("tema") ).toString() );					// tema
		ui.cbxDatos_Idioma->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("idioma") ).toString() );				// idioma
		ui.cbxDatos_Formato->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("formato") ).toString() );				// formato
		ui.cbxDatos_Anno->addItem( QIcon(":/img16/fecha.png") , query.value( rec.indexOf("anno") ).toString() );					// anno
		ui.cbxDatos_NumDisc->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("numdisc") ).toString() );				// numdisc
		ui.cbxDatos_SistemaOp->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("sistemaop") ).toString() );			// sistemaop
		ui.cbxDatos_Estado->addItem( QIcon(":/img16/sinimg.png") , query.value( rec.indexOf("estado") ).toString() );				// estado

// Selecciona el dato correspondiente.
		ui.cbxDatos_Genero->setCurrentIndex( ui.cbxDatos_Genero->findText( query.value( rec.indexOf("genero") ).toString() ) );						// genero
		ui.cbxDatos_Compania->setCurrentIndex( ui.cbxDatos_Compania->findText( query.value( rec.indexOf("compania") ).toString() ) );				// compania
		ui.cbxDatos_Desarrollador->setCurrentIndex( ui.cbxDatos_Desarrollador->findText( query.value( rec.indexOf("desarrollador") ).toString() ) );// desarrollador
		ui.cbxDatos_Tema->setCurrentIndex( ui.cbxDatos_Tema->findText( query.value( rec.indexOf("tema") ).toString() ) );							// tema
		ui.cbxDatos_Idioma->setCurrentIndex( ui.cbxDatos_Idioma->findText( query.value( rec.indexOf("idioma") ).toString() ) );						// idioma
		ui.cbxDatos_Formato->setCurrentIndex( ui.cbxDatos_Formato->findText( query.value( rec.indexOf("formato") ).toString() ) );					// formato
		ui.cbxDatos_Anno->setCurrentIndex( ui.cbxDatos_Anno->findText( query.value( rec.indexOf("anno") ).toString() ) );							// anno
		ui.cbxDatos_NumDisc->setCurrentIndex( ui.cbxDatos_NumDisc->findText( query.value( rec.indexOf("numdisc") ).toString() ) );					// numdisc
		ui.cbxDatos_SistemaOp->setCurrentIndex( ui.cbxDatos_SistemaOp->findText( query.value( rec.indexOf("sistemaop") ).toString() ) );			// sistemaop
		ui.cbxDatos_Graficos->setCurrentIndex( ui.cbxDatos_Graficos->findText( query.value( rec.indexOf("graficos") ).toString() ) );				// graficos
		ui.cbxDatos_Sonido->setCurrentIndex( ui.cbxDatos_Sonido->findText( query.value( rec.indexOf("sonido") ).toString() ) );						// sonido
		ui.cbxDatos_Jugabilidad->setCurrentIndex( ui.cbxDatos_Jugabilidad->findText( query.value( rec.indexOf("jugabilidad") ).toString() ) );		// jugabilidad
		ui.cbxDatos_Estado->setCurrentIndex( ui.cbxDatos_Estado->findText( query.value( rec.indexOf("estado") ).toString() ) );						// estado

		ui.chkDatos_Original->setChecked( query.value( rec.indexOf("original") ).toBool() );	// original
		ui.chkDatos_Favorito->setChecked( query.value( rec.indexOf("favorito") ).toBool() );	// favorito

		QString stIcono;
		stIcono.clear();
		stIcono = query.value( rec.indexOf("icono") ).toString();	// icono
		if( stIcono == "datos" || stIcono == "" )
			ui.cbxDatos_Icono->setCurrentIndex(0);
		else if( stIcono == "dosbox" )
			ui.cbxDatos_Icono->setCurrentIndex(1);
		else if( stIcono == "scummvm" )
			ui.cbxDatos_Icono->setCurrentIndex(2);
		else if( stIcono =="vdmsound" )
			ui.cbxDatos_Icono->setCurrentIndex(3);
		else
			ui.cbxDatos_Icono->setCurrentIndex( ui.cbxDatos_Icono->findText( stIcono ) );	// icono

		stThumbs.clear()	; // thumbs
		stCoverFront.clear(); // cover_front
		stCoverBack.clear()	; // cover_back
		stThumbs     = query.value( rec.indexOf("thumbs")     ).toString()	; // thumbs
		stCoverFront = query.value( rec.indexOf("cover_front")).toString()	; // cover_front
		stCoverBack  = query.value( rec.indexOf("cover_back") ).toString()	; // cover_back

		if( file_thumbs.exists(stThumbs) )
		{
			ui.lbImg_Thumbs->setPixmap( QPixmap(stThumbs) );
			ui.btnImgVer_Thumbs->setEnabled( true );
			ui.btnImgEliminar_Thumbs->setEnabled( true );
		} else {
			ui.lbImg_Thumbs->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_Thumbs->setEnabled( false );
			ui.btnImgEliminar_Thumbs->setEnabled( false );
		}
		if( file_cover_front.exists(stCoverFront) )
		{
			ui.lbImg_CoverFront->setPixmap( QPixmap(stCoverFront) );
			ui.btnImgVer_CoverFront->setEnabled( true );
			ui.btnImgEliminar_CoverFront->setEnabled( true );
		} else {
			ui.lbImg_CoverFront->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_CoverFront->setEnabled( false );
			ui.btnImgEliminar_CoverFront->setEnabled( false );
		}
		if( file_cover_back.exists(stCoverBack) )
		{
			ui.lbImg_CoverBack->setPixmap( QPixmap(stCoverBack) );
			ui.btnImgVer_CoverBack->setEnabled( true );
			ui.btnImgEliminar_CoverBack->setEnabled( true );
		} else {
			ui.lbImg_CoverBack->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
			ui.btnImgVer_CoverBack->setEnabled( false );
			ui.btnImgEliminar_CoverBack->setEnabled( false );
		}
	}

	ui.twDatosURL->clear();
	query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+stIDIndex);
	if(query.first())
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
			item_url->setText( 0 , query.value( rec.indexOf("url") ).toString()                 ); // url
			item_url->setIcon( 0 , QIcon(":/img16/edit_enlace.png")                             ); // icono
			item_url->setText( 1 , query.value( rec.indexOf("descripcion") ).toString()         ); // descripcion
			item_url->setText( 2 , fGrl.IntToStr( query.value( rec.indexOf("id") ).toInt() )    ); // id
			item_url->setText( 3 , fGrl.IntToStr( query.value( rec.indexOf("idgrl") ).toInt() ) ); // idgrl
		} while (query.next());
		ui.twDatosURL->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}

	ui.twDatosFiles->clear();
	query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+stIDIndex);
	if(query.first())
	{
		do {
			rec = query.record();
			QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twDatosFiles );
			item_files->setText( 0 , query.value( rec.indexOf("nombre") ).toString()              ); // nombre
			item_files->setIcon( 0 , QIcon(":/img16/importar.png")                                ); // icono
			item_files->setText( 1 , query.value( rec.indexOf("crc") ).toString()                 ); // crc
			item_files->setText( 2 , query.value( rec.indexOf("descripcion") ).toString()         ); // descripcion
			item_files->setText( 3 , query.value( rec.indexOf("size") ).toString()                ); // size
			item_files->setText( 4 , query.value( rec.indexOf("path") ).toString()                ); // path
			item_files->setText( 5 , fGrl.IntToStr( query.value( rec.indexOf("id") ).toInt() )    ); // id
			item_files->setText( 6 , fGrl.IntToStr( query.value( rec.indexOf("idgrl") ).toInt() ) ); // idgrl
		} while (query.next());
		ui.twDatosFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}
}

void frmAddEditJuego::setDatosJuegos()
{
	DatosJuego.clear();

	if( ui.cbxDatos_Icono->currentText()!="" )
		DatosJuego["icono"] = ui.cbxDatos_Icono->currentText();					// icono
	else DatosJuego["icono"] = ""+TipoEmulador;

	DatosJuego["titulo"]    = ui.txtDatos_Titulo->text();							// titulo
	DatosJuego["subtitulo"] = ui.txtDatos_Subtitulo->text();					// subtitulo

	if( ui.cbxDatos_Genero->currentText()!="" )
		DatosJuego["genero"] = ui.cbxDatos_Genero->currentText();				// genero
	else DatosJuego["genero"] = "";

	if( ui.cbxDatos_Compania->currentText()!="" )
		DatosJuego["compania"] = ui.cbxDatos_Compania->currentText();			// compania
	else DatosJuego["compania"] = "";

	if( ui.cbxDatos_Desarrollador->currentText()!="" )
		DatosJuego["desarrollador"] = ui.cbxDatos_Desarrollador->currentText();	// desarrollador
	else DatosJuego["desarrollador"] = "";

	if( ui.cbxDatos_Tema->currentText()!="" )
		DatosJuego["tema"] = ui.cbxDatos_Tema->currentText();					// tema
	else DatosJuego["tema"] = "";

	if( ui.cbxDatos_Idioma->currentText()!="" )
		DatosJuego["idioma"] = ui.cbxDatos_Idioma->currentText();				// idioma
	else DatosJuego["idioma"] = "";

	if( ui.cbxDatos_Formato->currentText()!="" )
		DatosJuego["formato"] = ui.cbxDatos_Formato->currentText();				// formato
	else DatosJuego["formato"] = "";

	if( ui.cbxDatos_Anno->currentText()!="" )
		DatosJuego["anno"] = ui.cbxDatos_Anno->currentText();					// anno
	else DatosJuego["anno"] = "";
	
	if( ui.cbxDatos_NumDisc->currentText()!="" )
		DatosJuego["numdisc"] = ui.cbxDatos_NumDisc->currentText();				// numdisc
	else DatosJuego["numdisc"] = "";
	
	if( ui.cbxDatos_SistemaOp->currentText()!="" )
		DatosJuego["sistemaop"] = ui.cbxDatos_SistemaOp->currentText();			// sistemaop
	else DatosJuego["sistemaop"] = "";

	DatosJuego["tamano"] = ui.txtDatos_Tamano->text();							// tamano

	if( ui.cbxDatos_Graficos->currentText()!="" )
		DatosJuego["graficos"] = ui.cbxDatos_Graficos->currentText();			// graficos
	else DatosJuego["graficos"] = "1";

	if( ui.cbxDatos_Sonido->currentText()!="" )
		DatosJuego["sonido"] = ui.cbxDatos_Sonido->currentText();				// sonido
	else DatosJuego["sonido"] = "1";

	if( ui.cbxDatos_Jugabilidad->currentText()!="" )
		DatosJuego["jugabilidad"] = ui.cbxDatos_Jugabilidad->currentText();		// jugabilidad
	else DatosJuego["jugabilidad"] = "1";

	if( ui.chkDatos_Original->isChecked() )
		DatosJuego["original"] = "true";								// original
	else DatosJuego["original"] = "false";

	if( ui.chkDatos_Favorito->isChecked() )
		DatosJuego["favorito"] = "true";								// favorito
	else DatosJuego["favorito"] = "false";

	if( ui.cbxDatos_Estado->currentText()!="" )
		DatosJuego["estado"] = ui.cbxDatos_Estado->currentText();		// estado
	else DatosJuego["estado"] = "";

	DatosJuego["thumbs"]      = stThumbs;								// thumbs
	DatosJuego["cover_front"] = stCoverFront;							// cover_front
	DatosJuego["cover_back"]  = stCoverBack;							// cover_back
	DatosJuego["fecha"]       = ui.lb_fechahora->text();				// fecha d/m/a h:m:s

	if( ui.cbxDatos_TipoEmu->currentText()!="" )
		DatosJuego["tipo_emu"] = ui.cbxDatos_TipoEmu->currentText();	// tipo_emu
	else DatosJuego["tipo_emu"] = ""+TipoEmulador;

	DatosJuego["comentario"]  = ui.txtDatos_Comentario->toPlainText();	// comentario
}

void frmAddEditJuego::on_btnImgAbrir_Thumbs()
{
	stThumbs = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stThumbs, stThumbs, tr("Todos los archivo") + " (*)", 0, false);
	if( file_thumbs.exists(stThumbs) )
	{
		ui.lbImg_Thumbs->setPixmap( QPixmap(stThumbs) );
		ui.btnImgVer_Thumbs->setEnabled( true );
		ui.btnImgEliminar_Thumbs->setEnabled( true );
	} else {
		ui.lbImg_Thumbs->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_Thumbs->setEnabled( false );
		ui.btnImgEliminar_Thumbs->setEnabled( false );
		stThumbs = "";
	}
}

void frmAddEditJuego::on_btnImgAbrir_CoverFront()
{
	stCoverFront = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stCoverFront, stCoverFront, tr("Todos los archivo") + " (*)", 0, false);
	if( file_cover_front.exists(stCoverFront) )
	{
		ui.lbImg_CoverFront->setPixmap( QPixmap(stCoverFront) );
		ui.btnImgVer_CoverFront->setEnabled( true );
		ui.btnImgEliminar_CoverFront->setEnabled( true );
	} else {
		ui.lbImg_CoverFront->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_CoverFront->setEnabled( false );
		ui.btnImgEliminar_CoverFront->setEnabled( false );
		stCoverFront = "";
	}
}

void frmAddEditJuego::on_btnImgAbrir_CoverBack()
{
	stCoverBack = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stCoverBack, stCoverBack, tr("Todos los archivo") + " (*)", 0, false);
	if( file_cover_back.exists(stCoverBack) )
	{
		ui.lbImg_CoverBack->setPixmap( QPixmap(stCoverBack) );
		ui.btnImgVer_CoverBack->setEnabled( true );
		ui.btnImgEliminar_CoverBack->setEnabled( true );
	} else {
		ui.lbImg_CoverBack->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		ui.btnImgVer_CoverBack->setEnabled( false );
		ui.btnImgEliminar_CoverBack->setEnabled( false );
		stCoverBack = "";
	}
}

void frmAddEditJuego::on_btnImgVer_Thumbs()
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

void frmAddEditJuego::on_btnImgVer_CoverFront()
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

void frmAddEditJuego::on_btnImgVer_CoverBack()
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

void frmAddEditJuego::on_btnImgEliminar_Thumbs()
{
	stThumbs = "";
	ui.lbImg_Thumbs->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_Thumbs->setEnabled( false );
	ui.btnImgEliminar_Thumbs->setEnabled( false );
}

void frmAddEditJuego::on_btnImgEliminar_CoverFront()
{
	stCoverFront = "";
	ui.lbImg_CoverFront->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_CoverFront->setEnabled( false );
	ui.btnImgEliminar_CoverFront->setEnabled( false );
}

void frmAddEditJuego::on_btnImgEliminar_CoverBack()
{
	stCoverBack = "";
	ui.lbImg_CoverBack->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
	ui.btnImgVer_CoverBack->setEnabled( false );
	ui.btnImgEliminar_CoverBack->setEnabled( false );
}

void frmAddEditJuego::on_cbxDatos_TipoEmu_txtChanged(const QString texto)
{
	if(texto == "datos")
		ui.cbxDatos_Icono->setCurrentIndex(0);

	if(texto == "dosbox")
	{
		ui.tabWidget_Datos->setTabEnabled (3, true );	// tab dosbox
		ui.cbxDatos_Icono->setCurrentIndex(1);
	} else
		ui.tabWidget_Datos->setTabEnabled (3, false );

	if(texto == "scummvm")
	{
		ui.tabWidget_Datos->setTabEnabled (4, true );	// tab scummvm
		ui.cbxDatos_Icono->setCurrentIndex(2) ;
	} else
		ui.tabWidget_Datos->setTabEnabled (4, false );

	if(texto == "vdmsound")
	{
		ui.tabWidget_Datos->setTabEnabled (5, true );	// tab vdmsound
		ui.cbxDatos_Icono->setCurrentIndex(3);
	} else
		ui.tabWidget_Datos->setTabEnabled (5, false );
}

void frmAddEditJuego::on_twDatoSmile_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ui.txtDatos_Comentario->textCursor().insertText( " " + item->text(0) + " " );
		// preview texto-html
		if( ui.btnTool_Preview->isChecked() )
			ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_Comentario->toPlainText(), listSmailes ) );
	} else
		return;
}

void frmAddEditJuego::on_btnTool_Cortar()
{
// cortar texto
	ui.txtDatos_Comentario->cut();
}

void frmAddEditJuego::on_btnTool_Copiar()
{
// copiar texto
	ui.txtDatos_Comentario->copy();
}

void frmAddEditJuego::on_btnTool_Pegar()
{
// pegar texto
	ui.txtDatos_Comentario->paste();
}

void frmAddEditJuego::on_btnTool_SelectAll()
{
// seleccionar el texto
	ui.txtDatos_Comentario->selectAll();
}

void frmAddEditJuego::on_btnTool_Deshacer()
{
// deshacer texto
	ui.txtDatos_Comentario->undo();
}

void frmAddEditJuego::on_btnTool_Rehacer()
{
// rehacer texto
	ui.txtDatos_Comentario->redo();
}

void frmAddEditJuego::on_btnTool_TextoNegrita()
{
// texto en negrita
	ui.txtDatos_Comentario->textCursor().insertText("<strong>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</strong>");
}

void frmAddEditJuego::on_btnTool_TextoCursiva()
{
// texto en cursiva
	ui.txtDatos_Comentario->textCursor().insertText("<em>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</em>");
}

void frmAddEditJuego::on_btnTool_TextoSubrayado()
{
// texto subrayado
	ui.txtDatos_Comentario->textCursor().insertText("<u>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</u>");
}

void frmAddEditJuego::on_btnTool_InsertarImg()
{
// insertar una imagen en el texto
	ui.txtDatos_Comentario->textCursor().insertText("<img src=\"direccion_imagen\" />");
}

void frmAddEditJuego::on_btnTool_InsertaUrl()
{
// insertar una url en el texto
	ui.txtDatos_Comentario->textCursor().insertText("<a href=\""+ui.txtDatos_Comentario->textCursor().selectedText()+"\">"+ui.txtDatos_Comentario->textCursor().selectedText()+"</a>");
}

void frmAddEditJuego::on_btnTool_Buscar()
{
// buscar texto

}

void frmAddEditJuego::on_btnTool_BuscarAnterior()
{
// buscar texto anterior

}

void frmAddEditJuego::on_btnTool_BuscarSiguiente()
{
// buscar texto siguiente

}

void frmAddEditJuego::on_btnTool_Reemplazar()
{
// reemlazar texto

}

void frmAddEditJuego::on_btnTool_Preview()
{
// preview texto-html
	if( ui.btnTool_Preview->isChecked() )
	{
		ui.txtDatos_Comentario->setVisible(false);
		ui.html_preview->setVisible(true);
		ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_Comentario->toPlainText(), listSmailes ) );
	} else {	
		ui.txtDatos_Comentario->setVisible(true);
		ui.html_preview->setVisible(false);
	}
}

void frmAddEditJuego::on_btnNuevaUrl()
{
	QString id_url;
	frmAddEditURL * AddEditURL = new frmAddEditURL();
	if( AddEditURL->exec() == QDialog::Accepted )
	{
		QHash<QString, QString> datos_url;
		datos_url.clear();
		datos_url["url"] = AddEditURL->ui.txt_addedit_url_1->currentText();			// 0
		datos_url["descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText();	// 1

		if(stItemIDGrl!="")
			id_url = sql->ItemInsertaUnURL( datos_url , stItemIDGrl );
		else
			id_url = "";

		QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
		item_url->setText( 0 , datos_url["url"]					); // url
		item_url->setIcon( 0 , QIcon(":/img16/edit_enlace.png")	); // icono
		item_url->setText( 1 , datos_url["descripcion"]			); // descripcion
		item_url->setText( 2 , id_url							); // id
		item_url->setText( 3 , stItemIDGrl						); // idgrl
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

		if( AddEditURL->exec() == QDialog::Accepted )
		{
			QHash<QString, QString> datos_url;
			datos_url.clear();
			datos_url["url"] = AddEditURL->ui.txt_addedit_url_1->currentText();			// 0
			datos_url["descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText();	// 1

			ui.twDatosURL->currentItem()->setText( 0 , datos_url["url"]					); // url
			ui.twDatosURL->currentItem()->setIcon( 0, QIcon(":/img16/edit_enlace.png")	); // icono
			ui.twDatosURL->currentItem()->setText( 1 , datos_url["descripcion"]			); // descripcion
			ui.twDatosURL->currentItem()->setText( 2 , id_url							); // id
			ui.twDatosURL->currentItem()->setText( 3 , stItemIDGrl						); // idgrl

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
			fGrl.DeleteItemTree( ui.twDatosURL->currentItem() );
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
	if( item )
	{
		my_url.clear();
		my_url = ui.twDatosURL->currentItem()->text(0);
		QDesktopServices::openUrl( my_url );
	}
}

void frmAddEditJuego::on_btnDatosFiles_PathFile()
{
	ui.txtDatosFiles_PathFile->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDatosFiles_PathFile->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnAddFile()
{
	QString id_file;
	
	if(ui.txtDatosFiles_PathFile->text()!="")
	{
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["nombre"] = ui.txtDatosFiles_FileName->text();				// 0 nombre
		datos_file["crc"] = ui.txtDatosFiles_Crc32->text();						// 1 crc32
		datos_file["descripcion"] = ui.txtDatosFiles_Comentario->toPlainText();	// 2 descripcion
		datos_file["path"] = ui.txtDatosFiles_PathFile->text();					// 3 path
		datos_file["size"] = ui.txtDatosFiles_Size->text();						// 4 size

		if(stItemIDGrl!="")
			id_file = sql->ItemInsertaUnFiles( datos_file , stItemIDGrl );
		else
			id_file = "";

		QTreeWidgetItem *item_file = new QTreeWidgetItem( ui.twDatosFiles );
		item_file->setText( 0 , datos_file["nombre"]			); // nombre
		item_file->setIcon( 0 , QIcon(":/img16/archivos.png")	); // icono
		item_file->setText( 1 , datos_file["crc"]				); // crc32
		item_file->setText( 2 , datos_file["descripcion"]		); // descripcion
		item_file->setText( 3 , datos_file["size"]				); // size
		item_file->setText( 4 , datos_file["path"]				); // path
		item_file->setText( 5 , id_file							); // id
		item_file->setText( 6 , stItemIDGrl						); // idgrl
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
		ui.txtDatosFiles_FileName->setText( ui.twDatosFiles->currentItem()->text(0)		); // 0 nombre
		ui.txtDatosFiles_Crc32->setText( ui.twDatosFiles->currentItem()->text(1)		); // 1 crc32
		ui.txtDatosFiles_Comentario->setText( ui.twDatosFiles->currentItem()->text(2)	); // 2 descripcion
		ui.txtDatosFiles_PathFile->setText( ui.twDatosFiles->currentItem()->text(4)		); // 3 path
		ui.txtDatosFiles_Size->setText( ui.twDatosFiles->currentItem()->text(3)			); // 4 size
	}
}

void frmAddEditJuego::on_btnUpdateFile()
{
	if( ui.twDatosFiles->isItemSelected( ui.twDatosFiles->currentItem() ) )
	{
		QString id_file;
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["nombre"]      = ui.txtDatosFiles_FileName->text();			// 0 nombre
		datos_file["crc"]         = ui.txtDatosFiles_Crc32->text();				// 1 crc32
		datos_file["descripcion"] = ui.txtDatosFiles_Comentario->toPlainText();	// 2 descripcion
		datos_file["path"]        = ui.txtDatosFiles_PathFile->text();			// 3 path
		datos_file["size"]        = ui.txtDatosFiles_Size->text();				// 4 size

		id_file = ui.twDatosFiles->currentItem()->text(5) ; // id 

		if(id_file!="")
			sql->ItemActualizaFiles( datos_file, id_file);

		ui.twDatosFiles->currentItem()->setText(0, datos_file["nombre"]		); // 0 nombre
		ui.twDatosFiles->currentItem()->setText(1, datos_file["crc"]		); // 1 crc32
		ui.twDatosFiles->currentItem()->setText(2, datos_file["descripcion"]); // 2 descripcion
		ui.twDatosFiles->currentItem()->setText(3, datos_file["size"]		); // 4 size
		ui.twDatosFiles->currentItem()->setText(4, datos_file["path"]		); // 3 path

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
			fGrl.DeleteItemTree( ui.twDatosFiles->currentItem() );
			if(id_file!="")
				sql->ItemEliminarFiles( id_file );
		}
	}
}

// ---------------------------------------------------------------
// Referente al ScummVM ------------------------------------------
void frmAddEditJuego::on_btnDirSvm_path()
{
	ui.txtSvm_path->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_path->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_save()
{
	ui.txtSvm_savepath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_savepath->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_extra()
{
	ui.txtSvm_extrapath->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_extrapath->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_capturas()
{
	ui.txtSvm_path_capturas->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_path_capturas->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_sonido()
{
	ui.txtSvm_path_sonido->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtSvm_path_sonido->text() ) );
}

void frmAddEditJuego::on_btnDirSvm_sound_font()
{
	ui.txtSvm_soundfont->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtSvm_soundfont->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::CargarDatosScummVM( QString stIDsvm )
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+stIDsvm+" LIMIT 0,1");
	if( query.first() )
	{
		rec = query.record();
		ui.txtSvm_game_label->setText( query.value( rec.indexOf("game") ).toString() ); // game

		QTreeWidgetItem *item;
		QList<QTreeWidgetItem *> found = ui.twScummVM->findItems(ui.txtSvm_game_label->text() , Qt::MatchExactly, 1);
		foreach (item, found)
			ui.twScummVM->setItemSelected(item, true);

		ui.cbxSvm_language->setCurrentIndex( ui.cbxSvm_language->findText( " - " + query.value( rec.indexOf("language") ).toString(), Qt::MatchContains ) );	// language
		ui.chkSvm_subtitles->setChecked( query.value( rec.indexOf("subtitles") ).toBool() )				; // subtitles
		ui.cbxSvm_platform->setCurrentIndex( ui.cbxSvm_platform->findText( " - " + query.value( rec.indexOf("platform") ).toString(), Qt::MatchContains ) );	// platform
		ui.cbxSvm_gfx_mode->setCurrentIndex( ui.cbxSvm_gfx_mode->findText( query.value( rec.indexOf("gfx_mode") ).toString(), Qt::MatchContains ) );			// gfx_mode
		ui.cbxSvm_render_mode->setCurrentIndex( ui.cbxSvm_render_mode->findText( query.value( rec.indexOf("render_mode") ).toString() ) );		// render_mode
		ui.chkSvm_fullscreen->setChecked( query.value( rec.indexOf("fullscreen") ).toBool() );													// fullscreen
		ui.chkSvm_aspect_ratio->setChecked( query.value( rec.indexOf("aspect_ratio") ).toBool() );												// aspect_ratio
		ui.txtSvm_path->setText( query.value( rec.indexOf("path") ).toString() );																// path
		ui.txtSvm_path_setup->setText( query.value( rec.indexOf("path_setup") ).toString() );													// path_setup
		ui.txtSvm_extrapath->setText( query.value( rec.indexOf("path_extra") ).toString() ); 													// path_extra
		ui.txtSvm_savepath->setText( query.value( rec.indexOf("path_save") ).toString() );														// path_save
		ui.txtSvm_path_capturas->setText( query.value( rec.indexOf("path_capturas") ).toString() );												// path_capturas
		ui.txtSvm_path_sonido->setText( query.value( rec.indexOf("path_sonido") ).toString() );													// path_sonido
		ui.cbxSvm_music_driver->setCurrentIndex( ui.cbxSvm_music_driver->findText( query.value( rec.indexOf("music_driver") ).toString()) );	// music_driver
		ui.chkSvm_enable_gs->setChecked( query.value( rec.indexOf("enable_gs") ).toBool() );													// enable_gs
		ui.chkSvm_multi_midi->setChecked( query.value( rec.indexOf("multi_midi") ).toBool() );													// multi_midi
		ui.chkSvm_native_mt32->setChecked( query.value( rec.indexOf("native_mt32") ).toBool() );												// native_mt32
		//"255"query.value( rec.indexOf("master_volume") ).toString();																			// master_volume
		ui.h_SliderSvm_music_volume->setSliderPosition(query.value( rec.indexOf("music_volume") ).toInt());										// music_volume
		ui.h_SliderSvm_sfx_volume->setSliderPosition(query.value( rec.indexOf("sfx_volume") ).toInt());											// sfx_volume
		ui.h_SliderSvm_speech_volume->setSliderPosition(query.value( rec.indexOf("speech_volume") ).toInt());									// speech_volume
		ui.h_SliderSvm_tempo->setSliderPosition(query.value( rec.indexOf("tempo") ).toInt());													// tempo
		ui.h_SliderSvm_talkspeed->setSliderPosition(query.value( rec.indexOf("talkspeed") ).toInt());											// talkspeed
		ui.h_SliderSvm_debuglevel->setSliderPosition(query.value( rec.indexOf("debuglevel") ).toInt());											// debuglevel
		//query.value( rec.indexOf("cdrom") ).toString()"0";																					// cdrom
		ui.cbxSvm_joystick_num->setCurrentIndex( query.value( rec.indexOf("joystick_num") ).toInt() );											// joystick_num 

		if(query.value( rec.indexOf("output_rate") ).toString()!="")
			ui.cbxSvm_output_rate->setCurrentIndex( ui.cbxSvm_output_rate->findText( query.value( rec.indexOf("output_rate") ).toString(), Qt::MatchContains ) ); // output_rate
		else
			ui.cbxSvm_output_rate->setCurrentIndex( 0 ); // output_rate

		ui.h_SliderSvm_midi_gain->setSliderPosition(query.value( rec.indexOf("midi_gain") ).toInt())	; // midi_gain
		ui.chkSvm_copy_protection->setChecked( query.value( rec.indexOf("copy_protection") ).toBool() )	; // copy_protection
		ui.txtSvm_soundfont->setText( query.value( rec.indexOf("sound_font") ).toString() )				; // sound_font 
	}
}

void frmAddEditJuego::setDatosScummVM()
{
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

	if( tmp_language == "" ) tmp_language = "";
	if( tmp_platform == "" ) tmp_platform = "";
	if( tmp_gfx_mode == "" ) tmp_gfx_mode = "";

	DatosScummvm.clear();
	DatosScummvm["game"] = ui.txtSvm_game_label->text();	// game
	DatosScummvm["language"] = tmp_language;				// language

	if( ui.chkSvm_subtitles->isChecked() ) DatosScummvm["subtitles"] = "true";
	else DatosScummvm["subtitles"] = "false";		// subtitles

	DatosScummvm["platform"] = tmp_platform;		// platform
	DatosScummvm["gfx_mode"] = tmp_gfx_mode;		// gfx_mode

	if( ui.cbxSvm_render_mode->currentText()!="" )
		DatosScummvm["render_mode"] = ui.cbxSvm_render_mode->currentText();	// render_mode
	else DatosScummvm["render_mode"] = "";

	if( ui.chkSvm_fullscreen->isChecked() )
		DatosScummvm["fullscreen"] = "true";								// fullscreen
	else DatosScummvm["fullscreen"] = "false";

	if( ui.chkSvm_aspect_ratio->isChecked() )
		DatosScummvm["aspect_ratio"] = "true";								// aspect_ratio
	else DatosScummvm["aspect_ratio"] = "false";

	DatosScummvm["path"]          = ui.txtSvm_path->text();					// path
	DatosScummvm["path_setup"]    = "";										// path_setup
	DatosScummvm["path_extra"]    = ui.txtSvm_extrapath->text();			// path_extra
	DatosScummvm["path_save"]     = ui.txtSvm_savepath->text();				// path_save
	DatosScummvm["path_capturas"] = ui.txtSvm_path_capturas->text();		// path_capturas
	DatosScummvm["path_sonido"]   = ui.txtSvm_path_sonido->text();			// path_sonido
	DatosScummvm["music_driver"]  = ui.cbxSvm_music_driver->currentText();	// music_driver

	if( ui.chkSvm_enable_gs->isChecked() )
		DatosScummvm["enable_gs"] = "true";							// enable_gs
	else DatosScummvm["enable_gs"] = "false";

	if( ui.chkSvm_multi_midi->isChecked() )
		DatosScummvm["multi_midi"] = "true";						// multi_midi
	else DatosScummvm["multi_midi"] = "false";

	if( ui.chkSvm_native_mt32->isChecked() )
		DatosScummvm["native_mt32"] = "true";						// native_mt32
	else DatosScummvm["native_mt32"] = "false";

	DatosScummvm["master_volume"] = "255";							// master_volume
	DatosScummvm["music_volume"]  = ui.posSliderSvm_1->text();		// music_volume
	DatosScummvm["sfx_volume"]    = ui.posSliderSvm_2->text();		// sfx_volume
	DatosScummvm["speech_volume"] = ui.posSliderSvm_3->text();		// speech_volume
	DatosScummvm["tempo"]         = ui.posSliderSvm_4->text();		// tempo
	DatosScummvm["talkspeed"]     = ui.posSliderSvm_5->text();		// talkspeed
	DatosScummvm["debuglevel"]    = ui.posSliderSvm_6->text();		// debuglevel
	DatosScummvm["cdrom"]         = "0";							// cdrom ui.cbxSvm_cdrom->currentIndex()
	DatosScummvm["joystick_num"]  = fGrl.IntToStr(ui.cbxSvm_joystick_num->currentIndex()); // joystick_num 

	if( ui.cbxSvm_output_rate->currentText()!="" && ui.cbxSvm_output_rate->currentIndex()!= 0 )
		DatosScummvm["output_rate"] = ui.cbxSvm_output_rate->currentText();
	else
		DatosScummvm["output_rate"] = "";

	if( ui.chkSvm_copy_protection->isChecked() )
		DatosScummvm["copy_protection"] = "true";					// copy_protection
	else DatosScummvm["copy_protection"] = "false";

	DatosScummvm["midi_gain"]  = ui.posSliderSvm_7->text();			// midi_gain 
	DatosScummvm["sound_font"] = ui.txtSvm_soundfont->text();		// sound_font 
}

void frmAddEditJuego::on_twScummVM_clicked( QTreeWidgetItem *item )
{
	ui.txtSvm_game_label->setText( item->text(1) );
}

void frmAddEditJuego::on_btnDefecto()
{
	ui.h_SliderSvm_music_volume->setValue(192);
	ui.h_SliderSvm_sfx_volume->setValue(192);
	ui.h_SliderSvm_speech_volume->setValue(192);
	ui.h_SliderSvm_tempo->setValue(100);
	ui.h_SliderSvm_talkspeed->setValue(60);
	ui.h_SliderSvm_debuglevel->setValue(0);
	ui.h_SliderSvm_midi_gain->setValue(100);
}
// ---------------------------------------------------------------
// Referente al DOSBox -------------------------------------------
void frmAddEditJuego::CargarDatosDosBox( QString stIDdbx )
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stIDdbx+" LIMIT 0,1");
	if( query.first() )
	{
		rec = query.record();
//0 id
//1 idgrl
		ui.chkDbx_sdl_fullscreen->setChecked( query.value( rec.indexOf("sdl_fullscreen") ).toBool() );										// sdl_fullscreen
		ui.chkDbx_sdl_fulldouble->setChecked(query.value( rec.indexOf("sdl_fulldouble") ).toBool() );										// sdl_fulldouble
		ui.chkDbx_sdl_fullfixed->setChecked( query.value( rec.indexOf("sdl_fullfixed") ).toBool() );										// sdl_fullfixed
		ui.cbxDbx_sdl_fullresolution->setCurrentIndex( ui.cbxDbx_sdl_fullresolution->findText( query.value( rec.indexOf("sdl_fullresolution" )).toString() ) );	// sdl_fullresolution
		ui.cbxDbx_sdl_windowresolution->setCurrentIndex( ui.cbxDbx_sdl_windowresolution->findText( query.value( rec.indexOf("sdl_windowresolution") ).toString() ) );	// sdl_windowresolution
		ui.cbxDbx_sdl_output->setCurrentIndex( ui.cbxDbx_sdl_output->findText( query.value( rec.indexOf("sdl_output") ).toString() ) );			// sdl_output
		ui.cbxDbx_sdl_hwscale->setCurrentIndex( ui.cbxDbx_sdl_hwscale->findText( query.value( rec.indexOf("sdl_hwscale") ).toString() ) );			// sdl_hwscale
		ui.chkDbx_sdl_autolock->setChecked( query.value( rec.indexOf("sdl_autolock") ).toBool() );											// sdl_autolock
		ui.cbxDbx_sdl_sensitivity->setCurrentIndex( ui.cbxDbx_sdl_sensitivity->findText( query.value( rec.indexOf("sdl_sensitivity") ).toString() ) );		// sdl_sensitivity
		ui.chkDbx_sdl_waitonerror->setChecked( query.value( rec.indexOf("sdl_waitonerror") ).toBool() );										// sdl_waitonerror
	QString sdl_priority_temp;
	QStringList sdl_priority_templist;
		sdl_priority_temp     = query.value( rec.indexOf("sdl_priority") ).toString();
		sdl_priority_templist = sdl_priority_temp.split(",");
		if( sdl_priority_templist.value(0) == "lower"  ) ui.rbtPrioridadBaja->setChecked(true);		// sdl_priority
		if( sdl_priority_templist.value(0) == "normal" ) ui.rbtPrioridadNormal->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(0) == "higher" ) ui.rbtPrioridadAlta->setChecked(true);		// sdl_priority
		if( sdl_priority_templist.value(0) == "highest") ui.rbtPrioridadMuyAlt->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "lower"  ) ui.rbtNoPrioridadBaja->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "normal" ) ui.rbtNoPrioridadNormal->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "higher" ) ui.rbtNoPrioridadAlta->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "highest") ui.rbtNoPrioridadMuyAlt->setChecked(true);	// sdl_priority

		ui.txtDbx_sdl_mapperfile->setText( query.value( rec.indexOf("sdl_mapperfile") ).toString() );										// sdl_mapperfile
		ui.chkDbx_sdl_usescancodes->setChecked( query.value( rec.indexOf("sdl_usescancodes") ).toBool() );										// sdl_usescancodes

		ui.txtDbx_dosbox_language->setText( query.value( rec.indexOf("dosbox_language") ).toString() );										// dosbox_language
		ui.cbxDbx_dosbox_machine->setCurrentIndex( ui.cbxDbx_dosbox_machine->findText( query.value( rec.indexOf("dosbox_machine") ).toString() ) );		// dosbox_machine
		ui.txtDbx_dosbox_captures->setText( query.value( rec.indexOf("dosbox_captures") ).toString() );										// dosbox_captures
		ui.cbxDbx_dosbox_memsize->setCurrentIndex( ui.cbxDbx_dosbox_memsize->findText( query.value( rec.indexOf("dosbox_memsize") ).toString() ) );		// dosbox_memsize

		ui.cbxDbx_render_frameskip->setCurrentIndex( ui.cbxDbx_render_frameskip->findText( query.value( rec.indexOf("render_frameskip") ).toString() ));	// render_frameskip
		ui.chkDbx_render_aspect->setChecked( query.value( rec.indexOf("render_aspect") ).toBool() );										// render_aspect
		ui.cbxDbx_render_scaler->setCurrentIndex( ui.cbxDbx_render_scaler->findText( query.value( rec.indexOf("render_scaler") ).toString() ) );		// render_scaler

		ui.cbxDbx_cpu_core->setCurrentIndex( ui.cbxDbx_cpu_core->findText( query.value( rec.indexOf("cpu_core") ).toString() ) );				// cpu_core
		ui.cbxDbx_cpu_cycles->setCurrentIndex( ui.cbxDbx_cpu_cycles->findText( query.value( rec.indexOf("cpu_cycles") ).toString() ));			// cpu_cycles
		ui.spinDbx_1->setValue( query.value( rec.indexOf("cpu_cycleup") ).toInt() );											// cpu_cycleup
		ui.spinDbx_2->setValue( query.value( rec.indexOf("cpu_cycledown") ).toInt() );											// cpu_cycledown

		ui.chkDbx_mixer_nosound->setChecked( query.value( rec.indexOf("mixer_nosound") ).toBool() );										// mixer_nosound
		ui.cbxDbx_mixer_rate->setCurrentIndex( ui.cbxDbx_mixer_rate->findText( query.value( rec.indexOf("mixer_rate") ).toString() ));			// mixer_rate
		ui.txtDbx_mixer_blocksize->setText( query.value( rec.indexOf("mixer_blocksize") ).toString() );										// mixer_blocksize
		ui.txtDbx_mixer_prebuffer->setText( query.value( rec.indexOf("mixer_prebuffer") ).toString() );										// mixer_prebuffer

		ui.cbxDbx_midi_mpu401->setCurrentIndex( ui.cbxDbx_midi_mpu401->findText( query.value( rec.indexOf("midi_mpu401") ).toString() ));			// midi_mpu401
		ui.chkDbx_midi_intelligent->setChecked( query.value( rec.indexOf("midi_intelligent") ).toBool() );									// midi_intelligent
		ui.cbxDbx_midi_device->setCurrentIndex( ui.cbxDbx_midi_device->findText( query.value( rec.indexOf("midi_device") ).toString() ));			// midi_device
		ui.txtDbx_midi_config->setText( query.value( rec.indexOf("midi_config") ).toString() );											// midi_config
		ui.cbxDbx_midi_mt32rate->setCurrentIndex( ui.cbxDbx_midi_mt32rate->findText( query.value( rec.indexOf("midi_mt32rate") ).toString() ));		// midi_mt32rate

		ui.cbxDbx_sblaster_sbtype->setCurrentIndex( ui.cbxDbx_sblaster_sbtype->findText( query.value( rec.indexOf("sblaster_sbtype") ).toString() ));		// sblaster_sbtype
		ui.cbxDbx_sblaster_sbbase->setCurrentIndex( ui.cbxDbx_sblaster_sbbase->findText( query.value( rec.indexOf("sblaster_sbbase") ).toString() ));		// sblaster_sbbase
		ui.cbxDbx_sblaster_irq->setCurrentIndex( ui.cbxDbx_sblaster_irq->findText( query.value( rec.indexOf("sblaster_irq") ).toString() ));		// sblaster_irq
		ui.cbxDbx_sblaster_dma->setCurrentIndex( ui.cbxDbx_sblaster_dma->findText( query.value( rec.indexOf("sblaster_dma") ).toString() ));		// sblaster_dma
		ui.cbxDbx_sblaster_hdma->setCurrentIndex( ui.cbxDbx_sblaster_hdma->findText( query.value( rec.indexOf("sblaster_hdma") ).toString() ));		// sblaster_hdma
		ui.chkDbx_sblaster_mixer->setChecked( query.value( rec.indexOf("sblaster_mixer") ).toBool() );										// sblaster_mixer
		ui.cbxDbx_sblaster_oplmode->setCurrentIndex( ui.cbxDbx_sblaster_oplmode->findText( query.value( rec.indexOf("sblaster_oplmode") ).toString() ));	// sblaster_oplmode
		ui.cbxDbx_sblaster_oplrate->setCurrentIndex( ui.cbxDbx_sblaster_oplrate->findText( query.value( rec.indexOf("sblaster_oplrate") ).toString() ));	// sblaster_oplrate

		ui.chkDbx_gus_gus->setChecked( query.value( rec.indexOf("gus_gus") ).toBool() );												// gus_gus
		ui.cbxDbx_gus_gusrate->setCurrentIndex( ui.cbxDbx_gus_gusrate->findText( query.value( rec.indexOf("gus_gusrate") ).toString() ));			// gus_gusrate
		ui.cbxDbx_gus_gusbase->setCurrentIndex( ui.cbxDbx_gus_gusbase->findText( query.value( rec.indexOf("gus_gusbase") ).toString() ));			// gus_gusbase
		ui.cbxDbx_gus_irq1->setCurrentIndex( ui.cbxDbx_gus_irq1->findText( query.value( rec.indexOf("gus_irq1") ).toString() ));			// gus_irq1
		ui.cbxDbx_gus_irq2->setCurrentIndex( ui.cbxDbx_gus_irq2->findText( query.value( rec.indexOf("gus_irq2") ).toString() ));			// gus_irq2
		ui.cbxDbx_gus_dma1->setCurrentIndex( ui.cbxDbx_gus_dma1->findText( query.value( rec.indexOf("gus_dma1") ).toString() ));			// gus_dma1
		ui.cbxDbx_gus_dma2->setCurrentIndex( ui.cbxDbx_gus_dma2->findText( query.value( rec.indexOf("gus_dma2") ).toString() ));			// gus_dma2
		ui.txtDbx_gus_ultradir->setText( query.value( rec.indexOf("gus_ultradir") ).toString() );											// gus_ultradir

		ui.chkDbx_speaker_pcspeaker->setChecked( query.value( rec.indexOf("speaker_pcspeaker") ).toBool() );									// speaker_pcspeaker
		ui.cbxDbx_speaker_pcrate->setCurrentIndex( ui.cbxDbx_speaker_pcrate->findText( query.value( rec.indexOf("speaker_pcrate") ).toString() ));		// speaker_pcrate
		ui.cbxDbx_speaker_tandy->setCurrentIndex( ui.cbxDbx_speaker_tandy->findText( query.value( rec.indexOf("speaker_tandy") ).toString() ));		// speaker_tandy
		ui.cbxDbx_speaker_tandyrate->setCurrentIndex( ui.cbxDbx_speaker_tandyrate->findText( query.value( rec.indexOf("speaker_tandyrate") ).toString() ));	// speaker_tandyrate
		ui.chkDbx_speaker_disney->setChecked( query.value( rec.indexOf("speaker_disney") ).toBool() );										// speaker_disney

		ui.cbxDbx_joystick_type->setCurrentIndex( ui.cbxDbx_joystick_type->findText( query.value( rec.indexOf("joystick_type") ).toString() ));		// joystick_type
		ui.chkDbx_joystick_timed->setChecked( query.value( rec.indexOf("joystick_timed") ).toBool() );										// joystick_timed
		ui.chkDbx_joystick_autofire->setChecked( query.value( rec.indexOf("joystick_autofire") ).toBool() );									// joystick_autofire
		ui.chkDbx_joystick_swap34->setChecked( query.value( rec.indexOf("joystick_swap34") ).toBool() );										// joystick_swap34
		ui.chkDbx_joystick_buttonwrap->setChecked( query.value( rec.indexOf("joystick_buttonwrap") ).toBool() );									// joystick_buttonwrap

		ui.chkDbx_modem_modem->setChecked( query.value( rec.indexOf("modem_modem") ).toBool() );											// modem_modem
		ui.cbxDbx_modem_comport->setCurrentIndex( query.value( rec.indexOf("modem_comport") ).toInt()-1 );									// modem_comport
		ui.txtDbx_modem_listenport->setText( query.value( rec.indexOf("modem_listenport") ).toString() );										// modem_listenport

		ui.chkDbx_dserial_directserial->setChecked( query.value( rec.indexOf("dserial_directserial") ).toBool() );								// dserial_directserial
		ui.cbxDbx_dserial_comport->setCurrentIndex( ui.cbxDbx_dserial_comport->findText( query.value( rec.indexOf("dserial_comport") ).toString() ));		// dserial_comport
		ui.cbxDbx_dserial_realport->setCurrentIndex( ui.cbxDbx_dserial_realport->findText( query.value( rec.indexOf("dserial_realport") ).toString() ));	// serial_realport
		ui.cbxDbx_dserial_defaultbps->setCurrentIndex( ui.cbxDbx_dserial_defaultbps->findText( query.value( rec.indexOf("dserial_defaultbps") ).toString() ));	// dserial_defaultbps
		ui.cbxDbx_dserial_parity->setCurrentIndex( ui.cbxDbx_dserial_parity->findText( query.value( rec.indexOf("dserial_parity") ).toString() ));		// dserial_parity
		ui.cbxDbx_dserial_bytesize->setCurrentIndex( ui.cbxDbx_dserial_bytesize->findText( query.value( rec.indexOf("dserial_bytesize") ).toString() ));	// dserial_bytesize
		ui.cbxDbx_dserial_stopbit->setCurrentIndex( ui.cbxDbx_dserial_stopbit->findText( query.value( rec.indexOf("dserial_stopbit") ).toString() ));		// dserial_stopbit
		ui.txtDbx_serial_1->setText( query.value( rec.indexOf("serial_1") ).toString() );												// serial_1
		ui.txtDbx_serial_2->setText( query.value( rec.indexOf("serial_2") ).toString() );												// serial_2
		ui.txtDbx_serial_3->setText( query.value( rec.indexOf("serial_3") ).toString() );												// serial_3
		ui.txtDbx_serial_4->setText( query.value( rec.indexOf("serial_4") ).toString() );												// serial_4

		ui.chkDbx_dos_xms->setChecked( query.value( rec.indexOf("dos_xms") ).toBool() );												// dos_xms
		ui.chkDbx_dos_ems->setChecked( query.value( rec.indexOf("dos_ems") ).toBool() );												// dos_ems
		ui.cbxDbx_dos_umb->setCurrentIndex( ui.cbxDbx_dos_umb->findText( query.value( rec.indexOf("dos_umb") ).toString() ));				// dos_umb
		ui.cbxDbx_dos_keyboardlayout->setCurrentIndex( ui.cbxDbx_dos_keyboardlayout->findText( query.value( rec.indexOf("dos_keyboardlayout") ).toString() ));	// dos_keyboardlayout

		ui.chkDbx_ipx_ipx->setChecked( query.value( rec.indexOf("ipx_ipx") ).toBool() );												// ipx_ipx

		ui.txtDbx_Autoexec->setText(query.value( rec.indexOf("autoexec") ).toString() );										// autoexec
		ui.chkDbx_autoexec->setChecked( query.value( rec.indexOf("opt_autoexec") ).toBool() );										// opt_autoexec

		ui.chkDbx_loadfix->setChecked( query.value( rec.indexOf("opt_loadfix") ).toBool() );											// opt_loadfix
		ui.txtDbx_loadfix_mem->setText( query.value( rec.indexOf("opt_loadfix_mem") ).toString()  );										// opt_loadfix_mem
		ui.chkDbx_consola_dbox->setChecked( query.value( rec.indexOf("opt_consola_dbox") ).toBool() );										// opt_consola_dbox
		ui.chkDbx_cerrar_dbox->setChecked( query.value( rec.indexOf("opt_cerrar_dbox") ).toBool() );										// opt_cerrar_dbox
		ui.chkDbx_cycle_sincronizar->setChecked( query.value( rec.indexOf("opt_cycle_sincronizar") ).toBool() );								// opt_cycle_sincronizar

		ui.txtDbx_path_conf->setText( query.value( rec.indexOf("path_conf") ).toString() );												// path_conf
		ui.txtDbx_path_sonido->setText( query.value( rec.indexOf("path_sonido") ).toString());											// path_sonido
		ui.txtDbx_path_exe->setText( query.value( rec.indexOf("path_exe") ).toString() );												// path_exe
		ui.txtDbx_path_setup->setText( query.value( rec.indexOf("path_setup") ).toString() );											// path_setup
		ui.txtDbx_parametros_exe->setText( query.value( rec.indexOf("parametros_exe") ).toString() );										// parametros_exe
		ui.txtDbx_parametros_setup->setText( query.value( rec.indexOf("parametros_setup") ).toString() );										// parametros_setup
	}
}

void frmAddEditJuego::on_setProfileGame(const QString ProfileGame)
{
	if(!ProfileGame.isEmpty() || ProfileGame!="<defecto>")
	{
		QStringList str_Lista, sdl_priority_templist;
		QString strProfile, tipoDrive, temp_opt_mount;

		strProfile.clear();
		strProfile = stHomeDir + "templates/" + ProfileGame + ".prof";

		TempProfileDosBox.clear();
		TempProfileDosBox = fGrl.Importar_Profile_DFend( strProfile );

		ui.chkDbx_sdl_fullscreen->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_fullscreen"] ) );												// sdl_fullscreen
		ui.chkDbx_sdl_fulldouble->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_fulldouble"] ) );												// sdl_fulldouble
		ui.chkDbx_sdl_fullfixed->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_fullfixed"] ) );												// sdl_fullfixed
		ui.cbxDbx_sdl_fullresolution->setCurrentIndex( ui.cbxDbx_sdl_fullresolution->findText( TempProfileDosBox["sdl_fullresolution"] ) );			// sdl_fullresolution
		ui.cbxDbx_sdl_windowresolution->setCurrentIndex( ui.cbxDbx_sdl_windowresolution->findText( TempProfileDosBox["sdl_windowresolution"] ) );	// sdl_windowresolution
		ui.cbxDbx_sdl_output->setCurrentIndex( ui.cbxDbx_sdl_output->findText( TempProfileDosBox["sdl_output"] ) );									// sdl_output
		ui.cbxDbx_sdl_hwscale->setCurrentIndex( ui.cbxDbx_sdl_hwscale->findText( TempProfileDosBox["sdl_hwscale"] ) );								// sdl_hwscale
		ui.chkDbx_sdl_autolock->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_autolock"] ) );													// sdl_autolock
		ui.cbxDbx_sdl_sensitivity->setCurrentIndex( ui.cbxDbx_sdl_sensitivity->findText( TempProfileDosBox["sdl_sensitivity"] ) );					// sdl_sensitivity
		ui.chkDbx_sdl_waitonerror->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_waitonerror"] ) );											// sdl_waitonerror

		sdl_priority_templist.clear();
		sdl_priority_templist = TempProfileDosBox["sdl_priority"].split(",");

		if( sdl_priority_templist.value(0) == "lower"  ) ui.rbtPrioridadBaja->setChecked(true);		// sdl_priority
		if( sdl_priority_templist.value(0) == "normal" ) ui.rbtPrioridadNormal->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(0) == "higher" ) ui.rbtPrioridadAlta->setChecked(true);		// sdl_priority
		if( sdl_priority_templist.value(0) == "highest") ui.rbtPrioridadMuyAlt->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "lower"  ) ui.rbtNoPrioridadBaja->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "normal" ) ui.rbtNoPrioridadNormal->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "higher" ) ui.rbtNoPrioridadAlta->setChecked(true);	// sdl_priority
		if( sdl_priority_templist.value(1) == "highest") ui.rbtNoPrioridadMuyAlt->setChecked(true);	// sdl_priority
	
		ui.txtDbx_sdl_mapperfile->setText( TempProfileDosBox["sdl_mapperfile"] );														// sdl_mapperfile
		ui.chkDbx_sdl_usescancodes->setChecked( fGrl.StrToBool( TempProfileDosBox["sdl_usescancodes"] ) );								// sdl_usescancodes
	
		ui.txtDbx_dosbox_language->setText( TempProfileDosBox["dosbox_language"] );														// dosbox_language
		ui.cbxDbx_dosbox_machine->setCurrentIndex( ui.cbxDbx_dosbox_machine->findText( TempProfileDosBox["dosbox_machine"] ) );			// dosbox_machine
		ui.txtDbx_dosbox_captures->setText( TempProfileDosBox["dosbox_captures"] );														// dosbox_captures
	
		ui.cbxDbx_dosbox_memsize->setEditText( TempProfileDosBox["dosbox_memsize"] );
	
		ui.cbxDbx_render_frameskip->setCurrentIndex( ui.cbxDbx_render_frameskip->findText( TempProfileDosBox["render_frameskip"] ) );	// render_frameskip
		ui.chkDbx_render_aspect->setChecked( fGrl.StrToBool( TempProfileDosBox["render_aspect"] ) );									// render_aspect
		ui.cbxDbx_render_scaler->setCurrentIndex( ui.cbxDbx_render_scaler->findText( TempProfileDosBox["render_scaler"] ) );			// render_scaler
	
		ui.cbxDbx_cpu_core->setCurrentIndex( ui.cbxDbx_cpu_core->findText( TempProfileDosBox["cpu_core"] ) );							// cpu_core
		ui.cbxDbx_cpu_cycles->setCurrentIndex( ui.cbxDbx_cpu_cycles->findText( TempProfileDosBox["cpu_cycles"] ) );						// cpu_cycles
		ui.spinDbx_1->setValue( fGrl.StrToInt( TempProfileDosBox["cpu_cycleup"] ) );													// cpu_cycleup
		ui.spinDbx_2->setValue( fGrl.StrToInt( TempProfileDosBox["cpu_cycledown"] ) );													// cpu_cycledown
	
		ui.chkDbx_mixer_nosound->setChecked( fGrl.StrToBool( TempProfileDosBox["mixer_nosound"] ) );									// mixer_nosound
		ui.cbxDbx_mixer_rate->setCurrentIndex( ui.cbxDbx_mixer_rate->findText( TempProfileDosBox["mixer_rate"] ) );						// mixer_rate
		ui.txtDbx_mixer_blocksize->setText( TempProfileDosBox["mixer_blocksize"] );														// mixer_blocksize
		ui.txtDbx_mixer_prebuffer->setText( TempProfileDosBox["mixer_prebuffer"] );														// mixer_prebuffer
	
		ui.cbxDbx_midi_mpu401->setCurrentIndex( ui.cbxDbx_midi_mpu401->findText( TempProfileDosBox["midi_mpu401"] ) );					// midi_mpu401
		ui.chkDbx_midi_intelligent->setChecked( fGrl.StrToBool( TempProfileDosBox["midi_intelligent"] ) );								// midi_intelligent
		ui.cbxDbx_midi_device->setCurrentIndex( ui.cbxDbx_midi_device->findText( TempProfileDosBox["midi_device"] ) );					// midi_device
		ui.txtDbx_midi_config->setText( TempProfileDosBox["midi_config"] );																// midi_config
		ui.cbxDbx_midi_mt32rate->setCurrentIndex( ui.cbxDbx_midi_mt32rate->findText( TempProfileDosBox["midi_mt32rate"] ) );				// midi_mt32rate
	
		ui.cbxDbx_sblaster_sbtype->setCurrentIndex( ui.cbxDbx_sblaster_sbtype->findText( TempProfileDosBox["sblaster_sbtype"] ) );		// sblaster_sbtype
		ui.cbxDbx_sblaster_sbbase->setCurrentIndex( ui.cbxDbx_sblaster_sbbase->findText( TempProfileDosBox["sblaster_sbbase"] ) );		// sblaster_sbbase
		ui.cbxDbx_sblaster_irq->setCurrentIndex( ui.cbxDbx_sblaster_irq->findText( TempProfileDosBox["sblaster_irq"] ) );				// sblaster_irq
		ui.cbxDbx_sblaster_dma->setCurrentIndex( ui.cbxDbx_sblaster_dma->findText( TempProfileDosBox["sblaster_dma"] ) );				// sblaster_dma
		ui.cbxDbx_sblaster_hdma->setCurrentIndex( ui.cbxDbx_sblaster_hdma->findText( TempProfileDosBox["sblaster_hdma"] ) );				// sblaster_hdma
		ui.chkDbx_sblaster_mixer->setChecked( fGrl.StrToBool( TempProfileDosBox["sblaster_mixer"] ) );									// sblaster_mixer
		ui.cbxDbx_sblaster_oplmode->setCurrentIndex( ui.cbxDbx_sblaster_oplmode->findText( TempProfileDosBox["sblaster_oplmode"] ) );	// sblaster_oplmode
		ui.cbxDbx_sblaster_oplrate->setCurrentIndex( ui.cbxDbx_sblaster_oplrate->findText( TempProfileDosBox["sblaster_oplrate"] ) );	// sblaster_oplrate
	
		ui.chkDbx_gus_gus->setChecked( fGrl.StrToBool( TempProfileDosBox["gus_gus"] ) );												// gus_gus
		ui.cbxDbx_gus_gusrate->setCurrentIndex( ui.cbxDbx_gus_gusrate->findText( TempProfileDosBox["gus_gusrate"] ) );					// gus_gusrate
		ui.cbxDbx_gus_gusbase->setCurrentIndex( ui.cbxDbx_gus_gusbase->findText( TempProfileDosBox["gus_gusbase"] ) );					// gus_gusbase
		ui.cbxDbx_gus_irq1->setCurrentIndex( ui.cbxDbx_gus_irq1->findText( TempProfileDosBox["gus_irq1"] ) );							// gus_irq1
		ui.cbxDbx_gus_irq2->setCurrentIndex( ui.cbxDbx_gus_irq2->findText( TempProfileDosBox["gus_irq2"] ) );							// gus_irq2
		ui.cbxDbx_gus_dma1->setCurrentIndex( ui.cbxDbx_gus_dma1->findText( TempProfileDosBox["gus_dma1"] ) );							// gus_dma1
		ui.cbxDbx_gus_dma2->setCurrentIndex( ui.cbxDbx_gus_dma2->findText( TempProfileDosBox["gus_dma2"] ) );							// gus_dma2
		ui.txtDbx_gus_ultradir->setText( TempProfileDosBox["gus_ultradir"] );															// gus_ultradir
	
		ui.chkDbx_speaker_pcspeaker->setChecked( fGrl.StrToBool( TempProfileDosBox["speaker_pcspeaker"] ) );							// speaker_pcspeaker
		ui.cbxDbx_speaker_pcrate->setCurrentIndex( ui.cbxDbx_speaker_pcrate->findText( TempProfileDosBox["speaker_pcrate"] ) );			// speaker_pcrate
		ui.cbxDbx_speaker_tandy->setCurrentIndex( ui.cbxDbx_speaker_tandy->findText( TempProfileDosBox["speaker_tandy"] ) );			// speaker_tandy
		ui.cbxDbx_speaker_tandyrate->setCurrentIndex( ui.cbxDbx_speaker_tandyrate->findText( TempProfileDosBox["speaker_tandyrate"] ) );// speaker_tandyrate
		ui.chkDbx_speaker_disney->setChecked( fGrl.StrToBool( TempProfileDosBox["speaker_disney"] ) );									// speaker_disney
	
		ui.cbxDbx_joystick_type->setCurrentIndex( ui.cbxDbx_joystick_type->findText( TempProfileDosBox["joystick_type"] ) );			// joystick_type
		ui.chkDbx_joystick_timed->setChecked( fGrl.StrToBool( TempProfileDosBox["joystick_timed"] ) );									// joystick_timed
		ui.chkDbx_joystick_autofire->setChecked( fGrl.StrToBool( TempProfileDosBox["joystick_autofire"] ) );							// joystick_autofire
		ui.chkDbx_joystick_swap34->setChecked( fGrl.StrToBool( TempProfileDosBox["joystick_swap34"] ) );								// joystick_swap34
		ui.chkDbx_joystick_buttonwrap->setChecked( fGrl.StrToBool( TempProfileDosBox["joystick_buttonwrap"] ) );						// joystick_buttonwrap
	
		ui.chkDbx_modem_modem->setChecked( fGrl.StrToBool( TempProfileDosBox["modem_modem"] ) );										// modem_modem
		ui.cbxDbx_modem_comport->setCurrentIndex( fGrl.StrToInt( TempProfileDosBox["modem_comport"] )-1 );								// modem_comport
		ui.txtDbx_modem_listenport->setText( TempProfileDosBox["modem_listenport"] );													// modem_listenport
	
		ui.chkDbx_dserial_directserial->setChecked( fGrl.StrToBool( TempProfileDosBox["dserial_directserial"] ) );						// dserial_directserial
		ui.cbxDbx_dserial_comport->setCurrentIndex( ui.cbxDbx_dserial_comport->findText( TempProfileDosBox["dserial_comport"] ) );		// dserial_comport
		ui.cbxDbx_dserial_realport->setCurrentIndex( ui.cbxDbx_dserial_realport->findText( TempProfileDosBox["dserial_realport"] ) );	// serial_realport
		ui.cbxDbx_dserial_defaultbps->setCurrentIndex( ui.cbxDbx_dserial_defaultbps->findText( TempProfileDosBox["dserial_defaultbps"] ) );	// dserial_defaultbps
		ui.cbxDbx_dserial_parity->setCurrentIndex( ui.cbxDbx_dserial_parity->findText( TempProfileDosBox["dserial_parity"] ) );			// dserial_parity
		ui.cbxDbx_dserial_bytesize->setCurrentIndex( ui.cbxDbx_dserial_bytesize->findText( TempProfileDosBox["dserial_bytesize"] ) );	// dserial_bytesize
		ui.cbxDbx_dserial_stopbit->setCurrentIndex( ui.cbxDbx_dserial_stopbit->findText( TempProfileDosBox["dserial_stopbit"] ) );		// dserial_stopbit
		ui.txtDbx_serial_1->setText( TempProfileDosBox["serial_1"] );	// serial_1
		ui.txtDbx_serial_2->setText( TempProfileDosBox["serial_2"] );	// serial_2
		ui.txtDbx_serial_3->setText( TempProfileDosBox["serial_3"] );	// serial_3
		ui.txtDbx_serial_4->setText( TempProfileDosBox["serial_4"] );	// serial_4
	
		ui.chkDbx_dos_xms->setChecked( fGrl.StrToBool( TempProfileDosBox["dos_xms"] ) );						// dos_xms
		ui.chkDbx_dos_ems->setChecked( fGrl.StrToBool( TempProfileDosBox["dos_ems"] ) );						// dos_ems
		ui.cbxDbx_dos_umb->setCurrentIndex( ui.cbxDbx_dos_umb->findText( TempProfileDosBox["dos_umb"] ) );		// dos_umb
		ui.cbxDbx_dos_keyboardlayout->setCurrentIndex( ui.cbxDbx_dos_keyboardlayout->findText( TempProfileDosBox["dos_keyboardlayout"] ) );	// dos_keyboardlayout
	
		ui.chkDbx_ipx_ipx->setChecked( fGrl.StrToBool( TempProfileDosBox["ipx_ipx"] ) );							// ipx_ipx
	
		ui.txtDbx_Autoexec->setText(TempProfileDosBox["autoexec"] );												// autoexec
		ui.chkDbx_autoexec->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_autoexec"] ) );						// opt_autoexec
	
		ui.chkDbx_loadfix->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_loadfix"] ) );						// opt_loadfix
		ui.txtDbx_loadfix_mem->setText( TempProfileDosBox["opt_loadfix_mem"]  );									// opt_loadfix_mem
		ui.chkDbx_consola_dbox->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_consola_dbox"] ) );				// opt_consola_dbox
		ui.chkDbx_cerrar_dbox->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_cerrar_dbox"] ) );				// opt_cerrar_dbox
		ui.chkDbx_cycle_sincronizar->setChecked( fGrl.StrToBool( TempProfileDosBox["opt_cycle_sincronizar"] ) );	// opt_cycle_sincronizar
	
		//ui.txtDbx_path_conf->setText( TempProfileDosBox["path_conf"] );					// path_conf
		ui.txtDbx_path_sonido->setText( TempProfileDosBox["path_sonido"]);					// path_sonido

		if( !TempProfileDosBox["path_exe"].isEmpty() )
			ui.txtDbx_path_exe->setText( TempProfileDosBox["path_exe"] );					// path_exe

		if( !TempProfileDosBox["path_setup"].isEmpty() )
			ui.txtDbx_path_setup->setText( TempProfileDosBox["path_setup"] );				// path_setup

		ui.txtDbx_parametros_exe->setText( TempProfileDosBox["parametros_exe"] );			// parametros_exe
		ui.txtDbx_parametros_setup->setText( TempProfileDosBox["parametros_setup"] );		// parametros_setup

		ui.twMontajes->clear();		
		ui.btnMount_AutoCrear->click();
/*
		int num_mounts = fGrl.StrToInt( TempProfileDosBox["NrOfMounts"] );
		if( num_mounts > 0 )
		{
			for(int i=0; i < num_mounts ; i++)
			{
				QTreeWidgetItem *twItemDfend = new QTreeWidgetItem(ui.twMontajes);
	
				str_Lista.clear();
				str_Lista = TempProfileDosBox[fGrl.IntToStr(i)].split("|");
	
				tipoDrive.clear();
				tipoDrive = str_Lista.value(1).toLower();// tipo_as
	
				if(tipoDrive=="drive")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_hd.png") );
					tipoDrive = "drive";
				}
				if(tipoDrive=="cdrom")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_cdrom.png") );
					tipoDrive = "cdrom";
				}
				if(tipoDrive=="floppy")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/drive_floppy.png") );
					tipoDrive = "floppy";
				}
				if(tipoDrive=="floppyimage")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/floppy_1.png") );
					tipoDrive = "IMG_floppy";
				}
				if(tipoDrive=="cdromimage")
				{
					twItemDfend->setIcon( 0, QIcon(":/img16/cd_iso.png") );
					tipoDrive = "IMG_iso";
				}
				if(tipoDrive=="image")
				{
					twItemDfend->setIcon(0, QIcon(":/img16/drive_hd.png") );
					tipoDrive = "IMG_hdd";
				}
				temp_opt_mount.clear();
				temp_opt_mount = str_Lista.value(5);
	
				twItemDfend->setText(0, ""+str_Lista.value(0) ); // path
				twItemDfend->setText(1, ""+str_Lista.value(4) ); // label
				twItemDfend->setText(2, ""+tipoDrive          ); // tipo_as
				twItemDfend->setText(3, ""+str_Lista.value(2) ); // letter
				twItemDfend->setText(4, ""+fGrl.IntToStr(i)   ); // indx_cd
				twItemDfend->setText(5, ""+temp_opt_mount     ); // opt_mount
				twItemDfend->setText(6, ""+str_Lista.value(3) ); // io_ctrl
				twItemDfend->setText(7, "x"                   ); // select_mount
			}
			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
			datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();
			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_cerrar_dbox->isChecked())
				datos_montaje["opt_cerrar_dbox"] = "true"; else datos_montaje["opt_cerrar_dbox"] = "false";

			ui.previer_mount->clear();
			ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
		}
*/
	}
}

void frmAddEditJuego::CargarDatosDBxMontaje( QString stIDdbx )
{
	QSqlQuery query;
	QSqlRecord rec;
	QString tipoDrive;
	query.exec("SELECT * FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+stIDdbx+" ORDER BY id_lista");
	if( query.first() )
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

			item->setText( 0 , query.value(rec.indexOf("path")).toString() );			// path			- directorio o iso
			item->setText( 1 , query.value(rec.indexOf("label")).toString() );			// label		- etiqueta
			item->setText( 2 , query.value(rec.indexOf("tipo_as")).toString() );		// tipo_as		- tipo de montaje
			item->setText( 3 , query.value(rec.indexOf("letter")).toString() );			// letter		- letra de montaje
			item->setText( 4 , query.value(rec.indexOf("indx_cd")).toString() );		// indx_cd		- index de la unidad de cd-rom
			item->setText( 5 , query.value(rec.indexOf("opt_mount")).toString() );		// opt_mount	- opciones del cd-rom
			item->setText( 6 , query.value(rec.indexOf("io_ctrl")).toString() );		// io_ctrl		- cd/dvd
			item->setText( 7 , query.value(rec.indexOf("select_mount")).toString());	// select_mount	- primer montaje
			item->setText( 8 , query.value(rec.indexOf("id")).toString() );				// id
			item->setText( 9 , query.value(rec.indexOf("id_lista")).toString() );		// id_lista		- id_lista
		} while (query.next());
	}

	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
	datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
	datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();

	if(ui.chkDbx_loadfix->isChecked())
		datos_montaje["opt_loadfix"] = "true";
	else
		datos_montaje["opt_loadfix"] = "false";

	if(ui.chkDbx_cerrar_dbox->isChecked())
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
	// sdl_fullscreen
		if(ui.chkDbx_sdl_fullscreen->isChecked() )
			DatosDosBox["sdl_fullscreen"] = "true"; else DatosDosBox["sdl_fullscreen"] = "false";
	// sdl_fulldouble
		if(ui.chkDbx_sdl_fulldouble->isChecked() )
			DatosDosBox["sdl_fulldouble"] = "true"; else DatosDosBox["sdl_fulldouble"] = "false";
	// sdl_fullfixed
		if(ui.chkDbx_sdl_fullfixed->isChecked() )
			DatosDosBox["sdl_fullfixed"] = "true"; else DatosDosBox["sdl_fullfixed"] = "false";
	// sdl_fullresolution
		if(ui.cbxDbx_sdl_fullresolution->currentText()!="")
			DatosDosBox["sdl_fullresolution"] = ui.cbxDbx_sdl_fullresolution->currentText(); else DatosDosBox["sdl_fullresolution"] = "original";
	// sdl_windowresolution
		if(ui.cbxDbx_sdl_windowresolution->currentText()!="")
			DatosDosBox["sdl_windowresolution"] = ui.cbxDbx_sdl_windowresolution->currentText(); else DatosDosBox["sdl_windowresolution"] = "original"; 
	// sdl_output
		if(ui.cbxDbx_sdl_output->currentText()!="")
			DatosDosBox["sdl_output"] = ui.cbxDbx_sdl_output->currentText(); else DatosDosBox["sdl_output"] = "surface";
	// sdl_hwscale
		if(ui.cbxDbx_sdl_hwscale->currentText()!="")
			DatosDosBox["sdl_hwscale"] = ui.cbxDbx_sdl_hwscale->currentText(); else DatosDosBox["sdl_hwscale"] = "1.00";
	// sdl_autolock
		if(ui.chkDbx_sdl_autolock->isChecked() )
			DatosDosBox["sdl_autolock"] = "true"; else DatosDosBox["sdl_autolock"] = "false";
	// sdl_sensitivity
		if(ui.cbxDbx_sdl_sensitivity->currentText()!="")
			DatosDosBox["sdl_sensitivity"] = ui.cbxDbx_sdl_sensitivity->currentText(); else DatosDosBox["sdl_sensitivity"] = "100";
	// sdl_waitonerror
		if(ui.chkDbx_sdl_waitonerror->isChecked() )
			DatosDosBox["sdl_waitonerror"] = "true"; else DatosDosBox["sdl_waitonerror"] = "false";
	// sdl_priority
		if(prioritySelect!="" && priorityNoSelect!="" )
			DatosDosBox["sdl_priority"] = prioritySelect+","+priorityNoSelect ; else DatosDosBox["sdl_priority"] = "higher,normal";
	// sdl_mapperfile
		if(ui.txtDbx_sdl_mapperfile->text()!="")
			DatosDosBox["sdl_mapperfile"] = ui.txtDbx_sdl_mapperfile->text(); else DatosDosBox["sdl_mapperfile"] = "mapper.txt";
	// sdl_usescancodes
		if(ui.chkDbx_sdl_usescancodes->isChecked()	)
			DatosDosBox["sdl_usescancodes"] = "true"; else DatosDosBox["sdl_usescancodes"] = "true";
	// dosbox_language
		if(ui.txtDbx_dosbox_language->text()!="")
			DatosDosBox["dosbox_language"] = ui.txtDbx_dosbox_language->text(); else DatosDosBox["dosbox_language"] = "";
	// dosbox_machine
		if(ui.cbxDbx_dosbox_machine->currentText()!="")
			DatosDosBox["dosbox_machine"] = ui.cbxDbx_dosbox_machine->currentText(); else DatosDosBox["dosbox_machine"] = "vga";
	// dosbox_captures
		if(ui.txtDbx_dosbox_captures->text()!="")
		DatosDosBox["dosbox_captures"] = ui.txtDbx_dosbox_captures->text(); else DatosDosBox["dosbox_captures"] = "";
	// dosbox_memsize
		if(ui.cbxDbx_dosbox_memsize->currentText()!="")
			DatosDosBox["dosbox_memsize"] = ui.cbxDbx_dosbox_memsize->currentText(); else DatosDosBox["dosbox_memsize"] = "16";
	// render_frameskip
		if(ui.cbxDbx_render_frameskip->currentText()!="")
			DatosDosBox["render_frameskip"] = ui.cbxDbx_render_frameskip->currentText(); else DatosDosBox["render_frameskip"] = "0";
	// render_aspect
		if(ui.chkDbx_render_aspect->isChecked() )
			DatosDosBox["render_aspect"] = "true"; else DatosDosBox["render_aspect"] = "false";
	// render_scaler
		if(ui.cbxDbx_render_scaler->currentText()!="")
			DatosDosBox["render_scaler"] = ui.cbxDbx_render_scaler->currentText(); else DatosDosBox["render_scaler"] = "normal2x";
	// cpu_core
		if(ui.cbxDbx_cpu_core->currentText()!="")
			DatosDosBox["cpu_core"] = ui.cbxDbx_cpu_core->currentText(); else DatosDosBox["cpu_core"] = "auto";
	// cpu_cycles
		if(ui.cbxDbx_cpu_cycles->currentText()!="")
			DatosDosBox["cpu_cycles"] = ui.cbxDbx_cpu_cycles->currentText(); else DatosDosBox["cpu_cycles"] = "auto";
	// cpu_cycleup
		if(ui.spinDbx_1->value() >= 0)
			DatosDosBox["cpu_cycleup"] = fGrl.IntToStr( ui.spinDbx_1->value() ); else DatosDosBox["cpu_cycleup"] = "500";
	// cpu_cycledown
		if(ui.spinDbx_2->value() >= 0)
			DatosDosBox["cpu_cycledown"] = fGrl.IntToStr( ui.spinDbx_2->value() ); else DatosDosBox["cpu_cycledown"] = "20";
	// mixer_nosound
		if(ui.chkDbx_mixer_nosound->isChecked() )
			DatosDosBox["mixer_nosound"] = "true"; else DatosDosBox["mixer_nosound"] = "false";
	// mixer_rate
		if(ui.cbxDbx_mixer_rate->currentText()!="")
			DatosDosBox["mixer_rate"] = ui.cbxDbx_mixer_rate->currentText(); else DatosDosBox["mixer_rate"] = "22050";
	// mixer_blocksize
		if(ui.txtDbx_mixer_blocksize->text()!="")
			DatosDosBox["mixer_blocksize"] = ui.txtDbx_mixer_blocksize->text(); else DatosDosBox["mixer_blocksize"] = "2048";
	// mixer_prebuffer
		if(ui.txtDbx_mixer_prebuffer->text()!="")
			DatosDosBox["mixer_prebuffer"] = ui.txtDbx_mixer_prebuffer->text(); else DatosDosBox["mixer_prebuffer"] = "10";
	// midi_mpu401
		if(ui.cbxDbx_midi_mpu401->currentText()!="")
			DatosDosBox["midi_mpu401"] = ui.cbxDbx_midi_mpu401->currentText(); else DatosDosBox["midi_mpu401"] = "intelligent";
	// midi_intelligent
		if(ui.chkDbx_midi_intelligent->isChecked() )
			DatosDosBox["midi_intelligent"] = "true"; else DatosDosBox["midi_intelligent"] = "false";
	// midi_device
		if(ui.cbxDbx_midi_device->currentText()!="")
			DatosDosBox["midi_device"] = ui.cbxDbx_midi_device->currentText(); else DatosDosBox["midi_device"] = "default";
	// midi_config
		if(ui.txtDbx_midi_config->text()!="")
			DatosDosBox["midi_config"] = ui.txtDbx_midi_config->text();else	DatosDosBox["midi_config"] = "";
	// midi_mt32rate
		if(ui.cbxDbx_midi_mt32rate->currentText()!="")
			DatosDosBox["midi_mt32rate"] = ui.cbxDbx_midi_mt32rate->currentText(); else DatosDosBox["midi_mt32rate"] = "22050"; 

	// sblaster_sbtype
		if(ui.cbxDbx_sblaster_sbtype->currentText()!="")
			DatosDosBox["sblaster_sbtype"] = ui.cbxDbx_sblaster_sbtype->currentText(); else DatosDosBox["sblaster_sbtype"] = "sb16";
	// sblaster_sbbase
		if(ui.cbxDbx_sblaster_sbbase->currentText()!="")
			DatosDosBox["sblaster_sbbase"] = ui.cbxDbx_sblaster_sbbase->currentText(); else DatosDosBox["sblaster_sbbase"] = "220";
	// sblaster_irq
		if(ui.cbxDbx_sblaster_irq->currentText()!="")
			DatosDosBox["sblaster_irq"] = ui.cbxDbx_sblaster_irq->currentText(); else DatosDosBox["sblaster_irq"] = "7";
	// sblaster_dma
		if(ui.cbxDbx_sblaster_dma->currentText()!="")
			DatosDosBox["sblaster_dma"] = ui.cbxDbx_sblaster_dma->currentText(); else DatosDosBox["sblaster_dma"] = "1";
	// sblaster_hdma
		if(ui.cbxDbx_sblaster_hdma->currentText()!="")
			DatosDosBox["sblaster_hdma"] = ui.cbxDbx_sblaster_hdma->currentText(); else DatosDosBox["sblaster_hdma"] = "5";
	// sblaster_mixer
		if(ui.chkDbx_sblaster_mixer->isChecked() )
			DatosDosBox["sblaster_mixer"] = "true"; else DatosDosBox["sblaster_mixer"] = "false";
	// sblaster_oplmode
		if(ui.cbxDbx_sblaster_oplmode->currentText()!="")
			DatosDosBox["sblaster_oplmode"] = ui.cbxDbx_sblaster_oplmode->currentText(); else DatosDosBox["sblaster_oplmode"] = "auto";
	// sblaster_oplrate
		if(ui.cbxDbx_sblaster_oplrate->currentText()!="")
			DatosDosBox["sblaster_oplrate"] = ui.cbxDbx_sblaster_oplrate->currentText(); else DatosDosBox["sblaster_oplrate"] = "22050";
	
	// gus_gus
		if(ui.chkDbx_gus_gus->isChecked() )
			DatosDosBox["gus_gus"] = "true"; else DatosDosBox["gus_gus"] = "false";
	// gus_gusrate
		if(ui.cbxDbx_gus_gusrate->currentText()!="")
			DatosDosBox["gus_gusrate"] = ui.cbxDbx_gus_gusrate->currentText(); else DatosDosBox["gus_gusrate"] = "22050";
	// gus_gusbase
		if(ui.cbxDbx_gus_gusbase->currentText()!="")
			DatosDosBox["gus_gusbase"] = ui.cbxDbx_gus_gusbase->currentText(); else DatosDosBox["gus_gusbase"] = "240";
	// gus_irq1
		if(ui.cbxDbx_gus_irq1->currentText()!="")
			DatosDosBox["gus_irq1"] = ui.cbxDbx_gus_irq1->currentText(); else DatosDosBox["gus_irq1"] = "5";
	// gus_irq2
		if(ui.cbxDbx_gus_irq2->currentText()!="")
			DatosDosBox["gus_irq2"] = ui.cbxDbx_gus_irq2->currentText(); else DatosDosBox["gus_irq2"] = "5";
	// gus_dma1
		if(ui.cbxDbx_gus_dma1->currentText()!="")
			DatosDosBox["gus_dma1"] = ui.cbxDbx_gus_dma1->currentText(); else DatosDosBox["gus_dma1"] = "3";
	// gus_dma2
		if(ui.cbxDbx_gus_dma2->currentText()!="")
			DatosDosBox["gus_dma2"] = ui.cbxDbx_gus_dma2->currentText(); else DatosDosBox["gus_dma2"] = "3";
	// gus_ultradir
		if(ui.txtDbx_gus_ultradir->text()!="")
			DatosDosBox["gus_ultradir"] = ui.txtDbx_gus_ultradir->text(); else DatosDosBox["gus_ultradir"] = "C:/ULTRASND";
		
	// speaker_pcspeaker
		if(ui.chkDbx_speaker_pcspeaker->isChecked() )
			DatosDosBox["speaker_pcspeaker"] = "true"; else DatosDosBox["speaker_pcspeaker"] = "false";
	// speaker_pcrate
		if(ui.cbxDbx_speaker_pcrate->currentText()!="")
			DatosDosBox["speaker_pcrate"] = ui.cbxDbx_speaker_pcrate->currentText(); else DatosDosBox["speaker_pcrate"] = "22050";
	// speaker_tandy
		if(ui.cbxDbx_speaker_tandy->currentText()!="")
			DatosDosBox["speaker_tandy"] = ui.cbxDbx_speaker_tandy->currentText(); else DatosDosBox["speaker_tandy"] = "auto";
	// speaker_tandyrate
		if(ui.cbxDbx_speaker_tandyrate->currentText()!="")
			DatosDosBox["speaker_tandyrate"] = ui.cbxDbx_speaker_tandyrate->currentText(); else DatosDosBox["speaker_tandyrate"] = "22050";
	// speaker_disney
		if(ui.chkDbx_speaker_disney->isChecked() )
			DatosDosBox["speaker_disney"] = "true"; else DatosDosBox["speaker_disney"] = "false";

	// joystick_type
		if(ui.cbxDbx_joystick_type->currentText()!="")
			DatosDosBox["joystick_type"] = ui.cbxDbx_joystick_type->currentText(); else DatosDosBox["joystick_type"] = "auto";
	// joystick_timed
		if(ui.chkDbx_joystick_timed->isChecked() )
			DatosDosBox["joystick_timed"] = "true"; else DatosDosBox["joystick_timed"] = "false";
	// joystick_autofire
		if(ui.chkDbx_joystick_autofire->isChecked() )
			DatosDosBox["joystick_autofire"] = "true"; else DatosDosBox["joystick_autofire"] = "false";
	// joystick_swap34
		if(ui.chkDbx_joystick_swap34->isChecked() )
			DatosDosBox["joystick_swap34"] = "true"; else DatosDosBox["joystick_swap34"] = "false";
	// joystick_buttonwrap
		if(ui.chkDbx_joystick_buttonwrap->isChecked() )
			DatosDosBox["joystick_buttonwrap"] = "true"; else DatosDosBox["joystick_buttonwrap"] = "false";

	// modem_modem
		if(ui.chkDbx_modem_modem->isChecked() )
			DatosDosBox["modem_modem"] = "true"; else DatosDosBox["modem_modem"] = "false";
	// modem_comport
		if(ui.cbxDbx_modem_comport->currentText()!="")
			DatosDosBox["modem_comport"] = fGrl.IntToStr(ui.cbxDbx_modem_comport->currentIndex()+1); else DatosDosBox["modem_comport"] = "2";
	// modem_listenport
		if(ui.txtDbx_modem_listenport->text()!="")
			DatosDosBox["modem_listenport"] = ui.txtDbx_modem_listenport->text() ; else DatosDosBox["modem_listenport"] = "";

	// dserial_directserial
		if(ui.chkDbx_dserial_directserial->isChecked() )
			DatosDosBox["dserial_directserial"] = "true"; else DatosDosBox["dserial_directserial"] = "false";
	// dserial_comport
		if(ui.cbxDbx_dserial_comport->currentText()!="")
			DatosDosBox["dserial_comport"] = ui.cbxDbx_dserial_comport->currentText(); else DatosDosBox["dserial_comport"] = "1";
	// dserial_realport
		if(ui.cbxDbx_dserial_realport->currentText()!="")
			DatosDosBox["dserial_realport"] = ui.cbxDbx_dserial_realport->currentText(); else DatosDosBox["dserial_realport"] = "COM1";
	// dserial_defaultbps
		if(ui.cbxDbx_dserial_defaultbps->currentText()!="")
			DatosDosBox["dserial_defaultbps"] = ui.cbxDbx_dserial_defaultbps->currentText(); else DatosDosBox["dserial_defaultbps"] = "1200";
	// dserial_parity
		if(ui.cbxDbx_dserial_parity->currentText()!="")
			DatosDosBox["dserial_parity"] = ui.cbxDbx_dserial_parity->currentText(); else DatosDosBox["dserial_parity"] = "N";
	// dserial_bytesize
		if(ui.cbxDbx_dserial_bytesize->currentText()!="")
			DatosDosBox["dserial_bytesize"] = ui.cbxDbx_dserial_bytesize->currentText(); else DatosDosBox["dserial_bytesize"] = "8";
	// dserial_stopbit
		if(ui.cbxDbx_dserial_stopbit->currentText()!="")
			DatosDosBox["dserial_stopbit"] = ui.cbxDbx_dserial_stopbit->currentText(); else DatosDosBox["dserial_stopbit"] = "1";

	// serial_1
		if(ui.txtDbx_serial_1->text()!="")
			DatosDosBox["serial_1"] = ui.txtDbx_serial_1->text(); else DatosDosBox["serial_1"] = "";
	// serial_2
		if(ui.txtDbx_serial_2->text()!="")
			DatosDosBox["serial_2"] = ui.txtDbx_serial_2->text(); else DatosDosBox["serial_2"] = "";
	// serial_3
		if(ui.txtDbx_serial_3->text()!="")
			DatosDosBox["serial_3"] = ui.txtDbx_serial_3->text(); else DatosDosBox["serial_3"] = "";
	// serial_4
		if(ui.txtDbx_serial_4->text()!="")
			DatosDosBox["serial_4"] = ui.txtDbx_serial_4->text(); else DatosDosBox["serial_4"] = "";

	// dos_xms
		if(ui.chkDbx_dos_xms->isChecked() )
			DatosDosBox["dos_xms"] = "true"; else DatosDosBox["dos_xms"] = "false";
	// dos_ems
		if(ui.chkDbx_dos_ems->isChecked() )
			DatosDosBox["dos_ems"] = "true"; else DatosDosBox["dos_ems"] = "false";
	// dos_umb
		if(ui.cbxDbx_dos_umb->currentText()!="")
			DatosDosBox["dos_umb"] = ui.cbxDbx_dos_umb->currentText(); else DatosDosBox["dos_umb"] = "true";
	// dos_keyboardlayout
		if(ui.cbxDbx_dos_keyboardlayout->currentText()!="")
			DatosDosBox["dos_keyboardlayout"] = ui.cbxDbx_dos_keyboardlayout->currentText(); else DatosDosBox["dos_keyboardlayout"] = "none";

	// ipx_ipx
		if(ui.chkDbx_ipx_ipx->isChecked() )
			DatosDosBox["ipx_ipx"] = "true"; else DatosDosBox["ipx_ipx"] = "false";

	// autoexec
		if(ui.txtDbx_Autoexec->toPlainText()!="")
			DatosDosBox["autoexec"] = ui.txtDbx_Autoexec->toPlainText(); else DatosDosBox["autoexec"] = "";

	// opt_autoexec
		if(ui.chkDbx_autoexec->isChecked() )
			DatosDosBox["opt_autoexec"] = "true"; else DatosDosBox["opt_autoexec"] = "false";
	// opt_loadfix
		if(ui.chkDbx_loadfix->isChecked() )
			DatosDosBox["opt_loadfix"] = "true"; else DatosDosBox["opt_loadfix"] = "false";
	// opt_loadfix_mem
		if(ui.txtDbx_loadfix_mem->text()!="")
			DatosDosBox["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text(); else  DatosDosBox["opt_loadfix_mem"] = "";
	// opt_consola_dbox
		if(ui.chkDbx_consola_dbox->isChecked() )
			DatosDosBox["opt_consola_dbox"] = "true"; else DatosDosBox["opt_consola_dbox"] = "false";
	// opt_cerrar_dbox
		if(ui.chkDbx_cerrar_dbox->isChecked() )
			DatosDosBox["opt_cerrar_dbox"] = "true"; else DatosDosBox["opt_cerrar_dbox"] = "false";
	// opt_cycle_sincronizar
		if(ui.chkDbx_cycle_sincronizar->isChecked() )
			DatosDosBox["opt_cycle_sincronizar"] = "true"; else DatosDosBox["opt_cycle_sincronizar"] = "false";

	// path_conf
		if(ui.txtDbx_path_conf->text()!="")
			DatosDosBox["path_conf"] = ui.txtDbx_path_conf->text() ; else DatosDosBox["path_conf"] = "";
	// path_sonido
		if(ui.txtDbx_path_sonido->text()!="")
			DatosDosBox["path_sonido"] = ui.txtDbx_path_sonido->text()	; else DatosDosBox["path_sonido"] = "";
	// path_exe
		if(ui.txtDbx_path_exe->text()!="")
			DatosDosBox["path_exe"] = ui.txtDbx_path_exe->text()	; else DatosDosBox["path_exe"] = "";
	// path_setup
		if(ui.txtDbx_path_setup->text()!="")
			DatosDosBox["path_setup"] = ui.txtDbx_path_setup->text(); else DatosDosBox["path_setup"] = "";
	// parametros_exe
		if(ui.txtDbx_parametros_exe->text()!="")
			DatosDosBox["parametros_exe"] = ui.txtDbx_parametros_exe->text(); else DatosDosBox["parametros_exe"] = "";
	// parametros_setup
		if(ui.txtDbx_parametros_setup->text()!="")
			DatosDosBox["parametros_setup"] = ui.txtDbx_parametros_setup->text(); else DatosDosBox["parametros_setup"] = "";
}

void frmAddEditJuego::on_btnDbx_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."),  stHomeDir + "confdbx/", ui.txtDbx_path_conf->text(), "Config DOSBox (*.conf);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );

		if(EditandoJuego != true)
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
		ui.txtDbx_path_conf->setText( "" );
}

void frmAddEditJuego::on_btnDbx_ExeJuego()
{
	ui.txtDbx_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDbx_ExeSetup()
{
	ui.txtDbx_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_path_setup->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDirGravisUltraSound()
{
	ui.txtDbx_gus_ultradir->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_gus_ultradir->text() ) );
}

void frmAddEditJuego::on_btnDbx_mapperfile()
{
	ui.txtDbx_sdl_mapperfile->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_sdl_mapperfile->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDbx_language()
{
	ui.txtDbx_dosbox_language->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtDbx_dosbox_language->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnDbx_capturas()
{
	ui.txtDbx_dosbox_captures->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_dosbox_captures->text() ) );
}

void frmAddEditJuego::on_btnDbx_musica()
{
	ui.txtDbx_path_sonido->setText( fGrl.VentanaDirectorios(tr("Selecciona el directorio"), stHomeDir , ui.txtDbx_path_sonido->text() ) );
}

void frmAddEditJuego::on_btnDbx_AddSerial()
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

QString frmAddEditJuego::setOpcionesSerial()
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

void frmAddEditJuego::on_btnMount_Add()
{	
	QHash<QString, QString> DatosMontaje;
	DatosMontaje.clear();
	DatosMontaje["path"]      = "";      // directorio o iso
	DatosMontaje["label"]     = "";      // etiqueta
	DatosMontaje["tipo_as"]   = "drive"; // tipo de montaje
	DatosMontaje["letter"]    = "C";     // letra de montaje
	DatosMontaje["indx_cd"]   = "";      // index de la unidad de cd-rom
	DatosMontaje["opt_mount"] = "";      // opciones del cd-rom
	DatosMontaje["io_ctrl"]   = "-aspi"; // cd/dvd

	frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
	if( AddEditMontajes->exec() == QDialog::Accepted )
	{
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
		QString tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];
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

		item->setText( 0 , AddEditMontajes->DatosMontaje["path"]		);	// directorio o iso
		item->setText( 1 , AddEditMontajes->DatosMontaje["label"]		);	// etiqueta
		item->setText( 2 , AddEditMontajes->DatosMontaje["tipo_as"]		);	// tipo de montaje
		item->setText( 3 , AddEditMontajes->DatosMontaje["letter"]		);	// letra de montaje
		item->setText( 4 , AddEditMontajes->DatosMontaje["indx_cd"]		);	// index de la unidad de cd-rom
		item->setText( 5 , AddEditMontajes->DatosMontaje["opt_mount"]	);	// opciones del cd-rom
		item->setText( 6 , AddEditMontajes->DatosMontaje["io_ctrl"]		);	// cd/dvd
		item->setText( 7 , "x"											);	// primer montaje

		if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
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

			item->setText( 8 , sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
		} else
			item->setText( 8 , "" );//

		item->setText( 9 , fGrl.IntToStr( item->childCount() ) );

		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"] = ui.txtDbx_path_exe->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_parametros_exe->text();
		datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();

		if(ui.chkDbx_loadfix->isChecked())
			datos_montaje["opt_loadfix"] = "true";
		else datos_montaje["opt_loadfix"] = "false";

		if(ui.chkDbx_cerrar_dbox->isChecked())
			datos_montaje["opt_cerrar_dbox"] = "true";
		else datos_montaje["opt_cerrar_dbox"] = "false";

		ui.previer_mount->clear();
		ui.previer_mount->addItems( fGrl.CreaConfigMontajes( ui.twMontajes, datos_montaje) );
	}
}

void frmAddEditJuego::on_btnMount_Edit()
{
	int pos = ui.twMontajes->indexOfTopLevelItem( ui.twMontajes->currentItem() );

	if( ui.twMontajes->topLevelItemCount()>0 && pos!=-1 )
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

		frmAddEditMontajes * AddEditMontajes = new frmAddEditMontajes( DatosMontaje );
		if( AddEditMontajes->exec() == QDialog::Accepted )
		{
			QString tipoDrive = AddEditMontajes->DatosMontaje["tipo_as"];
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

			ui.twMontajes->currentItem()->setText( 0 , AddEditMontajes->DatosMontaje["path"]		);	// directorio o iso
			ui.twMontajes->currentItem()->setText( 1 , AddEditMontajes->DatosMontaje["label"]		);	// etiqueta
			ui.twMontajes->currentItem()->setText( 2 , AddEditMontajes->DatosMontaje["tipo_as"]		);	// tipo de montaje
			ui.twMontajes->currentItem()->setText( 3 , AddEditMontajes->DatosMontaje["letter"]		);	// letra de montaje
			ui.twMontajes->currentItem()->setText( 4 , AddEditMontajes->DatosMontaje["indx_cd"]		);	// index de la unidad de cd-rom
			ui.twMontajes->currentItem()->setText( 5 , AddEditMontajes->DatosMontaje["opt_mount"]	);	// opciones del cd-rom
			ui.twMontajes->currentItem()->setText( 6 , AddEditMontajes->DatosMontaje["io_ctrl"]		);	// cd/dvd
			//ui.twMontajes->currentItem()->setText( 7 , "x"											);	// primer montaje
			//ui.twMontajes->currentItem()->setText( 9 , fGrl.IntToStr( ui.twMontajes->currentItem()->childCount() ) ); //

			QHash<QString, QString> datos_montaje;
			datos_montaje.clear();
			datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
			datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();

			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true";
			else datos_montaje["opt_loadfix"] = "false";

			if(ui.chkDbx_cerrar_dbox->isChecked())
				datos_montaje["opt_cerrar_dbox"] = "true";
			else datos_montaje["opt_cerrar_dbox"] = "false";

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
			datos_montaje["path_exe"] = ui.txtDbx_path_exe->text();
			datos_montaje["parametros_exe"] = ui.txtDbx_parametros_exe->text();
			datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();
			if(ui.chkDbx_loadfix->isChecked())
				datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
			if(ui.chkDbx_cerrar_dbox->isChecked())
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
	QFileInfo fi( ui.txtDbx_path_exe->text() );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twMontajes );
	item->setIcon( 0 , QIcon(":/img16/drive_hd.png")	);
	item->setText( 0 , fi.absolutePath()				);	// directorio o iso
	item->setText( 1 , ""								);	// etiqueta
	item->setText( 2 , "drive"							);	// tipo de montaje
	item->setText( 3 , "C"								);	// letra de montaje
	item->setText( 4 , ""								);	// index de la unidad de cd-rom
	item->setText( 5 , ""								);	// opciones del cd-rom
	item->setText( 6 , "-aspi"							);	// cd/dvd IOCtrl
	item->setText( 7 , "x"								);	// primer montaje

	if( (EditandoJuego == true) && (TipoEmulador == "dosbox") )
	{
		QHash<QString, QString> datos_montaje_nuevo;
		datos_montaje_nuevo.clear();
		datos_montaje_nuevo["id_lista"]     = fGrl.IntToStr( item->childCount() );
		datos_montaje_nuevo["path"]         = fi.absolutePath();
		datos_montaje_nuevo["label"]        = "";
		datos_montaje_nuevo["tipo_as"]      = "drive";
		datos_montaje_nuevo["letter"]       = "C";
		datos_montaje_nuevo["indx_cd"]      = "";
		datos_montaje_nuevo["opt_mount"]    = "-usecd 0";
		datos_montaje_nuevo["io_ctrl"]      = "-aspi";
		datos_montaje_nuevo["select_mount"] = "x";

		item->setText( 8 , sql->ItemInsertaUnMontajeDbx( datos_montaje_nuevo, stItemIDDbx) );	// id
	} else
		item->setText( 8 , "" );//
	item->setText( 9 , fGrl.IntToStr( item->childCount() )  ); //

	QHash<QString, QString> datos_montaje;
	datos_montaje.clear();
	datos_montaje["path_exe"]        = ui.txtDbx_path_exe->text();
	datos_montaje["parametros_exe"]  = ui.txtDbx_parametros_exe->text();
	datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();
	if(ui.chkDbx_loadfix->isChecked())
		datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
	if(ui.chkDbx_cerrar_dbox->isChecked())
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
		datos_montaje["path_exe"] = ui.txtDbx_path_exe->text();
		datos_montaje["parametros_exe"] = ui.txtDbx_parametros_exe->text();
		datos_montaje["opt_loadfix_mem"] = ui.txtDbx_loadfix_mem->text();
		if(ui.chkDbx_loadfix->isChecked())
			datos_montaje["opt_loadfix"] = "true"; else datos_montaje["opt_loadfix"] = "false";
		if(ui.chkDbx_cerrar_dbox->isChecked())
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
	if( query.first() )
	{
		rec = query.record();

		ui.txtVdms_path_conf->setText( query.value(rec.indexOf("path_conf")).toString() );	// 
		ui.txtVdms_path_exe->setText( query.value(rec.indexOf("path_exe")).toString() );	// 
		str_program       = query.value(rec.indexOf("program")).toString();					// 
		str_vdms_debug    = query.value(rec.indexOf("vdms_debug")).toString();				// 
		str_winnt_dos     = query.value(rec.indexOf("winnt_dos")).toString();				// 
		str_winnt_dosbox  = query.value(rec.indexOf("winnt_dosbox")).toString();			// 
		str_winnt_storage = query.value(rec.indexOf("winnt_storage")).toString();			// 

		QStringList list_program       = str_program.split("|");
		QStringList list_winnt_dos     = str_winnt_dos.split("|");
		QStringList list_vdms_debug    = str_vdms_debug.split("|");
		QStringList list_winnt_dosbox  = str_winnt_dosbox.split("|");
		QStringList list_winnt_storage = str_winnt_storage.split("|");

		ui.txtVdms_params->setText( list_program[0] ); 
		ui.txtVdms_icon->setText( list_program[1] ); 

		if(list_winnt_dos[0]=="yes")
			ui.chkVdms_useAutoexec->setChecked(true); else ui.chkVdms_useAutoexec->setChecked(false);

		strTemp.clear();
		strTemp = list_winnt_dos[1];
		ui.txtVdms_autoexec->setPlainText( strTemp.replace("%000d%000a", "\n") );

		if(list_vdms_debug[0]=="yes")
			ui.chkVdms_useCustomCfg->setChecked(true); else ui.chkVdms_useCustomCfg->setChecked(false);

		strTemp.clear();
		strTemp = list_vdms_debug[1];
		ui.txtVdms_customCfg->setPlainText( strTemp.replace("%000d%000a", "\n") );

		if(list_winnt_dosbox[0]=="yes")
			ui.chkVdms_exitclose->setChecked(true); else ui.chkVdms_exitclose->setChecked(false); 
		if(list_winnt_dosbox[1]=="yes")
			ui.chkVdms_exitWarn->setChecked(true); else ui.chkVdms_exitWarn->setChecked(false); 
		if(list_winnt_dosbox[2]=="yes")
			ui.chkVdms_fastPaste->setChecked(true); else ui.chkVdms_fastPaste->setChecked(false); 

		if(list_winnt_storage[0]=="yes")
			ui.chkVdms_useCDROM->setChecked(true); else ui.chkVdms_useCDROM->setChecked(false); 
		if(list_winnt_storage[1]=="yes")
			ui.chkVdms_useNetware->setChecked(true); else ui.chkVdms_useNetware->setChecked(false); 
	}
}

void frmAddEditJuego::setDatosVDMSound()
{
	QString strTemp;
	DatosVDMSound.clear();
	DatosVDMSound["path_conf"] = ui.txtVdms_path_conf->text();
	DatosVDMSound["path_exe"]  = ui.txtVdms_path_exe->text();
	DatosVDMSound["program_1"] = ui.txtVdms_params->text();
	DatosVDMSound["program_2"] = ui.txtVdms_icon->text();
	if(ui.chkVdms_useAutoexec->isChecked())
		DatosVDMSound["winnt_dos_1"] = "yes"; else DatosVDMSound["winnt_dos_1"] = "no";

	strTemp.clear();
	strTemp = ui.txtVdms_autoexec->toPlainText();
	DatosVDMSound["winnt_dos_2"] = strTemp.replace("\n", "%000d%000a");

	if(ui.chkVdms_useCustomCfg->isChecked())
		DatosVDMSound["vdms_debug_1"] = "yes"; else DatosVDMSound["vdms_debug_1"] = "no";

	strTemp.clear();
	strTemp = ui.txtVdms_customCfg->toPlainText();
	DatosVDMSound["vdms_debug_2"] = strTemp.replace("\n", "%000d%000a");

	if(ui.chkVdms_exitclose->isChecked())
		DatosVDMSound["winnt_dosbox_1"] = "yes"; else DatosVDMSound["winnt_dosbox_1"] = "no";
	if(ui.chkVdms_exitWarn->isChecked())
		DatosVDMSound["winnt_dosbox_2"] = "yes"; else DatosVDMSound["winnt_dosbox_2"] = "no";
	if(ui.chkVdms_fastPaste->isChecked())
		DatosVDMSound["winnt_dosbox_3"] = "yes"; else DatosVDMSound["winnt_dosbox_3"] = "no";
	if(ui.chkVdms_useCDROM->isChecked())
		DatosVDMSound["winnt_storage_1"] = "yes"; else DatosVDMSound["winnt_storage_1"] = "no";
	if(ui.chkVdms_useNetware->isChecked()) 
		DatosVDMSound["winnt_storage_2"] = "yes"; else DatosVDMSound["winnt_storage_2"] = "no";
}

void frmAddEditJuego::on_btnVdms_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir + "confvdms/", ui.txtVdms_path_conf->text(), "Config VDMSound (*.vlp);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
		if(EditandoJuego != true)
		{
			if( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
		}
		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
		str_ok = str.endsWith(".vlp");
		if(str_ok == false)
			str.append(".vlp");
		ui.txtVdms_path_conf->setText( str );
	} else 
		ui.txtVdms_path_conf->setText( "" );
}

void frmAddEditJuego::on_btnVdms_ExeJuego()
{
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtVdms_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );
}

void frmAddEditJuego::on_btnVdms_Icono()
{
	ui.txtVdms_icon->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtVdms_icon->text(), tr("Todos los archivo") + " (*)", 0, false) + ",0" );
}
// ---------------------------------------------------------------
