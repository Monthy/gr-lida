#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QTime>
#include <QPainter>
#include <QVBoxLayout>
#include <QUrl>

#include "grlida_media.h"

MediaVideoWidget::MediaVideoWidget(QWidget *parent) : Phonon::VideoWidget(parent)
{
	QPalette p = palette();
	p.setColor(QPalette::Window, Qt::black);
	setPalette(p);

	setAttribute(Qt::WA_OpaquePaintEvent);
	setMouseTracking(true);
}

GrlMedia::GrlMedia(QWidget *parent) : QWidget(parent)
{
	m_brush.setStyle(Qt::SolidPattern);
	m_brush.setColor(Qt::black);
	setMouseTracking(true);

	playerState  = Phonon::StoppedState;
	m_isPlaying  = false;
	m_playIcon   = true;
	m_fullScreen = false;

	m_videoWidget = new MediaVideoWidget(this);
	m_videoWidget->setMinimumSize(100, 100);
	m_videoWidget->setFocusPolicy(Qt::NoFocus);
	m_videoWidget->hide();

	QVBoxLayout *videoLayout = new QVBoxLayout(this);
	videoLayout->addWidget(m_videoWidget);
	videoLayout->setSpacing(0);
	videoLayout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(videoLayout);

	m_player      = new Phonon::MediaObject(this);
	m_AudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);

	connect(m_player, SIGNAL(totalTimeChanged(qint64)), this, SLOT(durationChanged(qint64)));
	connect(m_player, SIGNAL(tick(qint64)), this, SLOT(positionChanged(qint64)));
	connect(m_player, SIGNAL(bufferStatus(int)), this, SLOT(bufferingProgress(int)));
	connect(m_player, SIGNAL(hasVideoChanged(bool)), this, SLOT(videoAvailableChanged(bool)));
	connect(m_player, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(setState(Phonon::State,Phonon::State)));
	connect(m_AudioOutput, SIGNAL(volumeChanged(qreal)), this, SIGNAL(volumeChanged(qreal)));
	connect(m_AudioOutput, SIGNAL(mutedChanged(bool)), this, SIGNAL(mutedChanged(bool)));
	connect(m_player, SIGNAL(metaDataChanged()), this, SLOT(setMetaData()));
	connect(m_player, SIGNAL(finished()), this, SLOT(finished()));

	m_audioOutputPath = Phonon::createPath(m_player, m_AudioOutput);
	Phonon::createPath(m_player, m_videoWidget);

	emit isVideoVisible(false);
}

void GrlMedia::mouseDoubleClickEvent(QMouseEvent *event)
{
	setFullScreen(!m_fullScreen);
	event->accept();
}

void GrlMedia::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);
}

void GrlMedia::mouseMoveEvent(QMouseEvent *event)
{
	if (event->pos().y() > (height()-10))
		emit controlVisible(true);
	else
		emit controlVisible(false);
	QWidget::mouseMoveEvent(event);
}

bool GrlMedia::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::Close:
		// We just ignore the cose events on the video widget
		// this prevents ALT+F4 from having an effect in fullscreen mode
			event->ignore();
			return true;
		case QEvent::MouseMove:
#ifndef QT_NO_CURSOR
			unsetCursor();
#endif
		break;
		// Fall through
		case QEvent::WindowStateChange:
		{
		// We just update the state of the checkbox, in case it wasn't already
			if (m_fullScreen)
				m_timer.start(1000, this);
			else {
				m_timer.stop();
#ifndef QT_NO_CURSOR
				unsetCursor();
#endif
			}
		}
		break;
		default:
		break;
	}
	return QWidget::event(event);
}

void GrlMedia::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId())
	{
	// let's store the cursor shape
#ifndef QT_NO_CURSOR
		setCursor(Qt::BlankCursor);
#endif
	}
	QWidget::timerEvent(event);
}

void GrlMedia::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(rect(), m_brush);

	QWidget::paintEvent(event);
}

void GrlMedia::clearFile()
{
	m_player->clearQueue();
	m_player->clear();
}

void GrlMedia::setFile(const QString &fileName)
{
	m_player->setCurrentSource(Phonon::MediaSource(fileName));
}

void GrlMedia::setLocation(const QString &location)
{
	m_player->setCurrentSource(Phonon::MediaSource(QUrl::fromEncoded(location.toUtf8())));
}

bool GrlMedia::isPlayerAvailable() const
{
	return m_player->isValid();
}

Phonon::State GrlMedia::state() const
{
	return playerState;
}

void GrlMedia::setFullScreen(int fullScreen)
{
	m_fullScreen = fullScreen;
	emit fullScreenChanged(m_fullScreen);
}

int GrlMedia::volume() const
{
	return m_AudioOutput->volume();
}

