/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014-2016 Pedro A. Garcia Rosado Aka Monthy
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
#include "grdap_acercad.h"
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

	grlDir.Home   = fGrl->dirApp();
	grlDir.Temp   = grlDir.Home +"temp/";
	grlDir.Iconos = grlDir.Home +"iconos/dap/";

	fGrl->comprobarDirectorio(grlDir.Home);
	fGrl->comprobarDirectorio(grlDir.Temp);
	fGrl->comprobarDirectorio(grlDir.Iconos);

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	if (!grdCfg.main_fullscreen)
		this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
	if (!grdCfg.main_geometry.isEmpty())
		this->restoreGeometry(grdCfg.main_geometry);
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

	grdapView->clearScene();

	if (z_is_open)
		z_file->closeArchive();

	event->ignore();
	setHidden(true);
}

bool GrDap::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *key_Event = static_cast<QKeyEvent*>(event);

		switch (key_Event->key())
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
				if (ui->frame_images->isVisible())
					emit on_btn_rotar_img_clicked();
				if (ui->frame_a->isVisible())
					emit on_btn_rotar_a_clicked();
				if (ui->frame_b->isVisible())
					emit on_btn_rotar_b_clicked();
				if (ui->frame_c->isVisible())
					emit on_btn_rotar_c_clicked();
				if (ui->frame_d->isVisible())
					emit on_btn_rotar_d_clicked();
				return true;
			break;
			case Qt::Key_Left:
			case Qt::Key_Up:
				if (ui->frame_images->isVisible())
					emit on_btn_img_back_clicked();
				return true;
			break;
			case Qt::Key_Right:
			case Qt::Key_Down:
				if (ui->frame_images->isVisible())
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
	if (event->mimeData()->urls().size() > -1)
		cargarArchivo(event->mimeData()->urls().at(0).toLocalFile());
	event->accept();
}

void GrDap::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void GrDap::setAssociation(QString ext, QString desc, QString ico)
{
//	if (!FileAssoc::checkAssociation(ext))
//	{
//		if (FileAssoc::setAssociation(ext, desc, "GR-dap", ico))
// 			FileAssoc::refreshExplorer();
//	}
}

void GrDap::clearAssociation(QString ext)
{
//	if (FileAssoc::clearAssociation(ext))
//		FileAssoc::refreshExplorer();
}

