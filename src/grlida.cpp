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
#include "grlida_multimedia.h"
#include "grlida_ruleta.h"
#include "stardelegate.h"

#ifdef Q_OS_WIN32
#include <qt_windows.h>
#include <shellapi.h>
#pragma comment(lib, "SHELL32.LIB")
#endif

GrLida::GrLida(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

// Titulo de la aplicacion para los distintos Sistemas Operativos
	setWindowTitle( stTituloGrl() + " - " + fGrl.get_Plataforma() + " v" +  fGrl.stVersionGrl() );

	stHomeDir      = fGrl.GRlidaHomePath();
	stIconDir      = stHomeDir + "iconos/";
	stDatosDir     = stHomeDir + "datos/";
	stConfgDbxDir  = stHomeDir + "confdbx/";
	stConfgVdmSDir = stHomeDir + "confvdms/";

	stTheme = fGrl.ThemeGrl();

	createConnections();
	createDockBars();
	createToolBars();
	createStatusBar();
	CargarConfig();
	setTheme();

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
	delete sql;
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
	if(obj == ui.twJuegos || obj == ui.txtInfo_Comentario )
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
						on_twJuegos_clicked( ui.twJuegos->currentItem() );
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
						on_twJuegos_clicked( ui.twJuegos->currentItem() );
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
	sql = new dbSql( stdb_type, stdb_server, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port );

	if( !sql->dbisOpen() )
		QMessageBox::critical(0, tr("Error al abrir la Base de Datos"), tr("No se ha podido establecer una conexión con la Base de Datos.\nEsta aplicación necesita soporte de SQLite. Mira la documentación de Qt SQL driver para más información.\n\nClick Cancelar para salir."), QMessageBox::Cancel);
	else
		sql->CrearTablas();

	CrearMenuNav();
}

void GrLida::CargarListaJuegosDB(QString strBuscar, QString sqlQuery)
{
	QSqlQuery query;
	QString stIcono, stSqlWhere;
	int n, num_listadatos, num_juegos;
	QStringList str_ListaDatos;
	QImage img;

	num_juegos = sql->getCount("dbgrl");

	ui.PicFlowWidget->clear();
	ui.PicFlowWidget->setSlideSize(QSize(145, 186));
	id_ImgPicFlow = 0;
	id_ImgPicFlow_old = 0;

	ui.twJuegos->clear();

	if(stdb_Orden_ColTabla == "titulo" || stdb_Orden_ColTabla == "subtitulo" || stdb_Orden_ColTabla == "idgrl")
	{
		if( strBuscar == "" )
			stSqlWhere = sqlQuery;
		else
			stSqlWhere = "WHERE titulo LIKE '%"+strBuscar+"%'";

		setUpdatesEnabled( false );

		query.clear();
		query.exec("SELECT * FROM dbgrl "+stSqlWhere+" ORDER BY "+stdb_Orden_By+" "+stdb_Orden);
		if( query.first())
		{
			do {
				QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );
				item->setText( col_IdGrl   , query.record().value("idgrl").toString()    );	// idgrl
				item->setText( col_Titulo  , query.record().value("titulo").toString()   );	// titulo
				item->setText( col_Rating  , query.record().value("rating").toString()   );	// rating
				item->setText( col_TipoEmu , query.record().value("tipo_emu").toString() );	// tipo_emu
				item->setText( col_Favorito, query.record().value("favorito").toString() );	// favorito
				item->setText( col_PicFlow , QVariant( id_ImgPicFlow ).toString()        );

				if( img.load( query.record().value("thumbs").toString() ) )
					ui.PicFlowWidget->addSlide( img );
				else
					ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );

				id_ImgPicFlow++;

				stIcono = fGrl.getIconListaJuegos(query.record().value("icono").toString(), stIconDir);
				item->setIcon( col_IdGrl, QIcon(stIcono) );

				if( query.record().value("favorito").toString() == "true")
					item->setIcon( col_Titulo, QIcon(stTheme+"img16/"+stIconoFav)); // icono favorito

			} while ( query.next() );

			ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
			on_twJuegos_clicked( ui.twJuegos->currentItem() );

		} else {
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
			lbpanel_5->setText(" ");
		}

		setUpdatesEnabled( true );

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
		else if(stdb_Orden_ColTabla == "rating")
			str_ListaDatos << "0" << "1" << "2" << "3" << "4" << "5";

		if( strBuscar != "" )
			stSqlWhere = "AND titulo LIKE '%"+strBuscar+"%'";
		else
			stSqlWhere = "";

		num_listadatos = str_ListaDatos.size();

		if( num_listadatos >= 0 )
		{
			setUpdatesEnabled( false );
			for(n = 0; n < num_listadatos; ++n)// inicio de for lista datos
			{
				query.clear();
				query.exec("SELECT * FROM dbgrl WHERE "+stdb_Orden_ColTabla+" LIKE '"+ str_ListaDatos.at(n) +"' "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+ stdb_Orden );
				if( query.first() )
				{
					QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

					item->setText( col_IdGrl, "" );	// idgrl
					item->setIcon( col_IdGrl, QIcon(stTheme+"img24/emu_sin_imagen.png") );
					item->setTextColor( col_Titulo, QColor(0,0,0) );
					item->setFont( col_Titulo, QFont("Times", 10, QFont::Bold));

					if( str_ListaDatos.at(n) == "" || str_ListaDatos.at(n) == "NULL" )
						item->setText( col_Titulo, tr("Genero sin Clasificar") );	// titulo
					else {
						if( stdb_Orden_ColTabla == "graficos" )
							item->setText( col_Titulo, tr("Calificación Gráficos")+ ": " + str_ListaDatos.at(n) );
						else if( stdb_Orden_ColTabla == "sonido" )
							item->setText( col_Titulo, tr("Calificación Sonido")+ ": " + str_ListaDatos.at(n) );
						else if( stdb_Orden_ColTabla == "jugabilidad" )
							item->setText( col_Titulo, tr("Calificación Jugabilidad")+ ": " + str_ListaDatos.at(n) );
						else if( stdb_Orden_ColTabla == "original" )
						{
							if( str_ListaDatos.at(n) == "true")
								item->setText( col_Titulo, tr("Juegos Originales") );
							else
								item->setText( col_Titulo, tr("No lo son") );
						}
						else if( stdb_Orden_ColTabla == "favorito" )
						{
							if( str_ListaDatos.at(n) == "true")
								item->setText( col_Titulo, tr("Juegos Favoritos") );
							else
								item->setText( col_Titulo, tr("No lo son") );
						}
						else if( stdb_Orden_ColTabla == "rating" )
								item->setText( col_Titulo, tr("Calificación")+ ": " + str_ListaDatos.at(n) );
						else
							item->setText( col_Titulo, str_ListaDatos.at(n) );
					}
					item->setText( col_Rating , "" ); // rating
					item->setText( col_TipoEmu, "" ); // tipo_emu

					do {
						QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

						item->setText( col_IdGrl   , query.record().value("idgrl").toString()    );	// idgrl
						item->setText( col_Titulo  , query.record().value("titulo").toString()   );	// titulo
						item->setText( col_Rating  , query.record().value("rating").toString()   );	// rating
						item->setText( col_TipoEmu , query.record().value("tipo_emu").toString() );	// tipo_emu
						item->setText( col_Favorito, query.record().value("favorito").toString() );	// favorito
						item->setText( col_PicFlow , QVariant( id_ImgPicFlow ).toString()        );

						if( img.load( query.record().value("thumbs").toString() ) )
							ui.PicFlowWidget->addSlide( img );
						else
							ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );

						id_ImgPicFlow++;

						stIcono = fGrl.getIconListaJuegos(query.record().value("icono").toString(), stIconDir);
						item->setIcon( col_IdGrl, QIcon(stIcono) );

						if( query.record().value("favorito").toString() == "true" )
							item->setIcon( col_Titulo, QIcon(stTheme+"img16/"+stIconoFav) ); // icono favorito

					} while ( query.next() );

					ui.twJuegos->setCurrentItem( ui.twJuegos->itemAt(0,0) );
					on_twJuegos_clicked( ui.twJuegos->currentItem() );

				} else {
					lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
					lbpanel_5->setText(" ");
				}
			} // fin de for lista datos

			setUpdatesEnabled( true );
		}
	}

	if( num_juegos > 0)
		lbpanel_2->setText(" "+ tr("Nº Juegos") + ": " + QVariant(num_juegos).toString()+ "  " );
	else
		lbpanel_2->setText(" "+ tr("Nº Juegos") + ": 0  ");
}

// Conecta los distintos botones con las funciones de cada uno
void GrLida::createConnections()
{
	connect(ui.mnu_ver_archivos_url, SIGNAL( toggled(bool) ), this, SLOT( on_ver_archivos_url(bool) ));
	connect(ui.mnu_ver_datos       , SIGNAL( toggled(bool) ), this, SLOT( on_ver_datos(bool)        ));
	connect(ui.mnu_ver_capturas    , SIGNAL( toggled(bool) ), this, SLOT( on_ver_capturas(bool)     ));

	connect(ui.actionVerFilesUrl   , SIGNAL( toggled(bool) ), this, SLOT( on_ver_archivos_url(bool) ));
	connect(ui.actionVerDatos      , SIGNAL( toggled(bool) ), this, SLOT( on_ver_datos(bool)        ));
	connect(ui.actionVerCapturas   , SIGNAL( toggled(bool) ), this, SLOT( on_ver_capturas(bool)     ));

	connect(ui.mnu_ver_nav         , SIGNAL( toggled(bool) ), this                , SLOT( on_ver_nav(bool) ));
	connect(ui.mnu_ver_pictureflow , SIGNAL( toggled(bool) ), ui.dockw_PictureFlow, SLOT( setVisible(bool) ));
	connect(ui.mnu_ver_toolbar     , SIGNAL( toggled(bool) ), ui.toolBar          , SLOT( setVisible(bool) ));
	connect(ui.mnu_ver_ordenar     , SIGNAL( toggled(bool) ), ui.toolBar_ordenar  , SLOT( setVisible(bool) ));

	connect(ui.toolBar->toggleViewAction()        , SIGNAL( toggled(bool) ), ui.mnu_ver_toolbar, SLOT( setChecked(bool) ));
	connect(ui.toolBar_ordenar->toggleViewAction(), SIGNAL( toggled(bool) ), ui.mnu_ver_ordenar, SLOT( setChecked(bool) ));

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
	connect( ui.mnu_file_informacion   , SIGNAL( triggered(bool) ), this, SLOT( on_Informacion()       ));
	connect( ui.mnu_file_cerrar        , SIGNAL( triggered(bool) ), this, SLOT( close()                ));
	connect( ui.mnu_edit_nuevo_dosbox  , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewDbx()         ));
	connect( ui.mnu_edit_nuevo_scummvm , SIGNAL( triggered(bool) ), this, SLOT( on_AddNewSvm()         ));
	connect( ui.mnu_edit_nuevo_vdmsound, SIGNAL( triggered(bool) ), this, SLOT( on_AddNewVdms()        ));
	connect( ui.mnu_edit_nuevo         , SIGNAL( triggered(bool) ), this, SLOT( on_NuevoJuego()        ));
	connect( ui.mnu_edit_editar        , SIGNAL( triggered(bool) ), this, SLOT( on_EditarJuego()       ));
	connect( ui.mnu_edit_eliminar      , SIGNAL( triggered(bool) ), this, SLOT( on_EliminarJuego()     ));
	connect( ui.mnu_edit_favorito      , SIGNAL( triggered(bool) ), this, SLOT( on_setFavorito()       ));
	connect( ui.mnu_ejecutar_dosbox    , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarDosbox()    ));
	connect( ui.mnu_ejecutar_scummvm   , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarScummVM()   ));
	connect( ui.mnu_ejecutar_juego     , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarJuego()     ));
	connect( ui.mnu_ejecutar_setup     , SIGNAL( triggered(bool) ), this, SLOT( on_EjecutarSetup()     ));
	connect( ui.mnu_tool_importar      , SIGNAL( triggered(bool) ), this, SLOT( on_ImportarJuego()     ));
	connect( ui.mnu_tool_exportar      , SIGNAL( triggered(bool) ), this, SLOT( on_ExportarJuego()     ));
	connect( ui.mnu_tool_opciones      , SIGNAL( triggered(bool) ), this, SLOT( on_Opciones()          ));
	connect( ui.mnu_ver_acercad        , SIGNAL( triggered(bool) ), this, SLOT( on_AcercaD()           ));
	connect( ui.mnu_ver_rating         , SIGNAL( triggered(bool) ), this, SLOT( on_VerRating(bool)     ));

