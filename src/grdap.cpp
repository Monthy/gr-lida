/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection and
 * GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
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

#include "grdap.h"
#include "ui_grdap.h"

GrDap::GrDap(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::GrDap)
{
	ui->setupUi(this);

	fGrl       = new Funciones();
	grdapView  = new GrDapView(ui->frame_gv);
	isZip      = false;
	visor_show = false;

	this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd());
	this->setWindowIcon(QIcon(":/img16/gr-dap.png"));

	grlDir.Home   = fGrl->GRlidaHomePath();
	grlDir.Temp   = grlDir.Home +"temp/";
	grlDir.Iconos = grlDir.Home +"iconos/dap/";

	fGrl->comprobarDirectorio(grlDir.Home);
	fGrl->comprobarDirectorio(grlDir.Temp);
	fGrl->comprobarDirectorio(grlDir.Iconos);

	cargarConfig();

// centra la aplicacion en el escritorio
	if( !grdCfg.main_fullscreen )
		this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
	if( !grdCfg.main_geometry.isEmpty() )
		this->restoreGeometry( grdCfg.main_geometry );
	isInicio = false;
}

GrDap::~GrDap()
{
	delete grdapView;
	delete fGrl;
	delete ui;
}

void GrDap::closeEvent(QCloseEvent *event)
{
	guardarConfig();
	event->accept();
}

bool GrDap::eventFilter(QObject *object, QEvent *event)
{
	if( event->type() == QEvent::KeyPress )
	{
		QKeyEvent *key_Event = static_cast<QKeyEvent*>(event);

		switch( key_Event->key() )
		{
			case Qt::Key_Plus:
				emit on_btn_zoom_in_clicked();
				return true;
			break;
			case Qt::Key_Minus:
				emit on_btn_zoom_out_clicked();
				return true;
			break;
			case Qt::Key_Slash:
				emit on_btn_zoom_normal_clicked();
				return true;
			break;
			case Qt::Key_0:
				if( ui->frame_images->isVisible() )
					emit on_btn_rotar_img_clicked();
				if( ui->frame_a->isVisible() )
					emit on_btn_rotar_a_clicked();
				if( ui->frame_b->isVisible() )
					emit on_btn_rotar_b_clicked();
				if( ui->frame_c->isVisible() )
					emit on_btn_rotar_c_clicked();
				if( ui->frame_d->isVisible() )
					emit on_btn_rotar_d_clicked();
				return true;
			break;
			case Qt::Key_Left:
			case Qt::Key_Up:
				if( ui->frame_images->isVisible() )
					emit on_btn_img_back_clicked();
				return true;
			break;
			case Qt::Key_Right:
			case Qt::Key_Down:
				if( ui->frame_images->isVisible() )
					emit on_btn_img_next_clicked();
				return true;
			break;
			default:
				return false;
			break;
		}
	}

	return QMainWindow::eventFilter(object, event);
}

void GrDap::dropEvent(QDropEvent *event)
{
	if( event->mimeData()->urls().size() > -1 )
		cargarArchivo(event->mimeData()->urls().at(0).toLocalFile());
	event->accept();
}

