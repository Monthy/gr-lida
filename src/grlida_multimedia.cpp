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

#include <QtGui>

#define SLIDER_RANGE 8

#include "grlida_multimedia.h"
#include "ui_multimedia_setup.h"

MediaVideoWidget::MediaVideoWidget(frmMultiMedia *player, QWidget *parent) :
	Phonon::VideoWidget(parent), m_player(player), m_action(this)
{
	m_action.setCheckable(true);
	m_action.setChecked(false);
	m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
	m_action.setShortcutContext(Qt::WindowShortcut);
	connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
	addAction(&m_action);
	setAcceptDrops(false);
}

void MediaVideoWidget::setFullScreen(bool enabled)
{
	Phonon::VideoWidget::setFullScreen(enabled);
	emit fullScreenChanged(enabled);
}

void MediaVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	Phonon::VideoWidget::mouseDoubleClickEvent(e);
	setFullScreen(!isFullScreen());
}

void MediaVideoWidget::keyPressEvent(QKeyEvent *e)
{
	if(!e->modifiers())
	{
	// On non-QWERTY Symbian key-based devices, there is no space key.
	// The zero key typically is marked with a space character.
		if (e->key() == Qt::Key_Space || e->key() == Qt::Key_0)
		{
			m_player->playPause();
			e->accept();
			return;
		}
	// On Symbian devices, there is no key which maps to Qt::Key_Escape
	// On devices which lack a backspace key (i.e. non-QWERTY devices),
	// the 'C' key maps to Qt::Key_Backspace
		else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace)
		{
			setFullScreen(false);
			e->accept();
			return;
		}
	}
	Phonon::VideoWidget::keyPressEvent(e);
}

