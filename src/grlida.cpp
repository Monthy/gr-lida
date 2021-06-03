/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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

#include <QDesktopWidget>
#include <QStyleFactory>
#include <QPushButton>
#include <QMessageBox>
#include <QCheckBox>

#include "grlida.h"
#include "grlida_acercad.h"
#include "grlida_info.h"
#include "grlida_wizard_dosbox.h"
#include "grlida_wizard_scummvm.h"
#include "grlida_wizard_vdmsound.h"
#include "grlida_addedit_juego.h"
#include "grlida_opciones.h"
#include "grlida_importar_juego.h"
//#include "grlida_exportar_juego.h"
#include "grlida_instalar_juego.h"
#include "grlida_cambiar_categoria.h"
#include "grlida_list_icon_cfg.h"
#include "grlida_update.h"
#include "ui_grlida.h"

GrLida::GrLida(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::GrLida)
{
	ui->setupUi(this);
	fGrl = new Funciones;

// Título de la aplicación
	setWindowTitle(tituloGrl() +" - v"+ fGrl->versionGrl());

	grlDir.Home       = fGrl->dirApp();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/";
	grlDir.Iconos     = grlDir.Home +"iconos/";
	grlDir.Idiomas    = grlDir.Home +"idiomas/";
	grlDir.Scripts    = grlDir.Home +"scripts/";
	grlDir.Smiles     = grlDir.Home +"smiles/";
	grlDir.Temp       = grlDir.Home +"temp/";
	grlDir.Templates  = grlDir.Home +"templates/";
	grlDir.Themes     = grlDir.Home +"themes/";

	grlCfg       = fGrl->cargarGRLConfig(grlDir.Home +"GR-lida.conf");
	lwIconCfg    = fGrl->cargarListWidgetIconConf();
	isImportar   = false;
	isOpenGrDap  = false;
	isOpenPdf    = false;
	isMountImage = false;
	isAutoMountImageExe = grlCfg.AutoMountImageExe;

// Comprobación de nuevas versiones.
	if (grlCfg.chkVersion)
		checkUpdateGrl();

	conectarBaseDatos();
	comprobarArchivosDatos(grlCfg.Version, grlCfg.IdiomaSelect);

	createWidgets();
	createToolBars();
	createStatusBar();
	createTrayIcon();
	createTbShortCut();
	cargarConfig();
	setTheme();

	cargarListaShortCut();
	cargarListaCategorias();
}

GrLida::~GrLida()
{
	if (ui->dockw_PictureFlow->isFloating())
		ui->dockw_PictureFlow->setFloating(false);

	delete grl_tv_delegate;
	delete grl_lv_delegate;
	delete grl_picflow;
	delete data_model;
	delete sql;
	delete fGrl;

	delete ui;
}

void GrLida::closeEvent(QCloseEvent *event)
{
	if (isTrayIcon && trayIcon->isVisible())
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
	if (event->type() == QEvent::KeyPress)
	{
		if (ui->menubar->isHidden())
		{
			QKeyEvent *key_Event = static_cast<QKeyEvent*>(event);

			switch (key_Event->modifiers())
			{
				case Qt::ControlModifier:
					switch (key_Event->key())
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
							ui->mnu_ver_statusbar->setChecked(grlCfg.Pnl_StatusBar);
							return true;
						break;
					//	case Qt::Key_F4:
					//		qDebug() << "cambio de CD...";
					//		return true;
					//	break;
					}
				break;
				case Qt::AltModifier:
					switch (key_Event->key())
					{
						case Qt::Key_S:
							emit on_mnu_ejecutar_setup_triggered();
							return true;
						break;
					}
				break;
				case Qt::ShiftModifier:
					switch (key_Event->key())
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
					}
				break;
				default:
					switch (key_Event->key())
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
							ui->mnu_ver_nav->setChecked(grlCfg.Pnl_Nav);
							return true;
						break;
						case Qt::Key_F3:
							emit on_mnu_ver_cover_mode_triggered(grlCfg.ver_IconMode ? false : true);
							ui->mnu_ver_cover_mode->setChecked(grlCfg.ver_IconMode);
							return true;
						break;
						case Qt::Key_F4:
							emit on_mnu_ver_pictureflow_triggered(grlCfg.Pnl_PictureFlow ? false : true);
							ui->mnu_ver_pictureflow->setChecked(grlCfg.Pnl_PictureFlow);
							return true;
						break;
						case Qt::Key_F5:
							emit on_mnu_ver_datos_triggered(grlCfg.Pnl_Datos ? false : true);
							ui->mnu_ver_datos->setChecked(grlCfg.Pnl_Datos);
							return true;
						break;
						case Qt::Key_F6:
							emit on_mnu_ver_archivos_triggered(grlCfg.Pnl_Files ? false : true);
							ui->mnu_ver_archivos->setChecked(grlCfg.Pnl_Files);
							return true;
						break;
						case Qt::Key_F7:
							emit on_mnu_ver_capturas_triggered(grlCfg.Pnl_Capturas ? false : true);
							ui->mnu_ver_capturas->setChecked(grlCfg.Pnl_Capturas);
							return true;
						break;
						case Qt::Key_F8:
							emit on_mnu_ver_pictureflow_to_center_triggered(grlCfg.PictureFlowToCenter ? false : true);
							ui->mnu_ver_pictureflow_to_center->setChecked(grlCfg.PictureFlowToCenter);
							return true;
						break;
						case Qt::Key_F9:
							emit on_mnu_ver_menubar_triggered(grlCfg.Pnl_Menubar ? false : true);
							ui->mnu_ver_menubar->setChecked(grlCfg.Pnl_Menubar);
							return true;
						break;
						case Qt::Key_F10:
							emit on_mnu_ver_toolbar_triggered(grlCfg.Pnl_Toolbar ? false : true);
							ui->mnu_ver_toolbar->setChecked(grlCfg.Pnl_Toolbar);
							return true;
						break;
						case Qt::Key_F11:
							emit on_mnu_ver_fullscreen_triggered(grlCfg.main_fullscreen ? false : true);
							ui->mnu_ver_fullscreen->setChecked(grlCfg.main_fullscreen);
							return true;
						break;
						case Qt::Key_F12:
							emit on_mnu_ver_ordenar_triggered(grlCfg.Pnl_Ordenar ? false : true);
							ui->mnu_ver_ordenar->setChecked(grlCfg.Pnl_Ordenar);
							return true;
						break;
					}
				break;
			}
		}
	}

	if (object == ui->tvJuegos || object == ui->lvJuegos || object == grl_picflow)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

			if (!grlCfg.ver_IconMode)
			{
				switch (keyEvent->key())
				{
					case Qt::Key_Up:
						itemBack();
						return true;
					break;
					case Qt::Key_Down:
						itemNext();
						return true;
					break;
				}
			}

			switch (keyEvent->key())
			{
				case Qt::Key_Enter:
				case Qt::Key_Space:
				case Qt::Key_Return:
					emit on_mnu_ejecutar_juego_triggered();
					return true;
				break;
				case Qt::Key_Left:
					itemBack();
					return true;
				break;
				case Qt::Key_Right:
					itemNext();
					return true;
				break;
			}
		}
	}

	if (object == ui->twFiles || object == ui->twUrls || object == ui->lwVideos || object == ui->lwCapturas || object == ui->lwImagenes)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

			switch (keyEvent->key())
			{
				case Qt::Key_Return:
					if (object == ui->twFiles)
						emit on_twFiles_itemDoubleClicked(ui->twFiles->currentItem(), 0);
					if (object == ui->twUrls)
						emit on_twUrls_itemDoubleClicked(ui->twUrls->currentItem(), 0);
					if (object == ui->lwVideos)
						emit on_lwVideos_itemDoubleClicked(ui->lwVideos->currentItem());
					if (object == ui->lwCapturas)
						emit on_lwCapturas_itemDoubleClicked(ui->lwCapturas->currentItem());
					if (object == ui->lwImagenes)
						emit on_lwImagenes_itemDoubleClicked(ui->lwImagenes->currentItem());
					return true;
				break;
			}
		}
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
	QString db_host = grlCfg.db_host;
	if (grlCfg.db_type == "QSQLITE")
		db_host = fGrl->getDirRelative(grlCfg.db_host);

	sql = new dbSql(grlCfg.db_type, grlCfg.db_server, db_host, grlCfg.db_name, grlCfg.db_username, grlCfg.db_password, grlCfg.db_port, grlDir.Home, fGrl->idioma(), fGrl->theme());

	if (!sql->dbisOpen())
		QMessageBox::critical(this, tr("Error al abrir la base de datos"), tr("No se ha podido establecer una conexión con la base de datos.\nEsta aplicación necesita soporte de SQLite. Mira la documentación de Qt SQL driver para más información.\n\nClick cancelar para salir."), QMessageBox::Cancel);
	else
		sql->crearTablas();
}

void GrLida::actualizaArchivoDatos(QList<stGrlDatos> datos, QString archivo, int num_col, QString sep)
{
	QFile file_out;
	file_out.setFileName(archivo);
	if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");
		const int listDatosSize = datos.size();
		for (int i = 0; i < listDatosSize; ++i)
		{
			switch (num_col)
			{
				case 2:
					out << datos.at(i).titulo << sep << datos.at(i).icono << endl;
				break;
				case 3:
					out << datos.at(i).titulo << sep << datos.at(i).key << sep << datos.at(i).icono << endl;
				break;
				case 4:
					out << datos.at(i).titulo << sep << datos.at(i).key << sep << datos.at(i).extra  << sep << datos.at(i).icono << endl;
				break;
			}
		}
		out.flush();
	}
	file_out.close();
}

// Comprueba si existen los archivos de datos.
void GrLida::comprobarArchivosDatos(QString version_grl, QString lng)
{
	bool esNuevaVersionGRlida = fGrl->version_compare(version_grl, fGrl->versionGrl()).isNew;
	QString filterImages      = grlCfg.FormatsImage.join(";");

	QStringList log_list;
	log_list << "version_grl old: "+ version_grl;
	log_list << "version_grl new: "+ fGrl->versionGrl();
	log_list << "lng            : "+ lng;
	log_list << "---------------------------------------------------------------" << "";

	log_list << "Theme por defecto";
	log_list << "Comprobando direcctorios:";

// Theme por defecto
	log_list << "defecto/              : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/"));
	log_list << "defecto/images/       : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/images/"));
	log_list << "defecto/img_svm/      : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img_svm/"));
	log_list << "defecto/img_svm/level/: "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img_svm/level/"));
	log_list << "defecto/img16/        : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img16/"));
	log_list << "defecto/img16/cat/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img16/cat/"));
	log_list << "defecto/img16/lng/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img16/lng/"));
	log_list << "defecto/img24/        : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img24/"));
	log_list << "defecto/img24/cat/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img24/cat/"));
	log_list << "defecto/img24/lng/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img24/lng/"));
	log_list << "defecto/img32/        : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img32/"));
	log_list << "defecto/img32/cat/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img32/cat/"));
	log_list << "defecto/img32/lng/    : "+ fGrl->boolToStr(fGrl->comprobarDirectorio(grlDir.Themes +"defecto/img32/lng/")) << "";

	fGrl->moverArchivos(grlDir.Themes +"defecto/img16_cat/", grlDir.Themes +"defecto/img16/cat/", filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->moverArchivos(grlDir.Themes +"defecto/img24_cat/", grlDir.Themes +"defecto/img24/cat/", filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->moverArchivos(grlDir.Themes +"defecto/img32_cat/", grlDir.Themes +"defecto/img32/cat/", filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->eliminarDirectorio(grlDir.Themes +"defecto/img16_cat/");
	fGrl->eliminarDirectorio(grlDir.Themes +"defecto/img24_cat/");
	fGrl->eliminarDirectorio(grlDir.Themes +"defecto/img32_cat/");

	log_list << "Copiando archivos de imagen.......";
	fGrl->copiarArchivos(":/img_svm/"      , grlDir.Themes +"defecto/img_svm/"      , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img_svm/level/", grlDir.Themes +"defecto/img_svm/level/", filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img16/"        , grlDir.Themes +"defecto/img16/"        , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img16/cat/"    , grlDir.Themes +"defecto/img16/cat/"    , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img16/lng/"    , grlDir.Themes +"defecto/img16/lng/"    , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img24/"        , grlDir.Themes +"defecto/img24/"        , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img24/cat/"    , grlDir.Themes +"defecto/img24/cat/"    , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img24/lng/"    , grlDir.Themes +"defecto/img24/lng/"    , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img32/"        , grlDir.Themes +"defecto/img32/"        , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img32/cat/"    , grlDir.Themes +"defecto/img32/cat/"    , filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img32/lng/"    , grlDir.Themes +"defecto/img32/lng/"    , filterImages, ";", true, esNuevaVersionGRlida);

	fGrl->copiarArchivo(":/images/juego_eliminado.png"           , grlDir.Themes +"defecto/images/juego_eliminado.png"           , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/juego_sin_imagen.png"          , grlDir.Themes +"defecto/images/juego_sin_imagen.png"          , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/list_bg.jpg"                   , grlDir.Themes +"defecto/images/list_bg.jpg"                   , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/list_cover_title_bg.png"       , grlDir.Themes +"defecto/images/list_cover_title_bg.png"       , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/list_cover_title_bg_select.png", grlDir.Themes +"defecto/images/list_cover_title_bg_select.png", true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/list_cover_top.png"            , grlDir.Themes +"defecto/images/list_cover_top.png"            , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/images/list_cover_top_select.png"     , grlDir.Themes +"defecto/images/list_cover_top_select.png"     , true, esNuevaVersionGRlida);

	fGrl->copiarArchivo(":/StyleSheet.qss"        , grlDir.Themes +"defecto/StyleSheet.qss"        , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/StyleSheetList.qss"    , grlDir.Themes +"defecto/StyleSheetList.qss"    , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info.css"          , grlDir.Themes +"defecto/tpl_info.css"          , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info.html"         , grlDir.Themes +"defecto/tpl_info.html"         , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info_juego.css"    , grlDir.Themes +"defecto/tpl_info_juego.css"    , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info_juego.html"   , grlDir.Themes +"defecto/tpl_info_juego.html"   , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info_juego_no.css" , grlDir.Themes +"defecto/tpl_info_juego_no.css" , true, esNuevaVersionGRlida);
	fGrl->copiarArchivo(":/tpl_info_juego_no.html", grlDir.Themes +"defecto/tpl_info_juego_no.html", true, esNuevaVersionGRlida);

// Smiles e Iconos
	fGrl->copiarArchivos(":/smiles/"   , grlDir.Smiles, filterImages, ";", true, esNuevaVersionGRlida);
	fGrl->copiarArchivos(":/img24/cat/", grlDir.Iconos, filterImages, ";", true, esNuevaVersionGRlida);

// Scripts
	fGrl->copiarArchivos(":/scripts/", grlDir.Scripts, "*.js", ";", true, esNuevaVersionGRlida);
	log_list << "---------------------------------------------------------------" << "";

// Emuladores DOSBox
	QList<stGrlDatos> dbx_list_tmp = fGrl->cargaListaDatos(grlDir.Datos +"dbx_list.txt", 6, "|");
	if (dbx_list_tmp.isEmpty() && grlCfg.DOSBoxDisp)
	{
		stGrlDatos dbx_datos;
		dbx_datos.titulo  = "DOSBox";
		dbx_datos.icono   = "cat/dosbox.png";
		dbx_datos.extra   = fGrl->setDirRelative(grlCfg.DirDOSBox);
		dbx_datos.issvn   = fGrl->boolToStr(grlCfg.DOSBoxSVN, true);
		dbx_datos.version = grlCfg.VersionDBx;
		dbx_datos.key     = "dosbox";

		QString txt_dbx_datos_out = dbx_datos.titulo +"|"+ dbx_datos.key +"|"+ dbx_datos.extra +"|"+ dbx_datos.icono +"|"+ dbx_datos.version +"|"+ dbx_datos.issvn +"\n";

		fGrl->guardarArchivo(grlDir.Datos +"dbx_list.txt", txt_dbx_datos_out, "UTF-8");

		log_list << "Configuración:";
		log_list << "------------------------";
		log_list << "DOSBox disp   : "+ fGrl->boolToStr(grlCfg.DOSBoxDisp);
		log_list << "DOSBox path   : "+ dbx_datos.extra;
		log_list << "DOSBox version: "+ dbx_datos.version;
		log_list << "DOSBox SVN    : "+ dbx_datos.issvn << "";
		log_list << "---------------------------------------------------------------" << "";
	}

// Comprobar columna dbgrl_mnu_nav
	bool existe_mostrar_en_menu_nav = sql->comprobarColumnaTabla("dbgrl_mnu_nav", "mostrar");
	if (!existe_mostrar_en_menu_nav)
	{
		sql->crearTablaMnuNav(true);
		QSqlQuery menu_nav_query(sql->getSqlDB());
		menu_nav_query.exec("INSERT INTO dbgrl_mnu_nav_temp SELECT id, titulo, col_value , col_name, sql_query, archivo, img, orden, show, expanded FROM dbgrl_mnu_nav;");
		sql->eliminarTabla("dbgrl_mnu_nav");
		sql->renombrarTabla("dbgrl_mnu_nav_temp", "dbgrl_mnu_nav");
	}
	log_list << "---------------------------------------------------------------" << "";

	if (esNuevaVersionGRlida)
	{
		// Eliminamos las fichas cache para generarlas de nuevo.
		QFileInfoList list_ficha_html = fGrl->getListFiles(grlDir.DatosDbGrl, QString("*.html").split(";"), true);
		fGrl->eliminarArchivos(list_ficha_html);

// esNuevaVersionGRlida -----------------------------------------------------------------------------------------
	// Actualización de archivos de Datos.
		QStringList datos_txt_2col, datos_txt_3col, datos_txt_4col;
		datos_txt_2col.clear();
		datos_txt_2col << "companias.txt" << "sistemas.txt" << "dbx_cycles.txt" << "dbx_hwscale.txt"
					   << "dbx_memsize.txt" << "dbx_midi_device.txt" << "dbx_mpu401.txt" << "dbx_resolution.txt"
					   << "dbx_resolution_svn.txt" << "dbx_sb_oplemu.txt" << "dbx_sensitivity.txt" << "formatos.txt"
					   << "generos.txt" << "numdisc.txt" << "perspectivas.txt" << "temas.txt" << "smiles.txt";
		datos_txt_3col.clear();
		datos_txt_3col << "dbx_cpu_core.txt" << "dbx_cpu_type.txt" << "dbx_cpu_type_svn.txt" << "dbx_joystick.txt"
					   << "dbx_machine.txt" << "dbx_machine_svn.txt" << "dbx_output.txt" << "dbx_output_svn.txt"
					   << "dbx_sb_oplmode.txt" << "dbx_sbtype.txt" << "dbx_scaler.txt" << "dbx_scaler_svn.txt"
					   << "svm_gfxmode.txt" << "svm_music_driver.txt" << "svm_platform.txt" << "svm_render_mode.txt"
					   << "edad_recomendada.txt" << "idiomas.txt" << "svm_idioma.txt";
		datos_txt_4col.clear();
		datos_txt_4col << "dbx_keyboardlayout.txt";

		fGrl->comprobarDirectorio(grlDir.Datos);
		fGrl->comprobarDirectorio(grlDir.Datos + lng);

		QStringList no_admitidos;
		no_admitidos.clear();
//		no_admitidos << "svm_lista.txt" << "dbx_list.txt" <<  "emu_list.txt" << "grupos.txt" << "fechas.txt";
		no_admitidos << "svm_lista.txt" << "dbx_list.txt" <<  "emu_list.txt" << "virtual_driver_list.txt";

		QString sLng = sql->getArchivoIsLng("temas.txt");
		fGrl->moverArchivo(grlDir.Datos +"edad_recomendada.txt", grlDir.Datos + sLng +"edad_recomendada.txt");
		fGrl->moverArchivo(grlDir.Datos +"formatos.txt"        , grlDir.Datos + sLng +"formatos.txt");
		fGrl->moverArchivo(grlDir.Datos +"generos.txt"         , grlDir.Datos + sLng +"generos.txt");
		fGrl->moverArchivo(grlDir.Datos +"idiomas.txt"         , grlDir.Datos + sLng +"idiomas.txt");
		fGrl->moverArchivo(grlDir.Datos +"numdisc.txt"         , grlDir.Datos + sLng +"numdisc.txt");
		fGrl->moverArchivo(grlDir.Datos +"perspectivas.txt"    , grlDir.Datos + sLng +"perspectivas.txt");
		fGrl->moverArchivo(grlDir.Datos +"svm_idioma.txt"      , grlDir.Datos + sLng +"svm_idioma.txt");
		fGrl->moverArchivo(grlDir.Datos +"tema.txt"            , grlDir.Datos + sLng +"temas.txt");

		QList<stGrlDatos> dat_grl, dat_user, dat_final;
		QFileInfoList list_files = fGrl->getListFiles(grlDir.Datos, QString("*.txt").split(";"), true);

	// Actualización de la Base de Datos.
		stVersion version = fGrl->version_compare("0.11.1", version_grl);

		if (version.info == esMenor)
		{
			grlCfg.DirBaseGames = fGrl->setDirRelative(grlCfg.DirBaseGames, "DosGames");
			grlCfg.DirScummVM   = fGrl->setDirRelative(grlCfg.DirScummVM);
		}

		const int listSize = list_files.size();
		for (int j = 0; j < listSize; ++j)
		{
			QString fileName = list_files.at(j).fileName();

			if (!no_admitidos.contains(fileName))
			{
//				QString filePath = list_files.at(j).absoluteFilePath();
				int num_col = 2;
				QString sep = "|";
				sLng        = sql->getArchivoIsLng(fileName);

				if (datos_txt_3col.contains(fileName))
					num_col = 3;
				if (datos_txt_4col.contains(fileName))
					num_col = 4;
				if (fileName == "smiles.txt")
					sep = "\",\"";

				dat_grl.clear();
				dat_grl  = fGrl->cargaListaDatos(":/datos/"+ sLng + fileName, num_col, sep);
				dat_user.clear();
				dat_user = fGrl->cargaListaDatos(grlDir.Datos + sLng + fileName, num_col, sep);

				const int numDatUserSize = dat_user.size();
				for (int u = 0; u < numDatUserSize; ++u)
				{
					QString ext = fGrl->getInfoFile(dat_user.at(u).icono).Ext;
					if (ext.endsWith("."))
						dat_user[u].icono = dat_user.at(u).icono +".png";

					if (fileName == "idiomas.txt" || fileName == "svm_idioma.txt")
					{
						if (fileName == "svm_idioma.txt" && dat_user.at(u).key == "pt")
							dat_user[u].key = "br";
						if (dat_user.at(u).titulo.contains(""))
							dat_user[u].titulo.replace("Bokmal", "Bokmål");
						if (dat_user.at(u).icono == "zh-cn.png")
							dat_user[u].icono = "cn.png";
						if (dat_user.at(u).icono == "zh.png")
							dat_user[u].icono = "tw.png";
						if (dat_user.at(u).icono == "hb.png")
							dat_user[u].icono = "il.png";
						if (dat_user.at(u).icono == "nb.png")
							dat_user[u].icono = "no.png";
						if (!dat_user.at(u).icono.startsWith("lng/"))
							dat_user[u].icono = "lng/"+ dat_user.at(u).icono;
					}

					int numDatGrlSize = dat_grl.size()-1;
					for (int g = numDatGrlSize;  g >= 0; --g)
					{
						if (fGrl->compararStGrlDatos(dat_user.at(u), dat_grl.at(g)))
							dat_grl.removeAt(g);
					}
				}

				const int numFinalDatGrlSize = dat_grl.size();
				if (numFinalDatGrlSize > 0)
					dat_user << dat_grl;

				actualizaArchivoDatos(dat_user, grlDir.Datos + sLng + fileName, num_col, sep);
			}
		}

		QList<stGrlCats> tmp_cat_list = sql->getCategorias();
		const int listTmpCatSize = tmp_cat_list.size();
		for (int c = 0; c < listTmpCatSize; ++c)
		{
			log_list << "Categoria: "+ tmp_cat_list.at(c).tabla +", id: "+ tmp_cat_list.at(c).id;
			log_list << "------------------------";

			QString game_dir_cat_tabla = version.info == esMenor ? "" : tmp_cat_list.at(c).tabla +"/";

			QSqlQuery tmp_query(sql->getSqlDB());
			tmp_query.exec("SELECT idgrl, titulo, tipo_emu FROM "+ tmp_cat_list.at(c).tabla +";");
			if (tmp_query.first())
			{
				do {
					QString old_titulo_guiones, old_game_dir, old_game_dir_capturas, new_dir_game_base;
					QString old_file_thumbs, old_file_cover_front, old_file_cover_back;
					QString new_file_thumbs, new_file_cover_front, new_file_cover_back;

					stDatosJuego datos   = sql->show_Datos(tmp_cat_list.at(c).tabla, tmp_query.record().value("idgrl").toString());
					datos.titulo_guiones = sql->removeAccents(datos.titulo);
					datos.game_dir       = "id-"+ datos.idgrl +"_"+ datos.titulo_guiones +"_"+ datos.tipo_emu;
					datos.path_exe       = datos.path_exe.replace("\\", "/");
					datos.path_setup     = datos.path_setup.replace("\\", "/");
					datos.path_capturas  = datos.path_capturas.replace("\\", "/");

					// 'dd/MM/yyyy HH:mm:ss' a formato unix
					datos.fecha = sql->setTime(datos.fecha);

					if (datos.icono.toLower() == "datos")
						datos.icono = "cat/datos.png";
					if (datos.icono.toLower() == "dosbox")
						datos.icono = "cat/dosbox.png";
					if (datos.icono.toLower() == "scummvm")
						datos.icono = "cat/scummvm.png";
					if (datos.icono.toLower() == "vdmsound")
						datos.icono = "cat/vdmsound.png";
					if (datos.icono.toLower() == "pc")
						datos.icono = "cat/pc.png";
					if (datos.icono.toLower() == "sinimg")
						datos.icono = "cat/sinimg.png";

					log_list << "	titulo               : "+ datos.titulo;
					log_list << "	titulo_guiones       : "+ datos.titulo_guiones;
					log_list << "	game_dir             : "+ datos.game_dir << "";

					old_titulo_guiones    = fGrl->eliminar_caracteres(datos.titulo);
					old_game_dir          = "id-"+ datos.idgrl +"_"+ old_titulo_guiones +"_"+ datos.tipo_emu;
					old_game_dir_capturas = grlDir.Home +"capturas/"+ game_dir_cat_tabla + old_game_dir +"/";

					log_list << "	old_titulo_guiones   : "+ old_titulo_guiones;
					log_list << "	old_game_dir         : "+ old_game_dir;
					log_list << "	old_game_dir_capturas: "+ old_game_dir_capturas << "";

					new_dir_game_base = grlDir.Home +"datosdb/"+ tmp_cat_list.at(c).tabla +"/"+ datos.game_dir +"/";

					log_list << "	new_dir_game_base    : "+ new_dir_game_base << "";

				// Creación de la nueva estructura de directorios.
					fGrl->comprobarDirectorio(new_dir_game_base);
					fGrl->comprobarDirectorio(new_dir_game_base +"archivos/");
					fGrl->comprobarDirectorio(new_dir_game_base +"caja/");
					fGrl->comprobarDirectorio(new_dir_game_base +"capturas/");
					fGrl->comprobarDirectorio(new_dir_game_base +"imagenes/");
					fGrl->comprobarDirectorio(new_dir_game_base +"imagenes/small/");
					fGrl->comprobarDirectorio(new_dir_game_base +"sonidos/");
					fGrl->comprobarDirectorio(new_dir_game_base +"videos/");

				// Comprobamos las caratula frontal.
					old_file_cover_front = grlDir.Home +"covers/"+ game_dir_cat_tabla + datos.cover_front;
					if (!datos.cover_front.isEmpty() && QFile::exists(old_file_cover_front))
					{
						datos.cover_front = "cover_front"+ fGrl->getInfoFile(old_file_cover_front).Ext;
						new_file_cover_front = new_dir_game_base +"caja/"+ datos.cover_front;
						fGrl->moverArchivo(old_file_cover_front, new_file_cover_front, true, true);
					}

				// Comprobamos las caratula trasera.
					old_file_cover_back = grlDir.Home +"covers/"+ game_dir_cat_tabla + datos.cover_back;
					if (!datos.cover_back.isEmpty() && QFile::exists(old_file_cover_back))
					{
						datos.cover_back = "cover_back"+ fGrl->getInfoFile(old_file_cover_back).Ext;
						new_file_cover_back = new_dir_game_base +"caja/"+ datos.cover_back;
						fGrl->moverArchivo(old_file_cover_back, new_file_cover_back, true, true);
					}

				// Comprobamos el thumbs.
					old_file_thumbs = grlDir.Home +"thumbs/"+ game_dir_cat_tabla + datos.thumbs;
					if (!datos.thumbs.isEmpty() && QFile::exists(old_file_thumbs))
					{
						datos.thumbs = "thumbs"+ fGrl->getInfoFile(old_file_thumbs).Ext;
						new_file_thumbs = new_dir_game_base + datos.thumbs;
						fGrl->moverArchivo(old_file_thumbs, new_file_thumbs, true, true);
					}

					log_list << "	old_file_thumbs      : "+ old_file_thumbs;
					log_list << "	old_file_cover_front : "+ old_file_cover_front;
					log_list << "	old_file_cover_back  : "+ old_file_cover_back << "";
					log_list << "	new_file_thumbs      : "+ new_file_thumbs;
					log_list << "	new_file_cover_front : "+ new_file_cover_front;
					log_list << "	new_file_cover_back  : "+ new_file_cover_back << "";

				// Actualizamos los Datos del juego.
					sql->actualizaDatos(tmp_cat_list.at(c).tabla, datos);

				// Movemos las capturas de DOSBox, etc...
					QFileInfoList lista_imagenes = fGrl->getListFiles(old_game_dir_capturas, grlCfg.FormatsImage);
					QFileInfoList lista_videos   = fGrl->getListFiles(old_game_dir_capturas, grlCfg.FormatsVideo);
					QFileInfoList lista_sonidos  = fGrl->getListFiles(old_game_dir_capturas, grlCfg.FormatsMusic);

					log_list << "	Capturas:";
					const int listImagenesSize = lista_imagenes.size();
					for (int i = 0; i < listImagenesSize; ++i)
					{
						log_list << "		old: "+ old_game_dir_capturas + lista_imagenes.at(i).fileName();
						log_list << "		new: "+ new_dir_game_base +"capturas/"+ lista_imagenes.at(i).fileName();
						log_list << "		------";
						fGrl->moverArchivo(old_game_dir_capturas + lista_imagenes.at(i).fileName(), new_dir_game_base +"/capturas/"+ lista_imagenes.at(i).fileName(), true, true);
					} // fin bucle lista_imagenes

					log_list << "" << "	Videos:";
					const int listVideosSize = lista_videos.size();
					for (int i = 0; i < listVideosSize; ++i)
					{
						log_list << "		old: "+ old_game_dir_capturas + lista_videos.at(i).fileName();
						log_list << "		new: "+ new_dir_game_base +"capturas/"+ lista_videos.at(i).fileName();
						log_list << "		------";
						fGrl->moverArchivo(old_game_dir_capturas + lista_videos.at(i).fileName(), new_dir_game_base +"/capturas/"+ lista_videos.at(i).fileName(), true, true);
					} // fin bucle lista_videos

					log_list << "" << "	Sonidos:";
					const int listSonidosSize = lista_sonidos.size();
					for (int i = 0; i < listSonidosSize; ++i)
					{
						log_list << "		old: "+ old_game_dir_capturas + lista_sonidos.at(i).fileName();
						log_list << "		new: "+ new_dir_game_base +"capturas/"+ lista_sonidos.at(i).fileName();
						log_list << "		------";
						fGrl->moverArchivo(old_game_dir_capturas + lista_sonidos.at(i).fileName(), new_dir_game_base +"/capturas/"+ lista_sonidos.at(i).fileName(), true, true);
					} // fin bucle lista_sonidos

					log_list << "" << "	Eliminando carpeta capturas del juegos en "+ tmp_cat_list.at(c).tabla +":";
					log_list << "		old_game_dir_capturas: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(old_game_dir_capturas)) +" - "+ old_game_dir_capturas << "";

					if (datos.tipo_emu.toLower() == "dosbox")
					{
						log_list << "	Tiene DOSBox ...";

						stConfigDOSBox dosbox = sql->showConfg_DOSBox(datos.idgrl, tmp_cat_list.at(c).id);

						QString dosbox_path_exe   = fGrl->getRelative(dosbox.path_exe, "DosGames");
						QString dosbox_path_setup = fGrl->getRelative(dosbox.path_setup, "DosGames");

						dosbox.path_exe   = fGrl->setDirRelative(dosbox_path_exe, "DosGames");
						dosbox.path_setup = fGrl->setDirRelative(dosbox_path_setup, "DosGames");

						sql->actualizaDbx(dosbox);

						QList<stConfigDOSBoxMount> listMount = sql->showConfg_DOSBoxMount(dosbox.id);
						const int listMountSize = listMount.size();
						for (int i = 0; i < listMountSize; ++i)
						{
							QString listMount_path = fGrl->getRelative(listMount.at(i).path, "DosGames");

							listMount[i].path = fGrl->setDirRelative(listMount_path, "DosGames");

							sql->actualizaMontajeDbx(listMount.at(i));
						} // fin bucle listMount

						fGrl->crearArchivoConfigDbx(datos, dosbox, listMount, grlDir.Home, tmp_cat_list.at(c).tabla, new_dir_game_base +"dosbox.conf");
						fGrl->crearArchivoConfigDbx(datos, dosbox, listMount, grlDir.Home, tmp_cat_list.at(c).tabla, new_dir_game_base +"dosbox-setup.conf", true);

						log_list << "		Actualizando y creando la nueva configuración de DOSBox ...";

						QSqlQuery tmp_query_dbx(sql->getSqlDB());
						tmp_query_dbx.exec("SELECT path_conf FROM dbgrl_emu_dosbox WHERE id="+ dosbox.id +" LIMIT 0,1;");
						if (tmp_query_dbx.first())
						{
							QString tmp_dbx_path_conf = tmp_query_dbx.record().value("path_conf").toString();
							fGrl->eliminarArchivo(grlDir.Home +"confdbx/"+ game_dir_cat_tabla + tmp_dbx_path_conf);
							log_list << "		Eliminando archivo configuración DOSBox: "+ tmp_dbx_path_conf +" en "+ game_dir_cat_tabla << "";
						}
					} // fin tipo_emu DOSBox.

					if (datos.tipo_emu.toLower() == "scummvm")
					{
						log_list << "	Tiene ScummVM ...";
						stConfigScummVM scummvm = sql->showConfg_ScummVM(datos.idgrl, tmp_cat_list.at(c).id);

						QString scummvm_path       = fGrl->getRelative(scummvm.path, "DosGames");
						QString scummvm_path_extra = fGrl->getRelative(scummvm.path_extra, "DosGames");
						QString scummvm_path_save  = fGrl->getRelative(scummvm.path_save, "DosGames");

						scummvm.path        = fGrl->setDirRelative(scummvm_path, "DosGames");
						scummvm.path_extra  = fGrl->setDirRelative(scummvm_path_extra, "DosGames");
						scummvm.path_save   = fGrl->setDirRelative(scummvm_path_save, "DosGames");
						scummvm.emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;

						log_list << "		Actualizando y creando la nueva configuración de ScummVM ..." << "";

						fGrl->creaIniScummVM(scummvm, new_dir_game_base +"scummvm.ini");

						sql->actualizaSvm(scummvm);
					} // fin tipo_emu ScummVM.

					if (datos.tipo_emu.toLower() == "vdmsound")
					{
						log_list << "	Tiene VDMSound ...";

						stConfigVDMSound vdmsound = sql->showConfg_VDMSound(datos.idgrl, tmp_cat_list.at(c).id);
						QString vdmsound_path_exe = fGrl->getRelative(vdmsound.path_exe, "DosGames");
						vdmsound.path_exe = fGrl->setDirRelative(vdmsound_path_exe, "DosGames");

						sql->actualizaVdms(vdmsound);

						fGrl->crearArchivoConfigVdmS(vdmsound, new_dir_game_base +"vdmsound.vlp");

						log_list << "		Actualizando y creando la nueva configuración de VDMSound ...";

						QSqlQuery tmp_query_vdms(sql->getSqlDB());
						tmp_query_vdms.exec("SELECT path_conf FROM dbgrl_emu_vdmsound WHERE id="+ vdmsound.id +" LIMIT 0,1;");
						if (tmp_query_vdms.first())
						{
							QString tmp_vdms_path_conf = tmp_query_vdms.record().value("path_conf").toString();
							fGrl->eliminarArchivo(grlDir.Home +"confvdms/"+ game_dir_cat_tabla + tmp_vdms_path_conf);
							log_list << "		Eliminando archivo configuración VDMSound "+ tmp_vdms_path_conf +" en "+ game_dir_cat_tabla << "";
						}

						fGrl->crearArchivoConfigVdmS(vdmsound, new_dir_game_base +"vdmsound.vlp");

					} // fin tipo_emu VDMSound.

					sql->comit();
					log_list << "	------------------------";
				} while (tmp_query.next()); // fin bucle juegos en la categoria.

				if (!game_dir_cat_tabla.isEmpty())
				{
					log_list << "" << "Eliminando carpeta "+ game_dir_cat_tabla +" en capturas:";
					log_list << "old_dir_capturas: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"capturas/"+ tmp_cat_list.at(c).tabla +"/")) +" - "+ grlDir.Home +"capturas/"+ tmp_cat_list.at(c).tabla +"/";
					log_list << "old_dir_covers  : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"covers/"+ tmp_cat_list.at(c).tabla +"/")) +" - "+ grlDir.Home +"covers/"+ tmp_cat_list.at(c).tabla +"/";
					log_list << "old_dir_thumbs  : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"thumbs/"+ tmp_cat_list.at(c).tabla +"/")) +" - "+ grlDir.Home +"thumbs/"+ tmp_cat_list.at(c).tabla +"/";
					log_list << "old_dir_confdbx : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"confdbx/"+ tmp_cat_list.at(c).tabla +"/")) +" - "+ grlDir.Home +"confdbx/"+ tmp_cat_list.at(c).tabla +"/";
					log_list << "old_dir_confvdms: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"confvdms/"+ tmp_cat_list.at(c).tabla +"/")) +" - "+ grlDir.Home +"confvdms/"+ tmp_cat_list.at(c).tabla +"/";
					log_list << "---------------------------------------------------------------";
				}
			} // fin SQL SELECT categoria.

			log_list << "" << "Eliminando carpeta:";
			log_list << "capturas: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"capturas/")) +" - "+ grlDir.Home +"capturas/";
			log_list << "covers  : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"covers/")) +" - "+ grlDir.Home +"covers/";
			log_list << "thumbs  : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"thumbs/")) +" - "+ grlDir.Home +"thumbs/";
			log_list << "confdbx : "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"confdbx/")) +" - "+ grlDir.Home +"confdbx/";
			log_list << "confvdms: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"confvdms/")) +" - "+ grlDir.Home +"confvdms/" << "";
			log_list << "---------------------------------------------------------------" << "";
		} // fin bucle categorias.

		if (version.info == esMenor)
		{
			log_list << "Eliminando imagenes de "+ grlDir.Home +"thumbs_list/";
			fGrl->eliminarArchivos(fGrl->getListFiles(grlDir.Home +"thumbs_list/", grlCfg.FormatsImage));

			log_list << "" << "Eliminando carpeta:";
			log_list << "	thumbs_list: "+ fGrl->boolToStr(fGrl->eliminarDirectorio(grlDir.Home +"thumbs_list/")) +" - "+ grlDir.Home +"thumbs_list/" << "";
			log_list << "---------------------------------------------------------------";
		}
// --------------------------------------------------------------------------------------------------------------
	} else { // else version

	} // fin version

	QString log_out = "";
	for (int i = 0; i < log_list.size(); ++i)
	{
		QString item_log = log_list.at(i);
		log_out.append(item_log +"\n");
//		if (!item_log.isEmpty())
//			qDebug() << log_list.at(i);
	}

//	fGrl->guardarArchivo(grlDir.Home +"comprobarArchivosDatos-"+ fGrl->getTime() +".log", log_out, "UTF-8");
//	if (!QFile::exists(grlDir.Home +"comprobarArchivosDatos.log"))
//		fGrl->guardarArchivo(grlDir.Home +"comprobarArchivosDatos.log", log_out, "UTF-8");

/*	qDebug() << "Datos Juego --------------------------------------------------------------";
	qDebug() << "idgrl           : " << datos.idgrl;
// --
	qDebug() << "icono           : " << datos.icono;
	qDebug() << "titulo          : " << datos.titulo;
	qDebug() << "titulo_guiones  : " << datos.titulo_guiones;
	qDebug() << "subtitulo       : " << datos.subtitulo;
	qDebug() << "genero          : " << datos.genero;
	qDebug() << "compania        : " << datos.compania;
	qDebug() << "desarrollador   : " << datos.desarrollador;
	qDebug() << "tema            : " << datos.tema;
	qDebug() << "grupo           : " << datos.grupo;
	qDebug() << "perspectiva     : " << datos.perspectiva;
	qDebug() << "idioma          : " << datos.idioma;
	qDebug() << "idioma_voces    : " << datos.idioma_voces;
	qDebug() << "formato         : " << datos.formato;
	qDebug() << "anno            : " << datos.anno;
	qDebug() << "numdisc         : " << datos.numdisc;
	qDebug() << "sistemaop       : " << datos.sistemaop;
	qDebug() << "tamano          : " << datos.tamano;
	qDebug() << "graficos        : " << datos.graficos;
	qDebug() << "sonido          : " << datos.sonido;
	qDebug() << "jugabilidad     : " << datos.jugabilidad;
	qDebug() << "original        : " << datos.original;
	qDebug() << "estado          : " << datos.estado;
	qDebug() << "thumbs          : " << datos.thumbs;
	qDebug() << "cover_front     : " << datos.cover_front;
	qDebug() << "cover_back      : " << datos.cover_back;
	qDebug() << "cover_left      : " << datos.cover_left;
	qDebug() << "cover_right     : " << datos.cover_right;
	qDebug() << "cover_top       : " << datos.cover_top;
	qDebug() << "cover_bottom    : " << datos.cover_bottom;
	qDebug() << "fecha           : " << datos.fecha;
	qDebug() << "tipo_emu        : " << datos.tipo_emu;
	qDebug() << "comentario      : " << datos.comentario;
	qDebug() << "favorito        : " << datos.favorito;
	qDebug() << "rating          : " << datos.rating;
	qDebug() << "edad_recomendada: " << datos.edad_recomendada;
	qDebug() << "usuario         : " << datos.usuario;
	qDebug() << "path_exe        : " << datos.path_exe;
	qDebug() << "parametros_exe  : " << datos.parametros_exe;
	qDebug() << "path_setup      : " << datos.path_setup;
	qDebug() << "parametros_setup: " << datos.parametros_setup;
// --
	qDebug() << "path_capturas   : " << datos.path_capturas;
//	qDebug() << "thumbs_new      : " << datos.thumbs_new;
	qDebug() << "thumbs_url      : " << datos.thumbs_url;
//	qDebug() << "cover_front_new : " << datos.cover_front_new
	qDebug() << "cover_front_url : " << datos.cover_front_url;
//	qDebug() << "cover_back_new  : " << datos.cover_back_new;
	qDebug() << "cover_back_url  : " << datos.cover_back_url;
//	qDebug() << "cover_left_new  : " << datos.cover_left_new;
	qDebug() << "cover_left_url  : " << datos.cover_left_url;
//	qDebug() << "cover_right_new : " << datos.cover_right_new;
	qDebug() << "cover_right_url : " << datos.cover_right_url;
//	qDebug() << "cover_top_new   : " << datos.cover_top_new;
	qDebug() << "cover_top_url   : " << datos.cover_top_url;
//	qDebug() << "cover_bottom_new: " << datos.cover_bottom_new;
	qDebug() << "cover_bottom_url: " << datos.cover_bottom_url;
	qDebug() << "game_dir        : " << datos.game_dir;
*/
}