void GrDap::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void GrDap::cargarConfig()
{
	QMenu *menu = new QMenu(this);
	menu->addAction(ui->btn_acercade);
	menu->addAction(ui->btn_ayuda);
	menu->addSeparator();
	menu->addAction(ui->btn_info);
	menu->addAction(ui->btn_fullscreen);
	ui->btn_menu->setMenu(menu);

	QSettings settings(grlDir.Home +"GR-dap.conf", QSettings::IniFormat);
	settings.beginGroup("MainState");
		grdCfg.main_fullscreen = settings.value("main_fullscreen", false).toBool();
		grdCfg.main_maximized  = settings.value("main_maximized" , false).toBool();
		grdCfg.main_geometry   = settings.value("main_geometry"  , "").toByteArray();
	settings.endGroup();
	settings.beginGroup("OpcVer");
		grdCfg.show_info = settings.value("show_info", false).toBool();
	settings.endGroup();

	if( grdCfg.main_maximized )
		this->setWindowState( this->windowState() | Qt::WindowMaximized );

	isInicio = true;
	emit on_btn_fullscreen_toggled( grdCfg.main_fullscreen );

	ui->btn_fullscreen->setChecked( grdCfg.main_fullscreen );
	ui->btn_info->setChecked( grdCfg.show_info );
	ui->frame_info->setVisible( grdCfg.show_info );
	ui->verticalLayout_frame_gv->addWidget(grdapView);

	img_ext.clear();
	img_ext << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tif" << "*.tiff" << "*.xbm" << "*.xpm" << "*.svg";

	setDefecto();

// Efectos para el visor de texto oculto: Indiana Jones y la Última Cruzada
	ui->cbx_visor_mode->clear();
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("SourceOver")     , QPainter::CompositionMode_SourceOver     );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("DestinationOver"), QPainter::CompositionMode_DestinationOver);
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Clear")          , QPainter::CompositionMode_Clear          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Source")         , QPainter::CompositionMode_Source         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Destination")    , QPainter::CompositionMode_Destination    );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("SourceIn")       , QPainter::CompositionMode_SourceIn       );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("DestinationIn")  , QPainter::CompositionMode_DestinationIn  );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("SourceOut")      , QPainter::CompositionMode_SourceOut      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("DestinationOut") , QPainter::CompositionMode_DestinationOut );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("SourceAtop")     , QPainter::CompositionMode_SourceAtop     );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("DestinationAtop"), QPainter::CompositionMode_DestinationAtop);
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Xor")            , QPainter::CompositionMode_Xor            );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Plus")           , QPainter::CompositionMode_Plus           );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Multiply")       , QPainter::CompositionMode_Multiply       );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Screen")         , QPainter::CompositionMode_Screen         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Overlay")        , QPainter::CompositionMode_Overlay        );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Darken")         , QPainter::CompositionMode_Darken         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Lighten")        , QPainter::CompositionMode_Lighten        );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("ColorDodge")     , QPainter::CompositionMode_ColorDodge     );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("ColorBurn")      , QPainter::CompositionMode_ColorBurn      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("HardLight")      , QPainter::CompositionMode_HardLight      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("SoftLight")      , QPainter::CompositionMode_SoftLight      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Difference")     , QPainter::CompositionMode_Difference     );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Exclusion")      , QPainter::CompositionMode_Exclusion      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op SourceOrDestination")       , QPainter::RasterOp_SourceOrDestination       );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op SourceAndDestination")      , QPainter::RasterOp_SourceAndDestination      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op SourceXorDestination")      , QPainter::RasterOp_SourceXorDestination      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op NotSourceAndNotDestination"), QPainter::RasterOp_NotSourceAndNotDestination);
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op NotSourceOrNotDestination") , QPainter::RasterOp_NotSourceOrNotDestination );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op NotSourceXorDestination")   , QPainter::RasterOp_NotSourceXorDestination   );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op NotSource")                 , QPainter::RasterOp_NotSource                 );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op NotSourceAndDestination")   , QPainter::RasterOp_NotSourceAndDestination   );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), tr("Op SourceAndNotDestination")   , QPainter::RasterOp_SourceAndNotDestination   );
	ui->cbx_visor_mode->setCurrentIndex(16);
// --
	this->setAcceptDrops(true);
	this->installEventFilter(this);

	grdapView->installEventFilter(this);
	grdapView->setFocus();
//	connect(grdapView, SIGNAL(openfileDragDrop(QString)), this, SLOT(openfileDragDrop(QString)));
}

/*void GrDap::openfileDragDrop(QString filename)
{
	if( !filename.isEmpty() )
		cargarArchivo(filename);
}*/

void GrDap::guardarConfig()
{
	QSettings settings(grlDir.Home +"GR-dap.conf", QSettings::IniFormat);
	settings.beginGroup("MainState");
		settings.setValue("main_fullscreen", grdCfg.main_fullscreen );
		settings.setValue("main_maximized" , this->isMaximized()    );
		settings.setValue("main_geometry"  , this->saveGeometry()   );
	settings.endGroup();
	settings.beginGroup("OpcVer");
		settings.setValue("show_info", grdCfg.show_info );
	settings.endGroup();
}

QPixmap GrDap::loadPixmap(QString filename)
{
	QPixmap capa;
	if( filename.isEmpty() )
		capa.load(fGrl->Theme() +"img16/sinimg.png");
	else {
		if( isZip )
			capa = zip.loadImagen(filename);
		else
			capa.load(filename);
	}

	return capa;
}

QBitmap GrDap::loadBitmap(QString filename)
{
	return QBitmap(loadPixmap(filename));
}

