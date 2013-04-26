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

#include "grlida_img_viewer.h"
#include "ui_img_viewer.h"

frmImgViewer::frmImgViewer(stGrlCfg m_cfg, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::frmImgViewer)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home = fGrl->GRlidaHomePath();

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmImgViewer::~frmImgViewer()
{
	delete pxmItem;
	delete scene;
	delete ui;
}

void frmImgViewer::closeEvent(QCloseEvent *event)
{
	QSettings settings( grlDir.Home +"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("ImageViewerState");
		settings.setValue("maximized"  , isMaximized() );
		settings.setValue("fitToWindow", ui->fitToWindow->isChecked() );
		settings.setValue("verListaImagenes", ui->verListImages->isChecked() );
	settings.endGroup();

	event->accept();
}

bool frmImgViewer::eventFilter(QObject *object, QEvent *event)
{
	if( object == ui->lwListaImagenes || object == ui->gvImagen )
	{
		if( event->type() == QEvent::KeyPress)
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
		return QMainWindow::eventFilter(object, event); // pass the event on to the parent class
}

void frmImgViewer::resizeEvent(QResizeEvent *event)
{
	emit on_fitToWindow_triggered( grlCfg.img_fitToWindow );
	QMainWindow::resizeEvent(event);
}

void frmImgViewer::cargarConfig()
{
	id_imagen   = -1;
	total_img   = -1;
	scaleFactor = 1.0;
	isZip       = false;

// StatusBar
	lbpanel_1 = new QLabel( ui->statusBar );
	lbpanel_1->setFrameStyle( QFrame::NoFrame );
	lbpanel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	lbpanel_1->setPixmap( QPixmap(fGrl->Theme() +"img16/capturas.png") );
	lbpanel_1->setMinimumSize(20,20);

	lbpanel_2 = new QLabel( ui->statusBar );
	lbpanel_2->setFrameStyle( QFrame::NoFrame );
	lbpanel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui->statusBar->addWidget( lbpanel_1 );
	ui->statusBar->addWidget( lbpanel_2, 1 );

	if( grlCfg.img_maximized )
		setWindowState( windowState() | Qt::WindowMaximized );
	ui->fitToWindow->setChecked( grlCfg.img_fitToWindow );
	ui->verListImages->setChecked( grlCfg.img_verListaImagenes );
	emit on_verListImages_triggered( grlCfg.img_verListaImagenes );

	emit on_fitToWindow_triggered( grlCfg.img_fitToWindow );

	pxmItem = new QGraphicsPixmapItem;
	pxmItem->setZValue(0);

	scene = new QGraphicsScene;
	scene->setBackgroundBrush(QBrush(QColor(163, 158, 144)));
	scene->addItem(pxmItem);

	ui->gvImagen->setBackgroundBrush(QBrush(QColor(163,158,144)));
	ui->gvImagen->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	ui->gvImagen->setCacheMode(QGraphicsView::CacheBackground);
	ui->gvImagen->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//QGraphicsView::BoundingRectViewportUpdate
	ui->gvImagen->setDragMode(QGraphicsView::ScrollHandDrag);
	ui->gvImagen->setScene(scene);

	ui->gvImagen->installEventFilter(this);
	ui->lwListaImagenes->installEventFilter(this);

	ui->toolBar->addSeparator();
	ui->toolBar->addWidget( ui->frameRotar );
	ui->toolBar->addAction( ui->centrar );
}

void frmImgViewer::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );

	ui->open->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->print->setIcon( QIcon(fGrl->Theme() +"img16/imprimir.png") );
	ui->exit->setIcon( QIcon(fGrl->Theme() +"img16/cerrar.png") );
	ui->zoomIn->setIcon( QIcon(fGrl->Theme() +"img16/zoom_in.png") );
	ui->zoomOut->setIcon( QIcon(fGrl->Theme() +"img16/zoom_out.png") );
	ui->normalSize->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->fitToWindow->setIcon( QIcon(fGrl->Theme() +"img16/fit_to_win.png") );
	ui->backImagen->setIcon( QIcon(fGrl->Theme() +"img16/go-back.png") );
	ui->nextImagen->setIcon( QIcon(fGrl->Theme() +"img16/go-next.png") );
	ui->verListImages->setIcon( QIcon(fGrl->Theme() +"img16/categorias.png") );
}

void frmImgViewer::open(QString fileName)
{
	isZip = false;

	imgLista << fileName;
	int indx = imgLista.indexOf(fileName, total_img);

	QFileInfo f_info(fileName);
	QListWidgetItem *Item = new QListWidgetItem(ui->lwListaImagenes);
	Item->setIcon(QIcon(fGrl->Theme() +"img16/capturas.png"));
	Item->setText( f_info.baseName() );
	Item->setData(Qt::UserRole,  QVariant(indx).toString() );

	id_imagen = indx;
	total_img = imgLista.count();

	showImg(id_imagen);
}

void frmImgViewer::openZip(QString fileName)
{
	isZip = true;
	zip.abrirZip(fileName);
	open("", zip.listaZip(), isZip);
}

void frmImgViewer::open(QString fileName, QStringList lista, bool is_zip)
{
	isZip = is_zip;

	imgLista = lista;
	if( fileName.isEmpty() )
		id_imagen = 0;
	else
		id_imagen = imgLista.indexOf(fileName);
	total_img = imgLista.count();

	ui->lwListaImagenes->clear();
	for(int i=0 ; i < total_img; ++i)
	{
		QFileInfo f_info( imgLista.at(i) );
		QListWidgetItem *item = new QListWidgetItem(ui->lwListaImagenes);
		item->setIcon(QIcon(fGrl->Theme() +"img16/capturas.png"));
		item->setText( f_info.baseName() );
		item->setData(Qt::UserRole, fGrl->IntToStr(i) );

		if( i == id_imagen )
			item->setSelected(true);
	}

	if(total_img > 1)
	{
		ui->nextImagen->setEnabled(true);
		ui->backImagen->setEnabled(true);
	}

	showImg(id_imagen);
}