// Crea distintos widgets
void GrLida::createWidgets()
{
	ui->tvJuegos->setAutoScroll(false);

	QList<stGrlDatos> data_model_list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	QHash<QString, QString> data_model_emu_list;

	data_model_emu_list.insert("datos", "Datos");
	data_model_emu_list.insert("dosbox", "DOSBox");
	data_model_emu_list.insert("scummvm", "ScummVM");
	data_model_emu_list.insert("vdmsound", "VDMSound");

	foreach (const stGrlDatos &emu, data_model_list_emu)
		data_model_emu_list.insert(emu.key, emu.titulo);

	data_model      = new GrlTreeViewModel(data_model_emu_list, this);
	grl_lv_delegate = new GrlListViewDelegate(ui->lvJuegos);
	grl_tv_delegate = new GrlTreeViewDelegate(ui->tvJuegos);
	connect(grl_lv_delegate, SIGNAL(updateCol(const QModelIndex, int)), this, SLOT(updateCol(const QModelIndex, int)));
	connect(grl_tv_delegate, SIGNAL(updateCol(const QModelIndex, int)), this, SLOT(updateCol(const QModelIndex, int)));

	ui->txtInfo->installEventFilter(this);
	ui->txtInfo->setFrameShape(QFrame::NoFrame);
	ui->txtInfo->document()->setDocumentMargin(0);

	grlProcess = new QProcess(this);
	connect(grlProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(fin_Proceso(int, QProcess::ExitStatus)));
	connect(grlProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error_Proceso(QProcess::ProcessError)));

	grlProcessMount = new QProcess(this);
	connect(grlProcessMount, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(fin_ProcesoMount(int, QProcess::ExitStatus)));
	connect(grlProcessMount, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error_ProcesoMount(QProcess::ProcessError)));

	grl_picflow = new GrlPicFlow(this);
	grl_picflow->setFocusPolicy(Qt::StrongFocus);//NoFocus
	grl_picflow->setFocus(Qt::OtherFocusReason);
	grl_picflow->installEventFilter(this);
	grl_picflow->setContextMenuPolicy(Qt::CustomContextMenu);
	grl_picflow->setPicFlowType(grlCfg.PicFlowType);
	grl_picflow->setBackgroundColor(fGrl->getColor(grlCfg.PicFlowBgColor));
	grl_picflow->setPicFlowFont(grlCfg.PicFlowFontFamily, grlCfg.PicFlowFontSize, grlCfg.PicFlowFontBold, grlCfg.PicFlowFontItalic);
	grl_picflow->setFontColor(fGrl->getColor(grlCfg.PicFlowFontColor));
	grl_picflow->setMinimumHeight(grlCfg.PicFlowMinHeight);

	grl_picflow->setShowTitle(grlCfg.PicFlowShowTitle);
	grl_picflow->setSlideTitlePos(fGrl->getPosition(grlCfg.PicFlowTitlePos));
	grl_picflow->setSlideTitleMargin(grlCfg.PicFlowTitleMargin);

	grl_picflow->setShowTitleIcon(grlCfg.PicFlowShowTitleIcon);
	grl_picflow->setSlideTitleIconPos(grlCfg.PicFlowTitleIconPos);
	grl_picflow->setSlideTitleIconMargin(grlCfg.PicFlowTitleIconMargin);

	grl_picflow->setShowCaption(grlCfg.PicFlowShowCaption);
	grl_picflow->setSlideCaptionPos(fGrl->getPosition(grlCfg.PicFlowCaptionPos));
	grl_picflow->setSlideCaptionMargin(grlCfg.PicFlowCaptionMargin);

	grl_picflow->setShowNumber(grlCfg.PicFlowShowNumber);
	grl_picflow->setSlideNumberPos(fGrl->getPosition(grlCfg.PicFlowNumberPos));
	grl_picflow->setSlideNumberMargin(grlCfg.PicFlowNumberMargin);
	grl_picflow->setSlideNumberFormat(grlCfg.PicFlowNumberFormat);

	grl_picflow->setShowIconExtra(grlCfg.PicFlowShowIconExtra);
	grl_picflow->setSlideIconExtraPos(grlCfg.PicFlowIconExtraPos);
	grl_picflow->setSlideIconExtraMargin(grlCfg.PicFlowIconExtraMargin);

	grl_picflow->setSlideMargin(grlCfg.PicFlowMargin);

	grl_picflow->setShowTriangle(grlCfg.PicFlowShowTriangle);
	ui->vLayout_picflow->addWidget(grl_picflow);

// Conecta la Lista de Categorias
	connect(grl_picflow, SIGNAL(centerIndexChanged(int)), this, SLOT(grl_picflow_skip(int)));
	connect(grl_picflow, SIGNAL(finishedAnimation()), this, SLOT(grl_picflow_finishedAnimation()));
	connect(grl_picflow, SIGNAL(isActive(bool)), this, SLOT(grl_picflow_isActive(bool)));
	connect(grl_picflow, SIGNAL(selected(int)) , this, SLOT(grl_picflow_selected(int)));
	connect(grl_picflow, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(grl_picflow_customContextMenuRequested(const QPoint&)));
//--
	setDockNestingEnabled(true);
	this->splitDockWidget(ui->dockw_Capturas, ui->dockw_Datos, Qt::Horizontal);
	this->splitDockWidget(ui->dockw_Datos   , ui->dockw_Files, Qt::Vertical);
	this->tabifyDockWidget(ui->dockw_Files, ui->dockw_Urls);
	this->tabifyDockWidget(ui->dockw_Files, ui->dockw_Imagenes);
	this->tabifyDockWidget(ui->dockw_Files, ui->dockw_Videos);
	this->tabifyDockWidget(ui->dockw_Files, ui->dockw_Sonidos);
	ui->dockw_Files->raise();

// Conecta los distintos Paneles
	connect(ui->dockw_Nav->toggleViewAction()        , SIGNAL(toggled(bool)), this, SLOT(dockw_Nav_toggled(bool))        );
	connect(ui->dockw_Datos->toggleViewAction()      , SIGNAL(toggled(bool)), this, SLOT(dockw_Datos_toggled(bool))      );
	connect(ui->dockw_Files->toggleViewAction()      , SIGNAL(toggled(bool)), this, SLOT(dockw_Files_toggled(bool))      );
	connect(ui->dockw_Urls->toggleViewAction()       , SIGNAL(toggled(bool)), this, SLOT(dockw_Urls_toggled(bool))       );
	connect(ui->dockw_Videos->toggleViewAction()     , SIGNAL(toggled(bool)), this, SLOT(dockw_Videos_toggled(bool))     );
	connect(ui->dockw_Sonidos->toggleViewAction()    , SIGNAL(toggled(bool)), this, SLOT(dockw_Sonidos_toggled(bool))    );
	connect(ui->dockw_Capturas->toggleViewAction()   , SIGNAL(toggled(bool)), this, SLOT(dockw_Capturas_toggled(bool))   );
	connect(ui->dockw_Imagenes->toggleViewAction()   , SIGNAL(toggled(bool)), this, SLOT(dockw_Imagenes_toggled(bool))   );
	connect(ui->dockw_PictureFlow->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(dockw_PictureFlow_toggled(bool)));

	connect(&fs_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)));

// Centra la aplicación en el escritorio
	if (!grlCfg.main_fullscreen)
		this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
	if (!grlCfg.main_state.isEmpty())
		this->restoreState(grlCfg.main_state);
	if (!grlCfg.main_geometry.isEmpty())
		this->restoreGeometry(grlCfg.main_geometry);

	mediaSound = new GrlMultiMedia(grlDir.DatosGame, fGrl->theme(), grlCfg, true, true, ui->dockWidgetContents_Sonidos);
	mediaSound->setTemplate(fGrl->tplInfoJuego("tpl_info_media"));
	connect(mediaSound, SIGNAL(changeConfig()), this, SLOT(changeMediaConfig()));

	ui->vLayout_mediaSound->addWidget(mediaSound);

	mediaVideo = new GrlMultiMedia(grlDir.DatosGame, fGrl->theme(), grlCfg, false, true, ui->dockWidgetContents_Videos);
	mediaVideo->setWindowFlags(Qt::Window);
	mediaVideo->hide();
	connect(mediaVideo, SIGNAL(changeConfig()), this, SLOT(changeMediaConfig()));
}

// Crea la barra de herramientas, busqueda y ordenar
void GrLida::createToolBars()
{
	ui->tb_main->clear();
	ui->tb_main->addAction(ui->mnu_file_cerrar_big);
	ui->tb_main->addSeparator();
	ui->tb_main->addAction(ui->mnu_edit_nuevo_dosbox_big);
	ui->tb_main->addAction(ui->mnu_edit_nuevo_scummvm_big);
	ui->tb_main->addAction(ui->mnu_edit_nuevo_vdmsound_big);
	ui->tb_main->addSeparator();
	ui->tb_main->addAction(ui->mnu_ejecutar_juego_big);
	ui->tb_main->addAction(ui->mnu_ejecutar_setup_big);
	ui->tb_main->addSeparator();
	ui->tb_main->addAction(ui->mnu_edit_nuevo_big);
	ui->tb_main->addAction(ui->mnu_edit_editar_big);
	ui->tb_main->addAction(ui->mnu_edit_eliminar_big);
	ui->tb_main->addSeparator();
	ui->tb_main->addWidget(ui->cbxCategoriasTb);
	ui->tb_main->addSeparator();
	ui->tb_main->addAction(ui->mnu_tool_importar_big);
	ui->tb_main->addAction(ui->mnu_ver_datos_big);
	ui->tb_main->addAction(ui->mnu_ver_archivos_big);
	ui->tb_main->addAction(ui->mnu_ver_capturas_big);
	ui->tb_main->addSeparator();
	ui->tb_main->addAction(ui->mnu_tool_opciones_big);
	ui->tb_main->addAction(ui->mnu_ver_acercad_big);

	ui->tb_ordenar->clear();
	ui->tb_ordenar->addWidget(ui->widgetOrdenar);

#ifdef Q_OS_WIN
	ui->mnu_tool_virtual_drive->setEnabled(true);
	ui->mnu_ejecutar_montaje->setEnabled(true);
#else
	ui->mnu_tool_virtual_drive->setEnabled(false);
	ui->mnu_ejecutar_montaje->setEnabled(false);
#endif
}

void GrLida::createStatusBar()
{
	lb_panel_1 = new QLabel(ui->statusBar);
	lb_panel_1->setFrameStyle(QFrame::NoFrame);
	lb_panel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_1->setPixmap(QPixmap(fGrl->theme() +"img16/datos_3.png"));
	lb_panel_1->setMinimumSize(20, 20);

	lb_panel_2 = new QLabel(ui->statusBar);
	lb_panel_2->setFrameStyle(QFrame::NoFrame);
	lb_panel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	lb_panel_3 = new QLabel(ui->statusBar);
	lb_panel_3->setFrameStyle(QFrame::NoFrame);
	lb_panel_3->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_3->setPixmap(QPixmap(fGrl->theme() +"img16/sinimg.png"));
	lb_panel_3->setMinimumSize(20, 20);

	lb_panel_7 = new QLabel(ui->statusBar);
	lb_panel_7->setFrameStyle(QFrame::NoFrame);
	lb_panel_7->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_7->setPixmap(QPixmap(fGrl->theme() +"img16/sinimg.png"));
	lb_panel_7->setMinimumSize(20, 20);
	lb_panel_7->setText("");
	lb_panel_7->setVisible(false);

	lb_panel_4 = new QLabel(ui->statusBar);
	lb_panel_4->setFrameStyle(QFrame::NoFrame);
	lb_panel_4->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	lb_panel_5 = new QLabel(ui->statusBar);
	lb_panel_5->setFrameStyle(QFrame::NoFrame);
	lb_panel_5->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lb_panel_5->setPixmap(QPixmap(fGrl->theme() +"img16/datos_1.png"));
	lb_panel_5->setMinimumSize(20, 20);

	lb_panel_6 = new QLabel(ui->statusBar);
	lb_panel_6->setFrameStyle(QFrame::NoFrame);
	lb_panel_6->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui->statusBar->addWidget(lb_panel_1);
	ui->statusBar->addWidget(lb_panel_2);
	ui->statusBar->addWidget(lb_panel_7);
	ui->statusBar->addWidget(lb_panel_3);
	ui->statusBar->addWidget(lb_panel_4);
	ui->statusBar->addWidget(lb_panel_5);
	ui->statusBar->addWidget(lb_panel_6, 1);
}