void GrDap::setCbxNextIndex(QComboBox *cbx)
{
	int count_cbx = cbx->count();
	if( count_cbx > -1 )
	{
		int pos = cbx->currentIndex();
		if( count_cbx > 0 && pos != -1)
		{
			if( pos >= count_cbx - 1 )
				cbx->setCurrentIndex(0);
			else
				cbx->setCurrentIndex(pos + 1);
		}
	}
}

void GrDap::setCbxBackIndex(QComboBox *cbx)
{
	int count_cbx = cbx->count();
	if( count_cbx > -1 )
	{
		int pos = cbx->currentIndex();
		if( count_cbx > 0 && pos != -1)
		{
			if( pos <= 0 )
				cbx->setCurrentIndex(count_cbx - 1);
			else
				cbx->setCurrentIndex(pos - 1);
		}
	}
}

void GrDap::setColorBtn(QColor color)
{
	int w = 24;
	int h = 16;
	QPixmap img(w, h);
	QPainter painter(&img);
	painter.fillRect(img.rect(), color);
	painter.setPen(Qt::black);
	painter.drawRect(0, 0, w-1, h-1);
	painter.end();
	ui->btn_visor_color->setIconSize(QSize(w, h));
	ui->btn_visor_color->setIcon(QIcon(img));
}

void GrDap::setDefecto()
{
	stDir        = "";
	texto_pagina = "";
	visor_color  = QColor(255, 0, 0, 255);
	isZip        = false;
	visor_show   = false;

	ui->frame_zoom->setVisible(false);
	ui->frame_a->setVisible(false);
	ui->frame_b->setVisible(false);
	ui->frame_c->setVisible(false);
	ui->frame_d->setVisible(false);
	ui->frame_cartas->setVisible(false);
	ui->frame_images->setVisible(false);
	ui->frame_visor->setVisible(false);
	ui->btn_visor->setChecked(false);
	ui->btn_visor->setVisible(false);

	ui->lb_a_rotar->setText("0"+ tr("º"));
	ui->lb_b_rotar->setText("0"+ tr("º"));
	ui->lb_c_rotar->setText("0"+ tr("º"));
	ui->lb_d_rotar->setText("0"+ tr("º"));
	ui->lb_img_rotar->setText("0"+ tr("º"));

	config.clear();
	list_zip.clear();
	list_titulos.clear();
	ui->txt_info->clear();
}

void GrDap::getInfoImg()
{
	if( config["info_type"] == "imagenes" )
	{
		QHash<QString, QString> info_file;
		const int index = ui->cbx_img_pagina->currentIndex();
		if( isZip )
		{
			stQtZip f_info = zip.getFileInfo( stDir + ui->cbx_img_pagina->itemData(index).toString() );

			info_file["f_nombre"]  = f_info.filename;
			info_file["f_size"]    = f_info.size;
			info_file["f_path"]    = f_info.path;
			info_file["f_crc32"]   = f_info.crc32;
			info_file["f_extra_1"] = "<b>Coment:</b> "+ zip.getCommentZip() +"<br>";
			info_file["f_extra_2"] = "";
		} else {
			stFileInfo f_info = fGrl->getInfoFile(stDir + ui->cbx_img_pagina->itemData(index).toString(), hashAll);

			info_file["f_nombre"]  = f_info.NameExt;
			info_file["f_size"]    = f_info.Size;
			info_file["f_path"]    = f_info.Path;
			info_file["f_crc32"]   = f_info.Crc32;
			info_file["f_extra_1"] = "<b>Sha1:</b> "+ f_info.Sha1 +"<br>";
			info_file["f_extra_2"] = "<b>Md5:</b> "+ f_info.Md5 +"<br>";
		}

		ui->txt_info->setHtml("<b>"+ tr("Nombre") +":</b> "+ info_file["f_nombre"] +"<br>"
						"<b>"+ tr("Tamaño") +":</b> "+ info_file["f_size"] +"<br>"
						"<b>"+ tr("Directorio") +":</b> "+ info_file["f_path"] +"<br>"
						"<b>Crc32:</b> "+ info_file["f_crc32"] +"<br>"
						""+ info_file["f_extra_1"] + info_file["f_extra_2"] );
	}
}

