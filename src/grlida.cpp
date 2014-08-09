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
#include "grlida_pdf_viewer.h"
#include "grlida_multimedia.h"
#include "grlida_instalar_juego.h"
#include "grlida_list_icon_cfg.h"
#include "grlida_update.h"
#include "grdap.h"
#include "ui_grlida.h"

#ifdef Q_OS_WIN32
#include <qt_windows.h>
#include <shellapi.h>
	#if defined (_MSC_VER)
		#pragma comment(lib, "SHELL32.LIB")
	#endif
#endif

GrLida::GrLida(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::GrLida)
{
	ui->setupUi(this);
	fGrl = new Funciones;

// Título de la aplicación
	setWindowTitle( tituloGrl() +" - v"+ fGrl->versionGrl() );

	grlDir.Home      = fGrl->GRlidaHomePath();
	grlDir.Capturas  = grlDir.Home +"capturas/";
	grlDir.Confdbx   = grlDir.Home +"confdbx/";
	grlDir.Confvdms  = grlDir.Home +"confvdms/";
	grlDir.Covers    = grlDir.Home +"covers/";
	grlDir.Datos     = grlDir.Home +"datos/";
	grlDir.Iconos    = grlDir.Home +"iconos/";
	grlDir.Idiomas   = grlDir.Home +"idiomas/";
	grlDir.Scripts   = grlDir.Home +"scripts/";
	grlDir.Smiles    = grlDir.Home +"smiles/";
	grlDir.Temp      = grlDir.Home +"temp/";
	grlDir.Templates = grlDir.Home +"templates/";
	grlDir.Themes    = grlDir.Home +"themes/";
	grlDir.Thumbs    = grlDir.Home +"thumbs/";

	grlCfg = fGrl->cargarGRLConfig(grlDir.Home +"GR-lida.conf");
	fGrl->setTheme(grlCfg.NameDirTheme);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	conectarBaseDatos();
	comprobarArchivosDatos(grlCfg.Version, grlCfg.IdiomaSelect);

	createToolBars();
	createStatusBar();
	createTrayIcon();
	cargarConfig();

	createConnections();

	isInicio = true;
	emit on_mnu_ver_fullscreen_triggered( grlCfg.main_fullscreen );

// Centra la aplicación en el escritorio
	if( !grlCfg.main_fullscreen )
		this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
	if( !grlCfg.main_state.isEmpty() )
		this->restoreState( grlCfg.main_state );
	if( !grlCfg.main_geometry.isEmpty() )
		this->restoreGeometry( grlCfg.main_geometry );
	isInicio = false;
}

GrLida::~GrLida()
{
	if( ui->dockw_PictureFlow->isFloating() )
		ui->dockw_PictureFlow->setFloating(false);

	delete grl_tv_delegate;
	delete grl_lv_delegate;
	delete grl_picflow;
	delete tv_model;
	delete lv_model;
	delete sql;
	delete fGrl;

	delete ui;
}

void GrLida::closeEvent(QCloseEvent *event)
{
	if( isTrayIcon && trayIcon->isVisible() )
	{
		this->hide();
		event->ignore();
	} else {
		guardarConfig();
		event->accept();
	}
}

bool GrLida::eventFilter(QObject *object, QEvent *event)
{
	if( event->type() == QEvent::KeyPress )
	{
		if( ui->menubar->isHidden() )
		{
			QKeyEvent *key_Event = static_cast<QKeyEvent*>(event);

			switch( key_Event->modifiers() )
			{
				case Qt::ControlModifier:
					switch( key_Event->key() )
					{
						case Qt::Key_Q:
							emit on_mnu_file_cerrar_triggered();
							return true;
						break;
						case Qt::Key_D:
							emit on_mnu_edit_nuevo_dosbox_triggered();
							return true;
						break;
						case Qt::Key_S:
							emit on_mnu_edit_nuevo_scummvm_triggered();
							return true;
						break;
						case Qt::Key_V:
							emit on_mnu_edit_nuevo_vdmsound_triggered();
							return true;
						break;
						case Qt::Key_E:
							emit on_mnu_ejecutar_juego_triggered();
							return true;
						break;
						case Qt::Key_I:
							emit on_mnu_tool_instalar_triggered();
							return true;
						break;
						case Qt::Key_O:
							emit on_mnu_tool_opciones_triggered();
							return true;
						break;
						case Qt::Key_F9:
							emit on_mnu_ver_statusbar_triggered(grlCfg.Pnl_StatusBar ? false : true);
							ui->mnu_ver_statusbar->setChecked( grlCfg.Pnl_StatusBar );
							return true;
						break;
						default:
							return false;
						break;
					}
				break;
				case Qt::AltModifier:
					switch( key_Event->key() )
					{
						case Qt::Key_S:
							emit on_mnu_ejecutar_setup_triggered();
							return true;
						break;
						default:
							return false;
						break;
					}
				break;
				case Qt::ShiftModifier:
					switch( key_Event->key() )
					{
						case Qt::Key_Insert:
							emit on_mnu_edit_editar_triggered();
							return true;
						break;
						case Qt::Key_F:
							emit on_mnu_edit_favorito_triggered(ui->mnu_edit_favorito->isChecked() ? false : true);
							return true;
						break;
						case Qt::Key_D:
							emit on_mnu_ejecutar_dosbox_triggered();
							return true;
						break;
						case Qt::Key_S:
							emit on_mnu_ejecutar_scummvm_triggered();
							return true;
						break;
						case Qt::Key_I:
							emit on_mnu_tool_importar_triggered();
							return true;
						break;
						case Qt::Key_E:
							emit on_mnu_tool_exportar_triggered();
							return true;
						break;
						default:
							return false;
						break;
					}
				break;
				default:
					switch( key_Event->key() )
					{
						case Qt::Key_Insert:
							emit on_mnu_edit_nuevo_triggered();
							return true;
						break;
						case Qt::Key_Delete:
							emit on_mnu_edit_eliminar_triggered();
							return true;
						break;
						case Qt::Key_F1:
							return true;
						break;
						case Qt::Key_F2:
							emit on_mnu_ver_nav_triggered(grlCfg.Pnl_Nav ? false : true);
							ui->mnu_ver_nav->setChecked( grlCfg.Pnl_Nav );
							return true;
						break;
						case Qt::Key_F3:
							emit on_mnu_ver_cover_mode_triggered(grlCfg.ver_IconMode ? false : true);
							ui->mnu_ver_cover_mode->setChecked( grlCfg.ver_IconMode );
							return true;
						break;
						case Qt::Key_F4:
							emit on_mnu_ver_pictureflow_triggered(grlCfg.Pnl_PictureFlow ? false : true);
							ui->mnu_ver_pictureflow->setChecked( grlCfg.Pnl_PictureFlow );
							return true;
						break;
						case Qt::Key_F5:
							emit on_mnu_ver_datos_triggered(grlCfg.Pnl_Datos ? false : true);
							ui->mnu_ver_datos->setChecked( grlCfg.Pnl_Datos );
							return true;
						break;
						case Qt::Key_F6:
							emit on_mnu_ver_archivos_url_triggered(grlCfg.Pnl_FilesUrl ? false : true);
							ui->mnu_ver_archivos_url->setChecked( grlCfg.Pnl_FilesUrl );
							return true;
						break;
						case Qt::Key_F7:
							emit on_mnu_ver_capturas_triggered(grlCfg.Pnl_Capturas ? false : true);
							ui->mnu_ver_capturas->setChecked( grlCfg.Pnl_Capturas );
							return true;
						break;
						case Qt::Key_F8:
							emit on_mnu_ver_pictureflow_to_center_triggered(grlCfg.PictureFlowToCenter ? false : true);
							ui->mnu_ver_pictureflow_to_center->setChecked( grlCfg.PictureFlowToCenter );
							return true;
						break;
						case Qt::Key_F9:
							emit on_mnu_ver_menubar_triggered(grlCfg.Pnl_Menubar ? false : true);
							ui->mnu_ver_menubar->setChecked( grlCfg.Pnl_Menubar );
							return true;
						break;
						case Qt::Key_F10:
							emit on_mnu_ver_toolbar_triggered(grlCfg.Pnl_Toolbar ? false : true);
							ui->mnu_ver_toolbar->setChecked( grlCfg.Pnl_Toolbar );
							return true;
						break;
						case Qt::Key_F11:
							emit on_mnu_ver_fullscreen_triggered(grlCfg.main_fullscreen ? false : true);
							ui->mnu_ver_fullscreen->setChecked( grlCfg.main_fullscreen );
							return true;
						break;
						case Qt::Key_F12:
							emit on_mnu_ver_ordenar_triggered(grlCfg.Pnl_Ordenar ? false : true);
							ui->mnu_ver_ordenar->setChecked( grlCfg.Pnl_Ordenar );
							return true;
						break;
					}
				break;
			}
		}
	}

	if( object == ui->tvJuegos || object == ui->lvJuegos || object == grl_picflow )
	{
		if( event->type() == QEvent::KeyPress )
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

			switch( keyEvent->key() )
			{
				case Qt::Key_Enter:
				case Qt::Key_Space:
				case Qt::Key_Return:
					emit on_mnu_ejecutar_juego_triggered();
					return true;
				break;
				case Qt::Key_Left:
				case Qt::Key_Up:
					itemBack();
					return true;
				break;
				case Qt::Key_Right:
				case Qt::Key_Down:
					itemNext();
					return true;
				break;
				default:
					return false;
				break;
			}
		}// else
		//	return false;
	}

	return QMainWindow::eventFilter(object, event);
}

void GrLida::setVisible(bool visible)
{
	ui->mnu_trayicon_minimize->setEnabled(visible);
	ui->mnu_trayicon_maximize->setEnabled(!isMaximized());
	ui->mnu_trayicon_restore->setEnabled(isMaximized() || !visible);

	QMainWindow::setVisible(visible);
}

// Hace la conexión con la base de datos.
void GrLida::conectarBaseDatos()
{
	sql = new dbSql(grlCfg.db_type, grlCfg.db_server, fGrl->getDirRelative(grlCfg.db_host), grlCfg.db_name, grlCfg.db_username, grlCfg.db_password, grlCfg.db_port, fGrl->Idioma(), grlCfg.NameDirTheme);

	if( !sql->dbisOpen() )
		QMessageBox::critical(this, tr("Error al abrir la base de datos"), tr("No se ha podido establecer una conexión con la base de datos.\nEsta aplicación necesita soporte de SQLite. Mira la documentación de Qt SQL driver para más información.\n\nClick cancelar para salir."), QMessageBox::Cancel);
	else
		sql->crearTablas();
}

// Crea la barra de herramientas, busqueda y ordenar
void GrLida::createToolBars()
{
	ui->tb_main->clear();
	if( grlCfg.ToolbarBigIcon )
	{
		ui->tb_main->setIconSize(QSize(32,32));
		ui->cbxCategoriasTb->setIconSize(QSize(24,24));
		ui->cbxCategoriasTb->setMinimumSize(QSize(140,30));
	} else {
		ui->tb_main->setIconSize(QSize(16,16));
		ui->cbxCategoriasTb->setIconSize(QSize(16,16));
		ui->cbxCategoriasTb->setMinimumSize(QSize(140,22));
	}

	ui->tb_main->addAction( ui->mnu_file_cerrar_big );
	ui->tb_main->addSeparator();
	ui->tb_main->addAction( ui->mnu_edit_nuevo_dosbox_big );
	ui->tb_main->addAction( ui->mnu_edit_nuevo_scummvm_big );
	ui->tb_main->addAction( ui->mnu_edit_nuevo_vdmsound_big );
	ui->tb_main->addSeparator();
	ui->tb_main->addAction( ui->mnu_ejecutar_juego_big );
	ui->tb_main->addAction( ui->mnu_ejecutar_setup_big );
	ui->tb_main->addSeparator();
	ui->tb_main->addAction( ui->mnu_edit_nuevo_big );
	ui->tb_main->addAction( ui->mnu_edit_editar_big );
	ui->tb_main->addAction( ui->mnu_edit_eliminar_big );
	ui->tb_main->addSeparator();
	ui->tb_main->addWidget( ui->cbxCategoriasTb );
	ui->tb_main->addSeparator();
	ui->tb_main->addAction( ui->mnu_tool_importar_big );
	ui->tb_main->addAction( ui->mnu_ver_datos_big );
	ui->tb_main->addAction( ui->mnu_ver_archivos_url_big );
	ui->tb_main->addAction( ui->mnu_ver_capturas_big );
	ui->tb_main->addSeparator();
	ui->tb_main->addAction( ui->mnu_tool_opciones_big );
	ui->tb_main->addAction( ui->mnu_ver_acercad_big );

	ui->tb_ordenar->clear();
	ui->tb_ordenar->addWidget( ui->frameOrdenar );

	ui->cbxOrdenBy->clear();
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Por ID")       , "idgrl"        );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Título")       , "titulo"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Subtitulo")    , "subtitulo"    );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Genero")       , "genero"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Compañia")     , "compania"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Desarrollador"), "desarrollador");
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Tema")         , "tema"         );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Idioma")       , "idioma"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Formato")      , "formato"      );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Año")          , "anno"         );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Nº discos")    , "numdisc"      );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Sistema OS")   , "sistemaop"    );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Gráficos")     , "graficos"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Sonido")       , "sonido"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Jugabilidad")  , "jugabilidad"  );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Original")     , "original"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Tipo emulador"), "tipo_emu"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Favorito")     , "favorito"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png"), tr("Rating")       , "rating"       );

	ui->cbxOrden->clear();
	ui->cbxOrden->addItem(QIcon(fGrl->Theme() +"img16/orden_asc.png") , tr("Ascendente") , "ASC" );
	ui->cbxOrden->addItem(QIcon(fGrl->Theme() +"img16/orden_desc.png"), tr("Descendente"), "DESC");
}

void GrLida::createStatusBar()
{
	lb_panel_1 = new QLabel( ui->statusBar );
	lb_panel_1->setFrameStyle( QFrame::NoFrame );
	lb_panel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_1->setPixmap( QPixmap(fGrl->Theme() +"img16/datos_3.png") );
	lb_panel_1->setMinimumSize(20,20);

	lb_panel_2 = new QLabel( ui->statusBar );
	lb_panel_2->setFrameStyle( QFrame::NoFrame );
	lb_panel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	lb_panel_3 = new QLabel( ui->statusBar );
	lb_panel_3->setFrameStyle( QFrame::NoFrame );
	lb_panel_3->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/sinimg.png") );
	lb_panel_3->setMinimumSize(20,20);

	lb_panel_4 = new QLabel( ui->statusBar );
	lb_panel_4->setFrameStyle( QFrame::NoFrame );
	lb_panel_4->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_4->setPixmap( QPixmap(fGrl->Theme() +"img16/datos_1.png") );
	lb_panel_4->setMinimumSize(20,20);

	lb_panel_5 = new QLabel( ui->statusBar );
	lb_panel_5->setFrameStyle( QFrame::NoFrame );
	lb_panel_5->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui->statusBar->addWidget( lb_panel_1 );
	ui->statusBar->addWidget( lb_panel_2 );
	ui->statusBar->addWidget( lb_panel_3 );
	ui->statusBar->addWidget( lb_panel_4 );
	ui->statusBar->addWidget( lb_panel_5, 1 );
}

void GrLida::createTrayIcon()
{
	if( QSystemTrayIcon::isSystemTrayAvailable() )
	{
		trayIconMenu = new QMenu(this);
		trayIconMenu->addAction( ui->mnu_trayicon_restore  );
		trayIconMenu->addAction( ui->mnu_trayicon_minimize );
		trayIconMenu->addAction( ui->mnu_trayicon_maximize );
		trayIconMenu->addSeparator();
		trayIconMenu->addAction( ui->mnu_file_cerrar );
		trayIcon = new QSystemTrayIcon(this);
		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(fGrl->Theme() +"img16/gr-lida.png"));
		trayIcon->setToolTip("GR-lida");

		trayIcon->hide();

		connect(ui->mnu_trayicon_restore , SIGNAL(triggered()), this, SLOT(showNormal()));
		connect(ui->mnu_trayicon_minimize, SIGNAL(triggered()), this, SLOT(hide()));
		connect(ui->mnu_trayicon_maximize, SIGNAL(triggered()), this, SLOT(showMaximized()));

		isTrayIcon = true;
	} else
		isTrayIcon = false;
}

// Conecta los distintos botones con las funciones de cada uno
void GrLida::createConnections()
{
// Conecta la Lista de Categorias
	connect(grl_picflow, SIGNAL(centerIndexChanged(int)), this, SLOT(skipPicFlow(int)));
	connect(grl_picflow, SIGNAL(selected(unsigned int)), this, SLOT(on_mnu_ejecutar_juego_triggered()));
	connect(grl_picflow, SIGNAL(finishedAnimation()), this, SLOT(finishedAnimationPicFlow()));
	connect(grl_picflow, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(grl_picflow_customContextMenuRequested(const QPoint &)));
	connect(grl_picflow, SIGNAL(isActive(bool)), this, SLOT(isActivePicFlow(bool)));
}