void GrLida::createTrayIcon()
{
	if (QSystemTrayIcon::isSystemTrayAvailable())
	{
		trayIconMenu = new QMenu(this);
		trayIconMenu->addAction(ui->mnu_trayicon_restore );
		trayIconMenu->addAction(ui->mnu_trayicon_minimize);
		trayIconMenu->addAction(ui->mnu_trayicon_maximize);
		trayIconMenu->addSeparator();
		trayIconMenu->addAction(ui->mnu_file_cerrar);

		trayIcon = new QSystemTrayIcon(this);
		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(fGrl->theme() +"img16/gr-lida.png"));
		trayIcon->setToolTip("GR-lida");
		trayIcon->show(); // Primero lo mostramos
		trayIcon->hide(); // Segundo lo ocultamos, Fix Qt 5.1x.x

		connect(ui->mnu_trayicon_restore , SIGNAL(triggered()), this, SLOT(showNormal()));
		connect(ui->mnu_trayicon_minimize, SIGNAL(triggered()), this, SLOT(hide()));
		connect(ui->mnu_trayicon_maximize, SIGNAL(triggered()), this, SLOT(showMaximized()));
		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

		isTrayIcon = true;
	} else
		isTrayIcon = false;
}

void GrLida::createTbShortCut()
{
	grpActions = new QActionGroup(ui->tb_shortcut);
	connect(grpActions, SIGNAL(triggered(QAction*)), this, SLOT(actionShortCutClicked(QAction*)));

	ui->tb_shortcut->setMovable(true);
	ui->tb_shortcut->setFloatable(false);
	ui->tb_shortcut->setAllowedAreas(Qt::AllToolBarAreas);
//	ui->tb_shortcut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);// texto al lado
//	ui->tb_shortcut->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);// texto debajo
	ui->tb_shortcut->setToolButtonStyle(Qt::ToolButtonIconOnly);// solo iconos
//	ui->tb_shortcut->setToolButtonStyle(Qt::ToolButtonTextOnly);// solo texto
	ui->tb_shortcut->setIconSize(QSize(32, 32));

//	scrollArea = new QScrollArea(ui->tb_shortcut);
//	scrollArea->setFrameShape(QFrame::NoFrame);
//	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
//	scrollArea->setWidgetResizable(true);
//	connect(scrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(sliderRange(int,int)));

//	QToolBar *container = new QToolBar(scrollArea);
//	container->setMovable(false);
//	container->setFloatable(false);
//	container->setAllowedAreas(Qt::NoToolBarArea);
//	container->setToolButtonStyle(Qt::ToolButtonIconOnly);// solo iconos
//	container->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
//	container->setIconSize(QSize(32, 32));
//	container->setStyleSheet("QToolBar{border:1px;border-color: rgb(170, 85, 255);}");

//	scrollArea->setWidget(container);

//	ui->tb_shortcut->addAction(ui->btnLeftSc);
//	ui->tb_shortcut->addWidget(scrollArea);
//	ui->tb_shortcut->addAction(ui->btnRightSc);
}

void GrLida::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
//	QSystemTrayIcon::Trigger
//	QSystemTrayIcon::MiddleClick
	if (reason == QSystemTrayIcon::DoubleClick)
	{
		this->show();
	}
}

void GrLida::actionShortCutClicked(QAction *action)
{
	sql_where_select = action->data().toString();

//qDebug() << "ShortCut ------------------";
//qDebug() << "text: " << action->text();
//qDebug() << "slq : " << sql_where_select;
//qDebug() << "---------------------------";

//	if (sql_where_select == "grl:new_game")
//		emit on_mnu_edit_nuevo_triggered();
//	else {
		grl_picflow->setSlideTitle(action->text());
		grl_picflow->setSlideTitleIcon(action->icon().pixmap(action->icon().availableSizes().first()));
		cargarListaJuegosDB(sql_where_select);
//	}
}
/*
void GrLida::sliderRange(int min, int max)
{
	if (min == 0 && max == 0)
	{
		ui->btnLeftSc->setVisible(false);
		ui->btnRightSc->setVisible(false);
	} else {
		ui->btnLeftSc->setVisible(true);
		ui->btnRightSc->setVisible(true);
	}
}

void GrLida::on_btnLeftSc_triggered()
{
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() - 10);
}

void GrLida::on_btnRightSc_triggered()
{
	scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() + 10);
}
*/
// Carga la configuración del GR-lida.
void GrLida::cargarConfig()
{
// Menús Ver
	ui->mnu_ver_nav->setChecked(grlCfg.Pnl_Nav);
	ui->mnu_ver_cover_mode->setChecked(grlCfg.ver_IconMode);
	ui->mnu_ver_pictureflow->setChecked(grlCfg.Pnl_PictureFlow);
	ui->mnu_ver_pictureflow_to_center->setChecked(grlCfg.PictureFlowToCenter);
	ui->mnu_ver_datos->setChecked(grlCfg.Pnl_Datos);
	ui->mnu_ver_datos_big->setChecked(grlCfg.Pnl_Datos);
	ui->mnu_ver_archivos->setChecked(grlCfg.Pnl_Files);
	ui->mnu_ver_archivos_big->setChecked(grlCfg.Pnl_Files);
	ui->btnVer_Archivos->setChecked(grlCfg.Pnl_Files);
	ui->mnu_ver_capturas->setChecked(grlCfg.Pnl_Capturas);
	ui->mnu_ver_capturas_big->setChecked(grlCfg.Pnl_Capturas);
	ui->mnu_ver_imagenes->setChecked(grlCfg.Pnl_Imagenes);
	ui->mnu_ver_enlaces->setChecked(grlCfg.Pnl_Urls);
	ui->mnu_ver_videos->setChecked(grlCfg.Pnl_Videos);
	ui->mnu_ver_sonidos->setChecked(grlCfg.Pnl_Sonidos);
	ui->mnu_ver_menubar->setChecked(grlCfg.Pnl_Menubar);
	ui->mnu_ver_toolbar->setChecked(grlCfg.Pnl_Toolbar);
	ui->mnu_ver_toolbar_datos->setChecked(grlCfg.Pnl_ToolbarDatos);
	ui->mnu_ver_ordenar->setChecked(grlCfg.Pnl_Ordenar);
	ui->mnu_ver_shortcut->setChecked(grlCfg.Pnl_ShortCut);
	ui->mnu_ver_statusbar->setChecked(grlCfg.Pnl_StatusBar);
	ui->mnu_ver_fullscreen->setChecked(grlCfg.main_fullscreen);

// Menús Ver Paneles
	ui->mnu_ver_auto_hide_title_bars->setChecked(grlCfg.Pnl_AutoHideTitleBars);
	ui->mnu_ver_ignore_auto_hide_nav->setChecked(grlCfg.Pnl_Nav_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_datos->setChecked(grlCfg.Pnl_Datos_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_files->setChecked(grlCfg.Pnl_Files_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_urls->setChecked(grlCfg.Pnl_Urls_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_videos->setChecked(grlCfg.Pnl_Videos_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_sonidos->setChecked(grlCfg.Pnl_Sonidos_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_capturas->setChecked(grlCfg.Pnl_Capturas_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_imagenes->setChecked(grlCfg.Pnl_Imagenes_IgnoreAutoHide);
	ui->mnu_ver_ignore_auto_hide_pictureflow->setChecked(grlCfg.Pnl_PictureFlow_IgnoreAutoHide);

	ui->dockw_Nav->setIgnoreAutoHide(grlCfg.Pnl_Nav_IgnoreAutoHide);
	ui->dockw_Datos->setIgnoreAutoHide(grlCfg.Pnl_Datos_IgnoreAutoHide);
	ui->dockw_Files->setIgnoreAutoHide(grlCfg.Pnl_Files_IgnoreAutoHide);
	ui->dockw_Urls->setIgnoreAutoHide(grlCfg.Pnl_Urls_IgnoreAutoHide);
	ui->dockw_Videos->setIgnoreAutoHide(grlCfg.Pnl_Videos_IgnoreAutoHide);
	ui->dockw_Sonidos->setIgnoreAutoHide(grlCfg.Pnl_Sonidos_IgnoreAutoHide);
	ui->dockw_Capturas->setIgnoreAutoHide(grlCfg.Pnl_Capturas_IgnoreAutoHide);
	ui->dockw_Imagenes->setIgnoreAutoHide(grlCfg.Pnl_Imagenes_IgnoreAutoHide);
	ui->dockw_PictureFlow->setIgnoreAutoHide(grlCfg.Pnl_PictureFlow_IgnoreAutoHide);

	ui->mnu_ver_titlebars_nav->setChecked(grlCfg.Pnl_Nav_VisibleTitleBars);
	ui->mnu_ver_titlebars_datos->setChecked(grlCfg.Pnl_Datos_VisibleTitleBars);
	ui->mnu_ver_titlebars_files->setChecked(grlCfg.Pnl_Files_VisibleTitleBars);
	ui->mnu_ver_titlebars_urls->setChecked(grlCfg.Pnl_Urls_VisibleTitleBars);
	ui->mnu_ver_titlebars_videos->setChecked(grlCfg.Pnl_Videos_VisibleTitleBars);
	ui->mnu_ver_titlebars_sonidos->setChecked(grlCfg.Pnl_Sonidos_VisibleTitleBars);
	ui->mnu_ver_titlebars_capturas->setChecked(grlCfg.Pnl_Capturas_VisibleTitleBars);
	ui->mnu_ver_titlebars_imagenes->setChecked(grlCfg.Pnl_Imagenes_VisibleTitleBars);
	ui->mnu_ver_titlebars_pictureflow->setChecked(grlCfg.Pnl_PictureFlow_VisibleTitleBars);

	ui->dockw_Nav->setVisibleTitleBars(grlCfg.Pnl_Nav_VisibleTitleBars);
	ui->dockw_Datos->setVisibleTitleBars(grlCfg.Pnl_Datos_VisibleTitleBars);
	ui->dockw_Files->setVisibleTitleBars(grlCfg.Pnl_Files_VisibleTitleBars);
	ui->dockw_Urls->setVisibleTitleBars(grlCfg.Pnl_Urls_VisibleTitleBars);
	ui->dockw_Videos->setVisibleTitleBars(grlCfg.Pnl_Videos_VisibleTitleBars);
	ui->dockw_Sonidos->setVisibleTitleBars(grlCfg.Pnl_Sonidos_VisibleTitleBars);
	ui->dockw_Capturas->setVisibleTitleBars(grlCfg.Pnl_Capturas_VisibleTitleBars);
	ui->dockw_Imagenes->setVisibleTitleBars(grlCfg.Pnl_Imagenes_VisibleTitleBars);
	ui->dockw_PictureFlow->setVisibleTitleBars(grlCfg.Pnl_PictureFlow_VisibleTitleBars);

	emit on_mnu_ver_auto_hide_title_bars_triggered(grlCfg.Pnl_AutoHideTitleBars);

// Menús Ver columnas
	ui->mnu_ver_col_icono->setChecked(grlCfg.ver_col_Icono);
	ui->mnu_ver_col_subtitulo->setChecked(grlCfg.ver_col_Subtitulo);
	ui->mnu_ver_col_genero->setChecked(grlCfg.ver_col_Genero);
	ui->mnu_ver_col_compania->setChecked(grlCfg.ver_col_Compania);
	ui->mnu_ver_col_desarrollador->setChecked(grlCfg.ver_col_Desarrollador);
	ui->mnu_ver_col_tema->setChecked(grlCfg.ver_col_Tema);
	ui->mnu_ver_col_grupo->setChecked(grlCfg.ver_col_Grupo);
	ui->mnu_ver_col_perspectiva->setChecked(grlCfg.ver_col_Perspectiva);
	ui->mnu_ver_col_idioma->setChecked(grlCfg.ver_col_Idioma);
	ui->mnu_ver_col_idioma_voces->setChecked(grlCfg.ver_col_IdiomaVoces);
	ui->mnu_ver_col_formato->setChecked(grlCfg.ver_col_Formato);
	ui->mnu_ver_col_anno->setChecked(grlCfg.ver_col_Anno);
	ui->mnu_ver_col_numdisc->setChecked(grlCfg.ver_col_NumDisc);
	ui->mnu_ver_col_sistemaop->setChecked(grlCfg.ver_col_SistemaOp);
	ui->mnu_ver_col_tamano->setChecked(grlCfg.ver_col_Tamano);
	ui->mnu_ver_col_graficos->setChecked(grlCfg.ver_col_Graficos);
	ui->mnu_ver_col_sonido->setChecked(grlCfg.ver_col_Sonido);
	ui->mnu_ver_col_jugabilidad->setChecked(grlCfg.ver_col_Jugabilidad);
	ui->mnu_ver_col_original->setChecked(grlCfg.ver_col_Original);
	ui->mnu_ver_col_estado->setChecked(grlCfg.ver_col_Estado);
	ui->mnu_ver_col_fecha->setChecked(grlCfg.ver_col_Fecha);
	ui->mnu_ver_col_tipoemu->setChecked(grlCfg.ver_col_TipoEmu);
	ui->mnu_ver_col_favorito->setChecked(grlCfg.ver_col_Favorito);
	ui->mnu_ver_col_rating->setChecked(grlCfg.ver_col_Rating);
	ui->mnu_ver_col_edad->setChecked(grlCfg.ver_col_Edad);
	ui->mnu_ver_col_usuario->setChecked(grlCfg.ver_col_Usuario);

// Paneles
	ui->menubar->setVisible(grlCfg.Pnl_Menubar);
	ui->tb_main->setVisible(grlCfg.Pnl_Toolbar);
	ui->tb_ordenar->setVisible(grlCfg.Pnl_Ordenar);
	ui->tb_shortcut->setVisible(grlCfg.Pnl_ShortCut);
	ui->statusBar->setVisible(grlCfg.Pnl_StatusBar);
	ui->dockw_Nav->setVisible(grlCfg.Pnl_Nav);
	ui->tvJuegos->setVisible(!grlCfg.ver_IconMode);
	ui->lvJuegos->setVisible(grlCfg.ver_IconMode);
	ui->dockw_PictureFlow->setVisible(grlCfg.Pnl_PictureFlow);
	ui->dockw_PictureFlow->setMinimumHeight(grlCfg.PicFlowMinHeight);
	ui->dockw_Datos->setVisible(grlCfg.Pnl_Datos);
	ui->dockw_Files->setVisible(grlCfg.Pnl_Files);
	ui->dockw_Urls->setVisible(grlCfg.Pnl_Urls);
	ui->dockw_Videos->setVisible(grlCfg.Pnl_Videos);
	ui->dockw_Sonidos->setVisible(grlCfg.Pnl_Sonidos);
	ui->dockw_Capturas->setVisible(grlCfg.Pnl_Capturas);
	ui->dockw_Imagenes->setVisible(grlCfg.Pnl_Imagenes);
	ui->wInfoDatos->setVisible(grlCfg.Pnl_ToolbarDatos);

// Configuración del twListNav
	ui->twListNav->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twListNav->header()->setSectionsMovable(false);
	ui->twListNav->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twListNav->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
#else
	ui->twListNav->header()->setMovable(false);
	ui->twListNav->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twListNav->header()->setResizeMode(1, QHeaderView::ResizeToContents);
#endif
	ui->twListNav->setColumnWidth(1, 60);
	isNavItemChanged = false;

// Configuración del tvJuegos
	ui->tvJuegos->setModel(data_model);
	ui->tvJuegos->setItemDelegate(grl_tv_delegate);
	ui->tvJuegos->installEventFilter(this);
	ui->tvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tvJuegos->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tvJuegos->setDragDropMode(QAbstractItemView::NoDragDrop);
	ui->tvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tvJuegos->setProperty("showDropIndicator", QVariant(false));
	ui->tvJuegos->setAlternatingRowColors(lwIconCfg.tw_alternating_row_colors);
	ui->tvJuegos->setIconSize(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height));
	ui->tvJuegos->setRootIsDecorated(false);
	ui->tvJuegos->setUniformRowHeights(true);
	ui->tvJuegos->setItemsExpandable(false);
	ui->tvJuegos->setAllColumnsShowFocus(true);
	connect(ui->tvJuegos->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(tvJuegos_changed(QModelIndex,QModelIndex)));
	isItemChanged = false;

// Configuración del lvJuegos
	ui->lvJuegos->setModel(data_model);
	ui->lvJuegos->setItemDelegate(grl_lv_delegate);
	ui->lvJuegos->installEventFilter(this);
	ui->lvJuegos->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->lvJuegos->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->lvJuegos->setDragDropMode(QAbstractItemView::NoDragDrop);
	ui->lvJuegos->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->lvJuegos->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->lvJuegos->setAlternatingRowColors(false);
	ui->lvJuegos->setIconSize(QSize(lwIconCfg.icon_width, lwIconCfg.icon_height));
	ui->lvJuegos->setMovement(QListView::Static);
	ui->lvJuegos->setFlow(QListView::LeftToRight);
	ui->lvJuegos->setResizeMode(QListView::Adjust);
	ui->lvJuegos->setSpacing(0);
	ui->lvJuegos->setViewMode(QListView::IconMode);
	ui->lvJuegos->setUniformItemSizes(true);
	connect(ui->lvJuegos->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(lvJuegos_changed(QModelIndex,QModelIndex)));

// Configuración del twFiles
	ui->twFiles->installEventFilter(this);
	ui->twFiles->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twFiles->header()->setSectionsMovable(false);
	ui->twFiles->header()->setSectionResizeMode(0, QHeaderView::Interactive);
	ui->twFiles->header()->setSectionResizeMode(1, QHeaderView::Fixed);
	ui->twFiles->header()->setSectionResizeMode(2, QHeaderView::Interactive);
	ui->twFiles->header()->setSectionResizeMode(3, QHeaderView::Interactive);
	ui->twFiles->header()->setSectionResizeMode(4, QHeaderView::Interactive);
#else
	ui->twFiles->header()->setMovable(false);
	ui->twFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(1, QHeaderView::Fixed);
	ui->twFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twFiles->header()->setResizeMode(4, QHeaderView::Interactive);
#endif
	ui->twFiles->setColumnWidth(0, 200);
	ui->twFiles->setColumnWidth(1, 60);
	ui->twFiles->setColumnWidth(2, 100);
	ui->twFiles->setColumnWidth(3, 50);
	ui->twFiles->setColumnWidth(4, 50);

// Configuración del twUrls
	ui->twUrls->installEventFilter(this);
	ui->twUrls->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twUrls->header()->setSectionsMovable(false);
	ui->twUrls->header()->setSectionResizeMode(0, QHeaderView::Interactive);
	ui->twUrls->header()->setSectionResizeMode(1, QHeaderView::Interactive);
#else
	ui->twUrls->header()->setMovable(false);
	ui->twUrls->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twUrls->header()->setResizeMode(1, QHeaderView::Interactive);
#endif
	ui->twUrls->setColumnWidth(0, 250);

// Configuración lista Urls, Videos, Sonidos, Capturas
	ui->lwVideos->installEventFilter(this);
	ui->lwVideos->setIconSize(QSize(24, 24));
// Capturas
	ui->lwCapturas->installEventFilter(this);
	ui->lwCapturas->setIconSize(QSize(128, 128));
	ui->lwCapturas->setMovement(QListView::Static);
	ui->lwCapturas->setFlow(QListView::LeftToRight);
	ui->lwCapturas->setResizeMode(QListView::Adjust);
	ui->lwCapturas->setViewMode(QListView::IconMode);
// Imágenes
	ui->lwImagenes->installEventFilter(this);
	ui->lwImagenes->setIconSize(QSize(128, 128));
	ui->lwImagenes->setMovement(QListView::Static);
	ui->lwImagenes->setFlow(QListView::LeftToRight);
	ui->lwImagenes->setResizeMode(QListView::Adjust);
	ui->lwImagenes->setViewMode(QListView::IconMode);

	dbx_list = fGrl->cargaDatosQHash(grlDir.Datos +"dbx_list.txt", 6, "|");

	IdGame           = "";
	TipoEmu          = "";
	sql_where_select = "";
	skip_picflow     = 0;
	id_cat           = 0;

	img_pf_favorite      = QImage(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgFavorito);
	img_pf_favorite_gray = fGrl->imagenToDisabled(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgFavorito);

	img_pf_original      = QImage(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgOriginal);
	img_pf_original_gray = fGrl->imagenToDisabled(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgOriginal);

	virtual_drive_list = fGrl->cargarListVirtualDrive(grlDir.Datos +"virtual_driver_list.txt");
	virtualDrive       = virtual_drive_list["NO_VIRTUAL_DRIVE"];

	cargarTextResource();

	if (!grlCfg.LastSelectCatID.isEmpty())
		id_cat = fGrl->strToInt(grlCfg.LastSelectCatID);
}

// Guarda la configuración del GR-lida.
void GrLida::guardarConfig()
{
	if (!grlCfg.ver_IconMode)
		grlCfg.main_tvJuegos_state = ui->tvJuegos->header()->saveState();

	if (!IdGame.isEmpty())
		grlCfg.LastSelectGameID = IdGame;

	grlCfg.Version          = fGrl->versionGrl();
	grlCfg.main_maximized   = this->isMaximized();
	grlCfg.main_geometry    = this->saveGeometry();
	grlCfg.main_state       = this->saveState(); // Toolbar and DockWidget state.
	grlCfg.LastSelectCatID  = categoria[id_cat].id;

	fGrl->guardarGRLConfig(grlDir.Home +"GR-lida.conf", grlCfg);
}

void GrLida::setTheme()
{
	setWindowIcon(QIcon(":/img32/gr-lida.png"));

// Templates
	tpl_info_game_old = fGrl->tplInfoJuego("tpl_info_juego");
	tpl_info_game_empty_old = fGrl->tplInfoJuego("tpl_info_juego_no");
	tpl_info_game_empty_old.replace("{info_contenido}", tr("Información no disponible"));
// StatusBar
	lb_panel_1->setPixmap(QPixmap(fGrl->theme() +"img16/datos_3.png"));
	lb_panel_3->setPixmap(QPixmap(fGrl->theme() +"img16/sinimg.png"));
	lb_panel_5->setPixmap(QPixmap(fGrl->theme() +"img16/datos_1.png"));
// Menú archivo
	ui->mnu_file_informacion->setIcon(QIcon(fGrl->theme() +"img16/informacion.png"));
	ui->mnu_file_cerrar->setIcon(QIcon(fGrl->theme() +"img16/cerrar.png"));
// Menú editar
	ui->mnu_edit_nuevo_dosbox->setIcon(QIcon(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->mnu_edit_nuevo_scummvm->setIcon(QIcon(fGrl->theme() +"img16/cat/scummvm.png"));
	ui->mnu_edit_nuevo_vdmsound->setIcon(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"));
	ui->mnu_edit_nuevo->setIcon(QIcon(fGrl->theme() +"img16/nuevo.png"));
	ui->mnu_edit_editar->setIcon(QIcon(fGrl->theme() +"img16/editar.png"));
	ui->mnu_edit_eliminar->setIcon(QIcon(fGrl->theme() +"img16/eliminar.png"));
	ui->mnu_edit_favorito->setIcon(QIcon(fGrl->theme() +"img16/"+ grlCfg.IconoFav));
	ui->mnu_edit_original->setIcon(QIcon(fGrl->theme() +"img16/original.png"));
// Menú ejecutar
	ui->mnu_ejecutar_dosbox->setIcon(QIcon(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->mnu_ejecutar_scummvm->setIcon(QIcon(fGrl->theme() +"img16/cat/scummvm.png"));
	ui->mnu_ejecutar_juego->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app.png"));
	ui->mnu_ejecutar_setup->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app_setup.png"));
// Menú herramientas
	ui->mnu_tool_instalar->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app_setup.png"));
	ui->mnu_tool_virtual_drive->setIcon(QIcon(fGrl->theme() +"img16/virtual_drive.png"));
	ui->mnu_tool_importar->setIcon(QIcon(fGrl->theme() +"img16/importar.png"));
	ui->mnu_tool_exportar->setIcon(QIcon(fGrl->theme() +"img16/exportar.png"));
	ui->mnu_tool_cambiar_categoria->setIcon(QIcon(fGrl->theme() +"img16/cambiar_categoria.png"));
	ui->mnu_tool_edit_themes->setIcon(QIcon(fGrl->theme() +"img16/style.png"));
	ui->mnu_tool_opciones->setIcon(QIcon(fGrl->theme() +"img16/opciones.png"));
// Menú ver
	ui->mnu_ver_nav->setIcon(QIcon(fGrl->theme() +"img16/categorias.png"));
	ui->mnu_ver_cover_mode->setIcon(QIcon(fGrl->theme() +"img16/cover_mode.png"));
	emit on_mnu_ver_pictureflow_to_center_triggered(grlCfg.PictureFlowToCenter);
	ui->mnu_ver_datos->setIcon(QIcon(fGrl->theme() +"img16/cat/datos.png"));
	ui->mnu_ver_archivos->setIcon(QIcon(fGrl->theme() +"img16/archivos.png"));
	ui->mnu_ver_enlaces->setIcon(QIcon(fGrl->theme() +"img16/html.png"));
	ui->mnu_ver_videos->setIcon(QIcon(fGrl->theme() +"img16/video.png"));
	ui->mnu_ver_sonidos->setIcon(QIcon(fGrl->theme() +"img16/audio.png"));
	ui->mnu_ver_capturas->setIcon(QIcon(fGrl->theme() +"img16/capturas.png"));
	ui->mnu_ver_imagenes->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->mnu_ver_menubar->setIcon(QIcon(fGrl->theme() +"img16/barra_herramintas.png"));
	ui->mnu_ver_toolbar->setIcon(QIcon(fGrl->theme() +"img16/barra_herramintas.png"));
	ui->mnu_ver_ordenar->setIcon(QIcon(fGrl->theme() +"img16/orden_asc.png"));
	ui->mnu_ver_shortcut->setIcon(QIcon(fGrl->theme() +"img16/shortcut.png"));
	ui->mnu_ver_statusbar->setIcon(QIcon(fGrl->theme() +"img16/barra_herramintas.png"));
	ui->mnu_ver_fullscreen->setIcon(QIcon(fGrl->theme() +"img16/fit_to_win.png"));
	ui->mnu_ver_ayuda->setIcon(QIcon(fGrl->theme() +"img16/ayuda.png"));
	ui->mnu_ver_check_updates->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->mnu_ver_acercad->setIcon(QIcon(fGrl->theme() +"img16/acercad.png"));
// Menú ver columnas
	ui->menuMenuVerColumnas->setIcon(QIcon(fGrl->theme() +"img16/column_list.png"));
// Menú ver carpetas
	ui->menuMenuVerCarpetas->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->mnu_ver_carpeta_datos_juego->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->mnu_ver_carpeta_iconos->setIcon(QIcon(fGrl->theme() +"img16/capturas.png"));
	ui->mnu_ver_carpeta_smiles->setIcon(QIcon(fGrl->theme() +"img16/capturas.png"));
	ui->mnu_ver_carpeta_themes->setIcon(QIcon(fGrl->theme() +"img16/style.png"));
	ui->mnu_ver_carpeta_datos->setIcon(QIcon(fGrl->theme() +"img16/datos_3.png"));
	ui->mnu_ver_carpeta_scripts->setIcon(QIcon(fGrl->theme() +"img16/archivo_config.png"));
	ui->mnu_ver_carpeta_templates->setIcon(QIcon(fGrl->theme() +"img16/datos_3.png"));
	ui->mnu_ver_carpeta_idiomas->setIcon(QIcon(fGrl->theme() +"img16/idiomas.png"));
	ui->mnu_ver_carpeta_temp->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->mnu_ver_carpeta_home->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
// Barra de heramientas
	setToolbarBigIcon(grlCfg.ToolbarBigIcon);
// Barra de heramientas busqueda
	ui->cbxOrdenBy->clear();
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Por ID")       , "idgrl"        );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Título")       , "titulo"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Subtítulo")    , "subtitulo"    );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Genero")       , "genero"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Compañia")     , "compania"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Desarrollador"), "desarrollador");
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Tema")         , "tema"         );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Idioma")       , "idioma"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Formato")      , "formato"      );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Año")          , "anno"         );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Nº discos")    , "numdisc"      );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Sistema OS")   , "sistemaop"    );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Gráficos")     , "graficos"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Sonido")       , "sonido"       );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Jugabilidad")  , "jugabilidad"  );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Original")     , "original"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Tipo emulador"), "tipo_emu"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Favorito")     , "favorito"     );
	ui->cbxOrdenBy->addItem(QIcon(fGrl->theme() +"img16/datos_3.png"), tr("Rating")       , "rating"       );
	ui->cbxOrdenBy->setCurrentIndex(ui->cbxOrdenBy->findData(grlCfg.db_orden_by));

	ui->cbxOrden->clear();
	ui->cbxOrden->addItem(QIcon(fGrl->theme() +"img16/orden_asc.png") , tr("Ascendente") , "ASC");
	ui->cbxOrden->addItem(QIcon(fGrl->theme() +"img16/orden_desc.png"), tr("Descendente"), "DESC");
	ui->cbxOrden->setCurrentIndex(ui->cbxOrden->findData(grlCfg.db_orden));
	ui->btnOrdenar->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