void GrDap::cargarArchivo(QString filename)
{
	setDefecto();

	stFileInfo f_info = fGrl->getInfoFile(filename);

	if( img_ext.contains("*"+ f_info.Ext.toLower()) )
	{
		config["info_type"] = "imagenes";
		list_zip << f_info.FilePath;
	}
	else if( f_info.isDir )
	{
		QStringList lista_filtros, lista_cfg_name, lista_cfg_path;
		lista_filtros.clear();
		lista_cfg_name.clear();
		lista_cfg_path.clear();

		lista_filtros << "*.dap" << "*.conf" << "*.dapz" << "*.zip" << "*.cbz";
		QDirIterator dir_cfg(f_info.FilePath, lista_filtros, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories);
		while( dir_cfg.hasNext() )
		{
			dir_cfg.next();
			lista_cfg_name << dir_cfg.fileName();
			lista_cfg_path << dir_cfg.filePath();
		}

		int count_list_cfg = lista_cfg_name.size();
		if( lista_cfg_name.isEmpty() )
		{
			config["info_type"] = "imagenes";
			QDirIterator dir_img(f_info.FilePath, img_ext, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories);
			while( dir_img.hasNext() )
			{
				dir_img.next();
				list_zip << dir_img.filePath();
			}
		} else {
			if( count_list_cfg > 1 )
			{
				bool ok = false;
				QString file_select = QInputDialog::getItem(this, tr("Seleccionar"), tr("Seleccione la configuración que quieres abrir."), lista_cfg_name, 0, false, &ok);
				if( ok )
					filename = lista_cfg_path.at(lista_cfg_name.indexOf(file_select));
				else
					filename = lista_cfg_path.at(0);
			} else
				filename = lista_cfg_path.at(0);

			cargarArchivo( filename );
			return;
		}
	}
	else if( f_info.Ext.toLower() == ".dapz" || f_info.Ext.toLower() == ".zip" || f_info.Ext.toLower() == ".cbz" )
	{
		int index = -1;
		isZip = true;

		zip.abrirZip(f_info.FilePath, "");

		if( zip.isZipOpen )
		{
			list_zip = zip.listaZip();

			QRegExp rx(".+((\\.dap$)|(\\.conf$))");
			QStringList result = list_zip.filter(rx);
			const int num_result = result.size();

			if( !result.isEmpty() && num_result > -1 )
			{
				if( num_result > 1 )
				{
					bool ok = false;
					QString file_select = QInputDialog::getItem(this, tr("Seleccionar"), tr("Seleccione la configuración que quieres abrir."), result, 0, false, &ok);
					if( ok )
						index = list_zip.indexOf(file_select, 0);
					else
						index = list_zip.indexOf(result.at(0), 0);
				} else
					index = list_zip.indexOf(result.at(0), 0);
			}

			if( index > -1 )
			{
				QString ruleta_conf = list_zip.at(index);
				QFile data_out(grlDir.Temp +"conf_ruleta.ini");
				if( data_out.open(QFile::WriteOnly | QIODevice::Text | QFile::Truncate ) )
				{
					QTextStream out(&data_out);
					out << zip.loadTexto(ruleta_conf);
					out.flush();
				}
				data_out.close();

				filename = grlDir.Temp +"conf_ruleta.ini";
				int last = ruleta_conf.lastIndexOf("/");
				if( last > -1 )
				{
					stDir = ruleta_conf.left(last);
					if( !stDir.endsWith("/") )
						stDir.append("/");
				}
			} else
				config["info_type"] = "imagenes";
		} else
			return;
	} else
		stDir = f_info.Path;

	QSettings settings(filename, QSettings::IniFormat);
//	settings.setIniCodec("UTF-8");
	settings.beginGroup("info");
		config["info_type"] = settings.value("type", config["info_type"]).toString();
	settings.endGroup();

	ui->frame_zoom->setVisible(true);

	if( config["info_type"] == "ruletas" || config["info_type"] == "cartas_aitd2" || config["info_type"] == "texto_oculto" )
	{
		settings.beginGroup("info");
			config["info_title"]       = settings.value("title", "").toString();
			config["info_autor"]       = settings.value("autor", "").toString();
			config["info_icono"]       = settings.value("icon" , "").toString();
			config["info_url"]         = settings.value("url"  , "").toString();
			config["info_description"] = settings.value("description", "").toString();
			config["info_type"]        = settings.value("type", config["info_type"]).toString();
		settings.endGroup();

		QString zip_coment = "";
		if( isZip )
			zip_coment = "<br>"+ zip.getCommentZip();

		ui->txt_info->setHtml("<b>"+ tr("Título del juego") +":</b> "+ config["info_title"] +"<br>"
						"<b>"+ tr("Autor") +":</b> "+ config["info_autor"] +"<br>"
						"<b>"+ tr("Url") +":</b> <a href=\""+ config["info_url"] +"\">"+ config["info_url"] +"</a><br>"
						"<b>"+ tr("Descripción") +":</b><br>"+ config["info_description"] +"<br>"+ zip_coment);

		if( !config["info_icono"].isEmpty() )
		{
			QPixmap imgIcon = loadPixmap(stDir + config["info_icono"]);
	//		imgIcon.save(grlDir.Iconos + config["info_icono"], "PNG");
			this->setWindowIcon(QIcon(imgIcon));
		}

		settings.beginGroup("lienzo");
			int scene_width        = settings.value("scene_width" , 400).toInt();
			int scene_height       = settings.value("scene_height", 400).toInt();
			config["lienzo_fondo"] = settings.value("bg_image", "").toString();
			QStringList bgcolor    = settings.value("bg_color", "125, 125, 125, 255").toStringList();
		settings.endGroup();

		grdapView->setNewScene(scene_width, scene_height);

		if( config["lienzo_fondo"].isEmpty() )
		{
			if( bgcolor.size() > -1 && bgcolor.size() == 3)
				grdapView->setBackgroundBrush(QBrush(fGrl->getColor(bgcolor)));
		} else
			grdapView->setBackgroundBrush( loadPixmap(stDir + config["lienzo_fondo"]) );

		if( config["info_type"] == "ruletas" )
		{
			QString str, strm;
			settings.beginGroup("ruletas");
				for(int i = 0; i < 4; ++i)
				{
					QString ids = fGrl->IntToStr(i);
					config.insert("ruleta_"+ ids, settings.value("ruleta_"+ ids, "").toString() );
					config.insert("ruleta_"+ ids +"_mask", settings.value("ruleta_"+ ids +"_mask", "").toString() );

					str = strm = "";
					if( !config["ruleta_"+ ids].isEmpty() )
						str = stDir + config["ruleta_"+ ids];

					QPixmap capa = loadPixmap(str);

					if( !config["ruleta_"+ ids +"_mask"].isEmpty() )
					{
						strm = stDir + config["ruleta_"+ ids +"_mask"];
						capa.setMask( loadBitmap(strm) );
					}

					grdapView->imgItemAddScene(i, capa);
				}
			settings.endGroup();

			ui->frame_a->setVisible(!config["ruleta_1"].isEmpty());
			ui->frame_b->setVisible(!config["ruleta_2"].isEmpty());
			ui->frame_c->setVisible(!config["ruleta_3"].isEmpty());
			ui->frame_d->setVisible(!config["ruleta_4"].isEmpty());
			ui->slider_rotar_a->setValue(0);
			ui->slider_rotar_b->setValue(0);
			ui->slider_rotar_c->setValue(0);
			ui->slider_rotar_d->setValue(0);
		}
		else if( config["info_type"] == "cartas_aitd2" )
		{
			ui->frame_cartas->setVisible(true);
			ui->cbx_carta_top->clear();
			ui->cbx_carta_bot->clear();

			settings.beginGroup("cartas_aitd2");
				for(int i = 0; i < 12; ++i)
				{
					QString ids = fGrl->IntToStr(i);
					ui->cbx_carta_top->addItem(QIcon(fGrl->Theme() +"img16/cartas.png"), settings.value("titulo_"+ ids, tr("Carta") +" "+ ids).toString(), settings.value("carta_"+ ids, "").toString());
					ui->cbx_carta_bot->addItem(QIcon(fGrl->Theme() +"img16/cartas.png"), settings.value("titulo_"+ ids, tr("Carta") +" "+ ids).toString(), settings.value("carta_"+ ids, "").toString());
				}
			settings.endGroup();

			grdapView->imgItemAddScene(0, loadPixmap(stDir + ui->cbx_carta_top->itemData(0).toString()));
			grdapView->imgItemAddScene(1, loadPixmap(stDir + ui->cbx_carta_top->itemData(0).toString()));

			ui->cbx_carta_top->setCurrentIndex(0);
			ui->cbx_carta_bot->setCurrentIndex(0);
		}
		else if( config["info_type"] == "texto_oculto" )
		{
			list_zip.clear();
			settings.beginGroup("texto_oculto");
				visor_color = fGrl->getColor( settings.value("visor_color", "255, 0, 0, 255").toStringList() );
				visor_show  = settings.value("visor_show", true).toBool();
				const int num_pages = settings.value("num_pages", 0).toInt();
				for(int i = 0; i < num_pages; ++i)
				{
					QString ids = fGrl->IntToStr(i);
					list_titulos << settings.value("titulo_"+ ids, fGrl->IntToStr(i+1)).toString();
					list_zip << settings.value("pagina_"+ ids, "").toString();
				}
			settings.endGroup();
		}
	}

	if( config["info_type"] == "imagenes" || config["info_type"] == "texto_oculto" )
		cargarImagenes(list_zip, 0, false);

	this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd() +": "+ config["info_title"] + texto_pagina);
	emit on_btn_zoom_normal_clicked();
	emit on_btn_rotar_img_clicked();
}