void GrlMedia::setVolume(qreal volume)
{
	m_AudioOutput->setVolume(volume);
}

bool GrlMedia::isMuted() const
{
	return m_AudioOutput->isMuted();
}

void GrlMedia::setMuted(bool muted)
{
	m_AudioOutput->setMuted(muted);
}

void GrlMedia::playPausa()
{
	switch (playerState)
	{
		case Phonon::StoppedState:
		case Phonon::PausedState:
			emit play();
		break;
		case Phonon::PlayingState:
			emit pause();
		break;
		case Phonon::LoadingState:
		case Phonon::BufferingState:
		case Phonon::ErrorState:
		break;
	}
}

void GrlMedia::play()
{
	m_player->play();
}

void GrlMedia::pause()
{
	m_player->pause();
}

void GrlMedia::stop()
{
	m_player->stop();
	emit positionChanged(0);
	update();
}

void GrlMedia::setStatusInfo(const QString &info)
{
	lb_info = (!info.isEmpty()) ? info : "";
	emit infoChanged(lb_info);
}

void GrlMedia::handleCursor(Phonon::State status)
{
#ifndef QT_NO_CURSOR
	if (status == Phonon::LoadingState ||
		status == Phonon::BufferingState ||
		status == Phonon::ErrorState)
		setCursor(QCursor(Qt::BusyCursor));
	else
		unsetCursor();
#endif
}

void GrlMedia::updateDurationInfo(qint64 currentInfo)
{
	lb_tiempo = "00:00 / 00:00";
	if (currentInfo || m_duration)
	{
		int sec  = currentInfo / 1000;
		int min  = sec / 60;
		int hour = min / 60;
		int msec = currentInfo;
		QTime currentTime(hour%60, min%60, sec%60, msec%1000);

		sec  = m_duration / 1000;
		min  = sec / 60;
		hour = min / 60;
		msec = m_duration;
		QTime totalTime(hour%60, min%60, sec%60, msec%1000);

		QString format = "mm:ss";
		if (hour > 0)
			format = "hh:mm:ss";
		lb_tiempo = currentTime.toString(format) +" / "+ totalTime.toString(format);
	}
	emit timeChanged(lb_tiempo);
}

void GrlMedia::videoAvailableChanged(bool available)
{
	if (available)
		m_videoWidget->show();
	else
		m_videoWidget->hide();

	emit isVideoVisible(available);
	update();
}

void GrlMedia::setState(Phonon::State newstate, Phonon::State oldstate)
{
	handleCursor(newstate);

	if (oldstate == Phonon::LoadingState)
	{
		QRect videoHintRect = QRect(QPoint(0, 0), this->sizeHint());
		QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
		if (!m_fullScreen)
		{
			if (m_player->hasVideo())
			{
			// Flush event que so that sizeHint takes the
			// recently shown/hidden m_player into account:
				qApp->processEvents();
				emit resizeChanged(this->sizeHint());
			}
		}
	}

	playerState = newstate;

	setStatusInfo("");
	switch (newstate)
	{
		case Phonon::BufferingState:
			setStatusInfo("");
		break;
		case Phonon::LoadingState:
			setStatusInfo(tr("Loading..."));
		break;
		case Phonon::ErrorState:
			setStatusInfo(tr("Media Stalled"));
			displayErrorMessage();
		break;
		case Phonon::StoppedState:
			m_playIcon = true;
		break;
		case Phonon::PlayingState:
			m_playIcon = false;
		break;
		case Phonon::PausedState:
			m_playIcon = true;
		break;
	}
	emit stateChanged();
}

void GrlMedia::setMetaData()
{
	QMap<QString, QString> m_metaData = m_player->metaData();

	if (!m_metaData.isEmpty())
	{
		MediaMetaData metaData;
		metaData.title       = m_metaData.value("TITLE");
		metaData.albumTitle  = m_metaData.value("ALBUM");
		metaData.albumArtist = m_metaData.value("ARTIST");
		emit metaDataChanged(metaData);
	}
}

void GrlMedia::bufferingProgress(int progress)
{
	lb_buffer = QString(tr("Buffering %4%").arg(progress));
	emit bufferChanged(lb_buffer);
}

void GrlMedia::durationChanged(qint64 duration)
{
	m_duration = duration;
	emit maxSliderChanged(m_duration);
	updateDurationInfo(0);
}

void GrlMedia::positionChanged(qint64 progress)
{
	qint64 pos = progress;
	emit posSliderChanged(pos);
	updateDurationInfo(pos);
}

void GrlMedia::finished()
{
	emit endOfMedia();
}

void GrlMedia::displayErrorMessage()
{
	setStatusInfo(m_player->errorString());
}
