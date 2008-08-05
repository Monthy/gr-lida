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
#include "grlida_exportar_juego.h"
#include "grlida_img_viewer.h"

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
	setWindowTitle( stTituloGrl() + " - " + fGrl.get_Plataforma() + " v" +  fGrl.stVersionGrl() );

// Conecta los distintos botones con las funciones de cada uno
	connect( ui.actionNewDbx, SIGNAL( triggered() ), this, SLOT( on_AddNewDbx() ));
	connect( ui.actionNewSvm, SIGNAL( triggered() ), this, SLOT( on_AddNewSvm() ));
	connect( ui.actionNewVdms, SIGNAL( triggered() ), this, SLOT( on_AddNewVdms() ));
	connect( ui.actionEjectar, SIGNAL( triggered() ), this, SLOT( on_EjecutarJuego() ));
	connect( ui.actionEjectarSetup, SIGNAL( triggered() ), this, SLOT( on_EjecutarSetup() ));
	connect( ui.actionNuevo, SIGNAL( triggered() ), this, SLOT( on_NuevoJuego() ));
	connect( ui.actionEditar, SIGNAL( triggered() ), this, SLOT( on_EditarJuego() ));
	connect( ui.actionEiminar, SIGNAL( triggered() ), this, SLOT( on_EliminarJuego() ));
	connect( ui.actionImportarJuego, SIGNAL(triggered()), this, SLOT( on_ImportarJuego() ));
	connect( ui.actionOpciones, SIGNAL( triggered() ), this, SLOT( on_Opciones() ));
	connect( ui.actionAcercaD, SIGNAL( triggered() ), this, SLOT( on_AcercaD() ));
	connect( ui.btn_Ordenar, SIGNAL( clicked() ), this, SLOT( on_Ordenar_Lista() ));

// Conecta los botones de los menus
	connect( ui.mnu_file_informacion, SIGNAL( triggered() ), this, SLOT( on_Informacion() ));
	connect( ui.mnu_edit_nuevo_dosbox, SIGNAL(triggered()),this, SLOT( on_AddNewDbx() ));
	connect( ui.mnu_edit_nuevo_scummvm, SIGNAL(triggered()),this, SLOT( on_AddNewSvm() ));
	connect( ui.mnu_edit_nuevo_vdmsound, SIGNAL(triggered()),this, SLOT( on_AddNewVdms() ));
	connect( ui.mnu_edit_nuevo, SIGNAL( triggered() ), this, SLOT( on_NuevoJuego() ));
	connect( ui.mnu_edit_editar, SIGNAL( triggered() ), this, SLOT( on_EditarJuego() ));
	connect( ui.mnu_edit_eliminar, SIGNAL( triggered() ), this, SLOT( on_EliminarJuego() ));
	connect( ui.mnu_edit_favorito, SIGNAL(triggered()), this, SLOT( on_setFavorito() ));
	connect( ui.mnu_ejecutar_dosbox, SIGNAL( triggered() ), this, SLOT( on_EjecutarDosbox() ));
	connect( ui.mnu_ejecutar_scummvm, SIGNAL( triggered() ), this, SLOT( on_EjecutarScummVM() ));
	connect( ui.mnu_ejecutar_juego, SIGNAL( triggered() ), this, SLOT( on_EjecutarJuego() ));
	connect( ui.mnu_ejecutar_setup, SIGNAL( triggered() ), this, SLOT( on_EjecutarSetup() ));
	connect( ui.mnu_tool_importar, SIGNAL( triggered() ), this, SLOT( on_ImportarJuego() ));
	connect( ui.mnu_tool_exportar, SIGNAL( triggered() ), this, SLOT( on_ExportarJuego() ));
	connect( ui.mnu_tool_opciones, SIGNAL( triggered() ), this, SLOT( on_Opciones() ));
	connect( ui.mnu_ver_acercad, SIGNAL( triggered() ), this, SLOT( on_AcercaD() ));

// Conecta otros botones
	connect( ui.btnVer_CoverFront, SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverFront() ));
	connect( ui.btnVer_CoverBack , SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverBack() ));

// Conecta la lista de juegos
	connect( ui.twJuegos, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_clicked(QTreeWidgetItem*)));
	connect( ui.twJuegos, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_Dblclicked(QTreeWidgetItem*) ));
	connect( ui.twJuegos, SIGNAL( customContextMenuRequested(const QPoint&)), this, SLOT(showPopup(const QPoint&) ));

// Conecta la lista de capturas
	connect( ui.twCapturas, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twCapturas_Dblclicked(QTreeWidgetItem*) ));
	connect( ui.twUrls, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twUrls_Dblclicked(QTreeWidgetItem*) ));

	ui.toolBar_1->setContextMenuPolicy( Qt::PreventContextMenu );

	lbpanel_1.setFrameStyle( QFrame::NoFrame  );
	lbpanel_2.setFrameStyle( QFrame::NoFrame );
	lbpanel_3.setFrameStyle( QFrame::NoFrame );
	lbpanel_4.setFrameStyle( QFrame::NoFrame );
	lbpanel_5.setFrameStyle( QFrame::NoFrame | QFrame::Panel );

	lbpanel_1.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);	// icono
	lbpanel_2.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);		// Num juegos
	lbpanel_3.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);	// icono emu
	lbpanel_4.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);	// icono datos
	lbpanel_5.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);		// datos

	lbpanel_1.setPixmap( QPixmap(stTheme+"img16/datos_3.png") );
	lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
	lbpanel_4.setPixmap( QPixmap(stTheme+"img16/datos_1.png") );
	lbpanel_1.setMinimumSize(20,20); //w,h
	lbpanel_3.setMinimumSize(20,20); //w,h
	lbpanel_4.setMinimumSize(20,20); //w,h

	ui.statusBar->addWidget( &lbpanel_1    ); // icono
	ui.statusBar->addWidget( &lbpanel_2    ); // Num juegos
	ui.statusBar->addWidget( &lbpanel_3    ); // icono emu
	ui.statusBar->addWidget( &lbpanel_4    ); // icono datos
	ui.statusBar->addWidget( &lbpanel_5, 1 ); // datos

	ui.twJuegos->header()->setStretchLastSection(true);
	ui.twJuegos->header()->setMovable(false);
	ui.twJuegos->header()->setResizeMode(QHeaderView::Custom);
	ui.twJuegos->header()->resizeSection(0, 31 );
	ui.twJuegos->header()->resizeSection(1, 60 );
	ui.twJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.twJuegos->installEventFilter(this);
	ui.txtInfo_Comentario->installEventFilter(this);

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

//	Crea el icono para la bandeja de iconos
	isTrayIcon = createTrayIcon();

// Centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

GrLida::~GrLida(){}

void GrLida::closeEvent( QCloseEvent *e )
{
	if( isTrayIcon && trayIcon->isVisible() )
	{
		e->ignore();
		this->hide();
	} else {
		sql->dbClose();

		QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
		settings.beginGroup("OpcVer");
			settings.setValue("Pnl_Capturas" , ui.actionVerCapturas->isChecked() );
			settings.setValue("Pnl_Datos"    , ui.actionVerDatos->isChecked()    );
			settings.setValue("Pnl_FilesUrl" , ui.actionVerFilesUrl->isChecked() );
			settings.setValue("Pnl_Ordenar"  , ui.mnu_ver_ordenar->isChecked()   );
			settings.setValue("Pnl_PictureFlow", ui.mnu_ver_pictureflow->isChecked());
		settings.endGroup();
		settings.beginGroup("Updates");
			settings.setValue("Version" , fGrl.stVersionGrl() );
		settings.endGroup();
		settings.beginGroup("SqlDatabase");
			settings.setValue("db_Orden_ColTabla", stdb_Orden_ColTabla );
			settings.setValue("db_Orden_By"      , stdb_Orden_By       );
			settings.setValue("db_Orden"         , stdb_Orden          );
		settings.endGroup();
		settings.beginGroup("MainState");
			settings.setValue("maximized", isMaximized() );
		settings.endGroup();

		e->accept();
	}
}