void GrDap::cargarImagen(QString filename)
{
	QStringList images;
	images << filename;
	cargarImagenes(images, 0, true);
}

void GrDap::cargarImagenes(QStringList images, int index, bool directo)
{
	if( directo )
	{
		setDefecto();
		config["info_type"] = "imagenes";
		ui->frame_zoom->setVisible(true);
	}

	ui->frame_images->setVisible(true);
	ui->btn_visor->setVisible(true);

	if( config["info_type"] == "texto_oculto" )
	{
		ui->btn_visor->setChecked(visor_show);
		emit on_btn_visor_clicked(visor_show);
		ui->lb_img_pagina->setText( tr("Página") );
	} else {
		grdapView->setBackgroundBrush(Qt::darkGray);
		ui->lb_img_pagina->setText( tr("Imagen") );
	}

	ui->cbx_img_pagina->clear();
	num_img_pagina = 0;
	const int num_images = images.size();
	for(int i = 0; i < num_images; ++i)
	{
		if( img_ext.contains("*"+ fGrl->getInfoFile(images.at(i)).Ext.toLower()) )
		{
			QString titulo = fGrl->getInfoFile(images.at(i)).NameExt;
			if( config["info_type"] == "texto_oculto" )
				titulo = list_titulos.at(i).isEmpty() ? titulo : list_titulos.at(i);
			ui->cbx_img_pagina->addItem(QIcon(fGrl->Theme() +"img16/imagen.png"), titulo, images.at(i));
			num_img_pagina++;
		}
	}

	ui->lb_img_pagina_total->setText( fGrl->IntToStr(index+1) +"/"+ fGrl->IntToStr(num_img_pagina) );

	QPixmap capa = loadPixmap(stDir + ui->cbx_img_pagina->itemData(index).toString());

	setColorBtn( visor_color );
	grdapView->setNewScene(capa.width(), capa.height());
	grdapView->setVisorColor( visor_color );
	grdapView->imgItemAddScene(0, capa, false);
	grdapView->imgItemAddScene(1, QPixmap(fGrl->Theme() +"images/visor.png"), true);
	grdapView->setVisorVisible(ui->btn_visor->isChecked());

	ui->cbx_img_pagina->setCurrentIndex(index);
	ui->cbx_visor_mode->setCurrentIndex(16);
	emit on_cbx_visor_mode_activated(ui->cbx_visor_mode->currentIndex());
	getInfoImg();

	texto_pagina = (config["info_title"].isEmpty() ? "" : " - ")+ ui->cbx_img_pagina->currentText();

	if( directo )
	{
		this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd() +": "+ config["info_title"] + texto_pagina);
		emit on_btn_zoom_normal_clicked();
		emit on_btn_rotar_img_clicked();
	}
}

