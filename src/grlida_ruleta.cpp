/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_ruleta.h"
#include "ui_ruleta.h"

frmRuleta::frmRuleta(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmRuleta)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home = fGrl->GRlidaHomePath();
	grlDir.Temp = grlDir.Home +"temp/";

	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmRuleta::~frmRuleta()
{
	delete fGrl;
	delete ui;
}

void frmRuleta::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/ruleta.png") );

	ui->actionSalir->setIcon( QIcon(fGrl->Theme() +"img16/cerrar.png") );
	ui->actionInfo->setIcon( QIcon(fGrl->Theme() +"img16/informacion.png") );
	ui->actionZoomIn->setIcon( QIcon(fGrl->Theme() +"img16/zoom_in.png") );
	ui->actionZoomOut->setIcon( QIcon(fGrl->Theme() +"img16/zoom_out.png") );
	ui->actionSizeNormal->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
}

void frmRuleta::cargarRuleta(QString filename)
{
	scaleFactor = 1;
	isZip = false;

	stFileInfo f_info = fGrl->getInfoFile(filename);
	if( f_info.Ext.toLower() == ".zip" )
	{
		isZip = true;
		zip.abrirZip(filename);

		QStringList listzip = zip.listaZip();
		QRegExp rx("([^\"<>]*).conf");
		QString ruleta_conf = listzip.value(listzip.indexOf(rx, 0));

		QFile data(grlDir.Temp +"conf_ruleta.ini");
		if( data.open(QFile::WriteOnly | QFile::Truncate) )
		{
			QTextStream out(&data);
			out << zip.loadTexto(ruleta_conf);
			out.flush();
		}

		filename = grlDir.Temp +"conf_ruleta.ini";
	} else
		isZip = false;

	num_ruletas = 4;

	QSettings settings(filename, QSettings::IniFormat);
	config.clear();

	emit on_actionInfo_clicked(false);
//	ui->frameInfo->setFixedHeight(216);

	settings.beginGroup("info");
		config["info_title"]       = settings.value("title", "").toString();
		config["info_autor"]       = settings.value("autor", "").toString();
		config["info_url"]         = settings.value("url"  , "").toString();
		config["info_description"] = settings.value("description", "").toString();
		config["info_type"]        = settings.value("type", "").toString();
	settings.endGroup();

	ui->txtInfo->setHtml("<b>"+ tr("Título del juego") +":</b> "+ config["info_title"] +"<br>"
								"<b>"+ tr("Autor") +":</b> "+ config["info_autor"] +"<br>"
								"<b>"+ tr("Url") +":</b> <a href=\""+ config["info_url"] +"\">"+ config["info_url"] +"</a><br>"
								"<b>"+ tr("Descipción") +":</b><br>"+ config["info_description"] +"<br>");

	settings.beginGroup("lienzo");
		scene_width  = settings.value("scene_width" , 500).toInt();
		scene_height = settings.value("scene_height", 500).toInt();
		config["lienzo_fondo"] = settings.value("bg_image", "").toString();
		QStringList bgcolor    = settings.value("bg_color", "").toStringList();
	settings.endGroup();

	if( config["info_type"] == "ruletas" || config["info_type"].isEmpty() )
	{
		settings.beginGroup("ruletas");
			config["img_dir"] = settings.value("ruleta_dir", "").toString();
			for(int i = 0; i < num_ruletas; ++i)
			{
				config.insert("ruleta_"+ fGrl->IntToStr(i), settings.value("ruleta_"+ fGrl->IntToStr(i), "").toString() );
				config.insert("ruleta_"+ fGrl->IntToStr(i) +"_mask", settings.value("ruleta_"+ fGrl->IntToStr(i) +"_mask", "").toString() );
			}
		settings.endGroup();
		ui->ruleta_Cartas->setVisible(false);
	}

	if( config["info_type"] == "cartas_aitd2" )
	{
		ui->cbxCartaTop->clear();
		ui->cbxCartaBot->clear();
		settings.beginGroup("cartas_aitd2");
			config["img_dir"] = settings.value("carta_dir", "").toString();
			for(int i = 0; i < 12; ++i)
			{
				ui->cbxCartaTop->addItem(QIcon(":/img16/sinimg.png"), settings.value("titulo_"+ fGrl->IntToStr(i), "Carta "+ fGrl->IntToStr(i) ).toString(), settings.value("carta_"+ fGrl->IntToStr(i), "").toString());
				ui->cbxCartaBot->addItem(QIcon(":/img16/sinimg.png"), settings.value("titulo_"+ fGrl->IntToStr(i), "Carta "+ fGrl->IntToStr(i) ).toString(), settings.value("carta_"+ fGrl->IntToStr(i), "").toString());
			}
		settings.endGroup();
		ui->ruleta_Cartas->setVisible(true);
	}

	ui->ruleta_A->setVisible(!config["ruleta_1"].isEmpty());
	ui->ruleta_B->setVisible(!config["ruleta_2"].isEmpty());
	ui->ruleta_C->setVisible(!config["ruleta_3"].isEmpty());
	ui->ruleta_D->setVisible(!config["ruleta_4"].isEmpty());

	scene = new QGraphicsScene;
	scene->setSceneRect(0, 0, scene_width, scene_height);

	setWindowTitle(tr("Protección") +": "+ config["info_title"]);
	ui->lb_info_titulo->setText(config["info_title"]);

	if( bgcolor.count() > -1 && bgcolor.count() == 3)
		scene->setBackgroundBrush(QBrush(QColor(bgcolor.at(0).toInt(), bgcolor.at(1).toInt(), bgcolor.at(2).toInt())));
	else
		scene->setBackgroundBrush(QBrush(QPalette::Dark));

	stDir = "";
	if( !isZip )
	{
		f_info = fGrl->getInfoFile(filename);
		stDir = f_info.Path;

		if( !stDir.endsWith("/") )
			stDir.append("/");
	}

	if( !config["img_dir"].isEmpty() && !config["img_dir"].endsWith("/") )
		config["img_dir"].append("/");

	if( !config["lienzo_fondo"].isEmpty() )
	{
		if( isZip )
			imgBackground = zip.loadImagen(config["img_dir"] + config["lienzo_fondo"]);
		else
			imgBackground.load( stDir + config["img_dir"] + config["lienzo_fondo"] );
	} else
		imgBackground.load(":/img16/sinimg.png");

	QString str, strm;
	listCapas.clear();
	listItems.clear();
	scene->clear();

	if( config["info_type"] == "ruletas" || config["info_type"].isEmpty() )
	{
		for(int i = 0; i < num_ruletas; ++i)
		{
			if( !config["ruleta_"+ fGrl->IntToStr(i)].isEmpty() )
				str = stDir + config["img_dir"] + config["ruleta_"+ fGrl->IntToStr(i)];
			else
				str = "";

			if( !config["ruleta_"+ fGrl->IntToStr(i) +"_mask"].isEmpty() )
				strm = stDir + config["img_dir"] + config["ruleta_"+ fGrl->IntToStr(i) +"_mask"];
			else
				strm = "";

			imgItemAddScene(str, strm, i);
		}
	}

	if( config["info_type"] == "cartas_aitd2" )
	{
		for(int i = 0; i < 2; ++i)
		{
			if( !config["carta_"+ fGrl->IntToStr(i)].isEmpty() )
				str = stDir + config["img_dir"] + config["carta_"+ fGrl->IntToStr(i)];
			else
				str = "";

			imgItemAddScene(str, "", i);
		}
		emit on_cbxCartaTop_activated(0);
		emit on_cbxCartaBot_activated(0);
	}

	ui->gvRuleta->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	ui->gvRuleta->setBackgroundBrush( imgBackground );
	ui->gvRuleta->setCacheMode(QGraphicsView::CacheBackground);
	ui->gvRuleta->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//QGraphicsView::BoundingRectViewportUpdate
	ui->gvRuleta->setDragMode(QGraphicsView::ScrollHandDrag);
	ui->gvRuleta->setScene(scene);
}

