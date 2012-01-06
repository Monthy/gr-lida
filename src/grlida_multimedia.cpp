/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_multimedia.h"

class myVideoWidget : public Phonon::VideoWidget
{
public:
	myVideoWidget(frmMultiMedia *player, QWidget *parent = 0) :
		Phonon::VideoWidget(parent), m_player(player), m_action(this)
	{
		m_action.setCheckable(true);
		m_action.setChecked(false);
		m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
		m_action.setShortcutContext(Qt::WindowShortcut);
		connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
		addAction(&m_action);
	}

protected:
	void mouseDoubleClickEvent(QMouseEvent *e)
	{
		Phonon::VideoWidget::mouseDoubleClickEvent(e);
		setFullScreen(!isFullScreen());
	}

	void keyPressEvent(QKeyEvent *e)
	{
		if (e->key() == Qt::Key_Space && !e->modifiers())
		{
			m_player->mpPausa();
			e->accept();
			return;
		} else if (e->key() == Qt::Key_Escape && !e->modifiers()) {
			setFullScreen(false);
			e->accept();
			return;
		}
		Phonon::VideoWidget::keyPressEvent(e);
	}

	bool event(QEvent *e)
	{
		switch(e->type())
		{
		case QEvent::Close:
			e->ignore();
			return true;
		case QEvent::MouseMove:
			unsetCursor();
			//fall through
		case QEvent::WindowStateChange:
		{
			//we just update the state of the checkbox, in case it wasn't already
			m_action.setChecked(windowState() & Qt::WindowFullScreen);
			const Qt::WindowFlags flags = m_player->windowFlags();
			if (windowState() & Qt::WindowFullScreen)
			{
				m_timer.start(1000, this);
			} else {
				m_timer.stop();
				unsetCursor();
			}
		}
		break;
		default:
			//
		break;
		}
		return Phonon::VideoWidget::event(e);
	}

	void timerEvent(QTimerEvent *e)
	{
		if (e->timerId() == m_timer.timerId())
		{
			//let's store the cursor shape
			setCursor(Qt::BlankCursor);
		}
		Phonon::VideoWidget::timerEvent(e);
	}

private:
	frmMultiMedia *m_player;
	QBasicTimer m_timer;
	QAction m_action;
};

frmMultiMedia::frmMultiMedia(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	stTheme = fGrl.ThemeGrl();
	setTheme();

	m_MediaObject = new Phonon::MediaObject(this);
	m_AudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	m_videoWidget = new myVideoWidget(this);

	seekSlider    = new Phonon::SeekSlider(this);
	seekSlider->setMediaObject(m_MediaObject);

	total_source = -1;
	id_source = -1;

	QVBoxLayout *videoLayout = new QVBoxLayout();
	videoLayout->setContentsMargins(0, 0, 0, 0);
	videoLayout->addWidget(m_videoWidget);
	ui.m_videoWindow->setLayout(videoLayout);

	createToolBars();
	createStatusBar();
	createConnections();

	m_MediaObject->setTickInterval(250);

	m_audioOutputPath = Phonon::createPath(m_MediaObject, m_AudioOutput);
	Phonon::createPath(m_MediaObject, m_videoWidget);

	resize(minimumSizeHint());
/*
// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
*/
}

frmMultiMedia::~frmMultiMedia()
{

}

void frmMultiMedia::closeEvent( QCloseEvent *e )
{
	delete m_MediaObject;
	delete m_AudioOutput;
	delete m_videoWidget;

	e->accept();
}

void frmMultiMedia::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/multimedia.png") );

	icon_play.addFile(stTheme+"img16/mp_play.png");
	icon_pausa.addFile(stTheme+"img16/mp_pausa.png");
	icon_volume.addFile(stTheme+"img16/img_audio_winmidi.png");
	icon_muted.addFile(stTheme+"img16/img_audio_null.png");

	ui.actionPlayPausa->setIcon(icon_play);
	ui.actionMute->setIcon(icon_volume);
	ui.actionStop->setIcon(QIcon(stTheme+"img16/mp_stop.png"));
	ui.actionAnterior->setIcon(QIcon(stTheme+"img16/mp_anterior.png"));
	ui.actionRebobinarAtras->setIcon(QIcon(stTheme+"img16/mp_rebobinar_atras.png"));
	ui.actionRebobinarAdelante->setIcon(QIcon(stTheme+"img16/mp_rebobinar_adelante.png"));
	ui.actionSiguiente->setIcon(QIcon(stTheme+"img16/mp_siguiente.png"));
	ui.actionAbrir->setIcon(QIcon(stTheme+"img16/mp_eject.png"));
	ui.actionAleatorio->setIcon(QIcon(stTheme+"img16/mp_aleatorio.png"));
}

