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
#include "stardelegate.h"

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

	stHomeDir      = fGrl.GRlidaHomePath();
	stIconDir      = stHomeDir + "iconos/";
	stDatosDir     = stHomeDir + "datos/";
	stConfgDbxDir  = stHomeDir + "confdbx/";
#ifdef Q_OS_WIN32
	stConfgVdmSDir = stHomeDir + "confvdms/";
#endif
	stTheme = fGrl.ThemeGrl();

	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles(stDatosDir + "smiles.txt");

	createConnections();
	createToolBars();
	createStatusBar();

	CargarConfig();

	setTheme();

	ui.toolBar->setContextMenuPolicy( Qt::PreventContextMenu );
/*
	ui.twNav->setMinimumWidth( 186 );
	ui.twNav->headerItem()->setText(0,"");
	ui.twNav->header()->setStretchLastSection(true);
	ui.twNav->header()->setMovable(false);
	ui.twNav->header()->setResizeMode(0, QHeaderView::Stretch);
*/
	ui.twJuegos->headerItem()->setText(0,"");
	ui.twJuegos->header()->setStretchLastSection(false);
	ui.twJuegos->header()->setMovable(false);
	ui.twJuegos->header()->setResizeMode(0, QHeaderView::Fixed  );
	ui.twJuegos->header()->setResizeMode(1, QHeaderView::Stretch);
	ui.twJuegos->header()->setResizeMode(2, QHeaderView::Fixed  );
	ui.twJuegos->setColumnWidth(0,31);
	ui.twJuegos->setColumnWidth(2,90);
	ui.twJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.twJuegos->installEventFilter(this);
	ui.twJuegos->setItemDelegate( new StarDelegate( ui.twJuegos ) );

	ui.txtInfo_Comentario->installEventFilter(this);

	ui.twFiles->header()->setStretchLastSection(true);
	ui.twFiles->header()->setMovable(false);
	ui.twFiles->header()->setResizeMode(QHeaderView::Interactive);
	ui.twFiles->setColumnWidth(0, 100 );	// nombre
	ui.twFiles->setColumnWidth(1, 70  );	// crc32
	ui.twFiles->setColumnWidth(2, 140 );	// descripcion
	ui.twFiles->setColumnWidth(3, 70  );	// size

	ui.twUrls->header()->setStretchLastSection(true);
	ui.twUrls->header()->setMovable(false);
	ui.twUrls->header()->setResizeMode(QHeaderView::Interactive);
	ui.twUrls->setColumnWidth(0, 100 );

	ui.twCapturas->header()->setStretchLastSection(true);
	ui.twCapturas->header()->setMovable(false);
	ui.twCapturas->header()->setResizeMode(QHeaderView::Stretch);

//	Crea el icono para la bandeja de iconos
	isTrayIcon = createTrayIcon();

// Centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

GrLida::~GrLida()
{
	//
}