bool GrLida::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == ui.twJuegos || obj == ui.txtInfo_Comentario )
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

			int nItem, pos;
			nItem = 0;
			pos = ui.twJuegos->indexOfTopLevelItem( ui.twJuegos->currentItem() );

			switch( keyEvent->key() )
			{
				case Qt::Key_Enter:
				case Qt::Key_Space:
				case Qt::Key_Return:
					on_EjecutarJuego();
					return true;
				break;
				case Qt::Key_Left:
				case Qt::Key_Up:
					if( ui.twJuegos->topLevelItemCount()>0 && pos!=-1 )
					{
						nItem = pos;
						if( nItem == 0 )
						{
							nItem = ui.twJuegos->topLevelItemCount()-1 ;
							ui.twJuegos->setCurrentItem( ui.twJuegos->topLevelItem( nItem ), true);
						} else
							ui.twJuegos->setCurrentItem( ui.twJuegos->topLevelItem( nItem - 1), true);
					}
					return true;
				break;
				case Qt::Key_Right:
				case Qt::Key_Down:
					if( ui.twJuegos->topLevelItemCount()>0 && pos!=-1 )
					{
						nItem = pos;
						if( nItem == ui.twJuegos->topLevelItemCount()-1 )
						{
							nItem = 0 ;
							ui.twJuegos->setCurrentItem( ui.twJuegos->topLevelItem( 0 ), true);
						} else
							ui.twJuegos->setCurrentItem( ui.twJuegos->topLevelItem( nItem + 1), true);
					}
					return true;
				break;
				default:
					return false;
				break;
			}
		} else
			return false;
	} else
		return QMainWindow::eventFilter(obj, event); // pass the event on to the parent class
}

void GrLida::setVisible(bool visible)
{
	minimizeAction->setEnabled(visible);
	maximizeAction->setEnabled(!isMaximized());
	restoreAction->setEnabled(isMaximized() || !visible);
	QWidget::setVisible(visible);
}

bool GrLida::createTrayIcon()
{
	if( QSystemTrayIcon::isSystemTrayAvailable() )
	{
		minimizeAction = new QAction(tr("Mi&nimizar"), this);
		connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

		maximizeAction = new QAction(tr("Ma&ximizar"), this);
		connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

		restoreAction = new QAction(tr("&Restaurar"), this);
		connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

		quitAction = new QAction(tr("&Cerrar"), this);
		connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

		trayIconMenu = new QMenu(this);
		trayIconMenu->addAction(minimizeAction);
		trayIconMenu->addAction(maximizeAction);
		trayIconMenu->addAction(restoreAction);
		trayIconMenu->addSeparator();
		trayIconMenu->addAction(quitAction);

		trayIcon = new QSystemTrayIcon(this);
		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(":/img16/grlida.png"));
		trayIcon->setToolTip("GR-lida");

		trayIcon->hide();
		return true;
	} else
		return false;
}

void GrLida::on_setFavorito()
{
	if(ui.mnu_edit_favorito->isChecked())
	{
		ui.twJuegos->currentItem()->setIcon( 1, QIcon(stTheme+"img16/"+stIconoFav) ); // icono favorito
		ui.twJuegos->currentItem()->setText( 3 , "true");
		sql->ItemActualizaDatosFavorito("true", ui.twJuegos->currentItem()->text(0));
	} else {
		ui.twJuegos->currentItem()->setIcon( 1 , QIcon()); // icono favorito
		ui.twJuegos->currentItem()->setText( 3 , "false");
		sql->ItemActualizaDatosFavorito("false", ui.twJuegos->currentItem()->text(0));
	}
}

void GrLida::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	lbpanel_1.setPixmap( QPixmap(stTheme+"img16/datos_3.png") );
	lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
	lbpanel_4.setPixmap( QPixmap(stTheme+"img16/datos_1.png") );

	ui.actionClose->setIcon( QIcon(stTheme+"img32/btnClose.png") );
	ui.actionNewDbx->setIcon( QIcon(stTheme+"img32/btnDosBox.png") );
	ui.actionNewSvm->setIcon( QIcon(stTheme+"img32/btnScummVM.png") );
	ui.actionNewVdms->setIcon( QIcon(stTheme+"img32/btnVdmSound.png") );
	ui.actionEjectar->setIcon( QIcon(stTheme+"img32/btnEjecutar.png") );
	ui.actionEjectarSetup->setIcon( QIcon(stTheme+"img32/btnSetup.png") );
	ui.actionNuevo->setIcon( QIcon(stTheme+"img32/btnNuevo.png") );
	ui.actionEditar->setIcon( QIcon(stTheme+"img32/btnEditar.png") );
	ui.actionEiminar->setIcon( QIcon(stTheme+"img32/btnEliminar.png") );
	ui.actionImportarJuego->setIcon( QIcon(stTheme+"img32/btnImportarJuego.png") );
	ui.actionVerFilesUrl->setIcon( QIcon(stTheme+"img32/btnArchivos.png") );
	ui.actionVerDatos->setIcon( QIcon(stTheme+"img32/btnDatos.png") );
	ui.actionVerCapturas->setIcon( QIcon(stTheme+"img32/btnCapturas.png") );
	ui.actionOpciones->setIcon( QIcon(stTheme+"img32/btnOpciones.png") );
	ui.actionAcercaD->setIcon( QIcon(stTheme+"img32/btnAcercaD.png") );

	ui.btn_Ordenar->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.mnu_file_informacion->setIcon( QIcon(stTheme+"img16/informacion.png") );
	ui.mnu_file_cerrar->setIcon( QIcon(stTheme+"img16/cerrar.png") );

	ui.mnu_edit_nuevo_dosbox->setIcon( QIcon(stTheme+"img16/dosbox.png") );
	ui.mnu_edit_nuevo_scummvm->setIcon( QIcon(stTheme+"img16/scummvm.png") );
	ui.mnu_edit_nuevo_vdmsound->setIcon( QIcon(stTheme+"img16/vdmsound.png") );
	ui.mnu_edit_nuevo->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.mnu_edit_editar->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.mnu_edit_eliminar->setIcon( QIcon(stTheme+"img16/eliminar.png") );
	ui.mnu_edit_favorito->setIcon( QIcon(stTheme+"img16/fav_0.png") );

	ui.mnu_ejecutar_dosbox->setIcon( QIcon(stTheme+"img16/dosbox.png") );
	ui.mnu_ejecutar_scummvm->setIcon( QIcon(stTheme+"img16/scummvm.png") );
	ui.mnu_ejecutar_juego->setIcon( QIcon(stTheme+"img16/ejecutar_app.png") );
	ui.mnu_ejecutar_setup->setIcon( QIcon(stTheme+"img16/ejecutar_app_setup.png") );

	ui.mnu_tool_importar->setIcon( QIcon(stTheme+"img16/importar.png") );
	ui.mnu_tool_exportar->setIcon( QIcon(stTheme+"img16/exportar.png") );
	ui.mnu_tool_opciones->setIcon( QIcon(stTheme+"img16/opciones.png") );

	ui.mnu_ver_archivos_url->setIcon( QIcon(stTheme+"img16/edit_enlace.png") );
	ui.mnu_ver_datos->setIcon( QIcon(stTheme+"img16/datos_1.png") );
	ui.mnu_ver_capturas->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_ordenar->setIcon( QIcon(stTheme+"img16/orden_asc.png") );
	ui.mnu_ver_toolbar->setIcon( QIcon(stTheme+"img16/barra_herramintas.png") );
	ui.mnu_ver_acercad->setIcon( QIcon(stTheme+"img16/acercad.png") );

	ui.btnVer_CoverFront->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnVer_CoverBack->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.toolButton_11->setIcon( QIcon(stTheme+"img16/grlida.png") );

	ui.tabFileUrl->setTabIcon(0, QIcon(stTheme+"img16/edit_enlace.png") );
	ui.tabFileUrl->setTabIcon(1, QIcon(stTheme+"img16/archivos.png") );

	ui.PicFlowWidget->setReflectionEffect(PictureFlow::BlurredReflection);
//	ui.PicFlowWidget->setReflectionEffect(PictureFlow::PlainReflection);
}

