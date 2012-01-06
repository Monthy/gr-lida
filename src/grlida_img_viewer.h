/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QPrinter>
#include <QGraphicsRectItem>

#include "funciones.h"
#include "qtzip.h"
#include "ui_image_viewer.h"

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
	ImageViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ImageViewer();

	Ui::ImageViewerClass ui;

	void openZip(QString fileName);
	void openImg(QString fileName);
	void open(QString fileName);
	void open(QString fileName, QList<QString> lista, bool is_zip = false);

protected:
	void closeEvent( QCloseEvent *e );
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Funciones fGrl;
	QtZip zip;

	QString stHomeDir, stTheme;
	bool isZip;

	QGraphicsScene *scene;
	QGraphicsPixmapItem *pxmItem;

	int scene_width, scene_height;
	double scaleFactor;
	int total_img, id_imagen;

	QPixmap imagen_src;
	QList<QString> imgLista;

	QPrinter printer;

// StatusBar
	QLabel *lbpanel_1;
	QLabel *lbpanel_2;

	QLabel *lbrotar;
	QSlider *hsRotar;

	void createConnections();
	void setTheme();
	void createStatusBar();
	void updateActions();
	void scaleImage(double factor);
	void imgItemCentrar(QGraphicsPixmapItem *imgItem);
	void imgItemRotar(QGraphicsPixmapItem *imgItem, int r = 0);
	void cambiaImagenScene(QPixmap imgItem);
	void setScrollBarPolicy(bool activar = true);

private slots:
	void on_currentItemChanged(QListWidgetItem *item1,QListWidgetItem *item2);
	void on_itemClicked(QListWidgetItem *item);

	void open();
	void print();

	void zoomIn();
	void zoomOut();
	void normalSize();
	void imgRotar(int r = 0);
	void fitToWindow();
	void nextImagen();
	void backImagen();
	void setListImgVisible(bool visible);
};

#endif
