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

#ifndef QTZIP_H
#define QTZIP_H

#include <QtCore>
#include <QtGui>

#include <QPixmap>
#include <QBitmap>

#include "osdab-zip/zip.h"
#include "osdab-zip/unzip.h"

class QtZip
{
public:
	QtZip();
	~QtZip();

	bool abrirZip(const QString file, const QString pwd = "");
	bool extractZip(const QString file, const QString out, const QString pwd = "");

	QStringList listaZip();
	void listaZipTreeWidget(QTreeWidget *myTreeWidget);
	void listaZipListWidget(QListWidget *myListWidget);

	QString getCommentZip(){return comentarioZip;}
	QString loadTexto(QString filename);
	QPixmap loadImagen(QString filename);
	QBitmap loadImagenBitmap(QString filename);

private:
	UnZip::ErrorCode ec_uz;
	UnZip uz;

	bool isZipOpen;
	QString comentarioZip;
};

#endif /*QTZIP_H*/
