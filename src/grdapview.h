/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection and
 * GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRDAPVIEW_H
#define GRDAPVIEW_H

#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QScrollBar>

#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#include <QGLWidget>
#endif

#include <qmath.h>

// INICIO GrDapImgItem ------------------------------------------------------------------------------------------
class GrDapImgItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	GrDapImgItem(QPixmap &pixmap, QGraphicsItem *parent = 0) :
		QGraphicsPixmapItem(pixmap, parent){is_visor = false;}

	void setIsVisor(bool m_visor = false){is_visor = m_visor;}
	bool isVisor(){return is_visor;}

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		m_pos = pos();
		emit posChanged(m_pos, isVisor());
		QGraphicsPixmapItem::mouseMoveEvent(event);
	}

signals:
	void posChanged(QPointF v_pos, bool visor);

private:
	QPointF m_pos;
	bool is_visor;

};
// FIN GrDapImgItem ---------------------------------------------------------------------------------------------

// INICIO GrDapView ---------------------------------------------------------------------------------------------
class GrDapView : public QGraphicsView
{
	Q_OBJECT

public:
	GrDapView(QWidget *parent = 0);
	~GrDapView();

	void setNewScene(int scene_width = 400, int scene_height = 400, bool ogl = true);
	void setSpaceScene(int space = 4){scene_space = space;}
	void setOpenGlScene(bool usar_ogl = true);

	void imgItemAddScene(int id, QPixmap capa, bool isVisor = false, bool resizeScene = false);
	void imgItemCentrar(int id);
	void imgItemRotar(int id, int r);

	void setPixmapItem(int id, QPixmap capa, bool resizeScene = false);

	void setVisorMode(QPainter::CompositionMode mode = QPainter::CompositionMode_Darken);
	void setVisorHeight(int v_height = 76){visor_height = v_height;}
	void setVisorColor(QColor color);
	void setVisorVisible(bool v_visible = true);
	QColor getVisorColor(){return visor_color;}

protected:
#ifndef QT_NO_WHEELEVENT
	void wheelEvent(QWheelEvent *event);
#endif
/*	void dropEvent(QDropEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);*/

private:
	QGraphicsScene *scene;

	QList<GrDapImgItem *> listItems;
	int scene_space, visor_height, img_pos_x, img_pos_y, zoomSlider, rotateSlider;
	QPainter::CompositionMode visor_mode;
	QColor visor_color;
	QImage visor_img;

private slots:
	void setupMatrix();
	void posVisorChanged(QPointF v_pos, bool visor);

public slots:
	void rotarValue(int value = 0);

	void zoomValue(int value = 250);
	void setZoomIn(int level = 1);
	void setZoomOut(int level = 1);
//	void setZoomNormal();

signals:
	void openfileDragDrop(QString filename);

};
// FIN GrDapView ------------------------------------------------------------------------------------------------

#endif // GRDAPVIEW_H