void GrLida::on_ImportarJuego()
{
	frmImportarJuego * NewImportar = new frmImportarJuego( 0, Qt::Window );
	if( NewImportar->exec() == QDialog::Accepted )
	{
		QString lastID, lastID_Dbx;

		lastID = sql->ItemInsertaDatos( NewImportar->DatosJuego );

		sql->ItemInsertaFiles( NewImportar->ui.twDatosFiles, lastID );
		sql->ItemInsertaURL( NewImportar->ui.twDatosURL, lastID );

		if( NewImportar->DatosJuego["tipo_emu"]=="dosbox")
		{
			lastID_Dbx = sql->ItemInsertaDbx( NewImportar->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx( NewImportar->ui.twMontajes, lastID_Dbx );
			fGrl.CrearArchivoConfigDbx( NewImportar->DatosJuego, NewImportar->DatosDosBox, NewImportar->ui.twMontajes, stConfgDbxDir + NewImportar->DatosDosBox["path_conf"] );
		}
		if( NewImportar->DatosJuego["tipo_emu"]=="scummvm")
		{
			sql->ItemInsertaSvm( NewImportar->DatosScummvm, lastID);
		}
		if( NewImportar->DatosJuego["tipo_emu"]=="vdmsound")
		{
			sql->ItemInsertaVdms( NewImportar->DatosVDMSound, lastID);
		}
		NuevoItemTreeWidget( NewImportar->DatosJuego, NewImportar->DatosJuego["tipo_emu"], lastID);
	}
}

void GrLida::on_ExportarJuego()
{
	frmExportarJuego * NewExportar = new frmExportarJuego( 0, Qt::Window );
	NewExportar->exec();
	delete NewExportar;
}

void GrLida::CargarConfigInicial()
{
	stHomeDir      = fGrl.GRlidaHomePath();				// directorio de trabajo del GR-lida
	stIconDir      = stHomeDir + "iconos/";				// directorio de iconos para el GR-lida
	stDatosDir     = stHomeDir + "datos/";				// directorio para los distintos datos del GR-lida
	stConfgDbxDir  = stHomeDir + "confdbx/";			// directorio de configuracion para el DOSBox
#ifdef Q_OS_WIN32
	stConfgVdmSDir = stHomeDir + "confvdms/";			// directorio de configuracion para el VDMSound
#endif
	stTheme = fGrl.ThemeGrl();
	setTheme();

	QSettings settings(stHomeDir+"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("SqlDatabase");
		stdb_type           = settings.value("db_type"          , "QSQLITE" ).toString();
		stdb_host           = settings.value("db_host"          , stHomeDir+"db_grl.grl" ).toString();
		stdb_name           = settings.value("db_name"          , ""       ).toString();
		stdb_username       = settings.value("db_username"      , ""       ).toString();
		stdb_password       = settings.value("db_password"      , ""       ).toString();
		stdb_port           = settings.value("db_port"          , "3306"   ).toString();
		stdb_Orden_ColTabla = settings.value("db_Orden_ColTabla", "titulo" ).toString();
		stdb_Orden_By       = settings.value("db_Orden_By"      , "titulo" ).toString();
		stdb_Orden          = settings.value("db_Orden"         , "asc"    ).toString();
	settings.endGroup();

	if( !QFile::exists(stdb_host) )
		stdb_host = stHomeDir + "db_grl.grl";

	settings.beginGroup("OpcGeneral");
		stBinExeDbx = QDir::toNativeSeparators( settings.value("DirDOSBox" , "").toString() );
		stBinExeSvm = QDir::toNativeSeparators( settings.value("DirScummVM", "").toString() );
		ui.actionNewDbx->setEnabled( settings.value("DOSBoxDisp" , "false").toBool() );
		ui.actionNewSvm->setEnabled( settings.value("ScummVMDisp", "false").toBool() );
		stIdiomaSelect = settings.value("IdiomaSelect", "es_ES").toString();
#ifdef Q_OS_WIN32
		ui.actionNewVdms->setEnabled( settings.value("VDMSoundDisp", "false" ).toBool() );
#else
		ui.actionNewVdms->setEnabled( false );
#endif
		stIconoFav = settings.value("IconoFav", "fav_0.png").toString();
	settings.endGroup();

	settings.beginGroup("OpcVer");
		ui.actionVerDatos->setChecked(    settings.value("Pnl_Datos"   , "true"  ).toBool() );
		ui.actionVerCapturas->setChecked( settings.value("Pnl_Capturas", "false" ).toBool() );
		ui.actionVerFilesUrl->setChecked( settings.value("Pnl_FilesUrl", "false" ).toBool() );
		ui.mnu_ver_ordenar->setChecked(   settings.value("Pnl_Ordenar" , "false" ).toBool() );
		ui.dockw_Datos->setVisible(       settings.value("Pnl_Datos"   , "true"  ).toBool() );
		ui.dockw_Capturas->setVisible(    settings.value("Pnl_Capturas", "false" ).toBool() );
		ui.dockw_FilesUrl->setVisible(    settings.value("Pnl_FilesUrl", "false" ).toBool() );
		ui.toolBar_ordenar->setVisible(   settings.value("Pnl_Ordenar" , "false" ).toBool() );
		ui.mnu_ver_pictureflow->setChecked( settings.value("Pnl_PictureFlow", "true"  ).toBool() );
		ui.PicFlowWidget->setVisible( settings.value("Pnl_PictureFlow"      , "true"  ).toBool() );
	settings.endGroup();

	settings.beginGroup("MainState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
	settings.endGroup();

#ifdef Q_OS_WIN32
	if( (stBinExeDbx=="")&&(stBinExeSvm=="")&&(ui.actionNewVdms->isEnabled()==false) )
		ui.actionEjectar->setEnabled( false );
#else
	if( (stBinExeDbx=="")&&(stBinExeSvm=="") )
		ui.actionEjectar->setEnabled( false );
#endif

	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles(stDatosDir + "smiles.txt");

	QStringList lista_columnas_tabla;
	lista_columnas_tabla.clear();
	lista_columnas_tabla << "idgrl" << "titulo" << "subtitulo" << "genero" << "compania" << "desarrollador" << "tema" << "idioma" << "formato" << "anno" << "numdisc" << "sistemaop" << "graficos" << "sonido" << "jugabilidad" << "original" << "tipo_emu" << "favorito";

	ui.cbxColTabla->addItems(lista_columnas_tabla);
	ui.cbxOrdenBy->addItems(lista_columnas_tabla);

	ui.cbxOrden->addItem(QIcon(stTheme+"img16/orden_asc.png"),"ASC");
	ui.cbxOrden->addItem(QIcon(stTheme+"img16/orden_desc.png"),"DESC");

	ui.cbxColTabla->setCurrentIndex( ui.cbxColTabla->findText(stdb_Orden_ColTabla, Qt::MatchContains) );
	ui.cbxOrdenBy->setCurrentIndex( ui.cbxOrdenBy->findText(stdb_Orden_By, Qt::MatchContains) );
	ui.cbxOrden->setCurrentIndex( ui.cbxOrden->findText(stdb_Orden, Qt::MatchContains) );

	sql = new dbSql( stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port ); // Carga la base de datos

	if( !sql->dbisOpen() )
		QMessageBox::critical(0, tr("Error abrir la Base de Datos"), tr("No se ha podido establecer una conexión con la Base de Datos.\nEsta aplicación necesita soporte de SQLite. Mira la documentación de Qt SQL driver para más información.\n\nClick Cancelar para salir."), QMessageBox::Cancel);
	else
		sql->CrearTablas();	// Crea las tablas si no lo estan
}

void GrLida::on_AddNewDbx()
{
	frmDbxAdd * AddDbxNew = new frmDbxAdd();
	if( AddDbxNew->exec() == QDialog::Accepted )
	{
		QString lastID, lastID_Dbx;
		lastID = sql->ItemInsertaDatos( AddDbxNew->DatosJuego );
		lastID_Dbx = sql->ItemInsertaDbx( AddDbxNew->DatosDosBox , lastID);
		sql->ItemInsertaMontajesDbx( AddDbxNew->ui.twMontajes, lastID_Dbx );
		fGrl.CrearArchivoConfigDbx( AddDbxNew->DatosJuego, AddDbxNew->DatosDosBox, AddDbxNew->ui.twMontajes, stConfgDbxDir + AddDbxNew->DatosDosBox["path_conf"] );
		NuevoItemTreeWidget( AddDbxNew->DatosJuego, "dosbox", lastID);
	}
}

void GrLida::on_AddNewSvm()
{
	frmSvmAdd *AddSvmNew = new frmSvmAdd();
	if( AddSvmNew->exec() == QDialog::Accepted )
	{
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddSvmNew->DatosJuego );
		sql->ItemInsertaSvm( AddSvmNew->DatosScummvm, lastID);
		NuevoItemTreeWidget( AddSvmNew->DatosJuego, "scummvm", lastID);
	}
}

void GrLida::NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem)
{
	QString stIcono;
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

	if( imgEmu == "" )
		stIcono = stTheme+"img24/emu_sin_imagen.png";
	else if( imgEmu == "datos" )
		stIcono = stTheme+"img24/emu_datos.png";
	else if( imgEmu == "dosbox" )
		stIcono = stTheme+"img24/emu_dbx.png";
	else if( imgEmu == "scummvm" )
		stIcono = stTheme+"img24/emu_svm.png";
	else if( imgEmu =="vdmsound" )
		stIcono = stTheme+"img24/emu_vdms.png";
	else {
		bool existeIcono;
		existeIcono = QFile::exists(stIconDir + imgEmu);
		if( existeIcono )
			stIcono = stIconDir + imgEmu;
		else stIcono = stTheme+"img24/emu_sin_imagen.png";
	}

	item->setText( 0 , IDitem			 ); // idgrl
	item->setIcon( 0 , QIcon(stIcono)	 ); // icono
	item->setText( 1 , datos["titulo"]	 ); // titulo
	item->setText( 2 , datos["tipo_emu"] ); // tipo_emu
	item->setText( 3 , datos["favorito"] ); // favorito
	item->setText( 4 , fGrl.IntToStr( id_ImgPicFlow ) ); //

	id_ImgPicFlow++;

	if(datos["favorito"]=="true")
		item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito
	else
		item->setIcon( 1 , QIcon()); // icono favorito

	QImage img;
	if( img.load( datos["thumbs"] ) )
    	ui.PicFlowWidget->addSlide( img );
    else
    	ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );

	ui.twJuegos->setFocus();
	ui.twJuegos->clearSelection();
	ui.twJuegos->setCurrentItem( item );
	emit on_twJuegos_clicked( item );

	lbpanel_2.setText(" " + tr("Nº Juegos") + ": " + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
}

void GrLida::on_AddNewVdms()
{
	frmVdmsAdd *AddVdmsNew = new frmVdmsAdd();
	if( AddVdmsNew->exec() == QDialog::Accepted )
	{
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddVdmsNew->DatosJuego );
		sql->ItemInsertaVdms( AddVdmsNew->DatosVDMSound, lastID);
		fGrl.CrearArchivoConfigVdmS( AddVdmsNew->DatosVDMSound, stConfgVdmSDir + AddVdmsNew->DatosVDMSound["path_conf"] );
		NuevoItemTreeWidget( AddVdmsNew->DatosJuego, "vdmsound", lastID);
	}
}

