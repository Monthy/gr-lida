#include <QMediaMetaData>
#include <QMouseEvent>
#include <QTime>
#include <QPainter>
#include <QVBoxLayout>

#include "grlida_media_qt5.h"

MediaVideoWidget::MediaVideoWidget(QWidget *parent) : QVideoWidget(parent)
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

	playerState  = QMediaPlayer::StoppedState;
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

	m_player = new QMediaPlayer(this);
	m_player->setVideoOutput(m_videoWidget);

	connect(m_player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
	connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
	connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
	connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
	connect(m_player, SIGNAL(videoAvailableChanged(bool)), this, SLOT(videoAvailableChanged(bool)));
	connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));
	connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(setState(QMediaPlayer::State)));
	connect(m_player, SIGNAL(volumeChanged(int)), this, SIGNAL(volumeChanged(int)));
	connect(m_player, SIGNAL(mutedChanged(bool)), this, SIGNAL(mutedChanged(bool)));
	connect(m_player, SIGNAL(metaDataChanged()), this, SLOT(setMetaData()));

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
	m_player->setMedia(QMediaContent());
}

void GrlMedia::setFile(const QString &fileName)
{
	m_player->setMedia(QUrl::fromLocalFile(fileName));
}

void GrlMedia::setLocation(const QString &location)
{
	m_player->setMedia(QUrl::fromEncoded(location.toUtf8()));
}

bool GrlMedia::isPlayerAvailable() const
{
	return m_player->isAvailable();
}

QMediaPlayer::State GrlMedia::state() const
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
	return m_player->volume();
}

void GrlMedia::setVolume(int volume)
{
	m_player->setVolume(volume);
}

bool GrlMedia::isMuted() const
{
	return m_player->isMuted();
}

void GrlMedia::setMuted(bool muted)
{
	m_player->setMuted(muted);
}

void GrlMedia::playPausa()
{
	switch (playerState)
	{
		case QMediaPlayer::StoppedState:
		case QMediaPlayer::PausedState:
			emit play();
		break;
		case QMediaPlayer::PlayingState:
			emit pause();
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

void GrlMedia::seek(int seconds)
{
	m_player->setPosition(seconds * 1000);
}

void GrlMedia::setStatusInfo(const QString &info)
{
	lb_info = (!info.isEmpty()) ? info : "";
	emit infoChanged(lb_info);
}

void GrlMedia::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
	if (status == QMediaPlayer::LoadingMedia ||
		status == QMediaPlayer::BufferingMedia ||
		status == QMediaPlayer::StalledMedia)
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
		QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
		QTime totalTime((m_duration/3600)%60, (m_duration/60)%60, m_duration%60, (m_duration*1000)%1000);

		QString format = "mm:ss";
		if (m_duration > 3600)
			format = "hh:mm:ss";
		lb_tiempo = currentTime.toString(format) + " / " + totalTime.toString(format);
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

void GrlMedia::statusChanged(QMediaPlayer::MediaStatus status)
{
	handleCursor(status);

// handle status message
	switch (status)
	{
		case QMediaPlayer::UnknownMediaStatus:
		case QMediaPlayer::NoMedia:
		case QMediaPlayer::LoadedMedia:
		case QMediaPlayer::BufferingMedia:
		case QMediaPlayer::BufferedMedia:
			setStatusInfo("");
		break;
		case QMediaPlayer::LoadingMedia:
			setStatusInfo(tr("Loading..."));
		break;
		case QMediaPlayer::StalledMedia:
			setStatusInfo(tr("Media Stalled"));
		break;
		case QMediaPlayer::EndOfMedia:
			emit endOfMedia();
		break;
		case QMediaPlayer::InvalidMedia:
			displayErrorMessage();
		break;
	}
}

void GrlMedia::setState(QMediaPlayer::State state)
{
	if (state != playerState)
	{
		playerState = state;

		switch (state)
		{
			case QMediaPlayer::StoppedState:
				m_playIcon = true;
			break;
			case QMediaPlayer::PlayingState:
				m_playIcon = false;
			break;
			case QMediaPlayer::PausedState:
				m_playIcon = true;
			break;
		}
		emit stateChanged();
	}
}

void GrlMedia::setMetaData()
{
	if (m_player->isMetaDataAvailable())
	{
		MediaMetaData metaData;
		metaData.title       = m_player->metaData(QMediaMetaData::Title).toString();
		metaData.albumTitle  = m_player->metaData(QMediaMetaData::AlbumTitle).toString();
		metaData.albumArtist = m_player->metaData(QMediaMetaData::AlbumArtist).toString();
		emit metaDataChanged(metaData);

		QSize resolution = m_player->metaData(QMediaMetaData::Resolution).toSize();
		emit resizeChanged(resolution);
	}
}

void GrlMedia::bufferingProgress(int progress)
{
	lb_buffer = QString(tr("Buffering %4%").arg(progress));
	emit bufferChanged(lb_buffer);
}

void GrlMedia::durationChanged(qint64 duration)
{
	m_duration = duration / 1000;
	emit maxSliderChanged(m_duration);
	updateDurationInfo(0);
}

void GrlMedia::positionChanged(qint64 progress)
{
	qint64 pos = progress / 1000;
	emit posSliderChanged(pos);
	updateDurationInfo(pos);
}

void GrlMedia::displayErrorMessage()
{
	setStatusInfo(m_player->errorString());
}