bool MediaVideoWidget::event(QEvent *e)
{
	switch(e->type())
	{
	case QEvent::Close:
		// we just ignore the cose events on the video widget
		// this prevents ALT+F4 from having an effect in fullscreen mode
		e->ignore();
		return true;
		break;
	case QEvent::MouseMove:
#ifndef QT_NO_CURSOR
		unsetCursor();
#endif
		break;
		// fall through
	case QEvent::WindowStateChange:
		{
			// we just update the state of the checkbox, in case it wasn't already
			m_action.setChecked(windowState() & Qt::WindowFullScreen);
			const Qt::WindowFlags flags = m_player->windowFlags();
			if (windowState() & Qt::WindowFullScreen)
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
	return Phonon::VideoWidget::event(e);
}

void MediaVideoWidget::timerEvent(QTimerEvent *e)
{
	if (e->timerId() == m_timer.timerId())
	{
		// let's store the cursor shape
#ifndef QT_NO_CURSOR
		setCursor(Qt::BlankCursor);
#endif
	}
	Phonon::VideoWidget::timerEvent(e);
}

frmMultiMedia::frmMultiMedia(QWidget *parent) :
	QWidget(parent),
	nextEffect(0), settingsDialog(0), ui(0)
{
	fGrl = new Funciones;

	setWindowTitle(tr("Reproductor Multimedia"));
	setWindowIcon(QIcon(fGrl->Theme() +"img16/multimedia.png"));
	setContextMenuPolicy(Qt::CustomContextMenu);

	total_media = -1;
	id_media    = -1;

	m_MediaObject = new Phonon::MediaObject(this);
	m_AudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	m_videoWidget = new MediaVideoWidget(this);
	m_videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	QSize buttonSize(26, 26);

	playIcon  = QIcon(fGrl->Theme() +"img16/mp_play.png");
	pauseIcon = QIcon(fGrl->Theme() +"img16/mp_pausa.png");
	btnPlayPausa = new QPushButton(this);
	btnPlayPausa->setIcon(playIcon);
	btnPlayPausa->setMinimumSize(buttonSize);
	btnPlayPausa->setMaximumSize(buttonSize);
	btnPlayPausa->setEnabled(false);

	QPushButton *btnStop = new QPushButton(this);
	btnStop->setIcon(QIcon(fGrl->Theme() +"img16/mp_stop.png"));
	btnStop->setMinimumSize(buttonSize);
	btnStop->setMaximumSize(buttonSize);

	btnAnterior = new QPushButton(this);
	btnAnterior->setIcon(QIcon(fGrl->Theme() +"img16/mp_anterior.png"));
	btnAnterior->setMinimumSize(buttonSize);
	btnAnterior->setMaximumSize(buttonSize);
	btnAnterior->setEnabled(false);

	btnSiguiente = new QPushButton(this);
	btnSiguiente->setIcon(QIcon(fGrl->Theme() +"img16/mp_siguiente.png"));
	btnSiguiente->setMinimumSize(buttonSize);
	btnSiguiente->setMaximumSize(buttonSize);
	btnSiguiente->setEnabled(false);

	QPushButton *btnListaMedia = new QPushButton(this);
	btnListaMedia->setIcon(QIcon(fGrl->Theme() +"img16/categorias.png"));
	btnListaMedia->setMinimumSize(buttonSize);
	btnListaMedia->setMaximumSize(buttonSize);
	btnListaMedia->setCheckable(true);

	QPalette palette, bpal;
	QColor arrowcolor = bpal.buttonText().color();
	if (arrowcolor == Qt::black)
		arrowcolor = QColor(80, 80, 80);
	bpal.setBrush(QPalette::ButtonText, arrowcolor);

	QPushButton *btnAbrir = new QPushButton(this);
	btnAbrir->setIcon(QIcon(fGrl->Theme() +"img16/mp_eject.png"));
	btnAbrir->setMinimumSize(40, 26);
	btnAbrir->setMaximumSize(40, 26);
	btnAbrir->setPalette(bpal);

	slider = new Phonon::SeekSlider(this);
	slider->setMediaObject(m_MediaObject);
	volume = new Phonon::VolumeSlider(m_AudioOutput);
	volume->setFixedWidth(120);

	palette.setBrush(QPalette::WindowText, Qt::white);

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);

	info = new QLabel(this);
	info->setMinimumHeight(70);
	info->setAcceptDrops(false);
	info->setMargin(2);
	info->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	info->setLineWidth(2);
	info->setAutoFillBackground(true);
	info->setPalette(palette);
	info->setStyleSheet("background-color: rgb(0, 0, 0);");
	info->setText("<center>"+ tr("Ningún archivo multimedia") +"</center>");

	vLayout->addWidget(info);

	QVBoxLayout *videoLayout = new QVBoxLayout();
	videoLayout->addWidget(m_videoWidget);
	videoLayout->setContentsMargins(0, 0, 0, 0);
	videoLayout->setSpacing(0);
	m_videoWindow.setLayout(videoLayout);
	m_videoWindow.setMinimumSize(100, 100);
	vLayout->addWidget(&m_videoWindow);
	m_videoWindow.hide();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(4, 2, 4, 2);
	layout->setSpacing(4);
	layout->addWidget(btnPlayPausa);
	layout->addWidget(btnStop);
	layout->addWidget(btnAnterior);
	layout->addWidget(btnSiguiente);
	layout->addWidget(btnAbrir);
	layout->addWidget(btnListaMedia);
	layout->addStretch();
	layout->addWidget(volume);

	lwMedia = new QListWidget(this);
	lwMedia->setMinimumHeight(80);
	lwMedia->setMaximumHeight(80);
	lwMedia->setVisible(false);
	connect(lwMedia, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(lwMedia_DoubleClicked(QListWidgetItem*)));

	QVBoxLayout *buttonPanelLayout = new QVBoxLayout();
		QVBoxLayout *sliderLayout = new QVBoxLayout();
		sliderLayout->setContentsMargins(6, 6, 6, 6);
		sliderLayout->setSpacing(4);
		sliderLayout->addWidget(lwMedia);
		sliderLayout->addWidget(slider);
	buttonPanelLayout->addLayout(sliderLayout);
	buttonPanelLayout->addLayout(layout);

	lb_tiempo = new QLabel(this);
	lb_tiempo->setPalette(palette);

	lb_buffer = new QLabel(this);
	lb_buffer->setPalette(palette);

	QLabel *lb_icon = new QLabel(this);
	lb_icon->setFixedWidth(18);
	lb_icon->setPixmap(QPixmap(fGrl->Theme() +"img16/multimedia.png"));

	lb_status = new QLabel(this);
	lb_status->setPalette(palette);

	QWidget *statusPanel = new QWidget(this);
	statusPanel->setMinimumHeight(24);
	statusPanel->setStyleSheet("background-color: rgb(0, 0, 0);");
		QHBoxLayout *statusLayout = new QHBoxLayout();
		statusLayout->addWidget(lb_icon);
		statusLayout->addWidget(lb_status);
		statusLayout->addWidget(lb_tiempo);
		statusLayout->addWidget(lb_buffer);
		statusLayout->setContentsMargins(4, 0, 4, 0);
		statusLayout->setSpacing(0);
		statusLayout->setStretch(1, 1);
	statusPanel->setLayout(statusLayout);

	buttonPanelLayout->addWidget(statusPanel);
	buttonPanelLayout->setContentsMargins(0, 0, 0, 0);
	buttonPanelLayout->setSpacing(0);

	QWidget *buttonPanelWidget = new QWidget(this);
		buttonPanelWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		buttonPanelWidget->setLayout(buttonPanelLayout);
	vLayout->addWidget(buttonPanelWidget);

	setLayout(vLayout);

// Create menu bar:
	fileMenu = new QMenu(this);
	QAction *openFileAction = fileMenu->addAction(tr("&Abrir..."));
	QAction *openUrlAction = fileMenu->addAction(tr("Abrir &url..."));
	QAction *const openLinkAction = fileMenu->addAction(tr("Abrir archivo &RAM..."));
	connect(openLinkAction, SIGNAL(triggered(bool)), this, SLOT(openRamFile()));

	fileMenu->addSeparator();
	QMenu *aspectMenu = fileMenu->addMenu(tr("&Aspect ratio"));
	QActionGroup *aspectGroup = new QActionGroup(aspectMenu);
	connect(aspectGroup, SIGNAL(triggered(QAction*)), this, SLOT(aspectChanged(QAction*)));
	aspectGroup->setExclusive(true);
	QAction *aspectActionAuto = aspectMenu->addAction(tr("Auto"));
	aspectActionAuto->setCheckable(true);
	aspectActionAuto->setChecked(true);
	aspectGroup->addAction(aspectActionAuto);
	QAction *aspectActionScale = aspectMenu->addAction(tr("Scale"));
	aspectActionScale->setCheckable(true);
	aspectGroup->addAction(aspectActionScale);
	QAction *aspectAction16_9 = aspectMenu->addAction("16/9");
	aspectAction16_9->setCheckable(true);
	aspectGroup->addAction(aspectAction16_9);
	QAction *aspectAction4_3 = aspectMenu->addAction("4/3");
	aspectAction4_3->setCheckable(true);
	aspectGroup->addAction(aspectAction4_3);

	QMenu *scaleMenu = fileMenu->addMenu(tr("&Scale mode"));
	QActionGroup *scaleGroup = new QActionGroup(scaleMenu);
	connect(scaleGroup, SIGNAL(triggered(QAction*)), this, SLOT(scaleChanged(QAction*)));
	scaleGroup->setExclusive(true);
	QAction *scaleActionFit = scaleMenu->addAction(tr("Fit in view"));
	scaleActionFit->setCheckable(true);
	scaleActionFit->setChecked(true);
	scaleGroup->addAction(scaleActionFit);
	QAction *scaleActionCrop = scaleMenu->addAction(tr("Scale and crop"));
	scaleActionCrop->setCheckable(true);
	scaleGroup->addAction(scaleActionCrop);

	m_fullScreenAction = fileMenu->addAction(tr("Pantalla completa"));
	m_fullScreenAction->setCheckable(true);
	m_fullScreenAction->setEnabled(false); // enabled by hasVideoChanged
	bool b = connect(m_fullScreenAction, SIGNAL(toggled(bool)), m_videoWidget, SLOT(setFullScreen(bool)));
	Q_ASSERT(b);
	b = connect(m_videoWidget, SIGNAL(fullScreenChanged(bool)), m_fullScreenAction, SLOT(setChecked(bool)));
	Q_ASSERT(b);
	Q_UNUSED(b);

	fileMenu->addSeparator();
	QAction *settingsAction = fileMenu->addAction(tr("&Opciones..."));

// Setup signal connections:
	btnAbrir->setMenu(fileMenu);
	connect(btnPlayPausa , SIGNAL(clicked()), this, SLOT(playPause()));
	connect(btnStop      , SIGNAL(clicked()), this, SLOT(mpStop()));
	connect(btnAnterior  , SIGNAL(clicked()), this, SLOT(rewind()));
	connect(btnSiguiente , SIGNAL(clicked()), this, SLOT(forward()));
	connect(btnListaMedia, SIGNAL(toggled(bool)), lwMedia, SLOT(setVisible(bool)));

	connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(showSettingsDialog()));
	connect(openUrlAction , SIGNAL(triggered(bool)), this, SLOT(openUrl()));
	connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
	connect(m_videoWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
	connect(m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(updateInfo()));
	connect(m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime()));
	connect(m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime()));
	connect(m_MediaObject, SIGNAL(finished()), this, SLOT(finished()));
	connect(m_MediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)));
	connect(m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
	connect(m_MediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(hasVideoChanged(bool)));

	m_audioOutputPath = Phonon::createPath(m_MediaObject, m_AudioOutput);
	Phonon::createPath(m_MediaObject, m_videoWidget);

	resize(minimumSizeHint());

	grlDir.Home = fGrl->GRlidaHomePath();
	formatsVideo.clear();
	formatsMusic.clear();
	QSettings settings(grlDir.Home +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("OpcMultimedia");
		formatsVideo = settings.value("FormatsVideo", QStringList() << "*.avi" << "*.mkv" << "*.mov" << "*.mp4" << "*.mpeg" << "*.mpg" << "*.wmv").toStringList();
		formatsMusic = settings.value("FormatsMusic", QStringList() << "*.mp3" << "*.ogg" << "*.wav" << "*.wma").toStringList();
	settings.endGroup();
}

frmMultiMedia::~frmMultiMedia()
{
	delete fGrl;
}

void frmMultiMedia::closeEvent(QCloseEvent *e)
{
	m_MediaObject->stop();

	delete m_MediaObject;
	delete m_AudioOutput;
	delete m_videoWidget;

	e->accept();
}

void frmMultiMedia::lwMedia_DoubleClicked(QListWidgetItem *item)
{
	if( item )
	{
		id_media = lwMedia->row(item);
		setMediaId(id_media);
	}
}

void frmMultiMedia::addItemList(QString fileName)
{
	stFileInfo f_info = fGrl->getInfoFile( fileName );
	QListWidgetItem *Item = new QListWidgetItem(lwMedia);
	QString img_ext = "";
	if( formatsVideo.contains("*"+ f_info.Ext, Qt::CaseInsensitive) )
		img_ext = "img_tv2x.png";
	if( formatsMusic.contains("*"+ f_info.Ext, Qt::CaseInsensitive) )
		img_ext = "img_audio.png";
	if( img_ext.isEmpty() )
		img_ext = "multimedia.png";
	Item->setIcon(QIcon(fGrl->Theme() +"img16/"+ img_ext));
	Item->setText( f_info.Name );
	lista_media.append( fileName );
}

void frmMultiMedia::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
//	Q_UNUSED(oldstate);
	if (oldstate == Phonon::LoadingState)
	{
		QRect videoHintRect = QRect(QPoint(0, 0), m_videoWindow.sizeHint());
		QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
		if (!m_smallScreen)
		{
			if (m_MediaObject->hasVideo())
			{
			// Flush event que so that sizeHint takes the
			// recently shown/hidden m_videoWindow into account:
				qApp->processEvents();
				resize(sizeHint());
			} else
				resize(minimumSize());
		}
	}

	switch (newstate)
	{
		case Phonon::ErrorState:
			if (m_MediaObject->errorType() == Phonon::FatalError)
				btnPlayPausa->setEnabled(false);
			else
				m_MediaObject->pause();
			QMessageBox::warning(this, "Reproductor Multimedia", m_MediaObject->errorString(), QMessageBox::Close);
			break;
		case Phonon::StoppedState:
			m_videoWidget->setFullScreen(false);
			break;
			// Fall through
		case Phonon::PausedState:
			btnPlayPausa->setIcon(playIcon);
			if (m_MediaObject->currentSource().type() != Phonon::MediaSource::Invalid)
				btnPlayPausa->setEnabled(true);
			else
				btnPlayPausa->setEnabled(false);
			break;
		case Phonon::PlayingState:
			btnPlayPausa->setEnabled(true);
			btnPlayPausa->setIcon(pauseIcon);
			if (m_MediaObject->hasVideo())
				m_videoWindow.show();
			break;
			// Fall through
		case Phonon::BufferingState:
		//	btnAnterior->setEnabled(true);
			break;
		case Phonon::LoadingState:
		//	btnAnterior->setEnabled(false);
			break;
	}
}