void frmMultiMedia::createConnections()
{
	connect( ui.actionPlayPausa, SIGNAL( triggered(bool) ), this, SLOT( mpPlayPausa() ));
	connect( ui.actionStop     , SIGNAL( triggered(bool) ), this, SLOT( mpStop()      ));
	connect( ui.actionAnterior , SIGNAL( triggered(bool) ), this, SLOT( mpAnterior()  ));
	connect( ui.actionRebobinarAtras   , SIGNAL( triggered(bool) ), this, SLOT( mpRebobinarAtras()    ));
	connect( ui.actionRebobinarAdelante, SIGNAL( triggered(bool) ), this, SLOT( mpRebobinarAdelante() ));
	connect( ui.actionSiguiente, SIGNAL( triggered(bool) ), this, SLOT( mpSiguiente() ));
	connect( ui.actionAbrir    , SIGNAL( triggered(bool) ), this, SLOT( mpAbrir()     ));
	connect( ui.actionAleatorio, SIGNAL( triggered(bool) ), this, SLOT( mpAleatorio() ));

	connect( volume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
	connect( ui.actionMute, SIGNAL(toggled(bool)), this, SLOT(setMute(bool)));

	connect( m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(mpUpdateInfo()));
	connect( m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(mpUpdateTime()));
	connect( m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(mpUpdateTime()));
	connect( m_MediaObject, SIGNAL(finished()), this, SLOT(mpFinished()));
	connect( m_MediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mpStateChanged(Phonon::State, Phonon::State)));
	connect( m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(mpBufferStatus(int)));
}

void frmMultiMedia::createToolBars()
{
	QWidget *w_separador = new QWidget(ui.toolBar_Botones);
	w_separador->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	w_separador->setEnabled(false);

	volume = new QSlider(Qt::Horizontal, ui.toolBar_Botones);
	volume->setRange(0, 100);
	volume->setValue(100);
	volume->setMinimumWidth(40);
	volume->setFixedWidth(100);

	ui.toolBar_Botones->addWidget(w_separador);
	ui.toolBar_Botones->addAction(ui.actionMute);
	ui.toolBar_Botones->addWidget(volume);

	lb_tiempo = new QLabel(ui.toolBar_BarraProgreso);
	lb_buffer = new QLabel(ui.toolBar_BarraProgreso);

	lb_tiempo->setText("");
	lb_buffer->setText("");

	ui.toolBar_BarraProgreso->addWidget(seekSlider);
	ui.toolBar_BarraProgreso->addWidget(lb_tiempo);
	ui.toolBar_BarraProgreso->addWidget(lb_buffer);
}

void frmMultiMedia::createStatusBar()
{
	lbpanel_1 = new QLabel( ui.statusbar );
	lbpanel_1->setFrameStyle( QFrame::NoFrame );
	lbpanel_1->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	lbpanel_2 = new QLabel( ui.statusbar );
	lbpanel_2->setFrameStyle( QFrame::NoFrame );
	lbpanel_2->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	ui.statusbar->addWidget( lbpanel_1 );
	ui.statusbar->addWidget( lbpanel_2, 1 );
}

void frmMultiMedia::mpPlayPausa()
{
	if (m_MediaObject->state() == Phonon::PlayingState)
		mpPausa();
	else {
		if (m_MediaObject->currentTime() == m_MediaObject->totalTime())
			m_MediaObject->seek(0);
		mpPlay();
	}
}

void frmMultiMedia::mpPlay()
{
	if( (id_source > -1) && (id_source <= total_source) )
		m_MediaObject->play();
}

void frmMultiMedia::mpPausa()
{
	m_MediaObject->pause();
}

void frmMultiMedia::mpStop()
{
	m_MediaObject->stop();
}

void frmMultiMedia::mpAnterior()
{
	id_source--;

	if( id_source < 0 )
	{
		id_source = total_source;
		setItemID(id_source);
	} else
		setItemID(id_source);
}

void frmMultiMedia::mpRebobinarAtras()
{

}

void frmMultiMedia::mpRebobinarAdelante()
{

}

void frmMultiMedia::mpSiguiente()
{
	id_source++;

	if( id_source > total_source )
	{
		id_source = 0;
		setItemID(id_source);
	} else
		setItemID(id_source);
}

void frmMultiMedia::mpAbrir()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	AbrirArchivo(fileName);
	mpUpdateInfo();
}

void frmMultiMedia::mpAleatorio()
{

}

void frmMultiMedia::setPlayPause(bool state)
{
	if(state)
		ui.actionPlayPausa->setIcon(icon_play);
	else
		ui.actionPlayPausa->setIcon(icon_pausa);
}

void frmMultiMedia::setVolume(int volume)
{
	Q_UNUSED(volume);
	m_AudioOutput->setVolume(volume/100.0f);

	if (volume == 0)
		ui.actionMute->setIcon(icon_muted);
	else
		ui.actionMute->setIcon(icon_volume);
}

