#include "grlida_multimedia.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QDirIterator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QPainter>

GrlMultiMedia::GrlMultiMedia(QString dir_game, QString dir_theme_app, stGrlCfg m_cfg, bool onlySound, bool is_main, QWidget *parent) : QWidget(parent)
{
	setInfo("");
	stDirGame     = dir_game;
	m_fullScreen  = false;
	m_repetir     = false;
	m_onlySound   = onlySound;
	isMain        = is_main;
	current_media = -1;
	select_media  = -1;

	QVBoxLayout *vLayout_main = new QVBoxLayout(this);
	vLayout_main->setSpacing(4);
	vLayout_main->setContentsMargins(0, 0, 0, 0);
		splitter = new QSplitter(this);
		splitter->setOrientation(Qt::Vertical);
		splitter->installEventFilter(this);
			w_media = new QWidget(splitter);
			w_media->installEventFilter(this);
			QVBoxLayout *vLayout_media = new QVBoxLayout(w_media);
			vLayout_media->setSpacing(0);
			vLayout_media->setContentsMargins(0, 0, 0, 0);
				media = new GrlMedia(w_media);
				media->installEventFilter(this);
			vLayout_media->addWidget(media);
				mediaInfo = new AnimatedTextBrowser(w_media);
				mediaInfo->installEventFilter(this);
				mediaInfo->setFrameShape(QFrame::NoFrame);
				mediaInfo->document()->setDocumentMargin(0);
				mediaInfo->setMinimumHeight(60);
			vLayout_media->addWidget(mediaInfo);
		splitter->addWidget(w_media);
			lwMedia = new QListWidget(splitter);
			lwMedia->setMinimumHeight(100);
			lwMedia->setVisible(false);
			connect(lwMedia, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(lwMedia_DoubleClicked(QListWidgetItem*)));
			connect(lwMedia, SIGNAL(currentRowChanged(int)), this, SLOT(lwMedia_currentRowChanged(int)));
		splitter->addWidget(lwMedia);
	vLayout_main->addWidget(splitter);
		w_controls = new QWidget(this);
		QVBoxLayout *vLayout_controls = new QVBoxLayout(w_controls);
		vLayout_controls->setSpacing(2);
		vLayout_controls->setContentsMargins(4, 0, 4, 4);
			QHBoxLayout *hLayout_1 = new QHBoxLayout();
			hLayout_1->setSpacing(6);
				lb_status = new QLabel(w_controls);
				lb_status->setText("0 "+ tr("de") +" 0");
			hLayout_1->addWidget(lb_status);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
				slider = new Phonon::SeekSlider(w_controls);
				slider->setMediaObject(media->getMediaObject());
#else
				slider = new QSlider(Qt::Horizontal, w_controls);
				slider->setRange(0, static_cast<int>(media->duration() / 1000));
				slider->setEnabled(false);
				connect(slider, SIGNAL(sliderMoved(int)), media, SLOT(seek(int)));
#endif
			hLayout_1->addWidget(slider);
				lb_tiempo = new QLabel(w_controls);
				lb_tiempo->setText("00:00 / 00:00");
			hLayout_1->addWidget(lb_tiempo);
				lb_buffer = new QLabel(w_controls);
			hLayout_1->addWidget(lb_buffer);
		vLayout_controls->addLayout(hLayout_1);
			QHBoxLayout *hLayout_2 = new QHBoxLayout();
			hLayout_2->setMargin(0);
			hLayout_2->setSpacing(6);
				btnPlayPausa = new QToolButton(w_controls);
				btnPlayPausa->setMinimumSize(QSize(26, 26));
				btnPlayPausa->setMaximumSize(QSize(26, 26));
				btnPlayPausa->setEnabled(false);
				connect(btnPlayPausa, SIGNAL(clicked()), this, SLOT(playPausa()));
			hLayout_2->addWidget(btnPlayPausa);
				QSpacerItem *hSpacer_1 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
			hLayout_2->addItem(hSpacer_1);
				btnAnterior = new QToolButton(w_controls);
				btnAnterior->setMinimumSize(QSize(26, 26));
				btnAnterior->setMaximumSize(QSize(26, 26));
				btnAnterior->setEnabled(false);
				connect(btnAnterior, SIGNAL(clicked()), this, SLOT(previousClicked()));
			hLayout_2->addWidget(btnAnterior);
				btnStop = new QToolButton(w_controls);
				btnStop->setMinimumSize(QSize(26, 26));
				btnStop->setMaximumSize(QSize(26, 26));
				btnStop->setEnabled(false);
				connect(btnStop, SIGNAL(clicked()), media, SLOT(stop()));
			hLayout_2->addWidget(btnStop);
				btnSiguiente = new QToolButton(w_controls);
				btnSiguiente->setMinimumSize(QSize(26, 26));
				btnSiguiente->setMaximumSize(QSize(26, 26));
				btnSiguiente->setEnabled(false);
				connect(btnSiguiente, SIGNAL(clicked()), this, SLOT(nextClicked()));
			hLayout_2->addWidget(btnSiguiente);
				QSpacerItem *hSpacer_2 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
			hLayout_2->addItem(hSpacer_2);
				btnRepetir = new QToolButton(w_controls);
				btnRepetir->setMinimumSize(QSize(26, 26));
				btnRepetir->setMaximumSize(QSize(26, 26));
				btnRepetir->setCheckable(true);
				connect(btnRepetir, SIGNAL(toggled(bool)), this, SLOT(repetirClicked(bool)));
			hLayout_2->addWidget(btnRepetir);
				QSpacerItem *hSpacer_3 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
			hLayout_2->addItem(hSpacer_3);
				btnAbrir = new QToolButton(w_controls);
				btnAbrir->setMinimumSize(QSize(26, 26));
				btnAbrir->setMaximumSize(QSize(26, 26));
				btnAbrir->setFocusPolicy(Qt::NoFocus);
				connect(btnAbrir, SIGNAL(clicked()), this, SLOT(open()));
			hLayout_2->addWidget(btnAbrir);
				btnListaMedia = new QToolButton(w_controls);
				btnListaMedia->setMinimumSize(QSize(26, 26));
				btnListaMedia->setMaximumSize(QSize(26, 26));
				btnListaMedia->setCheckable(true);
				btnListaMedia->setChecked(false);
				connect(btnListaMedia, SIGNAL(toggled(bool)), this, SLOT(setPlaylistVisible(bool)));
			hLayout_2->addWidget(btnListaMedia);
				QSpacerItem *hSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			hLayout_2->addItem(hSpacer_4);
				btnMute = new QToolButton(w_controls);
				btnMute->setAutoRaise(true);
				connect(btnMute, SIGNAL(clicked()), this, SLOT(muteClicked()));
			hLayout_2->addWidget(btnMute);
				volumeSlider = new QSlider(Qt::Horizontal, w_controls);
				volumeSlider->setRange(0, 100);
				volumeSlider->setFixedWidth(100);
				connect(volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(setVolume(int)));
				volumeSlider->setValue(100);
			hLayout_2->addWidget(volumeSlider);
				btnFullScreen = new QToolButton(w_controls);
				btnFullScreen->setMinimumSize(QSize(26, 26));
				btnFullScreen->setMaximumSize(QSize(26, 26));
				btnFullScreen->setCheckable(true);
				connect(btnFullScreen, SIGNAL(clicked(bool)), this, SLOT(setFullScreen(bool)));
			hLayout_2->addWidget(btnFullScreen);
				btnMediaInfo = new QToolButton(w_controls);
				btnMediaInfo->setMinimumSize(QSize(26, 26));
				btnMediaInfo->setMaximumSize(QSize(26, 26));
				btnMediaInfo->setCheckable(true);
				connect(btnMediaInfo, SIGNAL(clicked(bool)), this, SLOT(setMediaInfoVisible(bool)));
			hLayout_2->addWidget(btnMediaInfo);
		vLayout_controls->addLayout(hLayout_2);
	vLayout_main->addWidget(w_controls);
	vLayout_main->setStretch(0, 1);

	setLayout(vLayout_main);

	connect(media, SIGNAL(maxSliderChanged(qint64)), this, SLOT(setMaxSlider(qint64)));
	connect(media, SIGNAL(posSliderChanged(qint64)), this, SLOT(setPosSlider(qint64)));
	connect(media, SIGNAL(infoChanged(QString)), this, SLOT(setInfo(QString)));
	connect(media, SIGNAL(timeChanged(QString)), lb_tiempo, SLOT(setText(QString)));
	connect(media, SIGNAL(bufferChanged(QString)), lb_buffer, SLOT(setText(QString)));
	connect(media, SIGNAL(stateChanged()), this, SLOT(stateChanged()));
	connect(media, SIGNAL(controlVisible(bool)), this, SLOT(controlVisible(bool)));
	connect(media, SIGNAL(fullScreenChanged(bool)), this, SLOT(setFullScreen(bool)));
	connect(media, SIGNAL(endOfMedia()), this, SLOT(endOfMedia()));
	connect(media, SIGNAL(resizeChanged(QSize)), this, SLOT(setResize(QSize)));
	connect(media, SIGNAL(metaDataChanged(MediaMetaData)), this, SLOT(setMetaData(MediaMetaData)));

	setOnlySound(m_onlySound);
	setIconSize(24);
	setTheme(dir_theme_app);
	setGrlCfg(m_cfg);

	clearPlaylist();

	if (!media->isPlayerAvailable())
	{
		QMessageBox::warning(this, tr("Service not available"),
							tr("The QMediaPlayer object does not have a valid service.\n"
							"Please check the media service plugins are installed."));

		btnAbrir->setEnabled(false);
	}

// centra la aplicacion en el escritorio
//	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

GrlMultiMedia::~GrlMultiMedia()
{
	delete media;
}

void GrlMultiMedia::closeEvent(QCloseEvent *event)
{
	closeMedia();
	event->ignore();
	setHidden(true);
}

void GrlMultiMedia::keyPressEvent(QKeyEvent *event)
{
	if (!m_onlySound)
	{
		if (event->key() == Qt::Key_Escape && isFullScreen())
		{
			m_fullScreen = false;
			media->setFullScreen(m_fullScreen);
			event->accept();
		}

		if (event->modifiers() & Qt::AltModifier && event->key() == Qt::Key_Enter)
		{
			media->setFullScreen(!m_fullScreen);
			event->accept();
		}
	}

	QWidget::keyPressEvent(event);
}

bool GrlMultiMedia::eventFilter(QObject *object, QEvent *event)
{
	if (m_fullScreen)
	{
		if (object == this || object == splitter || object == lwMedia)
		{
			if (event->type() == QEvent::Enter)
			{
				controlVisible(true);
				return true;
			}
		}
		if (object == media)
		{
			if (event->type() == QEvent::Enter)
			{
				controlVisible(false);
				return true;
			}
		}
	}

	return QWidget::eventFilter(object, event);
}

void GrlMultiMedia::setTheme(QString dir_theme)
{
	stTheme = dir_theme;

	setWindowIcon(QIcon(stTheme +"img16/multimedia.png"));

	playIcon  = QIcon(stTheme +"img16/mp_play.png");
	pauseIcon = QIcon(stTheme +"img16/mp_pausa.png");

	btnPlayPausa->setIcon(playIcon);
	btnAnterior->setIcon(QIcon(stTheme +"img16/mp_anterior.png"));
	btnStop->setIcon(QIcon(stTheme +"img16/mp_stop.png"));
	btnSiguiente->setIcon(QIcon(stTheme +"img16/mp_siguiente.png"));
	btnRepetir->setIcon(QIcon(stTheme +"img16/mp_repeat.png"));
	btnAbrir->setIcon(QIcon(stTheme +"img16/mp_eject.png"));
	btnListaMedia->setIcon(QIcon(stTheme +"img16/mp_playlist.png"));
	btnMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
	btnFullScreen->setIcon(QIcon(stTheme +"img16/fit_to_win.png"));
	btnMediaInfo->setIcon(QIcon(stTheme +"img16/informacion.png"));
}

void GrlMultiMedia::setTemplate(QString tpl)
{
	tpl_info_media_old = tpl;
}

void GrlMultiMedia::setGrlCfg(stGrlCfg m_cfg)
{
	grlCfg = m_cfg;

	audio_ext.clear();
	audio_ext << grlCfg.FormatsMusic;

	video_ext.clear();
	video_ext << grlCfg.FormatsVideo;

	if (m_onlySound)
	{
		w_media->setVisible(isMain ? grlCfg.VerInfoMediaMain : grlCfg.VerInfoMedia);
		btnMediaInfo->setChecked(isMain ? grlCfg.VerInfoMediaMain : grlCfg.VerInfoMedia);

		setPlaylistVisible(isMain ? grlCfg.VerPlaylistSoundMain : grlCfg.VerPlaylistSound);
	} else {
		w_media->setVisible(true);

		setPlaylistVisible(isMain ? grlCfg.VerPlaylistVideoMain : grlCfg.VerPlaylistVideo);
	}
}

void GrlMultiMedia::setIconSize(int size)
{
	lwMedia->setIconSize(QSize(size, size));
	sizeIcon = QString::number(size);
}

void GrlMultiMedia::setOnlySound(bool checked)
{
	m_onlySound = checked;

	btnFullScreen->setVisible(!m_onlySound);
	btnMediaInfo->setVisible(m_onlySound);
	media->setVisible(!m_onlySound);
	mediaInfo->setVisible(m_onlySound);
}

void GrlMultiMedia::setPlaylistVisible(bool checked)
{
	lwMedia->setVisible(checked);
	btnListaMedia->setChecked(checked);

	if (m_onlySound)
	{
		if (isMain)
			grlCfg.VerPlaylistSoundMain = checked;
		else
			grlCfg.VerPlaylistSound = checked;
	} else {
		if (isMain)
			grlCfg.VerPlaylistVideoMain = checked;
		else
			grlCfg.VerPlaylistVideo = checked;
	}

	emit changeConfig();
}

void GrlMultiMedia::setMediaInfoVisible(bool checked)
{
	w_media->setVisible(checked);
	btnMediaInfo->setChecked(checked);

	if (isMain)
		grlCfg.VerInfoMediaMain = checked;
	else
		grlCfg.VerInfoMedia = checked;

	emit changeConfig();
}

void GrlMultiMedia::setBtnPlaylistVisible(bool checked)
{
	btnListaMedia->setVisible(checked);
}

void GrlMultiMedia::setVolume(int volume)
{
	if (volumeSlider)
	{
		volumeSlider->setValue(volume);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		media->setVolume(double(volume / 100.0));
#else
		media->setVolume(volume);
#endif
	}
}

void GrlMultiMedia::setMuted(bool muted)
{
	if (muted != media->isMuted())
	{
		media->setMuted(muted);
		btnMute->setIcon(style()->standardIcon(muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
	}
}

void GrlMultiMedia::setStatus()
{
	lb_status->setText(QString("%1 "+ tr("de") +" %2").arg(current_media + 1).arg(total_media));
}

void GrlMultiMedia::clearPlaylist()
{
	media->stop();
	media->clearFile();
	lwMedia->clear();
	listMedia.clear();
	lb_tiempo->setText("00:00 / 00:00");

	current_media = -1;
	select_media  = -1;
	total_media   = listMedia.size();

	setStatus();
}

void GrlMultiMedia::addItemPlaylist(QFileInfo f_info, int row, int i)
{
	QString img = "multimedia.png";
	QString ext = f_info.suffix().toLower();
	if (audio_ext.contains("*."+ ext, Qt::CaseInsensitive))
	{
		img = "audio.png";
	}
	if (video_ext.contains("*."+ ext, Qt::CaseInsensitive))
	{
		img = "video.png";
	}

	int new_row = row + i;
	QListWidgetItem *item = new QListWidgetItem;
		item->setIcon(QIcon(stTheme +"img"+ sizeIcon +"/"+ img));
		item->setText(f_info.fileName());
	lwMedia->insertItem(new_row, item);
	listMedia.insert(new_row, f_info.filePath());
}

void GrlMultiMedia::setDirPlaylist(QString dir, QStringList filter, bool isClear, bool isSubDir)
{
	if (isClear)
		clearPlaylist();

	QDirIterator it(dir, filter, QDir::Files | QDir::Readable | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, (isSubDir ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags));
	int row = listMedia.size();
	int i = 0;
	while (it.hasNext())
	{
		it.next();
		if (!it.fileInfo().isDir())
		{
			addItemPlaylist(it.fileInfo(), row, i);
			++i;
		}
	}

	total_media = listMedia.size();
	if (total_media > 0)
	{
		current_media = 0;
		select_media  = 0;
		lwMedia->setCurrentRow(current_media);

		media->setFile(listMedia.at(current_media));
	}

	setStatus();

	btnAnterior->setEnabled(total_media > 0 ? true : false);
	btnSiguiente->setEnabled(total_media > 0 ? true : false);
	slider->setEnabled(total_media > 0 ? true : false);
	btnPlayPausa->setEnabled(total_media > 0 ? true : false);
	btnStop->setEnabled(total_media > 0 ? true : false);
}

void GrlMultiMedia::setPlaylist(QStringList fileNames, bool autoPlay, int id_select, bool isClear)
{
	if (isClear)
		clearPlaylist();

	int const countFileNames = fileNames.size();
	if (countFileNames > 0)
	{
		QFileInfo f_info;
		int row = listMedia.size();
		for (int i = 0; i < countFileNames; ++i)
		{
			f_info.setFile(fileNames.at(i));
			addItemPlaylist(f_info, row, i);
		}

		total_media = listMedia.size();
		current_media = (id_select == -1 ? row : id_select);
		select_media  = current_media;
		lwMedia->setCurrentRow(current_media);

		media->setFile(listMedia.at(current_media));
		if (autoPlay)
			media->play();
	}

	setStatus();

	btnAnterior->setEnabled(total_media > 0 ? true : false);
	btnSiguiente->setEnabled(total_media > 0 ? true : false);
	slider->setEnabled(total_media > 0 ? true : false);
	btnPlayPausa->setEnabled(total_media > 0 ? true : false);
	btnStop->setEnabled(total_media > 0 ? true : false);
}

void GrlMultiMedia::addPlaylist(QString fileName)
{
	QStringList fileNames;
	fileNames << fileName;

	setPlaylist(fileNames, false);
}

bool GrlMultiMedia::removeSelectMedia()
{
	if (lwMedia->count() > 0 && lwMedia->currentItem()->isSelected())
	{
		media->stop();
		media->clearFile();

		lb_tiempo->setText("00:00 / 00:00");
		current_media = lwMedia->currentRow();
		select_media  = current_media;

		listMedia.removeAt(current_media);
		delete lwMedia->currentItem();

		if (lwMedia->count() > 0 && lwMedia->currentItem()->isSelected())
		{
			current_media = lwMedia->currentRow();
			select_media  = current_media;
		}
	}

	total_media = listMedia.size();
	setStatus();

	return true;
}

void GrlMultiMedia::closeMedia()
{
	media->stop();
	media->clearFile();
}

QString GrlMultiMedia::getFileName()
{
	return listMedia.at(lwMedia->currentRow());
}

void GrlMultiMedia::open()
{
	QString ext_audio = audio_ext.join(" ");
	QString ext_video = video_ext.join(" ");
	QString filter = "";

	if (!m_onlySound)
	{
		filter = tr("Archivos soportados") +" - ("+ ext_video +" "+ ext_audio +");;"+
						tr("Archivos de video") +" - ("+ ext_video +");;"+
						tr("Archivos de audio") +" - ("+ ext_audio +");;";
	} else
		filter = tr("Archivos de audio") +" - ("+ ext_audio +");;";
	filter.append(tr("Todos los archivos") +" (*)");

	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Abrir archivo multimedia") +"...", stDirGame, filter);
	if (fileNames.size() > 0)
		setPlaylist(fileNames);
}

void GrlMultiMedia::lwMedia_DoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		current_media = lwMedia->currentRow();
		select_media  = current_media;

		setStatus();

		media->setFile(listMedia.at(current_media));
		media->play();
	}
}

void GrlMultiMedia::lwMedia_currentRowChanged(int row)
{
	if (row > -1)
	{
		select_media = row;
	}
}

void GrlMultiMedia::playPausa()
{
	if (current_media != select_media)
	{
		current_media = select_media;

		setStatus();

		media->stop();
		media->setFile(listMedia.at(select_media));
		media->play();
	} else
		media->playPausa();
}

void GrlMultiMedia::previousClicked()
{
	int row = current_media;
	if (total_media > 0 && row > -1)
	{
		if (row <= 0)
			row = total_media - 1;
		else
			row = row - 1;

		current_media = row;
		select_media  = row;
		lwMedia->setCurrentRow(row);

		setStatus();

		media->setFile(listMedia.at(current_media));
		media->play();
	}
}

void GrlMultiMedia::nextClicked()
{
	int row = current_media;
	if (total_media > 0 && row > -1)
	{
		if (row >= total_media - 1)
			row = 0;
		else
			row = row + 1;
		current_media = row;
		select_media  = row;
		lwMedia->setCurrentRow(row);

		setStatus();

		media->setFile(listMedia.at(current_media));
		media->play();
	}
}

void GrlMultiMedia::repetirClicked(bool checked)
{
	m_repetir = checked;
}

void GrlMultiMedia::muteClicked()
{
	setMuted(!media->isMuted());
}

void GrlMultiMedia::setFullScreen(bool checked)
{
	if (!m_onlySound)
	{
		btnFullScreen->setChecked(checked);
		if (checked)
		{
			m_fullScreen = true;
			w_controls->hide();
			this->showFullScreen();
		} else {
			m_fullScreen = false;
			w_controls->show();
			this->showNormal();
		}
	}
}

void GrlMultiMedia::setResize(QSize size)
{
	if (!m_onlySound)
	{
		if (size.width() > -1 && size.height() > -1)
		{
			if (!m_fullScreen)
			{
				int width    = size.width();
				int height   = size.height() + w_controls->height() +4;
				int d_width  = qApp->desktop()->width();
				int d_height = qApp->desktop()->height();

				if (width > d_width)
					width = d_width;
				if (height > d_height)
					height = d_height;

				resize(width, height);
				this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
			}
		}
	}
}

void GrlMultiMedia::setMetaData(MediaMetaData metaData)
{
	QString tpl_info_media_new = tpl_info_media_old;
		tpl_info_media_new.replace("{info_titulo}"      , metaData.title      );
		tpl_info_media_new.replace("{info_album_title}" , metaData.albumTitle );
		tpl_info_media_new.replace("{info_album_artist}", metaData.albumArtist);
	mediaInfo->setHtml(tpl_info_media_new);
}

void GrlMultiMedia::setInfo(QString info)
{
	if (!info.isEmpty())
		info = " - "+ info;
	setWindowTitle("GR-lida MultiMedia"+ info);
}

void GrlMultiMedia::controlVisible(bool visible)
{
	if (m_fullScreen)
		w_controls->setVisible(visible);
}

void GrlMultiMedia::stateChanged()
{
	if (media->isPlayIcon())
		btnPlayPausa->setIcon(playIcon);
	else
		btnPlayPausa->setIcon(pauseIcon);
}

void GrlMultiMedia::endOfMedia()
{
	if (m_repetir)
		media->play();
	else {
		if ((current_media + 1) >= total_media)
			media->stop();
		else
			emit nextClicked();
	}
}

void GrlMultiMedia::setMaxSlider(qint64 max)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	slider->setMaximum(static_cast<int>(max));
#else
	Q_UNUSED(max);
#endif
}

void GrlMultiMedia::setPosSlider(qint64 pos)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	if (!slider->isSliderDown())
		slider->setValue(static_cast<int>(pos));
#else
	Q_UNUSED(pos);
#endif
}