// Abre las distintas carpetas
	connect( ui.mnu_ver_carpeta_confdbx  , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_confdbx()   ));
	connect( ui.mnu_ver_carpeta_confvdms , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_confvdms()  ));
	connect( ui.mnu_ver_carpeta_thumbs   , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_thumbs()    ));
	connect( ui.mnu_ver_carpeta_covers   , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_covers()    ));
	connect( ui.mnu_ver_carpeta_iconos   , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_iconos()    ));
	connect( ui.mnu_ver_carpeta_smiles   , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_smiles()    ));
	connect( ui.mnu_ver_carpeta_themes   , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_themes()    ));
	connect( ui.mnu_ver_carpeta_datos    , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_datos()     ));
	connect( ui.mnu_ver_carpeta_scripts  , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_scripts()   ));
	connect( ui.mnu_ver_carpeta_templates, SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_templates() ));
	connect( ui.mnu_ver_carpeta_idiomas  , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_idiomas()   ));
	connect( ui.mnu_ver_carpeta_temp     , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_temp()      ));
	connect( ui.mnu_ver_carpeta_home     , SIGNAL( triggered(bool) ), this, SLOT( on_VerCarpeta_home()      ));

// Conecta los Menús del TrayIcon
	connect( ui.mnu_trayicon_minimize, SIGNAL( triggered(bool) ), this, SLOT( hide()          ));
	connect( ui.mnu_trayicon_maximize, SIGNAL( triggered(bool) ), this, SLOT( showMaximized() ));
	connect( ui.mnu_trayicon_restore , SIGNAL( triggered(bool) ), this, SLOT( showNormal()    ));
	connect( ui.mnu_file_cerrar      , SIGNAL( triggered(bool) ), this, SLOT( quit()          ));

// Conecta Otros Botones
	connect( ui.btnVer_CoverFront, SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverFront() ));
	connect( ui.btnVer_CoverBack , SIGNAL( clicked() ), this, SLOT( on_btnVer_CoverBack()  ));
	connect( ui.btnVer_Archivos  , SIGNAL( clicked() ), this, SLOT( on_btnVer_Archivos()   ));

// Conecta la Lista de Juegos
	connect( ui.twJuegos, SIGNAL( itemClicked(QTreeWidgetItem*,int)), this, SLOT( on_twJuegos_clicked(QTreeWidgetItem*) ));
	connect( ui.twJuegos, SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twJuegos_Dblclicked(QTreeWidgetItem*) ));
	connect( ui.twJuegos, SIGNAL( customContextMenuRequested(const QPoint&)), this, SLOT(showPopup(const QPoint&) ));

// Conecta la Lista Nav
	connect( ui.twListNav, SIGNAL( itemClicked( QTreeWidgetItem*,int)), this, SLOT( on_twListNav_clicked(QTreeWidgetItem*)));

// Conecta la Lista de Capturas
	connect( ui.twCapturas     , SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twCapturas_Dblclicked(QTreeWidgetItem*)      ));
	connect( ui.twCapturaVideo , SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twCapturaVideo_Dblclicked(QTreeWidgetItem*)  ));
	connect( ui.twCapturaSonido, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twCapturaSonido_Dblclicked(QTreeWidgetItem*) ));

// Conecta la Lista de Urls, Files
	connect( ui.twUrls , SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twUrls_Dblclicked(QTreeWidgetItem*)  ));
	connect( ui.twFiles, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twFiles_Dblclicked(QTreeWidgetItem*) ));
}

void GrLida::on_ver_nav(bool mChecked)
{
	ui.dockw_Nav->setVisible( mChecked );

//	if( mChecked )
//		CrearMenuNav();
}

void GrLida::on_ver_archivos_url(bool mChecked)
{
	ui.dockw_FilesUrl->setVisible( mChecked );
	ui.mnu_ver_archivos_url->setChecked( mChecked );
	ui.actionVerFilesUrl->setChecked( mChecked );

	if( mChecked )
	{
		MostrarArchivosURLDelJuego( stItemIndex );
		CargarCapturasVideoSonidos( stDirCapturas );
	}
}

void GrLida::on_ver_datos(bool mChecked)
{
	ui.dockw_Datos->setVisible( mChecked );
	ui.mnu_ver_datos->setChecked( mChecked );
	ui.actionVerDatos->setChecked( mChecked );

	if( mChecked )
		MostrarDatosDelJuego( stItemIndex );
}

void GrLida::on_ver_capturas(bool mChecked)
{
	ui.twCapturas->setVisible( mChecked );
	ui.mnu_ver_capturas->setChecked( mChecked );
	ui.actionVerCapturas->setChecked( mChecked );

	if( mChecked )
		CargarCapturasImagenes( stDirCapturas );
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

	cbxColTabla = new QComboBox( frameOrdenar );
	cbxColTabla->setObjectName("cbxColTabla");
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Por ID")       , "idgrl"        );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Titulo")       , "titulo"       );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Subtitulo")    , "subtitulo"    );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Genero")       , "genero"       );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Compañia")     , "compania"     );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Desarrollador"), "desarrollador");
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Tema")         , "tema"         );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Idioma")       , "idioma"       );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Formato")      , "formato"      );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Año")          , "anno"         );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Nº Discos")    , "numdisc"      );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Sistema OS")   , "sistemaop"    );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Gráficos")     , "graficos"     );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Sonido")       , "sonido"       );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Jugabilidad")  , "jugabilidad"  );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Original")     , "original"     );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Tipo emulador"), "tipo_emu"     );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Favorito")     , "favorito"     );
	cbxColTabla->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Rating")       , "rating"       );

	HLayoutOrdenar->addWidget(cbxColTabla);

	lb_ordenar_2 = new QLabel( frameOrdenar );
	lb_ordenar_2->setObjectName("lb_ordenar_2");
	lb_ordenar_2->setText( tr("por") );

	HLayoutOrdenar->addWidget(lb_ordenar_2);

	cbxOrdenBy = new QComboBox( frameOrdenar );
	cbxOrdenBy->setObjectName("cbxOrdenBy");
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Por ID")       , "idgrl"        );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Titulo")       , "titulo"       );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Subtitulo")    , "subtitulo"    );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Genero")       , "genero"       );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Compañia")     , "compania"     );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Desarrollador"), "desarrollador");
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Tema")         , "tema"         );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Idioma")       , "idioma"       );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Formato")      , "formato"      );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Año")          , "anno"         );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Nº Discos")    , "numdisc"      );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Sistema OS")   , "sistemaop"    );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Gráficos")     , "graficos"     );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Sonido")       , "sonido"       );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Jugabilidad")  , "jugabilidad"  );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Original")     , "original"     );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Tipo emulador"), "tipo_emu"     );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Favorito")     , "favorito"     );
	cbxOrdenBy->addItem(QIcon(stTheme+"img16/datos_3.png"), tr("Rating")       , "rating"       );

	HLayoutOrdenar->addWidget(cbxOrdenBy);

	cbxOrden = new QComboBox( frameOrdenar );
	cbxOrden->setObjectName("cbxOrden");
	cbxOrden->addItem(QIcon(stTheme+"img16/orden_asc.png") , tr("Ascendente") , "ASC" );
	cbxOrden->addItem(QIcon(stTheme+"img16/orden_desc.png"), tr("Descendente"), "DESC");

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

