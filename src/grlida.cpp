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

#include "grlida.h"
#include "grlida_acercad.h"
#include "grlida_info.h"
#include "grlida_dbxadd.h"
#include "grlida_svmadd.h"
#include "grlida_vdmsadd.h"
#include "grlida_addedit_juego.h"
#include "grlida_opciones.h"
#include "grlida_importar_juego.h"

#ifdef Q_OS_WIN32
#include <qt_windows.h>
//#include <qtwindowdefs_win.h>
//#include <shlobj.h>
#include <shellapi.h>
#endif

GrLida::GrLida( QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	
// Titulo de la aplicacion para los distintos Sistemas Operativos
	setWindowTitle(  stTituloGrl() + " - " + fGrl.get_Plataforma() + " v" +  fGrl.stVersionGrl() );
	
// Conecta los distintos botones con las funciones de cada uno	
	connect( ui.actionNewDbx  , SIGNAL( triggered() ), this, SLOT( on_AddNewDbx()		));
	connect( ui.actionNewSvm  , SIGNAL( triggered() ), this, SLOT( on_AddNewSvm()    	));
	connect( ui.actionNewVdms , SIGNAL( triggered() ), this, SLOT( on_AddNewVdms()   	));
    connect( ui.actionEjectar , SIGNAL( triggered() ), this, SLOT( on_EjecutarJuego() 	));
	connect( ui.actionEjectarSetup, SIGNAL( triggered() ), this, SLOT( on_EjecutarSetup() 	));
	connect( ui.actionNuevo   , SIGNAL( triggered() ), this, SLOT( on_NuevoJuego()		));
	connect( ui.actionEditar  , SIGNAL( triggered() ), this, SLOT( on_EditarJuego()     ));
	connect( ui.actionEiminar , SIGNAL( triggered() ), this, SLOT( on_EliminarJuego()   ));
    connect( ui.actionImportarJuego, SIGNAL(triggered()), this, SLOT( on_ImportarJuego()));
	connect( ui.actionOpciones, SIGNAL( triggered() ), this, SLOT( on_Opciones()        ));
    connect( ui.actionAcercaD , SIGNAL( triggered() ), this, SLOT( on_AcercaD()         ));

// Conecta los botones de los menus
	connect( ui.mnu_file_informacion , SIGNAL( triggered() ), this, SLOT( on_Informacion() ));
    connect( ui.mnu_edit_nuevo_dosbox   , SIGNAL(triggered()),this, SLOT( on_AddNewDbx()		));
    connect( ui.mnu_edit_nuevo_scummvm  , SIGNAL(triggered()),this, SLOT( on_AddNewSvm()    	));
    connect( ui.mnu_edit_nuevo_vdmsound , SIGNAL(triggered()),this, SLOT( on_AddNewVdms()   	));
    connect( ui.mnu_edit_nuevo       , SIGNAL( triggered() ), this, SLOT( on_NuevoJuego()		));
    connect( ui.mnu_edit_editar      , SIGNAL( triggered() ), this, SLOT( on_EditarJuego()      ));
    connect( ui.mnu_edit_eliminar    , SIGNAL( triggered() ), this, SLOT( on_EliminarJuego()    ));
	connect( ui.mnu_ejecutar_dosbox  , SIGNAL( triggered() ), this, SLOT( on_EjecutarDosbox() 	));
    connect( ui.mnu_ejecutar_scummvm , SIGNAL( triggered() ), this, SLOT( on_EjecutarScummVM() 	));
    connect( ui.mnu_ejecutar_juego   , SIGNAL( triggered() ), this, SLOT( on_EjecutarJuego() 	));
	connect( ui.mnu_ejecutar_setup   , SIGNAL( triggered() ), this, SLOT( on_EjecutarSetup() 	));
    connect( ui.mnu_tool_importar    , SIGNAL( triggered() ), this, SLOT( on_ImportarJuego()    ));
    connect( ui.mnu_tool_opciones    , SIGNAL( triggered() ), this, SLOT( on_Opciones()         ));
    connect( ui.mnu_ver_acercad      , SIGNAL( triggered() ), this, SLOT( on_AcercaD()          ));

// Conecta otros botones
    connect( ui.btn_fileurl_1 , SIGNAL( clicked() ), this, SLOT( on_btn_fileurl_1() ));
    connect( ui.btn_fileurl_2 , SIGNAL( clicked() ), this, SLOT( on_btn_fileurl_2() ));
    connect( ui.btn_imgtumb_1 , SIGNAL( clicked() ), this, SLOT( on_btn_imgtumb_1() ));
    connect( ui.btn_imgtumb_2 , SIGNAL( clicked() ), this, SLOT( on_btn_imgtumb_2() ));

// Conecta la lista de juegos
	connect( ui.twJuegos, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_clicked(QTreeWidgetItem*)));
	connect( ui.twJuegos, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_Dblclicked(QTreeWidgetItem*) )); 

	ui.toolBar_1->setContextMenuPolicy( Qt::PreventContextMenu );
	
	lbpanel_1.setFrameStyle( QFrame::NoFrame  );
	lbpanel_2.setFrameStyle( QFrame::NoFrame );
	lbpanel_3.setFrameStyle( QFrame::NoFrame );
	lbpanel_4.setFrameStyle( QFrame::NoFrame );
	lbpanel_5.setFrameStyle( QFrame::NoFrame | QFrame::Panel );

	lbpanel_1.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter); //icono
	lbpanel_2.setAlignment(Qt::AlignVCenter | Qt::AlignLeft)   ; //Num juegos
	lbpanel_3.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter); //icono emu
	lbpanel_4.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter); //icono datos
	lbpanel_5.setAlignment(Qt::AlignVCenter | Qt::AlignLeft)   ; //datos

	lbpanel_1.setPixmap( QPixmap(":/img16/datos_3.png") );
	lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );
	lbpanel_4.setPixmap( QPixmap(":/img16/datos_1.png") );
	lbpanel_1.setMinimumSize(20,20);//w,h
	lbpanel_3.setMinimumSize(20,20);//w,h
	lbpanel_4.setMinimumSize(20,20);//w,h

	ui.statusBar->addWidget( &lbpanel_1    ); // icono
	ui.statusBar->addWidget( &lbpanel_2    ); // Num juegos
	ui.statusBar->addWidget( &lbpanel_3    ); // icono emu
	ui.statusBar->addWidget( &lbpanel_4    ); // icono datos
	ui.statusBar->addWidget( &lbpanel_5, 1 ); // datos

	ui.twJuegos->header()->setStretchLastSection(true); 
	ui.twJuegos->header()->setMovable(false);
	ui.twJuegos->header()->setResizeMode(QHeaderView::Custom);
	ui.twJuegos->header()->resizeSection(0, 31 );//31
	ui.twJuegos->header()->resizeSection(1, 60 );

	ui.twFiles->header()->setStretchLastSection(true); 
	ui.twFiles->header()->setMovable(false);
	ui.twFiles->header()->setResizeMode(QHeaderView::Interactive);
	ui.twFiles->header()->resizeSection(0, 100 );	// nombre
	ui.twFiles->header()->resizeSection(1, 70 );	// crc32
	ui.twFiles->header()->resizeSection(2, 140 );	// descripcion
	ui.twFiles->header()->resizeSection(3, 70 );	// size

	ui.twUrls->header()->setStretchLastSection(true); 
	ui.twUrls->header()->setMovable(false);
	ui.twUrls->header()->setResizeMode(QHeaderView::Interactive);
	ui.twUrls->header()->resizeSection(0, 100 );

	ui.twCapturas->header()->setStretchLastSection(true); 
	ui.twCapturas->header()->setMovable(false);
	ui.twCapturas->header()->setResizeMode(QHeaderView::Interactive);

	ui.btn_fileurl_1->setChecked(true);
	ui.btn_fileurl_2->setChecked(false);
	ui.twUrls->setVisible(true);
	ui.twFiles->setVisible(false);

//	dBoxSvm = new QProcess( this );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

GrLida::~GrLida()
{
	delete dBoxSvm;
	delete sql;
}