void GrLida::on_EjecutarJuego()
{
	if( stTipoEmu!="datos" )
	{
		if( stConfgJuego!="" )
		{
		// Ejecuta el juego con el emulador DOSBox
			if( stTipoEmu=="dosbox" )
				Ejecutar( stBinExeDbx, stConfgJuego);
		// Ejecuta el juego con el emulador ScummVM
			if( stTipoEmu=="scummvm")
			{
			// Creamos el INI de configuracion del ScummVM
				fGrl.CreaIniScummVM(stHomeDir+"scummvm.ini", conf_scummvm);
				Ejecutar( stBinExeSvm, stConfgJuego);
			}
		// Ejecuta el juego con el emulador vdmsound
			if( stTipoEmu=="vdmsound" )
			{
				#ifdef Q_OS_WIN32
					if( (long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(stConfgJuego.utf16()), 0, 0, SW_SHOWNORMAL) <= 32)
						QMessageBox::information(this, stTituloGrl(), tr("No se ha podido ejecutar el juego"));
				#else
					QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)"));
				#endif
			}
		} else if( stConfgJuego=="")
			QMessageBox::information( this, stTituloGrl(),tr("No dispones de ninguna configuración") );
		else
			QMessageBox::information( this, stTituloGrl(),tr("Porfavor, seleccione un juego de la lista antes.") );
	}
}

void GrLida::on_EjecutarSetup()
{
	QHash<QString, QString> ConfgJuegoSetup;
	QString stConfgSetup;
	QSqlQuery query;
	QSqlRecord rec;

	if( stTipoEmu!="datos" )
	{
//		if( stConfgJuego!=""){
		// Ejecuta el juego con el emulador DOSBox
			if( stTipoEmu=="dosbox")
			{
				query.clear();
				query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stItemIndex+" LIMIT 0,1");
				query.first();
				rec = query.record();

				ConfgJuegoSetup.clear();
				//ConfgJuegoSetup["path_exe"]   = query.value( rec.indexOf("path_exe") ).toString()	;	// path_exe
				ConfgJuegoSetup["path_setup"] = query.value( rec.indexOf("path_setup") ).toString()	;	// path_setup

				stConfgSetup.clear();
				stConfgSetup = ConfgJuegoSetup["path_setup"];

				Ejecutar( stBinExeDbx, stConfgSetup);
			}
		// Ejecuta el juego con el emulador ScummVM
//			if( stTipoEmu=="scummvm")
//			{
//
//			}
		// Ejecuta el juego con el emulador vdmsound
//			if( stTipoEmu=="vdmsound")
//			{
//				#ifdef Q_OS_WIN32
//
//				#else
//					QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)"));
//				#endif
//			}
//		}else if( stConfgJuego==""){
//			QMessageBox::information( this, stTituloGrl(),tr("No dispones de ninguna configuración"));
//		}else{
//			QMessageBox::information( this, stTituloGrl(),tr("Porfavor, seleccione un juego de la lista antes."));
//		}
	}
}

void GrLida::on_EjecutarDosbox()
{
	Ejecutar( stBinExeDbx, "" );
}

void GrLida::on_EjecutarScummVM()
{
	Ejecutar( stBinExeSvm, "" );
}

void GrLida::Ejecutar( const QString& bin, const QString &parametros)
{
	dBoxSvm = new QProcess( this );

	QFile appBin( bin );
	if( appBin.exists() )
	{
		if( isTrayIcon )
		{
			if( !trayIcon->isVisible() )
				trayIcon->show();
		}

		if( parametros !="")
		{
			stl_param << parametros.split("|", QString::SkipEmptyParts);
			dBoxSvm->start( bin , stl_param );
			stl_param.clear();
		} else
			dBoxSvm->start( bin );

		ui.actionEjectar->setEnabled(false);

		connect ( dBoxSvm, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( fin_Proceso(int, QProcess::ExitStatus) ) );
		connect ( dBoxSvm, SIGNAL( error(QProcess::ProcessError) ), this, SLOT( fin_ProcesoError(QProcess::ProcessError) ) );

		if( isTrayIcon )
			this->hide();
	} else
		QMessageBox::information( this, stTituloGrl(),tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void GrLida::fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus)
{
	if( this->isHidden() )
		this->show();

	if( isTrayIcon )
	{
		if( trayIcon->isVisible() )
			trayIcon->hide();
	}

	switch ( exitStatus )
	{
		case QProcess::NormalExit:
		//	QMessageBox::information(0, stTituloGrl(), "El proceso ha salido normalmente" );
			qDebug() << exitCode;
		break;
		case QProcess::CrashExit:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error al salir del proceso", QMessageBox::Cancel);
			qDebug() << exitCode;
		break;
	}

	ui.actionEjectar->setEnabled(true);
}

void GrLida::fin_ProcesoError( QProcess::ProcessError error )
{
	if( this->isHidden() )
		this->show();

	if( isTrayIcon )
	{
		if( trayIcon->isVisible() )
			trayIcon->hide();
	}

	switch( error )
	{
		case QProcess::FailedToStart:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error al iniciar el proceeso de inicio", QMessageBox::Cancel);
		break;
		case QProcess::Crashed:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error en el proceso, tiempo después de empezar con éxito", QMessageBox::Cancel);
		break;
		case QProcess::Timedout:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error, waitFor...() última función el tiempo de espera", QMessageBox::Cancel);
		break;
		case QProcess::WriteError:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error al intentar escribir en el proceso de inicio", QMessageBox::Cancel);
		break;
		case QProcess::ReadError:
			QMessageBox::critical(0, stTituloGrl(), "Se ha producido un error al intentar leer en el proceso", QMessageBox::Cancel);
		break;
		case QProcess::UnknownError:
			QMessageBox::critical(0, stTituloGrl(), "Ha ocurrido un error desconocido", QMessageBox::Cancel);
		break;
	}

	ui.actionEjectar->setEnabled(true);
}

void GrLida::Confg_Svm_Dbx(QString IDitem)
{
	QSqlQuery query;
	QSqlRecord rec;
// Limpiamos primero la informacion que pueda tener stConfgJuego
	stConfgJuego.clear();
// Si el emulador es el ScummVM:
	if( stTipoEmu=="scummvm" )
	{
		conf_scummvm.clear();
		conf_scummvm = sql->showConfg_ScummVM( IDitem );
		conf_scummvm["description"] = ui.twJuegos->currentItem()->text(1);

		stConfgJuego = "-c" + stHomeDir + "scummvm.ini|" + "-d" + conf_scummvm["debuglevel"] + "|" + conf_scummvm["game"];

		stCapturasSvm =  conf_scummvm["path_capturas"]; // path_capturas
		stCapturasDbx.clear();

		if( stCapturasSvm !="" )
			CargarThumbsTreeWidget( stCapturasSvm );
		else
			ui.twCapturas->clear();

		ui.actionEjectar->setEnabled(true);
		ui.mnu_ejecutar_juego->setEnabled(true);

		if( QFile( conf_scummvm["path_setup"] ).exists() )
		{
			ui.actionEjectarSetup->setEnabled(true);
			ui.mnu_ejecutar_setup->setEnabled(true);
		} else {
			ui.actionEjectarSetup->setEnabled(false);
			ui.mnu_ejecutar_setup->setEnabled(false);
		}

// Si el emulador es el DOSBox:
	} else if( stTipoEmu=="dosbox" ) {

		QHash<QString, QString> conf_dosbox;

		conf_dosbox.clear();
		conf_dosbox = sql->showConfg_DOSBox( IDitem );

		QString consolaDbx;
		#ifdef Q_OS_WIN32
			if( conf_dosbox["opt_consola_dbox"]=="true")
				consolaDbx = "|-noconsole";
			else
				consolaDbx.clear();
		#else
			consolaDbx.clear();
		#endif

		stConfgJuego = "-conf|" + stConfgDbxDir + conf_dosbox["path_conf"] + consolaDbx;

		stCapturasDbx = conf_dosbox["dosbox_captures"];  //3 dosbox_capture
		stCapturasSvm.clear();

		if( stCapturasDbx !="" )
			CargarThumbsTreeWidget( stCapturasDbx );
		else
			ui.twCapturas->clear();

		if( !QFile( stConfgDbxDir + conf_dosbox["path_conf"] ).exists() )
		{
			ui.actionEjectar->setEnabled(false);
			ui.mnu_ejecutar_juego->setEnabled(false);
			QMessageBox::information( this, stTituloGrl(), tr("Configuración del juego no encontrada.")+ "\n\n'" + stConfgDbxDir + conf_dosbox["path_conf"] + "'" );
		} else {
			ui.actionEjectar->setEnabled(true);
			ui.mnu_ejecutar_juego->setEnabled(true);
		}

		if( QFile( conf_dosbox["path_setup"] ).exists() )
		{
			ui.actionEjectarSetup->setEnabled(true);
			ui.mnu_ejecutar_setup->setEnabled(true);
		} else {
			ui.actionEjectarSetup->setEnabled(false);
			ui.mnu_ejecutar_setup->setEnabled(false);
		}

// Si el emulador es el vdmsound:
	} else if( stTipoEmu=="vdmsound") {

		query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDitem+" LIMIT 0,1");
		query.first();
		rec = query.record();

		stConfgJuego = stConfgVdmSDir + query.value( rec.indexOf("path_conf") ).toString();

		ui.actionEjectar->setEnabled(true);
		ui.mnu_ejecutar_juego->setEnabled(true);
		ui.actionEjectarSetup->setEnabled(false);
	} else {
		stCapturasSvm.clear();
		stCapturasDbx.clear();
		stConfgJuego.clear();
		ui.twCapturas->clear();

		ui.actionEjectar->setEnabled(false);
		ui.mnu_ejecutar_juego->setEnabled(false);
		ui.actionEjectarSetup->setEnabled(false);
		ui.mnu_ejecutar_setup->setEnabled(false);
	}
}

