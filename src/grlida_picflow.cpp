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

#include "grlida_picflow.h"

GrlPicFlow::GrlPicFlow(QWidget* parent) : PictureFlow(parent)
{
	num_pic = 0;
	updateTimer = new QTimer;
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateImageData()));

	worker = new ImageLoader;
	connect(this, SIGNAL(centerIndexChanged(int)), this, SLOT(preload()));

	setReflectionEffect(PlainReflection);
	setFlowType( CoverFlowLike );
}

GrlPicFlow::~GrlPicFlow()
{
//	delete worker;	// Crash
	delete updateTimer;
}

void GrlPicFlow::setImagePaths(QStringList thumbs_list, QString dir, QString dir_theme)
{
	num_pic = 0;

	clear();
	imageFiles.clear();
	imagesLoaded.clear();
	imagesSetted.clear();

	const int listSize = thumbs_list.size();
	for (int i = 0; i < listSize; ++i)
		addImageSlide(thumbs_list.at(i), dir, dir_theme);

	numImagesLoaded = 0;
	setCenterIndex(0);
	worker->reset();
	preload();
}

void GrlPicFlow::addImageSlide(QString thumbs, QString dir, QString dir_theme)
{
	addSlide(img);

	if( thumbs.isEmpty() || !QFile::exists(dir + thumbs) )
		imageFiles.insert(num_pic, dir_theme +"images/juego_sin_imagen.png");
	else
		imageFiles.insert(num_pic, dir + thumbs);

	imagesLoaded.insert(num_pic, false);
	imagesSetted.insert(num_pic, false);

	++num_pic;
}

void GrlPicFlow::setPicFlowType(QString picflowtype)
{
	if( picflowtype.toLower() == "strip" )
		setFlowType( Strip );
	else if( picflowtype.toLower() == "stripoverlapped" )
		setFlowType( StripOverlapped );
	else
		setFlowType( CoverFlowLike );
}

void GrlPicFlow::preload()
{
	uint img_loaded = imagesLoaded.size();
	if(numImagesLoaded < img_loaded)
		updateTimer->start(30); //TODO comprobar rendimiento, originalmente era 70
}

void GrlPicFlow::updateImageData()
{
	// can't do anything, wait for the next possibility
	if(worker->busy())
		return;

	// set image of last one
	int idx = worker->index();
	if( idx >= 0 && !worker->result().isNull())
	{
		if(!imagesSetted[idx])
		{
			setSlide(idx, worker->result());
			imagesSetted[idx] = true;
			++numImagesLoaded;
			imagesLoaded[idx] = true;
		}
	}

	// try to load only few images on the left and right side
	// i.e. all visible ones plus some extra
#define COUNT 8
	int indexes[2*COUNT+1];
	int center = centerIndex();
	indexes[0] = center;
	for (int j = 0; j < COUNT; ++j)
	{
		indexes[j*2+1] = center+j+1;
		indexes[j*2+2] = center-j-1;
	}
	for (int c = 0; c < 2*COUNT+1; ++c)
	{
		int i = indexes[c];
		if((i >= 0) && (i < slideCount()))
			if(!imagesLoaded[i])//slide(i).isNull())
			{
				// schedule thumbnail generation
				QString fname = imageFiles[i];
//				imagesLoaded[i] = true;

				worker->generate(i, fname, slideSize());
				return;
			}
	}

	// no need to generate anything? stop polling...
	updateTimer->stop();
}

void GrlPicFlow::wheelEvent(QWheelEvent* event)
{
	if( event->delta() < 0 )
	{
		showNext();
		emit isActive(true);
	} else {
		showPrevious();
		emit isActive(true);
	}
	event->accept();
}

void GrlPicFlow::mousePressEvent(QMouseEvent* event)
{
	if(event->x() > (width()+slideSize().width())/2)
	{
		showNext();
		emit isActive(true);
	} else {
		if(event->x() < (width()-slideSize().width())/2)
		{
			showPrevious();
			emit isActive(true);
		}
	}
	//else (centered cover space)
}

void GrlPicFlow::mouseDoubleClickEvent(QMouseEvent* event)
{
	if((event->x() > (width()-slideSize().width())/2)&&(event->x() < (width()+slideSize().width())/2))
		emit selected(centerIndex());
}

//-----------------------------------------------------------------------------
//ImageLoader
//-----------------------------------------------------------------------------
static QImage loadImage(const QString& fileName)
{
	QImage image;
	bool result = image.load(fileName);

	if(!result)
		return QImage();

	return image;
}

ImageLoader::ImageLoader():
QThread(), restart(false), working(false), idx(-1)
{
}

ImageLoader::~ImageLoader()
{
	mutex.lock();
	condition.wakeOne();
	mutex.unlock();
	wait();
}

bool ImageLoader::busy() const
{
	return isRunning() ? working : false;
}

void ImageLoader::generate(int index, const QString& fileName, QSize size)
{
	mutex.lock();
	this->idx = index;
	this->fileName = fileName;
	this->size = size;
	this->img = QImage();
	mutex.unlock();

	if (!isRunning())
		start();
	else
	{
		// already running, wake up whenever ready
		restart = true;
		condition.wakeOne();
	}
}

void ImageLoader::run()
{
	for (;;)
	{
		// copy necessary data
		mutex.lock();
		this->working = true;
		QString fileName = this->fileName;
		mutex.unlock();

		QImage image = loadImage(fileName);

		// let everyone knows it is ready
		mutex.lock();
		this->working = false;
		this->img = image;
		mutex.unlock();

		// put to sleep
		mutex.lock();
		if (!this->restart)
			condition.wait(&mutex);
		restart = false;
		mutex.unlock();
	}
}

QImage ImageLoader::result()
{
	return img;
}