// Carga la configuración del GR-lida.
void GrLida::cargarConfig()
{
	if( grlCfg.main_maximized )
		setWindowState( windowState() | Qt::WindowMaximized );

	ui->mnu_ver_nav->setChecked         ( grlCfg.Pnl_Nav         );
	ui->mnu_ver_cover_mode->setChecked  ( grlCfg.ver_IconMode    );
	ui->mnu_ver_archivos_url->setChecked( grlCfg.Pnl_FilesUrl    );
	ui->mnu_ver_datos->setChecked       ( grlCfg.Pnl_Datos       );
	ui->mnu_ver_pictureflow->setChecked ( grlCfg.Pnl_PictureFlow );
	ui->mnu_ver_capturas->setChecked    ( grlCfg.Pnl_Capturas    );
	ui->mnu_ver_pictureflow_to_center->setChecked(grlCfg.PictureFlowToCenter);
	ui->mnu_ver_menubar->setChecked     ( grlCfg.Pnl_Menubar     );
	ui->mnu_ver_toolbar->setChecked     ( grlCfg.Pnl_Toolbar     );
	ui->mnu_ver_ordenar->setChecked     ( grlCfg.Pnl_Ordenar     );
	ui->mnu_ver_statusbar->setChecked   ( grlCfg.Pnl_StatusBar   );
	ui->mnu_ver_archivos_url_big->setChecked( grlCfg.Pnl_FilesUrl);
	ui->mnu_ver_datos_big->setChecked   ( grlCfg.Pnl_Datos       );
	ui->mnu_ver_capturas_big->setChecked( grlCfg.Pnl_Capturas    );
	ui->dockw_Nav->setVisible           ( grlCfg.Pnl_Nav         );
	ui->dockw_Datos->setVisible         ( grlCfg.Pnl_Datos       );
	ui->dockw_FilesUrl->setVisible      ( grlCfg.Pnl_FilesUrl    );
	ui->btnVer_Archivos->setChecked     ( grlCfg.Pnl_FilesUrl    );
	ui->twCapturas->setVisible          ( grlCfg.Pnl_Capturas    );
	ui->menubar->setVisible             ( grlCfg.Pnl_Menubar     );
	ui->tb_main->setVisible             ( grlCfg.Pnl_Toolbar     );
	ui->tb_ordenar->setVisible          ( grlCfg.Pnl_Ordenar     );
	ui->statusBar->setVisible           ( grlCfg.Pnl_StatusBar   );

	ui->mnu_ver_fullscreen->setChecked       ( grlCfg.main_fullscreen       );
	ui->mnu_ver_col_icono->setChecked        ( grlCfg.ver_col_Icono         );
	ui->mnu_ver_col_subtitulo->setChecked    ( grlCfg.ver_col_Subtitulo     );
	ui->mnu_ver_col_genero->setChecked       ( grlCfg.ver_col_Genero        );
	ui->mnu_ver_col_compania->setChecked     ( grlCfg.ver_col_Compania      );
	ui->mnu_ver_col_desarrollador->setChecked( grlCfg.ver_col_Desarrollador );
	ui->mnu_ver_col_tema->setChecked         ( grlCfg.ver_col_Tema          );
	ui->mnu_ver_col_grupo->setChecked        ( grlCfg.ver_col_Grupo         );
	ui->mnu_ver_col_perspectiva->setChecked  ( grlCfg.ver_col_Perspectiva   );
	ui->mnu_ver_col_idioma->setChecked       ( grlCfg.ver_col_Idioma        );
	ui->mnu_ver_col_idioma_voces->setChecked ( grlCfg.ver_col_IdiomaVoces   );
	ui->mnu_ver_col_formato->setChecked      ( grlCfg.ver_col_Formato       );
	ui->mnu_ver_col_anno->setChecked         ( grlCfg.ver_col_Anno          );
	ui->mnu_ver_col_numdisc->setChecked      ( grlCfg.ver_col_NumDisc       );
	ui->mnu_ver_col_sistemaop->setChecked    ( grlCfg.ver_col_SistemaOp     );
	ui->mnu_ver_col_tamano->setChecked       ( grlCfg.ver_col_Tamano        );
	ui->mnu_ver_col_graficos->setChecked     ( grlCfg.ver_col_Graficos      );
	ui->mnu_ver_col_sonido->setChecked       ( grlCfg.ver_col_Sonido        );
	ui->mnu_ver_col_jugabilidad->setChecked  ( grlCfg.ver_col_Jugabilidad   );
	ui->mnu_ver_col_original->setChecked     ( grlCfg.ver_col_Original      );
	ui->mnu_ver_col_estado->setChecked       ( grlCfg.ver_col_Estado        );
	ui->mnu_ver_col_fecha->setChecked        ( grlCfg.ver_col_Fecha         );
	ui->mnu_ver_col_tipoemu->setChecked      ( grlCfg.ver_col_TipoEmu       );
	ui->mnu_ver_col_favorito->setChecked     ( grlCfg.ver_col_Favorito      );
	ui->mnu_ver_col_rating->setChecked       ( grlCfg.ver_col_Rating        );
	ui->mnu_ver_col_edad->setChecked         ( grlCfg.ver_col_Edad          );
	ui->mnu_ver_col_usuario->setChecked      ( grlCfg.ver_col_Usuario       );

#ifdef Q_OS_WIN32
	if( grlCfg.DirDOSBox.isEmpty() && grlCfg.DirScummVM.isEmpty() && ui->mnu_edit_nuevo_vdmsound_big->isEnabled() == false )
		ui->mnu_ejecutar_juego_big->setEnabled( false );
#else
	if( grlCfg.DirDOSBox.isEmpty() && grlCfg.DirScummVM.isEmpty() )
		ui->mnu_ejecutar_juego_big->setEnabled( false );
#endif

	ui->cbxOrdenBy->setCurrentIndex( ui->cbxOrdenBy->findData( grlCfg.db_orden_by ) );
	ui->cbxOrden->setCurrentIndex( ui->cbxOrden->findData( grlCfg.db_orden ) );

	ui->tabFileUrl->setCurrentIndex( grlCfg.IndexTabArchivos );

	grl_tv_delegate = new GrlTreeViewDelegate(sql, grlDir.Home, fGrl->Theme(), fGrl->ThemeApp(), grlCfg.FormatsImage.join(";"), ui->twListNav, ui->tvJuegos);
	grl_lv_delegate = new GrlListViewDelegate(sql, grlDir.Home, fGrl->Theme(), fGrl->ThemeApp(), grlCfg.FormatsImage.join(";"), ui->lvJuegos);

	grl_picflow = new GrlPicFlow(ui->dockw_PictureFlow);
	grl_picflow->setFocusPolicy(Qt::StrongFocus);
	grl_picflow->setFocus(Qt::OtherFocusReason);
	grl_picflow->installEventFilter(this);
	grl_picflow->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->dockw_PictureFlow->setVisible( grlCfg.Pnl_PictureFlow );

// Configuración del twListNav
	ui->twListNav->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png"));
	ui->twListNav->header()->setMovable(false);
	ui->twListNav->header()->setStretchLastSection(false);
	ui->twListNav->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twListNav->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->twListNav->setColumnWidth(1, 60);

// Creamos el data model para el QTreeView
	tv_model = new GrlTreeViewModel(ui->tvJuegos, sql->getSqlDB(), grlCfg.FormatoFecha);
	tv_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
// Configuración del tvJuegos
	ui->tvJuegos->setVisible( !grlCfg.ver_IconMode );
	ui->tvJuegos->setItemDelegate( grl_tv_delegate );
	ui->tvJuegos->installEventFilter(this);
	ui->tvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tvJuegos->setRootIsDecorated(false);
	ui->tvJuegos->setUniformRowHeights(true);
	ui->tvJuegos->setAllColumnsShowFocus(true);

// Creamos el data model para el QListView
	lv_model = new QStandardItemModel( ui->lvJuegos );
// Configuración del lvJuegos
	ui->lvJuegos->setVisible( grlCfg.ver_IconMode );
	ui->lvJuegos->setItemDelegate( grl_lv_delegate );
	ui->lvJuegos->installEventFilter(this);
	ui->lvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->lvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->lvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);

// Configuración del twFiles
	ui->twFiles->installEventFilter(this);
	ui->twFiles->header()->setStretchLastSection(true);
	ui->twFiles->header()->setMovable(false);
	ui->twFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui->twFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(4, QHeaderView::Interactive);
	ui->twFiles->setColumnWidth(0, 200 );
	ui->twFiles->setColumnWidth(1, 60  );
	ui->twFiles->setColumnWidth(2, 100 );
	ui->twFiles->setColumnWidth(3, 50  );
	ui->twFiles->setColumnWidth(4, 50  );

// Configuración del twUrls
	ui->twUrls->installEventFilter(this);
	ui->twUrls->header()->setStretchLastSection(true);
	ui->twUrls->header()->setMovable(false);
	ui->twUrls->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twUrls->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twUrls->setColumnWidth(0, 250 );

// Configuración del twCapturas
	ui->twCapturas->installEventFilter(this);
	ui->twCapturas->header()->setStretchLastSection(true);
	ui->twCapturas->header()->setMovable(false);
	ui->twCapturas->setMinimumSize(QSize(170, 0));
	ui->twCapturas->setMaximumSize(QSize(170, 16777215));

// Configuración del twCapturaVideo
	ui->twCapturaVideo->installEventFilter(this);
	ui->twCapturaVideo->header()->setStretchLastSection(false);
	ui->twCapturaVideo->header()->setMovable(false);
	ui->twCapturaVideo->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twCapturaVideo->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twCapturaVideo->setColumnWidth(1, 200 );

// Configuración del twCapturaSonido
	ui->twCapturaSonido->installEventFilter(this);
	ui->twCapturaSonido->header()->setStretchLastSection(false);
	ui->twCapturaSonido->header()->setMovable(false);
	ui->twCapturaSonido->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twCapturaSonido->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twCapturaSonido->setColumnWidth(1, 200 );

	ui->txtInfo->installEventFilter(this);

	ui->btnVer_CoverFront->setEnabled( false );
	ui->btnVer_CoverBack->setEnabled( false );

	ui->tb_main->setContextMenuPolicy( Qt::PreventContextMenu );
	ui->tb_ordenar->setContextMenuPolicy( Qt::PreventContextMenu );

	smiles_list.clear();
	idiomas_list.clear();
	edades_list.clear();
	smiles_list  = fGrl->cargaDatosQHash(grlDir.Datos +"smiles.txt", 2, "\",\"");
	idiomas_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->Idioma() +"/idiomas.txt", 3, "|");
	edades_list  = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->Idioma() +"/edad_recomendada.txt", 3, "|");

	dbx_list.clear();
	dbx_list = fGrl->cargaDatosQHash(grlDir.Datos +"dbx_list.txt", 6, "|");
	stGrlDatos dbx_dato;
	dbx_dato.titulo  = "DOSBox";
	dbx_dato.icono   = "dosbox.png";
	dbx_dato.extra   = grlCfg.DirDOSBox;
	dbx_dato.issvn   = grlCfg.DOSBoxSVN;
	dbx_dato.version = grlCfg.VersionDBx;
	dbx_dato.key     = "dosbox";
	dbx_list.insert(dbx_dato.key, dbx_dato);

	IdGame  = "";
	TipoEmu = "";
	sql_where_select = "";
	row_select   = 0;
	col_select   = 0;
	skip_picflow = 0;

	if( grlCfg.LastSelectCatID.isEmpty() )
		id_cat = 0;
	else
		id_cat = fGrl->StrToInt(grlCfg.LastSelectCatID);

	setTheme();

	emit on_mnu_ver_pictureflow_to_center_triggered(grlCfg.PictureFlowToCenter);

	cargarListaCategorias();

// Comprobación de nuevas versiones.
	if( grlCfg.chkVersion )
		checkUpdateGrl(false);
}

// Guarda la configuración del GR-lida.
void GrLida::guardarConfig()
{
	QSettings settings(grlDir.Home +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("ImageViewerState");
		grlCfg.img_maximized        = settings.value("maximized"       , false).toBool();
		grlCfg.img_fitToWindow      = settings.value("fitToWindow"     , false).toBool();
		grlCfg.img_verListaImagenes = settings.value("verListaImagenes", false).toBool();
	settings.endGroup();

	if( !grlCfg.ver_IconMode )
		grlCfg.main_twJuegos_state = ui->tvJuegos->header()->saveState();

	grlCfg.Version               = fGrl->versionGrl();
	grlCfg.main_maximized        = this->isMaximized();
	grlCfg.main_geometry         = this->saveGeometry();
	grlCfg.main_state            = this->saveState(); // Toolbar and DockWidget state.
	grlCfg.LastSelectCatID       = categoria[id_cat].id;
	grlCfg.IndexTabArchivos      = ui->tabFileUrl->currentIndex();

	grlCfg.ver_col_Subtitulo     = ui->mnu_ver_col_subtitulo->isChecked();
	grlCfg.ver_col_Genero        = ui->mnu_ver_col_genero->isChecked();
	grlCfg.ver_col_Compania      = ui->mnu_ver_col_compania->isChecked();
	grlCfg.ver_col_Desarrollador = ui->mnu_ver_col_desarrollador->isChecked();
	grlCfg.ver_col_Tema          = ui->mnu_ver_col_tema->isChecked();
	grlCfg.ver_col_Grupo         = ui->mnu_ver_col_grupo->isChecked();
	grlCfg.ver_col_Perspectiva   = ui->mnu_ver_col_perspectiva->isChecked();
	grlCfg.ver_col_Idioma        = ui->mnu_ver_col_idioma->isChecked();
	grlCfg.ver_col_IdiomaVoces   = ui->mnu_ver_col_idioma_voces->isChecked();
	grlCfg.ver_col_Formato       = ui->mnu_ver_col_formato->isChecked();
	grlCfg.ver_col_Anno          = ui->mnu_ver_col_anno->isChecked();
	grlCfg.ver_col_NumDisc       = ui->mnu_ver_col_numdisc->isChecked();
	grlCfg.ver_col_SistemaOp     = ui->mnu_ver_col_sistemaop->isChecked();
	grlCfg.ver_col_Tamano        = ui->mnu_ver_col_tamano->isChecked();
	grlCfg.ver_col_Graficos      = ui->mnu_ver_col_graficos->isChecked();
	grlCfg.ver_col_Sonido        = ui->mnu_ver_col_sonido->isChecked();
	grlCfg.ver_col_Jugabilidad   = ui->mnu_ver_col_jugabilidad->isChecked();
	grlCfg.ver_col_Original      = ui->mnu_ver_col_original->isChecked();
	grlCfg.ver_col_Fecha         = ui->mnu_ver_col_fecha->isChecked();
	grlCfg.ver_col_Edad          = ui->mnu_ver_col_edad->isChecked();
	grlCfg.ver_col_Rating        = ui->mnu_ver_col_rating->isChecked();

	fGrl->guardarGRLConfig(grlDir.Home +"GR-lida.conf", grlCfg);
}

void GrLida::setTheme()
{
	str_html_old = "";
	if( QFile::exists(fGrl->ThemeApp() +"tpl_juego_info.html") )
		str_html_old = fGrl->leerArchivo(fGrl->ThemeApp() +"tpl_juego_info.html", "UTF-8");
	else
		str_html_old = fGrl->leerArchivo(":/tpl_juego_info.html", "UTF-8");

// Reempla las etiquetas
	str_html_old.replace("{lb_cover_front}"     , tr("Carátula frontal")  );
	str_html_old.replace("{lb_detalles_juego}"  , tr("Detalles del juego"));
	str_html_old.replace("{lb_calificacion}"    , tr("Calificación")      );
	str_html_old.replace("{lb_otros_datos}"     , tr("Otros datos")       );
	str_html_old.replace("{lb_usuario}"         , tr("Subido por")        );
	str_html_old.replace("{lb_subtitulo}"       , tr("Subtitulo")         );
	str_html_old.replace("{lb_compania}"        , tr("Publicado por")     );
	str_html_old.replace("{lb_desarrollador}"   , tr("Desarrollado por")  );
	str_html_old.replace("{lb_anno}"            , tr("Publicado")         );
	str_html_old.replace("{lb_edad_recomendada}", tr("Edad recomendada")  );
	str_html_old.replace("{lb_idioma}"          , tr("Idioma")            );
	str_html_old.replace("{lb_idioma_voces}"    , tr("Idioma voces")      );
	str_html_old.replace("{lb_formato}"         , tr("Formato")           );
	str_html_old.replace("{lb_genero}"          , tr("Genero")            );
	str_html_old.replace("{lb_tema}"            , tr("Tema")              );
	str_html_old.replace("{lb_grupo}"           , tr("Grupo")             );
	str_html_old.replace("{lb_perspectiva}"     , tr("Perspectiva")       );
	str_html_old.replace("{lb_sistemaop}"       , tr("Sistema operativo") );
	str_html_old.replace("{lb_favorito}"        , tr("Favorito")          );
	str_html_old.replace("{lb_fecha}"           , tr("Añadido el")        );
	str_html_old.replace("{lb_graficos}"        , tr("Gráficos")          );
	str_html_old.replace("{lb_sonido}"          , tr("Sonido")            );
	str_html_old.replace("{lb_jugabilidad}"     , tr("Jugabilidad")       );
	str_html_old.replace("{lb_original}"        , tr("Original")          );
	str_html_old.replace("{lb_estado}"          , tr("Estado")            );
	str_html_old.replace("{lb_tipo_emu}"        , tr("Tipo Emu")          );
	str_html_old.replace("{lb_comentario}"      , tr("Descripción")       );
	str_html_old.replace("{lb_rating}"          , tr("Rating")            );
	str_html_old.replace("{lb_numdisc}"         , tr("Nº discos")         );

	lb_panel_1->setPixmap( QPixmap(fGrl->Theme() +"img16/datos_3.png") );
	lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/sinimg.png") );
	lb_panel_4->setPixmap( QPixmap(fGrl->Theme() +"img16/datos_1.png") );

// Barra de heramientas
	QString ico_size = (grlCfg.ToolbarBigIcon) ? "32" : "16";
	ui->mnu_file_cerrar_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/cerrar.png") );
	ui->mnu_edit_nuevo_dosbox_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/dosbox.png") );
	ui->mnu_edit_nuevo_scummvm_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/scummvm.png") );
	ui->mnu_edit_nuevo_vdmsound_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/vdmsound.png") );
	ui->mnu_ejecutar_juego_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/ejecutar_app.png") );
	ui->mnu_ejecutar_setup_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/ejecutar_app_setup.png") );
	ui->mnu_edit_nuevo_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/nuevo.png") );
	ui->mnu_edit_editar_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/editar.png") );
	ui->mnu_edit_eliminar_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/eliminar.png") );
	ui->mnu_tool_importar_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/importar.png") );
	ui->mnu_ver_archivos_url_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/archivos.png") );
	ui->mnu_ver_datos_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/datos.png") );
	ui->mnu_ver_capturas_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/capturas.png") );
	ui->mnu_tool_opciones_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/opciones.png") );
	ui->mnu_ver_acercad_big->setIcon( QIcon(fGrl->Theme() +"img"+ ico_size +"/acercad.png") );
// Menú archivo
	ui->mnu_file_informacion->setIcon( QIcon(fGrl->Theme() +"img16/informacion.png") );
	ui->mnu_file_cerrar->setIcon( QIcon(fGrl->Theme() +"img16/cerrar.png") );
// Menú editar
	ui->mnu_edit_nuevo_dosbox->setIcon( QIcon(fGrl->Theme() +"img16/dosbox.png") );
	ui->mnu_edit_nuevo_scummvm->setIcon( QIcon(fGrl->Theme() +"img16/scummvm.png") );
	ui->mnu_edit_nuevo_vdmsound->setIcon( QIcon(fGrl->Theme() +"img16/vdmsound.png") );
	ui->mnu_edit_nuevo->setIcon( QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->mnu_edit_editar->setIcon( QIcon(fGrl->Theme() +"img16/editar.png") );
	ui->mnu_edit_eliminar->setIcon( QIcon(fGrl->Theme() +"img16/eliminar.png") );
	ui->mnu_edit_favorito->setIcon( QIcon(fGrl->Theme() +"img16/fav_0.png") );
// Menú ejecutar
	ui->mnu_ejecutar_dosbox->setIcon( QIcon(fGrl->Theme() +"img16/dosbox.png") );
	ui->mnu_ejecutar_scummvm->setIcon( QIcon(fGrl->Theme() +"img16/scummvm.png") );
	ui->mnu_ejecutar_juego->setIcon( QIcon(fGrl->Theme() +"img16/ejecutar_app.png") );
	ui->mnu_ejecutar_setup->setIcon( QIcon(fGrl->Theme() +"img16/ejecutar_app_setup.png") );
// Menú herramientas
	ui->mnu_tool_instalar->setIcon( QIcon(fGrl->Theme() +"img16/ejecutar_app_setup.png") );
	ui->mnu_tool_importar->setIcon( QIcon(fGrl->Theme() +"img16/importar.png") );
	ui->mnu_tool_exportar->setIcon( QIcon(fGrl->Theme() +"img16/exportar.png") );
	ui->mnu_tool_opciones->setIcon( QIcon(fGrl->Theme() +"img16/opciones.png") );
// Menú ver
	ui->mnu_ver_nav->setIcon( QIcon(fGrl->Theme() +"img16/categorias.png") );
	ui->mnu_ver_cover_mode->setIcon( QIcon(fGrl->Theme() +"img16/cover_mode.png") );
	ui->mnu_ver_datos->setIcon( QIcon(fGrl->Theme() +"img16/datos.png") );
	ui->mnu_ver_archivos_url->setIcon( QIcon(fGrl->Theme() +"img16/archivos.png") );
	ui->mnu_ver_capturas->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->mnu_ver_menubar->setIcon( QIcon(fGrl->Theme() +"img16/barra_herramintas.png") );
	ui->mnu_ver_toolbar->setIcon( QIcon(fGrl->Theme() +"img16/barra_herramintas.png") );
	ui->mnu_ver_ordenar->setIcon( QIcon(fGrl->Theme() +"img16/orden_asc.png") );
	ui->mnu_ver_statusbar->setIcon( QIcon(fGrl->Theme() +"img16/barra_herramintas.png") );
	ui->mnu_ver_fullscreen->setIcon( QIcon(fGrl->Theme() +"img16/fit_to_win.png") );
	ui->mnu_ver_ayuda->setIcon( QIcon(fGrl->Theme() +"img16/ayuda.png") );
	ui->mnu_ver_check_updates->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->mnu_ver_acercad->setIcon( QIcon(fGrl->Theme() +"img16/acercad.png") );
// Menú ver columnas
	ui->menuMenuVerColumnas->setIcon( QIcon(fGrl->Theme() +"img16/column_list.png") );
// Menú ver carpetas
	ui->menuMenuVerCarpetas->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->mnu_ver_carpeta_confdbx->setIcon( QIcon(fGrl->Theme() +"img16/dosbox.png") );
	ui->mnu_ver_carpeta_confvdms->setIcon( QIcon(fGrl->Theme() +"img16/vdmsound.png") );
	ui->mnu_ver_carpeta_thumbs->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->mnu_ver_carpeta_covers->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->mnu_ver_carpeta_iconos->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->mnu_ver_carpeta_smiles->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->mnu_ver_carpeta_themes->setIcon( QIcon(fGrl->Theme() +"img16/style.png") );
	ui->mnu_ver_carpeta_datos->setIcon( QIcon(fGrl->Theme() +"img16/datos_3.png") );
	ui->mnu_ver_carpeta_scripts->setIcon( QIcon(fGrl->Theme() +"img16/archivo_config.png") );
	ui->mnu_ver_carpeta_templates->setIcon( QIcon(fGrl->Theme() +"img16/datos_3.png") );
	ui->mnu_ver_carpeta_idiomas->setIcon( QIcon(fGrl->Theme() +"img16/idiomas.png") );
	ui->mnu_ver_carpeta_temp->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->mnu_ver_carpeta_home->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
// Otros botones
	ui->btnVer_Archivos->setIcon( QIcon(fGrl->Theme() +"img16/archivos.png") );
	ui->btnVer_CoverFront->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->btnVer_CoverBack->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->btnOrdenar->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
// Tab archivo/url
	ui->tabFileUrl->setTabIcon(0, QIcon(fGrl->Theme() +"img16/img_tv2x.png") );
	ui->tabFileUrl->setTabIcon(1, QIcon(fGrl->Theme() +"img16/img_audio.png") );
	ui->tabFileUrl->setTabIcon(2, QIcon(fGrl->Theme() +"img16/edit_enlace.png") );
	ui->tabFileUrl->setTabIcon(3, QIcon(fGrl->Theme() +"img16/archivos.png") );

	grl_picflow->setPicFlowType(grlCfg.PicFlowType);
	grl_picflow->setShowNumPos(grlCfg.PicFlowShowNumPos);
	grl_picflow->setShowTriangle(grlCfg.PicFlowShowTriangle);
	grl_picflow->setMinimumHeight(grlCfg.PicFlowMinHeight);
	grl_picflow->setBackgroundColor(fGrl->getColor(grlCfg.PicFlowBgColor));
	ui->dockw_PictureFlow->setMinimumHeight( grlCfg.PicFlowMinHeight );

	ui->lvJuegos->setStyleSheet( fGrl->myStyleSheet(true) );

	if( grlCfg.font_usar )
		this->setStyleSheet(fGrl->myStyleSheet() +"*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}");
	else
		this->setStyleSheet(fGrl->myStyleSheet());

	this->setStyle(QStyleFactory::create(grlCfg.Style));

	if( grlCfg.StylePalette )
		this->setPalette(QApplication::style()->standardPalette());
	else
		this->setPalette(QApplication::palette());
}

void GrLida::showPopup(QWidget *w_parent, const QPoint &pos)
{
// Creando el menú
	ljMenuPopUp = new QMenu(w_parent);

	ljMenuPopUp->addAction(ui->mnu_ejecutar_juego);
	ljMenuPopUp->addAction(ui->mnu_ejecutar_setup);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui->mnu_edit_nuevo);
	ljMenuPopUp->addAction(ui->mnu_edit_editar);
	ljMenuPopUp->addAction(ui->mnu_edit_eliminar);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui->mnu_edit_favorito);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addMenu(ui->menuVer);
	ljMenuPopUp->addAction(ui->mnu_ver_fullscreen);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui->mnu_tool_opciones);