void GrLida::createDockBars()
{
// Arrange dock widgets
	setDockNestingEnabled(true);

	setCorner(Qt::TopLeftCorner    , Qt::LeftDockWidgetArea );
	setCorner(Qt::BottomLeftCorner , Qt::LeftDockWidgetArea );
	setCorner(Qt::TopRightCorner   , Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	addDockWidget(Qt::LeftDockWidgetArea  , ui.dockw_Nav        );
	addDockWidget(Qt::RightDockWidgetArea , ui.dockw_Datos      );
//	addDockWidget(Qt::RightDockWidgetArea , ui.dockw_Info       );
//	addDockWidget(Qt::BottomDockWidgetArea, ui.dockw_Capturas   );
	addDockWidget(Qt::BottomDockWidgetArea, ui.dockw_PictureFlow);

	tabifyDockWidget(ui.dockw_FilesUrl, ui.dockw_Datos      );
//	tabifyDockWidget(ui.dockw_Capturas, ui.dockw_PictureFlow);
}

void GrLida::CrearMenuNav()
{
//	if( ui.mnu_ver_nav->isChecked() )
//	{
		setUpdatesEnabled( false );

		ui.twListNav->clear();

		MenuNav_AddCat( tr("Todos")        , stTheme+"img16/basedatos.png", "all"      );
		MenuNav_AddCat( tr("Favoritos")    , stTheme+"img16/fav_0.png"    , "favorito" );
		MenuNav_AddCat( tr("Originales")   , stTheme+"img16/tag.png"      , "original" );
		MenuNav_AddCat( tr("Tipo Emulador"), stTheme+"img16/tag.png"      , "all" );
			MenuNav_AddSubCat("DOSBox"     , stTheme+"img16/dosbox.png"   , "tipo_emu" );
			MenuNav_AddSubCat("ScummVM"    , stTheme+"img16/scummvm.png"  , "tipo_emu" );
			MenuNav_AddSubCat("VDMSound"   , stTheme+"img16/vdmsound.png" , "tipo_emu" );
			MenuNav_AddSubCat("Datos"      , stTheme+"img16/datos_3.png"  , "tipo_emu" );
	// Dinamicos?
	//	MenuNav_AddCategorias();
		MenuNav_AddCat( tr("Generos"), stTheme+"img16/datos_3.png", "genero"  );
			MenuNav_AddSubCat(":/datos/generos.txt", "genero", 1, false);

		MenuNav_AddCat( tr("Tema"), stTheme+"img16/datos_3.png", "tema"  );
			MenuNav_AddSubCat(":/datos/tema.txt", "tema", 1, false);

		MenuNav_AddCat( tr("Desarrolladores"), stTheme+"img16/datos_3.png", "desarrollador"  );
			MenuNav_AddSubCat(":/datos/companias.txt", "desarrollador", 1, false);

		MenuNav_AddCat( tr("Compañias"), stTheme+"img16/datos_3.png", "compania"  );
			MenuNav_AddSubCat(":/datos/companias.txt", "compania", 1, false);

		MenuNav_AddCat( tr("Año"), stTheme+"img16/fecha.png", "anno"  );
			MenuNav_AddSubCat(":/datos/fechas.txt", "anno", 1, false);

		MenuNav_AddCat( tr("Idioma"), stTheme+"img16/idiomas.png", "idioma"  );
			MenuNav_AddSubCat(":/datos/svm_idioma.txt", "idioma", 1, true);

		ui.twListNav->setCurrentItem( ui.twListNav->itemAt(0,0) );

		setUpdatesEnabled( true );
//	}
}

void GrLida::MenuNav_AddCat(QString etiqueta_cat, QString icono_cat, QString tabla, bool m_expanded)
{
	QString sqlquery;
	int total = 0;

	setUpdatesEnabled( false );

	if( tabla == "all" )
		sqlquery = "";
	else if( tabla == "original" )
		sqlquery = "WHERE original='true'";
	else if( tabla == "favorito" )
		sqlquery = "WHERE favorito='true'";
	else
		sqlquery = "WHERE "+tabla+"!=''";

//	total = sql->getCount("dbgrl", sqlquery);

	twCategoria = new QTreeWidgetItem( ui.twListNav );
	twCategoria->setIcon( 0, QIcon( icono_cat )  );

//	if( total > 0 )
//		twCategoria->setFont( 1, QFont("Times", 10, QFont::Bold) );
//	else
//		twCategoria->setFont( 1, QFont("Times", 10, QFont::Normal) );

	twCategoria->setText( 0, etiqueta_cat );
	twCategoria->setText( 1, QVariant(total).toString() );
	twCategoria->setText( 2, tabla );
	twCategoria->setText( 3, "cat"   );

	twCategoria->setExpanded( m_expanded );

	setUpdatesEnabled( true );
}

void GrLida::MenuNav_AddSubCat(QString etiqueta_subcat, QString icono_subcat, QString tabla)
{
	int total = 0;

	setUpdatesEnabled( false );

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem( twCategoria );

//	total = sql->getCount("dbgrl","WHERE "+tabla+" LIKE '%"+etiqueta_subcat+"%'");

	sub_cat->setIcon( 0, QIcon( icono_subcat ) );
	sub_cat->setText( 0, etiqueta_subcat );

//	if( total > 0 )
//		sub_cat->setFont( 1, QFont("Times", 10, QFont::Bold) );
//	else
//		sub_cat->setFont( 1, QFont("Times", 10, QFont::Normal) );

	sub_cat->setText( 1, QVariant(total).toString() );
	sub_cat->setText( 2, tabla );
	sub_cat->setText( 3, "subcat" );

	setUpdatesEnabled( true );
}

void GrLida::MenuNav_AddSubCat(QString archivo, QString tabla, int num_col, bool idioma_svm)
{
	QStringList lista, listaTemp;
	QString icono, iconoTemp;
	int total = 0;

	QFile file( archivo );
	if( file.open(QIODevice::ReadOnly) != 0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			listaTemp << in.readLine();

		for( int i = 0; i < listaTemp.count(); i++ )
		{
			lista = listaTemp[i].split( "|" );

			icono.clear();
			icono = stTheme+"img16/sinimg.png";

			if( num_col == 3 )
				iconoTemp = lista.value(2);
			else
				iconoTemp = lista.value(1);

			if( idioma_svm )
				icono = stTheme+"img_lng/"+iconoTemp+".png";
			else
				icono = stTheme+"img16/"+iconoTemp+".png";

			QTreeWidgetItem *sub_cat = new QTreeWidgetItem( twCategoria );

//			total = sql->getCount("dbgrl","WHERE "+tabla+" LIKE '%"+lista.value(0)+"%'");

			sub_cat->setIcon( 0, QIcon( icono ) );
			sub_cat->setText( 0, lista.value(0) );

//			if( total > 0 )
//				sub_cat->setFont( 1, QFont("Times", 10, QFont::Bold) );
//			else
//				sub_cat->setFont( 1, QFont("Times", 10, QFont::Normal) );

			sub_cat->setText( 1, QVariant(total).toString() );
			sub_cat->setText( 2, tabla );
			sub_cat->setText( 3, "subcat" );
		}
	}
	file.close();

	setUpdatesEnabled( true );
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
		trayIconMenu->addAction( ui.mnu_trayicon_restore  );
		trayIconMenu->addAction( ui.mnu_trayicon_minimize );
		trayIconMenu->addAction( ui.mnu_trayicon_maximize );
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
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	stdb_type           = GRLConfig["db_type"].toString();
	stdb_server         = GRLConfig["db_server"].toString();
	stdb_host           = GRLConfig["db_host"].toString();
	stdb_name           = GRLConfig["db_name"].toString();
	stdb_username       = GRLConfig["db_username"].toString();
	stdb_password       = GRLConfig["db_password"].toString();
	stdb_port           = GRLConfig["db_port"].toString();
	stdb_Orden_ColTabla = GRLConfig["db_Orden_ColTabla"].toString();
	stdb_Orden_By       = GRLConfig["db_Orden_By"].toString();
	stdb_Orden          = GRLConfig["db_Orden"].toString();

//	if( !QFile::exists(stdb_host) )
//		stdb_host = stHomeDir + "db_grl.grl";

	stBinExeDbx = QDir::toNativeSeparators( GRLConfig["DirDOSBox"].toString()  );
	stBinExeSvm = QDir::toNativeSeparators( GRLConfig["DirScummVM"].toString() );
	ui.actionNewDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool()  );
	ui.actionNewSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool() );
	ui.actionNewVdms->setEnabled( GRLConfig["VDMSoundDisp"].toBool() );
	stIdiomaSelect = GRLConfig["IdiomaSelect"].toString();
	stIconoFav          = GRLConfig["IconoFav"].toString();
	stPicFlowReflection = GRLConfig["PicFlowReflection"].toString();
	numSkip_PicFlow     = GRLConfig["Skip_PicFlow"].toInt();
	ui.tabFileUrl->setCurrentIndex( GRLConfig["IndexTabArchivos"].toInt() );

	if( GRLConfig["maximized"].toBool() ) // maximizado.
		setWindowState( windowState() | Qt::WindowMaximized );

//	QRect ( int x, int y, int width, int height )
	setGeometry(QRect(0,0,738,589));
	ui.dockw_Nav->setGeometry(QRect(0,75,180,506));
	ui.dockw_Datos->setGeometry(QRect(418,75,382,506));
	ui.dockw_PictureFlow->setGeometry(QRect(0,299,356,282));

	restoreGeometry( GRLConfig["geometry"].toByteArray() );

	if( GRLConfig["Version"].toString() >= fGrl.stVersionGrl() )
		restoreState( GRLConfig["state"].toByteArray() );

	ui.mnu_ver_nav->setChecked(         GRLConfig["Pnl_Nav"].toBool()         );
	ui.actionVerFilesUrl->setChecked(   GRLConfig["Pnl_FilesUrl"].toBool()    );
	ui.actionVerDatos->setChecked(      GRLConfig["Pnl_Datos"].toBool()       );
	ui.mnu_ver_pictureflow->setChecked( GRLConfig["Pnl_PictureFlow"].toBool() );
	ui.actionVerCapturas->setChecked(   GRLConfig["Pnl_Capturas"].toBool()    );
	ui.mnu_ver_ordenar->setChecked(     GRLConfig["Pnl_Ordenar"].toBool()     );
	ui.mnu_ver_toolbar->setChecked(     GRLConfig["Pnl_Toolbar"].toBool()     );
	ui.mnu_ver_rating->setChecked(      GRLConfig["ver_Rating"].toBool()      );

	ui.dockw_Nav->setVisible(           GRLConfig["Pnl_Nav"].toBool()         );
	ui.dockw_FilesUrl->setVisible(      GRLConfig["Pnl_FilesUrl"].toBool()    );
	ui.dockw_Datos->setVisible(         GRLConfig["Pnl_Datos"].toBool()       );
	ui.dockw_PictureFlow->setVisible(   GRLConfig["Pnl_PictureFlow"].toBool() );
	ui.twCapturas->setVisible(          GRLConfig["Pnl_Capturas"].toBool()    );
	ui.toolBar_ordenar->setVisible(     GRLConfig["Pnl_Ordenar"].toBool()     );
	emit on_VerRating( GRLConfig["ver_Rating"].toBool() );

#ifdef Q_OS_WIN32
	if( stBinExeDbx == "" && stBinExeSvm == "" && ui.actionNewVdms->isEnabled() == false )
		ui.actionEjectar->setEnabled( false );
#else
	if( stBinExeDbx == "" && stBinExeSvm == "" )
		ui.actionEjectar->setEnabled( false );
#endif

	cbxColTabla->setCurrentIndex( cbxColTabla->findData( stdb_Orden_ColTabla ) );
	 cbxOrdenBy->setCurrentIndex(  cbxOrdenBy->findData( stdb_Orden_By       ) );
	   cbxOrden->setCurrentIndex(    cbxOrden->findData( stdb_Orden          ) );

	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles(stDatosDir + "smiles.txt");

	ui.toolBar->setContextMenuPolicy( Qt::PreventContextMenu );

	ui.twListNav->headerItem()->setIcon(0, QIcon(stTheme+"img16/tag.png"));
	ui.twListNav->header()->setMovable(false);
	ui.twListNav->header()->setResizeMode(0, QHeaderView::Stretch);

	ui.twJuegos->headerItem()->setIcon(0, QIcon(stTheme+"img16/bullet_black.png"));
	ui.twJuegos->headerItem()->setIcon(1, QIcon(stTheme+"img16/tag.png"));
	ui.twJuegos->headerItem()->setIcon(2, QIcon(stTheme+"images/star_on.png"));
	ui.twJuegos->headerItem()->setText(0, "");
	ui.twJuegos->header()->setStretchLastSection(false);
	ui.twJuegos->header()->setMovable(false);
	ui.twJuegos->header()->setResizeMode(0, QHeaderView::Fixed  );
	ui.twJuegos->header()->setResizeMode(1, QHeaderView::Stretch);
	ui.twJuegos->header()->setResizeMode(2, QHeaderView::Fixed  );
	ui.twJuegos->setColumnWidth(0, 31);
	ui.twJuegos->setColumnWidth(2, 90);
//	ui.twJuegos->setColumnHidden(2, true);
	ui.twJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.twJuegos->installEventFilter(this);
	ui.twJuegos->setItemDelegate( new StarDelegate(stTheme) );