void GrDap::cargarConfig()
{
	z_file = new Qt7zip();
	is_load_7zlib = z_file->isLoad7zLib();

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

	QSettings settingsGRl(grlDir.Home +"GR-lida.conf", QSettings::IniFormat);
	settingsGRl.beginGroup("OpcGeneral");
		fGrl->setTheme(settingsGRl.value("NameDirTheme", "defecto").toString());
//		fGrl->setIdioma(settingsGRl.value("IdiomaSelect", "es_ES").toString());
	settingsGRl.endGroup();

#if defined(Q_WS_SIMULATOR) || defined(Q_OS_QNX)
	grdCfg.main_fullscreen = true;
	ui->btn_fullscreen->setEnabled(false);
#else
	ui->btn_fullscreen->setEnabled(true);
	if (grdCfg.main_maximized)
		this->setWindowState(this->windowState() | Qt::WindowMaximized);
#endif

	isInicio = true;
	emit on_btn_fullscreen_toggled(grdCfg.main_fullscreen);

	ui->btn_fullscreen->setChecked(grdCfg.main_fullscreen);
	ui->btn_info->setChecked(grdCfg.show_info);
	ui->frame_info->setVisible(grdCfg.show_info);
	ui->verticalLayout_frame_gv->addWidget(grdapView);

	dap_ext.clear();
	dap_ext << "*.dap" << "*.dapz" << "*.conf";

	img_ext.clear();
	img_ext << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tif" << "*.tiff" << "*.xbm" << "*.xpm" << "*.svg";

	comic_ext.clear();
	comic_ext << "*.cbz" << "*.cbr" << "*.cb7" << "*.cbt";

	z_ext.clear();
	z_ext << "*.zip" << "*.rar" << "*.7z" << "*.tar";

	setDefecto();

// Efectos para el visor de texto oculto: Indiana Jones y la Última Cruzada
	ui->cbx_visor_mode->clear();
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("SourceOver")                   , QPainter::CompositionMode_SourceOver         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("DestinationOver")              , QPainter::CompositionMode_DestinationOver    );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Clear")                        , QPainter::CompositionMode_Clear              );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Source")                       , QPainter::CompositionMode_Source             );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Destination")                  , QPainter::CompositionMode_Destination        );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("SourceIn")                     , QPainter::CompositionMode_SourceIn           );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("DestinationIn")                , QPainter::CompositionMode_DestinationIn      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("SourceOut")                    , QPainter::CompositionMode_SourceOut          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("DestinationOut")               , QPainter::CompositionMode_DestinationOut     );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("SourceAtop")                   , QPainter::CompositionMode_SourceAtop         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("DestinationAtop")              , QPainter::CompositionMode_DestinationAtop    );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Xor")                          , QPainter::CompositionMode_Xor                );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Plus")                         , QPainter::CompositionMode_Plus               );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Multiply")                     , QPainter::CompositionMode_Multiply           );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Screen")                       , QPainter::CompositionMode_Screen             );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Overlay")                      , QPainter::CompositionMode_Overlay            );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Darken")                       , QPainter::CompositionMode_Darken             );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Lighten")                      , QPainter::CompositionMode_Lighten            );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("ColorDodge")                   , QPainter::CompositionMode_ColorDodge         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("ColorBurn")                    , QPainter::CompositionMode_ColorBurn          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("HardLight")                    , QPainter::CompositionMode_HardLight          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("SoftLight")                    , QPainter::CompositionMode_SoftLight          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Difference")                   , QPainter::CompositionMode_Difference         );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Exclusion")                    , QPainter::CompositionMode_Exclusion          );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op SourceOrDestination")       , QPainter::RasterOp_SourceOrDestination       );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op SourceAndDestination")      , QPainter::RasterOp_SourceAndDestination      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op SourceXorDestination")      , QPainter::RasterOp_SourceXorDestination      );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op NotSourceAndNotDestination"), QPainter::RasterOp_NotSourceAndNotDestination);
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op NotSourceOrNotDestination") , QPainter::RasterOp_NotSourceOrNotDestination );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op NotSourceXorDestination")   , QPainter::RasterOp_NotSourceXorDestination   );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op NotSource")                 , QPainter::RasterOp_NotSource                 );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op NotSourceAndDestination")   , QPainter::RasterOp_NotSourceAndDestination   );
	ui->cbx_visor_mode->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Op SourceAndNotDestination")   , QPainter::RasterOp_SourceAndNotDestination   );
	ui->cbx_visor_mode->setCurrentIndex(16);
// --
	this->setAcceptDrops(true);
	this->installEventFilter(this);

	grdapView->installEventFilter(this);
	grdapView->setFocus();
//	connect(grdapView, SIGNAL(openfileDragDrop(QString)), this, SLOT(openfileDragDrop(QString)));
}

void GrDap::guardarConfig()
{
	QSettings settings(grlDir.Home +"GR-dap.conf", QSettings::IniFormat);
	settings.beginGroup("MainState");
		settings.setValue("main_fullscreen", grdCfg.main_fullscreen);
		settings.setValue("main_maximized" , this->isMaximized()   );
		settings.setValue("main_geometry"  , this->saveGeometry()  );
	settings.endGroup();
	settings.beginGroup("OpcVer");
		settings.setValue("show_info", grdCfg.show_info);
	settings.endGroup();
}