void GrLida::closeEvent( QCloseEvent *e )
{
	sql->dbClose();

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat ); 
	settings.beginGroup("OpcVer");
		settings.setValue("Pnl_Capturas" , ui.actionVerCapturas->isChecked() );
		settings.setValue("Pnl_Datos"    , ui.actionVerDatos->isChecked()    );
		settings.setValue("Pnl_FilesUrl" , ui.actionVerFilesUrl->isChecked() );
	settings.endGroup();
	settings.beginGroup("Updates");
		settings.setValue("Version" ,  fGrl.stVersionGrl() );
	settings.endGroup();	
	e->accept();

}

void GrLida::on_ImportarJuego()
{
	frmImportarJuego * NewImportar = new frmImportarJuego( 0, Qt::Window );
	if ( NewImportar->exec() == QDialog::Accepted ) {
		QString lastID, lastID_Dbx;

		lastID = sql->ItemInsertaDatos( NewImportar->DatosJuego );
	
		sql->ItemInsertaFiles( NewImportar->ui.twDatosFiles, lastID );
		//sql->ItemInsertaURL( NewImportar->ui.twDatosURL, lastID );

		if (NewImportar->DatosJuego["tipo_emu"]=="dosbox"){
			lastID_Dbx = sql->ItemInsertaDbx( NewImportar->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx( NewImportar->ui.twMontajes, lastID_Dbx );
			CrearArchivoConfigDbx( NewImportar->DatosDosBox, NewImportar->ui.twMontajes, stConfgDbxDir + NewImportar->DatosDosBox["path_conf"] );
		}
		if (NewImportar->DatosJuego["tipo_emu"]=="scummvm"){
			sql->ItemInsertaSvm( NewImportar->DatosScummvm, lastID);
		}
		if (NewImportar->DatosJuego["tipo_emu"]=="vdmsound"){
			sql->ItemInsertaVdms( NewImportar->DatosVDMSound, lastID);
		}
		NuevoItemTreeWidget( NewImportar->DatosJuego, NewImportar->DatosJuego["tipo_emu"], lastID);
	}
}

void GrLida::CargarConfigInicial()
{
	stHomeDir     = QDir::homePath()+"/.gr-lida/";		// directorio de trabajo del GR-lida
//	stHomeDir     = QDir::currentPath()+"/";	// directorio de trabajo del GR-lida
//	stHomeDir     = "./";								// directorio de trabajo del GR-lida	
	stIconDir     = stHomeDir + "iconos/";				// directorio de iconos para el GR-lida
	stDatosDir    = stHomeDir + "datos/";				// directorio para los distintos datos del GR-lida
	stConfgDbxDir = stHomeDir + "confdbx/";				// directorio de configuracion para el DOSBox
#ifdef Q_OS_WIN32
	stConfgVdmSDir= stHomeDir + "confvdms/";			// directorio de configuracion para el VDMSound
#endif
	QSettings settings(stHomeDir+"GR-lida.conf", QSettings::IniFormat); 
	settings.beginGroup("SqlDatabase");
	  stdb_type	   = settings.value("db_type"	 , "QSQLITE" ).toString();
	  stdb_host	   = settings.value("db_host"	 , stHomeDir+"db_grl.grl" ).toString();
	  stdb_name	   = settings.value("db_name"	 , "" 		).toString();
	  stdb_username= settings.value("db_username", "" 		).toString();
	  stdb_password= settings.value("db_password", "" 		).toString();
	  stdb_port    = settings.value("db_port"	 , "3306" 	).toString();
	settings.endGroup();	
	settings.beginGroup("OpcGeneral");
	  stBinExeDbx   = QDir::toNativeSeparators ( settings.value("DirDOSBox", "").toString() );	//
	  stBinExeSvm   = QDir::toNativeSeparators ( settings.value("DirScummVM", "").toString() );	//
	  ui.actionNewDbx->setEnabled( settings.value("DOSBoxDisp"   , "false").toBool() );
	  ui.actionNewSvm->setEnabled( settings.value("ScummVMDisp"  , "false").toBool() );
	  stIdiomaSelect = settings.value("IdiomaSelect", "es_ES").toString();
#ifdef Q_OS_WIN32
	  ui.actionNewVdms->setEnabled(settings.value("VDMSoundDisp" , "false").toBool());
#else
	  ui.actionNewVdms->setEnabled(false);
#endif
	settings.endGroup();
	settings.beginGroup("OpcVer");
	  ui.actionVerDatos->setChecked( settings.value("Pnl_Datos", "true").toBool() );
	  ui.actionVerCapturas->setChecked( settings.value("Pnl_Capturas", "false").toBool() );
	  ui.actionVerFilesUrl->setChecked( settings.value("Pnl_FilesUrl", "false").toBool() );
	  ui.dockw_Datos->setVisible(	settings.value("Pnl_Datos", "true").toBool() );
	  ui.dockw_Capturas->setVisible(settings.value("Pnl_Capturas", "false").toBool() );
	  ui.dockw_FilesUrl->setVisible( settings.value("Pnl_FilesUrl", "false").toBool() );
	settings.endGroup();
#ifdef Q_OS_WIN32
	if( (stBinExeDbx=="")&&(stBinExeSvm=="")&&(ui.actionNewVdms->isEnabled()==false))
		ui.actionEjectar->setEnabled( false );
#else
	if( (stBinExeDbx=="")&&(stBinExeSvm=="")) ui.actionEjectar->setEnabled( false );
#endif

	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles(stDatosDir + "smiles.txt");

	sql = new dbSql( stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port); // Carga la base de datos
}

void GrLida::on_AddNewDbx(){
	frmDbxAdd * AddDbxNew = new frmDbxAdd();
    if ( AddDbxNew->exec() == QDialog::Accepted ) {
		QString lastID, lastID_Dbx ;
		lastID = sql->ItemInsertaDatos( AddDbxNew->DatosJuego );
		lastID_Dbx = sql->ItemInsertaDbx( AddDbxNew->DatosDosBox , lastID);
		sql->ItemInsertaMontajesDbx( AddDbxNew->ui.twMontajes, lastID_Dbx );
		CrearArchivoConfigDbx( AddDbxNew->DatosDosBox, AddDbxNew->ui.twMontajes, stConfgDbxDir + AddDbxNew->DatosDosBox["path_conf"] );
		NuevoItemTreeWidget( AddDbxNew->DatosJuego, "dosbox", lastID);
	}
}

void GrLida::CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString PathSaveConfg)
{
	QSettings * settings = new QSettings( PathSaveConfg, QSettings::IniFormat ); 
	QFileInfo workdir( datosVdms["path_exe"] );

	settings->beginGroup("program");
	settings->setValue("executable"		, datosVdms["path_exe"]  );
	settings->setValue("workdir"		, workdir.absolutePath() );
	settings->setValue("params"			, datosVdms["program_1"] );
	settings->setValue("icon"			, datosVdms["program_2"] );
	settings->endGroup();

	settings->beginGroup("vdms.debug");
	settings->setValue("useCustomCfg"	, datosVdms["vdms_debug_1"] );
	settings->setValue("customCfg"		, datosVdms["vdms_debug_2"] );
	settings->endGroup();
	
	settings->beginGroup("winnt.dos");
	settings->setValue("useAutoexec"	, datosVdms["winnt_dos_1"] );
	settings->setValue("autoexec"		, datosVdms["winnt_dos_2"] );
	settings->endGroup();

	settings->beginGroup("winnt.dosbox");
	settings->setValue("exitclose"		, datosVdms["winnt_dosbox_1"] );
	settings->setValue("exitWarn"		, datosVdms["winnt_dosbox_2"] );
	settings->setValue("fastPaste"		, datosVdms["winnt_dosbox_3"] );
	settings->endGroup();

	settings->beginGroup("winnt.storage");
	settings->setValue("useCDROM"		, datosVdms["winnt_storage_1"] );
	settings->setValue("useNetware"		, datosVdms["winnt_storage_2"] );
	settings->endGroup();
}