//	ui.txtInfo_Comentario->installEventFilter( this );

	ui.twFiles->header()->setStretchLastSection(true);
	ui.twFiles->header()->setMovable(false);
//	ui.twFiles->header()->setResizeMode(QHeaderView::Interactive);
	ui.twFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui.twFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui.twFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui.twFiles->header()->setResizeMode(3, QHeaderView::Fixed      );
	ui.twFiles->header()->setResizeMode(4, QHeaderView::Interactive);
	ui.twFiles->setColumnWidth(0, 200 );	// nombre
	ui.twFiles->setColumnWidth(1, 50  );	// crc32
	ui.twFiles->setColumnWidth(2, 80  );	// descripcion
	ui.twFiles->setColumnWidth(3, 50  );	// size
	ui.twFiles->setColumnWidth(4, 50  );	//

	ui.twUrls->header()->setStretchLastSection(true);
	ui.twUrls->header()->setMovable(false);
	ui.twUrls->header()->setResizeMode(0, QHeaderView::Interactive);
	ui.twUrls->header()->setResizeMode(1, QHeaderView::Interactive);
	ui.twUrls->setColumnWidth(0, 250 );

	ui.twCapturas->header()->setStretchLastSection(true);
	ui.twCapturas->header()->setMovable(false);
	ui.twCapturas->setMinimumSize(QSize(170, 0));
	ui.twCapturas->setMaximumSize(QSize(170, 16777215));

	ui.twCapturaVideo->header()->setStretchLastSection(false);
	ui.twCapturaVideo->header()->setMovable(false);
	ui.twCapturaVideo->header()->setResizeMode(0, QHeaderView::Stretch);
	ui.twCapturaVideo->header()->setResizeMode(1, QHeaderView::Interactive);
	ui.twCapturaVideo->setColumnWidth(1, 200 );

	ui.twCapturaSonido->header()->setStretchLastSection(false);
	ui.twCapturaSonido->header()->setMovable(false);
	ui.twCapturaSonido->header()->setResizeMode(0, QHeaderView::Stretch);
	ui.twCapturaSonido->header()->setResizeMode(1, QHeaderView::Interactive);
	ui.twCapturaSonido->setColumnWidth(1, 200 );

	ui.btnVer_Archivos->setEnabled( false );
	ui.btnVer_CoverFront->setEnabled( false );
	ui.btnVer_CoverBack->setEnabled( false );
}

void GrLida::GuardarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	GRLConfig["IndexTabArchivos"]= ui.tabFileUrl->currentIndex();

	GRLConfig["Pnl_Nav"]         = ui.mnu_ver_nav->isChecked();
	GRLConfig["Pnl_FilesUrl"]    = ui.actionVerFilesUrl->isChecked();
	GRLConfig["Pnl_Datos"]       = ui.actionVerDatos->isChecked();
	GRLConfig["Pnl_PictureFlow"] = ui.mnu_ver_pictureflow->isChecked();
	GRLConfig["Pnl_Capturas"]    = ui.actionVerCapturas->isChecked();
	GRLConfig["Pnl_Ordenar"]     = ui.mnu_ver_ordenar->isChecked();
	GRLConfig["Pnl_Toolbar"]     = ui.mnu_ver_toolbar->isChecked();
	GRLConfig["ver_Rating"]     = ui.mnu_ver_rating->isChecked();

	GRLConfig["db_Orden_ColTabla"] = stdb_Orden_ColTabla;
	GRLConfig["db_Orden_By"]       = stdb_Orden_By;
	GRLConfig["db_Orden"]          = stdb_Orden;

	GRLConfig["maximized"] = isMaximized();
	GRLConfig["geometry"]  = saveGeometry();
	GRLConfig["state"]     = saveState(); // Toolbar and DockWidget state.

	fGrl.GuardarGRLConfig( stHomeDir + "GR-lida.conf", GRLConfig );
}

void GrLida::setTheme()
{
	setUpdatesEnabled( false );

	setStyleSheet( fGrl.StyleSheet() );

	QFile LeerArchivo(stTheme+"tpl_juego_info.html");
	if( LeerArchivo.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &LeerArchivo );
		in.setCodec("UTF-8");
		str_html_old.clear();
		str_html_old = in.readAll();
	} else
		str_html_old = "";
	LeerArchivo.close();

// Reempla las Etiquetas
	//str_html_old.replace("{dir_style_css}", "C:/eclipse/workspace/gr-lida/res/", Qt::CaseSensitive);
	str_html_old.replace("{lb_cover_front}"   , tr("Carátula Frontal")  , Qt::CaseSensitive);
	str_html_old.replace("{lb_detalles_juego}", tr("Detalles del juego"), Qt::CaseSensitive);
	str_html_old.replace("{lb_calificacion}"  , tr("Calificación")      , Qt::CaseSensitive);
	str_html_old.replace("{lb_otros_datos}"   , tr("Otros Datos")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_usuario}"       , tr("Subido por")        , Qt::CaseSensitive);
	str_html_old.replace("{lb_subtitulo}"     , tr("Subtitulo")         , Qt::CaseSensitive);
	str_html_old.replace("{lb_compania}"      , tr("Publicado por")     , Qt::CaseSensitive);
	str_html_old.replace("{lb_desarrollador}" , tr("Desarrollado por")  , Qt::CaseSensitive);
	str_html_old.replace("{lb_anno}"          , tr("Publicado")         , Qt::CaseSensitive);
	str_html_old.replace("{lb_idioma}"        , tr("Idioma")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_formato}"       , tr("Formato")           , Qt::CaseSensitive);
	str_html_old.replace("{lb_genero}"        , tr("Genero")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_tema}"          , tr("Tema")              , Qt::CaseSensitive);
	str_html_old.replace("{lb_sistemaop}"     , tr("Sistema Operativo") , Qt::CaseSensitive);
	str_html_old.replace("{lb_favorito}"      , tr("Favorito")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_fecha}"         , tr("Añadido el")        , Qt::CaseSensitive);
	str_html_old.replace("{lb_graficos}"      , tr("Gráficos")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_sonido}"        , tr("Sonido")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_jugabilidad}"   , tr("Jugabilidad")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_original}"      , tr("Original")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_estado}"        , tr("Estado")            , Qt::CaseSensitive);
	str_html_old.replace("{lb_tipo_emu}"      , tr("Tipo Emu")          , Qt::CaseSensitive);
	str_html_old.replace("{lb_comentario}"    , tr("Descripción")       , Qt::CaseSensitive);
	str_html_old.replace("{lb_rating}"        , tr("Rating")            , Qt::CaseSensitive);

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
// Menu Archivo
	ui.mnu_file_informacion->setIcon( QIcon(stTheme+"img16/informacion.png") );
	ui.mnu_file_cerrar->setIcon( QIcon(stTheme+"img16/cerrar.png") );
// Menu Editar
	ui.mnu_edit_nuevo_dosbox->setIcon( QIcon(stTheme+"img16/dosbox.png") );
	ui.mnu_edit_nuevo_scummvm->setIcon( QIcon(stTheme+"img16/scummvm.png") );
	ui.mnu_edit_nuevo_vdmsound->setIcon( QIcon(stTheme+"img16/vdmsound.png") );
	ui.mnu_edit_nuevo->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.mnu_edit_editar->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.mnu_edit_eliminar->setIcon( QIcon(stTheme+"img16/eliminar.png") );
	ui.mnu_edit_favorito->setIcon( QIcon(stTheme+"img16/fav_0.png") );
// Menu Ejecutar
	ui.mnu_ejecutar_dosbox->setIcon( QIcon(stTheme+"img16/dosbox.png") );
	ui.mnu_ejecutar_scummvm->setIcon( QIcon(stTheme+"img16/scummvm.png") );
	ui.mnu_ejecutar_juego->setIcon( QIcon(stTheme+"img16/ejecutar_app.png") );
	ui.mnu_ejecutar_setup->setIcon( QIcon(stTheme+"img16/ejecutar_app_setup.png") );
// Menu Herramientas
	ui.mnu_tool_importar->setIcon( QIcon(stTheme+"img16/importar.png") );
	ui.mnu_tool_exportar->setIcon( QIcon(stTheme+"img16/exportar.png") );
	ui.mnu_tool_opciones->setIcon( QIcon(stTheme+"img16/opciones.png") );
// Menu Ver
	ui.mnu_ver_nav->setIcon( QIcon(stTheme+"img16/categorias.png") );
	ui.mnu_ver_archivos_url->setIcon( QIcon(stTheme+"img16/archivos.png") );
	ui.mnu_ver_datos->setIcon( QIcon(stTheme+"img16/datos_1.png") );
	ui.mnu_ver_pictureflow->setIcon( QIcon(stTheme+"img16/pictureflow.png") );
	ui.mnu_ver_capturas->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_ordenar->setIcon( QIcon(stTheme+"img16/orden_asc.png") );
	ui.mnu_ver_toolbar->setIcon( QIcon(stTheme+"img16/barra_herramintas.png") );
	ui.mnu_ver_rating->setIcon( QIcon(stTheme+"images/star_on.png") );
	ui.mnu_ver_ayuda->setIcon( QIcon(stTheme+"img16/ayuda.png") );
	ui.mnu_ver_acercad->setIcon( QIcon(stTheme+"img16/acercad.png") );
// Sub Menu Ver Carpetas
	ui.mnu_ver_carpeta_confdbx->setIcon( QIcon(stTheme+"img16/dosbox.png") );
	ui.mnu_ver_carpeta_confvdms->setIcon( QIcon(stTheme+"img16/vdmsound.png") );
	ui.mnu_ver_carpeta_thumbs->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_carpeta_covers->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_carpeta_iconos->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_carpeta_smiles->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.mnu_ver_carpeta_themes->setIcon( QIcon(stTheme+"img16/style.png") );
	ui.mnu_ver_carpeta_datos->setIcon( QIcon(stTheme+"img16/datos_3.png") );
	ui.mnu_ver_carpeta_scripts->setIcon( QIcon(stTheme+"img16/archivo_config.png") );
	ui.mnu_ver_carpeta_templates->setIcon( QIcon(stTheme+"img16/datos_3.png") );
	ui.mnu_ver_carpeta_idiomas->setIcon( QIcon(stTheme+"img16/idiomas.png") );
	ui.mnu_ver_carpeta_temp->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.mnu_ver_carpeta_home->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
// Otros Botones
	ui.btnVer_Archivos->setIcon( QIcon(stTheme+"img16/archivos.png") );
	ui.btnVer_CoverFront->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnVer_CoverBack->setIcon( QIcon(stTheme+"img16/capturas.png") );

	ui.tabFileUrl->setTabIcon(0, QIcon(stTheme+"img16/img_tv2x.png") );
	ui.tabFileUrl->setTabIcon(1, QIcon(stTheme+"img16/img_audio.png") );
	ui.tabFileUrl->setTabIcon(2, QIcon(stTheme+"img16/edit_enlace.png") );
	ui.tabFileUrl->setTabIcon(3, QIcon(stTheme+"img16/archivos.png") );

	if( stPicFlowReflection == "BlurredReflection" || stPicFlowReflection.isEmpty() )
		ui.PicFlowWidget->setReflectionEffect(PictureFlow::BlurredReflection);
	else
		ui.PicFlowWidget->setReflectionEffect(PictureFlow::PlainReflection);

	setUpdatesEnabled( true );
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
//	ljMenuPopUp->exec(QCursor::pos());
	delete ljMenuPopUp;
}