void GrDap::setTheme()
{
	ui->btn_salir->setIcon(QIcon(fGrl->theme() +"img16/cerrar.png"));
	ui->btn_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btn_zoom_in->setIcon(QIcon(fGrl->theme() +"img16/zoom_in.png"));
	ui->btn_zoom_out->setIcon(QIcon(fGrl->theme() +"img16/zoom_out.png"));
	ui->btn_zoom_normal->setIcon(QIcon(fGrl->theme() +"img16/zoom.png"));
	ui->btn_img_back->setIcon(QIcon(fGrl->theme() +"img16/go-back.png"));
	ui->btn_img_next->setIcon(QIcon(fGrl->theme() +"img16/go-next.png"));
	ui->btn_rotar_img->setIcon(QIcon(fGrl->theme() +"img16/rotar.png"));
	ui->btn_visor_color->setIcon(QIcon(fGrl->theme() +"img16/sinimg.png"));
	ui->btn_rotar_d->setIcon(QIcon(fGrl->theme() +"img16/rotar.png"));
	ui->btn_rotar_c->setIcon(QIcon(fGrl->theme() +"img16/rotar.png"));
	ui->btn_rotar_b->setIcon(QIcon(fGrl->theme() +"img16/rotar.png"));
	ui->btn_rotar_a->setIcon(QIcon(fGrl->theme() +"img16/rotar.png"));
	ui->btn_visor->setIcon(QIcon(fGrl->theme() +"img16/visor.png"));
	ui->btn_menu->setIcon(QIcon(fGrl->theme() +"img16/applications.png"));
	ui->btn_info->setIcon(QIcon(fGrl->theme() +"img16/informacion.png"));
	ui->btn_ayuda->setIcon(QIcon(fGrl->theme() +"img16/ayuda.png"));
	ui->btn_acercade->setIcon(QIcon(fGrl->theme() +"img16/acercad.png"));
	ui->btn_fullscreen->setIcon(QIcon(fGrl->theme() +"img16/fit_to_win.png"));
}