void GrLida::closeEvent(QCloseEvent *e)
{
	if( isTrayIcon && trayIcon->isVisible() )
	{
		this->hide();
		e->ignore();
	} else {
		sql->dbClose();
		GuardarConfig();
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

void GrLida::ConectarBaseDatos()
{
// Conecta con la Base de Datos
	sql = new dbSql( stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port );

	if( !sql->dbisOpen() )
		QMessageBox::critical(0, tr("Error abrir la Base de Datos"), tr("No se ha podido establecer una conexión con la Base de Datos.\nEsta aplicación necesita soporte de SQLite. Mira la documentación de Qt SQL driver para más información.\n\nClick Cancelar para salir."), QMessageBox::Cancel);
	else
		sql->CrearTablas();
}

void GrLida::CargarListaJuegosDB(QString strBuscar)
{
	QString stIcono, stSqlWhere;
	QSqlQuery query;
	int n, num_juegos;
	QStringList str_ListaDatos;

	num_juegos = sql->getCount("dbgrl");

	ui.twJuegos->setColumnCount( ui.twJuegos->columnCount() );
	ui.twJuegos->clear();

	QImage img;
	ui.PicFlowWidget->clear();
	ui.PicFlowWidget->setSlideSize(QSize(145, 186));
	id_ImgPicFlow = 0;
	id_ImgPicFlow_old = 0;

	if(stdb_Orden_ColTabla == "titulo" || stdb_Orden_ColTabla == "subtitulo" || stdb_Orden_ColTabla == "idgrl")
	{
		if(!strBuscar.isEmpty() || strBuscar!="")
			stSqlWhere = " WHERE titulo LIKE '%"+strBuscar+"%'";
		else stSqlWhere = "";
		query.exec("SELECT * FROM dbgrl "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+stdb_Orden);
		if( query.first())
		{
			do {
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
				item->setText( 0 , query.record().value("idgrl").toString()    );	// idgrl
				item->setText( 1 , query.record().value("titulo").toString()   );	// titulo
				item->setText( 2 , query.record().value("rating").toString()   );	// rating
				item->setText( 3 , query.record().value("tipo_emu").toString() );	// tipo_emu
				item->setText( 4 , query.record().value("favorito").toString() );	// favorito
				item->setText( 5 , fGrl.IntToStr( id_ImgPicFlow ) );

				if( img.load( query.record().value("thumbs").toString() ) )
				{
					ui.PicFlowWidget->addSlide( img );
				} else {
					ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );
				}

				id_ImgPicFlow++;

				stIcono = fGrl.getIconListaJuegos(query.record().value("icono").toString(), stIconDir);
				item->setIcon( 0, QIcon(stIcono) );

				if(query.record().value("favorito").toString()=="true")
					item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito

			} while ( query.next() );

			ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
			on_twJuegos_clicked( ui.twJuegos->currentItem() );

		} else {
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
			lbpanel_5->setText(" ");
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

		if(strBuscar!="")
			stSqlWhere = " AND titulo LIKE '%"+strBuscar+"%'";
		else stSqlWhere = "";

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
						item->setText( 2 , "" );	// rating
						item->setText( 3 , "" );	// tipo_emu
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

						item->setText( 2 , "" ); // rating
						item->setText( 3 , "" ); // tipo_emu
					}

					do {
						QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
						item->setText( 0 , fGrl.IntToStr( query.record().value("idgrl").toInt() ) );	// idgrl
						item->setText( 1 , query.record().value("titulo").toString()              );	// titulo
						item->setText( 2 , query.record().value("rating").toString()              );	// rating
						item->setText( 3 , query.record().value("tipo_emu").toString()            );	// tipo_emu
						item->setText( 4 , query.record().value("favorito").toString()            );	// favorito
						item->setText( 5 , fGrl.IntToStr( id_ImgPicFlow ) );

						if( img.load( query.record().value("thumbs").toString() ) )
						{
							ui.PicFlowWidget->addSlide( img );
						} else {
							ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );
						}

						id_ImgPicFlow++;

						stIcono = fGrl.getIconListaJuegos(query.record().value("icono").toString(), stIconDir);
						item->setIcon( 0, QIcon(stIcono) );

						if(query.record().value("favorito").toString()=="true")
							item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito

					} while (query.next());
					ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
					on_twJuegos_clicked( ui.twJuegos->currentItem() );
				} else {
					lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
					lbpanel_5->setText(" ");
				}
			} // fin de for lista datos
		}
	}

	//ui.PicFlowWidget->resize( ui.PicFlowWidget->width(), ui.PicFlowWidget->height() );
	//ui.PicFlowWidget->setCenterIndex( ui.PicFlowWidget->slideCount()/2 );
	ui.PicFlowWidget->showSlide( 0 );
	ui.PicFlowWidget->update();

	if( num_juegos > 0)
		lbpanel_2->setText(" "+ tr("Nº Juegos") + ": " + fGrl.IntToStr(num_juegos)+ "  " );
	else
		lbpanel_2->setText(" "+ tr("Nº Juegos") + ": 0 ");
}

