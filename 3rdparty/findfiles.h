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

#ifndef FINDFILES_H
#define FINDFILES_H

#include <QThread>
#include <QDirIterator>

class findFiles : public QThread
{
	Q_OBJECT

public:
	explicit findFiles(QObject *parent = 0) : QThread(parent) {}
	QStringList files() const {return fileList;}

private:
	QDirIterator *iterator;
	QStringList fileList;
	QString f_dir, f_texto;
	bool stopped;

	void run()
	{
		while (!stopped && iterator->hasNext())
		{
			if (!iterator->fileInfo().isDir())
			{
				QString file_name = iterator->fileName().toLower();
				if (f_texto.isEmpty() || file_name == f_texto.toLower())
				{
					file_name = iterator->filePath();
					fileList << file_name;
					emit fileFind(file_name);
				}
			}
			iterator->next();
		}
	}

signals:
	void fileFind(QString filename);

public slots:
	void find(QString dir, QString f_txt = "")
	{
		QStringList filter;
		filter << "*.*";
		find(dir, f_txt, filter);
	}

	void find(QString dir, QString f_txt, QStringList filter)
	{
		fileList.clear();
		f_dir    = dir;
		f_texto  = f_txt;
		stopped  = false;
		iterator = new QDirIterator(f_dir, filter, QDir::Files | QDir::Readable | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
		start();
	}

	void stop()
	{
		stopped = true;
		wait(5);
		terminate();

		if (isFinished())
			delete iterator;
	}

	void pause(bool state)
	{
		stopped = state;
		if (!state)
			start();
	}

};

#endif // FINDFILES_H