void GrLida::CrearArchivoConfigDbx(const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg)
{
	QSettings * settings = new QSettings( PathSaveConfg, QSettings::IniFormat ); 
	settings->beginGroup("sdl");
	settings->setValue("fullscreen"			, datosDbx["sdl_fullscreen"]  	);
	settings->setValue("fulldouble"			, datosDbx["sdl_fulldouble"]   	);
	settings->setValue("fullfixed"			, datosDbx["sdl_fullfixed"]   	); // DOSBox 0.63
	settings->setValue("fullresolution"		, datosDbx["sdl_fullresolution"]);
	settings->setValue("windowresolution"	, datosDbx["sdl_windowresolution"]);
	settings->setValue("output"				, datosDbx["sdl_output"]   		);
	settings->setValue("hwscale"			, datosDbx["sdl_hwscale"]   	); // DOSBox 0.63
	settings->setValue("autolock"			, datosDbx["sdl_autolock"]   	);
	settings->setValue("sensitivity"		, datosDbx["sdl_sensitivity"]   );
	settings->setValue("waitonerror"		, datosDbx["sdl_waitonerror"]   );
	settings->setValue("priority"			, datosDbx["sdl_priority"]  	);
	settings->setValue("mapperfile"			, datosDbx["sdl_mapperfile"]  	);
	settings->setValue("usescancodes"		, datosDbx["sdl_usescancodes"]	);
	settings->endGroup();
	
	settings->beginGroup("dosbox");
	if(datosDbx["dosbox_language"]!="" || datosDbx["dosbox_language"]!=" ")
	settings->setValue("language"		, datosDbx["dosbox_language"]  );
	settings->setValue("machine"		, datosDbx["dosbox_machine"]  	);
	settings->setValue("captures"		, datosDbx["dosbox_captures"]  );
	settings->setValue("memsize"		, datosDbx["dosbox_memsize"]  	);
	settings->endGroup();

	settings->beginGroup("render");
	settings->setValue("frameskip"		, datosDbx["render_frameskip"]  );
	settings->setValue("aspect"			, datosDbx["render_aspect"]  	);
	settings->setValue("scaler"			, datosDbx["render_scaler"]  	);
	settings->endGroup();
	
	settings->beginGroup("cpu");
	settings->setValue("core"			, datosDbx["cpu_core"]  	);
	settings->setValue("cycles"			, datosDbx["cpu_cycles"]  	);
	settings->setValue("cycleup"		, datosDbx["cpu_cycleup"]  	);
	settings->setValue("cycledown"		, datosDbx["cpu_cycledown"] );
	settings->endGroup();

	settings->beginGroup("mixer");
	settings->setValue("nosound"		, datosDbx["mixer_nosound"]  	);
	settings->setValue("rate"			, datosDbx["mixer_rate"]  	);
	settings->setValue("blocksize"		, datosDbx["mixer_blocksize"] );
	settings->setValue("prebuffer"		, datosDbx["mixer_prebuffer"] );
	settings->endGroup();

	settings->beginGroup("midi");
	settings->setValue("mpu401"			, datosDbx["midi_mpu401"]  	);
	settings->setValue("intelligent"	, datosDbx["midi_intelligent"]  ); // DOSBox 0.63
	settings->setValue("device"			, datosDbx["midi_device"]  	);
	settings->setValue("config"			, datosDbx["midi_config"]  	);
	settings->setValue("mt32rate"		, datosDbx["midi_mt32rate"]  ); // DOSBox 0.63
	settings->endGroup();

	settings->beginGroup("sblaster");
	settings->setValue("sbtype"			, datosDbx["sblaster_sbtype"]  	);
	settings->setValue("sbbase"			, datosDbx["sblaster_sbbase"]  	);
	settings->setValue("irq"			, datosDbx["sblaster_irq"]  		);
	settings->setValue("dma"			, datosDbx["sblaster_dma"]  		);
	settings->setValue("hdma"			, datosDbx["sblaster_hdma"]  	);
	settings->setValue("mixer"			, datosDbx["sblaster_mixer"]  	);
	settings->setValue("oplmode"		, datosDbx["sblaster_oplmode"]  	);
	settings->setValue("oplrate"		, datosDbx["sblaster_oplrate"] 	);
	settings->endGroup();

	settings->beginGroup("gus");
	settings->setValue("gus"			, datosDbx["gus_gus"]  		);
	settings->setValue("gusrate"		, datosDbx["gus_gusrate"]  	);
	settings->setValue("gusbase"		, datosDbx["gus_gusbase"]  	);
	settings->setValue("irq1"			, datosDbx["gus_irq1"]  	);
	settings->setValue("irq2"			, datosDbx["gus_irq2"]  	);
	settings->setValue("dma1"			, datosDbx["gus_dma1"]		);
	settings->setValue("dma2"			, datosDbx["gus_dma2"]  	);
	settings->setValue("ultradir"		, datosDbx["gus_ultradir"]  );
	settings->endGroup();
	
	settings->beginGroup("speaker");
	settings->setValue("pcspeaker"		, datosDbx["speaker_pcspeaker"] );
	settings->setValue("pcrate"			, datosDbx["speaker_pcrate"]  	);
	settings->setValue("tandy"			, datosDbx["speaker_tandy"]  	);
	settings->setValue("tandyrate"		, datosDbx["speaker_tandyrate"] );
	settings->setValue("disney"			, datosDbx["speaker_disney"]  	);
	settings->endGroup();

	settings->beginGroup("joystick");
	settings->setValue("joysticktype"	, datosDbx["joystick_type"]);
	settings->setValue("timed"			, datosDbx["joystick_timed"]  	);
	settings->setValue("autofire"		, datosDbx["joystick_autofire"]  );
	settings->setValue("swap34"			, datosDbx["joystick_swap34"]  	);
	settings->setValue("buttonwrap"		, datosDbx["joystick_buttonwrap"]);
	settings->endGroup();

	settings->beginGroup("modem"); // DOSBox 0.63
	settings->setValue("modem"			, datosDbx["modem_modem"]  );
	settings->setValue("comport"		, datosDbx["modem_comport"]  );
	settings->setValue("listenport"		, datosDbx["modem_listenport"]  );
	settings->endGroup();
	
	settings->beginGroup("directserial"); // DOSBox 0.63
	settings->setValue("directserial"	, datosDbx["dserial_directserial"]  );
	settings->setValue("comport"		, datosDbx["dserial_comport"]  		);
	settings->setValue("realport"		, datosDbx["dserial_realport"]  	);
	settings->setValue("defaultbps"		, datosDbx["dserial_defaultbps"]  	);
	settings->setValue("parity"			, datosDbx["dserial_parity"]  		);
	settings->setValue("bytesize"		, datosDbx["dserial_bytesize"]  	);
	settings->setValue("stopbit"		, datosDbx["dserial_stopbit"] 		);
	settings->endGroup();

	settings->beginGroup("serial");
	settings->setValue("serial1"		, datosDbx["serial_1"]  );
	settings->setValue("serial2"		, datosDbx["serial_2"]  );
	settings->setValue("serial3"		, datosDbx["serial_3"]  );
	settings->setValue("serial4"		, datosDbx["serial_4"]  );
	settings->endGroup();

	settings->beginGroup("dos");
	settings->setValue("xms"			, datosDbx["dos_xms"]  );
	settings->setValue("ems"			, datosDbx["dos_ems"]  );
	settings->setValue("umb"			, datosDbx["dos_umb"]  );
	settings->setValue("keyboardlayout"	, datosDbx["dos_keyboardlayout"]  );
	settings->endGroup();

	settings->beginGroup("ipx");
	settings->setValue("ipx"			, datosDbx["ipx_ipx"]  );
	settings->endGroup();

	settings->sync();

	if(datosDbx["opt_autoexec"]== "true"){
		QFile file( PathSaveConfg );
		if (file.open(QFile::WriteOnly | QFile::Append )) {
			QTextStream stAutoexec( &file );
			stAutoexec << "\n[autoexec]\n";
			stAutoexec << datosDbx["autoexec"] << "\n";
			stAutoexec.flush();
			file.close();
			delete settings;
		}else{
			delete settings;
			return ;
		}
	}else{
		// Creando el Autoexec
		QStringList listamontaje;
		QHash<QString, QString> datos_montaje;
		datos_montaje.clear();
		datos_montaje["path_exe"]        = datosDbx["path_exe"];
		datos_montaje["parametros_exe"]  = datosDbx["parametros_exe"];
		datos_montaje["opt_loadfix_mem"] = datosDbx["opt_loadfix_mem"];
		datos_montaje["opt_loadfix"]     = datosDbx["opt_loadfix"];
		datos_montaje["opt_cerrar_dbox"] = datosDbx["opt_cerrar_dbox"];
		
		listamontaje.clear();
		listamontaje = fGrl.CreaConfigMontajes( treeWidget, datos_montaje );

		QFile file( PathSaveConfg );
		if (file.open(QFile::WriteOnly | QFile::Append )) {
			QTextStream stAutoexec( &file );
			stAutoexec << "\n[autoexec]\n";
			int i = 0;
			for (i = 0; i < listamontaje.size(); ++i){
			  stAutoexec << listamontaje.at(i).toLocal8Bit().constData() << "\n";
			}
			stAutoexec.flush();
			file.close();
			delete settings;
		}else{
			delete settings;
			return ;
		}
	}
}

