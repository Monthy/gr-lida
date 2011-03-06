/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_img_viewer.h"

ImageViewer::ImageViewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();
	stTheme   = fGrl.ThemeGrl();

	createConnections();
	setTheme();
	createStatusBar();

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("ImageViewerState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
		ui.fitToWindowAct->setChecked( settings.value("fitToWindow", false ).toBool() );
		ui.verListImagesAct->setChecked( settings.value("verListaImagenes", false).toBool() );
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		if( settings.value("font_usar", false).toBool() )
			setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+settings.value("font_family","Tahoma").toString()+"\";font-size:"+settings.value("font_size","8").toString()+"pt;}");
	settings.endGroup();

	if( ui.fitToWindowAct->isChecked() )
		fitToWindow();

	id_imagen   = -1;
	total_img   = -1;
	scaleFactor = 1.0;
	isZip = false;

	pxmItem = new QGraphicsPixmapItem;
	pxmItem->setZValue(0);

	scene = new QGraphicsScene;
	scene->setBackgroundBrush(QBrush(QColor(163,158,144)));
	scene->addItem(pxmItem);

	ui.gvImagen->setBackgroundBrush(QBrush(QColor(163,158,144)));
	ui.gvImagen->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	ui.gvImagen->setCacheMode(QGraphicsView::CacheBackground);
	ui.gvImagen->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//QGraphicsView::BoundingRectViewportUpdate
	ui.gvImagen->setDragMode(QGraphicsView::ScrollHandDrag);
	ui.gvImagen->setScene(scene);

	ui.gvImagen->installEventFilter(this);
	ui.lwListaImagenes->installEventFilter(this);

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

ImageViewer::~ImageViewer()
{
	delete lbrotar;
	delete hsRotar;
	delete pxmItem;
	delete scene;
}

void ImageViewer::closeEvent( QCloseEvent *e )
{
	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("ImageViewerState");
		settings.setValue("maximized"  , isMaximized() );
		settings.setValue("fitToWindow", ui.fitToWindowAct->isChecked() );
		settings.setValue("verListaImagenes", ui.verListImagesAct->isChecked() );
	settings.endGroup();

	e->accept();
}