void frmRuleta::imgItemAddScene(QString f_capa, QString f_capa_mask, int id)
{
	QPixmap capa;
	QBitmap mask;

	if( f_capa.isEmpty() )
		capa.load(":/img16/sinimg.png");
	else {
		if( isZip )
			capa = zip.loadImagen(f_capa);
		else
			capa.load(f_capa);
	}

	if( f_capa_mask.isEmpty() )
	{
		mask.load(":/img16/sinimg.png");
		capa.setMask( mask.scaled(capa.width(), capa.height()) );
	} else {
		if( isZip )
			mask = zip.loadImagenBitmap(f_capa_mask);
		else
			mask.load(f_capa_mask);
		capa.setMask( mask );
	}

	listCapas.insert(id, capa);
	listItems.insert(id, new QGraphicsPixmapItem(listCapas.value(id)));
	listItems.value(id)->setZValue(id);

	scene->addItem(listItems.value(id));

	imgItemCentrar(scene, listItems.value(id));
	imgItemRotar(listItems.value(id), 0);
}

void frmRuleta::imgItemCentrar(QGraphicsScene *scena, QGraphicsPixmapItem *imgItem)
{
	qreal pos_x = 0, pos_y = 0;

	pos_x = (scena->width()/2)-(imgItem->pixmap().width()/2);
	pos_y = (scena->height()/2)-(imgItem->pixmap().height()/2);
	imgItem->setPos(pos_x, pos_y);
}