void GrLida::on_AddNewSvm()
{
	frmSvmAdd *AddSvmNew = new frmSvmAdd();
    if ( AddSvmNew->exec() == QDialog::Accepted ) {
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddSvmNew->DatosJuego );
		sql->ItemInsertaSvm( AddSvmNew->DatosScummvm, lastID);
		NuevoItemTreeWidget( AddSvmNew->DatosJuego, "scummvm", lastID);
    }
}

void GrLida::NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem){
	QString stIcono;
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
	
	if ( imgEmu == "" ) 
		stIcono = ":/img24/emu_sin_imagen.png";
	else if ( imgEmu == "datos" ) 
		stIcono = ":/img24/emu_datos.png";
	else if ( imgEmu == "dosbox" )
		stIcono = ":/img24/emu_dbx.png";
	else if ( imgEmu == "scummvm" )
		stIcono = ":/img24/emu_svm.png";
	else if ( imgEmu =="vdmsound" ) 
		stIcono = ":/img24/emu_vdms.png";
	else{
		bool existeIcono;
		existeIcono = QFile::exists(stIconDir + imgEmu);
		if ( existeIcono )
			stIcono = stIconDir + imgEmu ;
		else stIcono = ":/img24/emu_sin_imagen.png";
	}
	item->setText( 0 , IDitem		    ) ; // idgrl
	item->setIcon( 0 , QIcon(stIcono)	) ; // icono
	item->setText( 1 , datos["titulo"]  ) ; // titulo
	item->setText( 2 , datos["tipo_emu"]) ; // tipo_emu
	
	lbpanel_2.setText( tr(" N Juegos: ") + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
}

void GrLida::on_AddNewVdms()
{
	frmVdmsAdd *AddVdmsNew = new frmVdmsAdd();
    if ( AddVdmsNew->exec() == QDialog::Accepted ) {
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddVdmsNew->DatosJuego );
		sql->ItemInsertaVdms( AddVdmsNew->DatosVDMSound, lastID);
		CrearArchivoConfigVdmS( AddVdmsNew->DatosVDMSound, stConfgVdmSDir + AddVdmsNew->DatosVDMSound["path_conf"] );
		NuevoItemTreeWidget( AddVdmsNew->DatosJuego, "vdmsound", lastID);
    }
}

void GrLida::on_EjecutarJuego()
{
	if(stTipoEmu!="datos"){
		if(stConfgJuego!=""){
		// Ejecuta el juego con el emulador DOSBox
			if(stTipoEmu=="dosbox")  Ejecutar( stBinExeDbx, stConfgJuego);
		// Ejecuta el juego con el emulador ScummVM 
			if(stTipoEmu=="scummvm") Ejecutar( stBinExeSvm, stConfgJuego);
		// Ejecuta el juego con el emulador vdmsound 	
			if(stTipoEmu=="vdmsound"){
				#ifdef Q_OS_WIN32
					if ((long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(stConfgJuego.utf16()), 0, 0, SW_SHOWNORMAL) <= 32)
						QMessageBox::information(this, stTituloGrl(), tr("No se ha podido ejecutar el juego")); 		
				#else
					QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)")); 			
				#endif	
			} 
		}else if(stConfgJuego==""){
			QMessageBox::information( this, stTituloGrl(),tr("No dispones de ninguna configuracin"));
		}else{
			QMessageBox::information( this, stTituloGrl(),tr("Porfavor, seleccione un juego de la lista antes."));
		}
	}
}

void GrLida::on_EjecutarSetup()
{
	QSqlQuery query;
	QSqlRecord rec;

	if(stTipoEmu!="datos"){
//		if(stConfgJuego!=""){
		// Ejecuta el juego con el emulador DOSBox
			if(stTipoEmu=="dosbox")
			{
				QHash<QString, QString> ConfgJuegoSetup;
				QString stConfgSetup;
				
				ConfgJuegoSetup.clear();
				stConfgSetup.clear();
				
				query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stItemIndex+" LIMIT 0,1");
				query.first();
				rec = query.record();

				//ConfgJuegoSetup["path_exe"]   = query.value( rec.indexOf("path_exe") ).toString()	;	// path_exe
				ConfgJuegoSetup["path_setup"] = query.value( rec.indexOf("path_setup") ).toString()	;	// path_setup

				stConfgSetup = ConfgJuegoSetup["path_setup"];

				Ejecutar( stBinExeDbx, stConfgSetup);
			}
		// Ejecuta el juego con el emulador ScummVM 
//			if(stTipoEmu=="scummvm")
//			{
//				//
//			}
		// Ejecuta el juego con el emulador vdmsound 	
//			if(stTipoEmu=="vdmsound")
//			{
//				#ifdef Q_OS_WIN32
//
//				#else
//					QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)")); 			
//				#endif	
//			} 
//		}else if(stConfgJuego==""){
//			QMessageBox::information( this, stTituloGrl(),tr("No dispones de ninguna configuracin"));
//		}else{
//			QMessageBox::information( this, stTituloGrl(),tr("Porfavor, seleccione un juego de la lista antes."));
//		}
	}
}

void GrLida::on_EjecutarDosbox()
{
	Ejecutar( stBinExeDbx, "");
	ui.txtDatos_1->setText( stBinExeDbx );
}

void GrLida::on_EjecutarScummVM()
{
	Ejecutar( stBinExeSvm, "");
}