void GrLida::on_NuevoJuego()
{
	QString lastID, lastID_Dbx;
	frmAddEditJuego * AddJuego = new frmAddEditJuego( false, "datos", "", 0, Qt::Window);

	if( AddJuego->exec() == QDialog::Accepted )
	{
		lastID = sql->ItemInsertaDatos( AddJuego->DatosJuego );

		sql->ItemInsertaFiles( AddJuego->ui.twDatosFiles, lastID );
		sql->ItemInsertaURL( AddJuego->ui.twDatosURL, lastID );

		if( AddJuego->DatosJuego["tipo_emu"]=="dosbox")
		{
			lastID_Dbx = sql->ItemInsertaDbx( AddJuego->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx( AddJuego->ui.twMontajes, lastID_Dbx );
			fGrl.CrearArchivoConfigDbx( AddJuego->DatosDosBox, AddJuego->DatosDosBox, AddJuego->ui.twMontajes, stConfgDbxDir + AddJuego->DatosDosBox["path_conf"] );
		}
		if( AddJuego->DatosJuego["tipo_emu"]=="scummvm")
		{
			sql->ItemInsertaSvm( AddJuego->DatosScummvm, lastID);
		}
		if( AddJuego->DatosJuego["tipo_emu"]=="vdmsound")
		{
			sql->ItemInsertaVdms( AddJuego->DatosVDMSound, lastID);
			fGrl.CrearArchivoConfigVdmS( AddJuego->DatosVDMSound, stConfgVdmSDir + AddJuego->DatosVDMSound["path_conf"] );
		}
		NuevoItemTreeWidget( AddJuego->DatosJuego, AddJuego->DatosJuego["tipo_emu"], lastID);
	}
}

void GrLida::on_EditarJuego()
{
	QString stIcono, stSelectIcon;
	if( (ui.twJuegos->topLevelItemCount()>0) && (stItemIndex!=""))
	{
		frmAddEditJuego * EditJuego = new frmAddEditJuego( true, stTipoEmu, stItemIndex, 0, Qt::Window);
		if( EditJuego->exec() == QDialog::Accepted )
		{
			sql->ItemActualizaDatos(EditJuego->DatosJuego, stItemIndex);
			ui.twJuegos->currentItem()->setText(1, EditJuego->DatosJuego["titulo"] );
			ui.twJuegos->currentItem()->setText(3, EditJuego->DatosJuego["favorito"] );

			QImage img;
			if( img.load( EditJuego->DatosJuego["thumbs"] ) )
				ui.PicFlowWidget->setSlide( fGrl.StrToInt( ui.twJuegos->currentItem()->text(4) ) , img );
			else
				ui.PicFlowWidget->setSlide( fGrl.StrToInt( ui.twJuegos->currentItem()->text(4) ) , QPixmap(stTheme+"images/juego_sin_imagen.png") );

			if( stTipoEmu=="dosbox")
			{
				sql->ItemActualizaDbx( EditJuego->DatosDosBox, EditJuego->stItemIDDbx );
				sql->ItemActualizaMontajeDbx(EditJuego->ui.twMontajes);
				fGrl.CrearArchivoConfigDbx( EditJuego->DatosJuego, EditJuego->DatosDosBox, EditJuego->ui.twMontajes, stConfgDbxDir + EditJuego->DatosDosBox["path_conf"] );
			}
			if( stTipoEmu=="scummvm")
			{
				sql->ItemActualizaSvm( EditJuego->DatosScummvm, EditJuego->stItemIDSvm );
			}
			if( stTipoEmu=="vdmsound")
			{
				sql->ItemActualizaVdms( EditJuego->DatosVDMSound, EditJuego->stItemIDVdms );
				fGrl.CrearArchivoConfigVdmS( EditJuego->DatosVDMSound, stConfgVdmSDir + EditJuego->DatosVDMSound["path_conf"] );
			}
			stIcono = EditJuego->DatosJuego["icono"];
			if( stIcono=="")
				stSelectIcon = stTheme+"img24/emu_sin_imagen.png";
			else if( stIcono == "datos" )
				stSelectIcon = stTheme+"img24/emu_datos.png";
			else if( stIcono == "dosbox" )
				stSelectIcon = stTheme+"img24/emu_dbx.png";
			else if( stIcono == "scummvm" )
				stSelectIcon = stTheme+"img24/emu_svm.png";
			else if( stIcono == "vdmsound" )
				stSelectIcon = stTheme+"img24/emu_vdms.png";
			else {
				bool existeIcono;
				existeIcono = QFile::exists(stIconDir + stIcono);
				if( existeIcono )
					stSelectIcon = stIconDir + stIcono;
				else stSelectIcon = stTheme+"img24/emu_sin_imagen.png";
			}
			ui.twJuegos->currentItem()->setIcon( 0, QIcon( stSelectIcon ) );

			if(EditJuego->DatosJuego["favorito"]=="true")
				ui.twJuegos->currentItem()->setIcon( 1, QIcon(stTheme+"img16/"+stIconoFav) ); // icono favorito
			else
				ui.twJuegos->currentItem()->setIcon( 1 , QIcon()); // icono favorito

			on_twJuegos_clicked( ui.twJuegos->currentItem() );
		}
	}
}

void GrLida::on_EliminarJuego()
{
	if( ui.twJuegos->topLevelItemCount()>0 )
	{
		if( stItemIndex.isEmpty() )
			QMessageBox::information( this, stTituloGrl(), tr("Porfavor selecciona un Juego de la lista para eliminarlo") );
		else {
			int EliminarSiNO;
			EliminarSiNO = QMessageBox::information( this, tr("¿Eliminar Juego...?"), tr("¿Deseas realmente eliminar este juego de la Base de Datos?\nSi es de DOSBox o VDMSound tambien se borrar el archivo de configuración"), tr("Si"), tr("Cancelar"), 0, 1 );
			if( EliminarSiNO == 0 )
			{
				sql->ItemEliminar(stItemIndex);

				ui.PicFlowWidget->setSlide( fGrl.StrToInt( ui.twJuegos->currentItem()->text(4) ), QPixmap(stTheme+"images/juego_eliminado.png") );

				fGrl.DeleteItemTree( ui.twJuegos->currentItem() );
				QMessageBox::information( this, stTituloGrl(), tr("Juego Eliminado correctamente"));
				if( ui.twJuegos->topLevelItemCount()<=0 )
					MostrarDatosDelJuego("");

				lbpanel_2.setText(" " + tr("Nº Juegos") + ": " + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
			}
		}
	} else
		MostrarDatosDelJuego("");
}

void GrLida::on_Opciones()
{
	frmOpciones * Opciones = new frmOpciones();
	if( Opciones->exec() == QDialog::Accepted )
	{
		if( Opciones->DatosConfiguracion["DOSBoxDisp"] == "true")
			ui.actionNewDbx->setEnabled( true ); else ui.actionNewDbx->setEnabled( false ); //0 DOSBox
		if( Opciones->DatosConfiguracion["ScummVMDisp"] == "true")
			ui.actionNewSvm->setEnabled( true ); else ui.actionNewSvm->setEnabled( false ); //1 ScummVM
		if( Opciones->DatosConfiguracion["VDMSoundDisp"] == "true")
			ui.actionNewVdms->setEnabled( true ); else ui.actionNewVdms->setEnabled( false );//2 vdmsound

		stBinExeDbx = Opciones->DatosConfiguracion["DirDOSBox"];	//4 DirDbx
		stBinExeSvm = Opciones->DatosConfiguracion["DirScummVM"];	//5 DirSvm

		if(Opciones->ui.cmbIconFav->currentText()!="")
			stIconoFav = Opciones->ui.cmbIconFav->currentText(); else stIconoFav = "fav_0.png";

		stTheme = Opciones->DatosConfiguracion["NameDirTheme"];
		setTheme();

	#ifdef Q_OS_WIN32
		if( (Opciones->DatosConfiguracion["DOSBoxDisp"]=="false")&&(Opciones->DatosConfiguracion["ScummVMDisp"]=="false")&&(Opciones->DatosConfiguracion["VDMSoundDisp"]=="false") )
			ui.actionEjectar->setEnabled( false );
		else
			ui.actionEjectar->setEnabled( true );
	#else
		if( (Opciones->DatosConfiguracion["DOSBoxDisp"]=="false")&&(Opciones->DatosConfiguracion["ScummVMDisp"]=="false"))
			ui.actionEjectar->setEnabled( false );
		else
			ui.actionEjectar->setEnabled( true );
	#endif
	}
}

void GrLida::on_AcercaD()
{
	frmAcercaD * AcercaD = new frmAcercaD();
	AcercaD->ui.label_4->setText( "v" + fGrl.stVersionGrl() );
	AcercaD->ui.lb_Dbx->setText( "Dosbox v" + fGrl.stVersionDbx() );
	AcercaD->ui.lb_Svm->setText( "ScummVM v" + fGrl.stVersionSvm() );
	AcercaD->ui.lb_Vdms->setText("VDMSound v" + fGrl.stVersionVdms() );// v2.0.4
	AcercaD->exec();
}

void GrLida::on_Informacion()
{
	frmInfo * NewInfo = new frmInfo();
	NewInfo->exec();
}

void GrLida::MostrarDatosDelJuego(QString IDitem)
{
	if(IDitem!="")
	{
		QSqlQuery query;
		QSqlRecord rec;
		QHash<QString, QString> strDatosJuego;

		strDatosJuego = sql->show_Datos( IDitem );

		QString strTempHtml;

		ui.txtInfo_Subtitulo->setText( strDatosJuego["subtitulo"] );			// subtitulo
		ui.txtInfo_Genero->setText( strDatosJuego["genero"] );					// genero
		ui.txtInfo_Idioma->setText( strDatosJuego["idioma"] );					// idioma
		ui.txtInfo_Formato->setText( strDatosJuego["formato"] );				// formato
		ui.txtInfo_NumDisc->setText( strDatosJuego["numdisc"] );				// numdisc
		ui.txtInfo_Anno->setText( strDatosJuego["anno"] );						// anno
		ui.txtInfo_Tamano->setText( strDatosJuego["tamano"] );					// tamano
		ui.txtInfo_Compania->setText( strDatosJuego["compania"] );				// compania
		ui.txtInfo_Desarrollador->setText( strDatosJuego["desarrollador"] );	// desarrollador
		ui.txtInfo_Tema->setText( strDatosJuego["tema"] );						// tema
		ui.txtInfo_SistemaOp->setText( strDatosJuego["sistemaop"] );			// sistemaop

		strTempHtml.clear();
		strTempHtml = fGrl.ReemplazaTextoSmiles( strDatosJuego["comentario"] , listSmailes);
		ui.txtInfo_Comentario->setHtml( strTempHtml ); // comentario

		QFile file_thumbs;  // thumbs
		if( file_thumbs.exists( strDatosJuego["thumbs"] ) )
			ui.lbImgThumbs->setPixmap( QPixmap( strDatosJuego["thumbs"] ) );
		else
			ui.lbImgThumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );

		stCaratula_Delantera = strDatosJuego["cover_front"];
		stCaratula_Trasera   = strDatosJuego["cover_back"];

		if(stCaratula_Delantera!="")
			ui.btnVer_CoverFront->setEnabled(true);
		else
			ui.btnVer_CoverFront->setEnabled(false);

		if(stCaratula_Trasera!="")
			ui.btnVer_CoverBack->setEnabled(true);
		else
			ui.btnVer_CoverBack->setEnabled(false);

		lbpanel_5.setText(" " + strDatosJuego["titulo"] + " - " + tr("introducido el") + " " + strDatosJuego["fecha"] + "  " );

		if( strDatosJuego["tipo_emu"] == "datos")
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/datos_1.png") );
		else if( strDatosJuego["tipo_emu"] == "dosbox")
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/dosbox.png") );
		else if( strDatosJuego["tipo_emu"] == "scummvm")
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/scummvm.png") );
		else if( strDatosJuego["tipo_emu"] == "vdmsound")
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/vdmsound.png") );
		else
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );

		ui.twUrls->clear();
		query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+IDitem);
		if( query.first())
		{
			do {
				rec = query.record();
				QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twUrls );
				item_url->setText( 0 , query.value( rec.indexOf("url") ).toString()        ); // url
				item_url->setIcon( 0 , QIcon(stTheme+"img16/edit_enlace.png")                    ); // icono
				item_url->setText( 1 , query.value( rec.indexOf("descripcion") ).toString()); // descripcion
			//	item_url->setText( 2 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    ); // id
			//	item_url->setText( 3 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) ); // idgrl
			} while (query.next());
			ui.twUrls->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
		}

		ui.twFiles->clear();
		query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+IDitem);
		if( query.first())
		{
			do {
				rec = query.record();
				QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twFiles );
				item_files->setText( 0 , query.value( rec.indexOf("nombre") ).toString()     );	// nombre
				item_files->setIcon( 0 , QIcon(stTheme+"img16/importar.png")                       );	// icono
				item_files->setText( 1 , query.value( rec.indexOf("crc") ).toString()        );	// crc
				item_files->setText( 2 , query.value( rec.indexOf("descripcion") ).toString());	// descripcion
				item_files->setText( 3 , query.value( rec.indexOf("size") ).toString()       );	// size
				item_files->setText( 4 , query.value( rec.indexOf("path") ).toString()       );	// path
			//	item_files->setText( 5 , fGrl.IntToStr(query.value(rec.indexOf("id")).toInt())    );	// id
			//	item_files->setText( 6 , fGrl.IntToStr(query.value(rec.indexOf("idgrl")).toInt()) );	// idgrl
			} while (query.next());
			ui.twFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
		}
	} else {
		ui.txtInfo_Subtitulo->clear();		// subtitulo
		ui.txtInfo_Genero->clear();			// genero
		ui.txtInfo_Idioma->clear();			// idioma
		ui.txtInfo_Formato->clear();		// formato
		ui.txtInfo_NumDisc->clear();		// numdisc
		ui.txtInfo_Anno->clear();			// anno
		ui.txtInfo_Tamano->clear();			// tamano
		ui.txtInfo_Compania->clear();		// compania
		ui.txtInfo_Desarrollador->clear();	// desarrollador
		ui.txtInfo_Tema->clear();			// tema
		ui.txtInfo_SistemaOp->clear();		// sistemaop
		ui.txtInfo_Comentario->clear();		// comentario

		ui.lbImgThumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		lbpanel_5.setText(" ");

		ui.twUrls->clear();
		ui.twFiles->clear();
	}
}