// Otros botones
	ui->btnVer_Archivos->setIcon(QIcon(fGrl->theme() +"img16/archivos.png"));
	ui->btnVer_CoverFront->setIcon(QIcon(fGrl->theme() +"img16/capturas.png"));
	ui->btnVer_CoverBack->setIcon(QIcon(fGrl->theme() +"img16/capturas.png"));

	ui->twListNav->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
	setConfigHeaderListaJuegos();

// StyleSheet
	QString font_usar = grlCfg.font_usar ? "*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}" : "";
	ui->lvJuegos->setStyleSheet(fGrl->myStyleSheet("StyleSheetList.qss"));
	this->setStyleSheet(fGrl->myStyleSheet("StyleSheet.qss") + font_usar);
	this->setStyle(QStyleFactory::create(grlCfg.Style));

	if (grlCfg.StylePalette)
		this->setPalette(QApplication::style()->standardPalette());
	else
		this->setPalette(QApplication::palette());
}

void GrLida::setSlideIconExtra()
{
	if (grlCfg.PicFlowIconExtraUse == "favorito")
	{
		if (ui->mnu_edit_favorito->isChecked())
			grl_picflow->setSlideIconExtra(img_pf_favorite);
		else
			grl_picflow->setSlideIconExtra(img_pf_favorite_gray);
	}
	else if (grlCfg.PicFlowIconExtraUse == "original")
	{
		if (ui->mnu_edit_original->isChecked())
			grl_picflow->setSlideIconExtra(img_pf_original);
		else
			grl_picflow->setSlideIconExtra(img_pf_original_gray);
	} else {
		grl_picflow->setSlideIconExtra(iconExtraImg);
	}
}

void GrLida::cargarTextResource()
{
	ui->txtInfo->clear();
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_on.png"), QImage(fGrl->theme() +"img16/star_on.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_star_off.png"), QImage(fGrl->theme() +"img16/star_off.png"));

	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_original.png"), QImage(fGrl->theme() +"img16/original.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_original_off.png"), fGrl->imagenToDisabled(fGrl->theme() +"img16/original.png"));

	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_favorito.png"), QImage(fGrl->theme() +"img16/"+ grlCfg.IconoFav));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_favorito_off.png"), fGrl->imagenToDisabled(fGrl->theme() +"img16/"+ grlCfg.IconoFav));

	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_gamepad.png"), QImage(fGrl->theme() +"img16/gamepad.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("img_rs_gamepad_off.png"), QImage(fGrl->theme() +"img16/gamepad_off.png"));

	edades_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->idioma() +"/edad_recomendada.txt", 3, "|");
	foreach (const stGrlDatos &edades, edades_list)
		ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("edad_rs_"+ edades.icono), QImage(fGrl->theme() +"img16/"+ edades.icono));

	idiomas_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->idioma() +"/idiomas.txt", 3, "|");
	foreach (const stGrlDatos &idioma, idiomas_list)
		ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("idioma_rs_"+ idioma.icono), QImage(fGrl->theme() +"img16/"+ idioma.icono));

	smiles_list = fGrl->cargaDatosQHash(grlDir.Datos +"smiles.txt", 2, "\",\"");
	foreach (const stGrlDatos &smile, smiles_list)
	{
		if (fGrl->getInfoFile(grlDir.Smiles + smile.icono).Ext == ".gif")
			ui->txtInfo->addAnimation(QUrl("smile_rs_"+ smile.key +"_"+ smile.icono), grlDir.Smiles + smile.icono);
		else
			ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("smile_rs_"+ smile.key +"_"+ smile.icono), QImage(grlDir.Smiles + smile.icono));
	}

	foreach (const stGrlDatos &dbx, dbx_list)
		ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_"+ dbx.key), QImage(fGrl->theme() +"img16/"+ dbx.icono));

	list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	foreach (const stGrlDatos &emu, list_emu)
		ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_"+ emu.key), QImage(fGrl->theme() +"img16/"+ emu.icono));

	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_datos"), QImage(fGrl->theme() +"img16/datos.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_dosbox"), QImage(fGrl->theme() +"img16/dosbox.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_scummvm"), QImage(fGrl->theme() +"img16/scummvm.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_vdmsound"), QImage(fGrl->theme() +"img16/vdmsound.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_pc"), QImage(fGrl->theme() +"img16/pc.png"));
	ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl("emu_rs_emuladores"), QImage(fGrl->theme() +"img16/emuladores.png"));
}

void GrLida::setToolbarBigIcon(bool bigIcon)
{
	ui->cbxCategoriasTb->setMinimumWidth(140);
	if (bigIcon)
	{
		ui->tb_main->setIconSize(QSize(32,32));
		ui->cbxCategoriasTb->setIconSize(QSize(24,24));
		ui->cbxCategoriasTb->setMinimumHeight(32);
		ui->cbxCategoriasTb->setMaximumHeight(32);
	} else {
		ui->tb_main->setIconSize(QSize(16,16));
		ui->cbxCategoriasTb->setIconSize(QSize(16,16));
		ui->cbxCategoriasTb->setMinimumHeight(24);
		ui->cbxCategoriasTb->setMaximumHeight(24);
	}

	QString ico_size = (bigIcon) ? "32" : "16";
	ui->mnu_file_cerrar_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/cerrar.png"));
	ui->mnu_edit_nuevo_dosbox_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/cat/dosbox.png"));
	ui->mnu_edit_nuevo_scummvm_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/cat/scummvm.png"));
	ui->mnu_edit_nuevo_vdmsound_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/cat/vdmsound.png"));
	ui->mnu_ejecutar_juego_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/ejecutar_app.png"));
	ui->mnu_ejecutar_setup_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/ejecutar_app_setup.png"));
	ui->mnu_edit_nuevo_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/nuevo.png"));
	ui->mnu_edit_editar_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/editar.png"));
	ui->mnu_edit_eliminar_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/eliminar.png"));
	ui->mnu_tool_importar_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/importar.png"));
	ui->mnu_ver_datos_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/cat/datos.png"));
	ui->mnu_ver_archivos_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/archivos.png"));
	ui->mnu_ver_capturas_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/capturas.png"));
	ui->mnu_tool_opciones_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/opciones.png"));
	ui->mnu_ver_acercad_big->setIcon(QIcon(fGrl->theme() +"img"+ ico_size +"/acercad.png"));

	ico_size = (bigIcon) ? "24" : "16";
	const int countCatTb = ui->cbxCategoriasTb->count();
	for (int i = 0; i < countCatTb; ++i)
	{
		int cat_id = ui->cbxCategoriasTb->itemData(i).toInt();
		ui->cbxCategoriasTb->setItemIcon(i, QIcon(fGrl->theme() +"img"+ ico_size +"/"+ categoria[cat_id].img));
	}
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
	ljMenuPopUp->addAction(ui->mnu_edit_original);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addMenu(ui->menuVer);
	ljMenuPopUp->addAction(ui->mnu_ver_fullscreen);
	ljMenuPopUp->addSeparator();
	ljMenuPopUp->addAction(ui->mnu_tool_cambiar_categoria);
	ljMenuPopUp->addAction(ui->mnu_tool_opciones);

// Muestra popupmenu
	ljMenuPopUp->exec(w_parent->mapToGlobal(pos));
	delete ljMenuPopUp;
}

void GrLida::checkUpdateGrl(bool is_menu)
{
	isUpdateMenu = is_menu;

	int ahora  = sql->getTime().toInt();
	int dia    = grlCfg.lastChkUpdate + (1 * 86400);
	int semana = grlCfg.lastChkUpdate + (7 * 86400);
	int mes    = grlCfg.lastChkUpdate + (30 * 86400);

	bool comprobar = false;
	if (grlCfg.chkUpdateInterval == 30)
		comprobar = ahora > mes ? true : false;
	else if (grlCfg.chkUpdateInterval == 7)
		comprobar = ahora > semana ? true : false;
	else
		comprobar = ahora > dia ? true : false;

	if (comprobar || is_menu)
	{
		if (!is_menu)
			grlCfg.lastChkUpdate = sql->getTime().toInt();

		httpdown = new HttpDownload(ui->statusBar);
		httpdown->setHidden(true);
		connect(httpdown, SIGNAL(statusFinished()), this, SLOT(statusFinished()));

		if (grlCfg.ProxyEnable)
			httpdown->setHttpProxy(grlCfg.ProxyType, grlCfg.ProxyHost, grlCfg.ProxyPort, grlCfg.ProxyUserName, grlCfg.ProxyPassword);
		httpdown->downloadFile(URL_GRL +"lastver.xml", grlDir.Temp +"lastver.xml");
	}
}

bool GrLida::getDatosUpdates(QList<stUpdates> &update, QString tipo)
{
	QScriptEngine engine;
	QString str_info, str_url, os;
	bool isNewVersion = false;
	str_info = "";
	str_url  = xml.attributes().value("url").toString();

	while (xml.readNextStartElement())
	{
		if (xml.name() == "info")
		{
			str_info = xml.readElementText();
		} else if (xml.name() == "item") {
			stUpdates up;
			up.title   = xml.attributes().value("title").toString();
			up.version = xml.attributes().value("ver").toString();
			up.ver_old = "0";
			up.file    = xml.attributes().value("file").toString();
			up.url     = str_url + xml.attributes().value("id").toString();
			up.tipo    = tipo;
			up.isNew   = false;
			up.isInstalled = false;

			os = xml.attributes().value("os").toString();

			if (tipo == "grl")
			{
				up.ver_old = fGrl->versionGrl();
				if (fGrl->getOS() == os)
					up.isInstalled = true;
			} else if (tipo == "scripts") {
				if (QFile::exists(grlDir.Scripts + up.file))
				{
					engine.evaluate(fGrl->leerArchivo(grlDir.Scripts + up.file, "UTF-8"));
					up.ver_old = fGrl->getValueScript(engine, "version", "0");
					up.isInstalled = true;
				}
				up.file = fGrl->getInfoFile(up.file).Name +".zip";
			} else if (tipo == "themes") {
				if (QFile::exists(grlDir.Themes + up.title +"/info.ini"))
				{
					QSettings settings(grlDir.Themes + up.title +"/info.ini", QSettings::IniFormat);
					settings.beginGroup("info");
						up.ver_old = settings.value("version", "0").toString();
					settings.endGroup();
					up.isInstalled = true;
				}
			}

			up.isNew     = fGrl->version_compare(up.ver_old, up.version).isNew;
			up.info      = str_info;
			up.info_item = xml.readElementText();

			if (up.isNew)
				isNewVersion = true;

			update << up;
		} else
			xml.skipCurrentElement();
	}

	return isNewVersion;
}

void GrLida::statusFinished()
{
	QList<stUpdates> updates_grl, updates_js, updates_theme;
	stUpdates updates_svm;

	bool isNuevaVersionGRlida  = false;
	bool isNuevosScript        = false;
	bool isNuevosThemes        = false;
	bool isNuevaVersionListSvm = false;

	xml.clear();
	xml.addData(fGrl->leerArchivo(grlDir.Temp +"lastver.xml", "UTF-8"));
	if (xml.readNextStartElement())
	{
		if (xml.name() == "updates" && xml.attributes().value("version") == "1.0")
		{
			Q_ASSERT(xml.isStartElement() && xml.name() == "updates");

			while (xml.readNextStartElement())
			{
				if (xml.name() == "grl")
				{
					isNuevaVersionGRlida = getDatosUpdates(updates_grl, "grl");
				} else if (xml.name() == "scripts") {
					isNuevosScript = getDatosUpdates(updates_js, "scripts");
				} else if (xml.name() == "themes") {
					isNuevosThemes = getDatosUpdates(updates_theme, "themes");
				} else if (xml.name() == "list_svm") {
					updates_svm.title       = xml.attributes().value("title").toString();
					updates_svm.version     = xml.attributes().value("ver").toString();
					updates_svm.ver_old     = !grlCfg.VerListSvm.isEmpty() ? grlCfg.VerListSvm : fGrl->versionSvm();
					updates_svm.file        = xml.attributes().value("file").toString();
					updates_svm.url         = "";
					updates_svm.tipo        = "list_svm";
					updates_svm.isNew       = fGrl->version_compare(updates_svm.ver_old, updates_svm.version).isNew;
					updates_svm.isInstalled = true;
					updates_svm.info        = xml.readElementText();
					updates_svm.info_item   = "";
				} else
					xml.skipCurrentElement();
			}
		} else {
			xml.raiseError(tr("The file is not an XBEL version 1.0 file."));
		}
	}
	xml.clear();

	int respuesta = 0;
	if (isUpdateMenu)
	{
		if (!isNuevaVersionGRlida && !isNuevaVersionListSvm && !isNuevosScript && !isNuevosThemes)
			respuesta = fGrl->questionMsg(tr("Actualizaciones"), tr("No existen actualizaciones."), tr("Mostrar"), tr("Aceptar"));
	}

	if (isNuevaVersionGRlida || isNuevaVersionListSvm || isNuevosScript || (isNuevosThemes && grlCfg.chkUpdateThemes) || respuesta)
	{
		frmUpdate *UpdateNew = new frmUpdate(updates_grl, updates_js, updates_theme, updates_svm, grlCfg, this);
		UpdateNew->exec();
		grlCfg = UpdateNew->getGrlCfg();
		delete UpdateNew;
	}
}

void GrLida::changeMediaConfig()
{
	grlCfg.VerPlaylistVideoMain = mediaVideo->getGrlCfg().VerPlaylistVideoMain;
	grlCfg.VerPlaylistSoundMain = mediaSound->getGrlCfg().VerPlaylistSoundMain;
	grlCfg.VerInfoMediaMain     = mediaSound->getGrlCfg().VerInfoMediaMain;
}

void GrLida::cargarListaShortCut()
{
	foreach (QAction *action, ui->tb_shortcut->actions())
	{
		ui->tb_shortcut->removeAction(action);
		grpActions->removeAction(action);
	}

	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT titulo, key_sequence, sql_query, img, separador FROM dbgrl_mnu_shortcut WHERE mostrar = 'true' ORDER By orden ASC;");
	if (sql->chequearQuery(query) && query.first())
	{
		do {
			if (fGrl->strToBool(query.record().value("separador").toString()))
				ui->tb_shortcut->addSeparator();
			//	container->addSeparator();
			else {
				action = new QAction(ui->tb_shortcut);
				action->setText(query.record().value("titulo").toString());
				action->setData(query.record().value("sql_query").toString());
				action->setIcon(QIcon(fGrl->theme() +"img32/"+ query.record().value("img").toString()));
				action->setShortcut(QKeySequence(query.record().value("key_sequence").toString()));
				action->setCheckable(true);
				action->setChecked(false);
				grpActions->addAction(action);
				ui->tb_shortcut->addAction(action);
			//	container->addAction(action);
			}
		} while (query.next());
	}

	if (grpActions->actions().count() > 0)
	{
		if (grlCfg.LastSelectShortCutID < grpActions->actions().count())
			grpActions->actions().at(grlCfg.LastSelectShortCutID)->setChecked(true);
		else
			grpActions->actions().at(0)->setChecked(true);
	}
//qDebug() << "tb_s count: " << ui->tb_shortcut->actions().count();
//qDebug() << "grpa count: " << grpActions->actions().count();
//qDebug() << "sql: " << ui->tb_shortcut->actions().at(3)->data().toString();
}