// --

void GrDap::on_btn_cerrar_info_clicked()
{
	ui->btn_info->setChecked(false);
	emit on_btn_info_toggled(false);
}

void GrDap::on_btn_salir_clicked()
{
	close();
}

void GrDap::on_btn_abrir_clicked()
{
	QString ext_img = img_ext.join(" ");
	QString archivo = QFileDialog::getOpenFileName(this, tr("Selecciona un archivo"), "",
				tr("Archivos soportados") +" - (*.conf *.dap *.dapz *.zip *.cbz *"+ ext_img +");;"+
				tr("Ruleta de protección") +" - (*.conf *.dap *.dapz *.zip);;"+
				tr("Comic o manuales") +" - (*.zip *.cbz);;"+
				tr("Imágenes soportadas") +" - ("+ ext_img +");;"+
				tr("Todos los archivo") +" (*)", 0);

	if( !archivo.isEmpty() && QFile::exists(archivo) )
		cargarArchivo( QDir::toNativeSeparators( archivo ) );
}

void GrDap::on_slider_zoom_valueChanged(int value)
{
	grdapView->zoomValue(value);
}

void GrDap::on_btn_zoom_in_clicked()
{
	ui->slider_zoom->setValue(ui->slider_zoom->value()+1);
}

void GrDap::on_btn_zoom_out_clicked()
{
	ui->slider_zoom->setValue(ui->slider_zoom->value()-1);
}