void GrLida::Ejecutar( const QString& bin, const QString &parametros)
{
	dBoxSvm = new QProcess( this );
	
	QFile appBin( bin );
	if ( appBin.exists() ){
		if(parametros !=""){
		  stl_param << parametros.split("|", QString::SkipEmptyParts); 
		  dBoxSvm->start( bin , stl_param );
		  stl_param.clear();
		}else dBoxSvm->start( bin );
	} else
		QMessageBox::information( this, stTituloGrl(),tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void GrLida::Confg_Svm_Dbx(QString IDitem)
{
	QSqlQuery query;
	QSqlRecord rec;
// Limpiamos primero la informacion que pueda tener stConfgJuego
	stConfgJuego.clear();
// Si el emulador es el ScummVM:
	if(stTipoEmu=="scummvm"){
		QHash<QString, QString> conf_scummvm;

		query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+IDitem+" LIMIT 0,1");
		query.first();
		rec = query.record();
		conf_scummvm.clear();
		conf_scummvm["game"] = query.value( rec.indexOf("game") ).toString();	// game
		if(query.value( rec.indexOf("language") ).toString()!="") conf_scummvm["language"] = "-q" + query.value( rec.indexOf("language") ).toString() + "|" ; else conf_scummvm["language"] = "|"			; // language
		if(query.value( rec.indexOf("subtitles") ).toBool()==true) conf_scummvm["subtitles"] = "-n|" ; else conf_scummvm["subtitles"] = "|"  																; // subtitles
		if(query.value( rec.indexOf("platform") ).toString()!="") conf_scummvm["platform"] = "--platform=" + query.value( rec.indexOf("platform") ).toString() + "|"; else conf_scummvm["platform"] = "|" 	; // platform
		if(query.value( rec.indexOf("gfx_mode") ).toString()!="") conf_scummvm["gfx_mode"] = "-g" + query.value(6).toString() + "|"; else conf_scummvm["gfx_mode"] = "|"								 	; // gfx_mode
		if(query.value( rec.indexOf("render_mode") ).toString()!="") conf_scummvm["render_mode"] = "--render-mode=" + query.value(7).toString() + "|"; else conf_scummvm["render_mode"] = "|"			 	; // render_mode
		if(query.value( rec.indexOf("fullscreen") ).toBool()==true) conf_scummvm["fullscreen"] = "-f|" ; else conf_scummvm["fullscreen"] = "-F|"														 	; // fullscreen
		if(query.value( rec.indexOf("aspect_ratio") ).toBool()==true) conf_scummvm["aspect_ratio"] = "--aspect-ratio|"; else conf_scummvm["aspect_ratio"] = "--no-aspect-ratio|"						 	; // aspect_ratio
		if(query.value( rec.indexOf("path_save") ).toString()!="") conf_scummvm["path_save"] = "--savepath=" + query.value(13).toString() + "|"; else conf_scummvm["path_save"] = "|"						;	// path_save
		if(query.value( rec.indexOf("enable_gs") ).toBool()==true) conf_scummvm["enable_gs"] = "--enable-gs|"  ; else conf_scummvm[""] = "|"		;	// enable_gs
		if(query.value( rec.indexOf("multi_midi") ).toBool()==true) conf_scummvm["multi_midi"] = "--multi-midi|" ; else conf_scummvm[""] = "|"		;	// multi_midi
		if(query.value( rec.indexOf("native_mt32") ).toBool()==true) conf_scummvm["native_mt32"] = "--native-mt32|"; else conf_scummvm[""] = "|"	;	// native_mt32
		conf_scummvm["music_driver"] = "-e" + query.value( rec.indexOf("music_driver") ).toString() + "|"											;	// music_driver
		conf_scummvm["master_volume"]= fGrl.IntToStr( query.value( rec.indexOf("master_volume") ).toInt() ) + "|"					;	// master_volume
		conf_scummvm["music_volume"] = "-m" + fGrl.IntToStr( query.value( rec.indexOf("music_volume") ).toInt() ) + "|"				;	// music_volume
		conf_scummvm["sfx_volume"]   = "-s" + fGrl.IntToStr( query.value( rec.indexOf("sfx_volume") ).toInt() ) + "|"				;	// sfx_volume
		conf_scummvm["speech_volume"]= "-r" + fGrl.IntToStr( query.value( rec.indexOf("speech_volume") ).toInt() ) + "|"				;	// speech_volume
		conf_scummvm["tempo"]        = "--tempo=" + fGrl.IntToStr( query.value( rec.indexOf("tempo") ).toInt() ) + "|"				;	// tempo
		conf_scummvm["talkspeed"]    = "--talkspeed=" + fGrl.IntToStr( query.value( rec.indexOf("talkspeed") ).toInt() ) + "|"		;	// talkspeed
		conf_scummvm["debuglevel"]   = "-d" + fGrl.IntToStr( query.value( rec.indexOf("debuglevel") ).toInt() ) + "|"				;	// debuglevel
		conf_scummvm["cdrom"]        = "--cdrom=" + fGrl.IntToStr( query.value( rec.indexOf("cdrom") ).toInt() ) + "|"				;	// cdrom
		conf_scummvm["joystick_num"] = "--joystick=" + fGrl.IntToStr( query.value( rec.indexOf("joystick_num") ).toInt() ) + "|"		;	// joystick_num
		conf_scummvm["path"]          = "--path=" + query.value( rec.indexOf("path") ).toString() + "|"			;	// path
		conf_scummvm["path_setup"]    = query.value( rec.indexOf("path_setup") ).toString()						;	// path_setup
		conf_scummvm["path_extra"]    = query.value( rec.indexOf("path_extra") ).toString() 					;	// path_extra
		conf_scummvm["path_capturas"] = query.value( rec.indexOf("path_capturas") ).toString()					;	// path_capturas
		conf_scummvm["path_sonido"]   = query.value( rec.indexOf("path_sonido") ).toString()					;	// path_sonido
		
		stConfgJuego= conf_scummvm["path"]			+	// path
					conf_scummvm["language"]		+	// language
					conf_scummvm["subtitles"]		+	// subtitles
					conf_scummvm["platform"] 		+	// platform
					conf_scummvm["gfx_mode"]		+	// gfx_mode
					conf_scummvm["render_mode"]		+	// render_mode
					conf_scummvm["fullscreen"]		+	// fullscreen
					conf_scummvm["aspect_ratio"]	+	// aspect_ratio
					conf_scummvm["path_save"]		+	// path_save
					conf_scummvm["music_driver"]	+	// music_driver
					conf_scummvm["enable_gs"]		+	// enable_gs
					conf_scummvm["multi_midi"]		+	// multi_midi
					conf_scummvm["native_mt32"]		+	// native_mt32
			 		conf_scummvm["music_volume"]	+	// music_volume
			 		conf_scummvm["sfx_volume"]		+	// sfx_volume
					conf_scummvm["speech_volume"]	+	// speech_volume
			 		conf_scummvm["tempo"]			+	// tempo
					conf_scummvm["talkspeed"]		+	// talkspeed
					conf_scummvm["debuglevel"]		+	// debuglevel
			 		conf_scummvm["cdrom"]			+	// cdrom
			 		conf_scummvm["joystick_num"]	+	// joystick_num*/
			 		conf_scummvm["game"];				// game
	
		//	conf_scummvm["path_sonido"];				// path_sonido
		stCapturasSvm =  conf_scummvm["path_capturas"]; // path_capturas
		stCapturasDbx = "";
		if( (stCapturasSvm !="") && ui.btn_imgtumb_2->isChecked() )
			CargarThumbsTreeWidget( stCapturasSvm );
		else ui.twCapturas->clear();
		
		if ( QFile( conf_scummvm["path_setup"] ).exists() )
			ui.actionEjectarSetup->setEnabled(true);
		else
			ui.actionEjectarSetup->setEnabled(false);	
	}else
// Si el emulador es el DOSBox:
	if(stTipoEmu=="dosbox"){
		QHash<QString, QString> conf_dosbox;

		query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+IDitem+" LIMIT 0,1");
		query.first();
		rec = query.record();
		conf_dosbox.clear();
		conf_dosbox["path_conf"]        = query.value( rec.indexOf("path_conf") ).toString()		;	// path_conf
		conf_dosbox["path_exe"]         = query.value( rec.indexOf("path_exe") ).toString()			;	// path_exe
		conf_dosbox["path_setup"]       = query.value( rec.indexOf("path_setup") ).toString()		;	// path_setup
		conf_dosbox["opt_consola_dbox"] = query.value( rec.indexOf("opt_consola_dbox") ).toString()	;	// opt_consola_dbox
		conf_dosbox["dosbox_captures"]  = query.value( rec.indexOf("dosbox_captures") ).toString()	;	// dosbox_capture

		QString consolaDbx;
		#ifdef Q_OS_WIN32
			if(conf_dosbox["opt_consola_dbox"]=="true") consolaDbx = "|-noconsole"; else consolaDbx = "" ;
		#else
			consolaDbx = "" ;
		#endif
		stConfgJuego = "-conf|" + stConfgDbxDir + conf_dosbox["path_conf"] + consolaDbx;

		if ( !QFile( stConfgDbxDir + conf_dosbox["path_conf"] ).exists() )
			QMessageBox::information( this, stTituloGrl(), tr("Configuracin del juego no encontrada!\n\n'") + stConfgDbxDir + conf_dosbox["path_conf"] + "'" );
		
		if ( QFile( conf_dosbox["path_setup"] ).exists() )
			ui.actionEjectarSetup->setEnabled(true);
		else
			ui.actionEjectarSetup->setEnabled(false);	
		
		stCapturasDbx = conf_dosbox["dosbox_captures"];  //3 dosbox_capture
		stCapturasSvm = "";
		
		if( (stCapturasDbx !="") && ui.btn_imgtumb_1->isChecked()  )
			CargarThumbsTreeWidget( stCapturasDbx );
		else ui.twCapturas->clear();
	}else
// Si el emulador es el vdmsound:	
	if(stTipoEmu=="vdmsound"){
		query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDitem+" LIMIT 0,1");
		query.first();
		rec = query.record();		
		
		stConfgJuego = stConfgVdmSDir + query.value( rec.indexOf("path_conf") ).toString();

		ui.actionEjectarSetup->setEnabled(false);
	}else{
		stCapturasSvm = "";
		stCapturasDbx = "";
		stConfgJuego = "";
		ui.twCapturas->clear();
	}
	
}