// Carga las categorias de la base de datos.
void GrLida::cargarListaCategorias()
{
	QString ico_size = (grlCfg.ToolbarBigIcon) ? "24" : "16";
	QList<stGrlCats> list_cat = sql->getCategorias();

	categoria.clear();
	ui->cbxCategoriasTb->clear();
	ui->cbxCategorias->clear();
	const int listCatSize = list_cat.size();
	for (int i = 0; i < listCatSize; ++i)
	{
		categoria.insert(list_cat.at(i).id.toInt(), list_cat.at(i));

		QString titulo_cat = QString("%1 (%2)").arg(list_cat.at(i).titulo).arg(list_cat.at(i).total);

		if (QFile::exists(fGrl->theme() +"img"+ ico_size +"/"+ list_cat.at(i).img))
			ui->cbxCategoriasTb->addItem(QIcon(fGrl->theme() +"img"+ ico_size +"/"+ list_cat.at(i).img), titulo_cat, list_cat.at(i).id);
		else
			ui->cbxCategoriasTb->addItem(QIcon(":/img"+ ico_size +"/sinimg.png"), titulo_cat, list_cat.at(i).id);

		if (QFile::exists(fGrl->theme() +"img32/"+ list_cat.at(i).img))
			ui->cbxCategorias->addItem(QIcon(fGrl->theme() +"img32/"+ list_cat.at(i).img), titulo_cat, list_cat.at(i).id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img32/sinimg.png"), titulo_cat, list_cat.at(i).id);
	}

	int id_index = ui->cbxCategoriasTb->findData(id_cat);
	if (id_index < 0)
		id_index = 0;

	ui->cbxCategoriasTb->setCurrentIndex(id_index);
	emit on_cbxCategoriasTb_activated(id_index);
}

void GrLida::setConfigHeaderListaJuegos()
{
	data_model->setHorizontalHeaderItem(col_Icono        , nuevoStdrItem(tr("Icono")        , fGrl->theme() +"img16/bullet_black.png"  ));
	data_model->setHorizontalHeaderItem(col_Titulo       , nuevoStdrItem(tr("Título")       , fGrl->theme() +"img16/tag.png"           ));
	data_model->setHorizontalHeaderItem(col_Subtitulo    , nuevoStdrItem(tr("Subtítulo")    , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Genero       , nuevoStdrItem(tr("Genero")       , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Compania     , nuevoStdrItem(tr("Compañia")     , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Desarrollador, nuevoStdrItem(tr("Desarrollador"), fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Tema         , nuevoStdrItem(tr("Tema")         , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Grupo        , nuevoStdrItem(tr("Grupo")        , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Perspectiva  , nuevoStdrItem(tr("Perspectiva")  , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Idioma       , nuevoStdrItem(tr("Idioma")       , fGrl->theme() +"img16/idiomas.png"       ));
	data_model->setHorizontalHeaderItem(col_IdiomaVoces  , nuevoStdrItem(tr("Idioma Voces") , fGrl->theme() +"img16/idiomas.png"       ));
	data_model->setHorizontalHeaderItem(col_Formato      , nuevoStdrItem(tr("Formato")      , fGrl->theme() +"img16/cartucho.png"      ));
	data_model->setHorizontalHeaderItem(col_Anno         , nuevoStdrItem(tr("Año")          , fGrl->theme() +"img16/fecha.png"         ));
	data_model->setHorizontalHeaderItem(col_NumDisc      , nuevoStdrItem(tr("Nº Discos")    , fGrl->theme() +"img16/cd_iso.png"        ));
	data_model->setHorizontalHeaderItem(col_SistemaOp    , nuevoStdrItem(tr("Sistema")      , fGrl->theme() +"img16/cat/pc.png"        ));
	data_model->setHorizontalHeaderItem(col_Tamano       , nuevoStdrItem(tr("Tamaño")       , fGrl->theme() +"img16/datos_3.png"       ));
	data_model->setHorizontalHeaderItem(col_Graficos     , nuevoStdrItem(tr("Gráficos")     , fGrl->theme() +"img16/img_tv2x.png"      ));
	data_model->setHorizontalHeaderItem(col_Sonido       , nuevoStdrItem(tr("Sonido")       , fGrl->theme() +"img16/audio.png"         ));
	data_model->setHorizontalHeaderItem(col_Jugabilidad  , nuevoStdrItem(tr("Jugabilidad")  , fGrl->theme() +"img16/controller.png"    ));
	data_model->setHorizontalHeaderItem(col_Original     , nuevoStdrItem(tr("Original")     , fGrl->theme() +"img16/original.png"      ));
	data_model->setHorizontalHeaderItem(col_Estado       , nuevoStdrItem(tr("Estado")       , fGrl->theme() +"img16/bullet_black.png"  ));
	data_model->setHorizontalHeaderItem(col_Fecha        , nuevoStdrItem(tr("Fecha")        , fGrl->theme() +"img16/fecha.png"         ));
	data_model->setHorizontalHeaderItem(col_TipoEmu      , nuevoStdrItem(tr("Tipo Emu")     , fGrl->theme() +"img16/dosbox-scummvm.png"));
	data_model->setHorizontalHeaderItem(col_Favorito     , nuevoStdrItem(tr("Favorito")     , fGrl->theme() +"img16/"+ grlCfg.IconoFav ));
	data_model->setHorizontalHeaderItem(col_Rating       , nuevoStdrItem(tr("Rating")       , fGrl->theme() +"img16/star_on.png"       ));
	data_model->setHorizontalHeaderItem(col_Edad         , nuevoStdrItem(tr("Edad")         , fGrl->theme() +"img16/edad_nd.png"       ));
	data_model->setHorizontalHeaderItem(col_Usuario      , nuevoStdrItem(tr("Usuario")      , fGrl->theme() +"img16/edad_nd.png"       ));

	if (grlCfg.main_tvJuegos_state.isEmpty())
	{
	// Tamaño de la columnas
		ui->tvJuegos->setColumnWidth(col_Icono, lwIconCfg.tw_icon_width + 8);
		ui->tvJuegos->setColumnWidth(col_Titulo       , 360);
		ui->tvJuegos->setColumnWidth(col_Subtitulo    , 100);
		ui->tvJuegos->setColumnWidth(col_Genero       , 100);
		ui->tvJuegos->setColumnWidth(col_Compania     , 100);
		ui->tvJuegos->setColumnWidth(col_Desarrollador, 100);
		ui->tvJuegos->setColumnWidth(col_Tema         , 100);
		ui->tvJuegos->setColumnWidth(col_Grupo        , 100);
		ui->tvJuegos->setColumnWidth(col_Perspectiva  , 100);
		ui->tvJuegos->setColumnWidth(col_Idioma       , 100);
		ui->tvJuegos->setColumnWidth(col_IdiomaVoces  , 100);
		ui->tvJuegos->setColumnWidth(col_Formato      , 100);
		ui->tvJuegos->setColumnWidth(col_Anno         ,  76);
		ui->tvJuegos->setColumnWidth(col_NumDisc      , 100);
		ui->tvJuegos->setColumnWidth(col_SistemaOp    , 100);
		ui->tvJuegos->setColumnWidth(col_Tamano       , 100);
		ui->tvJuegos->setColumnWidth(col_Graficos     ,  90);
		ui->tvJuegos->setColumnWidth(col_Sonido       ,  90);
		ui->tvJuegos->setColumnWidth(col_Jugabilidad  ,  90);
		ui->tvJuegos->setColumnWidth(col_Original     ,  72);
		ui->tvJuegos->setColumnWidth(col_Estado       , 100);
		ui->tvJuegos->setColumnWidth(col_Fecha        , 112);
		ui->tvJuegos->setColumnWidth(col_TipoEmu      ,  76);
		ui->tvJuegos->setColumnWidth(col_Favorito     ,  70);
		ui->tvJuegos->setColumnWidth(col_Rating       ,  85);
		ui->tvJuegos->setColumnWidth(col_Edad         ,  56);
		ui->tvJuegos->setColumnWidth(col_Usuario      , 100);
	} else
		ui->tvJuegos->header()->restoreState(grlCfg.main_tvJuegos_state);

	ui->tvJuegos->header()->setMinimumSectionSize(32);
	ui->tvJuegos->header()->setStretchLastSection(false);
	ui->tvJuegos->setColumnHidden(col_IdGrl, true);

// Ocultacion columnas
	emit on_mnu_ver_col_icono_triggered(grlCfg.ver_col_Icono);
	emit on_mnu_ver_col_subtitulo_triggered(grlCfg.ver_col_Subtitulo);
	emit on_mnu_ver_col_genero_triggered(grlCfg.ver_col_Genero);
	emit on_mnu_ver_col_compania_triggered(grlCfg.ver_col_Compania);
	emit on_mnu_ver_col_desarrollador_triggered(grlCfg.ver_col_Desarrollador);
	emit on_mnu_ver_col_tema_triggered(grlCfg.ver_col_Tema);
	emit on_mnu_ver_col_grupo_triggered(grlCfg.ver_col_Grupo);
	emit on_mnu_ver_col_perspectiva_triggered(grlCfg.ver_col_Perspectiva);
	emit on_mnu_ver_col_idioma_triggered(grlCfg.ver_col_Idioma);
	emit on_mnu_ver_col_idioma_voces_triggered(grlCfg.ver_col_IdiomaVoces);
	emit on_mnu_ver_col_formato_triggered(grlCfg.ver_col_Formato);
	emit on_mnu_ver_col_anno_triggered(grlCfg.ver_col_Anno);
	emit on_mnu_ver_col_numdisc_triggered(grlCfg.ver_col_NumDisc);
	emit on_mnu_ver_col_sistemaop_triggered(grlCfg.ver_col_SistemaOp);
	emit on_mnu_ver_col_tamano_triggered(grlCfg.ver_col_Tamano);
	emit on_mnu_ver_col_graficos_triggered(grlCfg.ver_col_Graficos);
	emit on_mnu_ver_col_sonido_triggered(grlCfg.ver_col_Sonido);
	emit on_mnu_ver_col_jugabilidad_triggered(grlCfg.ver_col_Jugabilidad);
	emit on_mnu_ver_col_original_triggered(grlCfg.ver_col_Original);
	emit on_mnu_ver_col_estado_triggered(grlCfg.ver_col_Estado);
	emit on_mnu_ver_col_fecha_triggered(grlCfg.ver_col_Fecha);
	emit on_mnu_ver_col_tipoemu_triggered(grlCfg.ver_col_TipoEmu);
	emit on_mnu_ver_col_favorito_triggered(grlCfg.ver_col_Favorito);
	emit on_mnu_ver_col_rating_triggered(grlCfg.ver_col_Rating);
	emit on_mnu_ver_col_edad_triggered(grlCfg.ver_col_Edad);
	emit on_mnu_ver_col_usuario_triggered(grlCfg.ver_col_Usuario);
}

// Abre y carga la base de datos.
void GrLida::cargarListaJuegosDB(QString sql_where)
{
	grlCfg.main_tvJuegos_state = ui->tvJuegos->header()->saveState();

	if (grlCfg.db_orden_by.isEmpty())
		grlCfg.db_orden_by = "titulo";

	if (grlCfg.db_orden.isEmpty())
		grlCfg.db_orden = "ASC";

	QSqlQuery query(sql->getSqlDB());

	isSelectRow  = false;
	total_juegos = 0;
	thumbs_list.clear();
	data_model->clear();
	QString strSQL = "SELECT idgrl, icono, titulo, titulo_guiones, subtitulo, genero, compania, desarrollador, tema, "
			"grupo, perspectiva, idioma, idioma_voces, formato, anno, numdisc, sistemaop, tamano, "
			"graficos, sonido, jugabilidad, original, estado, thumbs, fecha, tipo_emu, favorito, "
			"rating, edad_recomendada, usuario FROM "+ categoria[id_cat].tabla +" "+ sql_where +" "
			"ORDER BY "+ grlCfg.db_orden_by +" "+ grlCfg.db_orden;

	query.exec(strSQL);
	if (query.first())
	{
		do {
			stDatosJuego datos;
			datos.idgrl            = query.record().value("idgrl").toString();
			datos.icono            = query.record().value("icono").toString();
			datos.titulo           = query.record().value("titulo").toString();
			datos.titulo_guiones   = query.record().value("titulo_guiones").toString();
			datos.subtitulo        = query.record().value("subtitulo").toString();
			datos.genero           = query.record().value("genero").toString();
			datos.compania         = query.record().value("compania").toString();
			datos.desarrollador    = query.record().value("desarrollador").toString();
			datos.tema             = query.record().value("tema").toString();
			datos.grupo            = query.record().value("grupo").toString();
			datos.perspectiva      = query.record().value("perspectiva").toString();
			datos.idioma           = query.record().value("idioma").toString();
			datos.idioma_voces     = query.record().value("idioma_voces").toString();
			datos.formato          = query.record().value("formato").toString();
			datos.anno             = query.record().value("anno").toString();
			datos.numdisc          = query.record().value("numdisc").toString();
			datos.sistemaop        = query.record().value("sistemaop").toString();
			datos.tamano           = query.record().value("tamano").toString();
			datos.graficos         = query.record().value("graficos").toString();
			datos.sonido           = query.record().value("sonido").toString();
			datos.jugabilidad      = query.record().value("jugabilidad").toString();
			datos.original         = query.record().value("original").toString();
			datos.estado           = query.record().value("estado").toString();
			datos.thumbs           = query.record().value("thumbs").toString();
			datos.fecha            = query.record().value("fecha").toString();
			datos.tipo_emu         = query.record().value("tipo_emu").toString();
			datos.favorito         = query.record().value("favorito").toString();
			datos.rating           = query.record().value("rating").toString();
			datos.edad_recomendada = query.record().value("edad_recomendada").toString();
			datos.usuario          = query.record().value("usuario").toString();
			datos.game_dir         = "id-"+ datos.idgrl +"_"+ datos.titulo_guiones +"_"+ datos.tipo_emu;

			nuevoEditarDatosDelJuego(datos, true);
		} while (query.next());
	}
	query.clear();

	setConfigHeaderListaJuegos();

	if (total_juegos <= 0)
		setCurrentItem(-1);
	else if (!isSelectRow)
		setCurrentItem(0);

	if (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter)
		grl_picflow->setImagePaths(thumbs_list, select_row.row());

	int row_select = !IdGame.isEmpty() ? select_row.row() + 1 : 0;
	lb_panel_2->setText(" "+ tr("Nº juegos") +": "+ fGrl->intToStr(row_select) +" "+ tr("de") +" "+ fGrl->intToStr(total_juegos) +"  ");
}

void GrLida::setChangeCategorias(int cat_id)
{
	if (cat_id > -1)
		id_cat = cat_id;
	else
		id_cat = 0;

	grlCfg.LastSelectCatID = categoria[id_cat].id;

	lwIconCfg = fGrl->cargarListWidgetIconConf(categoria[id_cat].tabla);

	sql_where_select = "";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria[id_cat].tabla +"/";
	fGrl->comprobarDirectorio(grlDir.DatosDbGrl); // comprobar solo al crearla?

	grl_tv_delegate->setTheme(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height), grlCfg.IconoFav, fGrl->theme(), grlCfg.FormatsImage.join(";"));
	grl_lv_delegate->setTheme(lwIconCfg, fGrl->theme());
	grl_picflow->setSlideSize(QSize(lwIconCfg.pf_img_width, lwIconCfg.pf_img_height));
	grl_picflow->setSlideFixSize(lwIconCfg.pf_img_fixsize);
	ui->tvJuegos->setIconSize(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height));
	ui->lvJuegos->setIconSize(QSize(lwIconCfg.icon_width, lwIconCfg.icon_height));

	comprobarEmuloresDisp();
	crearMenuNav();
}

void GrLida::menuNavAddCat(QString etiqueta, QString dir_icono, QString icono, QString sql_query, bool m_expanded, bool m_show_total)
{
	QFont m_font;
	int total = 0;
	if (m_show_total)
		total = sql->getCount(categoria[id_cat].tabla, sql_query);

	twListMnuNav = new QTreeWidgetItem;

	if (QFile::exists(dir_icono + icono))
		twListMnuNav->setIcon(0, QIcon(dir_icono + icono));
	else
		twListMnuNav->setIcon(0, QIcon(dir_icono +"sinimg.png"));

	m_font.setBold((total > 0) ? true : false);

	twListMnuNav->setFont(1, m_font);
	twListMnuNav->setTextAlignment(1, Qt::AlignCenter);
	twListMnuNav->setText(0, etiqueta);
	twListMnuNav->setText(1, m_show_total ? fGrl->intToStr(total) : "");
	twListMnuNav->setText(2, sql_query);
	twListMnuNav->setText(3, icono);
	twListMnuNav->setText(4, fGrl->intToStr(ui->twListNav->topLevelItemCount()));

	ui->twListNav->addTopLevelItem(twListMnuNav);
	twListMnuNav->setExpanded(m_expanded);
}

void GrLida::menuNavAddSubCat(QString etiqueta, QString dir_icono, QString icono, QString sql_query, QString sql_col)
{
	QFont m_font;
	int total = 0;
	total = sql->getCount(categoria[id_cat].tabla, sql_query, sql_col, etiqueta);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem;

	if (QFile::exists(dir_icono + icono))
		sub_cat->setIcon(0, QIcon(dir_icono + icono));
	else
		sub_cat->setIcon(0, QIcon(dir_icono +"sinimg.png"));

	m_font.setBold((total > 0) ? true : false);

	sub_cat->setFont(1, m_font);
	sub_cat->setTextAlignment(1, Qt::AlignCenter);
	sub_cat->setText(0, etiqueta);
	sub_cat->setText(1, fGrl->intToStr(total));
	sub_cat->setText(2, sql_query);
	sub_cat->setText(3, icono);
	sub_cat->setText(4, twListMnuNav->text(4));

	twListMnuNav->addChild(sub_cat);
}

void GrLida::crearMenuNav()
{
	ui->twListNav->clear();

	menuNavAddCat(tr("Todos"), fGrl->theme() +"img16/", "gr-lida.png", "", false, true);
	menuNavAddCat(tr("Favoritos"), fGrl->theme() +"img16/", grlCfg.IconoFav, "WHERE favorito='true'", false);
	menuNavAddCat(tr("Originales"), fGrl->theme() +"img16/", "original.png" , "WHERE original='true'", false);

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);

	menuNavAddCat(tr("Tipo emulador"), fGrl->theme() +"img16/", "tag.png", "", true, false);
	if (lista.isEmpty() || lista.contains("all") || lista.contains("dosbox"))
		menuNavAddSubCat("DOSBox", fGrl->theme() +"img16/", "cat/dosbox.png", "WHERE tipo_emu='dosbox'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("scummvm"))
		menuNavAddSubCat("ScummVM", fGrl->theme() +"img16/", "cat/scummvm.png", "WHERE tipo_emu='scummvm'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound"))
		menuNavAddSubCat("VDMSound", fGrl->theme() +"img16/", "cat/vdmsound.png", "WHERE tipo_emu='vdmsound'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("datos"))
		menuNavAddSubCat("Datos", fGrl->theme() +"img16/", "datos_3.png", "WHERE tipo_emu='datos'");

	emu_list.clear();
	const int listEmuSize = list_emu.size();
	for (int i = 0; i < listEmuSize; ++i)
	{
		if (lista.isEmpty() || lista.contains("all") || lista.contains(list_emu.at(i).key))
		{
			emu_list.insert(list_emu.at(i).key, list_emu.at(i));
			menuNavAddSubCat(list_emu.at(i).titulo, fGrl->theme() +"img16/", list_emu.at(i).icono, "WHERE tipo_emu='"+ list_emu.at(i).key +"'");
		}
	}

	sql->cargarMenuNav(ui->twListNav, categoria[id_cat].tabla);

	if (fGrl->comprobarItemTwList(ui->twListNav, grlCfg.LastSelectNavID, grlCfg.LastSelectNavSubID))
		emit on_twListNav_itemClicked(ui->twListNav->currentItem(), 0);
	else {
		isNavItemChanged = true;
		cargarListaJuegosDB(sql_where_select);
	}
}

void GrLida::nuevoEditarDatosDelJuego(stDatosJuego datos, bool isNew, bool isMenu)
{
	QString img_thumb = fGrl->theme() +"images/juego_sin_imagen.png";
	QString datosGame = grlDir.DatosDbGrl + datos.game_dir +"/";

	int row = isNew ? data_model->rowCount() : select_row.row();
	for (int col = 0; col < 28; ++col)
	{
		QStandardItem *item = new QStandardItem();
		switch (col)
		{
			case col_IdGrl:
			{
				if (!datos.thumbs.isEmpty() && QFile::exists(datosGame + datos.thumbs))
					img_thumb = datosGame + datos.thumbs;

				item->setData(datos.idgrl           , IdGrlRole   );
				item->setData(QPixmap(img_thumb)    , ThumbsRole  );
				item->setData(datos.icono           , IconRole    );
				item->setData(datos.original        , OriginalRole);
				item->setData(datos.tipo_emu        , TipoEmuRole );
				item->setData(datos.favorito        , FavoritoRole);
				item->setData(datos.rating          , RatingRole  );
				item->setData(datos.edad_recomendada, EdadRole    );
				item->setText("");

				if (isNew)
				{
					GrlPicFlow::stListPicFlow lpicflow;
					lpicflow.caption = datos.titulo;
					lpicflow.path    = img_thumb;
					thumbs_list << lpicflow;

					if (isMenu)
					{
						if (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter)
							grl_picflow->addImageSlide(img_thumb, datos.titulo);
					}
				}
				if (!isNew && (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter))
					grl_picflow->setImageSlide(row, img_thumb, datos.titulo);
			}
			break;
			case col_Icono: item->setText(""); break;
			case col_Titulo: item->setText(datos.titulo); break;
			case col_Subtitulo: item->setText(datos.subtitulo); break;
			case col_Genero: item->setText(datos.genero); break;
			case col_Compania: item->setText(datos.compania); break;
			case col_Desarrollador: item->setText(datos.desarrollador); break;
			case col_Tema: item->setText(datos.tema); break;
			case col_Grupo: item->setText(datos.grupo); break;
			case col_Perspectiva: item->setText(datos.perspectiva); break;
			case col_Idioma: item->setText(datos.idioma); break;
			case col_IdiomaVoces: item->setText(datos.idioma_voces); break;
			case col_Formato: item->setText(datos.formato); break;
			case col_Anno: item->setText(datos.anno); break;
			case col_NumDisc: item->setText(datos.numdisc); break;
			case col_SistemaOp: item->setText(datos.sistemaop); break;
			case col_Tamano: item->setText(datos.tamano); break;
			case col_Graficos: item->setText(datos.graficos); break;
			case col_Sonido: item->setText(datos.sonido); break;
			case col_Jugabilidad: item->setText(datos.jugabilidad); break;
			case col_Original: item->setText(""); break;
			case col_Estado: item->setText(datos.estado); break;
			case col_Fecha: item->setText(datos.fecha); break;
			case col_TipoEmu: item->setText(datos.tipo_emu); break;
			case col_Favorito: item->setText(""); break;
			case col_Rating: item->setText(""); break;
			case col_Edad: item->setText(""); break;
			case col_Usuario: item->setText(datos.usuario); break;
			default: item->setText(""); break;
		};
		data_model->setItem(row, col, item);
	}

	if (isNew)
		++total_juegos;
	else
		fGrl->eliminarArchivo(datosGame +"/ficha.html");

	if (isNew && isMenu)
	{
		++categoria[id_cat].total;
		QString titulo_cat = QString("%1 (%2)").arg(categoria[id_cat].titulo).arg(categoria[id_cat].total);
		ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->currentIndex(), titulo_cat);
		ui->cbxCategorias->setItemText(ui->cbxCategorias->currentIndex(), titulo_cat);
	}

	if ((grlCfg.LastSelectGameID == datos.idgrl && grlCfg.LastSelectCatID == categoria[id_cat].id) || isMenu)
	{
		if (!isImportar)
		{
			setCurrentItem(row);
			isSelectRow = true;
		}
	}
}

QStandardItem* GrLida::nuevoStdrItem(QString nombre, QString icono)
{
	QStandardItem *item = new QStandardItem();
	if (!icono.isEmpty())
		item->setIcon(QIcon(icono));
	item->setText(nombre);
	return item;
}

void GrLida::listItemSelect(const QModelIndex &index, QString info)
{
	Q_UNUSED(info);

	if (index.isValid())
	{
		select_row = data_model->index(index.row(), col_IdGrl);
		int row = select_row.row();

		if (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter)
			grl_picflow->showSlide(row);

		IdGame = select_row.sibling(row, col_IdGrl).data(IdGrlRole).toString();
		grlCfg.LastSelectGameID = IdGame;
	} else
		IdGame = "";

	mostrarDatosDelJuego(IdGame);
}

void GrLida::listItemSelectDoubleClicked(const QModelIndex &index, QString info)
{
	Q_UNUSED(info);

	if (index.isValid())
	{
		IdGame = index.sibling(index.row(), col_IdGrl).data(IdGrlRole).toString();
		emit on_mnu_ejecutar_juego_triggered();
	}
}

void GrLida::mostrarDatosDelJuego(QString IDitem, bool soloInfo)
{
	cfgExec.f_exe         = "";
	cfgExec.f_exe_param   = "";
	cfgExec.f_exe_path    = "";
	cfgExec.f_setup       = "";
	cfgExec.f_setup_param = "";
	cfgExec.f_setup_path  = "";
	cfgExec.isCfgExec     = false;
	cfgExec.isCfgSetup    = false;

	// Desmontamos la imagen si previamente está montada.
	ejecutar_desmontaje();

	if (!IDitem.isEmpty())
	{
		if (!soloInfo)
		{
			if (!pathCapturas.isEmpty())
				fs_watcher.removePath(pathCapturas);
			if (!pathImagenes.isEmpty())
				fs_watcher.removePath(pathImagenes);
			if (!pathVideos.isEmpty())
				fs_watcher.removePath(pathVideos);
			if (!pathSonidos.isEmpty())
				fs_watcher.removePath(pathSonidos);
		}

		stDatosJuego datos = sql->show_Datos(categoria[id_cat].tabla, IDitem);
		TipoEmu    = datos.tipo_emu;
		CoverFront = datos.cover_front;
		CoverBack  = datos.cover_back;

		grlDir.DatosGame = grlDir.DatosDbGrl + datos.game_dir +"/";

		ui->mnu_tool_cambiar_categoria->setEnabled((categoria.size() > 1 ? true : false));
		ui->mnu_edit_editar->setEnabled(true);
		ui->mnu_edit_editar_big->setEnabled(true);
		ui->mnu_edit_eliminar->setEnabled(true);
		ui->mnu_edit_eliminar_big->setEnabled(true);
		ui->mnu_edit_favorito->setEnabled(true);
		ui->mnu_edit_favorito->setChecked(fGrl->strToBool(datos.favorito));
		ui->mnu_edit_original->setEnabled(true);
		ui->mnu_edit_original->setChecked(fGrl->strToBool(datos.original));

		if (!soloInfo)
		{
			if (grlCfg.Pnl_Files)
				sql->cargarDatosFiles(ui->twFiles, IDitem, categoria[id_cat].id);
			if (grlCfg.Pnl_Urls)
				sql->cargarDatosUrls(ui->twUrls, IDitem, categoria[id_cat].id);

			pathCapturas = fGrl->getDirRelative(grlDir.DatosGame +"capturas/");
			pathImagenes = fGrl->getDirRelative(grlDir.DatosGame +"imagenes/");
			pathVideos   = fGrl->getDirRelative(grlDir.DatosGame +"videos/");
			pathSonidos  = fGrl->getDirRelative(grlDir.DatosGame +"sonidos/");

			emit directoryChanged(pathCapturas);
			emit directoryChanged(pathImagenes);

			if (!fs_watcher.directories().contains(pathCapturas))
				fs_watcher.addPath(pathCapturas);
			if (!fs_watcher.directories().contains(pathImagenes))
				fs_watcher.addPath(pathImagenes);
			if (!fs_watcher.directories().contains(pathVideos))
				fs_watcher.addPath(pathVideos);
			if (!fs_watcher.directories().contains(pathSonidos))
				fs_watcher.addPath(pathSonidos);
		}

		QString tipo_emu      = datos.tipo_emu;
		QString img_tipo_emu  = "<img src=\"emu_rs_"+ tipo_emu +"\"> ";
		QString dbx_usado     = "";
		QString dbx_usado_img = "";

		if (TipoEmu == "datos")
			tipo_emu = tr("Datos");
		else if (TipoEmu == "scummvm")
			tipo_emu = "ScummVM";
		else if (TipoEmu == "vdmsound")
			tipo_emu = "VDMSound";
		else if (TipoEmu == "dosbox")
		{
			QHash<QString, QString> conf_dosbox = sql->getDatos("dbgrl_emu_dosbox", "WHERE idgrl="+ IdGame +" AND idcat="+ categoria[id_cat].id, "dosbox_emu_key");

			stGrlDatos dbx_dato;
			if (dbx_list.contains(conf_dosbox["dosbox_emu_key"]))
			{
				dbx_dato = dbx_list[conf_dosbox["dosbox_emu_key"]];
			} else {
				const int num_dbx_count = dbx_list.count(grlCfg.DOSBoxDefault);
				if (num_dbx_count > 0)
					dbx_dato = dbx_list[grlCfg.DOSBoxDefault];
				else {
					QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
					dbx_dato = i_dbx.value();
				}
			}

			dbx_usado     = dbx_dato.titulo;
			dbx_usado_img = dbx_dato.icono;
			tipo_emu      = "DOSBox";
		} else {
			if (emu_list.contains(TipoEmu))
			{
				tipo_emu     = emu_list[TipoEmu].titulo;
				img_tipo_emu = "<img src=\"emu_rs_"+ emu_list[TipoEmu].key +"\"> ";
			} else {
				tipo_emu     = tr("Otro emulador");
				img_tipo_emu = "<img src=\"emu_rs_emuladores\"> ";
			}
		}

		QString dat_icono = ":/img24/sinimg.png";
		if (!datos.icono.isEmpty())
		{
			if (datos.icono.startsWith("<dir_iconos>/", Qt::CaseInsensitive))
				dat_icono = datos.icono.replace("<dir_iconos>/", grlDir.Iconos);
			else
				dat_icono = fGrl->theme() +"img24/"+ datos.icono;

			if (!QFile::exists(dat_icono))
				dat_icono = ":/img24/sinimg.png";
		}

		iconExtraImg = dat_icono;

		if (grlCfg.Pnl_Datos)
		{
			QString tpl_info_game_new = "";
			if (!QFile::exists(grlDir.DatosDbGrl + datos.game_dir +"/ficha.html"))
			{
				QString dat_idioma          = fGrl->getImgDatos(idiomas_list, datos.idioma.split(";")      , true);
				QString dat_idioma_voces    = fGrl->getImgDatos(idiomas_list, datos.idioma_voces.split(";"), true);
				QString dat_img_edad_titulo = edades_list[datos.edad_recomendada].titulo;
				QString dat_img_edad        = "<img src=\"edad_rs_"+ edades_list[datos.edad_recomendada].icono +"\" alt=\""+ dat_img_edad_titulo +"\" title=\""+ dat_img_edad_titulo +"\"> ";
				QString dat_comentario      = fGrl->reemplazaTextoSmiles(datos.comentario, smiles_list);

				QString dat_thumbs = fGrl->theme() +"images/juego_sin_imagen.png";
				if (!datos.thumbs.isEmpty() && QFile::exists(grlDir.DatosGame + datos.thumbs))
					dat_thumbs = grlDir.DatosGame + datos.thumbs;

				QString dat_rating = "";
				for (int i=0; i < 5; ++i)
				{
					if (i < datos.rating.toInt())
						dat_rating.append("<img src=\"img_rs_star_on.png\" alt=\""+ fGrl->intToStr(i+1) +"\" title=\""+ fGrl->intToStr(i+1) +"\"> ");
					else
						dat_rating.append("<img src=\"img_rs_star_off.png\"> ");
				}

				QString img_original = "<img src=\"img_rs_original_off.png\"> ";
				if (fGrl->strToBool(datos.original))
					img_original = "<img src=\"img_rs_original.png\"> ";

				QString img_favorito = "<img src=\"img_rs_favorito_off.png\"> ";
				if (fGrl->strToBool(datos.favorito))
					img_favorito = "<img src=\"img_rs_favorito.png\"> ";

				QString img_gamepad = "<img src=\"img_rs_gamepad_off.png\"> ";
				if (fGrl->strToBool(datos.gamepad))
					img_gamepad = "<img src=\"img_rs_gamepad.png\"> ";

				tpl_info_game_new = tpl_info_game_old;
				tpl_info_game_new.replace("{info_icono}"               , "<img src=\""+ QUrl::fromLocalFile(dat_icono).path() +"\">"); //  width=\"24\" height=\"24\"
				tpl_info_game_new.replace("{info_titulo}"              , datos.titulo       );
				tpl_info_game_new.replace("{info_subtitulo}"           , datos.subtitulo    );
				tpl_info_game_new.replace("{info_genero}"              , datos.genero       );
				tpl_info_game_new.replace("{info_compania}"            , datos.compania     );
				tpl_info_game_new.replace("{info_desarrollador}"       , datos.desarrollador);
				tpl_info_game_new.replace("{info_tema}"                , datos.tema         );
				tpl_info_game_new.replace("{info_grupo}"               , datos.grupo        );
				tpl_info_game_new.replace("{info_perspectiva}"         , datos.perspectiva  );
				tpl_info_game_new.replace("{info_idioma}"              , dat_idioma         );
				tpl_info_game_new.replace("{info_idioma_voces}"        , dat_idioma_voces   );
				tpl_info_game_new.replace("{info_formato}"             , datos.formato      );
				tpl_info_game_new.replace("{info_anno}"                , datos.anno         );
				tpl_info_game_new.replace("{info_numdisc}"             , datos.numdisc      );
				tpl_info_game_new.replace("{info_img_edad_recomendada}", dat_img_edad       );
				tpl_info_game_new.replace("{info_edad_recomendada}"    , dat_img_edad_titulo);
				tpl_info_game_new.replace("{info_sistemaop}"           , datos.sistemaop    );
				tpl_info_game_new.replace("{info_tamano}"              , datos.tamano       );
				tpl_info_game_new.replace("{info_graficos}"            , datos.graficos     );
				tpl_info_game_new.replace("{info_sonido}"              , datos.sonido       );
				tpl_info_game_new.replace("{info_jugabilidad}"         , datos.jugabilidad  );
				tpl_info_game_new.replace("{info_original}"            , fGrl->strToBool(datos.original) ? tr("Si") : tr("No"));
				tpl_info_game_new.replace("{img_original}"             , img_original       );
				tpl_info_game_new.replace("{info_estado}"              , datos.estado       );
				tpl_info_game_new.replace("{info_thumbs}"              , QUrl::fromLocalFile(dat_thumbs).path());
				tpl_info_game_new.replace("{info_fecha}"               , fGrl->horaFechaActual(datos.fecha, grlCfg.FormatoFecha));
				tpl_info_game_new.replace("{info_tipo_emu}"            , tipo_emu           );
				tpl_info_game_new.replace("{img_tipo_emu}"             , img_tipo_emu       );
				tpl_info_game_new.replace("{info_favorito}"            , fGrl->strToBool(datos.favorito) ? tr("Si") : tr("No"));
				tpl_info_game_new.replace("{img_favorito}"             , img_favorito       );
				tpl_info_game_new.replace("{info_gamepad}"             , fGrl->strToBool(datos.gamepad) ? tr("Si") : tr("No"));
				tpl_info_game_new.replace("{img_gamepad}"              , img_gamepad        );
				tpl_info_game_new.replace("{info_rating}"              , dat_rating         );
				tpl_info_game_new.replace("{info_usuario}"             , datos.usuario      );
				tpl_info_game_new.replace("{info_comentario}"          , dat_comentario     );
				tpl_info_game_new.replace("<dir_scripts>"              , grlDir.Scripts     );
				tpl_info_game_new.replace("<theme>"                    , fGrl->theme()      );
				tpl_info_game_new.replace("{dbx_usado}"                , dbx_usado          );
				tpl_info_game_new.replace("{dbx_usado_img}"            , dbx_usado_img      );

				fGrl->guardarArchivo(grlDir.DatosDbGrl + datos.game_dir +"/ficha.html", tpl_info_game_new, "UTF-8");
			} else {
				tpl_info_game_new = fGrl->leerArchivo(grlDir.DatosDbGrl + datos.game_dir +"/ficha.html", "UTF-8");
			}

			ui->txtInfo->setHtml(tpl_info_game_new);

			ui->btnVer_CoverFront->setEnabled(false);
			if (!CoverFront.isEmpty() && QFile::exists(grlDir.DatosGame +"caja/"+ CoverFront))
				ui->btnVer_CoverFront->setEnabled(true);

			ui->btnVer_CoverBack->setEnabled(false);
			if (!CoverBack.isEmpty() && QFile::exists(grlDir.DatosGame +"caja/"+ CoverBack))
				ui->btnVer_CoverBack->setEnabled(true);
		}

		if (!soloInfo)
		{
			if (TipoEmu == "datos")
			{
				cfgExec.f_exe       = fGrl->getDirRelative(datos.path_exe);
				cfgExec.f_exe_param = fGrl->getParametrosEXE(datos.parametros_exe);
				cfgExec.f_exe_path  = fGrl->getInfoFile(cfgExec.f_exe).Path;

				cfgExec.f_setup       = fGrl->getDirRelative(datos.path_setup);
				cfgExec.f_setup_param = fGrl->getParametrosEXE(datos.parametros_setup);
				cfgExec.f_setup_path  = fGrl->getInfoFile(cfgExec.f_setup).Path;

				cfgExec.isCfgExec = false;
				if (!datos.path_exe.isEmpty() && QFile::exists(cfgExec.f_exe))
					cfgExec.isCfgExec = true;
			}

			if (TipoEmu == "datos" || TipoEmu == "dosbox" || TipoEmu == "vdmsound")
			{
				if (datos.path_image.isEmpty() || datos.virtual_drive == "NO_VIRTUAL_DRIVE")
				{
					ui->mnu_ejecutar_montaje->setEnabled(false);

					lb_panel_7->setVisible(false);
				} else {
					ui->mnu_ejecutar_montaje->setEnabled(true);

					lb_panel_7->setPixmap(QPixmap(fGrl->theme() +"img16/cd_iso.png"));
					lb_panel_7->setVisible(true);
				}
			}

			cargarConfigEmu(TipoEmu);
		}

		lb_panel_4->setText(" "+ tipo_emu +" ");
		lb_panel_6->setText(" "+ datos.titulo +" - "+ tr("introducido el") +" "+ fGrl->horaFechaActual(datos.fecha, grlCfg.FormatoFecha) +" ");
	} else {
		IdGame       = "";
		TipoEmu      = "";
		CoverFront   = "";
		CoverBack    = "";
		pathCapturas = "";
		pathImagenes = "";
		pathVideos   = "";
		pathSonidos  = "";
		iconExtraImg = ":/img24/sinimg.png";

		ui->twFiles->clear();
		ui->twUrls->clear();
		ui->lwVideos->clear();
		mediaSound->clearPlaylist();
		ui->lwCapturas->clear();
		ui->lwImagenes->clear();

		ui->btnVer_CoverFront->setEnabled(false);
		ui->btnVer_CoverBack->setEnabled(false);

		ui->mnu_tool_cambiar_categoria->setEnabled(false);
		ui->mnu_edit_editar->setEnabled(false);
		ui->mnu_edit_editar_big->setEnabled(false);
		ui->mnu_edit_eliminar->setEnabled(false);
		ui->mnu_edit_eliminar_big->setEnabled(false);
		ui->mnu_edit_favorito->setEnabled(false);
		ui->mnu_edit_favorito->setChecked(false);
		ui->mnu_edit_original->setEnabled(false);
		ui->mnu_edit_original->setChecked(false);

		cargarConfigEmu();

		QString tpl_info_game_new_empty = tpl_info_game_empty_old;
		ui->txtInfo->setHtml(tpl_info_game_new_empty);

		lb_panel_6->setText(" ");
	}

	// Opción para el icono extra del pictureflow
	setSlideIconExtra();

	int row_select = !IdGame.isEmpty() ? select_row.row() + 1 : 0;
	lb_panel_2->setText(" "+ tr("Nº juegos") +": "+ fGrl->intToStr(row_select) +" "+ tr("de") +" "+ fGrl->intToStr(total_juegos) +"  ");
}

void GrLida::itemNext()
{
	int pos = select_row.row();
	if (total_juegos > 0 && pos > -1)
	{
		if (pos >= total_juegos - 1)
			setCurrentItem(0);
		else
			setCurrentItem(pos + 1);
	}
}

void GrLida::itemBack()
{
	int pos = select_row.row();
	if (total_juegos > 0 && pos > -1)
	{
		if (pos <= 0)
			setCurrentItem(total_juegos - 1);
		else
			setCurrentItem(pos - 1);
	}
}

void GrLida::setCurrentItem(int index)
{
	if (index > -1)
	{
		select_row = data_model->index(index, col_IdGrl);
		if (select_row.isValid())
		{
			if (grlCfg.ver_IconMode)
			{
				ui->lvJuegos->clearSelection();
				ui->lvJuegos->setCurrentIndex(select_row);
				ui->lvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
			} else {
				ui->tvJuegos->clearSelection();
				ui->tvJuegos->setCurrentIndex(select_row);
				ui->tvJuegos->scrollTo(select_row, QAbstractItemView::PositionAtCenter);
			}
		} else {
			IdGame = "";
			mostrarDatosDelJuego(IdGame);
		}
	} else {
		IdGame = "";
		mostrarDatosDelJuego(IdGame);
	}
}

void GrLida::cargarConfigEmu(QString tipo_emu)
{
	if (tipo_emu.isEmpty())
	{
		ui->mnu_ejecutar_juego->setEnabled(false);
		ui->mnu_ejecutar_juego_big->setEnabled(false);
		ui->mnu_ejecutar_setup->setEnabled(false);
		ui->mnu_ejecutar_setup_big->setEnabled(false);

		lb_panel_3->setPixmap(QPixmap(fGrl->theme() +"img16/sinimg.png"));
	} else {
		lb_panel_3->setPixmap(QPixmap(fGrl->theme() +"img16/"+ tipo_emu +".png"));

		if (tipo_emu == "dosbox")
		{
			QHash<QString, QString> conf_dosbox = sql->getDatos("dbgrl_emu_dosbox", "WHERE idgrl="+ IdGame +" AND idcat="+ categoria[id_cat].id,
																"dosbox_emu_key, opt_consola_dbox, path_exe, path_setup, parametros_setup");

			if (conf_dosbox["dosbox_emu_key"].isEmpty() || !dbx_list.contains(conf_dosbox["dosbox_emu_key"]))
			{
				const int num_dbx_count = dbx_list.count(grlCfg.DOSBoxDefault);
				if (num_dbx_count > 0)
					conf_dosbox["dosbox_emu_key"] = grlCfg.DOSBoxDefault;
				else {
					QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
					conf_dosbox["dosbox_emu_key"] = i_dbx.key();
				}
			}

			conf_dosbox["path_conf"] = grlDir.Temp;
			if (grlCfg.DOSBoxSaveConfFile)
				conf_dosbox["path_conf"] = grlDir.DatosGame;

			cfgExec.f_exe       = fGrl->getDirRelative(dbx_list[conf_dosbox["dosbox_emu_key"]].extra);
			cfgExec.f_exe_param = "-conf|"+ conf_dosbox["path_conf"] +"dosbox.conf";
			cfgExec.f_exe_path  = fGrl->getInfoFile(fGrl->getDirRelative(conf_dosbox["path_exe"], "DosGames")).Path;

			if (!conf_dosbox["path_setup"].isEmpty())
			{
				cfgExec.f_setup       = cfgExec.f_exe;
				cfgExec.f_setup_param = "-conf|"+ conf_dosbox["path_conf"] +"dosbox-setup.conf";
				cfgExec.f_setup_path  = fGrl->getInfoFile(fGrl->getDirRelative(conf_dosbox["path_setup"], "DosGames")).Path;
			}

			#ifdef Q_OS_WIN
				if (fGrl->strToBool(conf_dosbox["opt_consola_dbox"]))
					cfgExec.f_exe_param.append("|-noconsole");
			#endif

			cfgExec.isCfgExec = false;
			if (!cfgExec.f_exe.isEmpty() && QFile::exists(cfgExec.f_exe))
			{
				if (!grlCfg.DOSBoxSaveConfFile)
						cfgExec.isCfgExec = true;
				else {
					if (QFile::exists(conf_dosbox["path_conf"]))
						cfgExec.isCfgExec = true;
				}
			}
		}
		else if (tipo_emu == "scummvm")
		{
			stConfigScummVM conf_scummvm = sql->showConfg_ScummVM(IdGame, categoria[id_cat].id);
			conf_scummvm.description = select_row.sibling(select_row.row(), col_Titulo).data().toString();

			QString dir_ini = grlDir.Temp +"scummvm.ini";
			if (grlCfg.ScummVMSaveConfFile)
				dir_ini = grlDir.DatosGame +"scummvm.ini";

			cfgExec.f_exe       = fGrl->getDirRelative(grlCfg.DirScummVM);
			cfgExec.f_exe_param = "-c"+ dir_ini +"|-d"+ conf_scummvm.debuglevel +"|"+ conf_scummvm.game_label;
			cfgExec.f_exe_path  = fGrl->getDirRelative(conf_scummvm.path, "DosGames");

			cfgExec.f_setup       = "";
			cfgExec.f_setup_param = "";
			cfgExec.f_setup_path  = "";

			cfgExec.isCfgExec = false;
			if (!cfgExec.f_exe.isEmpty() && QFile::exists(cfgExec.f_exe))
			{
				if (!grlCfg.ScummVMSaveConfFile)
					cfgExec.isCfgExec = true;
				 else {
					if (QFile::exists(dir_ini))
						cfgExec.isCfgExec = true;
				}
			}
		}
		else if (tipo_emu == "vdmsound")
		{
			stConfigVDMSound conf_vdmsound = sql->showConfg_VDMSound(IdGame, categoria[id_cat].id);

			cfgExec.f_exe       = grlDir.DatosGame +"vdmsound.vlp";
			cfgExec.f_exe_param = "";
			cfgExec.f_exe_path  = fGrl->getInfoFile(fGrl->getDirRelative(conf_vdmsound.path_exe)).Path;

			cfgExec.f_setup       = "";
			cfgExec.f_setup_param = "";
			cfgExec.f_setup_path  = "";

			cfgExec.isCfgExec = false;
			if (QFile::exists(cfgExec.f_exe))
				cfgExec.isCfgExec = true;
		} else {
			if (emu_list.contains(tipo_emu))
			{
				lb_panel_3->setPixmap(QPixmap(fGrl->theme() +"img16/"+ emu_list[tipo_emu].icono));

				QHash<QString, QString> conf_emu = sql->getDatos(categoria[id_cat].tabla, "WHERE idgrl="+ IdGame, "path_exe, parametros_exe, path_setup, parametros_setup");

				cfgExec.f_exe       = fGrl->getDirRelative(emu_list[tipo_emu].extra);
				cfgExec.f_exe_param = fGrl->getDirRelative(conf_emu["path_exe"]) +"|"+ conf_emu["parametros_exe"];
				cfgExec.f_exe_path  = fGrl->getInfoFile(cfgExec.f_exe).Path;

				cfgExec.f_setup       = ""; //fGrl->getDirRelative(conf_emu["path_setup"]);
				cfgExec.f_setup_param = ""; //conf_emu["parametros_setup"];
				cfgExec.f_setup_path  = ""; //fGrl->getInfoFile(cfgExec.f_setup).Path;

				cfgExec.isCfgExec = false;
				if (!cfgExec.f_exe.isEmpty() && QFile::exists(cfgExec.f_exe))
					cfgExec.isCfgExec = true;
			}
		}

		cfgExec.isCfgSetup = false;
		if (!cfgExec.f_setup.isEmpty() && QFile::exists(cfgExec.f_setup))
			cfgExec.isCfgSetup = true;

		ui->mnu_ejecutar_juego_big->setEnabled(cfgExec.isCfgExec);
		ui->mnu_ejecutar_juego->setEnabled(cfgExec.isCfgExec);
		ui->mnu_ejecutar_setup_big->setEnabled(cfgExec.isCfgSetup);
		ui->mnu_ejecutar_setup->setEnabled(cfgExec.isCfgSetup);

/*		qDebug() << "TipoEmu       : " << tipo_emu;
		qDebug() << "f_exe         : " << cfgExec.f_exe;
		qDebug() << "f_exe_param   : " << cfgExec.f_exe_param;
		qDebug() << "f_exe_path    : " << cfgExec.f_exe_path;
		qDebug() << "f_setup       : " << cfgExec.f_setup;
		qDebug() << "f_setup_param : " << cfgExec.f_setup_param;
		qDebug() << "f_setup_path  : " << cfgExec.f_setup_path;
		qDebug() << "----------------------------------------------------";*/
	}
}

void GrLida::comprobarEmuloresDisp()
{
	bool isWin, isEmuDisp, isEnabled;

	#if defined(Q_OS_WIN)
		isWin = true;
	#else
		isWin = false;
	#endif

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);

// DOSBox
	isEmuDisp = isEnabled = false;
	if (lista.isEmpty() || lista.contains("all") || lista.contains("dosbox"))
		isEmuDisp = true;

	if (isEmuDisp && !dbx_list.isEmpty())
	{
		stGrlDatos dbx_dato;
		int num_dbx = dbx_list.count(grlCfg.DOSBoxDefault);
		if (num_dbx > 0)
			dbx_dato = dbx_list[grlCfg.DOSBoxDefault];
		else {
			QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
			dbx_dato = i_dbx.value();
		}

		isEnabled = QFile::exists(fGrl->getDirRelative(dbx_dato.extra));
	}

	ui->mnu_ejecutar_dosbox->setEnabled(isEnabled);
	ui->mnu_edit_nuevo_dosbox->setEnabled(isEnabled);
	ui->mnu_edit_nuevo_dosbox_big->setEnabled(isEnabled);

// ScummVM
	isEmuDisp = isEnabled = false;
	if (lista.isEmpty() || lista.contains("all") || lista.contains("scummvm"))
		isEmuDisp = true;
	if (isEmuDisp && grlCfg.ScummVMDisp && QFile::exists(fGrl->getDirRelative(grlCfg.DirScummVM)))
		isEnabled = true;

	ui->mnu_ejecutar_scummvm->setEnabled(isEnabled);
	ui->mnu_edit_nuevo_scummvm->setEnabled(isEnabled);
	ui->mnu_edit_nuevo_scummvm_big->setEnabled(isEnabled);

// VDMSound
	isEmuDisp = isEnabled = false;
	if (lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound"))
		isEmuDisp = true;
	if (isEmuDisp && grlCfg.VDMSoundDisp && isWin)
		isEnabled = true;
	ui->mnu_edit_nuevo_vdmsound->setEnabled(isEnabled);
	ui->mnu_edit_nuevo_vdmsound_big->setEnabled(isEnabled);
}

void GrLida::ejecutar_desmontaje()
{
	if (isMountImage)
	{
		QString unmount_image = fGrl->getCommandLineMount(virtualDrive, false);

		fGrl->guardarArchivo(grlDir.Temp +"montaje.bat", "@echo off\n"+ unmount_image);
		montaje(grlDir.Temp +"montaje.bat", unmount_image, grlDir.Temp);

		isMountImage = false;
		ui->mnu_ejecutar_montaje->setText(tr("Montar imagen"));
	}

/*	qDebug() << "isMountImage           : " << isMountImage;
	qDebug() << "vd.titulo              : " << virtualDrive.titulo;
	qDebug() << "vd.etiqueta            : " << virtualDrive.etiqueta;
	qDebug() << "vd.icono               : " << virtualDrive.icono;
	qDebug() << "vd.path_exe            : " << virtualDrive.path_exe;
	qDebug() << "vd.path_image          : " << virtualDrive.path_image;
	qDebug() << "vd.letter              : " << virtualDrive.letter;
	qDebug() << "vd.param_mount         : " << virtualDrive.param_mount;
	qDebug() << "vd.param_unmount       : " << virtualDrive.param_unmount;
	qDebug() << "vd.param_extra_1       : " << virtualDrive.param_extra_1;
	qDebug() << "vd.param_extra_2       : " << virtualDrive.param_extra_2;
	qDebug() << "vd.command_line_mount  : " << virtualDrive.command_line_mount;
	qDebug() << "vd.command_line_unmount: " << virtualDrive.command_line_unmount;
	qDebug() << "----------------------------------------------------";*/
}

void GrLida::ejecutar(QString bin, QString parametros, QString working_dir)
{
	if (QFile::exists(bin))
	{
		ui->mnu_ejecutar_juego_big->setEnabled(false);
		ui->mnu_ejecutar_juego->setEnabled(false);
		ui->mnu_ejecutar_setup_big->setEnabled(false);
		ui->mnu_ejecutar_setup->setEnabled(false);

		if (isTrayIcon)
		{
			trayIcon->show();
			this->hide();
		}

		if (fGrl->comprobarDirectorio(working_dir, true))
		{
			QFileInfo fi(working_dir);
			if (fi.isWritable())
				grlProcess->setWorkingDirectory(working_dir);
		}

		QStringList stl_param;
		stl_param << parametros.split("|", QString::SkipEmptyParts);

		grlProcess->start(bin, stl_param);
	} else
		QMessageBox::information(this, tituloGrl(), tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void GrLida::montaje(QString bin, QString parametros, QString working_dir)
{
	if (QFile::exists(bin))
	{
		if (!working_dir.isEmpty())
		{
			if (fGrl->comprobarDirectorio(working_dir, true))
			{
				QFileInfo fi(working_dir);
				if (fi.isWritable())
					grlProcessMount->setWorkingDirectory(working_dir);
				else
					grlProcessMount->setWorkingDirectory(grlDir.Temp);
			}
		}

		QStringList stl_param;
		if (!parametros.isEmpty())
			stl_param << parametros.split("|", QString::SkipEmptyParts);

		grlProcessMount->start(bin, stl_param);
		grlProcessMount->waitForFinished(1200);
	} else
		QMessageBox::information(this, tituloGrl(), tr("No esta disponible el script para ejecutar el montaje de la imagen."));
}
// FIN de Funciones ---------------------------------------------------------------------------------------------

// INI de SLOTs -------------------------------------------------------------------------------------------------
void GrLida::on_txtInfo_anchorClicked(const QUrl &arg1)
{
	fGrl->abrirArchivo(arg1.toString());
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
	if (grlProcess->isOpen())
	{
		grlProcess->terminate();
		grlProcess->close();
	}

	if (grlProcessMount->isOpen())
	{
		grlProcessMount->terminate();
		grlProcessMount->close();
	}

	this->close();
}

// Menú editar
void GrLida::on_mnu_edit_nuevo_dosbox_triggered()
{
	frmWizardDosBox *wizardDosBoxNew = new frmWizardDosBox(sql, grlCfg, categoria[id_cat], this);
	wizardDosBoxNew->setWindowFlags(Qt::Window);

	if (wizardDosBoxNew->exec() == QDialog::Accepted)
	{
		nuevoEditarDatosDelJuego(wizardDosBoxNew->getDatosJuegos(), true, true);
	}
	grlCfg = wizardDosBoxNew->getGrlCfg();

	delete wizardDosBoxNew;
}

void GrLida::on_mnu_edit_nuevo_scummvm_triggered()
{
	frmWizardScummVM *wizardScummVMNew = new frmWizardScummVM(sql, grlCfg, categoria[id_cat], this);
	wizardScummVMNew->setWindowFlags(Qt::Window);

	if (wizardScummVMNew->exec() == QDialog::Accepted)
	{
		nuevoEditarDatosDelJuego(wizardScummVMNew->getDatosJuegos(), true, true);
	}
	grlCfg = wizardScummVMNew->getGrlCfg();

	delete wizardScummVMNew;
}

void GrLida::on_mnu_edit_nuevo_vdmsound_triggered()
{
	frmWizardVdmSound *wizardVdmSoundNew = new frmWizardVdmSound(sql, grlCfg, categoria[id_cat], this);
	wizardVdmSoundNew->setWindowFlags(Qt::Window);

	if (wizardVdmSoundNew->exec() == QDialog::Accepted)
	{
		nuevoEditarDatosDelJuego(wizardVdmSoundNew->getDatosJuegos(), true, true);
	}
	grlCfg = wizardVdmSoundNew->getGrlCfg();

	delete wizardVdmSoundNew;
}

void GrLida::on_mnu_edit_nuevo_triggered()
{
	frmAddEditJuego *AddJuego = new frmAddEditJuego(sql, grlCfg, categoria[id_cat], "", "datos", false, this);
	AddJuego->setWindowFlags(Qt::Window);

	if (AddJuego->exec() == QDialog::Accepted)
	{
		nuevoEditarDatosDelJuego(AddJuego->getDatosJuegos(), true, true);
	}
	grlCfg = AddJuego->getGrlCfg();

	delete AddJuego;
}

void GrLida::on_mnu_edit_editar_triggered()
{
	mediaSound->closeMedia();
	mediaVideo->closeMedia();

	if (!pathCapturas.isEmpty())
		fs_watcher.removePath(pathCapturas);
	if (!pathImagenes.isEmpty())
		fs_watcher.removePath(pathImagenes);
	if (!pathVideos.isEmpty())
		fs_watcher.removePath(pathVideos);
	if (!pathSonidos.isEmpty())
		fs_watcher.removePath(pathSonidos);

	frmAddEditJuego *EditJuego = new frmAddEditJuego(sql, grlCfg, categoria[id_cat], IdGame, TipoEmu, true, this);
	EditJuego->setWindowFlags(Qt::Window);

	if (EditJuego->exec() == QDialog::Accepted)
	{
		grlCfg = EditJuego->getGrlCfg();

		if (grlCfg.isChangedVirtualDrive)
			virtual_drive_list = fGrl->cargarListVirtualDrive(grlDir.Datos +"virtual_driver_list.txt");

		nuevoEditarDatosDelJuego(EditJuego->getDatosJuegos(), false, true);
		setConfigHeaderListaJuegos();
	} else
		grlCfg = EditJuego->getGrlCfg();

	delete EditJuego;
}

void GrLida::on_mnu_edit_eliminar_triggered()
{
	int pos = select_row.row();
	if (data_model->rowCount() > 0 && pos > -1)
	{
		if (IdGame.isEmpty())
			QMessageBox::information(this, tituloGrl(), tr("Por favor selecciona un juego de la lista para eliminarlo"));
		else {
			QString idgrl, titulo_guiones, tipo_emu, dir_game;
			QSqlQuery query_del(sql->getSqlDB());
			query_del.exec("SELECT idgrl, titulo_guiones, thumbs, cover_front, cover_back, cover_left, cover_right, cover_top, cover_bottom, tipo_emu FROM "+ categoria[id_cat].tabla +" WHERE idgrl="+ IdGame);

			if (query_del.first())
			{
				idgrl          = query_del.record().value("idgrl").toString();
				titulo_guiones = query_del.record().value("titulo_guiones").toString();
				tipo_emu       = query_del.record().value("tipo_emu").toString();
				dir_game       = grlDir.DatosDbGrl +"id-"+ idgrl +"_"+ titulo_guiones +"_"+ tipo_emu;

				QFont font;
				font.setBold(true);
				font.setPointSize(10);
				font.setWeight(75);

				QDialog *sqlDel = new QDialog(this);
				sqlDel->resize(440, 330);
				sqlDel->setMinimumSize(QSize(440, 330));
				sqlDel->setWindowIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
				sqlDel->setWindowTitle(tr("¿Eliminar juego...?"));

				QGridLayout *gLayout_del = new QGridLayout(sqlDel);
				gLayout_del->setHorizontalSpacing(9);
					QLabel *lb_del_pregunta = new QLabel(sqlDel);
					lb_del_pregunta->setFont(font);
					lb_del_pregunta->setAlignment(Qt::AlignCenter);
					lb_del_pregunta->setWordWrap(true);
					lb_del_pregunta->setText(tr("¿Deseas realmente eliminar este juego de la base de datos?"));
				gLayout_del->addWidget(lb_del_pregunta, 0, 0, 1, 2);
					QLabel *lb_del_info = new QLabel(sqlDel);
					lb_del_info->setWordWrap(true);
					lb_del_info->setText(tr("Si es de DOSBox o VDMSound también se borra el archivo de configuración.") +"\n"+
										tr("Ademas borrara lo marcado siempre y cuando este dentro del directorio del GR-lida."));
				gLayout_del->addWidget(lb_del_info, 1, 0, 1, 2);
					QVBoxLayout *vLayout_del = new QVBoxLayout();
						QLabel *lb_del_icon = new QLabel(sqlDel);
						lb_del_icon->setMinimumSize(QSize(129, 164));
						lb_del_icon->setMaximumSize(QSize(129, 164));
						lb_del_icon->setPixmap(qvariant_cast<QPixmap>(select_row.sibling(select_row.row(), col_IdGrl).data(ThumbsRole)));
						lb_del_icon->setScaledContents(true);
						lb_del_icon->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
					vLayout_del->addWidget(lb_del_icon);
						QSpacerItem *vSpacer_del_1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
					vLayout_del->addItem(vSpacer_del_1);
				gLayout_del->addLayout(vLayout_del, 2, 0, 8, 1);
					QCheckBox *chk_del_thumb = new QCheckBox(sqlDel);
					chk_del_thumb->setChecked(true);
					chk_del_thumb->setText(tr("Eliminar carátula thumbs."));
				gLayout_del->addWidget(chk_del_thumb, 2, 1, 1, 1);
					QCheckBox *chk_del_images_box = new QCheckBox(sqlDel);
					chk_del_images_box->setChecked(true);
					chk_del_images_box->setText(tr("Eliminar imágenes de la caja."));
				gLayout_del->addWidget(chk_del_images_box, 3, 1, 1, 1);
					QLabel *lb_del_info_box = new QLabel(sqlDel);
					lb_del_info_box->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
					lb_del_info_box->setWordWrap(true);
					lb_del_info_box->setText(tr("Carátula delantera, trasera, lateral izquierdo, derecho, arriba, abajo."));
				gLayout_del->addWidget(lb_del_info_box, 4, 1, 1, 1);
					QCheckBox *chk_del_capturas = new QCheckBox(sqlDel);
					chk_del_capturas->setChecked(true);
					chk_del_capturas->setText(tr("Eliminar capturas de pantalla."));
				gLayout_del->addWidget(chk_del_capturas, 5, 1, 1, 1);
					QCheckBox *chk_del_imagenes = new QCheckBox(sqlDel);
					chk_del_imagenes->setChecked(true);
					chk_del_imagenes->setText(tr("Eliminar imágenes."));
				gLayout_del->addWidget(chk_del_imagenes, 6, 1, 1, 1);
					QCheckBox *chk_del_sonidos = new QCheckBox(sqlDel);
					chk_del_sonidos->setChecked(true);
					chk_del_sonidos->setText(tr("Eliminar capturas de sonido."));
				gLayout_del->addWidget(chk_del_sonidos, 7, 1, 1, 1);
					QCheckBox *chk_del_videos = new QCheckBox(sqlDel);
					chk_del_videos->setChecked(true);
					chk_del_videos->setText(tr("Eliminar capturas de video."));
				gLayout_del->addWidget(chk_del_videos, 8, 1, 1, 1);
					QCheckBox *chk_del_archivos = new QCheckBox(sqlDel);
					chk_del_archivos->setChecked(true);
					chk_del_archivos->setText(tr("Eliminar archivos guardados."));
				gLayout_del->addWidget(chk_del_archivos, 9, 1, 1, 1);
					QSpacerItem *vSpacer_del_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
				gLayout_del->addItem(vSpacer_del_2, 10, 1, 1, 1);
					QHBoxLayout *hLayout_del = new QHBoxLayout();
					 hLayout_del->setSpacing(11);
						QSpacerItem *hSpacer_del_1 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
					hLayout_del->addItem(hSpacer_del_1);
						QPushButton *btn_del_yes = new QPushButton(sqlDel);
						btn_del_yes->setMinimumSize(QSize(75, 26));
						btn_del_yes->setText(tr("Si"));
						btn_del_yes->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
					hLayout_del->addWidget(btn_del_yes);
						QPushButton *btn_del_no = new QPushButton(sqlDel);
						btn_del_no->setMinimumSize(QSize(75, 26));
						btn_del_no->setText(tr("No"));
						btn_del_no->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
					hLayout_del->addWidget(btn_del_no);
						QSpacerItem *hSpacer_del_2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
					hLayout_del->addItem(hSpacer_del_2);
				gLayout_del->addLayout(hLayout_del, 11, 0, 1, 2);
				//gLayout_del->setColumnStretch(1, 1);

				connect(btn_del_yes, SIGNAL(clicked()), sqlDel, SLOT(accept()));
				connect(btn_del_no , SIGNAL(clicked()), sqlDel, SLOT(reject()));

				if (!pathCapturas.isEmpty())
					fs_watcher.removePath(pathCapturas);
				if (!pathImagenes.isEmpty())
					fs_watcher.removePath(pathImagenes);
				if (!pathVideos.isEmpty())
					fs_watcher.removePath(pathVideos);
				if (!pathSonidos.isEmpty())
					fs_watcher.removePath(pathSonidos);

				if (sqlDel->exec() == QDialog::Accepted)
				{
					int num_no_del = 0;
					QStringList del_mensaje;
					QString dir_capturas = dir_game +"/capturas/"; // fGrl->getDirRelative
					QString dir_imagenes = dir_game +"/imagenes/";
					QString dir_videos   = dir_game +"/videos/";
					QString dir_sonidos  = dir_game +"/sonidos/";
					QString dir_archivos = dir_game +"/archivos/";

					if (chk_del_thumb->isChecked())
					{
						del_mensaje << tr("Carátula");
						fGrl->eliminarArchivo(dir_game +"/"+ query_del.record().value("thumbs").toString());
					} else
						++num_no_del;

					if (chk_del_images_box->isChecked())
					{
						del_mensaje << tr("Imágenes de la caja.");
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_front").toString());
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_back").toString());
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_left").toString());
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_right").toString());
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_top").toString());
						fGrl->eliminarArchivo(dir_game +"/caja/"+ query_del.record().value("cover_bottom").toString());
					} else
						++num_no_del;

					if (chk_del_capturas->isChecked())
					{
						del_mensaje << tr("Capturas de pantalla");
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_capturas, grlCfg.FormatsImage));
					} else
						++num_no_del;

					if (chk_del_imagenes->isChecked())
					{
						del_mensaje << tr("Imágenes del juego");
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_imagenes, grlCfg.FormatsImage));
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_imagenes +"small/", grlCfg.FormatsImage));
					} else
						++num_no_del;

					if (chk_del_videos->isChecked())
					{
						del_mensaje << tr("Capturas de videos");
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_capturas, grlCfg.FormatsVideo));
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_videos, grlCfg.FormatsVideo));
					} else
						++num_no_del;

					if (chk_del_sonidos->isChecked())
					{
						del_mensaje << tr("Capturas de sonidos");
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_capturas, grlCfg.FormatsMusic));
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_sonidos, grlCfg.FormatsMusic));
					} else
						++num_no_del;

					if (chk_del_archivos->isChecked())
					{
						del_mensaje << tr("Archivos");
						QStringList filter_archivos;
						filter_archivos << "*.*";
						fGrl->eliminarArchivos(fGrl->getListFiles(dir_archivos, filter_archivos));
					} else
						++num_no_del;

				// DOSBox
					if (tipo_emu == "dosbox")
					{
						del_mensaje << tr("Configuración de DOSBox");
						QString IDdbx = "";
						query_del.exec("SELECT id FROM dbgrl_emu_dosbox WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);
						if (query_del.first())
						{
							do {
								IDdbx = query_del.record().value("id").toString();
								query_del.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdbx);
							} while (query_del.next());
						}
						query_del.exec("DELETE FROM dbgrl_emu_dosbox WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);

						fGrl->eliminarArchivo(dir_game +"/dosbox.conf");
						fGrl->eliminarArchivo(dir_game +"/dosbox-setup.conf");
					}
				// ScummVM
					if (tipo_emu == "scummvm")
					{
						del_mensaje << tr("Configuración de ScummVM");
						query_del.exec("DELETE FROM dbgrl_emu_scummvm WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);

						fGrl->eliminarArchivo(dir_game +"/scummvm.ini");
					}
				// VDMSound
					if (tipo_emu == "vdmsound")
					{
						del_mensaje << tr("Configuración de VDMSound");
						QString IDvdms = "";
						query_del.exec("SELECT id FROM dbgrl_emu_vdmsound WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);
						if (query_del.first())
						{
							IDvdms = query_del.record().value("id").toString();
							query_del.exec("DELETE FROM dbgrl_emu_vdmsound WHERE id="+ IDvdms);
						}
						fGrl->eliminarArchivo(dir_game +"/vdmsound.vlp");
					}

				// Datos url y archivos
					fGrl->eliminarArchivo(dir_game +"/ficha.html");

					query_del.exec("DELETE FROM dbgrl_urls WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);
					query_del.exec("DELETE FROM dbgrl_files WHERE idgrl="+ idgrl +" AND idcat="+ categoria[id_cat].id);
				// Datos
					query_del.exec("DELETE FROM "+ categoria[id_cat].tabla +" WHERE idgrl="+ idgrl);

				// Eliminamos la carpeta
					if (num_no_del == 0)
					{
						fGrl->eliminarDirectorio(dir_capturas);
						fGrl->eliminarDirectorio(dir_imagenes +"small/");
						fGrl->eliminarDirectorio(dir_imagenes);
						fGrl->eliminarDirectorio(dir_videos);
						fGrl->eliminarDirectorio(dir_sonidos);
						fGrl->eliminarDirectorio(dir_game +"/archivos/");
						fGrl->eliminarDirectorio(dir_game +"/caja/");
						fGrl->eliminarDirectorio(dir_game);
					} else
						fGrl->renombrarDirectorio(dir_game, dir_game +"_delete");

					--total_juegos;
					--categoria[id_cat].total;

					data_model->removeRow(pos);
					if (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter)
						grl_picflow->removeSlide(pos);

					QString titulo_cat = QString("%1 (%2)").arg(categoria[id_cat].titulo).arg(categoria[id_cat].total);
					ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->currentIndex(), titulo_cat);
					ui->cbxCategorias->setItemText(ui->cbxCategorias->currentIndex(), titulo_cat);

					sql->comit(); // Guardamos los cambios en la base de datos.
					itemBack();
					crearMenuNav();

					QMessageBox::information(this, tituloGrl(), tr("El juego ha sido eliminado correctamente") +"\n"+ tr("Ha borrado") +": "+ del_mensaje.join(", "));
				}
				delete sqlDel;
			} else {
				QMessageBox::information(this, tituloGrl(), tr("Por favor selecciona un juego de la lista para eliminarlo"));
			}
		}
	}
}