void frmMultiMedia::initSettingsDialog()
{
	settingsDialog = new QDialog(this);
	ui = new Ui_multimedia_setup();
	ui->setupUi(settingsDialog);

	connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
	connect(ui->hueSlider, SIGNAL(valueChanged(int)), this, SLOT(setHue(int)));
	connect(ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(setSaturation(int)));
	connect(ui->contrastSlider , SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
	connect(ui->aspectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAspect(int)));
	connect(ui->scalemodeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setScale(int)));

	ui->brightnessSlider->setValue(int(m_videoWidget->brightness() * SLIDER_RANGE));
	ui->hueSlider->setValue(int(m_videoWidget->hue() * SLIDER_RANGE));
	ui->saturationSlider->setValue(int(m_videoWidget->saturation() * SLIDER_RANGE));
	ui->contrastSlider->setValue(int(m_videoWidget->contrast() * SLIDER_RANGE));
	ui->aspectCombo->setCurrentIndex(m_videoWidget->aspectRatio());
	ui->scalemodeCombo->setCurrentIndex(m_videoWidget->scaleMode());
	connect(ui->effectButton, SIGNAL(clicked()), this, SLOT(configureEffect()));

#ifdef Q_WS_X11
	//Cross fading is not currently implemented in the GStreamer backend
	ui->crossFadeSlider->setVisible(false);
	ui->crossFadeLabel->setVisible(false);
	ui->crossFadeLabel1->setVisible(false);
	ui->crossFadeLabel2->setVisible(false);
	ui->crossFadeLabel3->setVisible(false);