// Muestra popupmenu
	ljMenuPopUp->exec(w_parent->mapToGlobal(pos));
	delete ljMenuPopUp;
}

void GrLida::checkUpdateGrl(bool is_menu)
{
	isUpdateMenu = is_menu;
	frmUpdate *UpdateNew = new frmUpdate(grlCfg, this);
	UpdateNew->checkUpdateGrl();
	UpdateNew->exec();
	grlCfg = UpdateNew->getGrlCfg();

	delete UpdateNew;
}

// Carga las categorias de la base de datos.
void GrLida::cargarListaCategorias(bool isEmit)
{
	QString ico_size = (grlCfg.ToolbarBigIcon) ? "24" : "16";

	QSqlQuery query(sql->getSqlDB());
	stGrlCats cat;

	categoria.clear();
	ui->cbxCategorias->clear();
	ui->cbxCategoriasTb->clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias ORDER BY orden ASC;");
	if( sql->chequearQuery(query) )
	{
		if( query.first() )
		{
			do {
				cat.id       = query.record().value("id").toString();
				cat.tabla    = query.record().value("tabla").toString();
				cat.titulo   = query.record().value("titulo").toString();
				cat.img      = query.record().value("img").toString();
				cat.orden    = query.record().value("orden").toString();
				cat.emu_show = query.record().value("emu_show").toString();
				categoria.insert(cat.id.toInt(), cat);

				QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
				if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ cat.img) )
					ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img32_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
				else
					ui->cbxCategorias->addItem(QIcon(":/img32_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);

				if( QFile::exists(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img) )
					ui->cbxCategoriasTb->addItem(QIcon(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
				else
					ui->cbxCategoriasTb->addItem(QIcon(":/img"+ ico_size +"_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);

			} while ( query.next() );
		} else {
			cat.tabla    = "dbgrl";
			cat.titulo   = "Db GR-lida";
			cat.img      = "pc.png";
			cat.orden    = "0";
			cat.emu_show = "all";
			cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
			categoria.insert(cat.id.toInt(), cat);

			QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
			if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ cat.img) )
				ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img32_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
			else
				ui->cbxCategorias->addItem(QIcon(":/img32_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);

			if( QFile::exists(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img) )
				ui->cbxCategoriasTb->addItem(QIcon(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
			else
				ui->cbxCategoriasTb->addItem(QIcon(":/img"+ ico_size +"_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
		}
	} else {
		cat.tabla    = "dbgrl";
		cat.titulo   = "Db GR-lida";
		cat.img      = "pc.png";
		cat.orden    = "0";
		cat.emu_show = "all";
		cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
		categoria.insert(cat.id.toInt(), cat);

		QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
		if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ cat.img) )
			ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img32_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img32_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);

		if( QFile::exists(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img) )
			ui->cbxCategoriasTb->addItem(QIcon(fGrl->ThemeApp() +"img"+ ico_size +"_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
		else
			ui->cbxCategoriasTb->addItem(QIcon(":/img"+ ico_size +"_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
	}
	query.clear();

	int id_index = ui->cbxCategoriasTb->findData(id_cat);
	if( id_index < 0 )
	{
		id_index = 0;
		isEmit   = true;
	}

	ui->cbxCategoriasTb->setCurrentIndex(id_index);
	if( isEmit )
		emit on_cbxCategoriasTb_activated(id_index);
	else {
		ui->cbxCategorias->setCurrentIndex(id_index);
		crearMenuNav();
	}
}

void GrLida::setConfigHeaderListaJuegos()
{
	col.clear();
	col.insert(col_IdGrl          , tv_model->fieldIndex("idgrl"           ) );
	col.insert(col_Icono          , tv_model->fieldIndex("icono"           ) );
	col.insert(col_Titulo         , tv_model->fieldIndex("titulo"          ) );
	col.insert(col_Subtitulo      , tv_model->fieldIndex("subtitulo"       ) );
	col.insert(col_Genero         , tv_model->fieldIndex("genero"          ) );
	col.insert(col_Compania       , tv_model->fieldIndex("compania"        ) );
	col.insert(col_Desarrollador  , tv_model->fieldIndex("desarrollador"   ) );
	col.insert(col_Tema           , tv_model->fieldIndex("tema"            ) );
	col.insert(col_Grupo          , tv_model->fieldIndex("grupo"           ) );
	col.insert(col_Perspectiva    , tv_model->fieldIndex("perspectiva"     ) );
	col.insert(col_Idioma         , tv_model->fieldIndex("idioma"          ) );
	col.insert(col_IdiomaVoces    , tv_model->fieldIndex("idioma_voces"    ) );
	col.insert(col_Formato        , tv_model->fieldIndex("formato"         ) );
	col.insert(col_Anno           , tv_model->fieldIndex("anno"            ) );
	col.insert(col_NumDisc        , tv_model->fieldIndex("numdisc"         ) );
	col.insert(col_SistemaOp      , tv_model->fieldIndex("sistemaop"       ) );
	col.insert(col_Tamano         , tv_model->fieldIndex("tamano"          ) );
	col.insert(col_Graficos       , tv_model->fieldIndex("graficos"        ) );
	col.insert(col_Sonido         , tv_model->fieldIndex("sonido"          ) );
	col.insert(col_Jugabilidad    , tv_model->fieldIndex("jugabilidad"     ) );
	col.insert(col_Original       , tv_model->fieldIndex("original"        ) );
	col.insert(col_Estado         , tv_model->fieldIndex("estado"          ) );
	col.insert(col_Thumbs         , tv_model->fieldIndex("thumbs"          ) );
	col.insert(col_Cover_front    , tv_model->fieldIndex("cover_front"     ) );
	col.insert(col_Cover_back     , tv_model->fieldIndex("cover_back"      ) );
	col.insert(col_Fecha          , tv_model->fieldIndex("fecha"           ) );
	col.insert(col_TipoEmu        , tv_model->fieldIndex("tipo_emu"        ) );
	col.insert(col_Comentario     , tv_model->fieldIndex("comentario"      ) );
	col.insert(col_Favorito       , tv_model->fieldIndex("favorito"        ) );
	col.insert(col_Rating         , tv_model->fieldIndex("rating"          ) );
	col.insert(col_Edad           , tv_model->fieldIndex("edad_recomendada") );
	col.insert(col_Usuario        , tv_model->fieldIndex("usuario"         ) );
	col.insert(col_PathExe        , tv_model->fieldIndex("path_exe"        ) );
	col.insert(col_ParametrosExe  , tv_model->fieldIndex("parametros_exe"  ) );
	col.insert(col_PathCapturas   , tv_model->fieldIndex("path_capturas"   ) );
	col.insert(col_PathSetup      , tv_model->fieldIndex("path_setup"      ) );
	col.insert(col_ParametrosSetup, tv_model->fieldIndex("parametros_setup") );
	grl_tv_delegate->setIdCol( col );
	grl_lv_delegate->setIdCol( col );

// Texto en el header del tvJuegos
	tv_model->setHeaderData( col[col_IdGrl]        , Qt::Horizontal, "ID"               , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Icono]        , Qt::Horizontal, ""                 , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Titulo]       , Qt::Horizontal, tr("Título")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Subtitulo]    , Qt::Horizontal, tr("Subtitulo")    , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Genero]       , Qt::Horizontal, tr("Genero")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Compania]     , Qt::Horizontal, tr("Compañia")     , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Desarrollador], Qt::Horizontal, tr("Desarrollador"), Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Tema]         , Qt::Horizontal, tr("Tema")         , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Grupo]        , Qt::Horizontal, tr("Grupo")        , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Perspectiva]  , Qt::Horizontal, tr("Perspectiva")  , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Idioma]       , Qt::Horizontal, tr("Idioma")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_IdiomaVoces]  , Qt::Horizontal, tr("Idioma voces") , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Formato]      , Qt::Horizontal, tr("Formato")      , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Anno]         , Qt::Horizontal, tr("Año")          , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_NumDisc]      , Qt::Horizontal, tr("Nº discos")    , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_SistemaOp]    , Qt::Horizontal, tr("Sistema")      , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Tamano]       , Qt::Horizontal, tr("Tamaño")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Graficos]     , Qt::Horizontal, tr("Gráficos")     , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Sonido]       , Qt::Horizontal, tr("Sonido")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Jugabilidad]  , Qt::Horizontal, tr("Jugabilidad")  , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Original]     , Qt::Horizontal, tr("Original")     , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Estado]       , Qt::Horizontal, tr("Estado")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Fecha]        , Qt::Horizontal, tr("Añadido el")   , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_TipoEmu]      , Qt::Horizontal, tr("Tipo Emu")     , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Favorito]     , Qt::Horizontal, tr("Favorito")     , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Rating]       , Qt::Horizontal, tr("Rating")       , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Edad]         , Qt::Horizontal, tr("Edad")         , Qt::DisplayRole );
	tv_model->setHeaderData( col[col_Usuario]      , Qt::Horizontal, tr("Usuario")      , Qt::DisplayRole );

// Iconos en el header del tvJuegos
	tv_model->setHeaderData( col[col_Icono]        , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/bullet_black.png")  , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Titulo]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/tag.png")           , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Subtitulo]    , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Genero]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Compania]     , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Desarrollador], Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Tema]         , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Grupo]        , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Perspectiva]  , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Idioma]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/idiomas.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_IdiomaVoces]  , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/idiomas.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Formato]      , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/cartucho.png")      , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Anno]         , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/fecha.png")         , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_NumDisc]      , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/cd_iso.png")        , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_SistemaOp]    , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/pc.png")            , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Tamano]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/datos_3.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Graficos]     , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/img_tv2x.png")      , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Sonido]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/img_audio.png")     , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Jugabilidad]  , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/controller.png")    , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Original]     , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/original.png")      , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Estado]       , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/bullet_black.png")  , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Fecha]        , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/fecha.png")         , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_TipoEmu]      , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/dosbox-scummvm.png"), Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Favorito]     , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/"+ grlCfg.IconoFav) , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Rating]       , Qt::Horizontal, QIcon(fGrl->Theme() +"images/star_on.png")      , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Edad]         , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/edad_nd.png")       , Qt::DecorationRole );
	tv_model->setHeaderData( col[col_Usuario]      , Qt::Horizontal, QIcon(fGrl->Theme() +"img16/edad_nd.png")       , Qt::DecorationRole );

	tv_model->setFormatoFecha(grlCfg.FormatoFecha);

	ui->tvJuegos->setColumnWidth( col[col_IdGrl]   , 30 );
	ui->tvJuegos->setColumnWidth( col[col_Icono]   , lwIconCfg.tw_icon_width+8 );
	ui->tvJuegos->setColumnWidth( col[col_Titulo]  , 360);
	ui->tvJuegos->setColumnWidth( col[col_Anno]    , 60 );
	ui->tvJuegos->setColumnWidth( col[col_Original], 52 );
	ui->tvJuegos->setColumnWidth( col[col_Favorito], 38 );
	ui->tvJuegos->setColumnWidth( col[col_Rating]  , 84 );
	ui->tvJuegos->setColumnWidth( col[col_Edad]    , 38 );

	if( !grlCfg.main_twJuegos_state.isEmpty() )
		ui->tvJuegos->header()->restoreState( grlCfg.main_twJuegos_state );

	ui->tvJuegos->setColumnHidden( col[col_IdGrl]          , true );
	ui->tvJuegos->setColumnHidden( col[col_Thumbs]         , true );
	ui->tvJuegos->setColumnHidden( col[col_Cover_front]    , true );
	ui->tvJuegos->setColumnHidden( col[col_Cover_back]     , true );
	ui->tvJuegos->setColumnHidden( col[col_Comentario]     , true );
	ui->tvJuegos->setColumnHidden( col[col_PathExe]        , true );
	ui->tvJuegos->setColumnHidden( col[col_ParametrosExe]  , true );
	ui->tvJuegos->setColumnHidden( col[col_PathCapturas]   , true );
	ui->tvJuegos->setColumnHidden( col[col_PathSetup]      , true );
	ui->tvJuegos->setColumnHidden( col[col_ParametrosSetup], true );

	emit on_mnu_ver_col_icono_triggered        ( grlCfg.ver_col_Icono         );
	emit on_mnu_ver_col_subtitulo_triggered    ( grlCfg.ver_col_Subtitulo     );
	emit on_mnu_ver_col_genero_triggered       ( grlCfg.ver_col_Genero        );
	emit on_mnu_ver_col_compania_triggered     ( grlCfg.ver_col_Compania      );
	emit on_mnu_ver_col_desarrollador_triggered( grlCfg.ver_col_Desarrollador );
	emit on_mnu_ver_col_tema_triggered         ( grlCfg.ver_col_Tema          );
	emit on_mnu_ver_col_grupo_triggered        ( grlCfg.ver_col_Grupo         );
	emit on_mnu_ver_col_perspectiva_triggered  ( grlCfg.ver_col_Perspectiva   );
	emit on_mnu_ver_col_idioma_triggered       ( grlCfg.ver_col_Idioma        );
	emit on_mnu_ver_col_idioma_voces_triggered ( grlCfg.ver_col_IdiomaVoces   );
	emit on_mnu_ver_col_formato_triggered      ( grlCfg.ver_col_Formato       );
	emit on_mnu_ver_col_anno_triggered         ( grlCfg.ver_col_Anno          );
	emit on_mnu_ver_col_numdisc_triggered      ( grlCfg.ver_col_NumDisc       );
	emit on_mnu_ver_col_sistemaop_triggered    ( grlCfg.ver_col_SistemaOp     );
	emit on_mnu_ver_col_tamano_triggered       ( grlCfg.ver_col_Tamano        );
	emit on_mnu_ver_col_graficos_triggered     ( grlCfg.ver_col_Graficos      );
	emit on_mnu_ver_col_sonido_triggered       ( grlCfg.ver_col_Sonido        );
	emit on_mnu_ver_col_jugabilidad_triggered  ( grlCfg.ver_col_Jugabilidad   );
	emit on_mnu_ver_col_original_triggered     ( grlCfg.ver_col_Original      );
	emit on_mnu_ver_col_estado_triggered       ( grlCfg.ver_col_Estado        );
	emit on_mnu_ver_col_fecha_triggered        ( grlCfg.ver_col_Fecha         );
	emit on_mnu_ver_col_tipoemu_triggered      ( grlCfg.ver_col_TipoEmu       );
	emit on_mnu_ver_col_favorito_triggered     ( grlCfg.ver_col_Favorito      );
	emit on_mnu_ver_col_rating_triggered       ( grlCfg.ver_col_Rating        );
	emit on_mnu_ver_col_edad_triggered         ( grlCfg.ver_col_Edad          );
	emit on_mnu_ver_col_usuario_triggered      ( grlCfg.ver_col_Usuario       );
}