void frmImgViewer::showImg(int index)
{
	if( index > -1 )
	{
		setUpdatesEnabled( false );

		QFileInfo f_info( imgLista.at(index) );

		if( isZip )
			imagen_src = zip.loadImagen( imgLista.at(index) );
		else
			imagen_src.load( imgLista.at(index) );

		scene_width  = imagen_src.width() + 4;
		scene_height = imagen_src.height() + 4;
		scene->setSceneRect(0, 0, scene_width, scene_height);
		pxmItem->setPixmap( imagen_src );
		pxmItem->setPos(2, 2);

		ui->gvImagen->centerOn( pxmItem );
		emit on_hsRotar_valueChanged(0);

		ui->lwListaImagenes->item(id_imagen)->setSelected(true);

		ui->print->setEnabled(true);
		ui->fitToWindow->setEnabled(true);

		emit on_fitToWindow_triggered( grlCfg.img_fitToWindow );

		setUpdatesEnabled( true );

		lbpanel_2->setText(tr("Total") +": "+ fGrl->IntToStr(id_imagen+1) +" "+ tr("de") +" "+ fGrl->IntToStr(total_img) +" "+
				tr("Tamaño") +": "+ fGrl->IntToStr(imagen_src.width()) +"x"+ fGrl->IntToStr(imagen_src.height()) +" | "+ f_info.fileName() );
	}
}

void frmImgViewer::nextImagen()
{
	id_imagen++;

	if( id_imagen == total_img )
	{
		id_imagen = 0;
		showImg( id_imagen );
	} else
		showImg( id_imagen );
}

void frmImgViewer::backImagen()
{
	id_imagen--;

	if( id_imagen < 0 )
	{
		id_imagen = total_img - 1;
		showImg( id_imagen );
	} else
		showImg( id_imagen );
}

void frmImgViewer::imgItemRotar(int radio)
{
	ui->lb_radio->setText(fGrl->IntToStr(radio) + tr("º"));

	int pos_x = 0, pos_y = 0;
	pos_x = pxmItem->pixmap().width() / 2;
	pos_y = pxmItem->pixmap().height() / 2;

	pxmItem->setTransform(QTransform().translate(pos_x, pos_y).rotate(radio).translate(-pos_x, -pos_y));
}

void frmImgViewer::updateActions()
{
	if( grlCfg.img_fitToWindow )
	{
		ui->zoomIn->setEnabled(false);
		ui->zoomOut->setEnabled(false);
		ui->gvImagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui->gvImagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	} else {
		ui->zoomIn->setEnabled(scaleFactor < 3.0);
		ui->zoomOut->setEnabled(scaleFactor > 0.333);
		ui->gvImagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		ui->gvImagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	ui->normalSize->setEnabled(!grlCfg.img_fitToWindow);
}

void frmImgViewer::on_exit_triggered()
{
	close();
}

void frmImgViewer::on_open_triggered()
{
	isZip = false;
	QString fileName = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlDir.Home, grlDir.Home,
											tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)", 0, false);
	if( !fileName.isEmpty() )
		open( fileName );
}

void frmImgViewer::on_print_triggered()
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

void frmImgViewer::on_zoomIn_triggered()
{
	scaleFactor = 1.25;
	ui->gvImagen->scale(scaleFactor, scaleFactor);
	scaleFactor = ui->gvImagen->transform().m11();
	updateActions();
}

void frmImgViewer::on_zoomOut_triggered()
{
	scaleFactor = 0.80;
	ui->gvImagen->scale(scaleFactor, scaleFactor);
	scaleFactor = ui->gvImagen->transform().m11();
	updateActions();
}

void frmImgViewer::on_normalSize_triggered()
{
	scaleFactor = 1.0;
	double newScale = scaleFactor;
	QMatrix oldMatrix = ui->gvImagen->matrix();
	ui->gvImagen->resetMatrix();
	ui->gvImagen->translate(oldMatrix.dx(), oldMatrix.dy());
	ui->gvImagen->scale(newScale, newScale);
	updateActions();
}

void frmImgViewer::on_fitToWindow_triggered(bool checked)
{
	if( ui->fitToWindow->isChecked() )
		ui->gvImagen->fitInView(0, 0, imagen_src.width(), imagen_src.height(), Qt::IgnoreAspectRatio);
	else {
		if( scaleFactor == 1.0 )
			emit on_normalSize_triggered();
	}
	grlCfg.img_fitToWindow = checked;
	updateActions();
}

void frmImgViewer::on_backImagen_triggered()
{
	backImagen();
}

void frmImgViewer::on_nextImagen_triggered()
{
	nextImagen();
}

void frmImgViewer::on_verListImages_triggered(bool checked)
{
	ui->lwListaImagenes->setVisible(checked);
}

void frmImgViewer::on_centrar_triggered()
{
	imgItemRotar(0);
	ui->hsRotar->setValue(0);
}

void frmImgViewer::on_lwListaImagenes_currentRowChanged(int currentRow)
{
	if( currentRow > -1 )
		showImg( ui->lwListaImagenes->item(currentRow)->data(Qt::UserRole).toInt() );
}

void frmImgViewer::on_hsRotar_valueChanged(int value)
{
	imgItemRotar(value);
}