// Conecta los distintos botones con las funciones de cada uno
void GrLida::createConnections()
{

	connect(ui.mnu_ver_datos       , SIGNAL( toggled(bool) ), ui.actionVerDatos      , SLOT( setChecked(bool) ));
	connect(ui.mnu_ver_archivos_url, SIGNAL( toggled(bool) ), ui.actionVerFilesUrl   , SLOT( setChecked(bool) ));
	connect(ui.mnu_ver_capturas    , SIGNAL( toggled(bool) ), ui.actionVerCapturas   , SLOT( setChecked(bool) ));
	connect(ui.mnu_ver_datos       , SIGNAL( toggled(bool) ), ui.dockw_Datos         , SLOT( setVisible(bool) ));
	connect(ui.mnu_ver_archivos_url, SIGNAL( toggled(bool) ), ui.dockw_FilesUrl      , SLOT( setVisible(bool) ));
	connect(ui.mnu_ver_capturas    , SIGNAL( toggled(bool) ), ui.dockw_Capturas      , SLOT( setVisible(bool) ));
	connect(ui.actionVerDatos      , SIGNAL( toggled(bool) ), ui.mnu_ver_datos       , SLOT( setChecked(bool) ));
	connect(ui.actionVerFilesUrl   , SIGNAL( toggled(bool) ), ui.mnu_ver_archivos_url, SLOT( setChecked(bool) ));
	connect(ui.actionVerCapturas   , SIGNAL( toggled(bool) ), ui.mnu_ver_capturas    , SLOT( setChecked(bool) ));
	connect(ui.actionVerDatos      , SIGNAL( toggled(bool) ), ui.dockw_Datos         , SLOT( setVisible(bool) ));
	connect(ui.actionVerFilesUrl   , SIGNAL( toggled(bool) ), ui.dockw_FilesUrl      , SLOT( setVisible(bool) ));
	connect(ui.actionVerCapturas   , SIGNAL( toggled(bool) ), ui.dockw_Capturas      , SLOT( setVisible(bool) ));
/*
	connect(ui.dockw_Datos->toggleViewAction()   , SIGNAL( toggled(bool) ), ui.mnu_ver_datos       , SLOT( setChecked(bool) ));
	connect(ui.dockw_FilesUrl->toggleViewAction(), SIGNAL( toggled(bool) ), ui.mnu_ver_archivos_url, SLOT( setChecked(bool) ));
	connect(ui.dockw_Capturas->toggleViewAction(), SIGNAL( toggled(bool) ), ui.mnu_ver_capturas    , SLOT( setChecked(bool) ));
*/
	connect(ui.toolBar->toggleViewAction(), SIGNAL( toggled(bool) ), ui.mnu_ver_toolbar, SLOT( setChecked(bool) ));
	connect(ui.mnu_ver_toolbar, SIGNAL( toggled(bool) ), ui.toolBar, SLOT(  setVisible(bool) ));

	connect(ui.toolBar_ordenar->toggleViewAction(), SIGNAL( toggled(bool) ), ui.mnu_ver_ordenar, SLOT( setChecked(bool) ));
	connect(ui.mnu_ver_ordenar, SIGNAL( toggled(bool) ), ui.toolBar_ordenar, SLOT(  setVisible(bool) ));

	connect(ui.mnu_ver_pictureflow, SIGNAL( toggled(bool) ), ui.PicFlowWidget, SLOT(  setVisible(bool) ));

// Barra de Heramientas
	connect( ui.actionClose        , SIGNAL( triggered(bool) ), this, SLOT( close()            ));
	connect( ui.actionNewDbx       , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewDbx()     ));
	connect( ui.actionNewSvm       , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewSvm()     ));
	connect( ui.actionNewVdms      , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewVdms()    ));
	connect( ui.actionEjectar      , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarJuego() ));
	connect( ui.actionEjectarSetup , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarSetup() ));
	connect( ui.actionNuevo        , SIGNAL( triggered(bool) ), this, SLOT( on_NuevoJuego()    ));
	connect( ui.actionEditar       , SIGNAL( triggered(bool) ), this, SLOT( on_EditarJuego()   ));
	connect( ui.actionEiminar      , SIGNAL( triggered(bool) ), this, SLOT( on_EliminarJuego() ));
	connect( ui.actionImportarJuego, SIGNAL( triggered(bool) ), this, SLOT( on_ImportarJuego() ));
	connect( ui.actionOpciones     , SIGNAL( triggered(bool) ), this, SLOT( on_Opciones()      ));
	connect( ui.actionAcercaD      , SIGNAL( triggered(bool) ), this, SLOT( on_AcercaD()       ));

// Conecta los Menús
	connect( ui.mnu_file_informacion   , SIGNAL( triggered(bool) ), this, SLOT( on_Informacion()     ));
	connect( ui.mnu_file_cerrar        , SIGNAL( triggered(bool) ), this, SLOT( close()              ));
	connect( ui.mnu_edit_nuevo_dosbox  , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewDbx()       ));
	connect( ui.mnu_edit_nuevo_scummvm , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewSvm()       ));
	connect( ui.mnu_edit_nuevo_vdmsound, SIGNAL( triggered(bool) ), this, SLOT( on_AddNewVdms()      ));
	connect( ui.mnu_edit_nuevo         , SIGNAL( triggered(bool) ), this, SLOT( on_NuevoJuego()      ));
	connect( ui.mnu_edit_editar        , SIGNAL( triggered(bool) ), this, SLOT( on_EditarJuego()     ));
	connect( ui.mnu_edit_eliminar      , SIGNAL( triggered(bool) ), this, SLOT( on_EliminarJuego()   ));
	connect( ui.mnu_edit_favorito      , SIGNAL( triggered(bool) ), this, SLOT( on_setFavorito()     ));
	connect( ui.mnu_ejecutar_dosbox    , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarDosbox()  ));
	connect( ui.mnu_ejecutar_scummvm   , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarScummVM() ));
	connect( ui.mnu_ejecutar_juego     , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarJuego()   ));
	connect( ui.mnu_ejecutar_setup     , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarSetup()   ));
	connect( ui.mnu_tool_importar      , SIGNAL( triggered(bool) ), this, SLOT( on_ImportarJuego()   ));
	connect( ui.mnu_tool_exportar      , SIGNAL( triggered(bool) ), this, SLOT( on_ExportarJuego()   ));
	connect( ui.mnu_tool_opciones      , SIGNAL( triggered(bool) ), this, SLOT( on_Opciones()        ));
	connect( ui.mnu_ver_acercad        , SIGNAL( triggered(bool) ), this, SLOT( on_AcercaD()         ));

// Conecta los Menús del TrayIcon
	connect( ui.mnu_trayicon_minimize, SIGNAL( triggered(bool) ), this, SLOT( hide()          ));
	connect( ui.mnu_trayicon_maximize, SIGNAL( triggered(bool) ), this, SLOT( showMaximized() ));
	connect( ui.mnu_trayicon_restore , SIGNAL( triggered(bool) ), this, SLOT( showNormal()    ));
	connect( ui.mnu_file_cerrar      , SIGNAL( triggered(bool) ), this, SLOT( quit()          ));

// Conecta Otros Botones
	connect( ui.btnVer_CoverFront, SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverFront() ));
	connect( ui.btnVer_CoverBack , SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverBack()  ));
	connect( ui.btnVer_Archivos  , SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverBack()  ));