// Abre y carga la base de datos.
void GrLida::cargarListaJuegosDB(QString sql_where)
{
	QString img_thumb;
	QModelIndex select;
	total_juegos = sql->getCount(categoria[id_cat].tabla, sql_where);
	row_select = total_juegos > 0 ? row_select : 0;
	col_select = total_juegos > 0 ? col_select : 0;
	int select_row = row_select > 0 ? row_select-1 : 0;

	if( grlCfg.db_orden_by.isEmpty() )
		grlCfg.db_orden_by = "titulo";

	if( grlCfg.db_orden.isEmpty() )
		grlCfg.db_orden = "ASC";

	if( ui->mnu_ver_cover_mode->isChecked() )
	{
		QSqlQuery query(sql->getSqlDB());
		int num_juegos = 0;

		lv_model->clear();
		query.exec("SELECT idgrl, titulo, thumbs, tipo_emu, rating FROM "+ categoria[id_cat].tabla +" "+ sql_where +" ORDER BY "+ grlCfg.db_orden_by +" "+ grlCfg.db_orden);
		if( query.first() )
		{
			do {
				img_thumb = query.record().value("thumbs").toString();
				if( img_thumb.isEmpty() || !QFile::exists(grlDir.Thumbs + img_thumb) )
					img_thumb = fGrl->ThemeApp() +"images/juego_sin_imagen.png";
				else
					img_thumb = grlDir.Thumbs + query.record().value("thumbs").toString();

				QStandardItem *item = new QStandardItem();
				item->setData(query.record().value("idgrl").toString()   , grl_lv_delegate->IdGrlRole   );
				item->setData(query.record().value("titulo").toString()  , grl_lv_delegate->TituloRole  );
				item->setData(QPixmap(img_thumb)                         , grl_lv_delegate->IconRole    );
				item->setData(query.record().value("tipo_emu").toString(), grl_lv_delegate->TipoEmuRole );
				item->setData(query.record().value("rating").toInt()     , grl_lv_delegate->RatingRole  );

				lv_model->setItem(num_juegos, 0, item);

				++num_juegos;
			} while ( query.next() );
		}
		query.clear();

		ui->lvJuegos->setIconSize( QSize(lwIconCfg.icon_width, lwIconCfg.icon_height) );
		ui->lvJuegos->setModel(lv_model);

		select = lv_model->index(select_row, 0);

		if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
			cargarListaPicFlow(sql_where);

		ui->lvJuegos->clearSelection();
		ui->lvJuegos->setCurrentIndex(select);
		ui->lvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);
		ui->lvJuegos->setFocus();

		emit on_lvJuegos_clicked(select);
	} else {
		tv_model->clear();
		tv_model->setTable( categoria[id_cat].tabla );

		QString sin_where = sql_where;
		tv_model->setFilter(sin_where.remove("WHERE", Qt::CaseInsensitive));

		int col_order = tv_model->fieldIndex(grlCfg.db_orden_by);
		if( grlCfg.db_orden == "ASC" )
			tv_model->sort(col_order, Qt::AscendingOrder);
		else
			tv_model->sort(col_order, Qt::DescendingOrder);
		tv_model->select();

		ui->tvJuegos->setIconSize( QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height) );
		ui->tvJuegos->setModel(tv_model);

		setConfigHeaderListaJuegos();

		if( ui->tvJuegos->isColumnHidden(col_select) )
			col_select = 0;

		select = tv_model->index(select_row, col_select);

		if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
			cargarListaPicFlow(sql_where);

		ui->tvJuegos->clearSelection();
		ui->tvJuegos->setCurrentIndex(select);
		ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);
		ui->tvJuegos->setFocus();

		emit on_tvJuegos_clicked(select);
	}

	row_select = total_juegos > 0 ? select_row+1 : 0;
	lb_panel_2->setText(" "+ tr("Nº juegos") +": "+ fGrl->IntToStr(row_select) +" "+ tr("de") +" "+ fGrl->IntToStr(total_juegos) +"  ");
}

void GrLida::cargarListaPicFlow(QString sql_where)
{
	QSqlQuery query(sql->getSqlDB());

	QStringList thumbs_list;
	thumbs_list.clear();
	query.exec("SELECT thumbs FROM "+ categoria[id_cat].tabla +" "+ sql_where +" ORDER BY "+ grlCfg.db_orden_by +" "+ grlCfg.db_orden);
	if( query.first() )
	{
		do {
			thumbs_list << query.record().value("thumbs").toString();
		} while ( query.next() );
	}
	query.clear();

	grl_picflow->setImagePaths(thumbs_list, grlDir.Thumbs, fGrl->ThemeApp());
	grl_picflow->setFocus(Qt::OtherFocusReason);
}

void GrLida::setChangeCategorias(int cat_id)
{
	if( cat_id > -1 )
		id_cat = cat_id;
	else
		id_cat = 0;

	grlCfg.LastSelectCatID = categoria[id_cat].id;

	lwIconCfg = fGrl->cargarListWidgetIconConf( categoria[id_cat].tabla );

	sql_where_select = "";
	grlDir.Capturas  = grlDir.Home +"capturas/"+ categoria[id_cat].tabla +"/";
	grlDir.Confdbx   = grlDir.Home +"confdbx/"+ categoria[id_cat].tabla +"/";
	grlDir.Confvdms  = grlDir.Home +"confvdms/"+ categoria[id_cat].tabla +"/";
	grlDir.Covers    = grlDir.Home +"covers/"+ categoria[id_cat].tabla +"/";
	grlDir.Thumbs    = grlDir.Home +"thumbs/"+ categoria[id_cat].tabla +"/";

	fGrl->comprobarDirectorio( grlDir.Capturas );
	fGrl->comprobarDirectorio( grlDir.Confdbx  );
	fGrl->comprobarDirectorio( grlDir.Confvdms );
	fGrl->comprobarDirectorio( grlDir.Covers   );
	fGrl->comprobarDirectorio( grlDir.Thumbs   );

	grl_tv_delegate->setDirApp( grlDir.Home );
	grl_tv_delegate->setTheme( fGrl->Theme() );
	grl_tv_delegate->setThemeCat( fGrl->ThemeApp() );
	grl_tv_delegate->setTable( categoria[id_cat].tabla );
	grl_tv_delegate->setIconSize( QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height) );
	grl_tv_delegate->setIconFav(grlCfg.IconoFav);

	grl_lv_delegate->setDirApp( grlDir.Home );
	grl_lv_delegate->setTheme( fGrl->Theme() );
	grl_lv_delegate->setThemeCat( fGrl->ThemeApp() );
	grl_lv_delegate->setTable( categoria[id_cat].tabla );
	grl_lv_delegate->setLwIconCfg(lwIconCfg);

	grl_picflow->setSlideSize( QSize(lwIconCfg.pf_img_width,lwIconCfg.pf_img_height) );

	crearMenuNav();
	cargarListaJuegosDB(sql_where_select);
}

void GrLida::menuNavAddCat(QString etiqueta, QString icono, QString sql_query, bool m_expanded, bool m_show_total)
{
	QFont m_font;
	int total = 0;
	if( m_show_total )
		total = sql->getCount(categoria[id_cat].tabla, sql_query);

	m_font.setBold( (total > 0) ? true : false);

	twListMnuNav = new QTreeWidgetItem( ui->twListNav );
	twListMnuNav->setIcon( 0, QIcon(icono) );
	twListMnuNav->setTextAlignment(1, Qt::AlignCenter);
	twListMnuNav->setFont( 1, m_font   );
	twListMnuNav->setText( 0, etiqueta );
	twListMnuNav->setText( 1, m_show_total ? fGrl->IntToStr(total) : "");
	twListMnuNav->setText( 2, sql_query );
	twListMnuNav->setExpanded( m_expanded );
}

void GrLida::menuNavAddSubCat(QString etiqueta, QString icono, QString sql_query, QString sql_col)
{
	QFont m_font;
	int total = 0;
	total = sql->getCount(categoria[id_cat].tabla, sql_query, sql_col, etiqueta);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem( twListMnuNav );

	if( QFile::exists(icono) )
		sub_cat->setIcon( 0, QIcon( icono ) );
	else
		sub_cat->setIcon( 0, QIcon(fGrl->Theme() +"img16/sinimg.png") );

	m_font.setBold( (total > 0) ? true : false);

	sub_cat->setTextAlignment(1, Qt::AlignCenter);
	sub_cat->setFont( 1, m_font               );
	sub_cat->setText( 0, etiqueta             );
	sub_cat->setText( 1, fGrl->IntToStr(total) );
	sub_cat->setText( 2, sql_query            );
}

void GrLida::crearMenuNav()
{
	ui->twListNav->clear();

	menuNavAddCat( tr("Todos")     , fGrl->Theme() +"img16/basedatos.png"    , "", false, true);
	menuNavAddCat( tr("Favoritos") , fGrl->Theme() +"img16/"+ grlCfg.IconoFav, "WHERE favorito='true'", false);
	menuNavAddCat( tr("Originales"), fGrl->Theme() +"img16/original.png"     , "WHERE original='true'", false);

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);

	menuNavAddCat( tr("Tipo emulador"), fGrl->Theme() +"img16/tag.png", "", true, false);
	if( lista.isEmpty() || lista.contains("all") || lista.contains("dosbox") )
		menuNavAddSubCat("DOSBox"  , fGrl->Theme() +"img16/dosbox.png"  , "WHERE tipo_emu='dosbox'"  );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("scummvm") )
		menuNavAddSubCat("ScummVM" , fGrl->Theme() +"img16/scummvm.png" , "WHERE tipo_emu='scummvm'" );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound") )
		menuNavAddSubCat("VDMSound", fGrl->Theme() +"img16/vdmsound.png", "WHERE tipo_emu='vdmsound'");
	if( lista.isEmpty() || lista.contains("all") || lista.contains("datos") )
		menuNavAddSubCat("Datos"   , fGrl->Theme() +"img16/datos_3.png" , "WHERE tipo_emu='datos'"   );

	QList<stGrlDatos> list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	emu_list.clear();
	const int listSize = list_emu.size();
	for (int i = 0; i < listSize; ++i)
	{
		if( lista.isEmpty() || lista.contains("all") || lista.contains(list_emu.at(i).key) )
		{
			emu_list.insert(list_emu.at(i).key, list_emu.at(i));
			menuNavAddSubCat(list_emu.at(i).titulo, fGrl->ThemeApp() +"img16_cat/"+ list_emu.at(i).icono, "WHERE tipo_emu='"+ list_emu.at(i).key +"'");
		}
	}

	sql->cargarMenuNav(ui->twListNav, categoria[id_cat].tabla);

	comprobarEmuloresDisp();
}

void GrLida::nuevoEditarDatosDelJuego(stDatosJuego datos, bool isNew)
{
	if( !datos.idgrl.isEmpty() )
	{
		int row = 0;

		if( isNew )
			row = total_juegos;
		else {
			if( ui->mnu_ver_cover_mode->isChecked() )
				row = ui->lvJuegos->currentIndex().row();
			else
				row = ui->tvJuegos->currentIndex().row();
		}

		QString img_thumb = datos.thumbs;
		if( img_thumb.isEmpty() || !QFile::exists(grlDir.Thumbs + img_thumb) )
			img_thumb = fGrl->ThemeApp() +"images/juego_sin_imagen.png";
		else
			img_thumb = grlDir.Thumbs + datos.thumbs;

		QModelIndex select;
		if( ui->mnu_ver_cover_mode->isChecked() )
		{
			QStandardItem *item = new QStandardItem();
			item->setData( datos.idgrl       , grl_lv_delegate->IdGrlRole   );
			item->setData( datos.titulo      , grl_lv_delegate->TituloRole  );
			item->setData( QPixmap(img_thumb), grl_lv_delegate->IconRole    );
			item->setData( datos.tipo_emu    , grl_lv_delegate->TipoEmuRole );
			item->setData( datos.rating      , grl_lv_delegate->RatingRole  );
			lv_model->setItem(row, 0, item);

			if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
			{
				if( isNew )
					grl_picflow->addImageSlide(datos.thumbs, grlDir.Thumbs, fGrl->ThemeApp());
				else
					grl_picflow->setCenterIndex(row);
//				grl_picflow->setSlide(row, QPixmap(img_thumb));
			}

			select = lv_model->index(row, 0);
			ui->lvJuegos->clearSelection();
			ui->lvJuegos->setCurrentIndex(select);
			ui->lvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);
		} else {
			if( isNew )
				tv_model->insertRow(row);

			tv_model->setData( tv_model->index(row, col[col_IdGrl]          ), datos.idgrl            );
			tv_model->setData( tv_model->index(row, col[col_Icono]          ), datos.icono            );
			tv_model->setData( tv_model->index(row, col[col_Titulo]         ), datos.titulo           );
			tv_model->setData( tv_model->index(row, col[col_Subtitulo]      ), datos.subtitulo        );
			tv_model->setData( tv_model->index(row, col[col_Genero]         ), datos.genero           );
			tv_model->setData( tv_model->index(row, col[col_Compania]       ), datos.compania         );
			tv_model->setData( tv_model->index(row, col[col_Desarrollador]  ), datos.desarrollador    );
			tv_model->setData( tv_model->index(row, col[col_Tema]           ), datos.tema             );
			tv_model->setData( tv_model->index(row, col[col_Grupo]          ), datos.grupo            );
			tv_model->setData( tv_model->index(row, col[col_Perspectiva]    ), datos.perspectiva      );
			tv_model->setData( tv_model->index(row, col[col_Idioma]         ), datos.idioma           );
			tv_model->setData( tv_model->index(row, col[col_IdiomaVoces]    ), datos.idioma_voces     );
			tv_model->setData( tv_model->index(row, col[col_Formato]        ), datos.formato          );
			tv_model->setData( tv_model->index(row, col[col_Anno]           ), datos.anno             );
			tv_model->setData( tv_model->index(row, col[col_NumDisc]        ), datos.numdisc          );
			tv_model->setData( tv_model->index(row, col[col_SistemaOp]      ), datos.sistemaop        );
			tv_model->setData( tv_model->index(row, col[col_Tamano]         ), datos.tamano           );
			tv_model->setData( tv_model->index(row, col[col_Graficos]       ), datos.graficos         );
			tv_model->setData( tv_model->index(row, col[col_Sonido]         ), datos.sonido           );
			tv_model->setData( tv_model->index(row, col[col_Jugabilidad]    ), datos.jugabilidad      );
			tv_model->setData( tv_model->index(row, col[col_Original]       ), datos.original         );
			tv_model->setData( tv_model->index(row, col[col_Estado]         ), datos.estado           );
			tv_model->setData( tv_model->index(row, col[col_Thumbs]         ), datos.thumbs           );
			tv_model->setData( tv_model->index(row, col[col_Cover_front]    ), datos.cover_front      );
			tv_model->setData( tv_model->index(row, col[col_Cover_back]     ), datos.cover_back       );
			tv_model->setData( tv_model->index(row, col[col_Fecha]          ), datos.fecha            );
			tv_model->setData( tv_model->index(row, col[col_TipoEmu]        ), datos.tipo_emu         );
			tv_model->setData( tv_model->index(row, col[col_Comentario]     ), datos.comentario       );
			tv_model->setData( tv_model->index(row, col[col_Favorito]       ), datos.favorito         );
			tv_model->setData( tv_model->index(row, col[col_Rating]         ), datos.rating           );
			tv_model->setData( tv_model->index(row, col[col_Edad]           ), datos.edad_recomendada );
			tv_model->setData( tv_model->index(row, col[col_Usuario]        ), datos.usuario          );
			tv_model->setData( tv_model->index(row, col[col_PathExe]        ), datos.path_exe         );
			tv_model->setData( tv_model->index(row, col[col_ParametrosExe]  ), datos.parametros_exe   );
			tv_model->setData( tv_model->index(row, col[col_PathSetup]      ), datos.path_setup       );
			tv_model->setData( tv_model->index(row, col[col_ParametrosSetup]), datos.parametros_setup );
			tv_model->setData( tv_model->index(row, col[col_PathCapturas]   ), datos.path_capturas    );
			tv_model->submitAll();

			setConfigHeaderListaJuegos();

			if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
			{
				if( isNew )
					grl_picflow->addImageSlide(datos.thumbs, grlDir.Thumbs, fGrl->ThemeApp());
				else
					grl_picflow->setCenterIndex(row);
	//				grl_picflow->setSlide(row, QPixmap(img_thumb));
			}

			select = tv_model->index(row, col[col_Titulo]);
			ui->tvJuegos->clearSelection();
			ui->tvJuegos->setCurrentIndex(select);
			ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);
		}

		if( isNew )
		{
			++total_juegos;
			ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->currentIndex(), categoria[id_cat].titulo +" ("+ fGrl->IntToStr(total_juegos) +")" );
			ui->cbxCategorias->setItemText(ui->cbxCategorias->currentIndex(), categoria[id_cat].titulo +" ("+ fGrl->IntToStr(total_juegos) +")" );
		}

		if( ui->mnu_ver_cover_mode->isChecked() )
			emit on_lvJuegos_clicked(select);
		else
			emit on_tvJuegos_clicked(select);

		crearMenuNav();
	}
}

stDatosJuego GrLida::nuevoItemCopiarImagenes(stDatosJuego datos, QString tabla, bool isNew)
{
	QString old_name_thumbs, old_name_cover_front, old_name_cover_back, img_tmp_name;

	old_name_thumbs      = datos.thumbs;
	old_name_cover_front = datos.cover_front;
	old_name_cover_back  = datos.cover_back;
	img_tmp_name         = "id-"+ datos.idgrl +"_"+ fGrl->eliminar_caracteres(datos.titulo) +"_"+ datos.tipo_emu;

// Actualizamos las imagenes
	if( !old_name_thumbs.isEmpty() )
	{
		stFileInfo fi_t = fGrl->getInfoFile(old_name_thumbs);
		datos.thumbs    = img_tmp_name +"_thumbs"+ fi_t.Ext;
		sql->actualizaDatosItem(tabla, datos.idgrl, "thumbs", datos.thumbs);

		if( isNew )
			QFile::copy(grlDir.Temp + old_name_thumbs, grlDir.Thumbs + datos.thumbs);
		else {
			if( fi_t.Exists )
				QFile::copy(old_name_thumbs, grlDir.Thumbs + datos.thumbs);
			else
				QFile::rename(grlDir.Thumbs + old_name_thumbs, grlDir.Thumbs + datos.thumbs);
		}
	} else
		datos.thumbs = "";

	if( !old_name_cover_front.isEmpty() )
	{
		stFileInfo fi_cf  = fGrl->getInfoFile(old_name_cover_front);
		datos.cover_front = img_tmp_name +"_cover_front"+ fi_cf.Ext;
		sql->actualizaDatosItem(tabla, datos.idgrl, "cover_front", datos.cover_front);

		if( isNew )
			QFile::copy(grlDir.Temp + old_name_cover_front, grlDir.Covers + datos.cover_front);
		else {
			if( fi_cf.Exists )
				QFile::copy(old_name_cover_front, grlDir.Covers + datos.cover_front);
			else
				QFile::rename(grlDir.Covers + old_name_cover_front, grlDir.Covers + datos.cover_front);
		}
	} else
		datos.cover_front = "";

	if( !old_name_cover_back.isEmpty() )
	{
		stFileInfo fi_cb = fGrl->getInfoFile(old_name_cover_back);
		datos.cover_back = img_tmp_name +"_cover_back"+ fi_cb.Ext;
		sql->actualizaDatosItem(tabla, datos.idgrl, "cover_back", datos.cover_back);

		if( isNew )
			QFile::copy(grlDir.Temp + old_name_cover_back, grlDir.Covers + datos.cover_back);
		else {
			if( fi_cb.Exists )
				QFile::copy(grlDir.Covers + old_name_cover_back, grlDir.Covers + datos.cover_back);
			else
				QFile::rename(old_name_cover_back, grlDir.Covers + datos.cover_back);
		}
	} else
		datos.cover_back = "";

	return datos;
}