void GrLida::on_mnu_edit_favorito_triggered(bool checked)
{
	if (!IdGame.isEmpty())
	{
		QString favorito = fGrl->boolToStr(checked);
		data_model->setData(data_model->index(select_row.row(), col_IdGrl), favorito, FavoritoRole);
		sql->actualizaDatosItem(categoria[id_cat].tabla, IdGame, "favorito", favorito);
		sql->comit();

		QFont m_font;
		int total_fav = sql->getCount(categoria[id_cat].tabla, "WHERE favorito='true'");
		if (total_fav > 0)
			m_font.setBold(true);
		else
			m_font.setBold(false);

		ui->twListNav->topLevelItem(1)->setFont(1, m_font);
		ui->twListNav->topLevelItem(1)->setText(1, fGrl->intToStr(total_fav));

		ui->tvJuegos->viewport()->repaint();
	}

	mostrarDatosDelJuego(IdGame, true);
}

void GrLida::on_mnu_edit_original_triggered(bool checked)
{
	if (!IdGame.isEmpty())
	{
		QString original = fGrl->boolToStr(checked);
		data_model->setData(data_model->index(select_row.row(), col_IdGrl), original, OriginalRole);
		sql->actualizaDatosItem(categoria[id_cat].tabla, IdGame, "original", original);
		sql->comit();

		QFont m_font;
		int total_org = sql->getCount(categoria[id_cat].tabla, "WHERE original='true'");
		if (total_org > 0)
			m_font.setBold(true);
		else
			m_font.setBold(false);

		ui->twListNav->topLevelItem(2)->setFont(1, m_font);
		ui->twListNav->topLevelItem(2)->setText(1, fGrl->intToStr(total_org));

		ui->tvJuegos->viewport()->repaint();
	}

	mostrarDatosDelJuego(IdGame, true);
}