// Conecta la Lista de Juegos
	connect( ui.twJuegos, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_clicked(QTreeWidgetItem*)));
	connect( ui.twJuegos, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_Dblclicked(QTreeWidgetItem*) ));
	connect( ui.twJuegos, SIGNAL( customContextMenuRequested(const QPoint&)), this, SLOT(showPopup(const QPoint&) ));

// Conecta la Lista Nav
//	connect( ui.twNav, SIGNAL( itemClicked( QTreeWidgetItem*, int)), this, SLOT( on_twNav_clicked(QTreeWidgetItem*)));

// Conecta la Lista de Capturas
	connect( ui.twCapturas, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twCapturas_Dblclicked(QTreeWidgetItem*) ));

// Conecta la Lista de Urls
	connect( ui.twUrls, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twUrls_Dblclicked(QTreeWidgetItem*) ));

	tabifyDockWidget(ui.dockw_FilesUrl, ui.dockw_Datos);
}

// Crea la Barra de Busqueda y Ordenar
void GrLida::createToolBars()
{
	frameOrdenar = new QFrame( ui.toolBar_ordenar );
	frameOrdenar->setObjectName("frameOrdenar");
	frameOrdenar->setFrameShape(QFrame::NoFrame);

	HLayoutOrdenar = new QHBoxLayout( frameOrdenar );
	HLayoutOrdenar->setObjectName("HLayoutOrdenar");
	HLayoutOrdenar->setContentsMargins(0, 0, 0, 0);

	txtBuscar = new QLineEdit( frameOrdenar );
	txtBuscar->setObjectName("txtBuscar");
	connect(txtBuscar, SIGNAL(textChanged(QString)), this, SLOT( on_txtBuscar_textChanged(const QString &) ));

	HLayoutOrdenar->addWidget(txtBuscar);

	lb_ordenar_1 = new QLabel( frameOrdenar );
	lb_ordenar_1->setObjectName("lb_ordenar_1");
	lb_ordenar_1->setText( tr("Ordenar:") );

	HLayoutOrdenar->addWidget(lb_ordenar_1);

	QStringList lista_columnas_tabla;
	lista_columnas_tabla.clear();
	lista_columnas_tabla << "idgrl" << "titulo" << "subtitulo" << "genero" << "compania" << "desarrollador" << "tema" << "idioma" << "formato" << "anno" << "numdisc" << "sistemaop" << "graficos" << "sonido" << "jugabilidad" << "original" << "tipo_emu" << "favorito";

	cbxColTabla = new QComboBox( frameOrdenar );
	cbxColTabla->setObjectName("cbxColTabla");
	cbxColTabla->addItems( lista_columnas_tabla );

	HLayoutOrdenar->addWidget(cbxColTabla);

	lb_ordenar_2 = new QLabel( frameOrdenar );
	lb_ordenar_2->setObjectName("lb_ordenar_2");
	lb_ordenar_2->setText( tr("por") );

	HLayoutOrdenar->addWidget(lb_ordenar_2);

	cbxOrdenBy = new QComboBox( frameOrdenar );
	cbxOrdenBy->setObjectName("cbxOrdenBy");
	cbxOrdenBy->addItems( lista_columnas_tabla );

	HLayoutOrdenar->addWidget(cbxOrdenBy);

	cbxOrden = new QComboBox( frameOrdenar );
	cbxOrden->setObjectName("cbxOrden");
	cbxOrden->addItem(QIcon(stTheme+"img16/orden_asc.png"),"ASC");
	cbxOrden->addItem(QIcon(stTheme+"img16/orden_desc.png"),"DESC");

	HLayoutOrdenar->addWidget(cbxOrden);

	btn_Ordenar = new QToolButton( frameOrdenar );
	btn_Ordenar->setObjectName("btn_Ordenar");
	btn_Ordenar->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	btn_Ordenar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	btn_Ordenar->setAutoRaise(true);
	btn_Ordenar->setText( tr("Ordenar") );
	connect( btn_Ordenar, SIGNAL( clicked() ), this, SLOT( on_Ordenar_Lista() ));

	HLayoutOrdenar->addWidget(btn_Ordenar);

	ui.toolBar_ordenar->addWidget( frameOrdenar );
}

void GrLida::createStatusBar()
{
	lbpanel_1 = new QLabel( ui.statusBar );
	lbpanel_1->setFrameStyle( QFrame::NoFrame );
	lbpanel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lbpanel_1->setPixmap( QPixmap(stTheme+"img16/datos_3.png") );
	lbpanel_1->setMinimumSize(20,20);

	lbpanel_2 = new QLabel( ui.statusBar );
	lbpanel_2->setFrameStyle( QFrame::NoFrame );
	lbpanel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	lbpanel_3 = new QLabel( ui.statusBar );
	lbpanel_3->setFrameStyle( QFrame::NoFrame );
	lbpanel_3->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
	lbpanel_3->setMinimumSize(20,20);

	lbpanel_4 = new QLabel( ui.statusBar );
	lbpanel_4->setFrameStyle( QFrame::NoFrame );
	lbpanel_4->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lbpanel_4->setPixmap( QPixmap(stTheme+"img16/datos_1.png") );
	lbpanel_4->setMinimumSize(20,20);

	lbpanel_5 = new QLabel( ui.statusBar );
	lbpanel_5->setFrameStyle( QFrame::NoFrame );
	lbpanel_5->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui.statusBar->addWidget( lbpanel_1 );
	ui.statusBar->addWidget( lbpanel_2 );
	ui.statusBar->addWidget( lbpanel_3 );
	ui.statusBar->addWidget( lbpanel_4 );
	ui.statusBar->addWidget( lbpanel_5, 1 );
}