void GrLida::mostrarDatosDelJuego(QString IDitem)
{
	if( IDitem.isEmpty() )
	{
		IdGame     = "";
		TipoEmu    = "";
		Thumbs     = "";
		CoverFront = "";
		CoverBack  = "";

		ui->btnVer_CoverFront->setEnabled(false);
		ui->btnVer_CoverBack->setEnabled(false);

		ui->mnu_edit_editar_big->setEnabled(false);
		ui->mnu_edit_eliminar_big->setEnabled(false);

		ui->mnu_edit_editar->setEnabled(false);
		ui->mnu_edit_eliminar->setEnabled(false);
		ui->mnu_edit_favorito->setEnabled(false);
		ui->mnu_edit_favorito->setChecked(false);

		ui->twUrls->clear();
		ui->twFiles->clear();

		QString str_html_new = "";
		if( QFile::exists(fGrl->ThemeApp() +"tpl_juego_no_info.html") )
			str_html_new = fGrl->leerArchivo(fGrl->ThemeApp() +"tpl_juego_no_info.html", "UTF-8");
		else
			str_html_new = fGrl->leerArchivo(":/tpl_juego_no_info.html", "UTF-8");

		str_html_new.replace("{lb_juego_no_info}", tr("Información no disponible"));
		ui->txtInfo->clear();
		ui->txtInfo->setHtml( str_html_new );

		lb_panel_2->setText(" "+ tr("Nº juegos") +": "+ fGrl->IntToStr(row_select) +" "+ tr("de") +" "+ fGrl->IntToStr(total_juegos) +"  ");
		lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/sinimg.png") );
		lb_panel_5->setText(" ");
	} else {
		stDatosJuego datos = sql->show_Datos(categoria[id_cat].tabla, IDitem);

		ui->mnu_edit_editar_big->setEnabled(true);
		ui->mnu_edit_eliminar_big->setEnabled(true);
		ui->mnu_edit_editar->setEnabled(true);
		ui->mnu_edit_eliminar->setEnabled(true);
		ui->mnu_edit_favorito->setEnabled(true);

		TipoEmu    = datos.tipo_emu;
		Thumbs     = datos.thumbs;
		CoverFront = datos.cover_front;
		CoverBack  = datos.cover_back;

		lb_panel_2->setText(" "+ tr("Nº juegos") +": "+ fGrl->IntToStr(row_select) +" "+ tr("de") +" "+ fGrl->IntToStr(total_juegos) +"  ");
		lb_panel_5->setText(" "+ datos.titulo +" - "+ tr("introducido el") +" "+ fGrl->HoraFechaActual(datos.fecha, grlCfg.FormatoFecha) +" ");

		if( ui->mnu_ver_datos->isChecked() )
		{
			QString dat_idioma, dat_idioma_voces, dat_img_edad, dat_icono, dat_thumbs, dat_rating;
			QString str_html_new = str_html_old;

			if( !Thumbs.isEmpty() && QFile::exists(grlDir.Thumbs + Thumbs) )
				dat_thumbs = grlDir.Thumbs + Thumbs;
			else {
				Thumbs     = "juego_sin_imagen.png";
				dat_thumbs = fGrl->ThemeApp() +"images/"+ Thumbs;
			}

			if( QFile::exists( fGrl->ThemeApp() +"img24_cat/"+ datos.icono ) )
				dat_icono = fGrl->ThemeApp() +"img24_cat/"+ datos.icono;
			else
				dat_icono = ":/img24_cat/sinimg.png";

			ui->txtInfo->clear();
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("thumb_rs_"+ Thumbs), QImage(dat_thumbs));
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("icono_rs_"+ datos.icono), QImage(dat_icono));
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_on.png"), QImage(fGrl->Theme() +"images/star_on.png"));
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_off.png"), QImage(fGrl->Theme() +"images/star_off.png"));
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("edad_rs_"+ edades_list[datos.edad_recomendada].icono), QImage(fGrl->Theme() +"img16/"+ edades_list[datos.edad_recomendada].icono));

			foreach (const stGrlDatos &smile, smiles_list)
				ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("smile_rs_"+ smile.icono), QImage(grlDir.Smiles + smile.icono));

			foreach (const stGrlDatos &idioma, idiomas_list)
				ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("idioma_rs_"+ idioma.icono), QImage(fGrl->Theme() +"img_lng/"+ idioma.icono));

			dat_idioma       = fGrl->getImgDatos(idiomas_list, datos.idioma.split(";")      , true);
			dat_idioma_voces = fGrl->getImgDatos(idiomas_list, datos.idioma_voces.split(";"), true);
			dat_img_edad     = "<img src=\":edad_rs_"+ edades_list[datos.edad_recomendada].icono +"\" alt=\""+ edades_list[datos.edad_recomendada].titulo +"\" title=\""+ edades_list[datos.edad_recomendada].titulo +"\"> ";

			int total_rating = datos.rating.toInt();
			dat_rating = "";
			for (int i=0; i < 5; ++i)
			{
				if( i < total_rating )
					dat_rating.append("<img src=\":img_rs_star_on.png\" alt=\""+ fGrl->IntToStr(i+1) +"\" title=\""+ fGrl->IntToStr(i+1) +"\"> ");
				else
					dat_rating.append("<img src=\":img_rs_star_off.png\"> ");
			}

			if( TipoEmu == "datos" )
				datos.tipo_emu = tr("Datos");
			else if( TipoEmu == "scummvm" )
				datos.tipo_emu = "ScummVM";
			else if( TipoEmu == "vdmsound" )
				datos.tipo_emu = "VDMSound";
			else if( TipoEmu == "dosbox" )
			{
				QHash<QString, QString> info_dosbox = sql->getDatos("dbgrl_emu_dosbox", "WHERE idgrl="+ IdGame +" AND idcat="+ categoria[id_cat].id, "dosbox_emu_key");
				if( info_dosbox["dosbox_emu_key"].isEmpty() || !dbx_list.contains(info_dosbox["dosbox_emu_key"]) )
					info_dosbox["dosbox_emu_key"] = grlCfg.DOSBoxDefault.isEmpty() ? "dosbox" : grlCfg.DOSBoxDefault;
				datos.tipo_emu = dbx_list[info_dosbox["dosbox_emu_key"]].titulo;
			} else {
				if( emu_list.contains(TipoEmu)  )
					datos.tipo_emu = emu_list[TipoEmu].titulo;
				else
					datos.tipo_emu = tr("Otro emulador");
			}

		// Reempla la info del juego.
			str_html_new.replace("{info_icono}"               , ":icono_rs_"+ datos.icono);
			str_html_new.replace("{info_titulo}"              , Qt::escape(datos.titulo)    );
			str_html_new.replace("{info_subtitulo}"           , Qt::escape(datos.subtitulo) );
			str_html_new.replace("{info_genero}"              , datos.genero        );
			str_html_new.replace("{info_compania}"            , datos.compania      );
			str_html_new.replace("{info_desarrollador}"       , datos.desarrollador );
			str_html_new.replace("{info_tema}"                , datos.tema          );
			str_html_new.replace("{info_grupo}"               , datos.grupo         );
			str_html_new.replace("{info_perspectiva}"         , datos.perspectiva   );
			str_html_new.replace("{info_idioma}"              , dat_idioma          );
			str_html_new.replace("{info_idioma_voces}"        , dat_idioma_voces    );
			str_html_new.replace("{info_formato}"             , datos.formato       );
			str_html_new.replace("{info_anno}"                , datos.anno          );
			str_html_new.replace("{info_numdisc}"             , datos.numdisc       );
			str_html_new.replace("{info_img_edad_recomendada}", dat_img_edad        );
			str_html_new.replace("{info_edad_recomendada}"    , edades_list[datos.edad_recomendada].titulo );
			str_html_new.replace("{info_sistemaop}"           , datos.sistemaop     );
			str_html_new.replace("{info_tamano}"              , datos.tamano        );
			str_html_new.replace("{info_graficos}"            , datos.graficos      );
			str_html_new.replace("{info_sonido}"              , datos.sonido        );
			str_html_new.replace("{info_jugabilidad}"         , datos.jugabilidad   );
			str_html_new.replace("{info_original}"            , datos.original      );
			str_html_new.replace("{info_estado}"              , datos.estado        );
			str_html_new.replace("{info_thumbs}"              , ":thumb_rs_"+ Thumbs);
			str_html_new.replace("{info_fecha}"               , fGrl->HoraFechaActual(datos.fecha, grlCfg.FormatoFecha) );
			str_html_new.replace("{info_tipo_emu}"            , datos.tipo_emu      );
			str_html_new.replace("{info_favorito}"            , datos.favorito == "true" ? tr("Si") : tr("No") );
			str_html_new.replace("{info_rating}"              , dat_rating          );
			str_html_new.replace("{info_usuario}"             , datos.usuario       );
			str_html_new.replace("{info_comentario}", fGrl->reemplazaTextoSmiles(datos.comentario, smiles_list) );

			ui->txtInfo->setHtml( str_html_new );

			if( !CoverFront.isEmpty() && QFile::exists(grlDir.Covers + CoverFront) )
				ui->btnVer_CoverFront->setEnabled(true);
			else
				ui->btnVer_CoverFront->setEnabled(false);

			if( !CoverBack.isEmpty() && QFile::exists(grlDir.Covers + CoverBack) )
				ui->btnVer_CoverBack->setEnabled(true);
			else
				ui->btnVer_CoverBack->setEnabled(false);
		}

		ui->mnu_edit_favorito->setChecked( fGrl->StrToBool( datos.favorito ) );

		if( ui->mnu_ver_archivos_url->isChecked() )
		{
			sql->cargarDatosUrls(ui->twUrls  , IDitem, categoria[id_cat].id);
			sql->cargarDatosFiles(ui->twFiles, IDitem, categoria[id_cat].id);
		}
	}
// Carga la configuración dependiendo el emulador
	cargarConfigEmu( TipoEmu );
}

void GrLida::itemNext()
{
	QModelIndex select;
	int nItem, pos, count;
	nItem = 0;
	count = 0;
	if( ui->mnu_ver_cover_mode->isChecked() )
	{
		pos = ui->lvJuegos->currentIndex().row();
		count = lv_model->rowCount();
		if( count > 0 && pos != -1)
		{
			nItem = pos;
			if( nItem >= count - 1 )
				select = lv_model->index(0, 0);
			else
				select = lv_model->index(nItem + 1, 0);

			ui->lvJuegos->clearSelection();
			ui->lvJuegos->setCurrentIndex(select);
			ui->lvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

			emit on_lvJuegos_clicked(select);
		}
	} else {
		pos = ui->tvJuegos->currentIndex().row();
		count = tv_model->rowCount();
		if( count > 0 && pos != -1 )
		{
			nItem = pos;
			if( nItem == count - 1 )
				select = tv_model->index(0, col_select);
			else
				select = tv_model->index(nItem + 1, col_select);

			ui->tvJuegos->clearSelection();
			ui->tvJuegos->setCurrentIndex(select);
			ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

			emit on_tvJuegos_clicked(select);
		}
	}
}

void GrLida::itemBack()
{
	QModelIndex select;
	int nItem, pos, count;
	nItem = 0;
	count = 0;
	if( ui->mnu_ver_cover_mode->isChecked() )
	{
		pos = ui->lvJuegos->currentIndex().row();
		count = lv_model->rowCount();
		if( count > 0 && pos != -1)
		{
			nItem = pos;
			if( nItem <= 0 )
				select = lv_model->index(count - 1, 0);
			else
				select = lv_model->index(nItem - 1, 0);

			ui->lvJuegos->clearSelection();
			ui->lvJuegos->setCurrentIndex(select);
			ui->lvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

			emit on_lvJuegos_clicked(select);
		}
	} else {
		pos = ui->tvJuegos->currentIndex().row();
		count = tv_model->rowCount();
		if( count > 0 && pos != -1 )
		{
			nItem = pos;
			if( nItem == 0 )
				select = tv_model->index(count - 1, col_select);
			else
				select = tv_model->index(nItem - 1, col_select);

			ui->tvJuegos->clearSelection();
			ui->tvJuegos->setCurrentIndex(select);
			ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

			emit on_tvJuegos_clicked(select);
		}
	}
}

void GrLida::cargarConfigEmu(QString tipo_emu)
{
	if( tipo_emu.isEmpty() )
	{
		cfgExec.f_exe         = "";
		cfgExec.f_param       = "";
		cfgExec.f_exe_setup   = "";
		cfgExec.f_param_setup = "";
		cfgExec.f_path        = "";
		cfgExec.f_path_setup  = "";
		cfgExec.isCfgExec     = false;

		ui->twCapturas->clear();
		ui->twCapturaVideo->clear();
		ui->twCapturaSonido->clear();

		ui->mnu_ejecutar_juego->setEnabled(false);
		ui->mnu_ejecutar_juego_big->setEnabled(false);
		ui->mnu_ejecutar_setup->setEnabled(false);
		ui->mnu_ejecutar_setup_big->setEnabled(false);

		lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/sinimg.png") );
	} else {
		QString Capturas;
		QHash<QString, QString> conf_datos = sql->getDatos(categoria[id_cat].tabla, "WHERE idgrl="+ IdGame, "titulo, path_exe, parametros_exe, path_capturas, path_setup, parametros_setup");

		if( tipo_emu == "datos" )
		{
			lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/datos.png") );

			cfgExec.f_exe         = fGrl->getDirRelative(conf_datos["path_exe"]);
			cfgExec.f_param       = conf_datos["parametros_exe"];
			cfgExec.f_exe_setup   = fGrl->getDirRelative(conf_datos["path_setup"]);
			cfgExec.f_param_setup = conf_datos["parametros_setup"];
			cfgExec.f_path        = fGrl->getInfoFile( cfgExec.f_exe ).Path;
			cfgExec.f_path_setup  = fGrl->getInfoFile( cfgExec.f_exe_setup ).Path;

			Capturas = fGrl->getDirRelative(conf_datos["path_capturas"]);
		}
		else if( tipo_emu == "dosbox" )
		{
			lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/dosbox.png") );

			QHash<QString, QString> conf_dosbox = sql->getDatos("dbgrl_emu_dosbox", "WHERE idgrl="+ IdGame +" AND idcat="+ categoria[id_cat].id,
																"dosbox_emu_key, dosbox_captures, opt_consola_dbox, path_conf, path_exe, path_setup, parametros_setup");

			if( conf_dosbox["dosbox_emu_key"].isEmpty() || !dbx_list.contains(conf_dosbox["dosbox_emu_key"]) )
				conf_dosbox["dosbox_emu_key"] = grlCfg.DOSBoxDefault.isEmpty() ? "dosbox" : grlCfg.DOSBoxDefault;

			cfgExec.f_exe         = fGrl->getDirRelative(dbx_list[conf_dosbox["dosbox_emu_key"]].extra);
			cfgExec.f_param       = "-conf|"+ QDir::toNativeSeparators(grlDir.Confdbx + conf_dosbox["path_conf"]);
			cfgExec.f_exe_setup   = fGrl->getDirRelative(conf_dosbox["path_setup"], "DosGames");
			cfgExec.f_param_setup = conf_dosbox["parametros_setup"];
			cfgExec.f_path        = fGrl->getInfoFile( fGrl->getDirRelative(conf_dosbox["path_exe"], "DosGames") ).Path;
			cfgExec.f_path_setup  = fGrl->getInfoFile( cfgExec.f_exe_setup ).Path;

			#ifdef Q_OS_WIN32
				if( fGrl->StrToBool(conf_dosbox["opt_consola_dbox"]) )
					cfgExec.f_param.append("|-noconsole");
			#endif

			if( conf_dosbox["path_conf"].isEmpty() || !QFile::exists(grlDir.Confdbx + conf_dosbox["path_conf"]) )
				cfgExec.isCfgExec = false;
			else
				cfgExec.isCfgExec = true;

			Capturas = fGrl->getDirRelative(conf_dosbox["dosbox_captures"]);
		}
		else if( tipo_emu == "scummvm" )
		{
			lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/scummvm.png") );

			stConfigScummVM conf_scummvm = sql->showConfg_ScummVM(IdGame, categoria[id_cat].id);
			conf_scummvm.description = conf_datos["titulo"];;

			cfgExec.f_exe         = fGrl->getDirRelative(grlCfg.DirScummVM);
			cfgExec.f_param       = "-c"+ grlDir.Temp +"scummvm.ini|-d"+ conf_scummvm.debuglevel +"|"+ conf_scummvm.game_label;
			cfgExec.f_exe_setup   = "";
			cfgExec.f_param_setup = "";
			cfgExec.f_path        = fGrl->getDirRelative(conf_scummvm.path_game, "DosGames");
			cfgExec.f_path_setup  = "";

			if( cfgExec.f_exe.isEmpty() || cfgExec.f_path.isEmpty() || !QFile::exists( cfgExec.f_path ) )
				cfgExec.isCfgExec = false;
			else {
				cfgExec.isCfgExec = true;
				conf_scummvm.emu_svmpath = fGrl->getInfoFile( cfgExec.f_exe ).Path;
				fGrl->creaIniScummVM(conf_scummvm, grlDir.Temp +"scummvm.ini");
			}

			Capturas = fGrl->getInfoFile( fGrl->getDirRelative(conf_scummvm.path_capturas) ).Path;
			cfgExec.f_path = Capturas;
		}
		else if( tipo_emu == "vdmsound" )
		{
			lb_panel_3->setPixmap( QPixmap(fGrl->Theme() +"img16/vdmsound.png") );

			stConfigVDMSound conf_vdmsound = sql->showConfg_VDMSound(IdGame, categoria[id_cat].id);

			cfgExec.f_exe         = grlDir.Confvdms + conf_vdmsound.path_conf;
			cfgExec.f_param       = "";
			cfgExec.f_exe_setup   = "";
			cfgExec.f_param_setup = "";
			cfgExec.f_path        = fGrl->getInfoFile( fGrl->getDirRelative(conf_vdmsound.path_exe) ).Path;
			cfgExec.f_path_setup  = "";

			Capturas = fGrl->getDirRelative(conf_datos["path_capturas"]);
		} else {
			if( emu_list.contains(tipo_emu)  )
			{
				lb_panel_3->setPixmap( QPixmap(fGrl->ThemeApp() +"img16_cat/"+ emu_list[tipo_emu].icono) );

				cfgExec.f_exe         = fGrl->getDirRelative(emu_list[tipo_emu].extra);
				cfgExec.f_param       = fGrl->getDirRelative(conf_datos["path_exe"]) +"|"+ conf_datos["parametros_exe"];
				cfgExec.f_exe_setup   = fGrl->getDirRelative(conf_datos["path_setup"]);
				cfgExec.f_param_setup = conf_datos["parametros_setup"];
				cfgExec.f_path        = fGrl->getInfoFile( fGrl->getDirRelative(conf_datos["path_exe"]) ).Path;
				cfgExec.f_path_setup  = fGrl->getInfoFile( cfgExec.f_exe_setup ).Path;

				Capturas = fGrl->getDirRelative(conf_datos["path_capturas"]);
			} else
				cargarConfigEmu("");
		}

		if( cfgExec.f_exe.isEmpty() || !QFile::exists(cfgExec.f_exe) )
			cfgExec.isCfgExec = false;
		else
			cfgExec.isCfgExec = true;

		if( cfgExec.f_exe_setup.isEmpty() || !QFile::exists(cfgExec.f_exe_setup) )
			cfgExec.isCfgSetup = false;
		else
			cfgExec.isCfgSetup = true;

		ui->mnu_ejecutar_juego_big->setEnabled(cfgExec.isCfgExec);
		ui->mnu_ejecutar_juego->setEnabled(cfgExec.isCfgExec);

		ui->mnu_ejecutar_setup_big->setEnabled(cfgExec.isCfgSetup);
		ui->mnu_ejecutar_setup->setEnabled(cfgExec.isCfgSetup);

//		comprobarEmuloresDisp();

		if( Capturas.isEmpty() || Capturas == "capture" )
		{
			Capturas = grlDir.Capturas +"id-"+ IdGame +"_"+ fGrl->eliminar_caracteres( conf_datos["titulo"] ) +"_"+ tipo_emu;

			if( tipo_emu == "scummvm" )
				cfgExec.f_path = Capturas;

			fGrl->comprobarDirectorio( Capturas );
		}

		if( ui->mnu_ver_capturas->isChecked() )
			fGrl->cargarArchivosTwLista( ui->twCapturas, Capturas, grlCfg.FormatsImage.join(";") );
		else
			ui->twCapturas->clear();

		if( ui->mnu_ver_archivos_url->isChecked() )
		{
			fGrl->cargarArchivosTwLista(ui->twCapturaVideo , Capturas, grlCfg.FormatsVideo.join(";"), ";", "img_tv2x.png" );
			fGrl->cargarArchivosTwLista(ui->twCapturaSonido, Capturas, grlCfg.FormatsMusic.join(";"), ";", "img_audio.png");
		} else {
			ui->twCapturaVideo->clear();
			ui->twCapturaSonido->clear();
		}
	}
}