void GrLida::on_NuevoJuego()
{
	QString lastID, lastID_Dbx;
	frmAddEditJuego * AddJuego = new frmAddEditJuego( false, "datos", "", 0, Qt::Window);
	
	
	if ( AddJuego->exec() == QDialog::Accepted ) {

		lastID = sql->ItemInsertaDatos( AddJuego->DatosJuego );
	
		sql->ItemInsertaFiles( AddJuego->ui.twDatosFiles, lastID );
		sql->ItemInsertaURL( AddJuego->ui.twDatosURL, lastID );

		if (AddJuego->DatosJuego["tipo_emu"]=="dosbox"){
			lastID_Dbx = sql->ItemInsertaDbx( AddJuego->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx( AddJuego->ui.twMontajes, lastID_Dbx );
			CrearArchivoConfigDbx( AddJuego->DatosDosBox, AddJuego->ui.twMontajes, stConfgDbxDir + AddJuego->DatosDosBox["path_conf"] );
		}
		if (AddJuego->DatosJuego["tipo_emu"]=="scummvm"){
			sql->ItemInsertaSvm( AddJuego->DatosScummvm, lastID);
		}
		if (AddJuego->DatosJuego["tipo_emu"]=="vdmsound"){
			sql->ItemInsertaVdms( AddJuego->DatosVDMSound, lastID);
			CrearArchivoConfigVdmS( AddJuego->DatosVDMSound, stConfgVdmSDir + AddJuego->DatosVDMSound["path_conf"] );
		}
		NuevoItemTreeWidget( AddJuego->DatosJuego, AddJuego->DatosJuego["tipo_emu"], lastID);
	}
}

void GrLida::on_EditarJuego()
{
	QString stIcono, stSelectIcon;
	if((ui.twJuegos->topLevelItemCount()>0) && (stItemIndex!="")){
		frmAddEditJuego * EditJuego = new frmAddEditJuego( true, stTipoEmu, stItemIndex, 0, Qt::Window);
		if ( EditJuego->exec() == QDialog::Accepted ) {
			sql->ItemActualizaDatos(EditJuego->DatosJuego, stItemIndex);
			ui.twJuegos->currentItem()->setText(1, EditJuego->DatosJuego["titulo"] );
			
			if (stTipoEmu=="dosbox"){
				sql->ItemActualizaDbx( EditJuego->DatosDosBox, EditJuego->stItemIDDbx );
				sql->ItemActualizaMontajeDbx(EditJuego->ui.twMontajes);
				CrearArchivoConfigDbx( EditJuego->DatosDosBox, EditJuego->ui.twMontajes, stConfgDbxDir + EditJuego->DatosDosBox["path_conf"] );
			}
			if (stTipoEmu=="scummvm"){
				sql->ItemActualizaSvm( EditJuego->DatosScummvm, EditJuego->stItemIDSvm );
			}
			if (stTipoEmu=="vdmsound"){
				sql->ItemActualizaVdms( EditJuego->DatosVDMSound, EditJuego->stItemIDVdms );
				CrearArchivoConfigVdmS( EditJuego->DatosVDMSound, stConfgVdmSDir + EditJuego->DatosVDMSound["path_conf"] );
			}
			stIcono = EditJuego->DatosJuego["icono"];
			if(stIcono=="")
				stSelectIcon = ":/img24/emu_sin_imagen.png";
			else if ( stIcono == "datos" ) 
				stSelectIcon = ":/img24/emu_datos.png";
			else if ( stIcono == "dosbox" )
				stSelectIcon = ":/img24/emu_dbx.png";
			else if ( stIcono == "scummvm" )
				stSelectIcon = ":/img24/emu_svm.png";
			else if ( stIcono == "vdmsound" ) 
				stSelectIcon = ":/img24/emu_vdms.png";
			else {
				bool existeIcono;
				existeIcono = QFile::exists(stIconDir + stIcono);
				if ( existeIcono )
					stSelectIcon = stIconDir + stIcono ;
				else stSelectIcon = ":/img24/emu_sin_imagen.png" ;
			}	
			ui.twJuegos->currentItem()->setIcon( 0, QIcon( stSelectIcon ) );
		}
	}
}

void GrLida::on_EliminarJuego()
{
	if( ui.twJuegos->topLevelItemCount()>0 ){
		if ( stItemIndex == "" ) {
			QMessageBox::information( this, stTituloGrl(), tr( "Porfavor selecciona un Juego de la lista para eliminarlo" ) );
		} else {
			int EliminarSiNO;
			EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar Juego...?"), tr("¿Deseas realmente eliminar este juego de la Base de Datos?\nSi es de DOSBox o VDMSound tambien se borrar el archivo de configuracin" ), tr( "Si" ), tr( "Cancelar" ), 0, 1 );
			if( EliminarSiNO == 0 ){
				sql->ItemEliminar(stItemIndex);
				fGrl.DeleteItemTree( ui.twJuegos->currentItem() ) ;
 				QMessageBox::information( this, stTituloGrl(), tr("Juego Eliminado correctamente"));
				if( ui.twJuegos->topLevelItemCount()<=0 ){
					lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );	
					lbpanel_5.setText(" ");	
				}
				lbpanel_2.setText( tr(" N Juegos: ") + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
			}
		}
	}else{
		lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );
		lbpanel_5.setText(" ");	
	}
}

void GrLida::on_Opciones()
{
	frmOpciones * Opciones = new frmOpciones();
    if ( Opciones->exec() == QDialog::Accepted ) {
		if (Opciones->DatosConfiguracion["DOSBoxDisp"] == "true")
			ui.actionNewDbx->setEnabled( true ); else ui.actionNewDbx->setEnabled( false ); //0 DOSBox
		if (Opciones->DatosConfiguracion["ScummVMDisp"] == "true")
			ui.actionNewSvm->setEnabled( true ); else ui.actionNewSvm->setEnabled( false ); //1 ScummVM
		if (Opciones->DatosConfiguracion["VDMSoundDisp"] == "true")
			ui.actionNewVdms->setEnabled( true ); else ui.actionNewVdms->setEnabled( false );//2 vdmsound
	
		stBinExeDbx   = Opciones->DatosConfiguracion["DirDOSBox"]  ; //4 DirDbx
		stBinExeSvm   = Opciones->DatosConfiguracion["DirScummVM"] ; //5 DirSvm
	#ifdef Q_OS_WIN32
		if( (Opciones->DatosConfiguracion["DOSBoxDisp"]=="false")&&(Opciones->DatosConfiguracion["ScummVMDisp"]=="false")&&(Opciones->DatosConfiguracion["VDMSoundDisp"]=="false") )
			ui.actionEjectar->setEnabled( false ); else ui.actionEjectar->setEnabled( true );
	#else
		if( (Opciones->DatosConfiguracion["DOSBoxDisp"]=="false")&&(Opciones->DatosConfiguracion["ScummVMDisp"]=="false"))
			ui.actionEjectar->setEnabled( false ); else ui.actionEjectar->setEnabled( true );
	#endif
	}
}