bool GrLida::createTrayIcon()
{
	if( QSystemTrayIcon::isSystemTrayAvailable() )
	{
		trayIconMenu = new QMenu(this);
		trayIconMenu->addAction( ui.mnu_trayicon_minimize );
		trayIconMenu->addAction( ui.mnu_trayicon_maximize );
		trayIconMenu->addAction( ui.mnu_trayicon_restore  );
		trayIconMenu->addSeparator();
		trayIconMenu->addAction( ui.mnu_file_cerrar );

		trayIcon = new QSystemTrayIcon(this);
		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(stTheme+"img16/grlida.png"));
		trayIcon->setToolTip("GR-lida");

		trayIcon->hide();
		return true;
	} else
		return false;
}

void GrLida::setVisible(bool visible)
{
	ui.mnu_trayicon_minimize->setEnabled(visible);
	ui.mnu_trayicon_maximize->setEnabled(!isMaximized());
	ui.mnu_trayicon_restore->setEnabled(isMaximized() || !visible);

	QMainWindow::setVisible(visible);
}

void GrLida::CargarConfig()
{
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
		stPicFlowReflection = settings.value("PicFlowReflection", "BlurredReflection").toString();
		numSkip_PicFlow = settings.value("Skip_PicFlow", 10).toInt();
	settings.endGroup();
	settings.beginGroup("MainState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
		restoreState(settings.value("state", saveState()).toByteArray()); // Toolbar and DockWidget state.
		//ui.splitter_pictureflow->restoreState(settings.value("splitter_pictureflow", ui.splitter_pictureflow->saveState()).toByteArray());
	settings.endGroup();
	settings.beginGroup("OpcVer");
		ui.actionVerDatos->setChecked(    settings.value("Pnl_Datos"   , "true"  ).toBool() );
		ui.dockw_Datos->setVisible(       settings.value("Pnl_Datos"   , "true"  ).toBool() );
		ui.actionVerCapturas->setChecked( settings.value("Pnl_Capturas", "false" ).toBool() );
		ui.dockw_Capturas->setVisible(    settings.value("Pnl_Capturas", "false" ).toBool() );
		ui.actionVerFilesUrl->setChecked( settings.value("Pnl_FilesUrl", "false" ).toBool() );
		ui.dockw_FilesUrl->setVisible(    settings.value("Pnl_FilesUrl", "false" ).toBool() );
		ui.mnu_ver_ordenar->setChecked(   settings.value("Pnl_Ordenar" , "false" ).toBool() );
		ui.toolBar_ordenar->setVisible(   settings.value("Pnl_Ordenar" , "false" ).toBool() );
		ui.mnu_ver_pictureflow->setChecked( settings.value("Pnl_PictureFlow","true").toBool() );
		ui.PicFlowWidget->setVisible( settings.value("Pnl_PictureFlow", "true").toBool() );
	settings.endGroup();

#ifdef Q_OS_WIN32
	if( (stBinExeDbx=="")&&(stBinExeSvm=="")&&(ui.actionNewVdms->isEnabled()==false) )
		ui.actionEjectar->setEnabled( false );
#else
	if( (stBinExeDbx=="")&&(stBinExeSvm=="") )
		ui.actionEjectar->setEnabled( false );
#endif

	cbxColTabla->setCurrentIndex( cbxColTabla->findText(stdb_Orden_ColTabla, Qt::MatchContains) );
	cbxOrdenBy->setCurrentIndex( cbxOrdenBy->findText(stdb_Orden_By, Qt::MatchContains) );
	cbxOrden->setCurrentIndex( cbxOrden->findText(stdb_Orden, Qt::MatchContains) );
}

void GrLida::GuardarConfig()
{
	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("OpcVer");
		settings.setValue("Pnl_Capturas"   , ui.actionVerCapturas->isChecked()  );
		settings.setValue("Pnl_Datos"      , ui.actionVerDatos->isChecked()     );
		settings.setValue("Pnl_FilesUrl"   , ui.actionVerFilesUrl->isChecked()  );
		settings.setValue("Pnl_Ordenar"    , ui.mnu_ver_ordenar->isChecked()    );
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
		settings.setValue("state", saveState()); // Toolbar and DockWidget state.
		settings.setValue("splitter_pictureflow", ui.splitter_pictureflow->saveState() );
	settings.endGroup();
}

void GrLida::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );

	lbpanel_1->setPixmap( QPixmap(stTheme+"img16/datos_3.png") );
	lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
	lbpanel_4->setPixmap( QPixmap(stTheme+"img16/datos_1.png") );

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

	btn_Ordenar->setIcon( QIcon(stTheme+"img16/aplicar.png") );
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

	ui.mnu_ver_archivos_url->setIcon( QIcon(stTheme+"img16/archivos.png") );
	ui.mnu_ver_datos->setIcon( QIcon(stTheme+"img16/datos_1.png") );
	ui.mnu_ver_capturas->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_ordenar->setIcon( QIcon(stTheme+"img16/orden_asc.png") );
	ui.mnu_ver_toolbar->setIcon( QIcon(stTheme+"img16/barra_herramintas.png") );
	ui.mnu_ver_acercad->setIcon( QIcon(stTheme+"img16/acercad.png") );

	ui.btnVer_Archivos->setIcon( QIcon(stTheme+"img16/archivos.png") );
	ui.btnVer_CoverFront->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnVer_CoverBack->setIcon( QIcon(stTheme+"img16/capturas.png") );

	ui.tabFileUrl->setTabIcon(0, QIcon(stTheme+"img16/edit_enlace.png") );
	ui.tabFileUrl->setTabIcon(1, QIcon(stTheme+"img16/archivos.png") );

	if( stPicFlowReflection == "BlurredReflection" || stPicFlowReflection.isEmpty() )
		ui.PicFlowWidget->setReflectionEffect(PictureFlow::BlurredReflection);
	else
		ui.PicFlowWidget->setReflectionEffect(PictureFlow::PlainReflection);
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