void GrLida::on_btnVer_CoverFront()
{
	if(stCaratula_Delantera!="")
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCaratula_Delantera );
		imgViewer->show();
	}
}
void GrLida::on_btnVer_CoverBack()
{
	if(stCaratula_Trasera!="")
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCaratula_Trasera );
		imgViewer->show();
	}
}

void GrLida::CargarThumbsTreeWidget(const QString directorio)
{
	if( (directorio!="") && ui.dockw_Capturas->isVisible()==true )
	{
		ui.twCapturas->clear();
		QStringList filters;
		filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas
		QDir dir( directorio );
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Size | QDir::Reversed);
		dir.setNameFilters(filters);
		if( dir.exists() )
		{
			QFileInfoList list = dir.entryInfoList();
			for (int i = 0; i < list.size(); ++i)
			{
				QFileInfo fileInfo = list.at(i);
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twCapturas );
				item->setIcon( 0 , QIcon( directorio + "/" + fileInfo.fileName() )); //
				item->setText( 0 , "" ); //
				item->setText( 1 , directorio + "/" + fileInfo.fileName() ); //
			}
		}
	} else
		ui.twCapturas->clear();
}

void GrLida::on_twJuegos_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
		emit on_twJuegos_clicked( ui.twJuegos->currentItem() );
	else
		return;
}