void GrLida::comprobarEmuloresDisp()
{
	bool isWin, isEmuDisp, isEnabled;

	#if defined(Q_OS_WIN32)
		isWin = true;
	#else
		isWin = false;
	#endif

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);
// DOSBox
	isEmuDisp = isEnabled = false;
	if( lista.isEmpty() || lista.contains("all") || lista.contains("dosbox") )
		isEmuDisp = true;
	if( isEmuDisp && grlCfg.DOSBoxDisp && QFile::exists(grlCfg.DirDOSBox) )
		isEnabled = true;
	ui->mnu_edit_nuevo_dosbox->setEnabled( isEnabled );
	ui->mnu_edit_nuevo_dosbox_big->setEnabled( isEnabled );

// ScummVM
	isEmuDisp = isEnabled = false;
	if( lista.isEmpty() || lista.contains("all") || lista.contains("scummvm") )
		isEmuDisp = true;
	if( isEmuDisp && grlCfg.ScummVMDisp && QFile::exists(grlCfg.DirScummVM) )
		isEnabled = true;
	ui->mnu_edit_nuevo_scummvm->setEnabled( isEnabled );
	ui->mnu_edit_nuevo_scummvm_big->setEnabled( isEnabled );

// VDMSound
	isEmuDisp = isEnabled = false;
	if( lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound") )
		isEmuDisp = true;
	if( isEmuDisp && grlCfg.VDMSoundDisp && isWin )
		isEnabled = true;
	ui->mnu_edit_nuevo_vdmsound->setEnabled( isEnabled );
	ui->mnu_edit_nuevo_vdmsound_big->setEnabled( isEnabled );
}

void GrLida::centerIndexPicFlow(const QModelIndex &index)
{
	if( index.isValid() )
	{
		if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
		{
			int skip = grlCfg.Skip_PicFlow;
			int distancia = grl_picflow->centerIndex() - index.row();
			if( abs(distancia) > skip )
			{
				if( distancia < 0 )
					grl_picflow->setCenterIndex(grl_picflow->centerIndex()+(-distancia)-skip);
				else
					grl_picflow->setCenterIndex(grl_picflow->centerIndex()-distancia+skip);
				skip_picflow = skip;
			} else
				skip_picflow = abs(grl_picflow->centerIndex()-index.row());
			grl_picflow->showSlide(index.row());
		//	grl_picflow->setFocus(Qt::OtherFocusReason);
		}
	}
}

void GrLida::skipPicFlow(int current)
{
	if( skip_picflow == 0 )
	{
		QModelIndex select;
		if( ui->mnu_ver_cover_mode->isChecked() )
		{
			select = lv_model->index(current, 0);
			ui->lvJuegos->clearSelection();
			ui->lvJuegos->setCurrentIndex(select);
			ui->lvJuegos->scrollTo(select, QAbstractItemView::PositionAtTop);
		} else {
			select = tv_model->index(current, col_select);
			ui->tvJuegos->clearSelection();
			ui->tvJuegos->setCurrentIndex(select);
			ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);
		}
	}
	skip_picflow ? (--skip_picflow) : 0;
}

void GrLida::finishedAnimationPicFlow()
{
	if( isPicFlowActive )
	{
		if( ui->mnu_ver_cover_mode->isChecked() )
			emit on_lvJuegos_clicked(ui->lvJuegos->currentIndex());
		else
			emit on_tvJuegos_clicked(ui->tvJuegos->currentIndex());
	}
}

void GrLida::isActivePicFlow(bool checked)
{
	isPicFlowActive = checked;
}

void GrLida::ejecutar(QString bin, QString parametros, QString working_dir)
{
	if( QFile::exists(bin) )
	{
		grlProcess = new QProcess(this);
		connect(grlProcess, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( fin_Proceso(int, QProcess::ExitStatus) ) );
		connect(grlProcess, SIGNAL( error(QProcess::ProcessError) ), this, SLOT( error_Proceso(QProcess::ProcessError) ) );

		if( fGrl->comprobarDirectorio(working_dir, true) )
		{
			QFileInfo fi( working_dir );
			if( fi.isWritable() )
				grlProcess->setWorkingDirectory( QDir::toNativeSeparators(working_dir) );
		}

		QStringList stl_param;
		stl_param << parametros.split("|", QString::SkipEmptyParts);
		grlProcess->start(QDir::toNativeSeparators(bin), stl_param);

		ui->mnu_ejecutar_juego->setEnabled(false);
		ui->mnu_ejecutar_juego_big->setEnabled(false);
		ui->mnu_ejecutar_setup->setEnabled(false);
		ui->mnu_ejecutar_setup_big->setEnabled(false);

		if( isTrayIcon )
		{
			trayIcon->show();
			this->hide();
		}
	} else
		QMessageBox::information(this, tituloGrl(), tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void GrLida::fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus)
{
	switch ( exitStatus )
	{
		case QProcess::NormalExit:
			//
		break;
		case QProcess::CrashExit:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error al salir del proceso") +"\nError: "+ fGrl->IntToStr(exitCode), QMessageBox::Ok);
		break;
	}

	ui->mnu_ejecutar_juego->setEnabled(true);
	ui->mnu_ejecutar_juego_big->setEnabled(true);
	ui->mnu_ejecutar_setup->setEnabled(true);
	ui->mnu_ejecutar_setup_big->setEnabled(true);

	this->show();

	if( isTrayIcon )
		trayIcon->hide();
}

void GrLida::error_Proceso(QProcess::ProcessError error)
{
	switch( error )
	{
		case QProcess::FailedToStart:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error al iniciar el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::Crashed:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error en el proceso, tiempo después de empezar con éxito"), QMessageBox::Ok);
		break;
		case QProcess::Timedout:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error, waitFor...() última función el tiempo de espera"), QMessageBox::Ok);
		break;
		case QProcess::WriteError:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error al intentar escribir en el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::ReadError:
			QMessageBox::critical(0, tituloGrl(), tr("Se ha producido un error al intentar leer en el proceso"), QMessageBox::Ok);
		break;
		case QProcess::UnknownError:
			QMessageBox::critical(0, tituloGrl(), tr("Ha ocurrido un error desconocido"), QMessageBox::Ok);
		break;
	}

	grlProcess->terminate();
	grlProcess->kill();

	ui->mnu_ejecutar_juego->setEnabled(true);
	ui->mnu_ejecutar_juego_big->setEnabled(true);
	ui->mnu_ejecutar_setup->setEnabled(true);
	ui->mnu_ejecutar_setup_big->setEnabled(true);

	this->show();

	if( isTrayIcon )
		trayIcon->hide();
}

// Comprueba si existen los archivos de datos.
void GrLida::comprobarArchivosDatos(QString version_grl, QString lng)
{
	bool isNuevaVersionGRlida = false;
	if( version_grl != fGrl->versionGrl() && fGrl->version_compare(version_grl, fGrl->versionGrl()) )
		isNuevaVersionGRlida = true;

	Q_UNUSED(lng);
// Copia iconos categorías.
	QList<stGrlDatos> list_icos = fGrl->cargarListaArchivos(":/img24_cat/", CbxListIcon);
	const int list_icosSize = list_icos.size();
	for(int i = 0; i < list_icosSize; ++i)
	{
		if( !QFile::exists(grlDir.Iconos + list_icos.at(i).key) )
			fGrl->copiarArchivo(list_icos.at(i).icono, grlDir.Iconos + list_icos.at(i).key, true);
	}

// Copia los emoticonos.
	QList<stGrlDatos> list_smile = fGrl->cargarListaArchivos(":/smiles/", CbxListIcon);
	const int list_smileSize = list_smile.size();
	for(int i = 0; i < list_smileSize; ++i)
	{
		if( !QFile::exists(grlDir.Smiles + list_smile.at(i).key) )
			fGrl->copiarArchivo(list_smile.at(i).icono, grlDir.Smiles + list_smile.at(i).key, true);
	}

// Copia los script de importar.
	if( !QFile::exists(grlDir.Scripts +"gr-lida.js") || isNuevaVersionGRlida )
		fGrl->copiarArchivo(":/scripts/gr-lida.js"  , grlDir.Scripts +"gr-lida.js", true, true);
	if( !QFile::exists(grlDir.Scripts +"mobygames.js") || isNuevaVersionGRlida )
		fGrl->copiarArchivo(":/scripts/mobygames.js", grlDir.Scripts +"mobygames.js", true, true);

// Comprueba iconos categoría.
	fGrl->comprobarDirectorio(grlDir.Themes +"defecto/");
	fGrl->comprobarDirectorio(grlDir.Themes +"defecto/images/");
	fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img16_cat/");
	fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img24_cat/");
	fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img32_cat/");

	if( !QFile::exists(fGrl->ThemeApp() +"list_cfg.ini") )
		fGrl->guardarListWidgetIconConf( fGrl->cargarListWidgetIconConf() );

	if(	fGrl->Theme() == ":/" )
	{
		if( !QFile::exists(fGrl->ThemeApp() +"StyleSheet.qss") )
			fGrl->copiarArchivo(":/StyleSheet.qss", fGrl->ThemeApp() +"StyleSheet.qss", true);

		if( !QFile::exists(fGrl->ThemeApp() +"StyleSheetList.qss") )
			fGrl->copiarArchivo(":/StyleSheetList.qss", fGrl->ThemeApp() +"StyleSheetList.qss", true);

		if( !QFile::exists(fGrl->ThemeApp() +"images/juego_sin_imagen.png") )
			fGrl->copiarArchivo(":/images/juego_sin_imagen.png", fGrl->ThemeApp() +"images/juego_sin_imagen.png", true);

		if( !QFile::exists(fGrl->ThemeApp() +"images/list_bg.jpg") )
			fGrl->copiarArchivo(":/images/list_bg.jpg", fGrl->ThemeApp() +"images/list_bg.jpg", true);

		if( !QFile::exists(fGrl->ThemeApp() +"images/list_cover_top.png") )
			fGrl->copiarArchivo(":/images/list_cover_top.png", fGrl->ThemeApp() +"images/list_cover_top.png", true);

		if( !QFile::exists(fGrl->ThemeApp() +"images/list_cover_top_select.png") )
			fGrl->copiarArchivo(":/images/list_cover_top_select.png", fGrl->ThemeApp() +"images/list_cover_top_select.png", true);

		if( !QFile::exists(fGrl->ThemeApp() +"images/list_cover_title_bg.png") )
			fGrl->copiarArchivo(":/images/list_cover_title_bg.png", fGrl->ThemeApp() +"images/list_cover_title_bg.png", true);

	// Copia los iconos categoría.
		QList<stGrlDatos> list_img_cat = fGrl->cargarListaArchivos(":/img16_cat/", CbxListIcon);
		for(int i = 0; i < list_img_cat.size(); ++i)
		{
			if( !QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ list_img_cat.at(i).key) )
				fGrl->copiarArchivo(list_img_cat.at(i).icono, fGrl->ThemeApp() +"img16_cat/"+ list_img_cat.at(i).key, true);
		}

		list_img_cat = fGrl->cargarListaArchivos(":/img24_cat/", CbxListIcon);
		for(int i = 0; i < list_img_cat.size(); ++i)
		{
			if( !QFile::exists(fGrl->ThemeApp() +"img24_cat/"+ list_img_cat.at(i).key) )
				fGrl->copiarArchivo(list_img_cat.at(i).icono, fGrl->ThemeApp() +"img24_cat/"+ list_img_cat.at(i).key, true);
		}

		list_img_cat = fGrl->cargarListaArchivos(":/img32_cat/", CbxListIcon);
		for(int i = 0; i < list_img_cat.size(); ++i)
		{
			if( !QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ list_img_cat.at(i).key) )
				fGrl->copiarArchivo(list_img_cat.at(i).icono, fGrl->ThemeApp() +"img32_cat/"+ list_img_cat.at(i).key, true);
		}

		if( !QFile::exists(fGrl->ThemeApp() +"tpl_info.html") )
			fGrl->copiarArchivo(":/tpl_info.html", fGrl->ThemeApp() +"tpl_info.html", true);

		if( !QFile::exists(fGrl->ThemeApp() +"tpl_juego_info.html") )
			fGrl->copiarArchivo(":/tpl_juego_info.html", fGrl->ThemeApp() +"tpl_juego_info.html", true);

		if( !QFile::exists(fGrl->ThemeApp() +"tpl_juego_no_info.html") )
			fGrl->copiarArchivo(":/tpl_juego_no_info.html", fGrl->ThemeApp() +"tpl_juego_no_info.html", true);
	}

	if( isNuevaVersionGRlida )
	{
	// Comprobamos los nuevos directorios
		fGrl->comprobarDirectorio(grlDir.Confdbx  +"dbgrl/");
		fGrl->comprobarDirectorio(grlDir.Confvdms +"dbgrl/");
		fGrl->comprobarDirectorio(grlDir.Covers   +"dbgrl/");
		fGrl->comprobarDirectorio(grlDir.Thumbs   +"dbgrl/");

		copiarArchivos(grlDir.Confvdms, grlDir.Confvdms +"dbgrl/", "*.vlp");
		copiarArchivos(grlDir.Covers  , grlDir.Covers   +"dbgrl/", grlCfg.FormatsImage.join(";"));
		copiarArchivos(grlDir.Datos   , grlDir.Datos    +"old_"  , "*.txt");
		copiarArchivos(grlDir.Thumbs  , grlDir.Thumbs   +"dbgrl/", grlCfg.FormatsImage.join(";"));

		QSqlQuery query(sql->getSqlDB());
		QSqlQuery query_dosbox(sql->getSqlDB());
		query.exec("SELECT idgrl, icono, titulo, thumbs, cover_front, cover_back, fecha, tipo_emu, rating FROM dbgrl ORDER BY idgrl ASC;");
		if( query.first() )
		{
			stDatosJuego dato;
			do {
				dato.idgrl       = query.record().value("idgrl").toString();
				dato.icono       = query.record().value("icono").toString();
				dato.titulo      = query.record().value("titulo").toString();
				dato.thumbs      = query.record().value("thumbs").toString();
				dato.cover_front = query.record().value("cover_front").toString();
				dato.cover_back  = query.record().value("cover_back").toString();
				dato.fecha       = query.record().value("fecha").toString();
				dato.tipo_emu    = query.record().value("tipo_emu").toString();
				dato.rating      = query.record().value("rating").toString();

				sql->actualizaDatosItem("dbgrl", dato.idgrl, "fecha", fGrl->setTime(dato.fecha, grlCfg.FormatoFecha));

				if( dato.icono.isEmpty() )
					sql->actualizaDatosItem("dbgrl", dato.idgrl, "icono", dato.tipo_emu +".png");
				else
					sql->actualizaDatosItem("dbgrl", dato.idgrl, "icono", dato.icono +".png");

				nuevoItemCopiarImagenes(dato, "dbgrl", false);

				if( dato.tipo_emu == "dosbox" )
				{
					query_dosbox.exec("SELECT id, path_conf FROM dbgrl_emu_dosbox WHERE idgrl="+ dato.idgrl +" AND idcat=1 LIMIT 0,1;");
					query_dosbox.first();

					QSettings settings(grlDir.Confdbx + query_dosbox.record().value("path_conf").toString(), QSettings::IniFormat);
					settings.beginGroup("dosbox");
						QString old_dir_capturas = settings.value("captures", "").toString().replace("//", "/");
					settings.endGroup();

					QString new_dir_capturas = grlDir.Capturas +"dbgrl/id-"+ dato.idgrl +"_"+ fGrl->eliminar_caracteres(dato.titulo) +"_"+ dato.tipo_emu;
					fGrl->comprobarDirectorio( new_dir_capturas );
					copiarArchivos(old_dir_capturas +"/", new_dir_capturas +"/", grlCfg.FormatsImage.join(";"));
					fGrl->eliminarDirectorio(old_dir_capturas);
				}
			} while ( query.next() );
		}

		copiarArchivos(grlDir.Confdbx, grlDir.Confdbx +"dbgrl/", "*.conf");
	}
}

void GrLida::copiarArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep)
{
	QFileInfoList list_files = fGrl->getListFiles(dir_old, filter.split(filter_sep, QString::SkipEmptyParts));
	const int listSize = list_files.size();
	for (int i = 0; i < listSize; ++i)
		QFile::rename(dir_old + list_files.at(i).fileName(), dir_new + list_files.at(i).fileName());
	list_files.clear();
}

// Menú archivo
void GrLida::on_mnu_file_informacion_triggered()
{
	frmInfo *InfoNew = new frmInfo(sql, grlCfg, this);
	InfoNew->exec();
	delete InfoNew;
}

void GrLida::on_mnu_file_cerrar_triggered()
{
	this->close();
}