void GrLida::MostrarDatosDelJuego(QString IDitem)
{
	if(IDitem!="")
	{
		QSqlQuery query;
		QHash<QString, QString> strDatosJuego;

		strDatosJuego = sql->show_Datos( IDitem );

		QFile file_thumbs;  // thumbs
		if( file_thumbs.exists( strDatosJuego["thumbs"] ) )
			ui.lbImgThumbs->setPixmap( QPixmap( strDatosJuego["thumbs"] ) );
		else
			ui.lbImgThumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );

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

		str_html_comentario.clear();
		str_html_comentario = fGrl.ReemplazaTextoSmiles( strDatosJuego["comentario"] , listSmailes);

		ui.txtInfo_Comentario->setHtml( str_html_comentario );

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

		lbpanel_5->setText(" " + strDatosJuego["titulo"] + " - " + tr("introducido el") + " " + strDatosJuego["fecha"] + "  " );

		if( strDatosJuego["tipo_emu"] == "datos")
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/datos_1.png") );
		else if( strDatosJuego["tipo_emu"] == "dosbox")
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/dosbox.png") );
		else if( strDatosJuego["tipo_emu"] == "scummvm")
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/scummvm.png") );
		else if( strDatosJuego["tipo_emu"] == "vdmsound")
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/vdmsound.png") );
		else
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );

		ui.twUrls->clear();
		query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+IDitem);
		if( query.first())
		{
			do {
				QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twUrls );
				item_url->setText( 0 , query.record().value("url").toString()        ); // url
				item_url->setIcon( 0 , QIcon(stTheme+"img16/edit_enlace.png")                    ); // icono
				item_url->setText( 1 , query.record().value("descripcion").toString()); // descripcion
			//	item_url->setText( 2 , fGrl.IntToStr(query.record().value("id").toInt())    ); // id
			//	item_url->setText( 3 , fGrl.IntToStr(query.record().value("idgrl").toInt()) ); // idgrl
			} while (query.next());
			ui.twUrls->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
		}

		ui.twFiles->clear();
		query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+IDitem);
		if( query.first())
		{
			do {
				QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twFiles );
				item_files->setText( 0 , query.record().value("nombre").toString()     );	// nombre
				item_files->setIcon( 0 , QIcon(stTheme+"img16/importar.png")           );	// icono
				item_files->setText( 1 , query.record().value("crc").toString()        );	// crc
				item_files->setText( 2 , query.record().value("descripcion").toString());	// descripcion
				item_files->setText( 3 , query.record().value("size").toString()       );	// size
				item_files->setText( 4 , query.record().value("path").toString()       );	// path
			//	item_files->setText( 5 , fGrl.IntToStr(query.record().value("id").toInt())    );	// id
			//	item_files->setText( 6 , fGrl.IntToStr(query.record().value("idgrl").toInt()) );	// idgrl
			} while (query.next());
			ui.twFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
		}
	} else {
		ui.txtInfo_Subtitulo->clear();
		ui.txtInfo_Genero->clear();
		ui.txtInfo_Idioma->clear();
		ui.txtInfo_Formato->clear();
		ui.txtInfo_NumDisc->clear();
		ui.txtInfo_Anno->clear();
		ui.txtInfo_Tamano->clear();
		ui.txtInfo_Compania->clear();
		ui.txtInfo_Desarrollador->clear();
		ui.txtInfo_Tema->clear();
		ui.txtInfo_SistemaOp->clear();
		ui.txtInfo_Comentario->clear();

		ui.lbImgThumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		lbpanel_5->setText(" ");

		ui.twUrls->clear();
		ui.twFiles->clear();
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

void GrLida::on_twJuegos_clicked(QTreeWidgetItem *item)
{
	if( item )
	{
		stItemIndex.clear();			// Limpiamos el idgrl
		stTipoEmu.clear();				// Limpiamos el tipo_emu
		stItemIndex  = item->text(0);	// idgrl del juego en la Base de Datos
		stTipoEmu    = item->text(3);	// tipo_emu

		if(item->text(4).isEmpty())
			ui.mnu_edit_favorito->setEnabled(false);
		else
			ui.mnu_edit_favorito->setEnabled(true);

		ui.mnu_edit_favorito->setChecked( fGrl.StrToBool( item->text(4) ) );

		if( !item->text(5).isEmpty() )
		{
			int current_imgPicFlow=0, diferencia=0;

			current_imgPicFlow = fGrl.StrToInt( item->text(5) );
			diferencia  = id_ImgPicFlow_old - current_imgPicFlow;

			if( diferencia > numSkip_PicFlow || diferencia < -numSkip_PicFlow  )
				ui.PicFlowWidget->setCenterIndex( current_imgPicFlow );
			else
				ui.PicFlowWidget->showSlide( current_imgPicFlow );

			id_ImgPicFlow_old = current_imgPicFlow;
		}
	// Muestra los distintos datos del juego
		MostrarDatosDelJuego( stItemIndex );
	// Carga  la configuración dependiendo el emulador
		if( stTipoEmu == "dosbox" )
			Config_Dbx( stItemIndex );
		else if( stTipoEmu == "scummvm" )
			Config_Svm( stItemIndex );
		else if( stTipoEmu == "vdmsound" )
			Config_Vdms( stItemIndex );
		else
			Config_Clear();

	} else {
		lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		lbpanel_5->setText(" ");
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
		stTipoEmu    = item->text(3);		// indica el tipo de emulador
		on_EjecutarJuego();					// ejecuta el juego
	}else
		return;
}

void GrLida::on_twJuegos_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( (item1)&&(item2) )
		emit on_twJuegos_clicked( ui.twJuegos->currentItem() );
	else
		return;
}