bool ImageViewer::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == ui.lwListaImagenes || obj == ui.gvImagen )
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			switch( keyEvent->key() )
			{
				case Qt::Key_Left:
				case Qt::Key_Up:
					backImagen();
				return true;
				break;
				case Qt::Key_Right:
				case Qt::Key_Down:
					nextImagen();
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

void ImageViewer::resizeEvent(QResizeEvent *event)
{
	fitToWindow();
	QMainWindow::resizeEvent(event);
}

void ImageViewer::setTheme()
{
	setWindowTitle(tr("Ver Imagen"));
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/capturas.png") );

	ui.openAct->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.printAct->setIcon( QIcon(stTheme+"img16/imprimir.png") );
	ui.exitAct->setIcon( QIcon(stTheme+"img16/cerrar.png") );
	ui.zoomInAct->setIcon( QIcon(stTheme+"img16/zoom_in.png") );
	ui.zoomOutAct->setIcon( QIcon(stTheme+"img16/zoom_out.png") );
	ui.normalSizeAct->setIcon( QIcon(stTheme+"img16/zoom.png") );
	ui.fitToWindowAct->setIcon( QIcon(stTheme+"img16/fit_to_win.png") );
	ui.backImagenAct->setIcon( QIcon(stTheme+"img16/go-back.png") );
	ui.nextImagenAct->setIcon( QIcon(stTheme+"img16/go-next.png") );
	ui.verListImagesAct->setIcon( QIcon(stTheme+"img16/categorias.png") );
}

void ImageViewer::createConnections()
{
	connect( ui.openAct       , SIGNAL(triggered()), this, SLOT(open()       ));
	connect( ui.printAct      , SIGNAL(triggered()), this, SLOT(print()      ));
	connect( ui.exitAct       , SIGNAL(triggered()), this, SLOT(close()      ));
	connect( ui.zoomInAct     , SIGNAL(triggered()), this, SLOT(zoomIn()     ));
	connect( ui.zoomOutAct    , SIGNAL(triggered()), this, SLOT(zoomOut()    ));
	connect( ui.normalSizeAct , SIGNAL(triggered()), this, SLOT(normalSize() ));
	connect( ui.fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
	connect( ui.backImagenAct , SIGNAL(triggered()), this, SLOT(backImagen() ));
	connect( ui.nextImagenAct , SIGNAL(triggered()), this, SLOT(nextImagen() ));
	connect( ui.verListImagesAct, SIGNAL(toggled(bool)), this, SLOT(setListImgVisible(bool)));

	connect( ui.lwListaImagenes, SIGNAL( itemClicked(QListWidgetItem*)), this, SLOT( on_itemClicked(QListWidgetItem*) ));
	connect( ui.lwListaImagenes, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT( on_currentItemChanged(QListWidgetItem*,QListWidgetItem*) ));


	lbrotar = new QLabel( ui.toolBar );
	lbrotar->setFrameStyle( QFrame::NoFrame );
	lbrotar->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	lbrotar->setMinimumSize(20,20);

	hsRotar = new QSlider(ui.toolBar);
	hsRotar->setMinimum(-180);
	hsRotar->setMaximum(180);
	hsRotar->setOrientation(Qt::Horizontal);

	connect(hsRotar, SIGNAL(valueChanged(int)), this, SLOT(imgRotar(int)));

	ui.toolBar->addSeparator();
	ui.toolBar->addWidget(lbrotar);
	ui.toolBar->addWidget(hsRotar);
}

void ImageViewer::setListImgVisible(bool visible)
{
	ui.lwListaImagenes->setVisible(visible);
	fitToWindow();
}

void ImageViewer::open()
{
	isZip = false;
	QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Imagen"), QDir::currentPath());
	if( !fileName.isEmpty() )
	{
		openImg( fileName );
	}
}

void ImageViewer::openZip(QString fileName)
{
	isZip = true;
	zip.abrirZip(fileName);
	open("", zip.listaZip(), isZip);
}

void ImageViewer::openImg(QString fileName)
{

	if( !fileName.isEmpty() )
	{
		setUpdatesEnabled( false );

		QFileInfo finfo(fileName);

		if( isZip )
			imagen_src = zip.loadImagen( fileName );
		else
			imagen_src.load( fileName );

		cambiaImagenScene(imagen_src);
		ui.lwListaImagenes->item(id_imagen)->setSelected(true);

		ui.printAct->setEnabled(true);
		ui.fitToWindowAct->setEnabled(true);

		setScrollBarPolicy(!ui.fitToWindowAct->isChecked());
		if( ui.fitToWindowAct->isChecked() )
			ui.gvImagen->fitInView(0,0, imagen_src.width(),imagen_src.height(),Qt::IgnoreAspectRatio);

		updateActions();

		setUpdatesEnabled( true );

		setWindowTitle(tr("Ver Imagen")+" - "+finfo.fileName() );
		lbpanel_2->setText(tr("Total:")+" "+QVariant(id_imagen+1).toString()+" "+tr("de")+" "+QVariant(total_img).toString()+" "+
				tr("Tamaño:")+" "+QVariant(imagen_src.width()).toString()+"x"+QVariant(imagen_src.height()).toString()+
				" | "+finfo.fileName() );
	}
}

void ImageViewer::open(QString fileName)
{
	isZip = false;
	QFileInfo finfo(fileName);

	imgLista << fileName;
	int indx = imgLista.indexOf(fileName, total_img);

	QListWidgetItem *Item = new QListWidgetItem(ui.lwListaImagenes);
	Item->setIcon(QIcon(stTheme+"img16/capturas.png"));
	Item->setText( finfo.baseName() );
	Item->setData(Qt::UserRole,  QVariant(indx).toString() );

	id_imagen = indx;
	total_img = imgLista.count();

	openImg(fileName);
}

void ImageViewer::open(QString fileName, QList<QString> lista, bool is_zip)
{
	isZip = is_zip;

	imgLista = lista;
	if( fileName.isEmpty() )
	{
		id_imagen = 0;
		fileName = imgLista.value(0);
	} else
		id_imagen = imgLista.indexOf(fileName);
	total_img = imgLista.count();

	ui.lwListaImagenes->clear();
	for(int i=0 ; i < total_img; i++)
	{
		QString file(imgLista.value(i));
		QFileInfo finfo(file);

		QListWidgetItem *Item = new QListWidgetItem(ui.lwListaImagenes);
		Item->setIcon(QIcon(stTheme+"img16/capturas.png"));
		Item->setText( finfo.baseName() );
		Item->setData(Qt::UserRole,  QVariant(i).toString() );

		if( i == id_imagen )
			Item->setSelected(true);
	}

	if(total_img > 1)
	{
		ui.nextImagenAct->setEnabled(true);
		ui.backImagenAct->setEnabled(true);
	}

	openImg(fileName);
}

void ImageViewer::print()
{
	Q_ASSERT(imagen_src);
	QPrintDialog dialog(&printer, this);
	if( dialog.exec() )
	{
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QSize size = imagen_src.size();
		size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(imagen_src.rect());
		painter.drawPixmap(0, 0, imagen_src);
	}
}

void ImageViewer::zoomIn()
{
	scaleFactor = 1.25;
	scaleImage(scaleFactor);
}

void ImageViewer::zoomOut()
{
	scaleFactor = 0.80;
	scaleImage(scaleFactor);
}

void ImageViewer::normalSize()
{
	scaleFactor  = 1.0;
	double newScale = scaleFactor;
	QMatrix oldMatrix = ui.gvImagen->matrix();
	ui.gvImagen->resetMatrix();
	ui.gvImagen->translate(oldMatrix.dx(), oldMatrix.dy());
	scaleImage(newScale);
}

void ImageViewer::scaleImage(double factor)
{
	ui.zoomInAct->setEnabled(factor < 3.0);
	ui.zoomOutAct->setEnabled(factor > 0.333);
	ui.gvImagen->scale(factor, factor);
}

void ImageViewer::imgRotar(int r)
{
	lbrotar->setText(tr("Rotar")+": "+QVariant(r).toString()+"  ");
	imgItemRotar(pxmItem,r);
}

void ImageViewer::imgItemRotar(QGraphicsPixmapItem *imgItem, int r)
{
	int pos_x = 0, pos_y = 0;
	pos_x = imgItem->pixmap().width() / 2;
	pos_y = imgItem->pixmap().height() / 2;
	imgItem->setTransform(QTransform().translate(pos_x, pos_y).rotate(r).translate(-pos_x, -pos_y));
}

void ImageViewer::imgItemCentrar(QGraphicsPixmapItem *imgItem)
{
	ui.gvImagen->centerOn( imgItem );
}

void ImageViewer::fitToWindow()
{
	setScrollBarPolicy(!ui.fitToWindowAct->isChecked());
	if( ui.fitToWindowAct->isChecked() )
		ui.gvImagen->fitInView(0,0, imagen_src.width(),imagen_src.height(),Qt::IgnoreAspectRatio);
	else
		normalSize();

	updateActions();
}

void ImageViewer::setScrollBarPolicy(bool activar)
{
	if( activar )
	{
		ui.gvImagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		ui.gvImagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	} else {
		ui.gvImagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui.gvImagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
}

void ImageViewer::backImagen()
{
	id_imagen--;

	if( id_imagen < 0 )
	{
		id_imagen = total_img - 1;
		openImg( imgLista[id_imagen] );
	} else
		openImg( imgLista[id_imagen] );
}

void ImageViewer::nextImagen()
{
	id_imagen++;

	if( id_imagen == total_img )
	{
		id_imagen = 0;
		openImg( imgLista[id_imagen] );
	} else
		openImg( imgLista[id_imagen] );
}

void ImageViewer::createStatusBar()
{
// StatusBar
	lbpanel_1 = new QLabel( ui.statusBar );
	lbpanel_1->setFrameStyle( QFrame::NoFrame );
	lbpanel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lbpanel_1->setPixmap( QPixmap(stTheme+"img16/capturas.png") );
	lbpanel_1->setMinimumSize(20,20);

	lbpanel_2 = new QLabel( ui.statusBar );
	lbpanel_2->setFrameStyle( QFrame::NoFrame );
	lbpanel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui.statusBar->addWidget( lbpanel_1 );
	ui.statusBar->addWidget( lbpanel_2, 1 );
}

void ImageViewer::updateActions()
{
	ui.zoomInAct->setEnabled(!ui.fitToWindowAct->isChecked());
	ui.zoomOutAct->setEnabled(!ui.fitToWindowAct->isChecked());
	ui.normalSizeAct->setEnabled(!ui.fitToWindowAct->isChecked());
}

void ImageViewer::cambiaImagenScene(QPixmap imgItem)
{
	scene_width  = imgItem.width() + 4;
	scene_height = imgItem.height() + 4;
	scene->setSceneRect(0, 0, scene_width, scene_height);
	pxmItem->setPixmap( imgItem );
	pxmItem->setPos(2,2);
	imgItemCentrar(pxmItem);
	imgRotar( hsRotar->value());
}

void ImageViewer::on_currentItemChanged(QListWidgetItem *item1,QListWidgetItem *item2)
{
	if( item1 && item2 )
		emit on_itemClicked(item1);
}

void ImageViewer::on_itemClicked(QListWidgetItem *item)
{
	if( item )
	{
		id_imagen = item->data(Qt::UserRole).toInt();
		openImg( imgLista[id_imagen] );
	}
}