// Menú editar
void GrLida::on_mnu_edit_nuevo_dosbox_triggered()
{
	frmDbxAdd *AddDbxNew = new frmDbxAdd(sql, grlCfg, categoria[id_cat], this);
	AddDbxNew->setWindowFlags(Qt::Window);

	if( AddDbxNew->exec() == QDialog::Accepted )
	{
		grlCfg = AddDbxNew->getGrlCfg();
		nuevoEditarDatosDelJuego(nuevoItemCopiarImagenes(AddDbxNew->getDatosJuegos(), categoria[id_cat].tabla, true), true);
	} else
		grlCfg = AddDbxNew->getGrlCfg();

	delete AddDbxNew;
}

void GrLida::on_mnu_edit_nuevo_scummvm_triggered()
{
	frmSvmAdd *AddSvmNew = new frmSvmAdd(sql, grlCfg, categoria[id_cat], this);
	AddSvmNew->setWindowFlags(Qt::Window);

	if( AddSvmNew->exec() == QDialog::Accepted )
	{
		grlCfg = AddSvmNew->getGrlCfg();
		nuevoEditarDatosDelJuego(nuevoItemCopiarImagenes(AddSvmNew->getDatosJuegos(), categoria[id_cat].tabla, true), true);
	} else
		grlCfg = AddSvmNew->getGrlCfg();

	delete AddSvmNew;
}

void GrLida::on_mnu_edit_nuevo_vdmsound_triggered()
{
	frmVdmsAdd *AddVdmsNew = new frmVdmsAdd(sql, grlCfg, categoria[id_cat], this);
	AddVdmsNew->setWindowFlags(Qt::Window);

	if( AddVdmsNew->exec() == QDialog::Accepted )
	{
		grlCfg = AddVdmsNew->getGrlCfg();
		nuevoEditarDatosDelJuego(nuevoItemCopiarImagenes(AddVdmsNew->getDatosJuegos(), categoria[id_cat].tabla, true), true);
	} else
		grlCfg = AddVdmsNew->getGrlCfg();

	delete AddVdmsNew;
}

void GrLida::on_mnu_edit_nuevo_triggered()
{
	frmAddEditJuego *AddJuego = new frmAddEditJuego(sql, grlCfg, categoria[id_cat], "", "datos", false, this);
	AddJuego->setWindowFlags(Qt::Window);

	if( AddJuego->exec() == QDialog::Accepted )
	{
		grlCfg = AddJuego->getGrlCfg();
		nuevoEditarDatosDelJuego(AddJuego->getDatosJuegos(), true);
	} else
		grlCfg = AddJuego->getGrlCfg();

	delete AddJuego;
}

void GrLida::on_mnu_edit_editar_triggered()
{
	frmAddEditJuego *EditJuego = new frmAddEditJuego(sql, grlCfg, categoria[id_cat], IdGame, TipoEmu, true, this);
	EditJuego->setWindowFlags(Qt::Window);

	if( EditJuego->exec() == QDialog::Accepted )
	{
		grlCfg = EditJuego->getGrlCfg();
		nuevoEditarDatosDelJuego(EditJuego->getDatosJuegos(), false);
	} else
		grlCfg = EditJuego->getGrlCfg();

	delete EditJuego;
}

void GrLida::on_mnu_edit_eliminar_triggered()
{
	int count = 0;
	if( ui->mnu_ver_cover_mode->isChecked() )
		count = lv_model->rowCount();
	else
		count = tv_model->rowCount();

	if( count > 0 )
	{
		if( IdGame.isEmpty() )
			QMessageBox::information( this, tituloGrl(), tr("Por favor selecciona un juego de la lista para eliminarlo") );
		else {
			int respuesta = QMessageBox::question(this, tr("¿Eliminar juego...?"), tr("¿Deseas realmente eliminar este juego de la base de datos?\nSi es de DOSBox o VDMSound también se borra el archivo de configuración"), tr("Si"), tr("No"), 0, 1);
			if( respuesta == 0 )
			{
				if( sql->eliminarDatos(IdGame, categoria[id_cat].id) )
				{
					bool isOk = false;
					QModelIndex select;
					int nItem, pos;
					nItem = 0;
					count = 0;

					if( ui->mnu_ver_cover_mode->isChecked() )
					{
						pos   = ui->lvJuegos->currentIndex().row();
						nItem = pos;
						isOk  = lv_model->removeRow( pos );
					} else {
						pos   = ui->tvJuegos->currentIndex().row();
						nItem = pos;
						isOk  = tv_model->removeRow( pos );
						tv_model->submitAll();
					}

					if( isOk )
					{
						--total_juegos;

						ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->currentIndex(), categoria[id_cat].titulo +" ("+ fGrl->IntToStr(total_juegos) +")" );
						ui->cbxCategorias->setItemText(ui->cbxCategorias->currentIndex(), categoria[id_cat].titulo +" ("+ fGrl->IntToStr(total_juegos) +")" );

						if( ui->mnu_ver_cover_mode->isChecked() )
						{
							if( nItem <= 0 )
							{
								if( total_juegos > 0 )
									select = lv_model->index(0, 0);
							} else
								select = lv_model->index(nItem - 1, 0);

							if( total_juegos > 0 )
							{
								ui->lvJuegos->clearSelection();
								ui->lvJuegos->setCurrentIndex(select);
								ui->lvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

								emit on_lvJuegos_clicked(select);
							}
						} else {
							if( nItem <= 0 )
							{
								if( total_juegos > 0 )
									select = tv_model->index(0, col_select);
							} else
								select = tv_model->index(nItem - 1, col_select);

							if( total_juegos > 0 )
							{
								ui->tvJuegos->clearSelection();
								ui->tvJuegos->setCurrentIndex(select);
								ui->tvJuegos->scrollTo(select, QAbstractItemView::EnsureVisible);

								emit on_tvJuegos_clicked(select);
							}
						}

						if( total_juegos < 1 )
						{
							row_select = 0;
							mostrarDatosDelJuego();
						}

						crearMenuNav();

						if( ui->mnu_ver_pictureflow->isChecked() || grlCfg.PictureFlowToCenter )
						{
							cargarListaPicFlow(sql_where_select);
							centerIndexPicFlow(select);
						}
					}
				}
			}
		}
	} else {
		row_select = 0;
		mostrarDatosDelJuego();
	}
}

void GrLida::on_mnu_edit_favorito_triggered(bool checked)
{
	if( !IdGame.isEmpty() )
	{
		QString favorito = (checked) ? "true" : "false";

		if( !ui->mnu_ver_cover_mode->isChecked() )
			tv_model->setData(tv_model->index(ui->tvJuegos->currentIndex().row(), col[col_Favorito]), favorito);

		sql->actualizaDatosFavorito(categoria[id_cat].tabla, IdGame, favorito);

		QFont m_font;
		int total_fav = sql->getCount(categoria[id_cat].tabla, "WHERE favorito='true'");
		if( total_fav > 0 )
			m_font.setBold(true);
		else
			m_font.setBold(false);
		ui->twListNav->topLevelItem(1)->setFont(1, m_font );
		ui->twListNav->topLevelItem(1)->setText(1, fGrl->IntToStr(total_fav));

		mostrarDatosDelJuego( IdGame );
	}
}

// Menú ejecutar
void GrLida::on_mnu_ejecutar_dosbox_triggered()
{
	ejecutar(grlCfg.DirDOSBox);
}

void GrLida::on_mnu_ejecutar_scummvm_triggered()
{
	ejecutar(grlCfg.DirScummVM);
}

void GrLida::on_mnu_ejecutar_juego_triggered()
{
	if( cfgExec.isCfgExec )
		ejecutar(cfgExec.f_exe, cfgExec.f_param, cfgExec.f_path);
}

void GrLida::on_mnu_ejecutar_setup_triggered()
{
	if( cfgExec.isCfgSetup )
		ejecutar(cfgExec.f_exe, cfgExec.f_exe_setup +"|"+ cfgExec.f_param_setup, cfgExec.f_path_setup);
}

// Menú herramientas
void GrLida::on_mnu_tool_instalar_triggered()
{
	frmInstalarJuego *NewInstalarJuego = new frmInstalarJuego(grlCfg, this);
	NewInstalarJuego->exec();
	delete NewInstalarJuego;
}

void GrLida::on_mnu_tool_importar_triggered()
{
	stDatosJuego     DatosJuego    = fGrl->getDefectDatos();
	stConfigDOSBox   DatosDosBox   = fGrl->getDefectDOSBox();
	stConfigScummVM  DatosScummVM  = fGrl->getDefectScummVM();
	stConfigVDMSound DatosVDMSound = fGrl->getDefectVDMSound();

	frmImportarJuego *ImportarJuego = new frmImportarJuego(sql, grlCfg, categoria[id_cat], DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, this);
	ImportarJuego->setWindowFlags(Qt::Window);

	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		grlCfg     = ImportarJuego->getGrlCfg();
		DatosJuego = ImportarJuego->getDatosJuegos(true);

		DatosJuego.idgrl = sql->insertaDatos(categoria[id_cat].tabla, DatosJuego);

		bool isOk = DatosJuego.idgrl.isEmpty() ? false : true;
		if( isOk )
		{
			sql->insertaFiles(ImportarJuego->ui->twDatosFiles, DatosJuego.idgrl, categoria[id_cat].id );
			sql->insertaURL(ImportarJuego->ui->twDatosURL, DatosJuego.idgrl, categoria[id_cat].id );

			if( DatosJuego.tipo_emu == "dosbox")
			{
				DatosDosBox       = ImportarJuego->getDatosDosBox();
				DatosDosBox.idgrl = DatosJuego.idgrl;
				DatosDosBox.idcat = categoria[id_cat].id;
				DatosDosBox.id    = sql->insertaDbx(DatosDosBox);

				QList<QString> url_list = sql->getDatosUrls(DatosJuego.idgrl, categoria[id_cat].id);
				sql->insertaMontajesDbx(ImportarJuego->ui->twMontajes, DatosDosBox.id);
				fGrl->crearArchivoConfigDbx(DatosJuego, url_list, DatosDosBox, ImportarJuego->ui->twMontajes, grlDir.Home, categoria[id_cat].tabla, grlDir.Confdbx + DatosDosBox.path_conf);
			}

			if( DatosJuego.tipo_emu == "scummvm" )
			{
				DatosScummVM       = ImportarJuego->getDatosScummVM();
				DatosScummVM.idgrl = DatosJuego.idgrl;
				DatosScummVM.idcat = categoria[id_cat].id;

				sql->insertaSvm(DatosScummVM);
			}

			if( DatosJuego.tipo_emu == "vdmsound" )
			{
				DatosVDMSound       = ImportarJuego->getDatosVDMSound();
				DatosVDMSound.idgrl = DatosJuego.idgrl;
				DatosVDMSound.idcat = categoria[id_cat].id;

				sql->insertaVdms(DatosVDMSound);
				fGrl->crearArchivoConfigVdmS(DatosVDMSound, grlDir.Confvdms + DatosVDMSound.path_conf);
			}

			nuevoEditarDatosDelJuego(nuevoItemCopiarImagenes(DatosJuego, categoria[id_cat].tabla, true), true);
		}
	} else
		grlCfg = ImportarJuego->getGrlCfg();

	delete ImportarJuego;
}

void GrLida::on_mnu_tool_exportar_triggered()
{
	frmExportarJuego *NewExportarJuego = new frmExportarJuego(sql, grlCfg, id_cat, this);
	NewExportarJuego->exec();
	grlCfg = NewExportarJuego->getGrlCfg();
	delete NewExportarJuego;
}

void GrLida::on_mnu_tool_edit_themes_triggered()
{
	frmListIconCfg *ListIconCfg  = new frmListIconCfg(sql, grlCfg, id_cat, grlCfg.NameDirTheme, this);
	ListIconCfg->setWindowFlags(Qt::Window);
	if( ListIconCfg->exec() == QDialog::Accepted )
	{
		lwIconCfg = fGrl->cargarListWidgetIconConf( categoria[id_cat].tabla );

		grl_tv_delegate->setDirApp( grlDir.Home );
		grl_tv_delegate->setTheme( fGrl->Theme() );
		grl_tv_delegate->setThemeCat( fGrl->ThemeApp() );
		grl_tv_delegate->setTable( categoria[id_cat].tabla );
		grl_tv_delegate->setIconSize( QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height) );
		grl_tv_delegate->setIconFav(grlCfg.IconoFav);

		grl_lv_delegate->setDirApp( grlDir.Home );
		grl_lv_delegate->setTheme( fGrl->Theme() );
		grl_lv_delegate->setThemeCat( fGrl->ThemeApp() );
		grl_lv_delegate->setTable( categoria[id_cat].tabla );
		grl_lv_delegate->setLwIconCfg(lwIconCfg);

		ui->tvJuegos->setIconSize( QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height) );
		ui->lvJuegos->setIconSize( QSize(lwIconCfg.icon_width, lwIconCfg.icon_height) );
		grl_picflow->setSlideSize( QSize(lwIconCfg.pf_img_width,lwIconCfg.pf_img_height) );

		ui->tvJuegos->update();
		ui->lvJuegos->update();
		setTheme();
		mostrarDatosDelJuego( IdGame );
	}
	delete ListIconCfg ;
}

void GrLida::on_mnu_tool_opciones_triggered()
{
	frmOpciones *Opciones = new frmOpciones(sql, grlCfg, this);
	Opciones->setWindowFlags(Qt::Window);
	if( Opciones->exec() == QDialog::Accepted )
	{
		grlCfg = Opciones->getConfig();

		fGrl->setTheme(grlCfg.NameDirTheme);
		sql->setIdioma(grlCfg.IdiomaSelect);

		if( grlCfg.isChangedEmuList || grlCfg.isChangedIdioma )
			crearMenuNav();

		if( grlCfg.ToolbarBigIcon )
		{
			ui->tb_main->setIconSize(QSize(32,32));
			ui->cbxCategoriasTb->setIconSize(QSize(24,24));
			ui->cbxCategoriasTb->setMinimumSize(QSize(140,30));
		} else {
			ui->tb_main->setIconSize(QSize(16,16));
			ui->cbxCategoriasTb->setIconSize(QSize(16,16));
			ui->cbxCategoriasTb->setMinimumSize(QSize(140,22));
		}

		if( grlCfg.isChangedCategoria )
			cargarListaCategorias(false);

		setTheme();
//		comprobarEmuloresDisp();

		grlCfg.isChangedEmuList   = false;
		grlCfg.isChangedCategoria = false;
	} else {
		grlCfg = Opciones->getConfig();
		sql->setIdioma(grlCfg.IdiomaSelect);
	}

	if( grlCfg.isChangedListDOSBox )
	{
		dbx_list.clear();
		dbx_list = fGrl->cargaDatosQHash(grlDir.Datos +"dbx_list.txt", 6, "|");
		stGrlDatos dbx_dato;
		dbx_dato.titulo  = "DOSBox";
		dbx_dato.icono   = "dosbox.png";
		dbx_dato.extra   = grlCfg.DirDOSBox;
		dbx_dato.issvn   = grlCfg.DOSBoxSVN;
		dbx_dato.version = grlCfg.VersionDBx;
		dbx_dato.key     = "dosbox";
		dbx_list.insert(dbx_dato.key, dbx_dato);
		grlCfg.isChangedListDOSBox = false;
	}

	if( grlCfg.isChangedIdioma )
	{
		QTranslator translator;
		translator.load( Opciones->getIdioma() );
		qApp->installTranslator(&translator);
		ui->retranslateUi(this);
		setWindowTitle( tituloGrl() +" - v"+ fGrl->versionGrl() );
	}

	delete Opciones;
}

// Menú ver
void GrLida::on_mnu_ver_nav_triggered(bool checked)
{
	grlCfg.Pnl_Nav = checked;
	ui->dockw_Nav->setVisible( checked );
}

void GrLida::on_mnu_ver_cover_mode_triggered(bool checked)
{
	grlCfg.ver_IconMode = checked;
	ui->tvJuegos->setVisible( !checked );
	ui->lvJuegos->setVisible( checked );

	cargarListaJuegosDB(sql_where_select);
}

void GrLida::on_mnu_ver_archivos_url_triggered(bool checked)
{
	grlCfg.Pnl_FilesUrl = checked;
	ui->mnu_ver_archivos_url_big->setChecked( checked );
	ui->btnVer_Archivos->setChecked( checked );
	ui->dockw_FilesUrl->setVisible( checked );

	if( checked )
	{
		sql->cargarDatosUrls(ui->twUrls  , IdGame, categoria[id_cat].id);
		sql->cargarDatosFiles(ui->twFiles, IdGame, categoria[id_cat].id);
		cargarConfigEmu( TipoEmu );
	}
}

void GrLida::on_mnu_ver_datos_triggered(bool checked)
{
	grlCfg.Pnl_Datos = checked;
	ui->mnu_ver_datos_big->setChecked( checked );
	ui->dockw_Datos->setVisible( checked );

	mostrarDatosDelJuego(IdGame);
}

void GrLida::on_mnu_ver_pictureflow_triggered(bool checked)
{
	grlCfg.Pnl_PictureFlow = checked;
	ui->dockw_PictureFlow->setVisible( checked );

	if( checked || grlCfg.PictureFlowToCenter)
		cargarListaPicFlow(sql_where_select);
	else
		grl_picflow->clear();
}

void GrLida::on_mnu_ver_pictureflow_to_center_triggered(bool checked)
{
	grlCfg.PictureFlowToCenter = checked;
	if( grlCfg.PictureFlowToCenter )
	{
		ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
		ui->horizontalLayout->setSpacing(0);

		cargarListaPicFlow(sql_where_select);

		ui->mnu_ver_pictureflow->setIcon( QIcon(fGrl->Theme() +"img16/cover_mode.png") );
		ui->mnu_ver_pictureflow->setText(tr("Ver lista de juegos"));
		ui->mnu_ver_pictureflow->setIconText(tr("Ver lista de juegos"));
		ui->mnu_ver_pictureflow->setToolTip(tr("Ver lista de juegos"));
		ui->dockw_PictureFlow->setWindowTitle(tr("Lista de juegos"));

		ui->mnu_ver_pictureflow_to_center->setIcon( QIcon(fGrl->Theme() +"img16/cover_mode.png") );
		ui->mnu_ver_pictureflow_to_center->setText(tr("Lista de juegos como principal"));
		ui->mnu_ver_pictureflow_to_center->setIconText(tr("Lista de juegos como principal"));
		ui->mnu_ver_pictureflow_to_center->setToolTip(tr("Lista de juegos como principal"));

		ui->hLayout_juegos->addWidget(grl_picflow);
		ui->vLayout_picflow->addWidget(ui->tvJuegos);
		ui->vLayout_picflow->addWidget(ui->lvJuegos);
		grl_picflow->setFocus();
	} else {
		if( grlCfg.main_fullscreen )
		{
			ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
			ui->horizontalLayout->setSpacing(0);
		} else {
			ui->horizontalLayout->setContentsMargins(1, 1, 1, 1);
			ui->horizontalLayout->setSpacing(2);
		}

		ui->mnu_ver_pictureflow->setIcon( QIcon(fGrl->Theme() +"img16/pictureflow.png") );
		ui->mnu_ver_pictureflow->setText("Ver PictureFlow");
		ui->mnu_ver_pictureflow->setIconText("Ver PictureFlow");
		ui->mnu_ver_pictureflow->setToolTip("Ver PictureFlow");
		ui->dockw_PictureFlow->setWindowTitle("PictureFlow");

		ui->mnu_ver_pictureflow_to_center->setIcon( QIcon(fGrl->Theme() +"img16/pictureflow.png") );
		ui->mnu_ver_pictureflow_to_center->setText(tr("PictureFlow como principal"));
		ui->mnu_ver_pictureflow_to_center->setIconText(tr("PictureFlow como principal"));
		ui->mnu_ver_pictureflow_to_center->setToolTip(tr("PictureFlow como principal"));

		ui->vLayout_picflow->addWidget(grl_picflow);
		ui->hLayout_juegos->addWidget(ui->tvJuegos);
		ui->hLayout_juegos->addWidget(ui->lvJuegos);

		if( grlCfg.ver_IconMode )
			ui->lvJuegos->setFocus();
		else
			ui->tvJuegos->setFocus();
	}
}