void frmRuleta::imgItemRotar(QGraphicsPixmapItem *imgItem, int r)
{
	int pos_x = 0, pos_y = 0;
	pos_x = imgItem->pixmap().width() / 2;
	pos_y = imgItem->pixmap().height() / 2;

	imgItem->setTransform(QTransform().translate(pos_x, pos_y).rotate(r).translate(-pos_x, -pos_y));
}

void frmRuleta::on_actionSalir_clicked()
{
	close();
}

void frmRuleta::on_actionInfo_clicked(bool checked)
{
	ui->frameInfo->setVisible(checked);
}

void frmRuleta::on_btnCerrarInfo_clicked(bool checked)
{
	ui->actionInfo->setChecked(checked);
	ui->frameInfo->setVisible(checked);
}

void frmRuleta::on_actionZoomIn_clicked()
{
	scaleFactor = 1.25;
	ui->gvRuleta->scale(scaleFactor, scaleFactor);
}

void frmRuleta::on_actionZoomOut_clicked()
{
	scaleFactor = 0.80;
	ui->gvRuleta->scale(scaleFactor, scaleFactor);
}

void frmRuleta::on_actionSizeNormal_clicked()
{
	scaleFactor = 1.0;
	double newScale = scaleFactor;
	QMatrix oldMatrix = ui->gvRuleta->matrix();
	ui->gvRuleta->resetMatrix();
	ui->gvRuleta->translate(oldMatrix.dx(), oldMatrix.dy());
	ui->gvRuleta->scale(newScale, newScale);
}

void frmRuleta::on_slider_A_valueChanged(int value)
{
	imgItemRotar(listItems.value(1), value);
}

void frmRuleta::on_slider_B_valueChanged(int value)
{
	imgItemRotar(listItems.value(2), value);
}

void frmRuleta::on_slider_C_valueChanged(int value)
{
	imgItemRotar(listItems.value(3), value);
}

void frmRuleta::on_slider_D_valueChanged(int value)
{
	imgItemRotar(listItems.value(4), value);
}

void frmRuleta::on_cbxCartaTop_activated(int index)
{
	if( index > -1 )
	{
		QPixmap capa;
		if( isZip )
			capa = zip.loadImagen(stDir + config["img_dir"] + ui->cbxCartaTop->itemData(index).toString());
		else
			capa.load(stDir + config["img_dir"] + ui->cbxCartaTop->itemData(index).toString());

		listItems.value(1)->setPixmap(capa);
		imgItemCentrar(scene, listItems.value(1));
		imgItemRotar(listItems.value(1), 0);
	}
}

void frmRuleta::on_cbxCartaBot_activated(int index)
{
	if( index > -1 )
	{
		QPixmap capa;
		if( isZip )
			capa = zip.loadImagen(stDir + config["img_dir"] + ui->cbxCartaBot->itemData(index).toString());
		else
			capa.load(stDir + config["img_dir"] + ui->cbxCartaBot->itemData(index).toString());

		listItems.value(0)->setPixmap(capa);
		imgItemCentrar(scene, listItems.value(0));
		imgItemRotar(listItems.value(0), 0);
	}
}
