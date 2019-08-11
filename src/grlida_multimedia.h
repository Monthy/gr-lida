#ifndef GRLIDA_MULTIMEDIA_H
#define GRLIDA_MULTIMEDIA_H

#include <QDialog>
#include <QWidget>
#include <QFileInfo>
#include <QLabel>
#include <QToolButton>
#include <QListWidget>
#include <QSplitter>
#include <QSlider>
#include <QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "grlida_media.h"
#else
#include "grlida_media_qt5.h"
#endif

#include "g_structs.h"
#include "animatedtextbrowser.h"

class GrlMultiMedia : public QWidget
{
	Q_OBJECT

public:
	GrlMultiMedia(QString dir_game, QString dir_theme, stGrlCfg m_cfg, bool onlySound, bool is_main, QWidget *parent = 0);
	~GrlMultiMedia();

	void setDirGame(QString dir_game) { stDirGame = dir_game; }
	void setTheme(QString dir_theme);
	void setTemplate(QString tpl);
	void setGrlCfg(stGrlCfg m_cfg);
	void setIconSize(int size);
	void setOnlySound(bool checked);
	void setBtnPlaylistVisible(bool checked);

	void clearPlaylist();
	void setDirPlaylist(QString dir, QStringList filter, bool isClear = false, bool isSubDir = false);
	void setPlaylist(QStringList fileNames, bool autoPlay = true, int id_select = -1, bool isClear = false);

	void addPlaylist(QString fileName);
	bool removeSelectMedia();
	void closeMedia();

	QString getFileName();
	int count() { return lwMedia->count(); }
	int currentRow() { return lwMedia->currentRow(); }
	stGrlCfg getGrlCfg(){ return grlCfg; }

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	bool eventFilter(QObject *object, QEvent *event);

signals:
	void changeConfig();

public slots:
	void playPausa();
	void setVolume(int volume); // 0 - 100
	void setMuted(bool muted);
	void setPlaylistVisible(bool checked);
	void setMediaInfoVisible(bool checked);

private:
	GrlMedia *media;
	QWidget  *w_media, *w_controls;
	stGrlCfg grlCfg;

	QIcon playIcon, pauseIcon;
	QString stDirGame, stTheme, sizeIcon, tpl_info_media_old;
	QStringList listMedia, audio_ext, video_ext;
	bool m_fullScreen, m_repetir, m_onlySound, isMain;
	int total_media, current_media, select_media;

	AnimatedTextBrowser *mediaInfo;
	QLabel *lb_status, *lb_tiempo, *lb_buffer;
	QToolButton *btnPlayPausa;
	QToolButton *btnAnterior;
	QToolButton *btnStop;
	QToolButton *btnSiguiente;
	QToolButton *btnRepetir;
	QToolButton *btnAbrir;
	QToolButton *btnListaMedia;
	QToolButton *btnMute;
	QToolButton *btnFullScreen;
	QToolButton *btnMediaInfo;
	QListWidget *lwMedia;
	QSplitter   *splitter;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	Phonon::SeekSlider *slider;
#else
	QSlider *slider;
#endif
	QSlider *volumeSlider;

	void setStatus();
	void addItemPlaylist(QFileInfo f_info, int row, int i);

private slots:
	void open();
	void lwMedia_DoubleClicked(QListWidgetItem *item);
	void lwMedia_currentRowChanged(int row);

	void previousClicked();
	void nextClicked();
	void repetirClicked(bool checked);
	void muteClicked();

	void setFullScreen(bool checked);
	void setResize(QSize size);
	void setMetaData(MediaMetaData);
	void setInfo(QString info);
	void controlVisible(bool visible);
	void stateChanged();
	void endOfMedia();

	void setMaxSlider(qint64 max);
	void setPosSlider(qint64 pos);

};

#endif // GRLIDA_MULTIMEDIA_H
