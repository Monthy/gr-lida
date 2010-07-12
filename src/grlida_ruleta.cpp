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

#include "grlida_ruleta.h"

frmRuleta::frmRuleta(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();
	stTheme   = fGrl.ThemeGrl();
	setTheme();
	scaleFactor = 1;

	connect(ui.actionSalir     , SIGNAL(clicked()), this, SLOT( close()      ));
	connect(ui.actionInfo      , SIGNAL(clicked()), this, SLOT( InfoRulea()  ));
	connect(ui.actionZoomIn    , SIGNAL(clicked()), this, SLOT( zoomIn()     ));
	connect(ui.actionZoomOut   , SIGNAL(clicked()), this, SLOT( zoomOut()    ));
	connect(ui.actionSizeNormal, SIGNAL(clicked()), this, SLOT( normalSize() ));

	connect(ui.rotarA, SIGNAL(valueChanged(int)), this, SLOT(imgRotarA(int)));
	connect(ui.rotarB, SIGNAL(valueChanged(int)), this, SLOT(imgRotarB(int)));
	connect(ui.rotarC, SIGNAL(valueChanged(int)), this, SLOT(imgRotarC(int)));

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("OpcFuente");
		if( settings.value("font_usar", false).toBool() )
			setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+settings.value("font_family","Tahoma").toString()+"\";font-size:"+settings.value("font_size","8").toString()+"pt;}");
	settings.endGroup();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmRuleta::~frmRuleta()
{
	//
}

void frmRuleta::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/ruleta.png") );

	ui.actionSalir->setIcon(QIcon(stTheme+"img16/cerrar.png"));
	ui.actionZoomIn->setIcon(QIcon(stTheme+"img16/zoom_in.png"));
	ui.actionZoomOut->setIcon(QIcon(stTheme+"img16/zoom_out.png"));
	ui.actionSizeNormal->setIcon(QIcon(stTheme+"img16/zoom.png"));
}

void frmRuleta::CargarRuleta(QString filename)
{
	isZip = false;

	QFileInfo tipofile;

	tipofile.setFile(filename);

	QString ruleta_conf;
	if(tipofile.completeSuffix().toLower() == "zip")
	{
		isZip = true;
		zip.abrirZip(filename);

		QStringList listzip = zip.listaZip();
		QRegExp rx("([^\"<>]*).conf");
		ruleta_conf = listzip.value(listzip.indexOf(rx, 0));

		QFile data(stHomeDir+"/temp/conf_ruleta.ini");
		if( data.open(QFile::WriteOnly | QFile::Truncate) )
		{
			QTextStream out(&data);
		//	out.setCodec("UTF-8");
			out << zip.loadTexto(ruleta_conf);
			out.flush();
		}

		filename = stHomeDir+"/temp/conf_ruleta.ini";
	} else
		isZip = false;

	num_ruletas = 4;

	QSettings settings(filename, QSettings::IniFormat);
	config.clear();

	settings.beginGroup("info");
		config["info_title"]       = settings.value("title", "").toString();
		config["info_autor"]       = settings.value("autor", "").toString();
		config["info_url"]         = settings.value("url", "").toString();
		config["info_description"] = settings.value("description", "").toString();
	settings.endGroup();

	settings.beginGroup("lienzo");
		scene_width  = settings.value("scene_width" , "").toInt();
		scene_height = settings.value("scene_height", "").toInt();
		config["lienzo_fondo"] = settings.value("bg_image", "").toString();
		QStringList bgcolor = settings.value("bg_color", "").toStringList();
	settings.endGroup();

	settings.beginGroup("ruletas");
		config["ruleta_dir"]  = settings.value("ruleta_dir" , "").toString();
		for(int i = 0; i < num_ruletas; i++)
		{
			config.insert("ruleta_"+fGrl.IntToStr(i)        , settings.value("ruleta_"+fGrl.IntToStr(i)        ,"").toString() );
			config.insert("ruleta_"+fGrl.IntToStr(i)+"_mask", settings.value("ruleta_"+fGrl.IntToStr(i)+"_mask","").toString() );
		}
	settings.endGroup();

	ui.rotarA->setEnabled(!config["ruleta_1"].isEmpty());
	ui.rotarB->setEnabled(!config["ruleta_2"].isEmpty());
	ui.rotarC->setEnabled(!config["ruleta_3"].isEmpty());

	ui.rotarA->setVisible(!config["ruleta_1"].isEmpty());
	ui.rotarB->setVisible(!config["ruleta_2"].isEmpty());
	ui.rotarC->setVisible(!config["ruleta_3"].isEmpty());

	ui.label->setVisible(!config["ruleta_1"].isEmpty());
	ui.label_2->setVisible(!config["ruleta_2"].isEmpty());
	ui.label_3->setVisible(!config["ruleta_3"].isEmpty());

	scene = new QGraphicsScene;
	scene->setSceneRect(0, 0, scene_width, scene_height);

	setWindowTitle(config["info_title"]);

	if( bgcolor.count() > -1 )
		scene->setBackgroundBrush(QBrush(QColor(bgcolor.value(0).toInt(),bgcolor.value(0).toInt(),bgcolor.value(0).toInt())));
	else
		scene->setBackgroundBrush(QBrush(QPalette::Dark));

	if( isZip )
		stDir = "";
	else {
		QFileInfo fileInfo(filename);
		stDir = fileInfo.absolutePath();

		if( !stDir.endsWith("/") )
			stDir.append("/");
	}

	if( config["ruleta_dir"]!="" && !config["ruleta_dir"].endsWith("/") )
		config["ruleta_dir"].append("/");

	if(config["lienzo_fondo"] != "" )
	{
		if( isZip )
			imgBackground = zip.loadImagen(config["ruleta_dir"] + config["lienzo_fondo"]);
		else
			imgBackground.load( stDir + config["ruleta_dir"] + config["lienzo_fondo"] );
	} else
		imgBackground.load(":/img16/sinimg.png");

	QString str, strm;
	listCapas.clear();
	listItems.clear();
	scene->clear();
	for(int i = 0; i < num_ruletas; i++)
	{
		if( config["ruleta_"+fGrl.IntToStr(i)] == "" )
			str = ":/img16/sinimg.png";
		else
			str = stDir + config["ruleta_dir"] + config["ruleta_"+fGrl.IntToStr(i)];

		if( config["ruleta_"+fGrl.IntToStr(i)+"_mask"] == "" )
			strm = ":/img16/sinimg.png";
		else
			strm = stDir + config["ruleta_dir"] + config["ruleta_"+fGrl.IntToStr(i)+"_mask"];

		imgItemAddScene(str, strm, i, isZip);
	}

	ui.gvRuleta->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	ui.gvRuleta->setBackgroundBrush( imgBackground );
	ui.gvRuleta->setCacheMode(QGraphicsView::CacheBackground);
	ui.gvRuleta->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//QGraphicsView::BoundingRectViewportUpdate
	ui.gvRuleta->setDragMode(QGraphicsView::ScrollHandDrag);
	ui.gvRuleta->setScene(scene);
}