#endif
	ui->crossFadeSlider->setValue((int)(2 * m_MediaObject->transitionTime() / 1000.0f));

	// Insert audio devices:
	QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	for (int i=0; i<devices.size(); i++)
	{
		QString itemText = devices[i].name();
		if (!devices[i].description().isEmpty())
			itemText += QString::fromLatin1(" (%1)").arg(devices[i].description());
		ui->deviceCombo->addItem(itemText);
		if (devices[i] == m_AudioOutput->outputDevice())
			ui->deviceCombo->setCurrentIndex(i);
	}

	// Insert audio effects:
	ui->audioEffectsCombo->addItem(tr("<sin efectos>"));
	QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
	Phonon::Effect *currEffect = currEffects.size() ? currEffects[0] : 0;
	QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	for (int i=0; i<availableEffects.size(); i++)
	{
		ui->audioEffectsCombo->addItem(availableEffects[i].name());
		if (currEffect && availableEffects[i] == currEffect->description())
			ui->audioEffectsCombo->setCurrentIndex(i+1);
	}
	connect(ui->audioEffectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(effectChanged()));
}

void frmMultiMedia::setVolume(qreal volume)
{
	m_AudioOutput->setVolume(volume);
}

void frmMultiMedia::setSmallScreen(bool smallScreen)
{
	m_smallScreen = smallScreen;
}