void GrLida::on_AcercaD()
{
	frmAcercaD * AcercaD = new frmAcercaD();
	AcercaD->ui.label_4->setText( "v" + fGrl.stVersionGrl()  );
	AcercaD->ui.lb_Dbx->setText( "Dosbox v" + fGrl.stVersionDbx()  );
	AcercaD->ui.lb_Svm->setText( "ScummVM v" + fGrl.stVersionSvm() );
	AcercaD->ui.lb_Vdms->setText("VDMSound v" + fGrl.stVersionVdms());// v2.0.4
	AcercaD->exec() ;
}

void GrLida::on_Informacion()
{
	frmInfo * NewInfo = new frmInfo();
	NewInfo->exec() ;
}

void GrLida::MostrarDatosDelJuego(QString IDitem)
{
	QSqlQuery query;
	QSqlRecord rec;
	query.exec("SELECT * FROM dbgrl WHERE idgrl="+IDitem+" LIMIT 0,1");
	if ( query.first() ){
		rec = query.record();
		ui.txtDatos_1->setText(  query.value( rec.indexOf("subtitulo") ).toString() )		; // subtitulo 
		ui.txtDatos_2->setText(  query.value( rec.indexOf("genero") ).toString()    )		; // genero
		ui.txtDatos_3->setText(  query.value( rec.indexOf("idioma") ).toString()    )		; // idioma
		ui.txtDatos_4->setText(  query.value( rec.indexOf("formato") ).toString()   )		; // formato
		ui.txtDatos_5->setText(  query.value( rec.indexOf("numdisc") ).toString()   )		; // numdisc
		ui.txtDatos_6->setText(  query.value( rec.indexOf("anno") ).toString()      )		; // anno
		ui.txtDatos_7->setText(  query.value( rec.indexOf("tamano") ).toString()    )		; // tamano
		ui.txtDatos_8->setText(  query.value( rec.indexOf("compania") ).toString()  )		; // compania
		ui.txtDatos_9->setText(  query.value( rec.indexOf("desarrollador") ).toString())	; // desarrollador
		ui.txtDatos_10->setText( query.value( rec.indexOf("tema") ).toString()      )		; // tema
		ui.txtDatos_11->setText( query.value( rec.indexOf("sistemaop") ).toString() )		; // sistemaop
		
		QString strTempHtml = query.value( rec.indexOf("comentario") ).toString();

		for (i_Hash = listSmailes.constBegin(); i_Hash != listSmailes.constEnd(); ++i_Hash){
			QString strSmaile = i_Hash.value();
			if( strSmaile.contains(":/smiles", Qt::CaseInsensitive) )
				strTempHtml.replace( i_Hash.key(), "<img src=\""+i_Hash.value()+"\" />");
			else
				strTempHtml.replace( i_Hash.key(), "<img src=\""+stHomeDir+i_Hash.value()+"\" />");
		}
		ui.txtDatos_12->setHtml( strTempHtml ); // comentario		
			
		QFile file_thumbs;  // thumbs
		if( file_thumbs.exists( query.value( rec.indexOf("thumbs") ).toString() ) ){
			ui.lbImgThumbs->setPixmap( QPixmap( query.value( rec.indexOf("thumbs") ).toString() ) );
		}else{
			ui.lbImgThumbs->setPixmap( QPixmap(":/images/juego_sin_imagen.png") );
		}
		/*
		fGrl.IntToStr(query.value(0).toInt()); // idgrl
		query.value(2).toString()       ; // titulo
		query.value(23).toString()      ; // tipo_emu
		query.value(3).toString()       ; // subtitulo
		query.value(4).toString()       ; // genero
		query.value(5).toString()       ; // compania
		query.value(6).toString()       ; // desarrollador
		query.value(7).toString()       ; // tema
		query.value(8).toString()       ; // idioma
		query.value(9).toString()       ; // formato
		query.value(10).toString()      ; // anno
		query.value(11).toString()      ; // numdisc
		query.value(12).toString()      ; // sistemaop
		query.value(13).toString()      ; // tamano
		query.value(14).toString()      ; // graficos
		query.value(15).toString()      ; // sonido
		query.value(16).toString()      ; // jugabilidad
		query.value(17).toString()      ; // original
		query.value(18).toString()      ; // estado
		query.value(19).toString()      ; // thumbs
		query.value(20).toString()      ; // cover_front
		query.value(21).toString()      ; // cover_back
		query.value(22).toString()      ; // fecha
		query.value(23).toString()      ; // tipo_emu
		query.value(24).toString()      ; // comentario
		*/
		lbpanel_5.setText(" " + query.value(2).toString() + " - introducido el " + query.value(22).toString() + " " );	
		if(query.value(23).toString()== "datos")
			lbpanel_3.setPixmap( QPixmap(":/img16/datos_1.png") );
		else if(query.value(23).toString()== "dosbox")
			lbpanel_3.setPixmap( QPixmap(":/img16/dosbox.png") );
		else if(query.value(23).toString()== "scummvm")
			lbpanel_3.setPixmap( QPixmap(":/img16/scummvm.png") );
		else if(query.value(23).toString()== "vdmsound")
			lbpanel_3.setPixmap( QPixmap(":/img16/vdmsound.png") );
		else
			lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );
	}
	
	ui.twUrls->clear();
	query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+IDitem);
	if (query.first()) {
		do {
			rec = query.record();
			QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twUrls );
			item_url->setText( 0 , query.value( rec.indexOf("url") ).toString()        ) ; // url
			item_url->setIcon( 0 , QIcon(":/img16/edit_enlace.png")                    ) ; // icono
			item_url->setText( 1 , query.value( rec.indexOf("descripcion") ).toString()) ; // descripcion
		//	item_url->setText( 2 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    ) ; // id
		//	item_url->setText( 3 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) ) ; // idgrl
		} while (query.next());
		ui.twUrls->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}
	
	ui.twFiles->clear();
	query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+IDitem);
	if (query.first()) {
		do {
			rec = query.record();
			QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twFiles );
			item_files->setText( 0 , query.value( rec.indexOf("nombre") ).toString()     ) ; // nombre
			item_files->setIcon( 0 , QIcon(":/img16/importar.png")                       ) ; // icono
			item_files->setText( 1 , query.value( rec.indexOf("crc") ).toString()        ) ; // crc
			item_files->setText( 2 , query.value( rec.indexOf("descripcion") ).toString()) ; // descripcion
			item_files->setText( 3 , query.value( rec.indexOf("size") ).toString()       ) ; // size
			item_files->setText( 4 , query.value( rec.indexOf("path") ).toString()       ) ; // path		
		//	item_files->setText( 5 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    ) ; // id
		//	item_files->setText( 6 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) ) ; // idgrl
		} while (query.next());
		ui.twFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}
}
void GrLida::on_btn_fileurl_1()
{
	if( ui.btn_fileurl_1->isChecked()==true){
		ui.btn_fileurl_2->setChecked(false);
		ui.twUrls->setVisible(true);
		ui.twFiles->setVisible(false);
	}else{
		ui.btn_fileurl_2->setChecked(true);
		ui.twUrls->setVisible(false);
		ui.twFiles->setVisible(true);
	}
}

void GrLida::on_btn_fileurl_2()
{
	if( ui.btn_fileurl_2->isChecked()== true ){
		ui.btn_fileurl_1->setChecked(false);
		ui.twUrls->setVisible(false);
		ui.twFiles->setVisible(true);
	}else{
		ui.btn_fileurl_1->setChecked(true);
		ui.twUrls->setVisible(true);
		ui.twFiles->setVisible(false);
	}
}
void GrLida::on_btn_imgtumb_1()
{
	if( ui.btn_imgtumb_1->isChecked()==true){
		ui.btn_imgtumb_2->setChecked(false);
		CargarThumbsTreeWidget( stCapturasDbx ); //carga imagenes dosbox
	}else{
		ui.btn_imgtumb_2->setChecked(true);
		CargarThumbsTreeWidget( stCapturasSvm ); //carga imagenes scummvm
	}
}

