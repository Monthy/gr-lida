/*
 *
 * AnimatedTextBrowser
 *
 * Thanks qtcentre.org and jpn (http://www.qtcentre.org/wiki/index.php?title=Animation_in_QTextEdit)
 * modified by: Pedro A. Garcia Rosado Aka Monthy (http://www.gr-lida.org)
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

#ifndef ANIMATEDTEXTBROWSER_H
#define ANIMATEDTEXTBROWSER_H

#include <QTextBrowser>
#include <QMovie>
#include <QHash>
#include <QList>

class AnimatedTextBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	AnimatedTextBrowser(QWidget *parent = 0) : QTextBrowser(parent)
	{
		setOpenExternalLinks(false);
		setOpenLinks(false);
		setReadOnly(true);
	}

	~AnimatedTextBrowser()
	{
		QList<QMovie*>listMovies = urls.keys();
		for (QList<QMovie*>::iterator it_movies = listMovies.begin(); it_movies != listMovies.end(); ++it_movies)
			delete *it_movies;
		urls.clear();
	}

	void addAnimation(const QUrl &url, const QString &fileName)
	{
		QList<QUrl> listUrls = urls.values();
		if (!listUrls.contains(url))
		{
			QMovie *movie = new QMovie(this);
			movie->setFileName(fileName);
			movie->setCacheMode(QMovie::CacheAll);
			urls.insert(movie, url);
			connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate()));
			movie->start();
		}
	}

private:
	QHash<QMovie*, QUrl> urls;

private slots:
	void animate()
	{
		if (QMovie *movie = qobject_cast<QMovie*>(sender()))
		{
			document()->addResource(QTextDocument::ImageResource, urls.value(movie), movie->currentPixmap());
			setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // causes reload
		}
	}
};

#endif // ANIMATEDTEXTBROWSER_H