void frmMultiMedia::effectChanged()
{
	int currentIndex = ui->audioEffectsCombo->currentIndex();
	if (currentIndex)
	{
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
		Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

		QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
		Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;

	// Deleting the running effect will stop playback, it is deleted when removed from path
		if (nextEffect && !(currentEffect && (currentEffect->description().name() == nextEffect->description().name())))
			delete nextEffect;

		nextEffect = new Phonon::Effect(chosenEffect);
	}
	ui->effectButton->setEnabled(currentIndex);
}

void frmMultiMedia::showSettingsDialog()
{
	const bool hasPausedForDialog = playPauseForDialog();

	if (!settingsDialog)
		initSettingsDialog();

	float oldBrightness = m_videoWidget->brightness();
	float oldHue = m_videoWidget->hue();
	float oldSaturation = m_videoWidget->saturation();
	float oldContrast = m_videoWidget->contrast();
	Phonon::VideoWidget::AspectRatio oldAspect = m_videoWidget->aspectRatio();
	Phonon::VideoWidget::ScaleMode oldScale = m_videoWidget->scaleMode();
	int currentEffect = ui->audioEffectsCombo->currentIndex();
	settingsDialog->exec();

	if (settingsDialog->result() == QDialog::Accepted)
	{
		m_MediaObject->setTransitionTime((int)(1000 * float(ui->crossFadeSlider->value()) / 2.0f));
		QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
		m_AudioOutput->setOutputDevice(devices[ui->deviceCombo->currentIndex()]);
		QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();

		if (ui->audioEffectsCombo->currentIndex() > 0)
		{
			Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
			if (!currentEffect || currentEffect->description() != nextEffect->description())
			{
				foreach(Phonon::Effect *effect, currEffects)
				{
					m_audioOutputPath.removeEffect(effect);
					delete effect;
				}
				m_audioOutputPath.insertEffect(nextEffect);
			}
		} else {
			foreach(Phonon::Effect *effect, currEffects)
			{
				m_audioOutputPath.removeEffect(effect);
				delete effect;
				nextEffect = 0;
			}
		}
	} else {
	// Restore previous settings
		m_videoWidget->setBrightness(oldBrightness);
		m_videoWidget->setSaturation(oldSaturation);
		m_videoWidget->setHue(oldHue);
		m_videoWidget->setContrast(oldContrast);
		m_videoWidget->setAspectRatio(oldAspect);
		m_videoWidget->setScaleMode(oldScale);
		ui->audioEffectsCombo->setCurrentIndex(currentEffect);
	}

	if (hasPausedForDialog)
		m_MediaObject->play();
}