void GrDap::on_btn_zoom_normal_clicked()
{
	ui->slider_zoom->setValue(250);
}

void GrDap::on_btn_fullscreen_toggled(bool checked)
{
	grdCfg.main_fullscreen = checked;
	if( grdCfg.main_fullscreen )
		this->showFullScreen();
	else {
		if( !isInicio )
			this->showNormal();
	}
}

void GrDap::on_btn_info_toggled(bool checked)
{
	grdCfg.show_info = checked;
	ui->frame_info->setVisible(checked);
}

void GrDap::on_btn_acercade_triggered()
{
	QDialog *AcercaD = new QDialog(this);
	AcercaD->resize(464, 340);
	AcercaD->setMinimumSize(QSize(464, 340));
	AcercaD->setMaximumSize(QSize(464, 340));
	AcercaD->setWindowTitle("GR-dap - Dial-A-Protection");
	AcercaD->setWindowIcon(QIcon(":/img16/gr-dap.png"));

	QVBoxLayout *vl_acercad = new QVBoxLayout(AcercaD);
	vl_acercad->setContentsMargins(4, 4, 4, 4);
	vl_acercad->setObjectName("vl_acercad");

	QLabel *lbad_logo = new QLabel(AcercaD);
	lbad_logo->setObjectName("lbad_logo");
	lbad_logo->setMinimumHeight(128);
	lbad_logo->setMaximumHeight(128);
	lbad_logo->setPixmap(QPixmap(":/images/gr-dap.png"));
	lbad_logo->setAlignment(Qt::AlignCenter);

	QLabel *lbad_info = new QLabel(AcercaD);
	lbad_info->setObjectName("lbad_info");
	lbad_info->setAlignment(Qt::AlignCenter);
	lbad_info->setWordWrap(true);
	lbad_info->setOpenExternalLinks(true);

	QSpacerItem *vs_acercad = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

	QHBoxLayout *hl_acercad = new QHBoxLayout();
	hl_acercad->setContentsMargins(0, 0, 0, 0);
	hl_acercad->setObjectName("hl_acercad");

	QSpacerItem *hs_acercad = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	QPushButton *btnCerrar = new QPushButton(AcercaD);
	btnCerrar->setObjectName("btnCerrar");
	btnCerrar->setText(tr("Cerrar"));

	hl_acercad->addItem(hs_acercad);
	hl_acercad->addWidget(btnCerrar);

	vl_acercad->addWidget(lbad_logo);
	vl_acercad->addWidget(lbad_info);
	vl_acercad->addItem(vs_acercad);
	vl_acercad->addLayout(hl_acercad);

	QString str_info_ad = "";
	str_info_ad.append("<p style=\"font-size:18pt; font-weight:600;\">GR-dap v"+ fGrl->versionGrd() +"</p>");
	str_info_ad.append("<p style=\"font-size:10pt;\">"+ tr("Diseñado y programado por") +": <b>Pedro A. Garcia Rosado</b> "+ tr("alias") +" <b>Monthy</b></p>");
	str_info_ad.append("<p style=\"font-size:10pt;\">"+ tr("Visor de protecciones de juegos") +", "+ tr("Ruletas del Monkey Island") +"<br />"+ tr("Cartas del Alone in the Dark 2") +"<br />"+ tr("Texto oculto en el manual de Indiana Jones y la Última Cruzada") +"</p>");
	str_info_ad.append("<p style=\"font-size:10pt;\"><a href=\"http://www.gr-lida.org/\" style=\"text-decoration:none;\">http://www.gr-lida.org/</a></p>");

	lbad_info->setText(str_info_ad);

	connect(btnCerrar, SIGNAL(clicked()), AcercaD, SLOT(close()));

// centra la aplicacion en el escritorio
	AcercaD->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, AcercaD->size(), qApp->desktop()->availableGeometry()));
	AcercaD->exec();
	delete AcercaD;
}