void GrLida::MostrarDatosDelJuego(QString IDitem)
{
	if( IDitem != "" && ui.mnu_ver_datos->isChecked() )
	{
		QSqlQuery query;
		QHash<QString, QString> strDatosJuego;
		QString stThumbs, stIcono;
		QFile file_thumbs;

		setUpdatesEnabled( false );

		str_html_new.clear();
		str_html_comentario.clear();

		strDatosJuego = sql->show_Datos( IDitem );

		str_html_new = str_html_old;
		str_html_comentario = fGrl.ReemplazaTextoSmiles( strDatosJuego["Dat_comentario"], listSmailes);

		stIcono = fGrl.getIconListaJuegos(strDatosJuego["Dat_icono"], stIconDir);

		if( file_thumbs.exists( strDatosJuego["Dat_thumbs"] ) )
			stThumbs = strDatosJuego["Dat_thumbs"];
		else
			stThumbs = stTheme+"images/juego_sin_imagen.png";

	// Reempla la info del juego.
		str_html_new.replace("{info_icono}"        , stIcono                       , Qt::CaseSensitive);
		str_html_new.replace("{info_titulo}"       , strDatosJuego["Dat_titulo"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_subtitulo}"    , strDatosJuego["Dat_subtitulo"]    , Qt::CaseSensitive);
		str_html_new.replace("{info_genero}"       , strDatosJuego["Dat_genero"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_compania}"     , strDatosJuego["Dat_compania"]     , Qt::CaseSensitive);
		str_html_new.replace("{info_desarrollador}", strDatosJuego["Dat_desarrollador"], Qt::CaseSensitive);
		str_html_new.replace("{info_tema}"         , strDatosJuego["Dat_tema"]         , Qt::CaseSensitive);
		str_html_new.replace("{info_idioma}"       , strDatosJuego["Dat_idioma"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_formato}"      , strDatosJuego["Dat_formato"]      , Qt::CaseSensitive);
		str_html_new.replace("{info_anno}"         , strDatosJuego["Dat_anno"]         , Qt::CaseSensitive);
//		str_html_new.replace("{info_numdisc}"      , strDatosJuego["Dat_numdisc"]      , Qt::CaseSensitive);
		str_html_new.replace("{info_sistemaop}"    , strDatosJuego["Dat_sistemaop"]    , Qt::CaseSensitive);
//		str_html_new.replace("{info_tamano}"       , strDatosJuego["Dat_tamano"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_graficos}"     , strDatosJuego["Dat_graficos"]     , Qt::CaseSensitive);
		str_html_new.replace("{info_sonido}"       , strDatosJuego["Dat_sonido"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_jugabilidad}"  , strDatosJuego["Dat_jugabilidad"]  , Qt::CaseSensitive);
		str_html_new.replace("{info_original}"     , strDatosJuego["Dat_original"]     , Qt::CaseSensitive);
		str_html_new.replace("{info_estado}"       , strDatosJuego["Dat_estado"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_thumbs}"       , stThumbs                      , Qt::CaseSensitive);
//		str_html_new.replace("{info_cover_front}"  , strDatosJuego["Dat_cover_front"]  , Qt::CaseSensitive);
//		str_html_new.replace("{info_cover_back}"   , strDatosJuego["Dat_cover_back"]   , Qt::CaseSensitive);
		str_html_new.replace("{info_fecha}"        , strDatosJuego["Dat_fecha"]        , Qt::CaseSensitive);
		str_html_new.replace("{info_tipo_emu}"     , strDatosJuego["Dat_tipo_emu"]     , Qt::CaseSensitive);
		str_html_new.replace("{info_favorito}"     , strDatosJuego["Dat_favorito"]     , Qt::CaseSensitive);
		str_html_new.replace("{info_rating}"       , strDatosJuego["Dat_rating"]       , Qt::CaseSensitive);
		str_html_new.replace("{info_usuario}"      , strDatosJuego["Dat_usuario"]      , Qt::CaseSensitive);
		str_html_new.replace("{info_comentario}"   , str_html_comentario           , Qt::CaseSensitive);

		ui.txtInfo_Comentario->setHtml( str_html_new );

		setUpdatesEnabled( true );

//		lbpanel_5->setText(" " + strDatosJuego["Dat_titulo"] + " - " + tr("introducido el") + " " + strDatosJuego["Dat_fecha"] + "  " );

		stCaratula_Delantera = strDatosJuego["Dat_cover_front"];
		stCaratula_Trasera   = strDatosJuego["Dat_cover_back"];

		if( stCaratula_Delantera.isEmpty() )
			ui.btnVer_CoverFront->setEnabled(false);
		else
			ui.btnVer_CoverFront->setEnabled(true);

		if( stCaratula_Trasera.isEmpty() )
			ui.btnVer_CoverBack->setEnabled(false);
		else
			ui.btnVer_CoverBack->setEnabled(true);
	} else {
		stCaratula_Delantera = "";
		stCaratula_Trasera   = "";
		ui.btnVer_CoverFront->setEnabled(false);
		ui.btnVer_CoverBack->setEnabled(false);
		ui.txtInfo_Comentario->clear();

		lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		lbpanel_5->setText(" ");
	}
}

void GrLida::MostrarArchivosURLDelJuego(QString IDitem)
{
	if( IDitem != "" && ui.mnu_ver_archivos_url->isChecked() )
	{
		QSqlQuery query;

		ui.twUrls->clear();
		query.clear();
		query.exec("SELECT * FROM dbgrl_url WHERE idgrl='"+IDitem+"' ORDER BY url ASC");
		if( query.first())
		{
			setUpdatesEnabled( false );
			do {
				QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twUrls );
				item_url->setText( 0, query.record().value("url").toString()         ); // url
				item_url->setIcon( 0, QIcon(stTheme+"img16/edit_enlace.png")         ); // icono
				item_url->setText( 1, query.record().value("descripcion").toString() ); // descripcion
			} while (query.next());
			setUpdatesEnabled( true );
		}

		ui.twFiles->clear();
		query.clear();
		query.exec("SELECT * FROM dbgrl_file WHERE idgrl='"+IDitem+"' ORDER BY nombre ASC");
		if( query.first() )
		{
			setUpdatesEnabled( false );
			do {
				QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twFiles );

				QString file_ico = query.record().value("tipo").toString();
				if( file_ico == "manual" )
					item_files->setIcon( 0, QIcon(stTheme+"img16/datos_1.png") ); // icono
				else if( file_ico == "ruleta" )
					item_files->setIcon( 0, QIcon(stTheme+"img16/ruleta.png") ); // icono
				else if( file_ico == "archivo" )
					item_files->setIcon( 0, QIcon(stTheme+"img16/archivos.png") ); // icono
				else
					item_files->setIcon( 0, QIcon(stTheme+"img16/archivos.png") ); // icono

				item_files->setText( 0, query.record().value("nombre").toString()      ); // nombre
				item_files->setText( 1, query.record().value("crc").toString()         ); // crc
				item_files->setText( 2, query.record().value("descripcion").toString() ); // descripcion
				item_files->setText( 3, query.record().value("size").toString()        ); // size
				item_files->setText( 4, query.record().value("path").toString()        ); // path
				item_files->setText( 5, query.record().value("tipo").toString()        ); // tipo
			} while (query.next());
			setUpdatesEnabled( true );
		}
	} else {
		ui.twUrls->clear();
		ui.twFiles->clear();
	}
}

void GrLida::CargarCapturasImagenes(const QString directorio)
{
	if( directorio !="" && ui.mnu_ver_capturas->isChecked() )
	{
		QStringList filters;
		filters.clear();
		filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas

		setUpdatesEnabled( false );
		fGrl.CargarListaDeCaptura(ui.twCapturas, directorio, filters, true);
		setUpdatesEnabled( true );
	} else
		ui.twCapturas->clear();
}

void GrLida::CargarCapturasVideoSonidos(const QString directorio)
{
	if( directorio !="" && ui.mnu_ver_archivos_url->isChecked() )
	{
		QStringList filters;

		filters.clear();
	// Video soportados
		filters << "*.avi" << "*.mkv" << "*.mov" << "*.mp4" << "*.mpeg" << "*.mpg" << "*.wmv";

		setUpdatesEnabled( false );
		fGrl.CargarListaDeCaptura(ui.twCapturaVideo, directorio, filters);
		setUpdatesEnabled( true );

		filters.clear();
	// Sonidos soportados
		filters << "*.mp3" << "*.ogg" << "*.wav" << "*.wma";

		setUpdatesEnabled( false );
		fGrl.CargarListaDeCaptura(ui.twCapturaSonido, directorio, filters);
		setUpdatesEnabled( true );
	} else {
		ui.twCapturaVideo->clear();
		ui.twCapturaSonido->clear();
	}
}

void GrLida::on_setFavorito()
{
	if( ui.mnu_edit_favorito->isChecked() )
	{
		ui.twJuegos->currentItem()->setIcon( col_Titulo  , QIcon(stTheme+"img16/"+stIconoFav) ); // icono favorito
		ui.twJuegos->currentItem()->setText( col_Favorito, "true" );
		sql->ItemActualizaDatosFavorito("true", ui.twJuegos->currentItem()->text(col_IdGrl) );
	} else {
		ui.twJuegos->currentItem()->setIcon( col_Titulo  , QIcon() ); // icono favorito
		ui.twJuegos->currentItem()->setText( col_Favorito, "false" );
		sql->ItemActualizaDatosFavorito("false", ui.twJuegos->currentItem()->text(col_IdGrl) );
	}
}

void GrLida::on_twJuegos_clicked(QTreeWidgetItem *item)
{
	if( item )
	{
		stItemIndex.clear();			// Limpiamos el idgrl
		stTipoEmu.clear();				// Limpiamos el tipo_emu
		stItemIndex = item->text(col_IdGrl);	// idgrl del juego en la Base de Datos
		stTipoEmu   = item->text(col_TipoEmu);	// tipo_emu

		if( item->text(col_Favorito).isEmpty() )
			ui.mnu_edit_favorito->setEnabled(false);
		else {
			ui.mnu_edit_favorito->setEnabled(true);
			ui.mnu_edit_favorito->setChecked( fGrl.StrToBool( item->text(col_Favorito) ) );
		}

		if( !item->text(col_PicFlow).isEmpty() && ui.mnu_ver_pictureflow->isChecked() )
		{
			int current_imgPicFlow=0, diferencia=0;
			current_imgPicFlow = QVariant( item->text(col_PicFlow) ).toInt();
			diferencia = id_ImgPicFlow_old - current_imgPicFlow;

			if( diferencia > numSkip_PicFlow || diferencia < -numSkip_PicFlow )
				ui.PicFlowWidget->setCenterIndex( current_imgPicFlow );
			else
				ui.PicFlowWidget->showSlide( current_imgPicFlow );

			id_ImgPicFlow_old = current_imgPicFlow;
		}

	// Muestra los distintos datos del juego
		MostrarDatosDelJuego( stItemIndex );
		MostrarArchivosURLDelJuego( stItemIndex );

		QSqlQuery query;
		query.clear();
		query.exec("SELECT titulo, fecha FROM dbgrl WHERE idgrl="+stItemIndex+" LIMIT 0,1");
		query.first();
		lbpanel_5->setText(" " + query.record().value("titulo").toString() + " - " + tr("introducido el") + " " + query.record().value("fecha").toString() + "  " );

	// Carga  la configuración dependiendo el emulador
		if( stTipoEmu == "datos" )
		{
			Config_Clear();
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/datos_1.png") );
		}
		else if( stTipoEmu == "dosbox" )
		{
			Config_Dbx( stItemIndex );
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/dosbox.png") );
		}
		else if( stTipoEmu == "scummvm" )
		{
			Config_Svm( stItemIndex );
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/scummvm.png") );
		}
		else if( stTipoEmu == "vdmsound" )
		{
			Config_Vdms( stItemIndex );
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/vdmsound.png") );
		} else {
			Config_Clear();
			lbpanel_3->setPixmap( QPixmap(stTheme+"img16/sinimg.png") );
		}
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
		stItemIndex.clear();					// Limpiamos el idgrl
		stTipoEmu.clear();						// Limpiamos el tipo_emu
		stItemIndex = item->text(col_IdGrl);	// index del juego en la Base de Datos
		stTipoEmu   = item->text(col_TipoEmu);	// indica el tipo de emulador
		on_EjecutarJuego();						// ejecuta el juego
	}else
		return;
}

void GrLida::on_twListNav_clicked(QTreeWidgetItem *item)
{
	QString sqlquery;
	if( item )
	{
		if( item->text(2) == "all" )
			sqlquery = "";
		else if( item->text(2) == "favorito" )
			sqlquery = "WHERE favorito='true'";
		else if( item->text(2) == "original" )
			sqlquery = "WHERE original='true'";
		else if( item->text(3) == "subcat" && item->text(2) != "all" && item->text(2) != "favorito" && item->text(2) != "original" )
			sqlquery = "WHERE "+item->text(2)+" LIKE '%"+item->text(0)+"%'";
		else
			sqlquery = "WHERE "+item->text(2)+"!=''";

		CargarListaJuegosDB("", sqlquery);
	} else
		return;
}

void GrLida::on_twListNav_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2)
{
	if( item1 && item2 )
		emit on_twListNav_clicked( ui.twListNav->currentItem() );
	else
		return;
}

void GrLida::on_twCapturas_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);

		QList<QString> lista;
		for(int num = 0; num < ui.twCapturas->topLevelItemCount(); num++ )
		{
			lista << ui.twCapturas->topLevelItem( num )->text(1);
		}
		imgViewer->open( item->text(1), lista );

		lista.clear();

		imgViewer->show();
	}
}