void GrLida::showPopup(const QPoint & aPosition)
{
// Creando el menu
	ljMenuPopUp = new QMenu(ui.twJuegos);
	ljMenuPopUp->addAction(ui.mnu_ejecutar_juego);
	ljMenuPopUp->addAction(ui.mnu_ejecutar_setup);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui.mnu_edit_nuevo);
	ljMenuPopUp->addAction(ui.mnu_edit_editar);
	ljMenuPopUp->addAction(ui.mnu_edit_eliminar);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui.mnu_edit_favorito);

// Muestra popupmenu
	ljMenuPopUp->exec(ui.twJuegos->mapToGlobal(aPosition));
	//ljMenuPopUp->exec(QCursor::pos());
	delete ljMenuPopUp;
}

void GrLida::on_twJuegos_clicked( QTreeWidgetItem *item)
{
	if( item )
	{
		stItemIndex.clear();					// Limpiamos el idgrl
		stTipoEmu.clear();						// Limpiamos el tipo_emu
		stItemIndex  = item->text(0);			// idgrl del juego en la Base de Datos
		stTipoEmu    = item->text(2);			// tipo_emu

		if(item->text(3)!="")
			ui.mnu_edit_favorito->setEnabled(true);
		else
			ui.mnu_edit_favorito->setEnabled(false);

		if(item->text(3)=="true")
			ui.mnu_edit_favorito->setChecked(true);
		else
			ui.mnu_edit_favorito->setChecked(false);

		if( !item->text(4).isEmpty() )
			ui.PicFlowWidget->showSlide( fGrl.StrToInt( item->text(4) ) );

		Confg_Svm_Dbx( stItemIndex );			// carga toda la configuracion
		MostrarDatosDelJuego( stItemIndex );	// Muestra los distintos datos del juego
	} else {
		lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		lbpanel_5.setText(" ");
		ui.mnu_edit_favorito->setEnabled(false);
		ui.mnu_edit_favorito->setChecked(false);
		return;
	}
}

void GrLida::on_twJuegos_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		stItemIndex.clear();				// Limpiamos el idgrl
		stTipoEmu.clear();					// Limpiamos el tipo_emu
		stItemIndex  = item->text(0);		// index del juego en la Base de Datos
		stTipoEmu    = item->text(2);		// indica el tipo de emulador
		Confg_Svm_Dbx( stItemIndex );		// carga toda la configuracion
		//MostrarDatosDelJuego( stItemIndex );// Muestra los distintos datos del juego
		on_EjecutarJuego();					// ejecuta el juego
	}else
		return;
}

void GrLida::on_twCapturas_Dblclicked( QTreeWidgetItem *item)
{
	if( item )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( item->text(1) );
		imgViewer->show();
	}
}

void GrLida::on_twUrls_Dblclicked( QTreeWidgetItem *item)
{
	if( item )
	{
		my_url.clear();
		my_url = ui.twUrls->currentItem()->text(0);
		QDesktopServices::openUrl( my_url );	// Abre la URL con el navegador por defecto
	}
}

void GrLida::on_txtBuscar_textChanged(const QString &)
{
	if( ui.cbxColTabla->currentText()!="" )
		stdb_Orden_ColTabla = ui.cbxColTabla->currentText();
	else
		stdb_Orden_ColTabla = "titulo";

	if( ui.cbxOrdenBy->currentText()!="" )
		stdb_Orden_By = ui.cbxOrdenBy->currentText();
	else
		stdb_Orden_By = "titulo";

	if( ui.cbxOrden->currentText()!="" )
		stdb_Orden = ui.cbxOrden->currentText();
	else
		stdb_Orden = "ASC";

	CargarBaseDatos( ui.txtBuscar->text() );
}

void GrLida::on_Ordenar_Lista()
{
	if( ui.cbxColTabla->currentText()!="" )
		stdb_Orden_ColTabla = ui.cbxColTabla->currentText();
	else
		stdb_Orden_ColTabla = "titulo";

	if( ui.cbxOrdenBy->currentText()!="" )
		stdb_Orden_By = ui.cbxOrdenBy->currentText();
	else
		stdb_Orden_By = "titulo";

	if( ui.cbxOrden->currentText()!="" )
		stdb_Orden = ui.cbxOrden->currentText();
	else
		stdb_Orden = "ASC";

	CargarBaseDatos();
}