void GrLida::on_twCapturas_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( item->text(1) );
		imgViewer->show();
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

void GrLida::on_twUrls_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		my_url.clear();
		my_url = ui.twUrls->currentItem()->text(0);
		QDesktopServices::openUrl( my_url );	// Abre la URL con el navegador por defecto
	}
}

void GrLida::on_txtBuscar_textChanged(const QString &text)
{
	if( cbxColTabla->currentText()!="" )
		stdb_Orden_ColTabla = cbxColTabla->currentText();
	else
		stdb_Orden_ColTabla = "titulo";

	if( cbxOrdenBy->currentText()!="" )
		stdb_Orden_By = cbxOrdenBy->currentText();
	else
		stdb_Orden_By = "titulo";

	if( cbxOrden->currentText()!="" )
		stdb_Orden = cbxOrden->currentText();
	else
		stdb_Orden = "ASC";

	CargarListaJuegosDB( text );
}

void GrLida::on_Ordenar_Lista()
{
	if( cbxColTabla->currentText()!="" )
		stdb_Orden_ColTabla = cbxColTabla->currentText();
	else
		stdb_Orden_ColTabla = "titulo";

	if( cbxOrdenBy->currentText()!="" )
		stdb_Orden_By = cbxOrdenBy->currentText();
	else
		stdb_Orden_By = "titulo";

	if( cbxOrden->currentText()!="" )
		stdb_Orden = cbxOrden->currentText();
	else
		stdb_Orden = "ASC";

	CargarListaJuegosDB( txtBuscar->text() );
}

void GrLida::on_Informacion()
{
	frmInfo * NewInfo = new frmInfo();
	NewInfo->exec();
}