void GrLida::on_btnVer_CoverFront()
{
	if( stCaratula_Delantera != "" )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->show();
		imgViewer->open( stCaratula_Delantera );
	}
}

void GrLida::on_btnVer_CoverBack()
{
	if( stCaratula_Trasera != "" )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->show();
		imgViewer->open( stCaratula_Trasera );
	}
}

void GrLida::on_btnVer_Archivos()
{
	//
}

void GrLida::on_twCapturaVideo_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		frmMultiMedia *m_Media = new frmMultiMedia(0, Qt::Window);
//		m_Media->setWindowModality(Qt::WindowModal);
		QStringList lista;
		for(int num = 0; num < ui.twCapturaVideo->topLevelItemCount(); num++ )
		{
			lista << ui.twCapturaVideo->topLevelItem( num )->text(1);
		}
		m_Media->show();
		int id = lista.indexOf(item->text(1),0);
		m_Media->AbrirArchivos(lista);
		m_Media->setItemID(id);
		lista.clear();
	}
}

void GrLida::on_twCapturaSonido_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		frmMultiMedia *m_Media = new frmMultiMedia(0, Qt::Window);
		m_Media->setWindowModality(Qt::WindowModal);
		QStringList lista;
		for(int num = 0; num < ui.twCapturaSonido->topLevelItemCount(); num++ )
		{
			lista << ui.twCapturaSonido->topLevelItem( num )->text(1);
		}
		m_Media->show();
		int id = lista.indexOf(item->text(1),0);
		m_Media->AbrirArchivos(lista);
		m_Media->setItemID(id);
		lista.clear();
	}
}

void GrLida::on_twUrls_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		my_url.clear();
		my_url = fGrl.url_correcta( item->text(0) );
		QDesktopServices::openUrl( my_url );	// Abre la URL con el navegador por defecto
	}
}

void GrLida::on_twFiles_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		if( item->text(5) == "manual" )
		{
			ImageViewer *imgViewer = new ImageViewer(this);
			imgViewer->setWindowModality(Qt::WindowModal);
			imgViewer->show();
			imgViewer->openZip( item->text(4) );
		}
		else if( item->text(5) == "ruleta" )
		{
			frmRuleta *m_Ruleta = new frmRuleta(0, Qt::Window);
			m_Ruleta->setWindowModality(Qt::WindowModal);
			m_Ruleta->CargarRuleta(item->text(4));
			m_Ruleta->exec();
			delete m_Ruleta;
		} else
			fGrl.abrirArchivo( item->text(4) );
	}
}

void GrLida::on_txtBuscar_textChanged(const QString &text)
{
	stdb_Orden_ColTabla = cbxColTabla->itemData( cbxColTabla->currentIndex() ).toString();
	stdb_Orden_By       = cbxOrdenBy->itemData( cbxOrdenBy->currentIndex() ).toString();
	stdb_Orden          = cbxOrden->itemData( cbxOrden->currentIndex() ).toString();

	if( stdb_Orden_ColTabla.isEmpty() )
		stdb_Orden_ColTabla = "titulo";

	if( stdb_Orden_By.isEmpty() )
		stdb_Orden_By = "titulo";

	if( stdb_Orden.isEmpty() )
		stdb_Orden = "ASC";

	if( text.length() == 0)
		CargarListaJuegosDB();
	else if(text.length() >= 3)
		CargarListaJuegosDB( text );
}

void GrLida::on_Ordenar_Lista()
{
	stdb_Orden_ColTabla = cbxColTabla->itemData( cbxColTabla->currentIndex() ).toString();
	stdb_Orden_By       = cbxOrdenBy->itemData( cbxOrdenBy->currentIndex() ).toString();
	stdb_Orden          = cbxOrden->itemData( cbxOrden->currentIndex() ).toString();

	if( stdb_Orden_ColTabla.isEmpty() )
		stdb_Orden_ColTabla = "titulo";

	if( stdb_Orden_By.isEmpty() )
		stdb_Orden_By = "titulo";

	if( stdb_Orden.isEmpty() )
		stdb_Orden = "ASC";

	CargarListaJuegosDB( txtBuscar->text() );
}

void GrLida::on_Informacion()
{
	frmInfo *NewInfo = new frmInfo();
	NewInfo->exec();
	delete NewInfo;
}

void GrLida::NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem)
{
	QTreeWidgetItem *item = new QTreeWidgetItem( ui.twJuegos );

	QString stIcono = fGrl.getIconListaJuegos(imgEmu, stIconDir);

	item->setText( col_IdGrl   , IDitem            ); // idgrl
	item->setIcon( col_IdGrl   , QIcon( stIcono )  ); // icono
	item->setText( col_Titulo  , datos["Dat_titulo"]   ); // titulo
	item->setText( col_Rating  , datos["Dat_rating"]   ); // rating
	item->setText( col_TipoEmu , datos["Dat_tipo_emu"] ); // tipo_emu
	item->setText( col_Favorito, datos["Dat_favorito"] ); // favorito
	item->setText( col_PicFlow , QVariant(id_ImgPicFlow).toString() );

	id_ImgPicFlow++;

	if( datos["Dat_favorito"] == "true" )
		item->setIcon( col_Titulo, QIcon(stTheme+"img16/"+stIconoFav));

	QImage img;
	if( img.load( datos["Dat_thumbs"] ) )
		ui.PicFlowWidget->addSlide( img );
	else
		ui.PicFlowWidget->addSlide( QPixmap(stTheme+"images/juego_sin_imagen.png") );

//	CrearMenuNav();

	ui.twJuegos->setFocus();
	ui.twJuegos->clearSelection();
	ui.twJuegos->setCurrentItem( item );
	on_twJuegos_clicked( item );

	lbpanel_2->setText(" " + tr("Nº Juegos") + ": " + QVariant(sql->getCount("dbgrl")).toString()+ "  " );
}

void GrLida::on_AddNewDbx()
{
	frmDbxAdd *AddDbxNew = new frmDbxAdd(0, Qt::Window);
	if( AddDbxNew->exec() == QDialog::Accepted )
	{
		QString lastID, lastID_Dbx;
		lastID = sql->ItemInsertaDatos( AddDbxNew->DatosJuego );
		lastID_Dbx = sql->ItemInsertaDbx( AddDbxNew->DatosDosBox , lastID);
		sql->ItemInsertaMontajesDbx( AddDbxNew->ui.twMontajes, lastID_Dbx );
		fGrl.CrearArchivoConfigDbx( AddDbxNew->DatosJuego, AddDbxNew->DatosDosBox, AddDbxNew->ui.twMontajes, stConfgDbxDir + AddDbxNew->DatosDosBox["Dbx_path_conf"] );
		NuevoItemTreeWidget( AddDbxNew->DatosJuego, "dosbox", lastID);
	}
	delete AddDbxNew;
}

void GrLida::on_AddNewSvm()
{
	frmSvmAdd *AddSvmNew = new frmSvmAdd(0, Qt::Window);
	if( AddSvmNew->exec() == QDialog::Accepted )
	{
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddSvmNew->DatosJuego );
		sql->ItemInsertaSvm( AddSvmNew->DatosScummVM, lastID);
		NuevoItemTreeWidget( AddSvmNew->DatosJuego, "scummvm", lastID);
	}
	delete AddSvmNew;
}

void GrLida::on_AddNewVdms()
{
	frmVdmsAdd *AddVdmsNew = new frmVdmsAdd(0, Qt::Window);
	if( AddVdmsNew->exec() == QDialog::Accepted )
	{
		QString lastID;
		lastID = sql->ItemInsertaDatos( AddVdmsNew->DatosJuego );
		sql->ItemInsertaVdms( AddVdmsNew->DatosVDMSound, lastID);
		fGrl.CrearArchivoConfigVdmS( AddVdmsNew->DatosVDMSound, stConfgVdmSDir + AddVdmsNew->DatosVDMSound["Vdms_path_conf"] );
		NuevoItemTreeWidget( AddVdmsNew->DatosJuego, "vdmsound", lastID);
	}
	delete AddVdmsNew;
}

