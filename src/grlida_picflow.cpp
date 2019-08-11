/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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
	updateTimer = new QTimer;
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateImageData()));

	worker = new ImageLoader;
	connect(this, SIGNAL(centerIndexChanged(int)), this, SLOT(preload()));
	connect(this, SIGNAL(centerIndexChangedSilent(int)), this, SLOT(preload()));

	setReflectionEffect(PlainReflection);
	setFlowType(CoverFlowLike);
}

GrlPicFlow::~GrlPicFlow()
{
	worker->terminate();

	delete worker;
	delete updateTimer;
//	worker->deleteLater();
}

void GrlPicFlow::setImagePaths(QList<stListPicFlow> thumbs_list, int index)
{
	clear();

	imageFiles.clear();
	imageFiles = thumbs_list;
	imagesLoaded.clear();
	imagesLoaded.fill(false, imageFiles.size());
	imagesSetted.clear();
	imagesSetted.fill(false, imageFiles.size());

	// populate with empty images
	QImage img; // TODO remove
	const int listSize = imageFiles.size();
	for (int i = 0; i < listSize; ++i)
	{
		addSlide(img, imageFiles.at(i).caption);
	}
	numImagesLoaded = 0;

	setCenterIndex(index);
	worker->reset();
	preload();
}

void GrlPicFlow::addImageSlide(QString thumb, QString caption)
{
// populate with empty images
	QImage img; //TODO remove
	addSlide(img, caption);

	int listSize = imageFiles.count();
	stListPicFlow info_picflow;
	info_picflow.caption = caption;
	info_picflow.path    = thumb;

	imageFiles.insert(listSize, info_picflow);
	imagesLoaded.insert(listSize, false);
	imagesSetted.insert(listSize, false);

//	setCenterIndex(listSize);
	worker->reset();
	preload();
}

void GrlPicFlow::setImageSlide(int index, QString thumb, QString caption)
{
	imageFiles[index].caption = caption;
	imageFiles[index].path = thumb;
	setSlide(index, QPixmap(thumb), caption);
}

void GrlPicFlow::clearImageSlide()
{
	clear();
	imageFiles.clear();
	imagesLoaded.clear();
	imagesSetted.clear();
	numImagesLoaded = 0;

	worker->reset();
	preload();
}

void GrlPicFlow::setPicFlowType(QString picflowtype)
{
	if (picflowtype.toLower() == "strip")
		setFlowType(Strip);
	else if (picflowtype.toLower() == "stripoverlapped")
		setFlowType(StripOverlapped);
	else
		setFlowType(CoverFlowLike);
}

void GrlPicFlow::preload()
{
	uint img_loaded = imagesLoaded.size();
	if (numImagesLoaded < img_loaded)
		updateTimer->start(30); // TODO comprobar rendimiento, originalmente era 70
}

void GrlPicFlow::updateImageData()
{
	// can't do anything, wait for the next possibility
	if (worker->busy())
		return;

	// set image of last one
	int idx = worker->index();
	if (idx >= 0 && !worker->result().isNull())
	{
		if (!imagesSetted[idx])
		{
			setSlide(idx, worker->result(), imageFiles.at(idx).caption);
			imagesSetted[idx] = true;
			numImagesLoaded++;
			imagesLoaded[idx]=true;
		}
	}

	// try to load only few images on the left and right side
	// i.e. all visible ones plus some extra
#define COUNT 8
	int indexes[2*COUNT+1];
	int center = centerIndex();
	indexes[0] = center;
	for (int j = 0; j < COUNT; j++)
	{
		indexes[j * 2 + 1] = center + j + 1;
		indexes[j * 2 + 2] = center - j - 1;
	}
	for (int c = 0; c < 2 * COUNT + 1; c++)
	{
		int i = indexes[c];
		if ((i >= 0) && (i < slideCount()))
			if (!imagesLoaded[i])//slide(i).isNull())
			{
				// schedule thumbnail generation
				QString fname = imageFiles[i].path;

				worker->generate(i, fname, slideSize());
				return;
			}
	}

	// no need to generate anything? stop polling...
	updateTimer->stop();
}

void GrlPicFlow::removeSlide(int cover)
{
	worker->lock();

	worker->reset();

	imageFiles.removeAt(cover);
	if (imagesLoaded[cover])
		numImagesLoaded--;
	imagesLoaded.remove(cover);
	imagesSetted.remove(cover);

	PictureFlow::removeSlide(cover);
	worker->unlock();

	preload();
}

void GrlPicFlow::resortCovers(QList<int> newOrder)
{
	worker->lock();
	worker->reset();

	PictureFlow::resortCovers(newOrder);

	QList<stListPicFlow> imageFilesNew;
	QVector<bool> imagesLoadedNew;
	QVector<bool> imagesSettedNew;

	const int listOrderSize = newOrder.size();
	for (int i = 0; i < listOrderSize; ++i)
	{
		imageFilesNew << imageFiles.at(newOrder.at(i));
		imagesLoadedNew << imagesLoaded.at(newOrder.at(i));
		imagesSettedNew << imagesSetted.at(newOrder.at(i));
	}

	imageFiles = imageFilesNew;
	imagesLoaded = imagesLoadedNew;
	imagesSetted = imagesSettedNew;

	worker->unlock();
}

void GrlPicFlow::wheelEvent(QWheelEvent* event)
{
	if (event->delta() < 0)
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
	if (event->x() > (width()+slideSize().width())/2)
	{
		showNext();
		emit isActive(true);
	} else {
		if (event->x() < (width()-slideSize().width())/2)
		{
			showPrevious();
			emit isActive(true);
		}
	}
	//else (centered cover space)
}

void GrlPicFlow::mouseDoubleClickEvent(QMouseEvent* event)
{
	if ((event->x() > (width() - slideSize().width()) / 2) && (event->x() < (width() + slideSize().width()) / 2))
		emit selected(centerIndex());
}

//-----------------------------------------------------------------------------
//ImageLoader
//-----------------------------------------------------------------------------
static QImage loadImage(const QString& fileName)
{
	QImage image;
	bool result = image.load(fileName);

	if (!result)
		return QImage();

	return image;
}

ImageLoader::ImageLoader() :
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
	else {
		// already running, wake up whenever ready
		restart = true;
		condition.wakeOne();
	}
}

void ImageLoader::lock()
{
	mutex.lock();
}

void ImageLoader::unlock()
{
	mutex.unlock();
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