void GrLida::NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem)
{
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

	QString stIcono = fGrl.getIconListaJuegos(imgEmu, stIconDir);

	item->setText( 0 , IDitem			 ); // idgrl
	item->setIcon( 0 , QIcon(stIcono)	 ); // icono
	item->setText( 1 , datos["titulo"]	 ); // titulo
	item->setText( 2 , datos["rating"]   ); // rating
	item->setText( 3 , datos["tipo_emu"] ); // tipo_emu
	item->setText( 4 , datos["favorito"] ); // favorito
	item->setText( 5 , fGrl.IntToStr( id_ImgPicFlow ) ); //

	id_ImgPicFlow++;

	if(datos["favorito"]=="true")
		item->setIcon( 1 , QIcon(stTheme+"img16/"+stIconoFav));
	else
		item->setIcon( 1 , QIcon());

	QImage img;
	if( img.load( datos["thumbs"] ) )
    	ui.PicFlowWidget->addSlide( img );
    else
    	ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );

	ui.twJuegos->setFocus();
	ui.twJuegos->clearSelection();
	ui.twJuegos->setCurrentItem( item );
	emit on_twJuegos_clicked( item );

	lbpanel_2->setText(" " + tr("Nº Juegos") + ": " + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
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
	if( (ui.twJuegos->topLevelItemCount()>0) && (stItemIndex!=""))
	{
		frmAddEditJuego * EditJuego = new frmAddEditJuego( true, stTipoEmu, stItemIndex, 0, Qt::Window);
		if( EditJuego->exec() == QDialog::Accepted )
		{
			sql->ItemActualizaDatos(EditJuego->DatosJuego, stItemIndex);
			ui.twJuegos->currentItem()->setText(1, EditJuego->DatosJuego["titulo"] );
			//ui.twJuegos->currentItem()->setText(2, EditJuego->DatosJuego["rating"] );
			ui.twJuegos->currentItem()->setText(4, EditJuego->DatosJuego["favorito"] );

			QImage img;
			if( img.load( EditJuego->DatosJuego["thumbs"] ) )
				ui.PicFlowWidget->setSlide( fGrl.StrToInt( ui.twJuegos->currentItem()->text(5) ) , img );
			else
				ui.PicFlowWidget->setSlide( fGrl.StrToInt( ui.twJuegos->currentItem()->text(5) ) , QPixmap(stTheme+"images/juego_sin_imagen.png") );

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

			QString stIcono = fGrl.getIconListaJuegos(EditJuego->DatosJuego["icono"], stIconDir);

			ui.twJuegos->currentItem()->setIcon( 0, QIcon( stIcono ) );

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
					MostrarDatosDelJuego();

				lbpanel_2->setText(" " + tr("Nº Juegos") + ": " + fGrl.IntToStr(sql->getCount("dbgrl"))+ "  " );
			}
		}
	} else
		MostrarDatosDelJuego();
}

void GrLida::Config_Dbx(QString IDitem)
{
	QHash<QString, QString> conf_dosbox;

// Limpiamos primero la informacion que pueda tener
	stConfgJuego.clear();
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

	stDirCapturas.clear();
	stDirCapturas = conf_dosbox["dosbox_captures"];

	if( stDirCapturas !="" )
		CargarThumbsTreeWidget( stDirCapturas );
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
}

void GrLida::Config_Svm(QString IDitem)
{
// Limpiamos primero la informacion que pueda tener
	stConfgJuego.clear();
	conf_scummvm.clear();

	conf_scummvm = sql->showConfg_ScummVM( IDitem );
	conf_scummvm["description"] = ui.twJuegos->currentItem()->text(1);

	stConfgJuego = "-c" + stHomeDir + "scummvm.ini|" + "-d" + conf_scummvm["debuglevel"] + "|" + conf_scummvm["game"];

	stDirCapturas.clear();
	stDirCapturas = conf_scummvm["path_capturas"];

	if( stDirCapturas !="" )
		CargarThumbsTreeWidget( stDirCapturas );
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
}

void GrLida::Config_Vdms(QString IDitem)
{
	QSqlQuery query;
// Limpiamos primero la informacion que pueda tener stConfgJuego
	stConfgJuego.clear();

	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDitem+" LIMIT 0,1");
	query.first();

	stConfgJuego = stConfgVdmSDir + query.record().value("path_conf").toString();

	ui.actionEjectar->setEnabled(true);
	ui.mnu_ejecutar_juego->setEnabled(true);
	ui.actionEjectarSetup->setEnabled(false);
}

void GrLida::Config_Clear()
{
	stDirCapturas.clear();
	stConfgJuego.clear();
	ui.twCapturas->clear();

	ui.actionEjectar->setEnabled(false);
	ui.mnu_ejecutar_juego->setEnabled(false);
	ui.actionEjectarSetup->setEnabled(false);
	ui.mnu_ejecutar_setup->setEnabled(false);
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

void GrLida::on_EjecutarDosbox()
{
	Ejecutar( stBinExeDbx );
}

void GrLida::on_EjecutarScummVM()
{
	Ejecutar( stBinExeSvm );
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

	if( stTipoEmu!="datos" )
	{
//		if( stConfgJuego!=""){
		// Ejecuta el juego con el emulador DOSBox
			if( stTipoEmu=="dosbox")
			{
				query.clear();
				query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stItemIndex+" LIMIT 0,1");
				query.first();

				ConfgJuegoSetup.clear();
			//	ConfgJuegoSetup["path_exe"]   = query.record().value("path_exe").toString()	;	// path_exe
				ConfgJuegoSetup["path_setup"] = query.record().value("path_setup").toString();	// path_setup

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

		if(Opciones->ui.cmbIconFav->currentText()!="")
			stIconoFav = Opciones->ui.cmbIconFav->currentText();
		else
			stIconoFav = "fav_0.png";

		numSkip_PicFlow = Opciones->ui.spinBox_SkipPicFlow->value();
		if(Opciones->DatosConfiguracion["PicFlowReflection"]=="BlurredReflection" || Opciones->DatosConfiguracion["PicFlowReflection"]== "")
			stPicFlowReflection = "BlurredReflection";
		else
			stPicFlowReflection = "PlainReflection";

		stTheme = Opciones->DatosConfiguracion["NameDirTheme"];
		setTheme();
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

void GrLida::ComprobarArchivosDatos(QString Version_GRL)
{
	QFile archivodatos;

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