QPixmap GrDap::loadPixmap(QString filename)
{
	QPixmap capa;
	if (filename.isEmpty())
		capa.load(fGrl->theme() +"img16/sinimg.png");
	else {
		if (isZip && z_is_open)
		{
			QByteArray imageData;
			z_file->extractRaw(filename, imageData);
			if (!capa.loadFromData(imageData))
				capa.load(fGrl->theme() +"img16/sinimg.png");
		} else
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
	if (count_cbx > -1)
	{
		int pos = cbx->currentIndex();
		if (count_cbx > 0 && pos != -1)
		{
			if (pos >= count_cbx - 1)
				cbx->setCurrentIndex(0);
			else
				cbx->setCurrentIndex(pos + 1);
		}
	}
}

void GrDap::setCbxBackIndex(QComboBox *cbx)
{
	int count_cbx = cbx->count();
	if (count_cbx > -1)
	{
		int pos = cbx->currentIndex();
		if (count_cbx > 0 && pos != -1)
		{
			if (pos <= 0)
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
	z_list.clear();
	list_titulos.clear();
	ui->txt_info->clear();
}

void GrDap::getInfoImg(int index)
{
	if (config["info_type"] == "imagenes")
	{
		QHash<QString, QString> info_file;
		if (isZip)
		{
			szEntryInfo f_info;
			int idx = z_list.indexOf(stDir + ui->cbx_img_pagina->itemData(index).toString());
			if (idx > -1)
			{
				f_info = z_listInfo.at(idx);
				info_file["f_nombre"]  = f_info.name;
				info_file["f_size"]    = fGrl->intToStr(f_info.size);
				info_file["f_path"]    = f_info.path;
				info_file["f_crc32"]   = f_info.crc32;
				info_file["f_extra_1"] = "<b>Coment:</b><br>";// "<b>Coment:</b> "+ zip.getCommentZip() +"<br>";
				info_file["f_extra_2"] = "";
			} else {
				info_file["f_nombre"]  = "";
				info_file["f_size"]    = "";
				info_file["f_path"]    = "";
				info_file["f_crc32"]   = "";
				info_file["f_extra_1"] = "<b>Coment:</b><br>";
				info_file["f_extra_2"] = "";
			}
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
						""+ info_file["f_extra_1"] + info_file["f_extra_2"]);
	}
}

void GrDap::cargarArchivo(QString filename)
{
	setDefecto();

	QStringList list_ext;
	list_ext << "*.dapz" << z_ext << comic_ext;

	stFileInfo f_info = fGrl->getInfoFile(filename);

	if (img_ext.contains("*"+ f_info.Ext.toLower()))
	{
		config["info_type"] = "imagenes";
		z_list << f_info.FilePath;
	}
	else if (f_info.isDir)
	{
		QStringList lista_cfg_name, lista_cfg_path;
		lista_cfg_name.clear();
		lista_cfg_path.clear();

		list_ext.clear();
		list_ext << dap_ext << z_ext << comic_ext;

		QDirIterator dir_cfg(f_info.FilePath, list_ext, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories);
		while (dir_cfg.hasNext())
		{
			dir_cfg.next();
			lista_cfg_name << dir_cfg.fileName();
			lista_cfg_path << dir_cfg.filePath();
		}

		int count_list_cfg = lista_cfg_name.size();
		if (lista_cfg_name.isEmpty())
		{
			config["info_type"] = "imagenes";
			QDirIterator dir_img(f_info.FilePath, img_ext, QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories);
			while (dir_img.hasNext())
			{
				dir_img.next();
				z_list << dir_img.filePath();
			}
		} else {
			if (count_list_cfg > 1)
			{
				bool ok = false;
				QString file_select = QInputDialog::getItem(this, tr("Seleccionar"), tr("Seleccione la configuración que quieres abrir."), lista_cfg_name, 0, false, &ok);
				if (ok)
					filename = lista_cfg_path.at(lista_cfg_name.indexOf(file_select));
				else
					filename = lista_cfg_path.at(0);
			} else
				filename = lista_cfg_path.at(0);

			cargarArchivo(filename);
			return;
		}
	}
	else if (list_ext.contains("*"+ f_info.Ext.toLower(), Qt::CaseInsensitive))
	{
		isZip = true;
		if (is_load_7zlib)
		{
			z_is_open = z_file->open(f_info.FilePath);
			if (z_is_open)
			{
				z_listInfo = z_file->entryListInfo();
				z_list     = z_file->entryList();

				if (f_info.Ext.toLower() == ".dapz")
				{
					QRegExp rx(".+((\\.dap$)|(\\.conf$))");
					QStringList result = z_list.filter(rx);
					const int num_result = result.size();

					if (!result.isEmpty() && num_result > 0)
					{
						int index = -1;
						if (num_result > 1)
						{
							bool ok = false;
							QString file_select = QInputDialog::getItem(this, tr("Seleccionar"), tr("Seleccione la configuración que quieres abrir."), result, 0, false, &ok);
							if (ok)
								index = z_list.indexOf(file_select, 0);
							else
								index = z_list.indexOf(result.at(0), 0);
						} else
							index = z_list.indexOf(result.at(0), 0);

						if (index > -1)
						{
							QString ruleta_conf = z_list.at(index);
							z_file->extract(index, grlDir.Temp, "conf_ruleta.ini");

							filename = grlDir.Temp +"conf_ruleta.ini";
							int last = ruleta_conf.lastIndexOf("/");
							if (last > -1)
							{
								stDir = ruleta_conf.left(last);
								if (!stDir.endsWith("/"))
									stDir.append("/");
							}
						} else
							config["info_type"] = "imagenes";
					} else
						config["info_type"] = "imagenes";
				} else
					config["info_type"] = "imagenes";
			} else
				return;
		} else
			return;
	} else
		stDir = f_info.Path;

	QSettings settings(filename, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");
	settings.beginGroup("info");
		config["info_type"] = settings.value("type", config["info_type"]).toString();
	settings.endGroup();

	ui->frame_zoom->setVisible(true);

	if (config["info_type"] == "ruletas" || config["info_type"] == "cartas_aitd2" || config["info_type"] == "texto_oculto")
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
		if (isZip)
			zip_coment = "<br>"+ z_file->getComment();

		ui->txt_info->setHtml("<b>"+ tr("Título del juego") +":</b> "+ config["info_title"] +"<br>"
						"<b>"+ tr("Autor") +":</b> "+ config["info_autor"] +"<br>"
						"<b>"+ tr("Url") +":</b> <a href=\""+ config["info_url"] +"\">"+ config["info_url"] +"</a><br>"
						"<b>"+ tr("Descripción") +":</b><br>"+ config["info_description"] +"<br>"+ zip_coment);

		if (!config["info_icono"].isEmpty())
		{
			QPixmap imgIcon = loadPixmap(stDir + config["info_icono"]);
		//	imgIcon.save(grlDir.Iconos + config["info_icono"], "PNG");
			this->setWindowIcon(QIcon(imgIcon));
		}

		settings.beginGroup("lienzo");
			int scene_width        = settings.value("scene_width" , 400).toInt();
			int scene_height       = settings.value("scene_height", 400).toInt();
			config["lienzo_fondo"] = settings.value("bg_image", "").toString();
			QStringList bgcolor    = settings.value("bg_color", "125, 125, 125, 255").toStringList();
		settings.endGroup();

		grdapView->setNewScene(scene_width, scene_height);

		if (config["lienzo_fondo"].isEmpty())
		{
			if (bgcolor.size() > -1 && bgcolor.size() == 3)
				grdapView->setBackgroundBrush(QBrush(fGrl->getColor(bgcolor)));
		} else
			grdapView->setBackgroundBrush(loadPixmap(stDir + config["lienzo_fondo"]));

		if (config["info_type"] == "ruletas")
		{
			QString str, strm;
			settings.beginGroup("ruletas");
				for (int i = 0; i < 5; ++i)
				{
					QString ids = fGrl->intToStr(i);
					config.insert("ruleta_"+ ids, settings.value("ruleta_"+ ids, "").toString());
					config.insert("ruleta_"+ ids +"_mask", settings.value("ruleta_"+ ids +"_mask", "").toString());
					config.insert("ruleta_"+ ids +"_name", settings.value("ruleta_"+ ids +"_name", "").toString());

					str = strm = "";
					if (!config["ruleta_"+ ids].isEmpty())
						str = stDir + config["ruleta_"+ ids];

					QPixmap capa = loadPixmap(str);

					if (!config["ruleta_"+ ids +"_mask"].isEmpty())
					{
						strm = stDir + config["ruleta_"+ ids +"_mask"];
						capa.setMask(loadBitmap(strm));
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
			ui->lb_a->setText(!config["ruleta_1_name"].isEmpty() ? config["ruleta_1_name"] : "Ruleta A");
			ui->lb_b->setText(!config["ruleta_2_name"].isEmpty() ? config["ruleta_2_name"] : "Ruleta B");
			ui->lb_c->setText(!config["ruleta_3_name"].isEmpty() ? config["ruleta_3_name"] : "Ruleta C");
			ui->lb_d->setText(!config["ruleta_4_name"].isEmpty() ? config["ruleta_4_name"] : "Ruleta D");
		}
		else if (config["info_type"] == "cartas_aitd2")
		{
			ui->frame_cartas->setVisible(true);
			ui->cbx_carta_top->clear();
			ui->cbx_carta_bot->clear();

			settings.beginGroup("cartas_aitd2");
				for (int i = 0; i < 12; ++i)
				{
					QString ids = fGrl->intToStr(i);
					ui->cbx_carta_top->addItem(QIcon(fGrl->theme() +"img16/cartas.png"), settings.value("titulo_"+ ids, tr("Carta") +" "+ ids).toString(), settings.value("carta_"+ ids, "").toString());
					ui->cbx_carta_bot->addItem(QIcon(fGrl->theme() +"img16/cartas.png"), settings.value("titulo_"+ ids, tr("Carta") +" "+ ids).toString(), settings.value("carta_"+ ids, "").toString());
				}
			settings.endGroup();

			grdapView->imgItemAddScene(0, loadPixmap(stDir + ui->cbx_carta_top->itemData(0).toString()));
			grdapView->imgItemAddScene(1, loadPixmap(stDir + ui->cbx_carta_top->itemData(0).toString()));

			ui->cbx_carta_top->setCurrentIndex(0);
			ui->cbx_carta_bot->setCurrentIndex(0);
		}
		else if (config["info_type"] == "texto_oculto")
		{
			z_list.clear();
			settings.beginGroup("texto_oculto");
				visor_color = fGrl->getColor(settings.value("visor_color", "255, 0, 0, 255").toStringList());
				visor_show  = settings.value("visor_show", true).toBool();
				const int num_pages = settings.value("num_pages", 0).toInt();
				for (int i = 0; i < num_pages; ++i)
				{
					QString ids = fGrl->intToStr(i);
					list_titulos << settings.value("titulo_"+ ids, fGrl->intToStr(i+1)).toString();
					z_list << settings.value("pagina_"+ ids, "").toString();
				}
			settings.endGroup();
		}
	}

	if (config["info_type"] == "imagenes" || config["info_type"] == "texto_oculto")
		cargarImagenes(z_list, 0, false);

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
	if (directo)
	{
		setDefecto();
		config["info_type"] = "imagenes";
		ui->frame_zoom->setVisible(true);
	}

	ui->frame_images->setVisible(true);
	ui->btn_visor->setVisible(true);

	if (config["info_type"] == "texto_oculto")
	{
		ui->btn_visor->setChecked(visor_show);
		emit on_btn_visor_clicked(visor_show);
		ui->lb_img_pagina->setText(tr("Página"));
	} else {
		grdapView->setBackgroundBrush(Qt::darkGray);
		ui->lb_img_pagina->setText(tr("Imagen"));
	}

	ui->cbx_img_pagina->clear();
	num_img_pagina = 0;
	const int num_images = images.size();
	for (int i = 0; i < num_images; ++i)
	{
		if (img_ext.contains("*"+ fGrl->getInfoFile(images.at(i)).Ext.toLower()))
		{
			QString titulo = fGrl->getInfoFile(images.at(i)).NameExt;
			if (config["info_type"] == "texto_oculto")
				titulo = list_titulos.at(i).isEmpty() ? titulo : list_titulos.at(i);
			ui->cbx_img_pagina->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), titulo, images.at(i));
			num_img_pagina++;
		}
	}

	ui->lb_img_pagina_total->setText(fGrl->intToStr(index+1) +" / "+ fGrl->intToStr(num_img_pagina));

	QPixmap capa = loadPixmap(stDir + ui->cbx_img_pagina->itemData(index).toString());

	setColorBtn(visor_color);
	grdapView->setNewScene(capa.width(), capa.height());
	grdapView->setVisorColor(visor_color);
	grdapView->imgItemAddScene(0, capa, false);
	grdapView->imgItemAddScene(1, QPixmap(":/images/visor.png"), true);
	grdapView->setVisorVisible(ui->btn_visor->isChecked());

	ui->cbx_img_pagina->setCurrentIndex(index);
	ui->cbx_visor_mode->setCurrentIndex(16);
	emit on_cbx_visor_mode_activated(ui->cbx_visor_mode->currentIndex());
	getInfoImg(index);

	texto_pagina = (config["info_title"].isEmpty() ? "" : " - ")+ ui->cbx_img_pagina->currentText();

	if (directo)
	{
		this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd() +": "+ config["info_title"] + texto_pagina);
		emit on_btn_zoom_normal_clicked();
		emit on_btn_rotar_img_clicked();
	}
}

// Slots
/*void GrDap::openfileDragDrop(QString filename)
{
	if (!filename.isEmpty())
		cargarArchivo(filename);
}*/

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
	QString ext_dap   = dap_ext.join(" ");
	QString ext_img   = img_ext.join(" ");
	QString ext_comic = comic_ext.join(" ");
	QString ext_z     = z_ext.join(" ");

	QString archivo = QFileDialog::getOpenFileName(this, tr("Selecciona un archivo"), "",
				tr("Archivos soportados") +" - ("+ ext_dap +" "+ ext_img +" "+ ext_z +" "+ ext_comic +");;"+
				tr("Ruleta de protección") +" - ("+ ext_dap +");;"+
				tr("Imágenes soportadas") +" - ("+ ext_img +");;"+
				tr("Comic o manuales") +" - ("+ ext_comic +");;"+
				tr("Archivos comprimidos") +" - ("+ ext_z +");;"+
				tr("Todos los archivos") +" (*)", 0);

	if (!archivo.isEmpty() && QFile::exists(archivo))
		cargarArchivo(archivo);
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
	if (grdCfg.main_fullscreen)
		this->showFullScreen();
	else {
		if (!isInicio)
			this->showNormal();
	}
}

void GrDap::on_btn_info_toggled(bool checked)
{
	grdCfg.show_info = checked;
	ui->frame_info->setVisible(checked);
}

void GrDap::on_btn_ayuda_triggered()
{
	QDesktopServices::openUrl(QUrl("http://www.gr-lida.org/ayuda/gr-dap/"));
}

void GrDap::on_btn_acercade_triggered()
{
	frmGrDapAcercaD *GrDapAcercaD = new frmGrDapAcercaD(this);
	GrDapAcercaD->exec();
	delete GrDapAcercaD;
}

// Ruletas de la A a la D
void GrDap::on_slider_rotar_a_valueChanged(int value)
{
	grdapView->imgItemRotar(1, value);
	ui->lb_a_rotar->setText(fGrl->intToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_b_valueChanged(int value)
{
	grdapView->imgItemRotar(2, value);
	ui->lb_b_rotar->setText(fGrl->intToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_c_valueChanged(int value)
{
	grdapView->imgItemRotar(3, value);
	ui->lb_c_rotar->setText(fGrl->intToStr(value) + tr("º"));
}

void GrDap::on_slider_rotar_d_valueChanged(int value)
{
	grdapView->imgItemRotar(4, value);
	ui->lb_d_rotar->setText(fGrl->intToStr(value) + tr("º"));
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

// Visor de cartas: Alone in the Dark 2
void GrDap::on_cbx_carta_top_activated(int index)
{
	if (index > -1)
		grdapView->setPixmapItem(1, loadPixmap(stDir + ui->cbx_carta_top->itemData(index).toString()));
}

void GrDap::on_cbx_carta_bot_activated(int index)
{
	if (index > -1)
		grdapView->setPixmapItem(0, loadPixmap(stDir + ui->cbx_carta_bot->itemData(index).toString()));
}

// Visor de Imágenes
void GrDap::on_cbx_img_pagina_activated(int index)
{
	if (index > -1)
	{
		grdapView->setPixmapItem(0, loadPixmap(stDir + ui->cbx_img_pagina->itemData(index).toString()), true);

		if (ui->frame_visor->isVisible())
			emit on_cbx_visor_mode_activated(ui->cbx_visor_mode->currentIndex());

		texto_pagina = (config["info_title"].isEmpty() ? "" : " - ")+ ui->cbx_img_pagina->itemText(index);
		this->setWindowTitle("GR-dap - Dial-A-Protection v"+ fGrl->versionGrd() +": "+ config["info_title"] + texto_pagina);
		ui->lb_img_pagina_total->setText(fGrl->intToStr(index+1) +" / "+ fGrl->intToStr(num_img_pagina));
		getInfoImg(index);
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
	ui->lb_img_rotar->setText(fGrl->intToStr(value) + tr("º"));
}

void GrDap::on_btn_rotar_img_clicked()
{
	ui->slider_rotar_img->setValue(0);
}

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
	if (colorDg->exec() == QDialog::Accepted)
		emit visorColorChanged(colorDg->selectedColor());
	else
		emit visorColorChanged(color);
	delete colorDg;
}

void GrDap::on_cbx_visor_mode_activated(int index)
{
	if (index > -1 && ui->frame_visor->isVisible())
	{
		QPainter::CompositionMode mode = (QPainter::CompositionMode)ui->cbx_visor_mode->itemData(index).toInt();
		grdapView->setVisorMode(mode);
	}
}

void GrDap::visorColorChanged(QColor color)
{
	setColorBtn(color);
	grdapView->setVisorColor(color);
}
