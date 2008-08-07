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

#include "grlida_img_viewer.h"

ImageViewer::ImageViewer( QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	stHomeDir = fGrl.GRlidaHomePath();
	stTheme = fGrl.ThemeGrl();

	imageLabel = new QLabel;
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	setCentralWidget(scrollArea);

	createActions();
	createMenus();
	createToolBars();

	setWindowTitle(tr("Ver Imagen"));
	resize(500, 400);

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("ImageViewerState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
		fitToWindowAct->setChecked( settings.value("fitToWindow", false ).toBool() );
	settings.endGroup();

	if( fitToWindowAct->isChecked() )
		fitToWindow();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

ImageViewer::~ImageViewer(){}

void ImageViewer::closeEvent( QCloseEvent *e )
{
	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("ImageViewerState");
		settings.setValue("maximized", isMaximized() );
		settings.setValue("fitToWindow", fitToWindowAct->isChecked() );
	settings.endGroup();

	e->accept();
}

void ImageViewer::open()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir Imagen"), QDir::currentPath());
	if( !fileName.isEmpty() )
	{
		QImage image( fileName );
		if( image.isNull() )
		{
			QMessageBox::information(this, tr("Ver Imagen"), tr("No se ha podido abrir: %1.").arg(fileName));
			return;
		}
		imageLabel->setPixmap( QPixmap::fromImage(image) );
		scaleFactor = 1.0;

		printAct->setEnabled(true);
		fitToWindowAct->setEnabled(true);
		updateActions();

		if( !fitToWindowAct->isChecked() )
			imageLabel->adjustSize();
	}
}

void ImageViewer::open(QString fileName)
{
	if( !fileName.isEmpty() )
	{
		QImage image(fileName);
		if (image.isNull())
		{
			QMessageBox::information(this, tr("Ver Imagen"), tr("No se ha podido abrir: %1.").arg(fileName));
			return;
		}
		imageLabel->setPixmap( QPixmap::fromImage(image) );
		scaleFactor = 1.0;

		printAct->setEnabled(true);
		fitToWindowAct->setEnabled(true);
		updateActions();

		if( !fitToWindowAct->isChecked() )
			imageLabel->adjustSize();
	}
}

void ImageViewer::print()
{
	Q_ASSERT(imageLabel->pixmap());
	QPrintDialog dialog(&printer, this);
	if( dialog.exec() )
	{
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QSize size = imageLabel->pixmap()->size();
		size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(imageLabel->pixmap()->rect());
		painter.drawPixmap(0, 0, *imageLabel->pixmap());
	}
}

void ImageViewer::zoomIn()
{
	scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
	scaleImage(0.8);
}

void ImageViewer::normalSize()
{
	imageLabel->adjustSize();
	scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
	bool fitToWindow = fitToWindowAct->isChecked();
	scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow)
	{
		normalSize();
	}
	updateActions();
}

void ImageViewer::createActions()
{
	openAct = new QAction(tr("&Abrir..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setIcon( QIcon(stTheme + "img16/carpeta_1.png") );
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	printAct = new QAction(tr("Im&primir..."), this);
	printAct->setShortcut(tr("Ctrl+P"));
	printAct->setIcon( QIcon(stTheme + "img16/imprimir.png") );
	printAct->setEnabled(false);
	connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

	exitAct = new QAction(tr("&Salir"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setIcon( QIcon(stTheme + "img16/cerrar.png") );
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setIcon( QIcon(stTheme + "img16/zoom_in.png") );
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

	zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setIcon( QIcon(stTheme + "img16/zoom_out.png") );
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

	normalSizeAct = new QAction(tr("Tamaño &Normal"), this);
	normalSizeAct->setShortcut(tr("Ctrl+N"));
	normalSizeAct->setIcon( QIcon(stTheme + "img16/zoom.png") );
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	fitToWindowAct = new QAction(tr("&Fit to Window"), this);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	fitToWindowAct->setIcon( QIcon(stTheme + "img16/fit_to_win.png") );
	connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
}

void ImageViewer::createMenus()
{
	fileMenu = new QMenu(tr("&Archivo"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	viewMenu = new QMenu(tr("&Ver"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(fitToWindowAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
}

void ImageViewer::createToolBars()
{
	imgToolBar = addToolBar(tr("ImageViewer"));
	imgToolBar->setIconSize( QSize(16,16) );
	imgToolBar->addAction(exitAct);
	imgToolBar->addSeparator();
	imgToolBar->addAction(openAct);
	imgToolBar->addAction(printAct);
	imgToolBar->addSeparator();
	imgToolBar->addAction(zoomInAct);
	imgToolBar->addAction(zoomOutAct);
	imgToolBar->addAction(normalSizeAct);
	imgToolBar->addSeparator();
	imgToolBar->addAction(fitToWindowAct);
}

void ImageViewer::updateActions()
{
	zoomInAct->setEnabled(!fitToWindowAct->isChecked());
	zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
	normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
	Q_ASSERT(imageLabel->pixmap());
	scaleFactor *= factor;
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

	adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(scrollArea->verticalScrollBar(), factor);

	zoomInAct->setEnabled(scaleFactor < 3.0);
	zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}