void GrLida::updateCol(const QModelIndex &index, int col)
{
	if (index.isValid())
	{
		select_row = data_model->index(index.row(), col_IdGrl);
		IdGame = index.sibling(index.row(), col_IdGrl).data(IdGrlRole).toString();

		switch (col)
		{
			case col_Original:
			{
				bool isOriginal = fGrl->strToBool(index.sibling(index.row(), col_IdGrl).data(OriginalRole).toString());
				ui->mnu_edit_original->setChecked(isOriginal);
				emit on_mnu_edit_original_triggered(isOriginal);
			}
			break;
			case col_Favorito:
			{
				bool isFavorito = fGrl->strToBool(index.sibling(index.row(), col_IdGrl).data(FavoritoRole).toString());
				ui->mnu_edit_favorito->setChecked(isFavorito);
				emit on_mnu_edit_favorito_triggered(isFavorito);
			}
			break;
			case col_Rating:
			{
				QString new_rating = index.sibling(index.row(), col_IdGrl).data(RatingRole).toString();
				sql->actualizaDatosItem(categoria[id_cat].tabla, IdGame, "rating", new_rating);
				sql->comit();
				mostrarDatosDelJuego(IdGame, true);
			}
			break;
		};
	} else
		IdGame = "";
}

// Menú ejecutar
void GrLida::on_mnu_ejecutar_dosbox_triggered()
{
	if (dbx_list.size() > 0)
	{
		if (dbx_list.size() > 1)
		{
		// Opción para selecionar que DOSBox ejecutar
			QDialog *dbxDialog = new QDialog(this);
			dbxDialog->resize(240, 88);
			dbxDialog->setMinimumSize(QSize(240, 88));
			//dbxDialog->setMaximumSize(QSize(240, 88));
			dbxDialog->setWindowTitle("DOSBox");
			QGridLayout *gLayout = new QGridLayout(dbxDialog);
			gLayout->setContentsMargins(6, 6, 6, 4);
				QComboBox *cbx_dbx = new QComboBox(dbxDialog);
				cbx_dbx->setMinimumSize(QSize(0, 40));
				cbx_dbx->setIconSize(QSize(32, 32));
				cbx_dbx->clear();
				foreach (const stGrlDatos &dat, dbx_list)
					cbx_dbx->addItem(QIcon(fGrl->theme() +"img32/"+ dat.icono), dat.titulo, dat.key);
			gLayout->addWidget(cbx_dbx, 0, 0, 1, 3);
				QPushButton *btn_dbx_exec = new QPushButton(dbxDialog);
				btn_dbx_exec->setMinimumSize(QSize(77, 26));
				btn_dbx_exec->setText(tr("Ejecutar"));
				connect(btn_dbx_exec, SIGNAL(clicked()), dbxDialog, SLOT(accept()));
			gLayout->addWidget(btn_dbx_exec, 1, 1, 1, 1);
				QSpacerItem *hSpacerDbx1 = new QSpacerItem(65, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			gLayout->addItem(hSpacerDbx1, 1, 0, 1, 1);
				QSpacerItem *hSpacerDbx2 = new QSpacerItem(64, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			gLayout->addItem(hSpacerDbx2, 1, 2, 1, 1);

			if (dbxDialog->exec() == QDialog::Accepted)
			{
				ejecutar(fGrl->getDirRelative(dbx_list[cbx_dbx->itemData(cbx_dbx->currentIndex()).toString()].extra));
			}
			delete dbxDialog;
		} else {
			ejecutar(fGrl->getDirRelative(dbx_list[dbx_list.keys().at(0)].extra));
		}
	}
}

void GrLida::on_mnu_ejecutar_scummvm_triggered()
{
	ejecutar(fGrl->getDirRelative(grlCfg.DirScummVM));
}

void GrLida::on_mnu_ejecutar_juego_triggered()
{
	stDatosJuego DatosJuego = sql->show_Datos(categoria[id_cat].tabla, IdGame);

	if (!grlCfg.DOSBoxSaveConfFile && TipoEmu == "dosbox")
	{
		stConfigDOSBox DatosDosBox = sql->showConfg_DOSBox(IdGame, categoria[id_cat].id);
		QList<stConfigDOSBoxMount> listMount = sql->showConfg_DOSBoxMount(DatosDosBox.id);
		fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, listMount, grlDir.Home, categoria[id_cat].tabla, grlDir.Temp +"dosbox.conf");
		fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, listMount, grlDir.Home, categoria[id_cat].tabla, grlDir.Temp +"dosbox-setup.conf", true);
	}
	else if (!grlCfg.ScummVMSaveConfFile && TipoEmu == "scummvm")
	{
		stConfigScummVM DatosScummVM = sql->showConfg_ScummVM(IdGame, categoria[id_cat].id);
		DatosScummVM.emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;
		fGrl->creaIniScummVM(DatosScummVM, grlDir.Temp +"scummvm.ini");
	}

	if (TipoEmu == "datos" || TipoEmu == "dosbox" || TipoEmu == "vdmsound")
	{
		if (isAutoMountImageExe)
			emit on_mnu_ejecutar_montaje_triggered();
	}

	ejecutar(cfgExec.f_exe, cfgExec.f_exe_param, cfgExec.f_exe_path);
}

void GrLida::on_mnu_ejecutar_setup_triggered()
{
	if (cfgExec.isCfgSetup)
		ejecutar(cfgExec.f_setup, cfgExec.f_setup_param, cfgExec.f_setup_path);
}

void GrLida::on_mnu_ejecutar_montaje_triggered()
{
	if (isMountImage)
		ejecutar_desmontaje();
	else {
		conf_vd.clear();
		conf_vd = sql->getDatos(categoria[id_cat].tabla, "WHERE idgrl="+ IdGame, "path_image, virtual_drive");

		if (!conf_vd["path_image"].isEmpty() && conf_vd["virtual_drive"] != "NO_VIRTUAL_DRIVE")
		{
			virtualDrive            = virtual_drive_list[conf_vd["virtual_drive"]];
			virtualDrive.path_image = conf_vd["path_image"];

			if (virtualDrive.etiqueta == conf_vd["virtual_drive"])
			{
				QString mount_image = fGrl->getCommandLineMount(virtualDrive, true);

				fGrl->guardarArchivo(grlDir.Temp +"montaje.bat", "@echo off\n"+ mount_image);
				montaje(grlDir.Temp +"montaje.bat", mount_image, grlDir.Temp);

				isMountImage = true;
				ui->mnu_ejecutar_montaje->setText(tr("Desmontar imagen"));
			}
		}
	}

/*	qDebug() << "isMountImage           : " << isMountImage;
	qDebug() << "vd.titulo              : " << virtualDrive.titulo;
	qDebug() << "vd.etiqueta            : " << virtualDrive.etiqueta;
	qDebug() << "vd.icono               : " << virtualDrive.icono;
	qDebug() << "vd.path_exe            : " << virtualDrive.path_exe;
	qDebug() << "vd.path_image          : " << virtualDrive.path_image;
	qDebug() << "vd.letter              : " << virtualDrive.letter;
	qDebug() << "vd.param_mount         : " << virtualDrive.param_mount;
	qDebug() << "vd.param_unmount       : " << virtualDrive.param_unmount;
	qDebug() << "vd.param_extra_1       : " << virtualDrive.param_extra_1;
	qDebug() << "vd.param_extra_2       : " << virtualDrive.param_extra_2;
	qDebug() << "vd.command_line_mount  : " << virtualDrive.command_line_mount;
	qDebug() << "vd.command_line_unmount: " << virtualDrive.command_line_unmount;
	qDebug() << "----------------------------------------------------";*/
}

// Menú herramientas
void GrLida::on_mnu_tool_instalar_triggered()
{
	frmInstalarJuego *instalarJuego = new frmInstalarJuego(sql, grlCfg, this);
	instalarJuego->exec();
	delete instalarJuego;
}

void GrLida::on_mnu_tool_virtual_drive_triggered()
{
	frmAddEditVirtualDrive *addeditVirtualDrive = new frmAddEditVirtualDrive(sql, grlCfg, this);
	addeditVirtualDrive->setWindowFlags(Qt::Window);

	if (addeditVirtualDrive->exec() == QDialog::Accepted)
		grlCfg = addeditVirtualDrive->getGrlCfg();
	else
		grlCfg = addeditVirtualDrive->getGrlCfg();

	if (grlCfg.isChangedVirtualDrive)
		virtual_drive_list = fGrl->cargarListVirtualDrive(grlDir.Datos +"virtual_driver_list.txt");

	delete addeditVirtualDrive;
}

void GrLida::on_mnu_tool_importar_triggered()
{
	stDatosJuego     DatosJuego    = sql->getDefectDatos();
	stConfigDOSBox   DatosDosBox   = sql->getDefectDOSBox();
	stConfigScummVM  DatosScummVM  = sql->getDefectScummVM();
	stConfigVDMSound DatosVDMSound = sql->getDefectVDMSound();

	frmImportarJuego *importarJuego = new frmImportarJuego(sql, grlCfg, categoria[id_cat], DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, true, false, this);
	importarJuego->setWindowFlags(Qt::Window);
	if (importarJuego->exec() == QDialog::Accepted)
	{
		int row = data_model->rowCount();
		isImportar = true;
		QList<stImport> datos = importarJuego->getDatosImport();
		for (int i = 0; i < datos.size(); ++i)
			nuevoEditarDatosDelJuego(datos.at(i).datos, true, true);
		isImportar = false;
		setCurrentItem(row);
	}
	grlCfg = importarJuego->getGrlCfg();

	delete importarJuego;
}

void GrLida::on_mnu_tool_exportar_triggered()
{
//	frmExportarJuego *NewExportarJuego = new frmExportarJuego(sql, grlCfg, id_cat, this);
//	NewExportarJuego->exec();
//	grlCfg = NewExportarJuego->getGrlCfg();
//	delete NewExportarJuego;
}

void GrLida::on_mnu_tool_cambiar_categoria_triggered()
{
	int pos = select_row.row();
	if (data_model->rowCount() > 0 && pos > -1)
	{
		mediaSound->closeMedia();
		mediaVideo->closeMedia();

		fs_watcher.removePath(pathCapturas);
		fs_watcher.removePath(pathImagenes);
		fs_watcher.removePath(pathVideos);
		fs_watcher.removePath(pathSonidos);

		frmCambiarCategoria *cambiarCategoria = new frmCambiarCategoria(sql, grlCfg, categoria[id_cat], IdGame, this);
		if (cambiarCategoria->exec() == QDialog::Accepted)
		{
			int id_cat_new = cambiarCategoria->getNuevaCategoria();

			--total_juegos;
			--categoria[id_cat].total;
			++categoria[id_cat_new].total;

			data_model->removeRow(pos);
			if (grlCfg.Pnl_PictureFlow || grlCfg.PictureFlowToCenter)
				grl_picflow->removeSlide(pos);

			QString titulo_cat = QString("%1 (%2)").arg(categoria[id_cat].titulo).arg(categoria[id_cat].total);
			ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->currentIndex(), titulo_cat);
			ui->cbxCategorias->setItemText(ui->cbxCategorias->currentIndex(), titulo_cat);

			titulo_cat = QString("%1 (%2)").arg(categoria[id_cat_new].titulo).arg(categoria[id_cat_new].total);
			ui->cbxCategoriasTb->setItemText(ui->cbxCategoriasTb->findData(categoria[id_cat_new].id), titulo_cat);
			ui->cbxCategorias->setItemText(ui->cbxCategorias->findData(categoria[id_cat_new].id), titulo_cat);

			if (total_juegos <= 0)
				setCurrentItem(-1);
			else
				itemBack();
			crearMenuNav();
		}
		delete cambiarCategoria;
	}
}

void GrLida::on_mnu_tool_edit_themes_triggered()
{
	frmListIconCfg *ListIconCfg = new frmListIconCfg(sql, grlCfg, id_cat, this);
	ListIconCfg->setWindowFlags(Qt::Window);
	if (ListIconCfg->exec() == QDialog::Accepted)
	{
		lwIconCfg = fGrl->cargarListWidgetIconConf(categoria[id_cat].tabla);

		grl_tv_delegate->setTheme(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height), grlCfg.IconoFav, fGrl->theme(), grlCfg.FormatsImage.join(";"));
		grl_lv_delegate->setTheme(lwIconCfg, fGrl->theme());
		grl_picflow->setSlideSize(QSize(lwIconCfg.pf_img_width, lwIconCfg.pf_img_height));
		grl_picflow->setSlideFixSize(lwIconCfg.pf_img_fixsize);
		ui->tvJuegos->setAlternatingRowColors(lwIconCfg.tw_alternating_row_colors);
		ui->tvJuegos->setIconSize(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height));
		ui->tvJuegos->setColumnWidth(col_Icono, lwIconCfg.tw_icon_width + 8);
		ui->lvJuegos->setIconSize(QSize(lwIconCfg.icon_width, lwIconCfg.icon_height));

		ui->tvJuegos->update();
		ui->lvJuegos->update();

		setTheme();
		mostrarDatosDelJuego(IdGame, true);
	}
	delete ListIconCfg;
}

void GrLida::on_mnu_tool_opciones_triggered()
{
	frmOpciones *Opciones = new frmOpciones(sql, grlCfg, this);
	Opciones->setWindowFlags(Qt::Window);
	Opciones->exec();
	grlCfg = Opciones->getConfig();

	isAutoMountImageExe = grlCfg.AutoMountImageExe;

	if (grlCfg.isChangedToolbarBigIcon)
		setToolbarBigIcon(grlCfg.ToolbarBigIcon);

	if (grlCfg.isChangedCategoria || grlCfg.isChangedEmuList || grlCfg.isChangedListDOSBox || grlCfg.isChangedFavorito)
	{
		dbx_list.clear();
		dbx_list = fGrl->cargaDatosQHash(grlDir.Datos +"dbx_list.txt", 6, "|");

		cargarTextResource();
	}

	if (grlCfg.isChangedTheme || grlCfg.isChangedFavorito)
	{
		fGrl->setTheme(grlCfg.NameDirTheme);
		setTheme();
		lwIconCfg = fGrl->cargarListWidgetIconConf(categoria[id_cat].tabla);
		grl_tv_delegate->setTheme(QSize(lwIconCfg.tw_icon_width, lwIconCfg.tw_icon_height), grlCfg.IconoFav, fGrl->theme(), grlCfg.FormatsImage.join(";"));
		grl_lv_delegate->setTheme(lwIconCfg, fGrl->theme());
	}

	if (grlCfg.isChangedFormatoFecha)
		data_model->setFormatoFecha(grlCfg.FormatoFecha);

	if (grlCfg.isChangedIdioma)
	{
		QTranslator translator;
		translator.load(Opciones->getIdioma());
		qApp->installTranslator(&translator);
		ui->retranslateUi(this);
		setWindowTitle(tituloGrl() +" - v"+ fGrl->versionGrl());

		fGrl->setIdioma(grlCfg.IdiomaSelect);
		sql->setIdioma(grlCfg.IdiomaSelect);
	}

	if (grlCfg.isChangedCategoria || grlCfg.isChangedEmuList || grlCfg.isChangedIdioma)
		cargarListaCategorias();

	img_pf_favorite      = QImage(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgFavorito);
	img_pf_favorite_gray = fGrl->imagenToDisabled(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgFavorito);

	img_pf_original      = QImage(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgOriginal);
	img_pf_original_gray = fGrl->imagenToDisabled(fGrl->theme() +"img32/"+ grlCfg.PicFlowIconExtraImgOriginal);

	setSlideIconExtra();

	grl_picflow->setPicFlowType(grlCfg.PicFlowType);
	grl_picflow->setBackgroundColor(fGrl->getColor(grlCfg.PicFlowBgColor));
	grl_picflow->setPicFlowFont(grlCfg.PicFlowFontFamily, grlCfg.PicFlowFontSize, grlCfg.PicFlowFontBold, grlCfg.PicFlowFontItalic);
	grl_picflow->setFontColor(fGrl->getColor(grlCfg.PicFlowFontColor));
	grl_picflow->setMinimumHeight(grlCfg.PicFlowMinHeight);

	grl_picflow->setShowTitle(grlCfg.PicFlowShowTitle);
	grl_picflow->setSlideTitlePos(fGrl->getPosition(grlCfg.PicFlowTitlePos));
	grl_picflow->setSlideTitleMargin(grlCfg.PicFlowTitleMargin);

	grl_picflow->setShowTitleIcon(grlCfg.PicFlowShowTitleIcon);
	grl_picflow->setSlideTitleIconPos(grlCfg.PicFlowTitleIconPos);
	grl_picflow->setSlideTitleIconMargin(grlCfg.PicFlowTitleIconMargin);

	grl_picflow->setShowCaption(grlCfg.PicFlowShowCaption);
	grl_picflow->setSlideCaptionPos(fGrl->getPosition(grlCfg.PicFlowCaptionPos));
	grl_picflow->setSlideCaptionMargin(grlCfg.PicFlowCaptionMargin);

	grl_picflow->setShowNumber(grlCfg.PicFlowShowNumber);
	grl_picflow->setSlideNumberPos(fGrl->getPosition(grlCfg.PicFlowNumberPos));
	grl_picflow->setSlideNumberMargin(grlCfg.PicFlowNumberMargin);
	grl_picflow->setSlideNumberFormat(grlCfg.PicFlowNumberFormat);

	grl_picflow->setShowIconExtra(grlCfg.PicFlowShowIconExtra);
	grl_picflow->setSlideIconExtraPos(grlCfg.PicFlowIconExtraPos);
	grl_picflow->setSlideIconExtraMargin(grlCfg.PicFlowIconExtraMargin);

	grl_picflow->setSlideMargin(grlCfg.PicFlowMargin);

	grl_picflow->setShowTriangle(grlCfg.PicFlowShowTriangle);

	cargarListaShortCut();
	guardarConfig();
	comprobarEmuloresDisp();

	if (!grlCfg.isChangedCategoria && !grlCfg.isChangedEmuList && (grlCfg.isChangedListDOSBox || grlCfg.isChangedFavorito))
		mostrarDatosDelJuego(IdGame);

	delete Opciones;
}

// Menú ver
void GrLida::on_mnu_ver_nav_triggered(bool checked)
{
	grlCfg.Pnl_Nav = checked;
	ui->dockw_Nav->setVisible(checked);
}

void GrLida::on_mnu_ver_cover_mode_triggered(bool checked)
{
	grlCfg.ver_IconMode = checked;
	ui->tvJuegos->setVisible(!checked);
	ui->lvJuegos->setVisible(checked);

	setCurrentItem(select_row.row());

	if (grlCfg.ver_IconMode)
		ui->lvJuegos->setFocus();
	else
		ui->tvJuegos->setFocus();
}

void GrLida::on_mnu_ver_pictureflow_triggered(bool checked)
{
	grlCfg.Pnl_PictureFlow = checked;
	ui->dockw_PictureFlow->setVisible(checked);

	if (!grlCfg.PictureFlowToCenter)
	{
		if (checked)
			grl_picflow->setImagePaths(thumbs_list, select_row.row());
		else
			grl_picflow->clearImageSlide();
	}
}

void GrLida::on_mnu_ver_pictureflow_to_center_triggered(bool checked)
{
	grlCfg.PictureFlowToCenter = checked;

	if (checked)
	{
		ui->hLayout_juegos->addWidget(grl_picflow);
		ui->vLayout_picflow->addWidget(ui->tvJuegos);
		ui->vLayout_picflow->addWidget(ui->lvJuegos);
		grl_picflow->setFocus();

		ui->mnu_ver_pictureflow->setIcon(QIcon(fGrl->theme() +"img16/cover_mode.png"));
		ui->mnu_ver_pictureflow->setIconText(tr("Ver lista de juegos"));
		ui->mnu_ver_pictureflow->setText(tr("Ver lista de juegos"));
		ui->mnu_ver_pictureflow->setToolTip(tr("Ver lista de juegos"));
		ui->dockw_PictureFlow->setWindowTitle(tr("Lista de juegos"));

		ui->mnu_ver_pictureflow_to_center->setIcon(QIcon(fGrl->theme() +"img16/pictureflow.png"));
		ui->mnu_ver_pictureflow_to_center->setIconText(tr("Lista de juegos como principal"));
		ui->mnu_ver_pictureflow_to_center->setText(tr("Lista de juegos como principal"));
		ui->mnu_ver_pictureflow_to_center->setToolTip(tr("Lista de juegos como principal"));

		if (!grlCfg.Pnl_PictureFlow)
			grl_picflow->setImagePaths(thumbs_list, select_row.row());
	} else {
		ui->vLayout_picflow->addWidget(grl_picflow);
		ui->hLayout_juegos->addWidget(ui->lvJuegos);
		ui->hLayout_juegos->addWidget(ui->tvJuegos);
		if (ui->lvJuegos->isVisible())
			ui->lvJuegos->setFocus();
		else
			ui->tvJuegos->setFocus();

		ui->mnu_ver_pictureflow->setIcon(QIcon(fGrl->theme() +"img16/pictureflow.png"));
		ui->mnu_ver_pictureflow->setIconText("Ver PictureFlow");
		ui->mnu_ver_pictureflow->setText("Ver PictureFlow");
		ui->mnu_ver_pictureflow->setToolTip("Ver PictureFlow");
		ui->dockw_PictureFlow->setWindowTitle("PictureFlow");

		ui->mnu_ver_pictureflow_to_center->setIcon(QIcon(fGrl->theme() +"img16/cover_mode.png"));
		ui->mnu_ver_pictureflow_to_center->setIconText(tr("PictureFlow como principal"));
		ui->mnu_ver_pictureflow_to_center->setText(tr("PictureFlow como principal"));
		ui->mnu_ver_pictureflow_to_center->setToolTip(tr("PictureFlow como principal"));
	}
}

void GrLida::on_mnu_ver_datos_triggered(bool checked)
{
	grlCfg.Pnl_Datos = checked;
	ui->mnu_ver_datos_big->setChecked(checked);
	ui->dockw_Datos->setVisible(checked);

	if (checked)
		mostrarDatosDelJuego(IdGame, true);
}

void GrLida::on_mnu_ver_archivos_triggered(bool checked)
{
	grlCfg.Pnl_Files = checked;
	ui->mnu_ver_archivos_big->setChecked(checked);
	ui->btnVer_Archivos->setChecked(checked);
	ui->dockw_Files->setVisible(checked);

	if (checked)
		sql->cargarDatosFiles(ui->twFiles, IdGame, categoria[id_cat].id);
}

void GrLida::on_mnu_ver_enlaces_triggered(bool checked)
{
	grlCfg.Pnl_Urls = checked;
	ui->dockw_Urls->setVisible(checked);

	if (checked)
		sql->cargarDatosUrls(ui->twUrls, IdGame, categoria[id_cat].id);
}

void GrLida::on_mnu_ver_videos_triggered(bool checked)
{
	grlCfg.Pnl_Videos = checked;
	ui->dockw_Videos->setVisible(checked);

	if (checked)
	{
		fGrl->cargarArchivosLwLista(ui->lwVideos, pathCapturas, grlCfg.FormatsVideo.join(";"), ";", "video.png");
		fGrl->cargarArchivosLwLista(ui->lwVideos, pathVideos  , grlCfg.FormatsVideo.join(";"), ";", "video.png", false);
	}
}

void GrLida::on_mnu_ver_sonidos_triggered(bool checked)
{
	grlCfg.Pnl_Sonidos = checked;
	ui->dockw_Sonidos->setVisible(checked);

	if (checked)
	{
		mediaSound->setDirGame(grlDir.DatosGame);
		mediaSound->setTheme(fGrl->theme());
		mediaSound->setDirPlaylist(pathCapturas, grlCfg.FormatsMusic, true);
		mediaSound->setDirPlaylist(pathSonidos , grlCfg.FormatsMusic);
	}
}

void GrLida::on_mnu_ver_capturas_triggered(bool checked)
{
	grlCfg.Pnl_Capturas = checked;
	ui->mnu_ver_capturas_big->setChecked(checked);
	ui->dockw_Capturas->setVisible(checked);

	if (checked)
		fGrl->cargarArchivosLwLista(ui->lwCapturas, pathCapturas, grlCfg.FormatsImage.join(";"), ";", "capturas.png", true, true);
}

void GrLida::on_mnu_ver_imagenes_triggered(bool checked)
{
	grlCfg.Pnl_Imagenes = checked;
	ui->dockw_Imagenes->setVisible(checked);

	if (checked)
		fGrl->cargarArchivosLwLista(ui->lwImagenes, pathImagenes, grlCfg.FormatsImage.join(";"), ";", "imagen.png", true, true, true);
}

