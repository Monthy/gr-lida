/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
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

#ifndef GRLIDA_PICFLOW_H
#define GRLIDA_PICFLOW_H

#define PICTUREFLOW_QT4 1

#include <QtCore>
#include <QThread>
#include <QImage>
#include <QImageReader>
#include <QMutex>
#include <QWaitCondition>
#include <QString>
#include <QWheelEvent>

#include "pictureflow-qt/pictureflow.h"

class ImageLoader;
class GrlPicFlow : public PictureFlow
{
	Q_OBJECT

public:
	GrlPicFlow(QWidget* parent = 0);
	~GrlPicFlow();

	void setImagePaths(QStringList thumbs_list, QString dir, QString dir_theme);
	void addImageSlide(QString thumbs, QString dir, QString dir_theme);
	void setPicFlowType(QString picflowtype);

protected:
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

signals:
	void selected(unsigned int centerIndex);
	void isActive(bool checked);

private:
	QStringList imageFiles;
	QVector<bool> imagesLoaded;
	QVector<bool> imagesSetted;
	uint numImagesLoaded, num_pic;
	QTimer *updateTimer;
	ImageLoader *worker;
	QImage img;

private slots:
	void preload();
	void updateImageData();

};

//-----------------------------------------------------------------------------
// Source code of ImageLoader class was modified from http://code.google.com/p/photoflow/
//------------------------------------------------------------------------------
class ImageLoader : public QThread
{
public:
  ImageLoader();
  ~ImageLoader();
  // returns FALSE if worker is still busy and can't take the task
  bool busy() const;
  void generate(int index, const QString& fileName, QSize size);
  void reset(){idx = -1;}
  int index() const { return idx; }
  QImage result();

protected:
  void run();

private:
  QMutex mutex;
  QWaitCondition condition;

  bool restart;
  bool working;
  int idx;
  QString fileName;
  QSize size;
  QImage img;

};

#endif // GRLIDA_PICFLOW_H
