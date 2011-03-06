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

#ifndef GRLIDA_MULTIMEDIA_H
#define GRLIDA_MULTIMEDIA_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/videowidget.h>
#include <phonon/volumeslider.h>

#include "funciones.h"
#include "ui_multimedia.h"

class frmMultiMedia : public QMainWindow
{
    Q_OBJECT

public:
	frmMultiMedia(QWidget *parent = 0, Qt::WFlags flags = 0);
	~frmMultiMedia();

	Ui::MultiMediaClass ui;

	void AbrirArchivo(QString archivo);
	void AbrirArchivos(QStringList archivos);
	void setItemID(int id = -1);
protected:
	void closeEvent(QCloseEvent *e);

private:
	Funciones fGrl;

	Phonon::MediaObject *m_MediaObject;
	Phonon::AudioOutput *m_AudioOutput;
	Phonon::VideoWidget *m_videoWidget;
	Phonon::Path m_audioOutputPath;
	Phonon::SeekSlider *seekSlider;

	long duration;
	int total_source, id_source;
	QList<Phonon::MediaSource> sources_list;

	QString stTheme;

// Barra de Heramientas Volumen
	QIcon icon_volume;
	QIcon icon_muted;
	QIcon icon_play;
	QIcon icon_pausa;
	QLabel *lb_tiempo;
	QLabel *lb_buffer;
	QSlider *volume;

// StatusBar
	QLabel *lbpanel_1;
	QLabel *lbpanel_2;

	void setTheme();
	void createConnections();
	void createToolBars();
	void createStatusBar();

public slots:
	void mpPausa();

private slots:
	void mpPlayPausa();

	void mpPlay();
	void mpStop();
	void mpAnterior();
	void mpRebobinarAtras();
	void mpRebobinarAdelante();
	void mpSiguiente();

	void mpAbrir();

	void mpAleatorio();

	void setPlayPause(bool state);

	void setVolume(int volume);
	void setMute(bool estado);

	void mpUpdateInfo();
	void mpUpdateTime();
	void mpFinished();
	void mpStateChanged(Phonon::State newstate, Phonon::State oldstate);
	void mpBufferStatus(int percent);

};

#endif // GRLIDA_MULTIMEDIA_H