void frmMultiMedia::configureEffect()
{
	if (!nextEffect)
		return;

	QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
	const QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
	if (ui->audioEffectsCombo->currentIndex() > 0)
	{
		Phonon::EffectDescription chosenEffect = availableEffects[ui->audioEffectsCombo->currentIndex() - 1];

		QDialog effectDialog;
		effectDialog.setWindowTitle(tr("Configurar el efecto"));
		QVBoxLayout *topLayout = new QVBoxLayout(&effectDialog);

		QLabel *description = new QLabel("<b>"+ tr("Descripción") +":</b><br>"+ chosenEffect.description(), &effectDialog);
		description->setWordWrap(true);
		topLayout->addWidget(description);

		QScrollArea *scrollArea = new QScrollArea(&effectDialog);
		topLayout->addWidget(scrollArea);

		QVariantList savedParamValues;
		foreach(Phonon::EffectParameter param, nextEffect->parameters())
			savedParamValues << nextEffect->parameterValue(param);

		QWidget *scrollWidget = new Phonon::EffectWidget(nextEffect);
		scrollWidget->setMinimumWidth(320);
		scrollWidget->setContentsMargins(10, 10, 10,10);
		scrollArea->setWidget(scrollWidget);

		QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
		connect(bbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
		connect(bbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
		topLayout->addWidget(bbox);

		effectDialog.exec();

		if (effectDialog.result() != QDialog::Accepted)
		{
			// we need to restore the parameters values
			int currentIndex = 0;
			foreach(Phonon::EffectParameter param, nextEffect->parameters())
				nextEffect->setParameterValue(param, savedParamValues.at(currentIndex++));
		}
	}
}

void frmMultiMedia::playPause()
{
	if (m_MediaObject->state() == Phonon::PlayingState)
		m_MediaObject->pause();
	else {
		if (m_MediaObject->currentTime() == m_MediaObject->totalTime())
			m_MediaObject->seek(0);
		m_MediaObject->play();
	}
}

void frmMultiMedia::mpStop()
{
	m_MediaObject->stop();
	btnPlayPausa->setIcon(playIcon);
}

void frmMultiMedia::open(int id, QStringList lista)
{
	lista_media.clear();
	const int listSize = lista.size();
	for (int i = 0; i < listSize; ++i)
		addItemList( lista.at(i) );
	total_media = lista_media.size()-1;
	setMediaId(id);
	btnAnterior->setEnabled(total_media > 0);
	btnSiguiente->setEnabled(total_media > 0);
}

void frmMultiMedia::setMediaId(int id)
{
	if( id > -1 )
	{
		id_media = id;
		lwMedia->setCurrentRow(id_media);
		m_MediaObject->clearQueue();
		QString fileName = lista_media.at(id);
		setWindowTitle(fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1));
		QDir dir(fileName);
		if( dir.exists() )
			m_MediaObject->setCurrentSource(Phonon::MediaSource(fileName));
		else
			m_MediaObject->setCurrentSource(Phonon::MediaSource(QUrl::fromEncoded(fileName.toUtf8())));
		m_MediaObject->play();
	}
}

bool frmMultiMedia::playPauseForDialog()
{
// If we're running on a small screen, we want to pause the video when
// popping up dialogs. We neither want to tamper with the state if the
// user has paused.
	if (m_smallScreen && m_MediaObject->hasVideo())
	{
		if (Phonon::PlayingState == m_MediaObject->state())
		{
			m_MediaObject->pause();
			return true;
		}
	}
	return false;
}

void frmMultiMedia::openFile()
{
	const bool hasPausedForDialog = playPauseForDialog();

	QStringList fileNames = QFileDialog::getOpenFileNames(this, QString(), QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

	if (hasPausedForDialog)
		m_MediaObject->play();

	total_media = lista_media.size()-1;
	if (fileNames.size() > 0)
	{
		for (int i=0; i < fileNames.size(); i++)
			addItemList( fileNames.at(i) );
		setMediaId(total_media+1);
	}
	total_media = lista_media.size()-1;
	btnAnterior->setEnabled(total_media > 0);
	btnSiguiente->setEnabled(total_media > 0);
}

void frmMultiMedia::bufferStatus(int percent)
{
	if (percent == 100)
		lb_buffer->setText(QString());
	else {
		QString str = QString::fromLatin1("(%1%)").arg(percent);
		lb_buffer->setText(str);
	}
}

void frmMultiMedia::setSaturation(int val)
{
	m_videoWidget->setSaturation(val / qreal(SLIDER_RANGE));
}

void frmMultiMedia::setHue(int val)
{
	m_videoWidget->setHue(val / qreal(SLIDER_RANGE));
}

void frmMultiMedia::setAspect(int val)
{
	m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(val));
}

void frmMultiMedia::setScale(int val)
{
	m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(val));
}

