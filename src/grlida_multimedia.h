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

#ifndef GRLIDA_MULTIMEDIA_H
#define GRLIDA_MULTIMEDIA_H

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>
#include <QtCore/QBasicTimer>
#include <QtGui/QAction>

#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/effectwidget.h>
#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/videowidget.h>
#include <phonon/volumeslider.h>

#include "funciones.h"

QT_BEGIN_NAMESPACE
class QStringList;
class QListWidget;
class QPushButton;
class QLabel;
class QSlider;
class QTextEdit;
class QMenu;
class Ui_multimedia_setup;
QT_END_NAMESPACE

class frmMultiMedia;

class MediaVideoWidget : public Phonon::VideoWidget
{
	Q_OBJECT

public:
	MediaVideoWidget(frmMultiMedia *player, QWidget *parent = 0);

public slots:
// Over-riding non-virtual Phonon::VideoWidget slot
	void setFullScreen(bool);

signals:
	void fullScreenChanged(bool);

protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	bool event(QEvent *e);
	void timerEvent(QTimerEvent *e);

private:
	frmMultiMedia *m_player;
	QBasicTimer m_timer;
	QAction m_action;
};

class frmMultiMedia : public QWidget
{
	Q_OBJECT

public:
	frmMultiMedia(QWidget *parent = 0);
	~frmMultiMedia();

	void open(int id, QStringList lista);
	void setMediaId(int id = -1);
	void addItemList(QString fileName);

	void initVideoWindow();
	void initSettingsDialog();
	void setVolume(qreal volume);
	void setSmallScreen(bool smallScreen);

protected:
	void closeEvent(QCloseEvent *e);

public slots:
	void openFile();
	void rewind();
	void forward();
	void updateInfo();
	void updateTime();
	void finished();
	void playPause();
	void mpStop();
	void scaleChanged(QAction *);
	void aspectChanged(QAction *);

private slots:
	void setAspect(int);
	void setScale(int);
	void setSaturation(int);
	void setContrast(int);
	void setHue(int);
	void setBrightness(int);
	void stateChanged(Phonon::State newstate, Phonon::State oldstate);
	void effectChanged();
	void showSettingsDialog();
	void showContextMenu(const QPoint& point);
	void bufferStatus(int percent);
	void openUrl();
	void openRamFile();
	void configureEffect();
	void hasVideoChanged(bool);

	void lwMedia_DoubleClicked(QListWidgetItem *item);

private:
	bool playPauseForDialog();

	Funciones *fGrl;
	stGrlDir grlDir;

	Phonon::SeekSlider *slider;
	Phonon::VolumeSlider *volume;
	Phonon::Effect *nextEffect;
	Phonon::MediaObject *m_MediaObject;
	Phonon::AudioOutput *m_AudioOutput;
	Phonon::Path m_audioOutputPath;

	int id_media, total_media;
	QStringList lista_media, formatsVideo, formatsMusic;
	QListWidget *lwMedia;

	QIcon playIcon;
	QIcon pauseIcon;

	QPushButton *btnPlayPausa;
	QPushButton *btnAnterior;
	QPushButton *btnSiguiente;

	QLabel *lb_status;
	QLabel *lb_tiempo;
	QLabel *lb_buffer;
	QLabel *info;

	QSlider *m_hueSlider;
	QSlider *m_satSlider;
	QSlider *m_contSlider;

	QDialog *settingsDialog;
	Ui_multimedia_setup *ui;

	QMenu *fileMenu;
	QAction *m_fullScreenAction;
	QWidget m_videoWindow;

	MediaVideoWidget *m_videoWidget;

	bool m_smallScreen;

};

#endif //GRLIDA_MULTIMEDIA_H
