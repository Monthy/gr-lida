/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014-2016 Pedro A. Garcia Rosado Aka Monthy
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

#include "grdapview.h"

GrDapView::GrDapView(QWidget *parent)
	: QGraphicsView(parent)
{
	scene_space  = 4;
	zoomSlider   = 250;
	rotateSlider = 0;
	visor_height = 76;
	visor_mode   = QPainter::CompositionMode_Darken;
	visor_color  = QColor(255, 0, 0, 255);
	visor_img.load(":/img16/sinimg.png");

	scene = new QGraphicsScene(this);
	setAlignment(Qt::AlignCenter);
//	setAcceptDrops(true);
	setAutoFillBackground(true);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setBackgroundBrush(Qt::darkGray);
	setBackgroundRole(QPalette::Dark);
	setCacheMode(QGraphicsView::CacheBackground);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//SmartViewportUpdate FullViewportUpdate
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setFrameShape(QFrame::NoFrame);
	setNewScene(400, 400, true);
}

GrDapView::~GrDapView()
{
	delete scene;
}

#ifndef QT_NO_WHEELEVENT
void GrDapView::wheelEvent(QWheelEvent *event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->delta() > 0)
			setZoomIn(6);
		else
			setZoomOut(6);
		event->accept();
	} else
		QGraphicsView::wheelEvent(event);
}
#endif
/*
void GrDapView::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->urls().count() > -1)
		emit openfileDragDrop(event->mimeData()->urls().at(0).toLocalFile());
	event->acceptProposedAction();
}

void GrDapView::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}
*/

void GrDapView::clearScene()
{
	scene->clear();
}

void GrDapView::setNewScene(int scene_width, int scene_height, bool ogl)
{
	listItems.clear();
	setOpenGlScene(ogl);
	scene->clear();
	scene->setSceneRect(0, 0, scene_width + scene_space, scene_height + scene_space);
	setScene(scene);
	setAcceptDrops(true);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
	verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
}

void GrDapView::setOpenGlScene(bool usar_ogl)
{
#ifndef QT_NO_OPENGL
	setViewport(usar_ogl ? new QGLWidget(QGLFormat(QGL::SampleBuffers), this) : new QWidget);
//QGL::DoubleBuffer
#endif
}

void GrDapView::setupMatrix()
{
	qreal scale = qPow(qreal(2), (zoomSlider - 250) / qreal(50));

	QMatrix matrix;
	matrix.scale(scale, scale);
	matrix.rotate(rotateSlider);
	setMatrix(matrix);
}

void GrDapView::rotarValue(int value)
{
	rotateSlider = value;
	emit setupMatrix();
}

void GrDapView::zoomValue(int value)
{
	zoomSlider = value;
	emit setupMatrix();
}

void GrDapView::setZoomIn(int level)
{
	zoomSlider = zoomSlider + level;
	emit setupMatrix();
}

void GrDapView::setZoomOut(int level)
{
	zoomSlider = zoomSlider - level;
	emit setupMatrix();
}

/*void GrDapView::setZoomNormal()
{
	zoomSlider   = 250;
	rotateSlider = 0;
	emit setupMatrix();
	ensureVisible(QRectF(0, 0, 0, 0));
}*/

void GrDapView::imgItemAddScene(int id, QPixmap capa, bool isVisor, bool resizeScene)
{
	if (resizeScene)
		scene->setSceneRect(0, 0, capa.width() + scene_space, capa.height() + scene_space);

	GrDapImgItem *imagen = new GrDapImgItem(capa);
	imagen->setPixmap(capa);
	imagen->setZValue(id);
	imagen->setVisible(true);
	imagen->setIsVisor(isVisor);
	listItems.insert(id, imagen);
	imgItemCentrar(id);
	imgItemRotar(id, 0);

	if (isVisor)
	{
		imagen->setFlag(QGraphicsItem::ItemIsMovable, true);
		visor_img = capa.toImage();
		connect(imagen, SIGNAL(posChanged(QPointF,bool)), this, SLOT(posVisorChanged(QPointF,bool)));
		emit posVisorChanged(imagen->pos(), isVisor);
	}

	scene->addItem(imagen);

	if (resizeScene)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
		verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
	}
}

void GrDapView::imgItemCentrar(int id)
{
	if (listItems.contains(listItems.value(id)))
	{
		qreal pos_x = 0, pos_y = 0;
		pos_x = (scene->width() / 2) - (listItems.value(id)->pixmap().width() / 2);
		pos_y = (scene->height() / 2) - (listItems.value(id)->pixmap().height() / 2);
		listItems.value(id)->setPos(pos_x, pos_y);
	}
}

void GrDapView::imgItemRotar(int id, int r)
{
	if (listItems.contains(listItems.value(id)))
	{
		qreal pos_x = 0, pos_y = 0;
		pos_x = listItems.value(id)->pixmap().width() / 2;
		pos_y = listItems.value(id)->pixmap().height() / 2;
		listItems.value(id)->setTransform(QTransform().translate(pos_x, pos_y).rotate(r).translate(-pos_x, -pos_y));
	}
}

void GrDapView::setPixmapItem(int id, QPixmap capa, bool resizeScene)
{
	if (listItems.contains(listItems.value(id)))
	{
		if (resizeScene)
			scene->setSceneRect(0, 0, capa.width() + scene_space, capa.height() + scene_space);
		listItems.value(id)->setPixmap(capa);
		imgItemCentrar(id);
//		imgItemRotar(id, 0);
		horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
		verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
	}
}

void GrDapView::setVisorMode(QPainter::CompositionMode mode)
{
	visor_mode = mode;
	emit posVisorChanged(QPointF(img_pos_x, img_pos_y), true);
}

void GrDapView::setVisorColor(QColor color)
{
	visor_color = color;
	emit posVisorChanged(QPointF(img_pos_x, img_pos_y), true);
}

void GrDapView::setVisorVisible(bool v_visible)
{
	if (listItems.contains(listItems.value(1)))
		listItems.value(1)->setVisible(v_visible);
}

void GrDapView::posVisorChanged(QPointF v_pos, bool visor)
{
	if (visor && listItems.contains(listItems.value(1)))
	{
		img_pos_x = v_pos.x();
		img_pos_y = v_pos.y();
		qreal sep_scene_x = (scene->width() - listItems.value(0)->pixmap().width()) / 2;
		qreal sep_scene_y = (scene->height() - listItems.value(0)->pixmap().height()) / 2;
		QImage img_src = listItems.value(0)->pixmap().toImage();
		QImage img_final(QSize(visor_img.width(), visor_img.height()), QImage::Format_ARGB32_Premultiplied);

		QPainter painter(&img_final);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(img_final.rect(), Qt::transparent);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.fillRect(0, 0, visor_img.width(), visor_height, Qt::white);
		painter.drawImage(0, 0, img_src, static_cast<int>(img_pos_x - sep_scene_x), static_cast<int>(img_pos_y - sep_scene_y), visor_img.width(), visor_height);
		painter.setCompositionMode(visor_mode);
		painter.fillRect(0, 0, visor_img.width(), visor_height, visor_color);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawImage(0, 0, visor_img);
		painter.end();

		listItems.value(1)->setPixmap(QPixmap::fromImage(img_final));
	}
}