void GrLida::on_NuevoJuego()
{
	QString lastID, lastID_Dbx;
	frmAddEditJuego *AddJuego = new frmAddEditJuego( false, "datos", "", 0, Qt::Window);

	if( AddJuego->exec() == QDialog::Accepted )
	{
		lastID = sql->ItemInsertaDatos( AddJuego->DatosJuego );

		sql->ItemInsertaFiles( AddJuego->ui.twDatosFiles, lastID );
		sql->ItemInsertaURL( AddJuego->ui.twDatosURL, lastID );

		if( AddJuego->DatosJuego["Dat_tipo_emu"] == "dosbox")
		{
			lastID_Dbx = sql->ItemInsertaDbx( AddJuego->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx( AddJuego->ui.wDbx->ui.twMontajes, lastID_Dbx );
			fGrl.CrearArchivoConfigDbx( AddJuego->DatosDosBox, AddJuego->DatosDosBox, AddJuego->ui.wDbx->ui.twMontajes, stConfgDbxDir + AddJuego->DatosDosBox["Dbx_path_conf"] );
		}
		if( AddJuego->DatosJuego["Dat_tipo_emu"] == "scummvm")
		{
			sql->ItemInsertaSvm( AddJuego->DatosScummVM, lastID);
		}
		if( AddJuego->DatosJuego["Dat_tipo_emu"] == "vdmsound")
		{
			sql->ItemInsertaVdms( AddJuego->DatosVDMSound, lastID);
			fGrl.CrearArchivoConfigVdmS( AddJuego->DatosVDMSound, stConfgVdmSDir + AddJuego->DatosVDMSound["Vdms_path_conf"] );
		}

		NuevoItemTreeWidget( AddJuego->DatosJuego, AddJuego->DatosJuego["Dat_tipo_emu"], lastID);
	}
	delete AddJuego;
}

void GrLida::on_EditarJuego()
{
	if( ui.twJuegos->topLevelItemCount() > 0 && stItemIndex != "" )
	{
		frmAddEditJuego *EditJuego = new frmAddEditJuego( true, stTipoEmu, stItemIndex, 0, Qt::Window);
		if( EditJuego->exec() == QDialog::Accepted )
		{
			sql->ItemActualizaDatos(EditJuego->DatosJuego, stItemIndex);
			ui.twJuegos->currentItem()->setText( col_Titulo  , EditJuego->DatosJuego["Dat_titulo"]   );
			ui.twJuegos->currentItem()->setText( col_Rating  , EditJuego->DatosJuego["Dat_rating"]   );
			ui.twJuegos->currentItem()->setText( col_Favorito, EditJuego->DatosJuego["Dat_favorito"] );

			QImage img;
			if( img.load( EditJuego->DatosJuego["Dat_thumbs"] ) )
				ui.PicFlowWidget->setSlide( QVariant( ui.twJuegos->currentItem()->text(col_PicFlow) ).toInt(), img );
			else
				ui.PicFlowWidget->setSlide( QVariant( ui.twJuegos->currentItem()->text(col_PicFlow) ).toInt(), QPixmap(stTheme+"images/juego_sin_imagen.png") );

			if( stTipoEmu == "dosbox" )
			{
				sql->ItemActualizaDbx( EditJuego->DatosDosBox, EditJuego->DatosDosBox["Dbx_id"] );
				sql->ItemActualizaMontajeDbx(EditJuego->ui.wDbx->ui.twMontajes);
				fGrl.CrearArchivoConfigDbx( EditJuego->DatosJuego, EditJuego->DatosDosBox, EditJuego->ui.wDbx->ui.twMontajes, stConfgDbxDir + EditJuego->DatosDosBox["Dbx_path_conf"] );
			}
			if( stTipoEmu == "scummvm" )
			{
				sql->ItemActualizaSvm( EditJuego->DatosScummVM, EditJuego->DatosScummVM["Svm_id"] );
			}
			if( stTipoEmu == "vdmsound" )
			{
				sql->ItemActualizaVdms( EditJuego->DatosVDMSound, EditJuego->stItemIDVdms );
				fGrl.CrearArchivoConfigVdmS( EditJuego->DatosVDMSound, stConfgVdmSDir + EditJuego->DatosVDMSound["Vdms_path_conf"] );
			}

			QString stIcono = fGrl.getIconListaJuegos(EditJuego->DatosJuego["Dat_icono"], stIconDir);

			ui.twJuegos->currentItem()->setIcon( col_IdGrl, QIcon( stIcono ) );

			if(EditJuego->DatosJuego["Dat_favorito"] == "true" )
				ui.twJuegos->currentItem()->setIcon( col_Titulo, QIcon(stTheme+"img16/"+stIconoFav) ); // icono favorito

//			CrearMenuNav();

			on_twJuegos_clicked( ui.twJuegos->currentItem() );
		}
		delete EditJuego;
	}
}

void GrLida::on_EliminarJuego()
{
	if( ui.twJuegos->topLevelItemCount() > 0 )
	{
		if( stItemIndex.isEmpty() )
			QMessageBox::information( this, stTituloGrl(), tr("Porfavor selecciona un Juego de la lista para eliminarlo") );
		else {
			int respuesta = QMessageBox::question( this, tr("¿Eliminar Juego...?"), tr("¿Deseas realmente eliminar este juego de la Base de Datos?\nSi es de DOSBox o VDMSound tambien se borrar el archivo de configuración"), tr("Si"), tr("Cancelar"), 0, 1 );
			if( respuesta == 0 )
			{
				sql->ItemEliminar( stItemIndex );

				ui.PicFlowWidget->setSlide( QVariant( ui.twJuegos->currentItem()->text(col_PicFlow) ).toInt(), QPixmap(stTheme+"images/juego_eliminado.png") );

				fGrl.DeleteItemTree( ui.twJuegos->currentItem() );

				QMessageBox::information( this, stTituloGrl(), tr("Juego Eliminado correctamente"));

				if( ui.twJuegos->topLevelItemCount() > 0 )
					on_twJuegos_clicked( ui.twJuegos->currentItem() );
				else
					MostrarDatosDelJuego();

				lbpanel_2->setText(" " + tr("Nº Juegos") + ": " + QVariant(sql->getCount("dbgrl")).toString()+ "  " );

//				CrearMenuNav();
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
	stDirWorkingJuego.clear();
	conf_dosbox.clear();

	conf_dosbox = sql->showConfg_DOSBox( IDitem );

	QString consolaDbx;
	#ifdef Q_OS_WIN32
		if( conf_dosbox["Dbx_opt_consola_dbox"] == "true" )
			consolaDbx = "|-noconsole";
		else
			consolaDbx.clear();
	#else
		consolaDbx.clear();
	#endif

	stConfgJuego = "-conf|" + stConfgDbxDir + conf_dosbox["Dbx_path_conf"] + consolaDbx;

	QFileInfo fi( conf_dosbox["Dbx_path_exe"] );
	if( fi.exists() )
		stDirWorkingJuego = fi.absolutePath();
	else
		stDirWorkingJuego = "";

	stDirCapturas.clear();
	stDirCapturas = conf_dosbox["Dbx_dosbox_captures"];

	CargarCapturasImagenes( stDirCapturas );
	CargarCapturasVideoSonidos( stDirCapturas );

	if( !QFile( stConfgDbxDir + conf_dosbox["Dbx_path_conf"] ).exists() )
	{
		ui.actionEjectar->setEnabled(false);
		ui.mnu_ejecutar_juego->setEnabled(false);
		QMessageBox::information( this, stTituloGrl(), tr("Configuración del juego no encontrada.")+ "\n\n'" + stConfgDbxDir + conf_dosbox["Dbx_path_conf"] + "'" );
	} else {
		ui.actionEjectar->setEnabled(true);
		ui.mnu_ejecutar_juego->setEnabled(true);
	}

	if( QFile( conf_dosbox["Dbx_path_setup"] ).exists() )
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
	stDirWorkingJuego.clear();
	conf_scummvm.clear();

	conf_scummvm = sql->showConfg_ScummVM( IDitem );
	conf_scummvm["Svm_description"] = ui.twJuegos->currentItem()->text(col_Titulo);

	stConfgJuego = "-c" + stHomeDir + "scummvm.ini|" + "-d" + conf_scummvm["Svm_debuglevel"] + "|" + conf_scummvm["Svm_game_label"];

	QDir dir( conf_scummvm["Svm_path"] );
	if( dir.exists() )
		stDirWorkingJuego = conf_scummvm["Svm_path"];
	else
		stDirWorkingJuego = "";

	stDirCapturas.clear();
	stDirCapturas = conf_scummvm["Svm_path_capturas"];

	CargarCapturasImagenes( stDirCapturas );
	CargarCapturasVideoSonidos( stDirCapturas );

	ui.actionEjectar->setEnabled(true);
	ui.mnu_ejecutar_juego->setEnabled(true);

	if( QFile( conf_scummvm["Svm_path_setup"] ).exists() )
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
	stDirWorkingJuego.clear();

	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDitem+" LIMIT 0,1");
	query.first();

	stConfgJuego = stConfgVdmSDir + query.record().value("path_conf").toString();

	QFileInfo fi( query.record().value("path_exe").toString() );
	if( fi.exists() )
		stDirWorkingJuego = fi.absolutePath();
	else
		stDirWorkingJuego = "";

	stDirCapturas.clear();
	stDirCapturas = "";

	CargarCapturasImagenes(stDirCapturas);
	CargarCapturasVideoSonidos(stDirCapturas);

	ui.actionEjectar->setEnabled(true);
	ui.mnu_ejecutar_juego->setEnabled(true);
	ui.actionEjectarSetup->setEnabled(false);
}

void GrLida::Config_Clear()
{
	stDirCapturas.clear();
	stConfgJuego.clear();
	stDirWorkingJuego.clear();
	ui.twCapturas->clear();

	ui.actionEjectar->setEnabled(false);
	ui.mnu_ejecutar_juego->setEnabled(false);
	ui.actionEjectarSetup->setEnabled(false);
	ui.mnu_ejecutar_setup->setEnabled(false);
}

void GrLida::Ejecutar(const QString &bin, const QString &parametros, const QString &dirWorking)
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

		if( dirWorking != "" )
			dBoxSvm->setWorkingDirectory( dirWorking );

		if( parametros !="")
		{
			stl_param << parametros.split("|", QString::SkipEmptyParts);
			dBoxSvm->start( bin , stl_param );
			stl_param.clear();
		} else
			dBoxSvm->start( bin );

		ui.actionEjectar->setEnabled(false);

		connect( dBoxSvm, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( fin_Proceso(int, QProcess::ExitStatus) ) );
		connect( dBoxSvm, SIGNAL( error(QProcess::ProcessError) ), this, SLOT( fin_ProcesoError(QProcess::ProcessError) ) );

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
	if( stTipoEmu != "datos" )
	{
		if( stConfgJuego != "" )
		{
		// Ejecuta el juego con el emulador DOSBox
			if( stTipoEmu == "dosbox" )
				Ejecutar( stBinExeDbx, stConfgJuego, stDirWorkingJuego);
		// Ejecuta el juego con el emulador ScummVM
			if( stTipoEmu == "scummvm")
			{
			// Creamos el INI de configuracion del ScummVM
				fGrl.CreaIniScummVM(stHomeDir+"scummvm.ini", conf_scummvm);
				Ejecutar( stBinExeSvm, stConfgJuego, stDirWorkingJuego);
			}
		// Ejecuta el juego con el emulador vdmsound
			if( stTipoEmu == "vdmsound" )
			{
				#ifdef Q_OS_WIN32
					if( (long)ShellExecute(0, 0, reinterpret_cast<const WCHAR*>(stConfgJuego.utf16()), 0, 0, SW_SHOWNORMAL) <= 32)
						QMessageBox::information(this, stTituloGrl(), tr("No se ha podido ejecutar el juego"));
				#else
					QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)"));
				#endif
			}
		} else if( stConfgJuego == "")
			QMessageBox::information( this, stTituloGrl(),tr("No dispones de ninguna configuración") );
		else
			QMessageBox::information( this, stTituloGrl(),tr("Porfavor, seleccione un juego de la lista antes.") );
	}
}

void GrLida::on_EjecutarSetup()
{
	QString stConfgSetup;
	QSqlQuery query;

	if( stTipoEmu != "datos" )
	{
		query.clear();
		stConfgSetup.clear();
	// Ejecuta el juego con el emulador DOSBox
		if( stTipoEmu == "dosbox" )
		{
			query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+stItemIndex+" LIMIT 0,1");
			query.first();

			stConfgSetup = query.record().value("path_setup").toString();	// path_setup
		}
	// Ejecuta el juego con el emulador ScummVM
		if( stTipoEmu=="scummvm")
		{
			query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+stItemIndex+" LIMIT 0,1");
			query.first();

			stConfgSetup = query.record().value("path_setup").toString();	// path_setup
		}
	// Ejecuta el juego con el emulador vdmsound
//		if( stTipoEmu=="vdmsound")
//		{
//			#ifdef Q_OS_WIN32
//
//			#else
//				QMessageBox::information(this, stTituloGrl(), tr("Solo disponible para SO, Windows (NT/2000/XP)"));
//			#endif
//		}
		QFileInfo fi( stConfgSetup );
		if( fi.exists() )
			stDirWorkingJuego = fi.absolutePath();
		else
			stDirWorkingJuego = "";

		if( stConfgJuego == "" || stDirWorkingJuego == "" )
			QMessageBox::information(this, stTituloGrl(),tr("No dispones de ninguna configuración"));
		else {
			#ifdef Q_OS_WIN32
				if( stDirWorkingJuego.endsWith("/") )
					stDirWorkingJuego.replace("/","\\");
				else
					stDirWorkingJuego.append("/").replace("/","\\");

				Ejecutar( stBinExeDbx, fGrl.getShortPathName( stConfgSetup ), stDirWorkingJuego);
			#else
				Ejecutar( stBinExeDbx, stConfgSetup, stDirWorkingJuego);
			#endif
		}
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
	frmImportarJuego *ImportarJuego = new frmImportarJuego("", 0, Qt::Window );
	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		QString lastID, lastID_Dbx;

		lastID = sql->ItemInsertaDatos( ImportarJuego->DatosJuego );

		sql->ItemInsertaFiles( ImportarJuego->ui.twDatosFiles, lastID );
		sql->ItemInsertaURL( ImportarJuego->ui.twDatosURL, lastID );

		if( ImportarJuego->DatosJuego["Dat_tipo_emu"] == "dosbox")
		{
			lastID_Dbx = sql->ItemInsertaDbx(ImportarJuego->DatosDosBox , lastID);
			sql->ItemInsertaMontajesDbx(ImportarJuego->ui.twMontajes, lastID_Dbx );
			fGrl.CrearArchivoConfigDbx(ImportarJuego->DatosJuego, ImportarJuego->DatosDosBox, ImportarJuego->ui.twMontajes, stConfgDbxDir + ImportarJuego->DatosDosBox["Dbx_path_conf"]);
		}

		if( ImportarJuego->DatosJuego["Dat_tipo_emu"] == "scummvm" )
		{
			sql->ItemInsertaSvm(ImportarJuego->DatosScummVM, lastID);
		}

		if( ImportarJuego->DatosJuego["Dat_tipo_emu"] == "vdmsound" )
		{
			sql->ItemInsertaVdms(ImportarJuego->DatosVDMSound, lastID);
		}

		NuevoItemTreeWidget(ImportarJuego->DatosJuego, ImportarJuego->DatosJuego["Dat_tipo_emu"], lastID);
	}
	delete ImportarJuego;
}