void GrLida::on_mnu_ver_capturas_triggered(bool checked)
{
	grlCfg.Pnl_Capturas = checked;
	ui->mnu_ver_capturas_big->setChecked( checked );
	ui->twCapturas->setVisible( checked );

	if( checked )
		cargarConfigEmu(TipoEmu);
}

void GrLida::on_mnu_ver_ordenar_triggered(bool checked)
{
	grlCfg.Pnl_Ordenar = checked;
	ui->tb_ordenar->setVisible( checked );
}

void GrLida::on_mnu_ver_menubar_triggered(bool checked)
{
	grlCfg.Pnl_Menubar = checked;
	ui->menubar->setVisible( checked );
}

void GrLida::on_mnu_ver_toolbar_triggered(bool checked)
{
	grlCfg.Pnl_Toolbar = checked;
	ui->tb_main->setVisible( checked );
}

void GrLida::on_mnu_ver_statusbar_triggered(bool checked)
{
	grlCfg.Pnl_StatusBar = checked;
	ui->statusBar->setVisible( checked );
}

void GrLida::on_mnu_ver_fullscreen_triggered(bool checked)
{
	grlCfg.main_fullscreen = checked;
	if( grlCfg.main_fullscreen )
	{
		ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
		ui->horizontalLayout->setSpacing(0);

		this->showFullScreen();
	} else {
		if( grlCfg.PictureFlowToCenter )
		{
			ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
			ui->horizontalLayout->setSpacing(0);
		} else {
			ui->horizontalLayout->setContentsMargins(1, 1, 1, 1);
			ui->horizontalLayout->setSpacing(2);
		}

		if( !isInicio )
			this->showNormal();
	}
}

void GrLida::on_mnu_ver_ayuda_triggered()
{
	QDesktopServices::openUrl( QUrl("http://www.gr-lida.org/ayuda/") );
}

void GrLida::on_mnu_ver_check_updates_triggered()
{
	checkUpdateGrl(true);
}

void GrLida::on_mnu_ver_acercad_triggered()
{
	frmAcercaD *AcercaD = new frmAcercaD(grlCfg, this);
	AcercaD->exec();
	delete AcercaD;
}

// Menú ver columnas
void GrLida::on_mnu_ver_col_icono_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Icono], !checked);
	grlCfg.ver_col_Icono = checked;

	if( ui->tvJuegos->columnWidth(col[col_Icono]) < (lwIconCfg.tw_icon_width+8))
		ui->tvJuegos->setColumnWidth(col[col_Icono], lwIconCfg.tw_icon_width+8);
}

void GrLida::on_mnu_ver_col_subtitulo_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Subtitulo], !checked);
	grlCfg.ver_col_Subtitulo = checked;
}

void GrLida::on_mnu_ver_col_genero_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Genero], !checked);
	grlCfg.ver_col_Genero = checked;
}

void GrLida::on_mnu_ver_col_compania_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Compania], !checked);
	grlCfg.ver_col_Compania = checked;
}

void GrLida::on_mnu_ver_col_desarrollador_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Desarrollador], !checked);
	grlCfg.ver_col_Desarrollador = checked;
}

void GrLida::on_mnu_ver_col_tema_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Tema], !checked);
	grlCfg.ver_col_Tema = checked;
}

void GrLida::on_mnu_ver_col_grupo_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Grupo], !checked);
	grlCfg.ver_col_Grupo = checked;
}

void GrLida::on_mnu_ver_col_perspectiva_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Perspectiva], !checked);
	grlCfg.ver_col_Perspectiva = checked;
}

void GrLida::on_mnu_ver_col_idioma_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Idioma], !checked);
	grlCfg.ver_col_Idioma = checked;
}

void GrLida::on_mnu_ver_col_idioma_voces_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_IdiomaVoces], !checked);
	grlCfg.ver_col_IdiomaVoces = checked;
}

void GrLida::on_mnu_ver_col_formato_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Formato], !checked);
	grlCfg.ver_col_Formato = checked;
}

void GrLida::on_mnu_ver_col_anno_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Anno], !checked);
	grlCfg.ver_col_Anno = checked;
}

void GrLida::on_mnu_ver_col_numdisc_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_NumDisc], !checked);
	grlCfg.ver_col_NumDisc = checked;
}

void GrLida::on_mnu_ver_col_sistemaop_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_SistemaOp], !checked);
	grlCfg.ver_col_SistemaOp = checked;
}

void GrLida::on_mnu_ver_col_tamano_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Tamano], !checked);
	grlCfg.ver_col_Tamano = checked;
}

void GrLida::on_mnu_ver_col_graficos_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Graficos], !checked);
	grlCfg.ver_col_Graficos = checked;
}

void GrLida::on_mnu_ver_col_sonido_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Sonido], !checked);
	grlCfg.ver_col_Sonido = checked;
}

void GrLida::on_mnu_ver_col_jugabilidad_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Jugabilidad], !checked);
	grlCfg.ver_col_Jugabilidad = checked;
}

void GrLida::on_mnu_ver_col_original_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Original], !checked);
	grlCfg.ver_col_Original = checked;
}

void GrLida::on_mnu_ver_col_estado_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Estado], !checked);
	grlCfg.ver_col_Estado = checked;
}

void GrLida::on_mnu_ver_col_fecha_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Fecha], !checked);
	grlCfg.ver_col_Fecha = checked;
}

void GrLida::on_mnu_ver_col_tipoemu_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_TipoEmu], !checked);
	grlCfg.ver_col_TipoEmu = checked;
}

void GrLida::on_mnu_ver_col_favorito_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Favorito], !checked);
	grlCfg.ver_col_Favorito = checked;
}

void GrLida::on_mnu_ver_col_rating_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Rating], !checked);
	grlCfg.ver_col_Rating = checked;
}

void GrLida::on_mnu_ver_col_edad_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Edad], !checked);
	grlCfg.ver_col_Edad = checked;
}

void GrLida::on_mnu_ver_col_usuario_triggered(bool checked)
{
	ui->tvJuegos->setColumnHidden(col[col_Usuario], !checked);
	grlCfg.ver_col_Usuario = checked;
}

// Menú ver carpetas
void GrLida::on_mnu_ver_carpeta_confdbx_triggered()
{
	fGrl->abrirDirectorio( grlDir.Confdbx );
}

void GrLida::on_mnu_ver_carpeta_confvdms_triggered()
{
	fGrl->abrirDirectorio( grlDir.Confvdms );
}

void GrLida::on_mnu_ver_carpeta_thumbs_triggered()
{
	fGrl->abrirDirectorio( grlDir.Thumbs );
}

void GrLida::on_mnu_ver_carpeta_covers_triggered()
{
	fGrl->abrirDirectorio( grlDir.Covers );
}

void GrLida::on_mnu_ver_carpeta_iconos_triggered()
{
	fGrl->abrirDirectorio( grlDir.Iconos );
}

void GrLida::on_mnu_ver_carpeta_smiles_triggered()
{
	fGrl->abrirDirectorio( grlDir.Smiles );
}

void GrLida::on_mnu_ver_carpeta_themes_triggered()
{
	fGrl->abrirDirectorio( grlDir.Themes );
}

void GrLida::on_mnu_ver_carpeta_datos_triggered()
{
	fGrl->abrirDirectorio( grlDir.Datos );
}

void GrLida::on_mnu_ver_carpeta_scripts_triggered()
{
	fGrl->abrirDirectorio( grlDir.Scripts );
}

void GrLida::on_mnu_ver_carpeta_templates_triggered()
{
	fGrl->abrirDirectorio( grlDir.Templates );
}

void GrLida::on_mnu_ver_carpeta_idiomas_triggered()
{
	fGrl->abrirDirectorio( grlDir.Idiomas );
}

void GrLida::on_mnu_ver_carpeta_temp_triggered()
{
	fGrl->abrirDirectorio( grlDir.Temp );
}

void GrLida::on_mnu_ver_carpeta_home_triggered()
{
	fGrl->abrirDirectorio( grlDir.Home );
}

// Barra de heramientas
void GrLida::on_mnu_file_cerrar_big_triggered()
{
	emit on_mnu_file_cerrar_triggered();
}

void GrLida::on_mnu_edit_nuevo_dosbox_big_triggered()
{
	emit on_mnu_edit_nuevo_dosbox_triggered();
}

void GrLida::on_mnu_edit_nuevo_scummvm_big_triggered()
{
	emit on_mnu_edit_nuevo_scummvm_triggered();
}

void GrLida::on_mnu_edit_nuevo_vdmsound_big_triggered()
{
	emit on_mnu_edit_nuevo_vdmsound_triggered();
}

void GrLida::on_mnu_ejecutar_juego_big_triggered()
{
	emit on_mnu_ejecutar_juego_triggered();
}

void GrLida::on_mnu_ejecutar_setup_big_triggered()
{
	emit on_mnu_ejecutar_setup_triggered();
}

void GrLida::on_mnu_edit_nuevo_big_triggered()
{
	emit on_mnu_edit_nuevo_triggered();
}

void GrLida::on_mnu_edit_editar_big_triggered()
{
	emit on_mnu_edit_editar_triggered();
}

void GrLida::on_mnu_edit_eliminar_big_triggered()
{
	emit on_mnu_edit_eliminar_triggered();
}

void GrLida::on_mnu_tool_importar_big_triggered()
{
	emit on_mnu_tool_importar_triggered();
}

void GrLida::on_mnu_ver_archivos_url_big_triggered(bool checked)
{
	ui->mnu_ver_archivos_url->setChecked( checked );
	ui->btnVer_Archivos->setChecked( checked );
	emit on_mnu_ver_archivos_url_triggered( checked );
}

void GrLida::on_mnu_ver_datos_big_triggered(bool checked)
{
	ui->mnu_ver_datos->setChecked( checked );
	emit on_mnu_ver_datos_triggered( checked );
}

void GrLida::on_mnu_ver_capturas_big_triggered(bool checked)
{
	ui->mnu_ver_capturas->setChecked( checked );
	emit on_mnu_ver_capturas_triggered( checked );
}

void GrLida::on_mnu_tool_opciones_big_triggered()
{
	emit on_mnu_tool_opciones_triggered();
}

void GrLida::on_mnu_ver_acercad_big_triggered()
{
	emit on_mnu_ver_acercad_triggered();
}

void GrLida::on_txtBuscar_textChanged(const QString &arg1)
{
	if( arg1.length() == 0 )
		cargarListaJuegosDB("");
	else if(arg1.length() > 2)
		cargarListaJuegosDB("WHERE titulo LIKE '%"+ arg1 +"%'");
}

void GrLida::on_cbxOrdenBy_activated(int index)
{
	if( index > -1 )
		grlCfg.db_orden_by = ui->cbxOrdenBy->itemData(index).toString();
}

void GrLida::on_cbxOrden_activated(int index)
{
	if( index > -1 )
		grlCfg.db_orden = ui->cbxOrden->itemData(index).toString();
}

void GrLida::on_btnOrdenar_clicked()
{
	if( ui->txtBuscar->text().length() == 0 )
		cargarListaJuegosDB("");
	else
		cargarListaJuegosDB("WHERE titulo LIKE '%"+ ui->txtBuscar->text() +"%'");
}

void GrLida::on_cbxCategoriasTb_activated(int index)
{
	if( index > -1 )
	{
		int cat_id = ui->cbxCategoriasTb->itemData(index, Qt::UserRole).toInt();
		ui->cbxCategorias->setCurrentIndex( index );
		setChangeCategorias(cat_id);
	}
}

void GrLida::on_cbxCategorias_activated(int index)
{
	if( index > -1 )
	{
		int cat_id = ui->cbxCategorias->itemData(index, Qt::UserRole).toInt();
		ui->cbxCategoriasTb->setCurrentIndex( index );
		setChangeCategorias(cat_id);
	}
}

void GrLida::on_twListNav_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		if( !grlCfg.ver_IconMode )
			grlCfg.main_twJuegos_state = ui->tvJuegos->header()->saveState();

		sql_where_select = item->text(2);
		row_select = 0;
		cargarListaJuegosDB(sql_where_select);
	}
}

void GrLida::on_twListNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twListNav_itemClicked(current, 0);
}

void GrLida::on_lvJuegos_clicked(const QModelIndex &index)
{
	if( index.isValid() )
	{
		IdGame = index.data(GrlListViewDelegate::IdGrlRole).toString();
		row_select = index.row()+1;
		isPicFlowActive = false;

		emit centerIndexPicFlow(index);

		mostrarDatosDelJuego( IdGame );
	} else
		mostrarDatosDelJuego();
}


void GrLida::on_lvJuegos_doubleClicked(const QModelIndex &index)
{
	if( index.isValid() )
		emit on_mnu_ejecutar_juego_triggered();
}

void GrLida::on_lvJuegos_customContextMenuRequested(const QPoint &pos)
{
	showPopup(ui->lvJuegos, pos);
}

void GrLida::grl_picflow_customContextMenuRequested(const QPoint &pos)
{
	if( grlCfg.PictureFlowToCenter )
		showPopup(ui->centralwidget, pos);
	else
		showPopup(ui->dockw_PictureFlow, pos);
}

void GrLida::on_tvJuegos_clicked(const QModelIndex &index)
{
	if( index.isValid() )
	{
		IdGame = index.sibling(index.row(), col[col_IdGrl]).data(Qt::DisplayRole).toString();
		row_select = index.row()+1;
		col_select = total_juegos > 0 ? index.column() : 0;
		isPicFlowActive = false;

		emit centerIndexPicFlow(index);

		mostrarDatosDelJuego( IdGame );
	} else
		mostrarDatosDelJuego();
}

void GrLida::on_tvJuegos_doubleClicked(const QModelIndex &index)
{
	if( index.isValid() )
		emit on_mnu_ejecutar_juego_triggered();
}

void GrLida::on_tvJuegos_customContextMenuRequested(const QPoint &pos)
{
	showPopup(ui->tvJuegos, pos);
}

void GrLida::on_twCapturas_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		GrDap *grdap = new GrDap(this);
		QStringList lista;
		const int listSize = ui->twCapturas->topLevelItemCount();
		for (int i = 0; i < listSize; ++i)
		{
			lista << ui->twCapturas->topLevelItem( i )->text(1);
		}
		int id_imagen = lista.indexOf( item->text(1) );
		grdap->cargarImagenes(lista, (id_imagen != -1 ? id_imagen : 0), true);
		grdap->show();
	}
}

void GrLida::on_twCapturaVideo_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		frmMultiMedia *m_Media = new frmMultiMedia(this);
		m_Media->setWindowFlags(Qt::Window);
		QStringList lista;
		int id = -1;
		const int listSize = ui->twCapturaVideo->topLevelItemCount();
		for (int i = 0; i < listSize; ++i)
		{
			lista << ui->twCapturaVideo->topLevelItem( i )->text(1);
			if( item->text(1) == ui->twCapturaVideo->topLevelItem( i )->text(1) )
				id = i;
		}
		m_Media->open(id, lista);//
		m_Media->show();
	}
}

void GrLida::on_twCapturaSonido_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		frmMultiMedia *m_Media = new frmMultiMedia(this);
		m_Media->setWindowFlags(Qt::Window);
		QStringList lista;
		int id = -1;
		const int listSize = ui->twCapturaSonido->topLevelItemCount();
		for (int i = 0; i < listSize; ++i)
		{
			lista << ui->twCapturaSonido->topLevelItem( i )->text(1);
			if( item->text(1) == ui->twCapturaSonido->topLevelItem( i )->text(1) )
				id = i;
		}
		m_Media->open(id, lista);//
		m_Media->show();
	}
}

void GrLida::on_twUrls_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
		QDesktopServices::openUrl( QUrl( item->text(0) ) );
}

void GrLida::on_twFiles_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		stFileInfo archivo = fGrl->getInfoFile( fGrl->getDirRelative(item->text(4)) );
		if( item->text(5) == "ruleta" || item->text(5) == "manual" )
		{
			if( archivo.Ext == ".cbz" || archivo.Ext == ".zip" ||
				archivo.Ext == ".dap" || archivo.Ext == ".dapz" ||
				archivo.Ext == ".conf" )
			{
				GrDap *grdap = new GrDap(this);
				grdap->cargarArchivo( archivo.FilePath );
				grdap->show();
			} else
				fGrl->abrirArchivo( archivo.FilePath );
		}
		else if( item->text(5) == "pdf" )
		{
			if( grlCfg.OpenPdfExternal || archivo.Ext != ".pdf")
				fGrl->abrirArchivo( archivo.FilePath );
			else {
				frmPdfViewer *PdfViewer = new frmPdfViewer(this);
				PdfViewer->setWindowFlags(Qt::Window);
				PdfViewer->openPdf( archivo.FilePath );
				PdfViewer->show();
			}
		} else
			fGrl->abrirArchivo( archivo.FilePath );
	}
}

void GrLida::on_btnVer_Archivos_clicked(bool checked)
{
	ui->mnu_ver_archivos_url->setChecked( checked );
	ui->mnu_ver_archivos_url_big->setChecked( checked );
	emit on_mnu_ver_archivos_url_triggered( checked );
}

void GrLida::on_btnVer_CoverFront_clicked()
{
	if( !CoverFront.isEmpty() )
	{
		GrDap *grdap = new GrDap(this);
		grdap->cargarImagen( grlDir.Covers + CoverFront );
		grdap->show();
	}
}

void GrLida::on_btnVer_CoverBack_clicked()
{
	if( !CoverBack.isEmpty() )
	{
		GrDap *grdap = new GrDap(this);
		grdap->cargarImagen( grlDir.Covers + CoverBack );
		grdap->show();
	}
}

void GrLida::on_dockw_Nav_visibilityChanged(bool visible)
{
//	if( ui->dockw_Nav->isHidden() )
//	{
		grlCfg.Pnl_Nav = visible;
		ui->mnu_ver_nav->setChecked(visible);
//	}
}

void GrLida::on_dockw_Datos_visibilityChanged(bool visible)
{
//	if( ui->dockw_Datos->isHidden() )
//	{
		grlCfg.Pnl_Datos = visible;
		ui->mnu_ver_datos->setChecked( visible );
		ui->mnu_ver_datos_big->setChecked( visible );

//	}
}

void GrLida::on_dockw_FilesUrl_visibilityChanged(bool visible)
{
//	if( ui->dockw_FilesUrl->isHidden() )
//	{
		grlCfg.Pnl_FilesUrl = visible;
		ui->mnu_ver_archivos_url->setChecked( visible );
		ui->mnu_ver_archivos_url_big->setChecked( visible );
		ui->btnVer_Archivos->setChecked( visible );
//	}
}

void GrLida::on_dockw_PictureFlow_visibilityChanged(bool visible)
{
//	if( ui->dockw_PictureFlow->isHidden() )
		grlCfg.Pnl_PictureFlow = visible;
		ui->mnu_ver_pictureflow->setChecked( visible );
}