void frmMultiMedia::setMute(bool estado)
{
	if(estado)
		setVolume(0);
	else
		setVolume(100);
}

void frmMultiMedia::mpUpdateInfo()
{
	QString fileName;

	if( m_MediaObject->currentSource().type() == Phonon::MediaSource::Url )
		fileName = m_MediaObject->currentSource().url().toString();
	else
		fileName = m_MediaObject->currentSource().fileName();

	QFileInfo fi(fileName);

	lbpanel_1->setText(" "+fGrl.IntToStr(id_source+1)+" "+tr("de")+" "+fGrl.IntToStr(total_source+1)+" ");
	lbpanel_2->setText( fi.fileName() );
}

void frmMultiMedia::mpUpdateTime()
{
	long len = m_MediaObject->totalTime();
	long pos = m_MediaObject->currentTime();
	QString timeString;
	if (pos || len)
	{
		int sec = pos/1000;
		int min = sec/60;
		int hour = min/60;
		int msec = pos;

		QTime playTime(hour%60, min%60, sec%60, msec%1000);
		sec = len / 1000;
		min = sec / 60;
		hour = min / 60;
		msec = len;

		QTime stopTime(hour%60, min%60, sec%60, msec%1000);
		QString timeFormat = "m:ss";
		if (hour > 0)
			timeFormat = "h:mm:ss";
		timeString = playTime.toString(timeFormat);
		if (len)
			timeString += " / " + stopTime.toString(timeFormat);
	}
	lb_tiempo->setText("  " + timeString);
}

void frmMultiMedia::mpFinished()
{
//	mpSiguiente();
}

void frmMultiMedia::mpStateChanged(Phonon::State newstate, Phonon::State oldstate)
{
	Q_UNUSED(oldstate);

	if(oldstate == Phonon::LoadingState)
	{
		ui.m_videoWindow->setVisible(m_MediaObject->hasVideo());

		QRect videoHintRect = QRect(QPoint(0, 0), ui.m_videoWindow->sizeHint());
		QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
		if (m_MediaObject->hasVideo())
		{
			qApp->processEvents();
			resize(sizeHint());
		} else
			resize(minimumSize());
	}

	switch (newstate)
	{
		case Phonon::ErrorState:
			QMessageBox::warning(this, "MultiMedia", m_MediaObject->errorString(), QMessageBox::Close);
			if (m_MediaObject->errorType() == Phonon::FatalError)
			{
				ui.actionPlayPausa->setEnabled(false);
				ui.actionRebobinarAtras->setEnabled(false);
			} else {
				m_MediaObject->pause();
				setPlayPause(false);
			}
			break;
		case Phonon::PausedState:
		case Phonon::StoppedState:
			setPlayPause(true);
			if (m_MediaObject->currentSource().type() != Phonon::MediaSource::Invalid)
			{
				ui.actionPlayPausa->setEnabled(true);
				ui.actionRebobinarAtras->setEnabled(true);
			} else {
				ui.actionPlayPausa->setEnabled(false);
				ui.actionRebobinarAtras->setEnabled(false);
			}
			break;
		case Phonon::PlayingState:
			ui.actionPlayPausa->setEnabled(true);
			ui.actionPlayPausa->setIcon(icon_pausa);
			setPlayPause(false);

			if (m_MediaObject->hasVideo())
				ui.m_videoWindow->show();
		case Phonon::BufferingState:
			ui.actionRebobinarAtras->setEnabled(true);
		break;
		case Phonon::LoadingState:
			ui.actionRebobinarAtras->setEnabled(false);
		break;
	}
}

void frmMultiMedia::mpBufferStatus(int percent)
{
	if (percent == 0 || percent == 100)
		lb_buffer->setText(" ");
	else {
		QString str(" (");
		str += QString::number(percent);
		str += "%) ";
		lb_buffer->setText(str);
	}
}

void frmMultiMedia::AbrirArchivo(QString archivo)
{
	if(archivo.isEmpty())
		return;

	sources_list.append(Phonon::MediaSource(archivo));

	total_source = sources_list.size()-1;
//	id_source = 0;
//	setItemID(id_source);
}

void frmMultiMedia::AbrirArchivos(QStringList archivos)
{
	sources_list.clear();
	total_source = -1;
	id_source = -1;

	if(archivos.isEmpty())
		return;

	foreach (QString string, archivos)
		sources_list.append(Phonon::MediaSource(string));

	total_source = sources_list.size()-1;
//	id_source = 0;
//	setItemID(id_source);

	ui.actionSiguiente->setEnabled(true);
}

void frmMultiMedia::setItemID(int id)
{
	if( (id > -1) && (id <= total_source) )
	{
		id_source = id;
		m_MediaObject->setCurrentSource(sources_list.at(id_source));
		mpPlay();
	}
}