// Ruletas de la A a la D
void GrDap::on_slider_rotar_a_valueChanged(int value)
{
	grdapView->imgItemRotar(1, value);
	ui->lb_a_rotar->setText(fGrl->IntToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_b_valueChanged(int value)
{
	grdapView->imgItemRotar(2, value);
	ui->lb_b_rotar->setText(fGrl->IntToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_c_valueChanged(int value)
{
	grdapView->imgItemRotar(3, value);
	ui->lb_c_rotar->setText(fGrl->IntToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_d_valueChanged(int value)
{
	grdapView->imgItemRotar(4, value);
	ui->lb_d_rotar->setText(fGrl->IntToStr(value) + tr("º"));
}

void GrDap::on_btn_rotar_a_clicked()
{
	ui->slider_rotar_a->setValue(0);
}

void GrDap::on_btn_rotar_b_clicked()
{
	ui->slider_rotar_b->setValue(0);
}

void GrDap::on_btn_rotar_c_clicked()
{
	ui->slider_rotar_c->setValue(0);
}

void GrDap::on_btn_rotar_d_clicked()
{
	ui->slider_rotar_d->setValue(0);
}
// --

// Visor de cartas: Alone in the Dark 2
void GrDap::on_cbx_carta_top_activated(int index)
{
	if( index > -1 )
		grdapView->setPixmapItem(1, loadPixmap(stDir + ui->cbx_carta_top->itemData(index).toString()));
}

void GrDap::on_cbx_carta_bot_activated(int index)
{
	if( index > -1 )
		grdapView->setPixmapItem(0, loadPixmap(stDir + ui->cbx_carta_bot->itemData(index).toString()));
}
// --

// Visor de imagenes
void GrDap::on_cbx_img_pagina_activated(int index)
{
	if( index > -1 )
	{
		grdapView->setPixmapItem(0, loadPixmap(stDir + ui->cbx_img_pagina->itemData(index).toString()), true);

		if( ui->frame_visor->isVisible() )
			emit on_cbx_visor_mode_activated(ui->cbx_visor_mode->currentIndex());

		texto_pagina = (config["info_title"].isEmpty() ? "" : " - ")+ ui->cbx_img_pagina->itemText(index);
		this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd() +": "+ config["info_title"] + texto_pagina);
		ui->lb_img_pagina_total->setText( fGrl->IntToStr(index+1) +"/"+ fGrl->IntToStr(num_img_pagina) );
		getInfoImg();
		emit on_btn_rotar_img_clicked();
	}
}

void GrDap::on_btn_img_back_clicked()
{
	setCbxBackIndex(ui->cbx_img_pagina);
	emit on_cbx_img_pagina_activated(ui->cbx_img_pagina->currentIndex());
}

void GrDap::on_btn_img_next_clicked()
{
	setCbxNextIndex(ui->cbx_img_pagina);
	emit on_cbx_img_pagina_activated(ui->cbx_img_pagina->currentIndex());
}

void GrDap::on_slider_rotar_img_valueChanged(int value)
{
	grdapView->rotarValue(value);
	ui->lb_img_rotar->setText(fGrl->IntToStr(value) + tr("º"));
}

void GrDap::on_btn_rotar_img_clicked()
{
	ui->slider_rotar_img->setValue(0);
}
// --

// Visor de texto oculto: Indiana Jones y la Última Cruzada
void GrDap::on_btn_visor_clicked(bool checked)
{
	ui->frame_visor->setVisible(checked);
	emit on_cbx_visor_mode_activated(ui->cbx_visor_mode->currentIndex());
	grdapView->setVisorVisible(checked);
}

void GrDap::on_btn_visor_color_clicked()
{
	QColor color = grdapView->getVisorColor();
	QColorDialog *colorDg = new QColorDialog(color, this);
	colorDg->setOption(QColorDialog::ShowAlphaChannel);
	connect(colorDg, SIGNAL(currentColorChanged(QColor)), this, SLOT(visorColorChanged(QColor)));
	if( colorDg->exec() == QDialog::Accepted )
		emit visorColorChanged( colorDg->selectedColor() );
	else
		emit visorColorChanged( color );
	delete colorDg;
}

void GrDap::on_cbx_visor_mode_activated(int index)
{
	if( index > -1 && ui->frame_visor->isVisible() )
	{
		QPainter::CompositionMode mode = (QPainter::CompositionMode)ui->cbx_visor_mode->itemData(index).toInt();
		grdapView->setVisorMode( mode );
	}
}

void GrDap::visorColorChanged(QColor color)
{
	setColorBtn(color);
	grdapView->setVisorColor( color );
}
// --