void frmRuleta::InfoRulea()
{
	fGrl.VentanaInfo(tr("Información"),stTheme+"img16/informacion.png",400,280,
		"<b>"+tr("Titulo del juego")+":</b> "+config["info_title"]+"<br>"
		"<b>"+tr("Autor")+":</b> "+config["info_autor"]+"<br>"
		"<b>"+tr("Url")+":</b> <a href=\""+config["info_url"]+"\">"+config["info_url"]+"</a><br>"
		"<b>"+tr("Descipción")+":</b><br>"+config["info_description"]+"<br>");
}

void frmRuleta::zoomIn()
{
	scaleFactor = 1.25;
	ui.gvRuleta->scale(scaleFactor,scaleFactor);
}

void frmRuleta::zoomOut()
{
	scaleFactor = 0.80;
	ui.gvRuleta->scale(scaleFactor,scaleFactor);
}

void frmRuleta::normalSize()
{
	scaleFactor  = 1.0;
	double newScale = scaleFactor;
	QMatrix oldMatrix = ui.gvRuleta->matrix();
	ui.gvRuleta->resetMatrix();
	ui.gvRuleta->translate(oldMatrix.dx(), oldMatrix.dy());
	ui.gvRuleta->scale(newScale, newScale);
}

void frmRuleta::imgRotarA(int r)
{
	imgItemRotar(listItems.value(1), r);
}

void frmRuleta::imgRotarB(int r)
{
	imgItemRotar(listItems.value(2), r);
}

void frmRuleta::imgRotarC(int r)
{
	imgItemRotar(listItems.value(3), r);
}

void frmRuleta::imgItemAddScene(QString capa, QString capa_mask, int id, bool is_Zip)
{
	QPixmap tmp_capa;
	QBitmap tmp_mask;
	if( is_Zip )
	{
		tmp_mask = zip.loadImagenBitmap(capa_mask);
		tmp_capa = zip.loadImagen(capa);
	} else {
		tmp_mask.load(capa_mask);
		tmp_capa.load(capa);
	}
	tmp_capa.setMask( tmp_mask );

	listCapas.insert(id, tmp_capa);
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
	imgItem->setPos(pos_x,pos_y);
}

void frmRuleta::imgItemRotar(QGraphicsPixmapItem *imgItem, int r)
{
	int pos_x = 0, pos_y = 0;
//	pos_x = scene->width()/2;
//	pos_y = scene->height()/2;
	pos_x = imgItem->pixmap().width() / 2;
	pos_y = imgItem->pixmap().height() / 2;

	imgItem->setTransform(QTransform().translate(pos_x, pos_y).rotate(r).translate(-pos_x, -pos_y));
}
