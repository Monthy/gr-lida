#ifndef GRLIDA_MEDIA_H
#define GRLIDA_MEDIA_H

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QBasicTimer>
#include <QWidget>

struct MediaMetaData {
	QString title;
	QString albumTitle;
	QString albumArtist;
};

class MediaVideoWidget : public QVideoWidget
{
	Q_OBJECT
public:
	MediaVideoWidget(QWidget *parent = 0);
};

class GrlMedia : public QWidget
{
	Q_OBJECT

public:
	GrlMedia(QWidget *parent = 0);

	void clearFile();
	void setFile(const QString &fileName);
	void setLocation(const QString &location);

	bool isPlayerAvailable() const;
	bool isPlaying() { return m_isPlaying; }
	bool isPlayIcon() { return m_playIcon; }

	QMediaPlayer::State state() const;

	int volume() const;
	bool isMuted() const;

	qint64 duration() const { return m_duration; }

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	bool event(QEvent *event);
	void timerEvent(QTimerEvent *event);
	void paintEvent(QPaintEvent *event);

signals:
	void isVideoVisible(bool available);
	void fullScreenChanged(bool fullScreen);
	void endOfMedia();
	void resizeChanged(QSize size);
	void controlVisible(bool visible);
	void infoChanged(QString info);
	void timeChanged(QString time);
	void bufferChanged(QString buffer);
	void volumeChanged(int volume);
	void mutedChanged(bool muted);
	void stateChanged();
	void metaDataChanged(MediaMetaData metaData);
	void maxSliderChanged(qint64 max);
	void posSliderChanged(qint64 pos);

public slots:
	void setFullScreen(int fullScreen);
	void setVolume(int volume);
	void setMuted(bool muted);

	void playPausa();
	void play();
	void pause();
	void stop();

	void seek(int seconds);

private:
	MediaVideoWidget *m_videoWidget;
	QMediaPlayer *m_player;
	QMediaPlayer::State playerState;

	QBrush m_brush;
	QString lb_info, lb_tiempo, lb_buffer;
	qint64  m_duration;
	bool m_isPlaying, m_playIcon, m_fullScreen;
	QBasicTimer m_timer;

	void setStatusInfo(const QString &info);
	void handleCursor(QMediaPlayer::MediaStatus status);
	void updateDurationInfo(qint64 currentInfo);

private slots:
	void videoAvailableChanged(bool available);
	void statusChanged(QMediaPlayer::MediaStatus status);
	void setState(QMediaPlayer::State state);
	void setMetaData();

	void bufferingProgress(int progress);
	void durationChanged(qint64 duration);
	void positionChanged(qint64 progress);

	void displayErrorMessage();

};

#endif // GRLIDA_MEDIA_H
