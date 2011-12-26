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

#ifndef GRLIDA_RULETA_H
#define GRLIDA_RULETA_H

#include <QMainWindow>
#include <QGraphicsRectItem>

#include "funciones.h"
#include "qtzip.h"
#include "ui_ruleta.h"

class frmRuleta : public QMainWindow
{
    Q_OBJECT

public:
	frmRuleta(QWidget *parent = 0, Qt::WFlags flags = 0);
	~frmRuleta();

	Ui::RuletaClass ui;

	void CargarRuleta(QString filename);

private:
	Funciones fGrl;
	QtZip zip;

	QString stHomeDir, stTheme, stDir;
	bool isZip;

	QGraphicsScene *scene;

	int scene_width, scene_height;
	int num_ruletas;
	double scaleFactor;

	QHash<QString, QString> config;
	QList<QGraphicsPixmapItem *> listItems;
	QList<QPixmap> listCapas;
	QPixmap imgBackground;

// StatusBar
	QLabel *lbpanel_1;
	QLabel *lbpanel_2;

	void setTheme();
	void imgItemAddScene(QString capa, QString capa_mask, int id, bool is_Zip);
	void imgItemCentrar(QGraphicsScene *scena, QGraphicsPixmapItem *imgItem);
	void imgItemRotar(QGraphicsPixmapItem *imgItem, int r = 0);

private slots:
	void InfoRulea();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void imgRotarA(int r = 0);
	void imgRotarB(int r = 0);
	void imgRotarC(int r = 0);

};

#endif // GRLIDA_RULETA_H