void GrLida::on_btn_imgtumb_2()
{
	if( ui.btn_imgtumb_2->isChecked()== true ){
		ui.btn_imgtumb_1->setChecked(false);
		CargarThumbsTreeWidget( stCapturasSvm ); //carga imagenes scummvm
	}else{
		ui.btn_imgtumb_1->setChecked(true);
		CargarThumbsTreeWidget( stCapturasDbx ); //carga imagenes dosbox
	}
}

void GrLida::CargarThumbsTreeWidget(const QString directorio){
	if ((directorio!="") && ui.dockw_Capturas->isVisible()==true ){
	 	ui.twCapturas->clear();
		QStringList filters;
		filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas
		QDir dir( directorio );
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Size | QDir::Reversed);
		dir.setNameFilters(filters);
		if( dir.exists() ){
			QFileInfoList list = dir.entryInfoList();
			for (int i = 0; i < list.size(); ++i) {
				QFileInfo fileInfo = list.at(i);
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twCapturas );
				item->setIcon( 0 , QIcon( directorio + "/" + fileInfo.fileName() )) ; //
			}
		}
	}else ui.twCapturas->clear();
}

void GrLida::on_twJuegos_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
		emit on_twJuegos_clicked( ui.twJuegos->currentItem() );
	else return;
}

void GrLida::on_twJuegos_clicked( QTreeWidgetItem *item)
{
	if( item ){
		stItemIndex  = ""					; // Limpiamos el idgrl
		stTipoEmu    = ""					; // Limpiamos el tipo_emu
		stItemIndex  = item->text(0)		; // idgrl del juego en la Base de Datos
		stTipoEmu    = item->text(2)		; // tipo_emu
		Confg_Svm_Dbx( stItemIndex )		; // carga toda la configuracion
		MostrarDatosDelJuego( stItemIndex )	; // Muestra los distintos datos del juego 	
	}else{
		lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );	
		lbpanel_5.setText(" ");	
		return;
	}
}

void GrLida::on_twJuegos_Dblclicked(QTreeWidgetItem *item)
{
	if( item ){
		stItemIndex  = ""					; // Limpiamos el idgrl
		stTipoEmu    = ""					; // Limpiamos el tipo_emu
		stItemIndex  = item->text(0)		; // index del juego en la Base de Datos 
		stTipoEmu    = item->text(2)		; // indica el tipo de emulador
		Confg_Svm_Dbx( stItemIndex )		; // carga toda la configuracion
		on_EjecutarJuego()					; // ejecuta el juego
	}else return;
}

void GrLida::CargarBaseDatos()
{
	if (! sql->dbisOpen()) {
		QMessageBox::critical(0, tr("Error abrir la Base de Datos"), tr("No se ha podido establecer una conesin con la Base de Datos.\nEsta aplicacion necesita soporte de SQLite.Mira la documentacion de Qt SQL driver para ms informacin.\n\nClick Cancelar para salir."), QMessageBox::Cancel);
	} else {
		QString stIcono ;
		QSqlQuery query;
		QSqlRecord rec;

		sql->CrearTablas();	// Crea las tablas si no lo estan

		ui.twJuegos->setColumnCount( ui.twJuegos->columnCount() );
		ui.twJuegos->clear();
		query.exec("SELECT * FROM dbgrl");
		if (query.first()) {
			do {
				rec = query.record();
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
				item->setText( 0 , fGrl.IntToStr(query.value( rec.indexOf("idgrl") ).toInt()) )  ; // idgrl
				item->setText( 1 , query.value( rec.indexOf("titulo") ).toString()       )  ; // titulo
				item->setText( 2 , query.value( rec.indexOf("tipo_emu") ).toString()     )  ; // tipo_emu
				stIcono = query.value( rec.indexOf("icono") ).toString()					; // icono
				if(stIcono=="")
					item->setIcon( 0, QIcon(":/img24/emu_sin_imagen.png") );
				else if ( stIcono == "datos" ) 
					item->setIcon( 0, QIcon(":/img24/emu_datos.png") );
				else if ( stIcono == "dosbox" )
					item->setIcon( 0, QIcon(":/img24/emu_dbx.png") );
				else if ( stIcono == "scummvm" )
					item->setIcon( 0, QIcon(":/img24/emu_svm.png") );
				else if ( stIcono == "vdmsound" ) 
					item->setIcon( 0, QIcon(":/img24/emu_vdms.png") );
				else {
					bool existeIcono;
					existeIcono = QFile::exists(stIconDir + stIcono);
					if ( existeIcono )
						item->setIcon( 0, QIcon(stIconDir + stIcono) );
					else item->setIcon( 0, QIcon(":/img24/emu_sin_imagen.png") );
				}
			} while (query.next());
			ui.twJuegos->sortItems( 3, Qt::AscendingOrder ); // Qt::DescendingOrder
			ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
			lbpanel_2.setText( tr(" N Juegos: ") + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
			on_twJuegos_clicked( ui.twJuegos->currentItem() );
		} else {
			lbpanel_2.setText( tr(" Nº Juegos: 0  ") );
			lbpanel_3.setPixmap( QPixmap(":/img16/sinimg.png") );	
			lbpanel_5.setText(" ");	
		}
	}
}

void GrLida::ComprobarArchivosDatos(QString Version_GRL)
{	
	QFile archivodatos;
	if( Version_GRL != fGrl.stVersionGrl() ){
		QDir dirFileDatos( stDatosDir );
//		if( archivodatos.exists( stDatosDir + "svm_gfxmode.txt") )
//			dirFileDatos.remove( stDatosDir + "svm_gfxmode.txt" );
/*		if( archivodatos.exists( stDatosDir + "idioma.txt") )
			dirFileDatos.remove( stDatosDir + "idioma.txt" );
		if( archivodatos.exists( stDatosDir + "svm_lista.txt") )
			dirFileDatos.remove( stDatosDir + "svm_lista.txt" );
		if( archivodatos.exists( stDatosDir + "svm_platform.txt") )
			dirFileDatos.remove( stDatosDir + "svm_platform.txt" );
		if( archivodatos.exists( stDatosDir + "xmldb.txt") )
			dirFileDatos.remove( stDatosDir + "xmldb.txt" );
*/
	}

	if ( !archivodatos.exists(stDatosDir + "generos.txt") )
		CrearArchivoDato("generos.txt");	
	if ( !archivodatos.exists(stDatosDir + "companias.txt") )
		CrearArchivoDato("companias.txt");
	if ( !archivodatos.exists(stDatosDir + "tema.txt") )
		CrearArchivoDato("tema.txt");
	if ( !archivodatos.exists(stDatosDir + "formatos.txt") )
		CrearArchivoDato("formatos.txt");	
	if ( !archivodatos.exists(stDatosDir + "fechas.txt") )
		CrearArchivoDato("fechas.txt");
	if ( !archivodatos.exists(stDatosDir + "numdisc.txt") )
		CrearArchivoDato("numdisc.txt");
	if ( !archivodatos.exists(stDatosDir + "sistemaop.txt") )
		CrearArchivoDato("sistemaop.txt");
	if ( !archivodatos.exists(stDatosDir + "smiles.txt") )
		CrearArchivoDato("smiles.txt");	
	if ( !archivodatos.exists(stDatosDir + "xmldb.txt") )
		CrearArchivoDato("xmldb.txt");
}

void GrLida::CrearArchivoDato(QString archivo)
{
    QString NombreArchivo;
    QFile *CrearArchivo;

    NombreArchivo = ":/datos/"+archivo ;

	QFile LeerArchivo( NombreArchivo );
	if ( !LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        return ;
	}
    QTextStream in( &LeerArchivo );
	in.setCodec("ISO 8859-1");
    QString line = in.readAll();
    LeerArchivo.close();
 
	NombreArchivo.clear();
	NombreArchivo = stDatosDir + archivo;

	CrearArchivo = new QFile( NombreArchivo );
	if ( !CrearArchivo->open( QIODevice::WriteOnly | QIODevice::Text ) ) {
		return ;
	}

	QTextStream out( CrearArchivo );
	out.setCodec("ISO 8859-1");
	out << line;
	out.flush();
    CrearArchivo->close();
    line.clear();
}