void frmMultiMedia::setBrightness(int val)
{
	m_videoWidget->setBrightness(val / qreal(SLIDER_RANGE));
}

void frmMultiMedia::setContrast(int val)
{
	m_videoWidget->setContrast(val / qreal(SLIDER_RANGE));
}

void frmMultiMedia::updateInfo()
{
	int maxLength = 30;
	QString font = "<font color=#ffeeaa>";

	QMap <QString, QString> metaData = m_MediaObject->metaData();
	QString trackArtist = metaData.value("ARTIST");
	if (trackArtist.length() > maxLength)
		trackArtist = trackArtist.left(maxLength) +"...";

	QString trackTitle = metaData.value("TITLE");
	int trackBitrate = metaData.value("BITRATE").toInt();

	QString fileName;
	if (m_MediaObject->currentSource().type() == Phonon::MediaSource::Url)
		fileName = m_MediaObject->currentSource().url().toString();
	else {
		fileName = m_MediaObject->currentSource().fileName();
		fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
		if (fileName.length() > maxLength)
			fileName = fileName.left(maxLength) +"...";
	}

	QString title;
	if (!trackTitle.isEmpty())
	{
		if (trackTitle.length() > maxLength)
			trackTitle = trackTitle.left(maxLength) +"...";
		title = "Title: "+ font + trackTitle +"</font><br>";
	}
	else if (!fileName.isEmpty())
	{
		if (fileName.length() > maxLength)
			fileName = fileName.left(maxLength) +"...";
		title = font + fileName +"</font><br>";
		if (m_MediaObject->currentSource().type() == Phonon::MediaSource::Url)
			title.prepend("Url: ");
		else
			title.prepend("File: ");
	}

	QString artist;
	if (!trackArtist.isEmpty())
		artist = "Artist:  "+ font + trackArtist +"</font>";

	QString bitrate;
	if (trackBitrate != 0)
		bitrate = "<br>Bitrate:  "+ font + QString::number(trackBitrate/1000) +"kbit</font>";

	info->setText(title + artist + bitrate);
	lb_status->setText(" "+ fGrl->IntToStr(id_media+1) +" "+ tr("de") +" "+ fGrl->IntToStr(total_media+1) +" ");
}