void GrLida::CargarBaseDatos(QString str)
{
	QString stIcono, stSqlWhere;
	QSqlQuery query;
	QSqlRecord rec;
	QStringList str_ListaDatos;
	int n, num_juegos;

	num_juegos = sql->getCount("dbgrl");

	ui.twJuegos->setColumnCount( ui.twJuegos->columnCount() );
	ui.twJuegos->clear();

	QImage img;
	ui.PicFlowWidget->clear();
	ui.PicFlowWidget->setSlideSize(QSize(145, 186));
	id_ImgPicFlow = 0;

	if(stdb_Orden_ColTabla == "titulo" || stdb_Orden_ColTabla == "subtitulo" || stdb_Orden_ColTabla == "idgrl")
	{
		if(str!="")
			stSqlWhere = " WHERE titulo LIKE '%"+str+"%'"; else stSqlWhere = "";

		query.exec("SELECT * FROM dbgrl "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+stdb_Orden);
		if( query.first())
		{
			do {
				rec = query.record();
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
				item->setText( 0 , fGrl.IntToStr(query.value( rec.indexOf("idgrl") ).toInt()) );	// idgrl
				item->setText( 1 , query.value( rec.indexOf("titulo") ).toString()            );	// titulo
				item->setText( 2 , query.value( rec.indexOf("tipo_emu") ).toString()          );	// tipo_emu
				item->setText( 3 , query.value( rec.indexOf("favorito") ).toString()          );	// favorito
				item->setText( 4 , fGrl.IntToStr( id_ImgPicFlow ) );

			    if( img.load( query.record().value("thumbs").toString() ) )
			    {
			    	ui.PicFlowWidget->addSlide( img );
			    } else {
			    	ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );
			    }

			    id_ImgPicFlow++;

				stIcono = query.value( rec.indexOf("icono") ).toString();	// icono
				if( stIcono=="")
					item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
				else if( stIcono == "datos" )
					item->setIcon( 0, QIcon(stTheme+"img24/emu_datos.png") );
				else if( stIcono == "dosbox" )
					item->setIcon( 0, QIcon(stTheme+"img24/emu_dbx.png") );
				else if( stIcono == "scummvm" )
					item->setIcon( 0, QIcon(stTheme+"img24/emu_svm.png") );
				else if( stIcono == "vdmsound" )
					item->setIcon( 0, QIcon(stTheme+"img24/emu_vdms.png") );
				else {
					bool existeIcono;
					existeIcono = QFile::exists(stIconDir + stIcono);
					if( existeIcono )
						item->setIcon( 0, QIcon(stIconDir + stIcono) );
					else
						item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
				}

				if(query.value(rec.indexOf("favorito")).toString()=="true")
					item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito

			} while (query.next());
			ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
			on_twJuegos_clicked( ui.twJuegos->currentItem() );
		} else {
			lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
			lbpanel_5.setText(" ");
		}
	} else {
		str_ListaDatos.clear();
		if(stdb_Orden_ColTabla == "genero")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "generos.txt"  , "|");
		else if(stdb_Orden_ColTabla == "compania")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "companias.txt", "|");
		else if(stdb_Orden_ColTabla == "desarrollador")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "companias.txt", "|");
		else if(stdb_Orden_ColTabla == "tema")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "tema.txt"     , "|");
		else if(stdb_Orden_ColTabla == "idioma")
			str_ListaDatos = fGrl.CargaDatosListas(":/datos/svm_idioma.txt"    , "|");
		else if(stdb_Orden_ColTabla == "formato")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "formatos.txt" , "|");
		else if(stdb_Orden_ColTabla == "anno")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "fechas.txt"   , "|");
		else if(stdb_Orden_ColTabla == "numdisc")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "numdisc.txt"  , "|");
		else if(stdb_Orden_ColTabla == "sistemaop")
			str_ListaDatos = fGrl.CargaDatosListas(stDatosDir + "sistemaop.txt", "|");
		else if(stdb_Orden_ColTabla == "graficos")
			str_ListaDatos << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
		else if(stdb_Orden_ColTabla == "sonido")
			str_ListaDatos << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
		else if(stdb_Orden_ColTabla == "jugabilidad")
			str_ListaDatos << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10";
		else if(stdb_Orden_ColTabla == "original")
			str_ListaDatos << "true" << "false";
		else if(stdb_Orden_ColTabla == "tipo_emu")
			str_ListaDatos << "datos" << "dosbox" << "scummvm" << "vdmsound";
		else if(stdb_Orden_ColTabla == "favorito")
			str_ListaDatos << "true" << "false";

		if(str!="")
			stSqlWhere = " AND titulo LIKE '%"+str+"%'"; else stSqlWhere = "";

		if( str_ListaDatos.size() >= 0 )
		{
			for(n = 0; n < str_ListaDatos.size(); ++n)// inicio de for lista datos
			{
				query.exec("SELECT * FROM dbgrl WHERE "+stdb_Orden_ColTabla+" LIKE '"+ str_ListaDatos.at(n) +"' "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+ stdb_Orden );
				if( query.first())
				{
					QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

					if( str_ListaDatos.at(n) == "" || str_ListaDatos.at(n) == "NULL" )
					{
						item->setText( 0 , "" );	// idgrl
						item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
						item->setTextColor(1,QColor(0,0,0));
						item->setFont( 1, QFont("Times", 10, QFont::Bold));
						item->setText( 1 , tr("Genero sin Clasificar") );	// titulo
						item->setText( 2 , "" );	// tipo_emu
					} else {
						item->setText( 0 , "" ); // idgrl
						item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
						item->setTextColor(1,QColor(0,0,0));
						item->setFont( 1, QFont("Times", 10, QFont::Bold));

						if(stdb_Orden_ColTabla == "graficos")
							item->setText( 1 , tr("Calificación Gráficos")+ ": " + str_ListaDatos.at(n) );
						else if(stdb_Orden_ColTabla == "sonido")
							item->setText( 1 , tr("Calificación Sonido")+ ": " + str_ListaDatos.at(n) );
						else if(stdb_Orden_ColTabla == "jugabilidad")
							item->setText( 1 , tr("Calificación Jugabilidad")+ ": " + str_ListaDatos.at(n) );
						else if(stdb_Orden_ColTabla == "original")
						{
							if( str_ListaDatos.at(n) == "true")
								item->setText( 1 , tr("Juegos Originales") );
							else
								item->setText( 1 , tr("No lo son") );
						}
						else if(stdb_Orden_ColTabla == "favorito")
						{
							if( str_ListaDatos.at(n) == "true")
								item->setText( 1 , tr("Juegos Favoritos") );
							else
								item->setText( 1 , tr("No lo son") );
						} else
							item->setText( 1 , str_ListaDatos.at(n) );

						item->setText( 2 , "" ); // tipo_emu
					}

					do {
						rec = query.record();
						QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
						item->setText( 0 , fGrl.IntToStr(query.value( rec.indexOf("idgrl") ).toInt()) );	// idgrl
						item->setText( 1 , query.value( rec.indexOf("titulo") ).toString()            );	// titulo
						item->setText( 2 , query.value( rec.indexOf("tipo_emu") ).toString()          );	// tipo_emu
						item->setText( 3 , query.value( rec.indexOf("favorito") ).toString()          );	// favorito
						item->setText( 4 , fGrl.IntToStr( id_ImgPicFlow ) );

					    if( img.load( query.record().value("thumbs").toString() ) )
					    {
					    	ui.PicFlowWidget->addSlide( img );
					    } else {
					    	ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );
					    }

					    id_ImgPicFlow++;

						stIcono = query.value( rec.indexOf("icono") ).toString();	// icono
						if( stIcono=="")
							item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
						else if( stIcono == "datos" )
							item->setIcon( 0, QIcon(stTheme+"img24/emu_datos.png") );
						else if( stIcono == "dosbox" )
							item->setIcon( 0, QIcon(stTheme+"img24/emu_dbx.png") );
						else if( stIcono == "scummvm" )
							item->setIcon( 0, QIcon(stTheme+"img24/emu_svm.png") );
						else if( stIcono == "vdmsound" )
							item->setIcon( 0, QIcon(stTheme+"img24/emu_vdms.png") );
						else {
							bool existeIcono;
							existeIcono = QFile::exists(stIconDir + stIcono);
							if( existeIcono )
								item->setIcon( 0, QIcon(stIconDir + stIcono) );
							else
								item->setIcon( 0, QIcon(stTheme+"img24/emu_sin_imagen.png") );
						}

						if(query.value( rec.indexOf("favorito") ).toString()=="true")
							item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito

					} while (query.next());
					ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
					on_twJuegos_clicked( ui.twJuegos->currentItem() );
				} else {
					lbpanel_3.setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
					lbpanel_5.setText(" ");
				}
			} // fin de for lista datos
		}
	}

	//ui.PicFlowWidget->resize( ui.PicFlowWidget->width(), ui.PicFlowWidget->height() );
	//ui.PicFlowWidget->setCenterIndex( ui.PicFlowWidget->slideCount()/2 );
	ui.PicFlowWidget->showSlide( 0 );
	ui.PicFlowWidget->update();

	if( num_juegos > 0)
		lbpanel_2.setText(" "+ tr("Nº Juegos") + ": " + fGrl.IntToStr(num_juegos)+ "  " );
	else
		lbpanel_2.setText(" "+ tr("Nº Juegos") + ": 0 ");
}

void GrLida::ComprobarArchivosDatos(QString Version_GRL)
{
	QFile archivodatos;
	QSqlQuery query;

	if( Version_GRL != fGrl.stVersionGrl() && Version_GRL < fGrl.stVersionGrl())
	{
		QDir dirFileDatos( stDatosDir );
		if( archivodatos.exists( stDatosDir + "xmldb.txt") )
			dirFileDatos.remove( stDatosDir + "xmldb.txt" );
	}

	if( !archivodatos.exists(stDatosDir + "generos.txt") )
		CrearArchivoDato("generos.txt");
	if( !archivodatos.exists(stDatosDir + "companias.txt") )
		CrearArchivoDato("companias.txt");
	if( !archivodatos.exists(stDatosDir + "tema.txt") )
		CrearArchivoDato("tema.txt");
	if( !archivodatos.exists(stDatosDir + "formatos.txt") )
		CrearArchivoDato("formatos.txt");
	if( !archivodatos.exists(stDatosDir + "fechas.txt") )
		CrearArchivoDato("fechas.txt");
	if( !archivodatos.exists(stDatosDir + "numdisc.txt") )
		CrearArchivoDato("numdisc.txt");
	if( !archivodatos.exists(stDatosDir + "sistemaop.txt") )
		CrearArchivoDato("sistemaop.txt");
	if( !archivodatos.exists(stDatosDir + "smiles.txt") )
		CrearArchivoDato("smiles.txt");
	if( !archivodatos.exists(stDatosDir + "xmldb.txt") )
		CrearArchivoDato("xmldb.txt");
}

void GrLida::CrearArchivoDato(QString archivo)
{
	QString NombreArchivo;
	QFile *CrearArchivo;

	NombreArchivo = ":/datos/"+archivo;

	QFile LeerArchivo( NombreArchivo );
	if( !LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		return;
	}

	QTextStream in( &LeerArchivo );
	in.setCodec("UTF-8");
	QString line = in.readAll();
	LeerArchivo.close();

	NombreArchivo.clear();
	NombreArchivo = stDatosDir + archivo;

	CrearArchivo = new QFile( NombreArchivo );
	if( !CrearArchivo->open( QIODevice::WriteOnly | QIODevice::Text ) )
		return;

	QTextStream out( CrearArchivo );
	out.setCodec("UTF-8");
	out << line;
	out.flush();
	CrearArchivo->close();
	line.clear();
}
