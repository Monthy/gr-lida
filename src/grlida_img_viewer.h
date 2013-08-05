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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QPrinter>
#include <QGraphicsRectItem>

#include "funciones.h"
#include "qtzip.h"

namespace Ui {
	class frmImgViewer;
}

class frmImgViewer : public QMainWindow
{
	Q_OBJECT

public:
	explicit frmImgViewer(stGrlCfg m_cfg, QWidget *parent = 0);
	~frmImgViewer();

	void open(QString fileName);
	void openZip(QString fileName);
	void open(QString fileName, QStringList lista, bool is_zip = false);
	void showImg(int index);

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *object, QEvent *event);

private:
	Ui::frmImgViewer *ui;

	Funciones *fGrl;
	QtZip zip;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QGraphicsScene *scene;
	QGraphicsPixmapItem *pxmItem;

	bool isZip;
	int scene_width, scene_height;
	double scaleFactor;
	int total_img, id_imagen;

	QPixmap imagen_src;
	QStringList imgLista;

	QPrinter printer;

// StatusBar
	QLabel *lbpanel_1;
	QLabel *lbpanel_2;

	void cargarConfig();
	void setTheme();

	void nextImagen();
	void backImagen();
	void imgItemRotar(int radio = 0);
	void updateActions();

private slots:
	void on_exit_triggered();
	void on_open_triggered();
	void on_print_triggered();
	void on_zoomIn_triggered();
	void on_zoomOut_triggered();
	void on_normalSize_triggered();
	void on_fitToWindow_triggered(bool checked);
	void on_backImagen_triggered();
	void on_nextImagen_triggered();
	void on_verListImages_triggered(bool checked);
	void on_centrar_triggered();

	void on_lwListaImagenes_currentRowChanged(int currentRow);
	void on_hsRotar_valueChanged(int value);

};

#endif