void frmMultiMedia::updateTime()
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
			timeString += " / "+ stopTime.toString(timeFormat);
	}
	lb_tiempo->setText(timeString);
}

void frmMultiMedia::rewind()
{
	id_media--;
	if( id_media < 0 )
	{
		id_media = total_media;
		setMediaId(id_media);
	} else
		setMediaId(id_media);
}

void frmMultiMedia::forward()
{
	id_media++;
	if( id_media > total_media )
	{
		id_media = 0;
		setMediaId(id_media);
	} else
		setMediaId(id_media);
}

void frmMultiMedia::openUrl()
{
	QSettings settings;
	settings.beginGroup(QLatin1String("BrowserMainWindow"));
	QString sourceURL = settings.value("location").toString();
	bool ok = false;
	sourceURL = QInputDialog::getText(this, tr("Abrir dirección"), tr("Por favor, introduzca una dirección válida aquí:"), QLineEdit::Normal, sourceURL, &ok);
	if (ok && !sourceURL.isEmpty())
	{
		total_media = lista_media.size()-1;
		addItemList( sourceURL );
		setMediaId(total_media+1);
		total_media = lista_media.size()-1;
		settings.setValue("location", sourceURL);
	}
}

void frmMultiMedia::openRamFile()
{
	QSettings settings;
	settings.beginGroup(QLatin1String("BrowserMainWindow"));

	const QStringList fileNameList(QFileDialog::getOpenFileNames(this, QString(), settings.value("openRamFile").toString(), QLatin1String("RAM files (*.ram)")));

	if (fileNameList.isEmpty())
		return;

	QFile linkFile;
	QList<QUrl> list;
	QByteArray sourceURL;
	for (int i = 0; i < fileNameList.count(); i++)
	{
		linkFile.setFileName(fileNameList[i]);
		if (linkFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			while (!linkFile.atEnd())
			{
				sourceURL = linkFile.readLine().trimmed();
				if (!sourceURL.isEmpty())
				{
					const QUrl url(QUrl::fromEncoded(sourceURL));
					if (url.isValid())
						list.append(url);
				}
			}
			linkFile.close();
		}
	}

	if (!list.isEmpty())
	{
		total_media = lista_media.size()-1;
		for (int i = 0; i < list.count(); i++)
			addItemList( list.at(i).toString() );
		setMediaId(total_media+1);
		m_MediaObject->play();
	}
	total_media = lista_media.size()-1;
	btnAnterior->setEnabled(total_media > 0);
	btnSiguiente->setEnabled(total_media > 0);
	settings.setValue("openRamFile", fileNameList[0]);
}

void frmMultiMedia::finished()
{
	//
}

void frmMultiMedia::showContextMenu(const QPoint &p)
{
	fileMenu->popup(m_videoWidget->isFullScreen() ? p : mapToGlobal(p));
}

void frmMultiMedia::scaleChanged(QAction *act)
{
	if (act->text() == tr("Scale and crop"))
		m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	else
		m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
}

void frmMultiMedia::aspectChanged(QAction *act)
{
	if (act->text() == "16/9")
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	else if (act->text() == tr("Scale"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	else if (act->text() == "4/3")
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	else
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
}

void frmMultiMedia::hasVideoChanged(bool bHasVideo)
{
	info->setVisible(!bHasVideo);
	m_videoWindow.setVisible(bHasVideo);
	m_fullScreenAction->setEnabled(bHasVideo);
}