void GrLida::on_ExportarJuego()
{
	frmExportarJuego *NewExportar = new frmExportarJuego(0, Qt::Window);
	NewExportar->exec();
	delete NewExportar;
}

void GrLida::on_Opciones()
{
	frmOpciones *Opciones = new frmOpciones();
	if( Opciones->exec() == QDialog::Accepted )
	{
		GRLConfig = Opciones->DatosConfiguracion;

		ui.actionNewDbx->setEnabled( GRLConfig["DOSBoxDisp"].toBool()  );
		ui.actionNewSvm->setEnabled( GRLConfig["ScummVMDisp"].toBool()  );
		ui.actionNewVdms->setEnabled( GRLConfig["VDMSoundDisp"].toBool() );

		stBinExeDbx = GRLConfig["DirDOSBox"].toString();
		stBinExeSvm = GRLConfig["DirScummVM"].toString();

	#ifdef Q_OS_WIN32
		if( (GRLConfig["DOSBoxDisp"].toBool()==false)&&(GRLConfig["ScummVMDisp"].toBool()==false)&&(GRLConfig["VDMSoundDisp"].toBool()==false) )
			ui.actionEjectar->setEnabled( false );
		else
			ui.actionEjectar->setEnabled( true );
	#else
		if( (GRLConfig["DOSBoxDisp"].toBool()==false)&&(GRLConfig["ScummVMDisp"].toBool()==false) )
			ui.actionEjectar->setEnabled( false );
		else
			ui.actionEjectar->setEnabled( true );
	#endif

		stIconoFav = GRLConfig["IconoFav"].toString();

		numSkip_PicFlow = GRLConfig["Skip_PicFlow"].toInt();

		if( GRLConfig["PicFlowReflection"].toString() == "BlurredReflection" || GRLConfig["PicFlowReflection"].toString() == "")
			stPicFlowReflection = "BlurredReflection";
		else
			stPicFlowReflection = "PlainReflection";

		stTheme = GRLConfig["stTheme"].toString();
		setTheme();
	}
	delete Opciones;
}

void GrLida::on_AcercaD()
{
	frmAcercaD *AcercaD = new frmAcercaD();
	AcercaD->ui.label_4->setText( "v" + fGrl.stVersionGrl() );
	AcercaD->ui.lb_Dbx->setText( "Dosbox v" + fGrl.stVersionDbx() );
	AcercaD->ui.lb_Svm->setText( "ScummVM v" + fGrl.stVersionSvm() );
	AcercaD->ui.lb_Vdms->setText("VDMSound v" + fGrl.stVersionVdms() );// v2.0.4
	AcercaD->exec();
	delete AcercaD;
}

void GrLida::on_VerRating(bool visible)
{
	ui.twJuegos->setColumnHidden(2, !visible);
}

void GrLida::on_VerCarpeta_confdbx()
{
	fGrl.abrirDirectorio( stConfgDbxDir );
}

void GrLida::on_VerCarpeta_confvdms()
{
	fGrl.abrirDirectorio( stConfgVdmSDir );
}

void GrLida::on_VerCarpeta_thumbs()
{
	fGrl.abrirDirectorio( stHomeDir+"thumbs/" );
}

void GrLida::on_VerCarpeta_covers()
{
	fGrl.abrirDirectorio( stHomeDir+"covers/" );
}

void GrLida::on_VerCarpeta_iconos()
{
	fGrl.abrirDirectorio( stIconDir );
}

void GrLida::on_VerCarpeta_smiles()
{
	fGrl.abrirDirectorio( stHomeDir+"smiles/" );
}

void GrLida::on_VerCarpeta_themes()
{
	fGrl.abrirDirectorio( stHomeDir+"themes/" );
}

void GrLida::on_VerCarpeta_datos()
{
	fGrl.abrirDirectorio( stDatosDir );
}

void GrLida::on_VerCarpeta_scripts()
{
	fGrl.abrirDirectorio( stHomeDir+"scripts/" );
}

void GrLida::on_VerCarpeta_templates()
{
	fGrl.abrirDirectorio( stHomeDir+"templates/" );
}

void GrLida::on_VerCarpeta_idiomas()
{
	fGrl.abrirDirectorio( stHomeDir+"idiomas/" );
}

void GrLida::on_VerCarpeta_temp()
{
	fGrl.abrirDirectorio( stHomeDir+"temp/" );
}

void GrLida::on_VerCarpeta_home()
{
	fGrl.abrirDirectorio( stHomeDir );
}

void GrLida::ComprobarArchivosDatos(QString Version_GRL)
{
	QFile archivodatos;

	if( Version_GRL != fGrl.stVersionGrl() && Version_GRL < fGrl.stVersionGrl())
	{
		QDir dirFileDatos( stDatosDir);
		if( archivodatos.exists( stDatosDir+"xmldb.txt") )
			dirFileDatos.remove( stDatosDir+"xmldb.txt" );
	}

	CrearArchivoDato(":/datos/generos.txt"  , stDatosDir+"generos.txt");
	CrearArchivoDato(":/datos/companias.txt", stDatosDir+"companias.txt");
	CrearArchivoDato(":/datos/tema.txt"     , stDatosDir+"tema.txt");
	CrearArchivoDato(":/datos/formatos.txt" , stDatosDir+"formatos.txt");
	CrearArchivoDato(":/datos/fechas.txt"   , stDatosDir+"fechas.txt");
	CrearArchivoDato(":/datos/numdisc.txt"  , stDatosDir+"numdisc.txt");
	CrearArchivoDato(":/datos/sistemaop.txt", stDatosDir+"sistemaop.txt");
	CrearArchivoDato(":/datos/smiles.txt"   , stDatosDir+"smiles.txt");

	CrearArchivoDato(":/scripts/laisladelabandoware.js", stHomeDir+"scripts/laisladelabandoware.js");
	CrearArchivoDato(":/scripts/mobygames.js", stHomeDir+"scripts/mobygames.js");
}

void GrLida::CrearArchivoDato(QString origen, QString destino)
{
	QFile leerArchivo( origen );

	if( !leerArchivo.exists( destino ) )
	{
		if( leerArchivo.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
			QTextStream in( &leerArchivo );
			in.setCodec("UTF-8");
			QString line = in.readAll();
			leerArchivo.close();

			QFile *crearArchivo = new QFile( destino );
			if( crearArchivo->open(QIODevice::WriteOnly | QIODevice::Text) )
			{
				QTextStream out( crearArchivo );
				out.setCodec("UTF-8");
				out << line;
				out.flush();
			}
			crearArchivo->close();
			line.clear();

			delete crearArchivo;
		}
	}
}
