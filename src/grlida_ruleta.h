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

#ifndef GRLIDA_RULETA_H
#define GRLIDA_RULETA_H

#include <QDialog>
#include <QGraphicsRectItem>

#include "funciones.h"
#include "qtzip.h"

namespace Ui {
    class frmRuleta;
}

class frmRuleta : public QDialog
{
	Q_OBJECT

public:
	explicit frmRuleta(stGrlCfg m_cfg, QWidget *parent = 0);
	~frmRuleta();

	void cargarRuleta(QString filename);

private:
	Ui::frmRuleta *ui;

	Funciones *fGrl;
	QtZip zip;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QGraphicsScene *scene;

	bool isZip;
	int scene_width, scene_height;
	int num_ruletas;
	double scaleFactor;
	QString stDir;

	QHash<QString, QString> config;
	QList<QGraphicsPixmapItem *> listItems;
	QList<QPixmap> listCapas;
	QPixmap imgBackground;

	void setTheme();

	void imgItemAddScene(QString f_capa, QString f_capa_mask, int id);
	void imgItemCentrar(QGraphicsScene *scena, QGraphicsPixmapItem *imgItem);
	void imgItemRotar(QGraphicsPixmapItem *imgItem, int r = 0);

private slots:
	void on_actionSalir_clicked();
	void on_actionInfo_clicked(bool checked);
	void on_btnCerrarInfo_clicked(bool checked);
	void on_actionZoomIn_clicked();
	void on_actionZoomOut_clicked();
	void on_actionSizeNormal_clicked();

	void on_slider_A_valueChanged(int value);
	void on_slider_B_valueChanged(int value);
	void on_slider_C_valueChanged(int value);
	void on_slider_D_valueChanged(int value);
	void on_cbxCartaTop_activated(int index);
	void on_cbxCartaBot_activated(int index);

};

#endif // GRLIDA_RULETA_H