void GrLida::on_mnu_ver_menubar_triggered(bool checked)
{
	grlCfg.Pnl_Menubar = checked;
	ui->menubar->setVisible(checked);
}

void GrLida::on_mnu_ver_toolbar_triggered(bool checked)
{
	grlCfg.Pnl_Toolbar = checked;
	ui->tb_main->setVisible(checked);
}

void GrLida::on_mnu_ver_toolbar_datos_triggered(bool checked)
{
	grlCfg.Pnl_ToolbarDatos = checked;
	ui->wInfoDatos->setVisible(checked);
}

void GrLida::on_mnu_ver_ordenar_triggered(bool checked)
{
	grlCfg.Pnl_Ordenar = checked;
	ui->tb_ordenar->setVisible(checked);
}

void GrLida::on_mnu_ver_shortcut_triggered(bool checked)
{
	grlCfg.Pnl_ShortCut = checked;
	ui->tb_shortcut->setVisible(checked);
}

void GrLida::on_mnu_ver_statusbar_triggered(bool checked)
{
	grlCfg.Pnl_StatusBar = checked;
	ui->statusBar->setVisible(checked);
}

void GrLida::on_mnu_ver_fullscreen_triggered(bool checked)
{
	grlCfg.main_fullscreen = checked;
	if (checked)
		this->showFullScreen();
	else
		this->showNormal();
}

void GrLida::on_mnu_ver_ayuda_triggered()
{
	QDesktopServices::openUrl(QUrl("http://www.gr-lida.org/ayuda/"));
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

// Menús Ver Paneles
void GrLida::on_mnu_ver_ignore_auto_hide_nav_triggered(bool checked)
{
	grlCfg.Pnl_Nav_IgnoreAutoHide = checked;
	ui->dockw_Nav->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_datos_triggered(bool checked)
{
	grlCfg.Pnl_Datos_IgnoreAutoHide = checked;
	ui->dockw_Datos->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_files_triggered(bool checked)
{
	grlCfg.Pnl_Files_IgnoreAutoHide = checked;
	ui->dockw_Files->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_urls_triggered(bool checked)
{
	grlCfg.Pnl_Urls_IgnoreAutoHide = checked;
	ui->dockw_Urls->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_videos_triggered(bool checked)
{
	grlCfg.Pnl_Videos_IgnoreAutoHide = checked;
	ui->dockw_Videos->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_sonidos_triggered(bool checked)
{
	grlCfg.Pnl_Sonidos_IgnoreAutoHide = checked;
	ui->dockw_Sonidos->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_capturas_triggered(bool checked)
{
	grlCfg.Pnl_Capturas_IgnoreAutoHide = checked;
	ui->dockw_Capturas->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_imagenes_triggered(bool checked)
{
	grlCfg.Pnl_Imagenes_IgnoreAutoHide = checked;
	ui->dockw_Imagenes->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_ignore_auto_hide_pictureflow_triggered(bool checked)
{
	grlCfg.Pnl_PictureFlow_IgnoreAutoHide = checked;
	ui->dockw_PictureFlow->setIgnoreAutoHide(checked);
}

void GrLida::on_mnu_ver_titlebars_nav_triggered(bool checked)
{
	grlCfg.Pnl_Nav_VisibleTitleBars = checked;
	ui->dockw_Nav->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_datos_triggered(bool checked)
{
	grlCfg.Pnl_Datos_VisibleTitleBars = checked;
	ui->dockw_Datos->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_files_triggered(bool checked)
{
	grlCfg.Pnl_Files_VisibleTitleBars = checked;
	ui->dockw_Files->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_urls_triggered(bool checked)
{
	grlCfg.Pnl_Urls_VisibleTitleBars = checked;
	ui->dockw_Urls->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_videos_triggered(bool checked)
{
	grlCfg.Pnl_Videos_VisibleTitleBars = checked;
	ui->dockw_Videos->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_sonidos_triggered(bool checked)
{
	grlCfg.Pnl_Sonidos_VisibleTitleBars = checked;
	ui->dockw_Sonidos->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_capturas_triggered(bool checked)
{
	grlCfg.Pnl_Capturas_VisibleTitleBars = checked;
	ui->dockw_Capturas->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_imagenes_triggered(bool checked)
{
	grlCfg.Pnl_Imagenes_VisibleTitleBars = checked;
	ui->dockw_Imagenes->setVisibleTitleBars(checked);
}

void GrLida::on_mnu_ver_titlebars_pictureflow_triggered(bool checked)
{
	grlCfg.Pnl_PictureFlow_VisibleTitleBars = checked;
	ui->dockw_PictureFlow->setVisibleTitleBars(checked);
}

// Menú ver columnas
void GrLida::on_mnu_ver_col_icono_triggered(bool checked)
{
	grlCfg.ver_col_Icono = checked;
	ui->tvJuegos->setColumnHidden(col_Icono, !checked);
}

void GrLida::on_mnu_ver_col_subtitulo_triggered(bool checked)
{
	grlCfg.ver_col_Subtitulo = checked;
	ui->tvJuegos->setColumnHidden(col_Subtitulo, !checked);
}

void GrLida::on_mnu_ver_col_genero_triggered(bool checked)
{
	grlCfg.ver_col_Genero = checked;
	ui->tvJuegos->setColumnHidden(col_Genero, !checked);
}

void GrLida::on_mnu_ver_col_compania_triggered(bool checked)
{
	grlCfg.ver_col_Compania = checked;
	ui->tvJuegos->setColumnHidden(col_Compania, !checked);
}

void GrLida::on_mnu_ver_col_desarrollador_triggered(bool checked)
{
	grlCfg.ver_col_Desarrollador = checked;
	ui->tvJuegos->setColumnHidden(col_Desarrollador, !checked);
}

void GrLida::on_mnu_ver_col_tema_triggered(bool checked)
{
	grlCfg.ver_col_Tema = checked;
	ui->tvJuegos->setColumnHidden(col_Tema, !checked);
}

void GrLida::on_mnu_ver_col_grupo_triggered(bool checked)
{
	grlCfg.ver_col_Grupo = checked;
	ui->tvJuegos->setColumnHidden(col_Grupo, !checked);
}

void GrLida::on_mnu_ver_col_perspectiva_triggered(bool checked)
{
	grlCfg.ver_col_Perspectiva = checked;
	ui->tvJuegos->setColumnHidden(col_Perspectiva, !checked);
}

void GrLida::on_mnu_ver_col_idioma_triggered(bool checked)
{
	grlCfg.ver_col_Idioma = checked;
	ui->tvJuegos->setColumnHidden(col_Idioma, !checked);
}

void GrLida::on_mnu_ver_col_idioma_voces_triggered(bool checked)
{
	grlCfg.ver_col_IdiomaVoces = checked;
	ui->tvJuegos->setColumnHidden(col_IdiomaVoces, !checked);
}

void GrLida::on_mnu_ver_col_formato_triggered(bool checked)
{
	grlCfg.ver_col_Formato = checked;
	ui->tvJuegos->setColumnHidden(col_Formato, !checked);
}

void GrLida::on_mnu_ver_col_anno_triggered(bool checked)
{
	grlCfg.ver_col_Anno = checked;
	ui->tvJuegos->setColumnHidden(col_Anno, !checked);
}

void GrLida::on_mnu_ver_col_numdisc_triggered(bool checked)
{
	grlCfg.ver_col_NumDisc = checked;
	ui->tvJuegos->setColumnHidden(col_NumDisc, !checked);
}

void GrLida::on_mnu_ver_col_sistemaop_triggered(bool checked)
{
	grlCfg.ver_col_SistemaOp = checked;
	ui->tvJuegos->setColumnHidden(col_SistemaOp, !checked);
}

void GrLida::on_mnu_ver_col_tamano_triggered(bool checked)
{
	grlCfg.ver_col_Tamano = checked;
	ui->tvJuegos->setColumnHidden(col_Tamano, !checked);
}

void GrLida::on_mnu_ver_col_graficos_triggered(bool checked)
{
	grlCfg.ver_col_Graficos = checked;
	ui->tvJuegos->setColumnHidden(col_Graficos, !checked);
}

void GrLida::on_mnu_ver_col_sonido_triggered(bool checked)
{
	grlCfg.ver_col_Sonido = checked;
	ui->tvJuegos->setColumnHidden(col_Sonido, !checked);
}

void GrLida::on_mnu_ver_col_jugabilidad_triggered(bool checked)
{
	grlCfg.ver_col_Jugabilidad = checked;
	ui->tvJuegos->setColumnHidden(col_Jugabilidad, !checked);
}

void GrLida::on_mnu_ver_col_original_triggered(bool checked)
{
	grlCfg.ver_col_Original = checked;
	ui->tvJuegos->setColumnHidden(col_Original, !checked);
}

void GrLida::on_mnu_ver_col_estado_triggered(bool checked)
{
	grlCfg.ver_col_Estado = checked;
	ui->tvJuegos->setColumnHidden(col_Estado, !checked);
}

void GrLida::on_mnu_ver_col_fecha_triggered(bool checked)
{
	grlCfg.ver_col_Fecha = checked;
	ui->tvJuegos->setColumnHidden(col_Fecha, !checked);
}

void GrLida::on_mnu_ver_col_tipoemu_triggered(bool checked)
{
	grlCfg.ver_col_TipoEmu = checked;
	ui->tvJuegos->setColumnHidden(col_TipoEmu, !checked);
}

void GrLida::on_mnu_ver_col_favorito_triggered(bool checked)
{
	grlCfg.ver_col_Favorito = checked;
	ui->tvJuegos->setColumnHidden(col_Favorito, !checked);
}

void GrLida::on_mnu_ver_col_rating_triggered(bool checked)
{
	grlCfg.ver_col_Rating = checked;
	ui->tvJuegos->setColumnHidden(col_Rating, !checked);
}

void GrLida::on_mnu_ver_col_edad_triggered(bool checked)
{
	grlCfg.ver_col_Edad = checked;
	ui->tvJuegos->setColumnHidden(col_Edad, !checked);
}

void GrLida::on_mnu_ver_col_usuario_triggered(bool checked)
{
	grlCfg.ver_col_Usuario = checked;
	ui->tvJuegos->setColumnHidden(col_Usuario, !checked);
}

// Menú ver carpetas
void GrLida::on_mnu_ver_carpeta_datos_juego_triggered()
{
	fGrl->abrirDirectorio(grlDir.DatosGame);
}

void GrLida::on_mnu_ver_carpeta_iconos_triggered()
{
	fGrl->abrirDirectorio(grlDir.Iconos);
}

void GrLida::on_mnu_ver_carpeta_smiles_triggered()
{
	fGrl->abrirDirectorio(grlDir.Smiles);
}

void GrLida::on_mnu_ver_carpeta_themes_triggered()
{
	fGrl->abrirDirectorio(grlDir.Themes);
}

void GrLida::on_mnu_ver_carpeta_datos_triggered()
{
	fGrl->abrirDirectorio(grlDir.Datos);
}

void GrLida::on_mnu_ver_carpeta_scripts_triggered()
{
	fGrl->abrirDirectorio(grlDir.Scripts);
}

void GrLida::on_mnu_ver_carpeta_templates_triggered()
{
	fGrl->abrirDirectorio(grlDir.Templates);
}

void GrLida::on_mnu_ver_carpeta_idiomas_triggered()
{
	fGrl->abrirDirectorio(grlDir.Idiomas);
}

void GrLida::on_mnu_ver_carpeta_temp_triggered()
{
	fGrl->abrirDirectorio(grlDir.Temp);
}

void GrLida::on_mnu_ver_carpeta_home_triggered()
{
	fGrl->abrirDirectorio(grlDir.Home);
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

void GrLida::on_mnu_ver_datos_big_triggered(bool checked)
{
	emit on_mnu_ver_datos_triggered(checked);
}

void GrLida::on_mnu_ver_archivos_big_triggered(bool checked)
{
	emit on_mnu_ver_archivos_triggered(checked);
}

void GrLida::on_mnu_ver_capturas_big_triggered(bool checked)
{
	emit on_mnu_ver_capturas_triggered(checked);
}

void GrLida::on_mnu_tool_opciones_big_triggered()
{
	emit on_mnu_tool_opciones_triggered();
}

void GrLida::on_mnu_ver_acercad_big_triggered()
{
	emit on_mnu_ver_acercad_triggered();
}

// Barra de heramientas busqueda
void GrLida::on_txtBuscar_textChanged(const QString &arg1)
{
	if (arg1.length() == 0)
		cargarListaJuegosDB();
	else
		cargarListaJuegosDB("WHERE titulo LIKE '%"+ arg1 +"%'");
}

void GrLida::on_cbxOrdenBy_activated(int index)
{
	if (index > -1)
		grlCfg.db_orden_by = ui->cbxOrdenBy->itemData(index).toString();
}

void GrLida::on_cbxOrden_activated(int index)
{
	if (index > -1)
		grlCfg.db_orden = ui->cbxOrden->itemData(index).toString();
}

void GrLida::on_btnOrdenar_clicked()
{
	emit on_txtBuscar_textChanged(ui->txtBuscar->text());
}

// Menú desplegable de categorias
void GrLida::on_cbxCategoriasTb_activated(int index)
{
	if (index > -1)
	{
		int cat_id = ui->cbxCategoriasTb->itemData(index).toInt();
		ui->cbxCategorias->setCurrentIndex(index);
		setChangeCategorias(cat_id);
	}
}

void GrLida::on_cbxCategorias_activated(int index)
{
	if (index > -1)
	{
		int cat_id = ui->cbxCategorias->itemData(index).toInt();
		ui->cbxCategoriasTb->setCurrentIndex(index);
		setChangeCategorias(cat_id);
	}
}

// Lista navegación
void GrLida::listNavItemSelect(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		sql_where_select = item->text(2);
		cargarListaJuegosDB(sql_where_select);

		grl_picflow->setSlideTitle(item->text(0));
		grl_picflow->setSlideTitleIcon(fGrl->theme() +"img32/"+ item->text(3));

		grlCfg.LastSelectNavID = item->text(4).toInt();
		int itemIndex = ui->twListNav->indexOfTopLevelItem(item);
		if (itemIndex == -1)
			grlCfg.LastSelectNavSubID = ui->twListNav->currentIndex().row();
		else
			grlCfg.LastSelectNavSubID = -1;
	}
}

void GrLida::on_twListNav_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		if (!isNavItemChanged)
			listNavItemSelect(item, column);
		isNavItemChanged = false;
	}
}

void GrLida::on_twListNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == NULL)
		return;

	if (current != previous)
	{
		isNavItemChanged = true;
		listNavItemSelect(current, 0);
	}
}

// Lista de juegos QListView
void GrLida::lvJuegos_changed(const QModelIndex &current, const QModelIndex &previous)
{
	if (current.isValid())
	{
		isItemChanged = true;
		listItemSelect(current, "lvJuegos_changed");
	} else if (previous.isValid()) {

	}
}

void GrLida::on_lvJuegos_clicked(const QModelIndex &index)
{
	if (!isItemChanged)
		listItemSelect(index, "on_lvJuegos_clicked");
	isItemChanged = false;
}

void GrLida::on_lvJuegos_doubleClicked(const QModelIndex &index)
{
	if (index.isValid())
		listItemSelectDoubleClicked(index, "on_lvJuegos_doubleClicked");
}

void GrLida::on_lvJuegos_customContextMenuRequested(const QPoint &pos)
{
	showPopup(ui->lvJuegos, pos);
}

// Lista principal QTreeView
void GrLida::tvJuegos_changed(const QModelIndex &current, const QModelIndex &previous)
{
	if (current.isValid())
	{
		isItemChanged = true;
		listItemSelect(current, "tvJuegos_changed");
	} else if (previous.isValid()) {

	}
}

void GrLida::on_tvJuegos_clicked(const QModelIndex &index)
{
	if (!isItemChanged)
		listItemSelect(index, "on_tvJuegos_clicked");
	isItemChanged = false;
}

void GrLida::on_tvJuegos_doubleClicked(const QModelIndex &index)
{
	if (index.isValid())
		listItemSelectDoubleClicked(index, "on_tvJuegos_doubleClicked");
}

void GrLida::on_tvJuegos_customContextMenuRequested(const QPoint &pos)
{
	showPopup(ui->tvJuegos, pos);
}

// PictureFlow
void GrLida::grl_picflow_skip(int current)
{
	if (current > -1)
		select_row = data_model->index(current, col_IdGrl);
}

void GrLida::grl_picflow_finishedAnimation()
{
	if (isPicFlowActive)
		setCurrentItem(select_row.row());
}

void GrLida::grl_picflow_isActive(bool active)
{
	isPicFlowActive = active;
}

void GrLida::grl_picflow_selected(int current)
{
	IdGame = select_row.sibling(current, col_IdGrl).data(IdGrlRole).toString();
	emit on_mnu_ejecutar_juego_triggered();
}

void GrLida::grl_picflow_customContextMenuRequested(const QPoint &pos)
{
	showPopup(grl_picflow, pos);
}

// Opciones de Datos
void GrLida::on_btnVer_Archivos_clicked(bool checked)
{
	ui->mnu_ver_archivos->setChecked(checked);
	ui->mnu_ver_archivos_big->setChecked(checked);
	emit on_mnu_ver_archivos_triggered(checked);
}

void GrLida::on_btnVer_CoverFront_clicked()
{
	if (!CoverFront.isEmpty())
	{
		QStringList covers;
		covers << grlDir.DatosGame +"caja/"+ CoverFront;

		if (!CoverBack.isEmpty())
			covers << grlDir.DatosGame +"caja/"+ CoverBack;

		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagenes(covers, 0, true);
		grdap->show();
	}
}

void GrLida::on_btnVer_CoverBack_clicked()
{
	if (!CoverBack.isEmpty())
	{
		QStringList covers;
		covers << grlDir.DatosGame +"caja/"+ CoverBack;

		if (!CoverFront.isEmpty())
			covers << grlDir.DatosGame +"caja/"+ CoverFront;;

		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagenes(covers, 0, true);
		grdap->show();
	}
}

// Lista de files, urls, videos, sonidos y capturas
void GrLida::on_twFiles_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		stFileInfo archivo = fGrl->getInfoFile(fGrl->getDirRelative(item->text(4)));
		if (item->text(5) == "ruleta" || item->text(5) == "manual")
		{
			QStringList list_ext;
			list_ext << "*.dap" << "*.dapz" << "*.conf"
					<< "*.zip" << "*.rar" << "*.7z" << "*.tar"
					<< "*.cbz" << "*.cbr" << "*.cb7" << "*.cbt";
			if (list_ext.contains("*"+ archivo.Ext))
			{
				if (!isOpenGrDap)
				{
					isOpenGrDap = true;
					grdap = new GrDap(this);
				}

				grdap->cargarArchivo(archivo.FilePath);
				grdap->show();
			} else
				fGrl->abrirArchivo(archivo.FilePath);
		}
		else if (item->text(5) == "pdf")
		{
			if (grlCfg.OpenPdfExternal || archivo.Ext != ".pdf")
				fGrl->abrirArchivo(archivo.FilePath);
			else {
				if (!isOpenPdf)
				{
					isOpenPdf = true;
					pdfViewer = new frmPdfViewer(this);
					pdfViewer->setWindowFlags(Qt::Window);
				}

				pdfViewer->openPdf(archivo.FilePath);
				pdfViewer->show();
			}
		} else
			fGrl->abrirArchivo(archivo.FilePath);
	}
}

void GrLida::on_twUrls_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
		fGrl->abrirArchivo(item->text(0));
}

void GrLida::on_lwVideos_itemDoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		mediaVideo->setDirGame(grlDir.DatosGame);
		mediaVideo->setTheme(fGrl->theme());

		QStringList lista;
		const int listSize = ui->lwVideos->count();
		for (int i = 0; i < listSize; ++i)
			lista << ui->lwVideos->item(i)->data(Qt::UserRole).toString();

		mediaVideo->setPlaylist(lista, true, ui->lwVideos->currentRow(), true);
		mediaVideo->show();
	}
}

void GrLida::on_lwCapturas_itemDoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		QStringList lista;
		const int listSize = ui->lwCapturas->count();
		for (int i = 0; i < listSize; ++i)
			lista << ui->lwCapturas->item(i)->data(Qt::UserRole).toString();
		int id_imagen = lista.indexOf(item->data(Qt::UserRole).toString());

		grdap->cargarImagenes(lista, (id_imagen != -1 ? id_imagen : 0), true);
		grdap->show();
	}
}

void GrLida::on_lwImagenes_itemDoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		QStringList lista;
		const int listSize = ui->lwImagenes->count();
		for (int i = 0; i < listSize; ++i)
			lista << ui->lwImagenes->item(i)->data(Qt::UserRole).toString();
		int id_imagen = lista.indexOf(item->data(Qt::UserRole).toString());

		grdap->cargarImagenes(lista, (id_imagen != -1 ? id_imagen : 0), true);
		grdap->show();
	}
}

void GrLida::directoryChanged(QString dir)
{
	if (grlCfg.Pnl_Capturas && pathCapturas == dir)
	{
		fGrl->cargarArchivosLwLista(ui->lwCapturas, pathCapturas, grlCfg.FormatsImage.join(";"), ";", "capturas.png", true, true);
	}
	if (grlCfg.Pnl_Imagenes && pathImagenes == dir)
	{
		fGrl->cargarArchivosLwLista(ui->lwImagenes, pathImagenes, grlCfg.FormatsImage.join(";"), ";", "imagen.png", true, true, true);
	}
	if (grlCfg.Pnl_Videos && (pathVideos == dir || pathCapturas == dir))
	{
		fGrl->cargarArchivosLwLista(ui->lwVideos, pathCapturas, grlCfg.FormatsVideo.join(";"), ";", "video.png");
		fGrl->cargarArchivosLwLista(ui->lwVideos, pathVideos  , grlCfg.FormatsVideo.join(";"), ";", "video.png", false);
	}
	if (grlCfg.Pnl_Sonidos && (pathSonidos == dir || pathCapturas == dir))
	{
		mediaSound->setDirGame(grlDir.DatosGame);
		mediaSound->setTheme(fGrl->theme());
		mediaSound->setDirPlaylist(pathCapturas, grlCfg.FormatsMusic, true);
		mediaSound->setDirPlaylist(pathSonidos , grlCfg.FormatsMusic);
	}
}

// Paneles
void GrLida::on_mnu_ver_auto_hide_title_bars_triggered(bool checked)
{
	grlCfg.Pnl_AutoHideTitleBars = checked;

	ui->dockw_Nav->setAutoHideTitleBars(checked);
	ui->dockw_Datos->setAutoHideTitleBars(checked);
	ui->dockw_Files->setAutoHideTitleBars(checked);
	ui->dockw_Urls->setAutoHideTitleBars(checked);
	ui->dockw_Videos->setAutoHideTitleBars(checked);
	ui->dockw_Sonidos->setAutoHideTitleBars(checked);
	ui->dockw_Capturas->setAutoHideTitleBars(checked);
	ui->dockw_Imagenes->setAutoHideTitleBars(checked);
	ui->dockw_PictureFlow->setAutoHideTitleBars(checked);
}

void GrLida::dockw_Nav_toggled(bool visible)
{
	grlCfg.Pnl_Nav = visible;
	ui->mnu_ver_nav->setChecked(visible);
}

void GrLida::dockw_Datos_toggled(bool visible)
{
	grlCfg.Pnl_Datos = visible;
	ui->mnu_ver_datos->setChecked(visible);
	ui->mnu_ver_datos_big->setChecked(visible);
}

void GrLida::dockw_Files_toggled(bool visible)
{
	grlCfg.Pnl_Files = visible;
	ui->mnu_ver_archivos->setChecked(visible);
	ui->mnu_ver_archivos_big->setChecked(visible);
	ui->btnVer_Archivos->setChecked(visible);
}

void GrLida::dockw_Urls_toggled(bool visible)
{
	grlCfg.Pnl_Urls = visible;
	ui->mnu_ver_enlaces->setChecked(visible);
}

void GrLida::dockw_Videos_toggled(bool visible)
{
	grlCfg.Pnl_Videos = visible;
	ui->mnu_ver_videos->setChecked(visible);
}

void GrLida::dockw_Sonidos_toggled(bool visible)
{
	grlCfg.Pnl_Sonidos = visible;
	ui->mnu_ver_sonidos->setChecked(visible);
}

void GrLida::dockw_Capturas_toggled(bool visible)
{
	grlCfg.Pnl_Capturas = visible;
	ui->mnu_ver_capturas->setChecked(visible);
	ui->mnu_ver_capturas_big->setChecked(visible);
}

void GrLida::dockw_Imagenes_toggled(bool visible)
{
	grlCfg.Pnl_Imagenes = visible;
	ui->mnu_ver_imagenes->setChecked(visible);
}

void GrLida::dockw_PictureFlow_toggled(bool visible)
{
	grlCfg.Pnl_PictureFlow = visible;
	ui->mnu_ver_pictureflow->setChecked(visible);
}

// Procesos
void GrLida::fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus)
{
	switch (exitStatus)
	{
		case QProcess::NormalExit:
			//
		break;
		case QProcess::CrashExit:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error al salir del proceso") +"\nError: "+ fGrl->intToStr(exitCode), QMessageBox::Ok);
		break;
	}

	grlProcess->terminate();
	grlProcess->close();

	ui->mnu_ejecutar_juego_big->setEnabled(cfgExec.isCfgExec);
	ui->mnu_ejecutar_juego->setEnabled(cfgExec.isCfgExec);
	ui->mnu_ejecutar_setup_big->setEnabled(cfgExec.isCfgSetup);
	ui->mnu_ejecutar_setup->setEnabled(cfgExec.isCfgSetup);

	this->show();

	if (isTrayIcon)
	{
		trayIcon->hide();

		if (isAutoMountImageExe)
			ejecutar_desmontaje();
	}
}

void GrLida::fin_ProcesoMount(int exitCode, QProcess::ExitStatus exitStatus)
{
	switch (exitStatus)
	{
		case QProcess::NormalExit:
			//if (isMountImage)
			//	isMountImage = false;
			//else
			//	isMountImage = true;
		break;
		case QProcess::CrashExit:
			//isMountImage = false;
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error al salir del proceso") +"\nError: "+ fGrl->intToStr(exitCode), QMessageBox::Ok);
		break;
	}

	grlProcessMount->terminate();
	grlProcessMount->close();

	//ui->mnu_ejecutar_montaje->setEnabled(false);

	// TODO: Si es lento el montaje añadir posibilidad de retardo?
	//if (isMountImage && cfgExec.isCfgExec)
	//	QTimer::singleShot(1000, this, SLOT(fin_ProcesoMountEjecutar()));
}


//void GrLida::fin_ProcesoMountEjecutar()
//{
//	ejecutar(cfgExec.f_exe, cfgExec.f_exe_param, cfgExec.f_exe_path);
//}

void GrLida::error_msg_Proceso(QProcess::ProcessError error)
{
	switch (error)
	{
		case QProcess::FailedToStart:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error al iniciar el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::Crashed:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error en el proceso, tiempo después de empezar con éxito"), QMessageBox::Ok);
		break;
		case QProcess::Timedout:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error, waitFor...() última función el tiempo de espera"), QMessageBox::Ok);
		break;
		case QProcess::WriteError:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error al intentar escribir en el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::ReadError:
			QMessageBox::critical(this, tituloGrl(), tr("Se ha producido un error al intentar leer en el proceso"), QMessageBox::Ok);
		break;
		case QProcess::UnknownError:
			QMessageBox::critical(this, tituloGrl(), tr("Ha ocurrido un error desconocido"), QMessageBox::Ok);
		break;
	}
}

void GrLida::error_Proceso(QProcess::ProcessError error)
{
	error_msg_Proceso(error);

	grlProcess->terminate();
	grlProcess->close();

	ui->mnu_ejecutar_juego_big->setEnabled(cfgExec.isCfgExec);
	ui->mnu_ejecutar_juego->setEnabled(cfgExec.isCfgExec);
	ui->mnu_ejecutar_setup_big->setEnabled(cfgExec.isCfgSetup);
	ui->mnu_ejecutar_setup->setEnabled(cfgExec.isCfgSetup);

	this->show();

	if (isTrayIcon)
	{
		trayIcon->hide();
	}
}

void GrLida::error_ProcesoMount(QProcess::ProcessError error)
{
	error_msg_Proceso(error);

	grlProcessMount->terminate();
	grlProcessMount->close();
}
