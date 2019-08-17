/*
 *
 * GR-lida by Monthy
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

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QStyleFactory>
#include <QScriptEngine>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>

#include "grlida_opciones.h"
#include "grlida_list_icon_cfg.h"
#include "grlida_buscar.h"
#include "ui_opciones.h"

frmOpciones::frmOpciones(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmOpciones)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql    = m_sql;
	grlCfg = m_cfg;
	grlCfg.isChangedToolbarBigIcon = false;
	grlCfg.isChangedTheme          = false;
	grlCfg.isChangedFormatoFecha   = false;
	grlCfg.isChangedCategoria      = false;
	grlCfg.isChangedEmuList        = false;
	grlCfg.isChangedIdioma         = false;
	grlCfg.isChangedListDOSBox     = false;
	grlCfg.isChangedShortcut       = false;
	grlCfg.isChangedFavorito       = false;

	grlDir.Home       = fGrl->dirApp();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/";
	grlDir.Idiomas    = grlDir.Home +"idiomas/";
	grlDir.Scripts    = grlDir.Home +"scripts/";
	grlDir.Smiles     = grlDir.Home +"smiles/";
	grlDir.Themes     = grlDir.Home +"themes/";

	IdiomaSelect = grlCfg.IdiomaSelect;
	ThemeSelect  = grlCfg.NameDirTheme;

	createWidgets();
	cargarConfig();
	setTheme();
	cargarListaThemes();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmOpciones::~frmOpciones()
{
	delete grl_picflow;
	delete cbxCatEmuShow;
	delete fGrl;
	delete ui;
}

void frmOpciones::closeEvent(QCloseEvent *event)
{
	emit on_btnOk_clicked();
	event->accept();
}

bool frmOpciones::eventFilter(QObject *object, QEvent *event)
{
	if (object == ui->txtShortcutKeySequence)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

			int keyInt = keyEvent->key();
			Qt::Key key = static_cast<Qt::Key>(keyInt);
			if(key == Qt::Key_unknown)
			{
				// key desconocida de una macro probablemente
				return false;
			}

			// El usuario solo ha hecho clic y solo las teclas especiales Ctrl, Shift, Alt, Meta.
			if(key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta)
			{
				// Un solo clic de la tecla especial: Ctrl, Shift, Alt o Meta
				return false;
			}

			// Comprueba si hay una combinación de clics de usuario
			Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
			QString keyText = keyEvent->text();
			// Si el keyText está vacío entonces es una tecla especial como F1, F5, ...

			QList<Qt::Key> modifiersList;
			if (modifiers & Qt::ShiftModifier)
				keyInt += Qt::SHIFT;
			if (modifiers & Qt::ControlModifier)
				keyInt += Qt::CTRL;
			if (modifiers & Qt::AltModifier)
				keyInt += Qt::ALT;
			if (modifiers & Qt::MetaModifier)
				keyInt += Qt::META;
			if (list_key_sequence.size() > 3)
				list_key_sequence.clear();

			list_key_sequence << QKeySequence(keyInt).toString(QKeySequence::NativeText);

			ui->txtShortcutKeySequence->setText(list_key_sequence.join(", "));
		}
	}

	return QDialog::eventFilter(object, event);
}

void frmOpciones::createWidgets()
{
	cbxCatEmuShow = new QCheckComboBox(ui->gBox_categorias);
	cbxCatEmuShow->setMinimumHeight(24);
	ui->gridLayout_categorias->addWidget(cbxCatEmuShow, 2, 1, 1, 1);

	connect(cbxCatEmuShow, SIGNAL(activated(int)), this, SLOT(cbxCatEmuShow_activated(int)));

	grl_picflow = new GrlPicFlow(ui->tab_2);
	grl_picflow->setFocusPolicy(Qt::StrongFocus);
	grl_picflow->setFocus(Qt::OtherFocusReason);
	grl_picflow->setShowLinesPos(true);
	grl_picflow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	ui->verticalLayout_PictureFlow->addWidget(grl_picflow);
}

void frmOpciones::cargarConfig()
{
	regexp.setPatternSyntax(QRegExp::RegExp);

	ui->lwOpciones->setCurrentRow(grlCfg.LastTabOptSelect);

// Opciones generales ----------------
	ui->txtDirBaseGames->setText(grlCfg.DirBaseGames);
	fGrl->cargarScriptsComboBox(ui->cbxScriptURL, grlDir.Scripts, "img16/", grlCfg.url_xmldb);
	ui->txtFormatoFecha->setText(grlCfg.FormatoFecha);

// Otras opciones --------------------
	ui->chkComprobarVersionGrl->setChecked(grlCfg.chkVersion);
	ui->chkShowNext->setChecked(grlCfg.Primeravez);
	ui->chkOpenPdfExternal->setChecked(grlCfg.OpenPdfExternal);

// Actualizaciones -------------------
	ui->cbxUpdateInterval->clear();
	ui->cbxUpdateInterval->addItem(QIcon(fGrl->theme() +"img16/fecha.png"), "Cada veinticuatro horas",  1);
	ui->cbxUpdateInterval->addItem(QIcon(fGrl->theme() +"img16/fecha.png"), "Semanalmente",  7);
	ui->cbxUpdateInterval->addItem(QIcon(fGrl->theme() +"img16/fecha.png"), "Mensualmente", 30);

	int idx_update_interval = ui->cbxUpdateInterval->findData(grlCfg.chkUpdateInterval, Qt::UserRole, Qt::MatchExactly);
	ui->cbxUpdateInterval->setCurrentIndex(idx_update_interval);
	ui->chkUpdateThemes->setChecked(grlCfg.chkUpdateThemes);

// ScummVM ---------------------------
	ui->chkScummVMDisp->setChecked(grlCfg.ScummVMDisp);
	ui->txtSvmPath->setText(grlCfg.DirScummVM);

// DOSBox ----------------------------
	ui->chkDOSBoxDisp->setChecked(grlCfg.DOSBoxDisp);
	regexp.setPattern("[a-z-_0-9]+");
	ui->txtDbxDato->setValidator(new QRegExpValidator(regexp, ui->txtDbxDato));
	ui->chkDbxSaveConfFile->setChecked(grlCfg.DOSBoxSaveConfFile);
	ui->chkSvmSaveConfFile->setChecked(grlCfg.ScummVMSaveConfFile);
	emit on_btnDbxCancel_clicked();

	ui->cbxDbxVersion->clear();
	ui->cbxDbxVersion->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), "0.74-2", "0.74-2");
	ui->cbxDbxVersion->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), "0.74", "0.74");
	ui->cbxDbxVersion->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), "0.73", "0.73");
	ui->cbxDbxVersion->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), "0.72", "0.72");
	ui->cbxDbxVersion->setCurrentIndex(0);

	ui->cbxDbxImg->clear();
	ui->cbxDbxImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxDbxImg, fGrl->theme(), "img32/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	emit on_btnDbxCancel_clicked();

	ui->twDOSBox->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
//	ui->twDOSBox->headerItem()->setIcon(1, QIcon(fGrl->theme() +"img16/bullet_black.png"));
//	ui->twDOSBox->headerItem()->setIcon(2, QIcon(fGrl->theme() +"img16/bullet_black.png"));
//	ui->twDOSBox->headerItem()->setIcon(3, QIcon(fGrl->theme() +"img16/bullet_black.png"));
//	ui->twDOSBox->headerItem()->setIcon(4, QIcon(fGrl->theme() +"img16/bullet_black.png"));
	ui->twDOSBox->headerItem()->setTextAlignment(2, Qt::AlignCenter);
	ui->twDOSBox->headerItem()->setTextAlignment(3, Qt::AlignCenter);
	ui->twDOSBox->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twDOSBox->header()->setSectionsMovable(false);
	ui->twDOSBox->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twDOSBox->header()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui->twDOSBox->header()->setSectionResizeMode(2, QHeaderView::Interactive);
	ui->twDOSBox->header()->setSectionResizeMode(3, QHeaderView::Interactive);
	ui->twDOSBox->header()->setSectionResizeMode(4, QHeaderView::Interactive);
#else
	ui->twDOSBox->header()->setMovable(false);
	ui->twDOSBox->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twDOSBox->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDOSBox->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twDOSBox->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twDOSBox->header()->setResizeMode(4, QHeaderView::Interactive);
#endif
	ui->twDOSBox->setColumnWidth(1, 90);
	ui->twDOSBox->setColumnWidth(2, 50);
	ui->twDOSBox->setColumnWidth(3, 50);
	ui->twDOSBox->setColumnWidth(4, 50);

	ui->twDOSBox->clear();
	fGrl->cargarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx, "|");

// VDMSound --------------------------
	ui->chkVDMSoundDisp->setChecked(grlCfg.VDMSoundDisp);

// Otros emuladores ------------------
	regexp.setPattern("[a-z-_0-9]+");
	ui->txtEmuDato->setValidator(new QRegExpValidator(regexp, ui->txtEmuDato));

	ui->cbxEmuImg->clear();
	ui->cbxEmuImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxEmuImg, fGrl->theme(), "img32/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxEmuImg->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxEmuImg, fGrl->theme(), "img32/", "", grlCfg.FormatsImage.join(";"));

	int row = ui->cbxEmuImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxEmuImg->setCurrentIndex(row);

	ui->twEmus->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
//	ui->twEmus->headerItem()->setIcon(1, QIcon(fGrl->theme() +"img16/bullet_black.png"));
//	ui->twEmus->headerItem()->setIcon(2, QIcon(fGrl->theme() +"img16/bullet_black.png"));
	ui->twEmus->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twEmus->header()->setSectionsMovable(false);
	ui->twEmus->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twEmus->header()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui->twEmus->header()->setSectionResizeMode(2, QHeaderView::Interactive);
#else
	ui->twEmus->header()->setMovable(false);
	ui->twEmus->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twEmus->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twEmus->header()->setResizeMode(3, QHeaderView::Interactive);
#endif
//	ui->twEmus->setColumnWidth(0, 150);
	ui->twEmus->setColumnWidth(1, 90);
	ui->twEmus->setColumnWidth(3, 80);
	ui->twEmus->setColumnHidden(2, true);

	ui->twEmus->clear();
	fGrl->cargarDatosTwLista(ui->twEmus, grlDir.Datos +"emu_list.txt", TwListEmu, "|");

// Crear, editar categorías ----------
	regexp.setPattern("[A-Za-z-_0-9]+");
	ui->txtCatTabla->setValidator(new QRegExpValidator(regexp, ui->txtCatTabla));

	ui->cbxCatImg->clear();
	ui->cbxCatImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxCatImg, fGrl->theme(), "img32/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxCatImg->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxCatImg, fGrl->theme(), "img32/", "", grlCfg.FormatsImage.join(";"));

	row = ui->cbxCatImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxCatImg->setCurrentIndex(row);

	emu_list.clear();
	emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");

	ui->twCategorias->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
	ui->twCategorias->headerItem()->setTextAlignment(2, Qt::AlignCenter);
	ui->twCategorias->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twCategorias->header()->setSectionsMovable(false);
	ui->twCategorias->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twCategorias->header()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui->twCategorias->header()->setSectionResizeMode(2, QHeaderView::Fixed);
#else
	ui->twCategorias->header()->setMovable(false);
	ui->twCategorias->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twCategorias->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twCategorias->header()->setResizeMode(2, QHeaderView::Fixed);
#endif
	ui->twCategorias->setColumnWidth(1, 90);
	ui->twCategorias->setColumnWidth(2, 50);

	sql->cargarCategorias(ui->twCategorias);

// Crear, editar menú nav ------------
	ui->cbxMnuNavImg->clear();
	ui->cbxMnuNavImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuNavImg, fGrl->theme(), "img16/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxMnuNavImg->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuNavImg, fGrl->theme(), "img16/", "", grlCfg.FormatsImage.join(";"));

	ui->cbxMnuNavImg->addItemParent("Imágenes idiomas");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuNavImg, fGrl->theme(), "img16/lng/", "lng/", grlCfg.FormatsImage.join(";"));

	row = ui->cbxMnuNavImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxMnuNavImg->setCurrentIndex(row);

	ui->cbxMnuNavColValue->clear();
	ui->cbxMnuNavColValue->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), "{value}");
	ui->cbxMnuNavColValue->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), "true"   );
	ui->cbxMnuNavColValue->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), "false"  );
	ui->cbxMnuNavColValue->setCurrentIndex(0);

	ui->cbxMnuNavSqlQuery->clear();
	ui->cbxMnuNavSqlQuery->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), "WHERE {col_name} LIKE ('%{col_value}%')");
	ui->cbxMnuNavSqlQuery->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), "WHERE {col_name}='{col_value}'"         );
	ui->cbxMnuNavSqlQuery->setCurrentIndex(0);

// Crear, editar menú shortcut -------
	ui->cbxMnuShortCutImg->setIconSize(QSize(32, 32));
	ui->cbxMnuShortCutImg->clear();
	ui->cbxMnuShortCutImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuShortCutImg, fGrl->theme(), "img32/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxMnuShortCutImg->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuShortCutImg, fGrl->theme(), "img32/", "", grlCfg.FormatsImage.join(";"));

	ui->cbxMnuShortCutImg->addItemParent("Imágenes idiomas");
	fGrl->cargarIconosGroupComboBox(ui->cbxMnuShortCutImg, fGrl->theme(), "img32/lng/", "lng/", grlCfg.FormatsImage.join(";"));

	row = ui->cbxMnuShortCutImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxMnuShortCutImg->setCurrentIndex(row);

	ui->cbxMnuShortCutSqlQuery->clear();
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE favorito='true'");
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE original='true'");
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE tipo_emu='datos'");
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE tipo_emu='dosbox'");
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE tipo_emu='scummvm'");
	ui->cbxMnuShortCutSqlQuery->addItem("WHERE tipo_emu='vdmsound'");

	ui->twMnuShortCut->clear();
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT id, titulo, key_sequence, sql_query, img, orden, mostrar, separador FROM dbgrl_mnu_shortcut ORDER By orden ASC;");
	if (sql->chequearQuery(query) && query.first())
	{
		QList<QTreeWidgetItem *> items;
		do {
			QTreeWidgetItem *item = new QTreeWidgetItem;
			item->setIcon(0, QIcon(fGrl->theme() +"img32/"+ query.record().value("img").toString()));
			item->setText(0, query.record().value("titulo").toString());
			item->setText(1, query.record().value("sql_query").toString());
			item->setText(2, query.record().value("img").toString());
			item->setText(3, query.record().value("mostrar").toString());
			item->setText(4, query.record().value("separador").toString());
			item->setText(5, query.record().value("id").toString());
			item->setText(6, query.record().value("orden").toString());
			item->setText(7, query.record().value("key_sequence").toString());
			items << item;
		} while (query.next());
		ui->twMnuShortCut->addTopLevelItems(items);
	}
	ui->txtShortcutKeySequence->installEventFilter(this);

// Editar datos, configuraciones -----
	regexp.setPattern("[A-Za-z-_0-9]+");
	ui->txtDatDato->setValidator(new QRegExpValidator(regexp, ui->txtDatDato));
	fGrl->cargarIconosComboBox(ui->cbxDatImgCmpt, fGrl->theme(), "img_svm/level/", "", "sinimg.png", grlCfg.FormatsImage.join(";"));

	ui->cbxDatImg->clear();
	ui->cbxDatImg->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxDatImg, fGrl->theme(), "img16/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxDatImg->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxDatImg, fGrl->theme(), "img16/", "", grlCfg.FormatsImage.join(";"));

	row = ui->cbxDatImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxDatImg->setCurrentIndex(row);

// Opciones de base de datos ---------
	ui->txtDirBD->setText(fGrl->setDirRelative(grlCfg.db_host));
	ui->txt_dbserver->setText(grlCfg.db_server);
	ui->txt_dbname->setText(grlCfg.db_name);
	ui->txt_dbusername->setText(grlCfg.db_username);
	ui->txt_dbpassword->setText(grlCfg.db_password);
	ui->sbx_dbport->setValue(grlCfg.db_port);

// Opciones de estilos y apariencia --
	ui->cbxStyle->clear();
	ui->cbxStyle->addItem(QIcon(fGrl->theme() +"img16/style.png"), "Style default", "default");
	QStringList list_style = QStyleFactory::keys();
	const int listStyle = list_style.size();
	for (int i= 0; i < listStyle; ++i)
	{
		QString icon_style = "style.png";
		if (list_style.at(i).toLower() == "windows")
			icon_style = "win9x.png";
		if (list_style.at(i).toLower() == "windowsxp")
			icon_style = "winxp.png";
		ui->cbxStyle->addItem(QIcon(fGrl->theme() +"img16/"+ icon_style), "Style "+ list_style.at(i), list_style.at(i));
	}

	int index_style = ui->cbxStyle->findData(grlCfg.Style);
	if (index_style < 0) index_style = 0;
	ui->cbxStyle->setCurrentIndex(index_style);
	emit on_cbxStyle_activated(index_style);

	ui->chkStylePalette->setChecked(grlCfg.StylePalette);
	ui->chkToolbarBigIcon->setChecked(grlCfg.ToolbarBigIcon);

// Opciones fuente -------------------
	ui->chkUsarTipoFuente->setChecked(grlCfg.font_usar);
	ui->cbxFuenteTipo->setCurrentIndex(ui->cbxFuenteTipo->findText(grlCfg.font_family, Qt::MatchContains));
	ui->cbxFontSize->setValue(grlCfg.font_size.toInt());

// PictureFlow -----------------------
	QString sLng = sql->getArchivoIsLng("posiciones.txt");

	setColorBtn(ui->btnPicFlowBgColor, fGrl->getColor(grlCfg.PicFlowBgColor));
	grl_picflow->setBackgroundColor(fGrl->getColor(grlCfg.PicFlowBgColor));
	ui->cbxPicFlowFontFamily->setCurrentIndex(ui->cbxPicFlowFontFamily->findText(grlCfg.PicFlowFontFamily, Qt::MatchContains));
	emit on_cbxPicFlowFontFamily_activated(grlCfg.PicFlowFontFamily);
	ui->spinBox_PicFlowFontSize->setValue(grlCfg.PicFlowFontSize);
	ui->chkPicFlowFontBold->setChecked(grlCfg.PicFlowFontBold);
	ui->chkPicFlowFontItalic->setChecked(grlCfg.PicFlowFontItalic);
	setColorBtn(ui->btnPicFlowFontColor, fGrl->getColor(grlCfg.PicFlowFontColor));
	grl_picflow->setFontColor(fGrl->getColor(grlCfg.PicFlowFontColor));
	ui->spinBox_PicFlowMinHeight->setValue(grlCfg.PicFlowMinHeight);
	ui->spinBox_PicFlowSkip->setValue(grlCfg.PicFlowSkip);

	ui->chkPicFlowShowTitle->setChecked(grlCfg.PicFlowShowTitle);
	emit on_chkPicFlowShowTitle_toggled(grlCfg.PicFlowShowTitle);
	fGrl->cargarDatosComboBox(ui->cbxPicFlowTitlePos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
	ui->cbxPicFlowTitlePos->setCurrentIndex(grlCfg.PicFlowTitlePos);
	emit on_cbxPicFlowTitlePos_activated(grlCfg.PicFlowTitlePos);
	ui->spinBox_PicFlowTitleMarginX->setValue(grlCfg.PicFlowTitleMargin.x());
	ui->spinBox_PicFlowTitleMarginY->setValue(grlCfg.PicFlowTitleMargin.y());

	ui->chkPicFlowShowTitleIcon->setChecked(grlCfg.PicFlowShowTitleIcon);
	emit on_chkPicFlowShowTitleIcon_toggled(grlCfg.PicFlowShowTitleIcon);
	fGrl->cargarDatosComboBox(ui->cbxPicFlowTitleIconPos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
	ui->spinBox_PicFlowTitleIconMarginX->setValue(grlCfg.PicFlowTitleIconMargin.x());
	ui->spinBox_PicFlowTitleIconMarginY->setValue(grlCfg.PicFlowTitleIconMargin.y());

	ui->chkPicFlowShowCaption->setChecked(grlCfg.PicFlowShowCaption);
	emit on_chkPicFlowShowCaption_toggled(grlCfg.PicFlowShowCaption);
	fGrl->cargarDatosComboBox(ui->cbxPicFlowCaptionPos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
	ui->cbxPicFlowCaptionPos->setCurrentIndex(grlCfg.PicFlowCaptionPos);
	emit on_cbxPicFlowCaptionPos_activated(grlCfg.PicFlowCaptionPos);
	ui->spinBox_PicFlowCaptionMarginX->setValue(grlCfg.PicFlowCaptionMargin.x());
	ui->spinBox_PicFlowCaptionMarginY->setValue(grlCfg.PicFlowCaptionMargin.y());

	ui->chkPicFlowShowNumber->setChecked(grlCfg.PicFlowShowNumber);
	emit on_chkPicFlowShowNumber_toggled(grlCfg.PicFlowShowNumber);
	fGrl->cargarDatosComboBox(ui->cbxPicFlowNumberPos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
	ui->cbxPicFlowNumberPos->setCurrentIndex(grlCfg.PicFlowNumberPos);
	emit on_cbxPicFlowNumberPos_activated(grlCfg.PicFlowNumberPos);
	ui->spinBox_PicFlowNumberMarginX->setValue(grlCfg.PicFlowNumberMargin.x());
	ui->spinBox_PicFlowNumberMarginY->setValue(grlCfg.PicFlowNumberMargin.y());
	ui->txtPicFlowNumberFormat->setText(grlCfg.PicFlowNumberFormat);

	ui->chkPicFlowShowIconExtra->setChecked(grlCfg.PicFlowShowIconExtra);
	emit on_chkPicFlowShowIconExtra_toggled(grlCfg.PicFlowShowIconExtra);
	fGrl->cargarDatosComboBox(ui->cbxPicFlowIconExtraPos, ":/datos/"+ sLng +"posiciones.txt", "img16/", "", 3, "|");
	ui->cbxPicFlowIconExtraPos->setCurrentIndex(grlCfg.PicFlowIconExtraPos);
	emit on_cbxPicFlowIconExtraPos_activated(grlCfg.PicFlowIconExtraPos);
	ui->spinBox_PicFlowIconExtraMarginX->setValue(grlCfg.PicFlowIconExtraMargin.x());
	ui->spinBox_PicFlowIconExtraMarginY->setValue(grlCfg.PicFlowIconExtraMargin.y());

	ui->spinBox_PicFlowMarginX->setValue(grlCfg.PicFlowMargin.x());
	ui->spinBox_PicFlowMarginY->setValue(grlCfg.PicFlowMargin.y());

	ui->chkPicFlowShowTriangle->setChecked(grlCfg.PicFlowShowTriangle);
	emit on_chkPicFlowShowTriangle_toggled(grlCfg.PicFlowShowTriangle);

	ui->cbxPicFlowIconExtraImage->clear();
	ui->cbxPicFlowIconExtraImage->setIconSize(QSize(32, 32));
	ui->cbxPicFlowIconExtraImage->addItemParent("Imágenes Categorías");
	fGrl->cargarIconosGroupComboBox(ui->cbxPicFlowIconExtraImage, fGrl->theme(), "img32/cat/", "cat/", grlCfg.FormatsImage.join(";"));

	ui->cbxPicFlowIconExtraImage->addItemParent("Imágenes defecto");
	fGrl->cargarIconosGroupComboBox(ui->cbxPicFlowIconExtraImage, fGrl->theme(), "img32/", "", grlCfg.FormatsImage.join(";"));

	ui->cbxPicFlowIconExtraImage->addItemParent("Imágenes idiomas");
	fGrl->cargarIconosGroupComboBox(ui->cbxPicFlowIconExtraImage, fGrl->theme(), "img32/lng/", "lng/", grlCfg.FormatsImage.join(";"));

	ui->cbxPicFlowIconExtraUse->clear();
	ui->cbxPicFlowIconExtraUse->addItem(QIcon(fGrl->theme() +"img16/fav_0.png")   , tr("Favorito")       , "favorito"  );
	ui->cbxPicFlowIconExtraUse->addItem(QIcon(fGrl->theme() +"img16/original.png"), tr("Original")       , "original"  );
	ui->cbxPicFlowIconExtraUse->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Icono del juego"), "icon_juego");

	row = ui->cbxPicFlowIconExtraUse->findData(grlCfg.PicFlowIconExtraUse, Qt::UserRole, Qt::MatchContains);
	ui->cbxPicFlowIconExtraUse->setCurrentIndex(row);
	emit on_cbxPicFlowIconExtraUse_activated(row);

// Apariencia ------------------------
	ui->twThemes->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
	ui->twThemes->headerItem()->setTextAlignment(1, Qt::AlignCenter);
	ui->twThemes->headerItem()->setTextAlignment(2, Qt::AlignCenter);
	ui->twThemes->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twThemes->header()->setSectionsMovable(false);
	ui->twThemes->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twThemes->header()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui->twThemes->header()->setSectionResizeMode(2, QHeaderView::Fixed);
#else
	ui->twThemes->header()->setMovable(false);
	ui->twThemes->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twThemes->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twThemes->header()->setResizeMode(2, QHeaderView::Fixed);
#endif
	ui->twThemes->setColumnWidth(1, 90);
	ui->twThemes->setColumnWidth(2, 50);

// Opciones del servidor proxy -------
	ui->chkEnableProxy->setChecked(grlCfg.ProxyEnable);
	ui->txt_px_host->setText(grlCfg.ProxyHost);
	ui->sbx_px_puerto->setValue(grlCfg.ProxyPort);
	ui->chkLogin->setChecked(grlCfg.ProxyLogin);
	ui->txt_px_username->setText(grlCfg.ProxyUserName);
	ui->txt_px_password->setText(grlCfg.ProxyPassword);

// Otras opciones --------------------
	ui->lwExtVideo->clear();
	ui->lwExtVideo->addItems(grlCfg.FormatsVideo);
	ui->lwExtVideo->setCurrentRow(0);
	ui->lwExtMusic->clear();
	ui->lwExtMusic->addItems(grlCfg.FormatsMusic);
	ui->lwExtMusic->setCurrentRow(0);
	ui->lwExtImage->clear();
	ui->lwExtImage->addItems(grlCfg.FormatsImage);
	ui->lwExtImage->setCurrentRow(0);

	ui->cbx_thumb_format->clear();
	ui->cbx_thumb_format->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Thumb en JPG"), "JPG");
	ui->cbx_thumb_format->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Thumb en JPEG"), "JPEG");
	ui->cbx_thumb_format->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Thumb en PNG"), "PNG");
	ui->cbx_thumb_format->addItem(QIcon(fGrl->theme() +"img16/imagen.png"), tr("Thumb en BMP"), "BMP");

	row = ui->cbx_thumb_format->findData(grlCfg.thumb_format, Qt::UserRole, Qt::MatchContains);
	ui->cbx_thumb_format->setCurrentIndex(row);

	ui->spinBox_thumb_width->setValue(grlCfg.thumb_width);
	ui->spinBox_thumb_height->setValue(grlCfg.thumb_height);
	ui->spinBox_thumb_quality->setValue(grlCfg.thumb_quality);
	ui->cbx_thumb_format->findData(grlCfg.thumb_format);
	ui->spinBox_thumb_img_width->setValue(grlCfg.thumb_img_width);
	ui->spinBox_thumb_img_height->setValue(grlCfg.thumb_img_height);
	ui->spinBox_thumb_img_quality->setValue(grlCfg.thumb_img_quality);

// Idioma ----------------------------
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, ":/idiomas/"  , "img16/lng/");
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, grlDir.Idiomas, "img16/lng/", "", false);
	int idx_idioma = ui->cbxIdioma->findData(grlCfg.IdiomaSelect +".qm", Qt::UserRole, Qt::MatchEndsWith);
	ui->cbxIdioma->setCurrentIndex(idx_idioma);
	emit on_cbxIdioma_activated(idx_idioma);
}

void frmOpciones::guardarConfig()
{
	if (ui->btnDbxCancel->isEnabled())
		emit on_btnDbxCancel_clicked();
// SqlDatabase
	grlCfg.db_type     = ui->cbxMotorDataBase->itemData(ui->cbxMotorDataBase->currentIndex()).toString();
	grlCfg.db_server   = ui->txt_dbserver->text();
	grlCfg.db_host     = ui->txtDirBD->text();
	grlCfg.db_name     = ui->txt_dbname->text();
	grlCfg.db_username = ui->txt_dbusername->text();
	grlCfg.db_password = ui->txt_dbpassword->text();
	grlCfg.db_port     = ui->sbx_dbport->value();
// OpcGeneral
	grlCfg.Primeravez          = ui->chkShowNext->isChecked();
//	grlCfg.DirDOSBox           = ui->txtDbxPath->text();
	grlCfg.DirScummVM          = ui->txtSvmPath->text();
	grlCfg.DirBaseGames        = ui->txtDirBaseGames->text();
	grlCfg.DOSBoxDisp          = ui->chkDOSBoxDisp->isChecked();
	grlCfg.ScummVMDisp         = ui->chkScummVMDisp->isChecked();
	grlCfg.VDMSoundDisp        = ui->chkVDMSoundDisp->isChecked();
	grlCfg.isChangedIdioma     = IdiomaSelect != grlCfg.IdiomaSelect ? true : false;
	grlCfg.OpenPdfExternal     = ui->chkOpenPdfExternal->isChecked();
	grlCfg.url_xmldb           = ui->cbxScriptURL->itemData(ui->cbxScriptURL->currentIndex()).toString();
	grlCfg.FormatoFecha        = ui->txtFormatoFecha->text().isEmpty() ? "dd/MM/yyyy HH:mm:ss" : ui->txtFormatoFecha->text();
	grlCfg.isChangedToolbarBigIcon = grlCfg.ToolbarBigIcon == ui->chkToolbarBigIcon->isChecked() ? false : true;
	grlCfg.ToolbarBigIcon      = ui->chkToolbarBigIcon->isChecked();
	grlCfg.StylePalette        = ui->chkStylePalette->isChecked();
	grlCfg.isChangedTheme      = ThemeSelect != grlCfg.NameDirTheme ? true : false;
	grlCfg.IconoFav            = ui->cbxIconFav->itemData(ui->cbxIconFav->currentIndex()).toString();
//	grlCfg.VersionDBx          = ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString();
//	grlCfg.DOSBoxSVN           = ui->chkDbxSVN->isChecked();
	grlCfg.DOSBoxSaveConfFile  = ui->chkDbxSaveConfFile->isChecked();
	grlCfg.ScummVMSaveConfFile = ui->chkSvmSaveConfFile->isChecked();
	grlCfg.LastTabOptSelect    = ui->lwOpciones->currentRow();
// OpcPicFlow
	grlCfg.PicFlowMinHeight = ui->spinBox_PicFlowMinHeight->value();
	grlCfg.PicFlowSkip      = ui->spinBox_PicFlowSkip->value();
// OpcFuente
	grlCfg.font_usar   = ui->chkUsarTipoFuente->isChecked();
	grlCfg.font_family = ui->cbxFuenteTipo->currentText();
	grlCfg.font_size   = fGrl->intToStr(ui->cbxFontSize->value());
// HttpProxy
	grlCfg.ProxyEnable   = ui->chkEnableProxy->isChecked();
	grlCfg.ProxyType     = ui->cbxTypeProxy->itemData(ui->cbxTypeProxy->currentIndex()).toInt();
	grlCfg.ProxyHost     = ui->txt_px_host->text();
	grlCfg.ProxyPort     = ui->sbx_px_puerto->value();
	grlCfg.ProxyLogin    = ui->chkLogin->isChecked();
	grlCfg.ProxyUserName = ui->txt_px_username->text();
	grlCfg.ProxyPassword = ui->txt_px_password->text();
// OpcMultimedia
	grlCfg.FormatsVideo = fGrl->lwToStrList(ui->lwExtVideo);
	grlCfg.FormatsMusic = fGrl->lwToStrList(ui->lwExtMusic);
	grlCfg.FormatsImage = fGrl->lwToStrList(ui->lwExtImage);
// Updates
	grlCfg.chkVersion        = ui->chkComprobarVersionGrl->isChecked();
	grlCfg.chkUpdateInterval = ui->cbxUpdateInterval->itemData(ui->cbxUpdateInterval->currentIndex()).toInt();
	grlCfg.chkUpdateThemes   = ui->chkUpdateThemes->isChecked();
// Otras opciones
	grlCfg.thumb_width       = ui->spinBox_thumb_width->value();
	grlCfg.thumb_height      = ui->spinBox_thumb_height->value();
	grlCfg.thumb_quality     = ui->spinBox_thumb_quality->value();
	grlCfg.thumb_format      = ui->cbx_thumb_format->itemData(ui->cbx_thumb_format->currentIndex()).toString();
	grlCfg.thumb_img_width   = ui->spinBox_thumb_img_width->value();
	grlCfg.thumb_img_height  = ui->spinBox_thumb_img_height->value();
	grlCfg.thumb_img_quality = ui->spinBox_thumb_img_quality->value();
}

void frmOpciones::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/barra_herramintas.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
// Lista opciones
	ui->lwOpciones->item(0)->setIcon(QIcon(fGrl->theme() +"img24/opciones.png"));
	ui->lwOpciones->item(1)->setIcon(QIcon(fGrl->theme() +"img24/emuladores.png"));
	ui->lwOpciones->item(2)->setIcon(QIcon(fGrl->theme() +"img24/categorias.png"));
	ui->lwOpciones->item(3)->setIcon(QIcon(fGrl->theme() +"img24/menu_nav.png"));
	ui->lwOpciones->item(4)->setIcon(QIcon(fGrl->theme() +"img24/shortcut.png"));
	ui->lwOpciones->item(5)->setIcon(QIcon(fGrl->theme() +"img24/datos_1.png"));
	ui->lwOpciones->item(6)->setIcon(QIcon(fGrl->theme() +"img24/basedatos.png"));
	ui->lwOpciones->item(7)->setIcon(QIcon(fGrl->theme() +"img24/style.png"));
	ui->lwOpciones->item(8)->setIcon(QIcon(fGrl->theme() +"img24/html.png"));
	ui->lwOpciones->item(9)->setIcon(QIcon(fGrl->theme() +"img24/ejecutar_app_setup.png"));
// General
	ui->btnSowEmus->setIcon(QIcon(fGrl->theme() +"img24/emuladores.png"));
	ui->btnDirBaseGames->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDirBaseGames_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnOpenUrl->setIcon(QIcon(fGrl->theme() +"img16/edit_enlace.png"));
	ui->btnInfoFormatoFecha->setIcon(QIcon(fGrl->theme() +"img16/informacion.png"));
// Emuladores Tabs
	ui->tabwEmuladores->setTabIcon(0, QIcon(fGrl->theme() +"img16/dosbox-scummvm.png"));
	ui->tabwEmuladores->setTabIcon(1, QIcon(fGrl->theme() +"img16/emuladores.png"));
// Emulador ScummVM
	ui->btnSvmPath->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnSvmPath_find->setIcon(QIcon(fGrl->theme() +"img16/buscar.png"));
	ui->btnSvmPath_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
// Emulador DOSBox
	ui->btnDbxPath->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbxPath_find->setIcon(QIcon(fGrl->theme() +"img16/buscar.png"));
	ui->btnDbxPath_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDbxNew->setIcon(QIcon(fGrl->theme() +"img16/nuevo.png"));
	ui->btnDbxAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDbxEdit->setIcon(QIcon(fGrl->theme() +"img16/editar.png"));
	ui->btnDbxUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnDbxSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnDbxBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDbxDefault->setIcon(QIcon(fGrl->theme() +"img16/go-back.png"));
	ui->btnDbxDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDbxCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
// Emuladores
	ui->btnEmuPath->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnEmuPath_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnEmuAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnEmuUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnEmuSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnEmuBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnEmuDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Categorias
	ui->btnCatAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnCatUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnCatSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnCatBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnCatEditTheme->setIcon(QIcon(fGrl->theme() +"img16/style.png"));
	ui->btnCatDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Menú navegación
	ui->btnMnuNavAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnMnuNavUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnMnuNavSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnMnuNavBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnMnuNavDefecto->setIcon(QIcon(fGrl->theme() +"img16/categorias.png"));
	ui->btnMnuNavDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Menú ShortCut
	ui->btnMnuShortCutAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnMnuShortCutUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnMnuShortCutSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnMnuShortCutBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnMnuShortCutDefecto->setIcon(QIcon(fGrl->theme() +"img16/categorias.png"));
	ui->btnMnuShortCutDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Datos de archivos
	ui->btnDatAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDatUpdate->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnDatSubir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnDatBajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDatAddSmile->setIcon(QIcon(fGrl->theme() +"img16/smile.png"));
	ui->btnDatDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Opciones de la Base de Datos
	ui->btnDirDB->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDirDB_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
// Apariencia Tabs
	ui->tabApariencia->setTabIcon(0, QIcon(fGrl->theme() +"img16/style.png"));
	ui->tabApariencia->setTabIcon(1, QIcon(fGrl->theme() +"img16/pictureflow.png"));
// Apariencia general
	ui->btnEditTheme->setIcon(QIcon(fGrl->theme() +"img16/style.png"));
// Apariencia pictureflow
	grl_picflow->setSlideTitleIcon(QImage(fGrl->theme() +"img32/datos_3.png"));
// Otras opciones
	ui->btnExtVideoAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnExtVideoDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnExtMusicAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnExtMusicDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnExtImageAdd->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnExtImageDelete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));

	emit on_chkUsarTipoFuente_clicked(grlCfg.font_usar);
}

void frmOpciones::cargarListaThemes()
{
	QString autor, version;
	QTreeWidgetItem *item;
	QList<QTreeWidgetItem *> items;
	int index = -1;

	QDir dir(grlDir.Themes);
	QStringList list_themes = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
	list_themes.insert(0, "defecto");

	ui->lb_theme_example->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));

	ui->twThemes->clear();
	const int list_themesSize = list_themes.size();
	for (int i = 0; i < list_themesSize; ++i)
	{
		autor   = "-";
		version = "-";

		if (QFile::exists(grlDir.Themes + list_themes.at(i) +"/info.ini"))
		{
			QSettings settings(grlDir.Themes + list_themes.at(i) +"/info.ini", QSettings::IniFormat);
			settings.beginGroup("info");
				autor   = settings.value("autor"  , "-").toString();
				version = settings.value("version", "-").toString();
			settings.endGroup();
		}

		item = new QTreeWidgetItem;//(ui->twThemes)
		item->setIcon(0, QIcon(fGrl->theme() +"img16/style.png"));
		item->setTextAlignment(1, Qt::AlignCenter);
		item->setTextAlignment(2, Qt::AlignCenter);
		item->setText(0, list_themes.at(i));
		item->setText(1, autor  );
		item->setText(2, version);
		items << item;

		if (list_themes.at(i) == grlCfg.NameDirTheme)
		{
			index = i;

			if (QFile::exists(grlDir.Themes + list_themes.at(i) +"/preview.png"))
				ui->lb_theme_example->setPixmap(QPixmap(grlDir.Themes + list_themes.at(i) +"/preview.png"));
		}
	}

	ui->twThemes->addTopLevelItems(items);

	if (index > -1)
	{
		ui->twThemes->clearSelection();
		ui->twThemes->setItemSelected(items.at(index), true);
	}
}

void frmOpciones::on_btnOk_clicked()
{
	guardarConfig();
	QDialog::accept();
}

void frmOpciones::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmOpciones::on_lwOpciones_currentRowChanged(int currentRow)
{
	if (currentRow > -1)
		ui->swConfigOpciones->setCurrentIndex(currentRow);
	else
		ui->swConfigOpciones->setCurrentIndex(0);

	grlCfg.LastTabOptSelect = currentRow;
}

void frmOpciones::setColorBtn(QPushButton *btnColor, QColor color)
{
	int w = 40;
	int h = 16;
	QPixmap img(w, h);
	QPainter painter(&img);
	painter.fillRect(img.rect(), color);
	painter.setPen(Qt::black);
	painter.drawRect(0, 0, w-1, h-1);
	painter.end();
	btnColor->setIconSize(QSize(w, h));
	btnColor->setIcon(QIcon(img));
}

// General ------------------------------------------
void frmOpciones::on_cbxIdioma_activated(int index)
{
	if (index > -1)
	{
		idioma = ui->cbxIdioma->itemData(index).toString();
		translator.load(idioma);

		idioma.remove(grlDir.Idiomas +"gr-lida_");
		idioma.remove(":/idiomas/gr-lida_");
		idioma.remove(".qm");

		grlCfg.IdiomaSelect = idioma;
	} else {
		grlCfg.IdiomaSelect = "es_ES";
		translator.load(":/idiomas/gr-lida_"+ grlCfg.IdiomaSelect +".qm");
	}

	fGrl->setIdioma(grlCfg.IdiomaSelect);
	sql->setIdioma(grlCfg.IdiomaSelect);

	qApp->installTranslator(&translator);
	ui->retranslateUi(this);

// Crear, editar categorías ----------
	cbxCatEmuShow->clear();
	cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/tag.png")         , tr("Todos")   , "all"     );
	cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/datos.png")   , tr("Datos")   , "datos"   );
	cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
	cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png") , tr("ScummVM") , "scummvm" );
	cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"), tr("VDMSound"), "vdmsound");

	foreach (const stGrlDatos &dat, emu_list)
		cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/"+ dat.icono), dat.titulo, dat.key);

// Crear, editar menú nav ------------
	ui->cbxMnuNavColName->clear();
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Genero")          , "genero"          );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Compañia")        , "compania"        );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Desarrollador")   , "desarrollador"   );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Tema")            , "tema"            );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Grupo")           , "grupo"           );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Perspectiva")     , "perspectiva"     );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/idiomas.png") , tr("Idioma")          , "idioma"          );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/idiomas.png") , tr("Idioma voces")    , "idioma_voces"    );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Formato")         , "formato"         );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/fecha.png")   , tr("Año")             , "anno"            );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")  , tr("Nº discos")       , "numdisc"         );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/cat/pc.png")  , tr("Sistema")         , "sistemaop"       );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/grafica.png") , tr("Gráficos")        , "graficos"        );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/grafica.png") , tr("Sonido")          , "sonido"          );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/grafica.png") , tr("Jugabilidad")     , "jugabilidad"     );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/original.png"), tr("Original")        , "original"        );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , tr("Estado")          , "estado"          );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/fecha.png")   , tr("Fecha")           , "fecha"           );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/cartucho.png"), tr("Tipo Emu")        , "tipo_emu"        );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/fav_0.png")   , tr("Favorito")        , "favorito"        );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/star_on.png") , tr("Calificación")    , "rating"          );
	ui->cbxMnuNavColName->addItem(QIcon(fGrl->theme() +"img16/edad_nd.png") , tr("Edad recomendada"), "edad_recomendada");
	ui->cbxMnuNavColName->setCurrentIndex(0);

	ui->cbxMnuNavArchivo->clear();
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_3.png") , "<"+ tr("Defecto") +">"  , "defecto"             );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Generos")            , "generos.txt"         );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Compañias")          , "companias.txt"       );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Temas")              , "temas.txt"           );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Grupos")             , "grupos.txt"          );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Perspectivas")       , "perspectivas.txt"    );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/idiomas.png") , tr("Idiomas")            , "idiomas.txt"         );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png") , tr("Formatos")           , "formatos.txt"        );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/fecha.png")   , tr("Años")               , "fechas.txt"          );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")  , tr("Nº discos")          , "numdisc.txt"         );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/pc.png")  , tr("Sistemas")           , "sistemas.txt"        );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/grafica.png") , tr("Puntuación")         , "puntuacion.txt"      );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/star_on.png") , tr("Calificación")       , "rating.txt"          );
	ui->cbxMnuNavArchivo->addItem(QIcon(fGrl->theme() +"img16/edad_nd.png") , tr("Edades recomendadas"), "edad_recomendada.txt");
	ui->cbxMnuNavArchivo->setCurrentIndex(0);

	ui->twMnuNav->clear();
	sql->cargarMenuNav(ui->twMnuNav, "", true);

// Editar datos, configuraciones -----
	ui->cbxDatArchivo->clear();
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Generos")              , "generos.txt"           );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Compañias")            , "companias.txt"         );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Temas")                , "temas.txt"             );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Perspectivas")         , "perspectivas.txt"      );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/idiomas.png")    , tr("Modificar archivo") +" - "+ tr("Idiomas")              , "idiomas.txt"           );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Formatos")             , "formatos.txt"          );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/datos_1.png")    , tr("Modificar archivo") +" - "+ tr("Grupos")               , "grupos.txt"            );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")     , tr("Modificar archivo") +" - "+ tr("Nº discos")            , "numdisc.txt"           );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/pc.png")     , tr("Modificar archivo") +" - "+ tr("Sistemas")             , "sistemas.txt"          );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/edad_nd.png")    , tr("Modificar archivo") +" - "+ tr("Edades recomendadas")  , "edad_recomendada.txt"  );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/smile.png" )     , tr("Modificar archivo") +" - "+ tr("Smiles")               , "smiles.txt"            );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_cpu_core"          , "dbx_cpu_core.txt"      );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_cpu_type"          , "dbx_cpu_type.txt"      );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox SVN - dbx_cpu_type_svn"  , "dbx_cpu_type_svn.txt"  );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_cycles"            , "dbx_cycles.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_hwscale"           , "dbx_hwscale.txt"       );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_joystick"          , "dbx_joystick.txt"      );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_keyboardlayout"    , "dbx_keyboardlayout.txt");
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_machine"           , "dbx_machine.txt"       );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox SVN - dbx_machine_svn"   , "dbx_machine_svn.txt"   );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_memsize"           , "dbx_memsize.txt"       );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_midi_device"       , "dbx_midi_device.txt"   );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_mpu401"            , "dbx_mpu401.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_output"            , "dbx_output.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox SVN - dbx_output_svn"    , "dbx_output_svn.txt"    );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_resolution"        , "dbx_resolution.txt"    );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox SVN - dbx_resolution_svn", "dbx_resolution_svn.txt");
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_sb_oplemu"         , "dbx_sb_oplemu.txt"     );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_sb_oplmode"        , "dbx_sb_oplmode.txt"    );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_sbtype"            , "dbx_sbtype.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_scaler"            , "dbx_scaler.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox SVN - dbx_scaler_svn"    , "dbx_scaler_svn.txt"    );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png") , tr("Modificar archivo") +" DOSBox - dbx_sensitivity"       , "dbx_sensitivity.txt"   );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_gfxmode"          , "svm_gfxmode.txt"       );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_idioma"           , "svm_idioma.txt"        );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_lista"            , "svm_lista.txt"         );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_music_driver"     , "svm_music_driver.txt"  );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_platform"         , "svm_platform.txt"      );
	ui->cbxDatArchivo->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("Modificar archivo") +" ScummVM - svm_render_mode"      , "svm_render_mode.txt"   );
	ui->cbxDatArchivo->setCurrentIndex(0);
	emit on_cbxDatArchivo_activated(ui->cbxDatArchivo->currentIndex());

// Opciones de base de datos ---------
	ui->cbxMotorDataBase->clear();
	ui->cbxMotorDataBase->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), tr("Base de datos") +" SQLite"    , "QSQLITE");
	ui->cbxMotorDataBase->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), tr("Base de datos") +" MySQL"     , "QMYSQL" );
	ui->cbxMotorDataBase->addItem(QIcon(fGrl->theme() +"img16/basedatos.png"), tr("Base de datos") +" PostgreSQL", "QPSQL"  );
	ui->cbxMotorDataBase->setCurrentIndex(ui->cbxMotorDataBase->findData(grlCfg.db_type));

// Icono favorito --------------------
	fGrl->cargarIconosComboBox(ui->cbxIconFav, fGrl->theme(), "img16/", "", grlCfg.IconoFav, grlCfg.FormatsImage.join(";"), true, "fav_");

// PictureFlow -----------------------
	ui->cbxPicFlowType->clear();
	ui->cbxPicFlowType->addItem(QIcon(fGrl->theme() +"img16/capturas.png"), tr("Tipo CoverFlow")    , "CoverFlowLike"  );
	ui->cbxPicFlowType->addItem(QIcon(fGrl->theme() +"img16/capturas.png"), tr("Tipo tira")         , "Strip"          );
	ui->cbxPicFlowType->addItem(QIcon(fGrl->theme() +"img16/capturas.png"), tr("Tipo tira solapada"), "StripOverlapped");

	int index_picflowtype = ui->cbxPicFlowType->findData(grlCfg.PicFlowType);
	if (index_picflowtype < 0) index_picflowtype = 0;
	ui->cbxPicFlowType->setCurrentIndex(index_picflowtype);
	emit on_cbxPicFlowType_activated(index_picflowtype);

	grl_picflow->clear();
	grl_picflow->setSlideTitle(tr("Título descriptivo"), grlCfg.PicFlowTitlePos);
	grl_picflow->addSlide(QImage(fGrl->theme() +"images/juego_sin_imagen.png"), tr("Título Juego") +" 1");
	grl_picflow->addSlide(QImage(fGrl->theme() +"images/juego_sin_imagen.png"), tr("Título Juego") +" 2");
	grl_picflow->addSlide(QImage(fGrl->theme() +"images/juego_sin_imagen.png"), tr("Título Juego") +" 3");
	grl_picflow->addSlide(QImage(fGrl->theme() +"images/juego_sin_imagen.png"), tr("Título Juego") +" 4");
	grl_picflow->addSlide(QImage(fGrl->theme() +"images/juego_sin_imagen.png"), tr("Título Juego") +" 5");

// Opciones del servidor proxy -------
	ui->cbxTypeProxy->clear();
	ui->cbxTypeProxy->addItem(QIcon(fGrl->theme() +"img16/html.png"), tr("No usar proxy"), "2");
	ui->cbxTypeProxy->addItem(QIcon(fGrl->theme() +"img16/html.png"), tr("El proxy se determina sobre la base de la aplicación"), "0");
	ui->cbxTypeProxy->addItem(QIcon(fGrl->theme() +"img16/html.png"), tr("Socks5 proxy se utilizara"), "1");
	ui->cbxTypeProxy->addItem(QIcon(fGrl->theme() +"img16/html.png"), tr("HTTP se utiliza proxy transparente"), "3");
	ui->cbxTypeProxy->addItem(QIcon(fGrl->theme() +"img16/html.png"), tr("HTTP proxy sólo para las solicitudes"), "4");
	ui->cbxTypeProxy->setCurrentIndex(ui->cbxTypeProxy->findData(grlCfg.ProxyType));
}

void frmOpciones::on_btnSowEmus_clicked()
{
	ui->lwOpciones->setCurrentRow(1);
}

void frmOpciones::on_btnDirBaseGames_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.DirBaseGames, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->txtDirBaseGames->setText(fGrl->setDirRelative(directorio, "DosGames"));
		grlCfg.DirBaseGames = ui->txtDirBaseGames->text();
	}
}

void frmOpciones::on_btnDirBaseGames_clear_clicked()
{
	ui->txtDirBaseGames->setText("./DosGames/");
}

void frmOpciones::on_btnOpenUrl_clicked()
{
	QString archivo = ui->cbxScriptURL->itemData(ui->cbxScriptURL->currentIndex()).toString();
	if (archivo != "_desde_archivo_")
	{
	// Cargamos el script
		QScriptEngine engine;
		engine.evaluate(fGrl->leerArchivo(grlDir.Scripts + archivo, "UTF-8"));
	// Abre la URL con el navegador por defecto
		if (engine.evaluate("url_site").isValid())
			fGrl->abrirArchivo(engine.evaluate("url_site").toString());
	}
}

void frmOpciones::on_txtFormatoFecha_textEdited(const QString &arg1)
{
	grlCfg.isChangedFormatoFecha = arg1 != grlCfg.FormatoFecha ? true : false;
}

void frmOpciones::on_btnInfoFormatoFecha_clicked()
{
	sql->ventanaInfo(tr("Información sobre el formato de la fecha y hora"), "informacion.png", 500, 400,
		tr("Devuelve una cadena con formato de acuerdo a la cadena de formato dada.") +"<br>"+
		tr("Lista básica de parámetros:") +"<br>"
		"<table border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\">"
		"  <thead>"
		"    <tr>"
		"      <td class=\"row_header\"><p align=\"center\">"+ tr("Expresión") +"</p></td>"
		"      <td class=\"row_header\"><p align=\"center\">"+ tr("Salida") +"</p></td>"
		"    </tr>"
		"  </thead>"
		"  <tr>"
		"    <td class=\"row_1\"><p>d</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Día del mes sin ceros iniciales (1 a 31).") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>dd</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Día del mes, 2 dígitos con ceros iniciales (01 a 31).") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>ddd</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Una representación textual de un día, tres letras (e.j. 'Lun' a 'Dom').") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>dddd</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Una representación textual completa del día de la semana (e.j. 'Lunes' a 'Domingo').") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>M</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Representación numérica de un mes, sin ceros iniciales (1-12).") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>MM</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Representación numérica de un mes, con ceros iniciales (01-12).") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>MMM</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Una representación textual corta de un mes, tres letras (e.j. 'Ene' a 'Dic').") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>MMMM</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Una representación textual completa de un mes, como Enero o Marzo (e.j. 'Enero' a 'Diciembre').") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>yy</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Una representación de dos dígitos de un año (00-99).") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>yyyy</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Una representación numérica completa de un año, 4 dígitos.") +"</p></td>"
		"  </tr>"
		"</table>"
		"<br><br>"+ tr("Estas expresiones pueden ser utilizadas para el tiempo.") +"<br>"
		"<table border=\"0\" align=\"center\" cellspacing=\"1\" cellpadding=\"2\">"
		"  <thead>"
		"    <tr>"
		"      <td class=\"row_header\"><p align=\"center\">"+ tr("Expresión") +"</p></td>"
		"      <td class=\"row_header\"><p align=\"center\">"+ tr("Salida") +"</p></td>"
		"    </tr>"
		"  </thead>"
		"  <tr>"
		"    <td class=\"row_1\"><p>h</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("La hora sin ceros iniciales (0 a 23 o 1 a 12 si AM/PM display)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>hh</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("La hora con ceros iniciales (00 a 23 o de 01 a 12 si AM/PM display)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>H</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("La hora sin ceros iniciales (0 a 23, incluso con AM/PM display)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>HH</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("La hora con un cero (00 a 23, incluso con AM/PM display)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>m</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Los minutos sin ceros iniciales (0 a 59)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>mm</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Los minutos con ceros iniciales (00 a 59)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>s</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Los segundos sin ceros iniciales (0 a 59)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>ss</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Los segundos con ceros iniciales (00 a 59)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>z</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Los milisegundos sin ceros iniciales (0 a 999)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>zzz</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Los milisegundos con ceros iniciales (000 a 999)") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_1\"><p>AP or A</p></td>"
		"    <td class=\"row_1\"><p>"+ tr("Interpretar como un AM/PM del tiempo. AP debe ser \"AM\" o \"PM\".") +"</p></td>"
		"  </tr>"
		"  <tr>"
		"    <td class=\"row_2\"><p>ap or a</p></td>"
		"    <td class=\"row_2\"><p>"+ tr("Interpretar como un AM/PM del tiempo. AP debe ser \"am\" o \"pm\".") +"</p></td>"
		"  </tr>"
		"</table>", false
	);
}

// Emuladores ---------------------------------------
// ScummVM --
void frmOpciones::on_btnSvmPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona el ejecutable del ScummVM"), grlCfg.Svm_path, "", "ScummVM (scummvm.exe scummvm);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtSvmPath->setText(fGrl->setDirRelative(archivo));
			grlCfg.Svm_path = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmOpciones::on_btnSvmPath_find_clicked()
{
	frmBuscar *buscar = new frmBuscar("ScummVM", ".exe", ui->txtSvmPath->text(), fGrl->theme(), this);
	if (buscar->exec() == QDialog::Accepted)
		ui->txtSvmPath->setText(fGrl->setDirRelative(buscar->getSelect()));
	delete buscar;
}

void frmOpciones::on_btnSvmPath_clear_clicked()
{
	ui->txtSvmPath->clear();
}

// DOSBox --
void frmOpciones::addEditTwDOSBox(bool isAdd)
{
	bool siguiente = false;
	if (ui->txtDbxTitulo->text().isEmpty())
	{
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner un título."));
		ui->txtDbxTitulo->setFocus();
	} else if (ui->txtDbxDato->text().isEmpty()) {
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner una etiqueta."));
		ui->txtDbxDato->setFocus();
	} else if (ui->txtDbxPath->text().isEmpty()) {
		QMessageBox::information(this, tr("Opciones"), tr("Debes indicar el ejecutable del DOSBox."));
		ui->txtDbxPath->setFocus();
	} else {
		QTreeWidgetItem *item = NULL;
		if (!isAdd)
			item = ui->twDOSBox->currentItem();

		int total_dbx_find_cfg = 0;
		if (!isAdd && ui->txtDbxDato->text() == item->text(1))
			total_dbx_find_cfg = 0;
		else {
			QList<QTreeWidgetItem *> dbx_find_cfg = ui->twDOSBox->findItems(ui->txtDbxDato->text(), Qt::MatchExactly, 1);
			total_dbx_find_cfg = dbx_find_cfg.size();
		}

		if (total_dbx_find_cfg > 0)
			QMessageBox::information(this, tr("Opciones"), tr("Ya existe la misma etiqueta para el ejecutable del DOSBox."));
		else {
			siguiente = true;
			if (isAdd)
				item = new QTreeWidgetItem;

			QString imgDbx = ui->cbxDbxImg->itemData(ui->cbxDbxImg->currentIndex()).toString();

			item->setTextAlignment(2, Qt::AlignCenter);
			item->setTextAlignment(3, Qt::AlignCenter);
			item->setIcon(0, QIcon(fGrl->theme() +"img32/"+ imgDbx));
			item->setText(0, ui->txtDbxTitulo->text());
			item->setText(1, ui->txtDbxDato->text());
			item->setText(2, ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString());
			item->setText(3, fGrl->boolToStr(ui->chkDbxSVN->isChecked(),true));
			item->setText(4, ui->txtDbxPath->text());
			item->setText(5, imgDbx);

			if (isAdd)
				ui->twDOSBox->addTopLevelItem(item);

			fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
			grlCfg.isChangedListDOSBox = true;
		}
	}

	if (siguiente)
		emit on_btnDbxCancel_clicked();
}

void frmOpciones::textEditedTxtDbxDato()
{
	if (ui->btnDbxAdd->isVisible() || ui->btnDbxUpdate->isVisible())
	{
		QString text = fGrl->eliminar_caracteres(ui->txtDbxTitulo->text());
		text.append("_"+ fGrl->eliminar_caracteres(ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString()));
		text.append(ui->chkDbxSVN->isChecked()?"_svn":"");
		ui->txtDbxDato->setText(text.toLower());
	}
}

void frmOpciones::on_txtDbxTitulo_textEdited(const QString &arg1)
{
	Q_UNUSED(arg1);
	textEditedTxtDbxDato();
}

void frmOpciones::on_chkDbxSVN_toggled(bool checked)
{
	Q_UNUSED(checked);
	textEditedTxtDbxDato();
}

void frmOpciones::on_cbxDbxVersion_activated(int index)
{
	if (index > -1)
		textEditedTxtDbxDato();
}

void frmOpciones::on_btnDbxPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona el ejecutable del DOSBox"), grlCfg.Dbx_path, "", "DOSBox (dosbox.exe dosbox);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbxPath->setText(fGrl->setDirRelative(archivo));
			grlCfg.Dbx_path = fGrl->setDirRelative(f_info.Path);
			grlCfg.isChangedListDOSBox = true;
		}
	}
}

void frmOpciones::on_btnDbxPath_find_clicked()
{
	frmBuscar *buscar = new frmBuscar("DOSBox", ".exe", ui->txtDbxPath->text(), fGrl->theme(), this);
	if (buscar->exec() == QDialog::Accepted)
	{
		ui->txtDbxPath->setText(fGrl->setDirRelative(buscar->getSelect()));
		grlCfg.isChangedListDOSBox = true;
	}
	delete buscar;
}

void frmOpciones::on_btnDbxPath_clear_clicked()
{
	ui->txtDbxPath->clear();
}

void frmOpciones::on_btnDbxDefault_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if (ui->twDOSBox->topLevelItemCount() > 0 && pos != -1)
		grlCfg.DOSBoxDefault = ui->twDOSBox->currentItem()->text(1);
}

void frmOpciones::on_btnDbxNew_clicked()
{
	ui->wAddEditDbx->setEnabled(true);
	ui->twDOSBox->setEnabled(false);
	ui->btnDbxNew->setVisible(false);
	ui->btnDbxAdd->setVisible(true);
	ui->btnDbxAdd->setEnabled(true);
	ui->btnDbxEdit->setEnabled(false);
	ui->btnDbxUpdate->setVisible(false);
	ui->btnDbxSubir->setEnabled(false);
	ui->btnDbxBajar->setEnabled(false);
	ui->btnDbxDefault->setEnabled(false);
	ui->btnDbxDelete->setEnabled(false);
	ui->btnDbxCancel->setEnabled(true);

	ui->txtDbxTitulo->setText("");
	ui->txtDbxDato->setText("");
	ui->txtDbxPath->setText("");
	ui->chkDbxSVN->setChecked(false);
	ui->cbxDbxVersion->setCurrentIndex(0);
	ui->txtDbxTitulo->setFocus();

	int row = ui->cbxDbxImg->findData("cat/dosbox.png", Qt::UserRole, Qt::MatchContains);
	if (row < 0) row = 0;
	ui->cbxDbxImg->setCurrentIndex(row);
}

void frmOpciones::on_btnDbxAdd_clicked()
{
	addEditTwDOSBox(true);
}

void frmOpciones::on_btnDbxEdit_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if (ui->twDOSBox->topLevelItemCount() > 0 && pos != -1)
	{
		QTreeWidgetItem *item = ui->twDOSBox->currentItem();

		ui->txtDbxTitulo->setText(item->text(0));
		ui->txtDbxDato->setText(item->text(1));
		ui->txtDbxPath->setText(item->text(4));
		ui->chkDbxSVN->setChecked(fGrl->strToBool(item->text(3)));
		int row = ui->cbxDbxVersion->findData(item->text(2), Qt::UserRole, Qt::MatchExactly);
		if (row < 0) row = 0;
		ui->cbxDbxVersion->setCurrentIndex(row);

		row = ui->cbxDbxImg->findData(item->text(5), Qt::UserRole, Qt::MatchContains);
		if (row < 0) row = 0;
		ui->cbxDbxImg->setCurrentIndex(row);

		ui->wAddEditDbx->setEnabled(true);
		ui->twDOSBox->setEnabled(false);
		ui->btnDbxNew->setEnabled(false);
		ui->btnDbxAdd->setVisible(false);
		ui->btnDbxEdit->setVisible(false);
		ui->btnDbxUpdate->setVisible(true);
		ui->btnDbxUpdate->setEnabled(true);
		ui->btnDbxSubir->setEnabled(false);
		ui->btnDbxBajar->setEnabled(false);
		ui->btnDbxDefault->setEnabled(false);
		ui->btnDbxDelete->setEnabled(false);
		ui->btnDbxCancel->setEnabled(true);
	}
}

void frmOpciones::on_btnDbxUpdate_clicked()
{
	addEditTwDOSBox(false);
}

void frmOpciones::on_btnDbxSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twDOSBox);
	fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
	grlCfg.isChangedListDOSBox = true;
}

void frmOpciones::on_btnDbxBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twDOSBox);
	fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
	grlCfg.isChangedListDOSBox = true;
}

void frmOpciones::on_btnDbxDelete_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if (ui->twDOSBox->topLevelItemCount() > 0 && pos != -1)
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este DOSBox de la lista?") +"\n"+ tr("Se actualizaran los juegos al emulador por defecto.")))
		{
		// Actualizamos el DOSBox que se usaba a uno por defecto.
			QSqlQuery query(sql->getSqlDB());
			QString strSQL = "UPDATE dbgrl_emu_dosbox SET dosbox_emu_key = :new_dosbox_emu_key WHERE dosbox_emu_key = :old_dosbox_emu_key;";
			query.prepare(strSQL);
			query.bindValue(":new_dosbox_emu_key", grlCfg.DOSBoxDefault);
			query.bindValue(":old_dosbox_emu_key", ui->twDOSBox->currentItem()->text(1));
			query.exec();
			sql->chequearQuery(query);

			fGrl->deleteItemTree(ui->twDOSBox->currentItem());
			fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
			grlCfg.isChangedListDOSBox = true;
		}
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione un dato de la lista para eliminarlo"));
}

void frmOpciones::on_btnDbxCancel_clicked()
{
	ui->wAddEditDbx->setEnabled(false);
	ui->twDOSBox->setEnabled(true);

	ui->btnDbxNew->setVisible(true);
	ui->btnDbxNew->setEnabled(true);
	ui->btnDbxAdd->setVisible(false);
	ui->btnDbxEdit->setVisible(true);
	ui->btnDbxEdit->setEnabled(true);
	ui->btnDbxUpdate->setVisible(false);
	ui->btnDbxSubir->setEnabled(true);
	ui->btnDbxBajar->setEnabled(true);
	ui->btnDbxDefault->setEnabled(true);
	ui->btnDbxDelete->setEnabled(true);
	ui->btnDbxCancel->setEnabled(false);

	ui->txtDbxTitulo->setText("");
	ui->txtDbxDato->setText("");
	ui->txtDbxPath->setText("");
	ui->chkDbxSVN->setChecked(false);
	ui->cbxDbxVersion->setCurrentIndex(0);


	int row = ui->cbxDbxImg->findData("cat/sinimg.png", Qt::UserRole, Qt::MatchContains);
	if (row < 0) row = 0;
	ui->cbxDbxImg->setCurrentIndex(row);
}

// Otros emuladores ---------------------------------
void frmOpciones::addEditTwEmus(bool isAdd)
{
	if (ui->txtEmuTitulo->text().isEmpty())
	{
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner un título."));
		ui->txtEmuTitulo->setFocus();
	} else if (ui->txtEmuDato->text().isEmpty()) {
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner una etiqueta."));
		ui->txtEmuDato->setFocus();
	} else if (ui->txtEmuPath->text().isEmpty()) {
		QMessageBox::information(this, tr("Opciones"), tr("Debes indicar el ejecutable del Emulador."));
		ui->txtEmuPath->setFocus();
	} else {
		QTreeWidgetItem *item = NULL;
		if (!isAdd)
			item = ui->twEmus->currentItem();

		int total_emu_find_cfg = 0;
		if (!isAdd && ui->txtEmuDato->text() == item->text(1))
			total_emu_find_cfg = 0;
		else {
			QList<QTreeWidgetItem *> emu_find_cfg = ui->twEmus->findItems(ui->txtEmuDato->text(), Qt::MatchExactly, 1);
			total_emu_find_cfg = emu_find_cfg.size();
		}

		if (total_emu_find_cfg > 0)
		{
			QMessageBox::information(this, tr("Opciones"), tr("Ya existe la misma etiqueta para el ejecutable del Emulador."));
			ui->txtEmuDato->setFocus();
		} else {
			if (isAdd)
				item = new QTreeWidgetItem;

			QString imgEmu = ui->cbxEmuImg->itemData(ui->cbxEmuImg->currentIndex()).toString();

			item->setIcon(0, QIcon(fGrl->theme() +"img32/"+ imgEmu));
			item->setText(0, ui->txtEmuTitulo->text());
			item->setText(1, ui->txtEmuDato->text());
			item->setText(2, "-");
			item->setText(3, ui->txtEmuPath->text());
			item->setText(4, imgEmu);

			item->setSelected(true);
			ui->twEmus->setCurrentItem(item, 0);

			if (isAdd)
				ui->twEmus->addTopLevelItem(item);;

			fGrl->guardarDatosTwLista(ui->twEmus, grlDir.Datos +"emu_list.txt", TwListEmu);
			grlCfg.isChangedEmuList = true;

			cbxCatEmuShow->clear();
			cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/tag.png")         , tr("Todos")   , "all"     );
			cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/datos.png")   , tr("Datos")   , "datos"   );
			cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
			cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png") , tr("ScummVM") , "scummvm" );
			cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"), tr("VDMSound"), "vdmsound");

			emu_list.clear();
			emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");
			foreach (const stGrlDatos &dat, emu_list)
				cbxCatEmuShow->addItem(QIcon(fGrl->theme() +"img16/"+ dat.icono), dat.titulo, dat.key);

			ui->btnEmuUpdate->setEnabled(false);
		}
	}
}

void frmOpciones::on_txtEmuTitulo_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twEmus, ui->btnEmuUpdate, arg1, 0);
}

void frmOpciones::on_txtEmuDato_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twEmus, ui->btnEmuUpdate, arg1, 1);
}

void frmOpciones::on_txtEmuPath_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twEmus, ui->btnEmuUpdate, arg1, 3);
}

void frmOpciones::on_cbxEmuImg_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twEmus, ui->btnEmuUpdate, ui->cbxEmuImg->itemData(index).toString(), 4);
	else
		ui->btnEmuUpdate->setEnabled(false);
}

void frmOpciones::on_btnEmuPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona el ejecutable"), grlCfg.DirOtherEmus, "", tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtEmuPath->setText(fGrl->setDirRelative(archivo));
			grlCfg.DirOtherEmus = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmOpciones::on_btnEmuPath_clear_clicked()
{
	ui->txtEmuPath->clear();
}

void frmOpciones::on_btnEmuAdd_clicked()
{
	addEditTwEmus(true);
}

void frmOpciones::on_btnEmuUpdate_clicked()
{
	int pos = ui->twEmus->indexOfTopLevelItem(ui->twEmus->currentItem());
	if (ui->twEmus->topLevelItemCount() > 0 && pos != -1)
		addEditTwEmus(false);
}

void frmOpciones::on_btnEmuSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twEmus);
	fGrl->guardarDatosTwLista(ui->twEmus, grlDir.Datos +"emu_list.txt", TwListEmu);
	grlCfg.isChangedEmuList = true;
}

void frmOpciones::on_btnEmuBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twEmus);
	fGrl->guardarDatosTwLista(ui->twEmus, grlDir.Datos +"emu_list.txt", TwListEmu);
	grlCfg.isChangedEmuList = true;
}

void frmOpciones::on_btnEmuDelete_clicked()
{
	int pos = ui->twEmus->indexOfTopLevelItem(ui->twEmus->currentItem());
	if (ui->twEmus->topLevelItemCount() > 0 && pos != -1)
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este emulador de la lista?")))
		{
			fGrl->deleteItemTree(ui->twEmus->currentItem());
			fGrl->guardarDatosTwLista(ui->twEmus, grlDir.Datos +"emu_list.txt", TwListEmu);
			grlCfg.isChangedEmuList = true;
		}
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione un emulador de la lista para eliminarlo"));

	if (ui->twEmus->topLevelItemCount() <= 0)
	{
		ui->txtEmuTitulo->clear();
		ui->txtEmuDato->clear();
		ui->txtEmuPath->clear();
		int row = ui->cbxEmuImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
		if (row < 0) row = 0;
		ui->cbxEmuImg->setCurrentIndex(row);
	}
}

void frmOpciones::on_twEmus_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->txtEmuTitulo->setText(item->text(0));
		ui->txtEmuDato->setText(item->text(1));
		ui->txtEmuPath->setText(item->text(3));

		int row = ui->cbxEmuImg->findData(item->text(4), Qt::UserRole, Qt::MatchExactly);
		if (row < 0) row = 0;
		ui->cbxEmuImg->setCurrentIndex(row);
	}
}

void frmOpciones::on_twEmus_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		emit on_twEmus_itemClicked(current, 0);
}

// Categoría ----------------------------------------
void frmOpciones::addEditTwCategorias(bool isAdd)
{
	stGrlCats cat;
	QString tabla_old;
	bool siguiente = true;

	cat.titulo   = ui->txtCatNombre->text();
	cat.tabla    = ui->txtCatTabla->text().toLower();
	cat.img      = ui->cbxCatImg->itemData(ui->cbxCatImg->currentIndex()).toString();
	cat.emu_show = cbxCatEmuShow->getCheckedItemsUserData().isEmpty() ? "all" : cbxCatEmuShow->getCheckedItemsUserData();

	if (isAdd)
	{
		cat.total = 0;
		cat.orden = fGrl->intToStr(ui->twCategorias->topLevelItemCount());
	} else {
		tabla_old = ui->twCategorias->currentItem()->text(1);
		cat.total = ui->twCategorias->currentItem()->text(2).toInt();
		cat.id    = ui->twCategorias->currentItem()->text(3);
		cat.orden = ui->twCategorias->currentItem()->text(5);

		if (tabla_old == "dbgrl")
		{
			cat.tabla = "dbgrl";
			ui->txtCatTabla->setText(cat.tabla);
		}
	}

	if (cat.tabla.isEmpty())
	{
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner el nombre de la tabla para la nueva categoría."));
		ui->txtCatTabla->setFocus();
	} else {
		if (cat.titulo.isEmpty())
			QMessageBox::information(this, tr("Opciones"), tr("Debes poner el título de la nueva categoría."));
		else {
			QStringList lista_tablas = sql->listarTablas();
			QString tabla_tmp = cat.tabla +"_"+ fGrl->horaFechaActual(sql->getTime(), "ddMMyyyy_HHmmss");

			if (isAdd)
			{
				siguiente = true;
				if (lista_tablas.contains(cat.tabla))
				{
					siguiente = false;
					if (fGrl->questionMsg(tr("Ya existe") +"...", tr("¿La tabla ya existe, quieres usar el siguiente nombre para la tabla?") +"\n"+ tabla_tmp))
					{
						cat.tabla = tabla_tmp;
						siguiente = true;
					}
				}
			} else {
				if (QString::compare(tabla_old, cat.tabla, Qt::CaseInsensitive) == 0)
					siguiente  = true;
				else {
					if (lista_tablas.contains(cat.tabla))
					{
						siguiente = false;
						if (fGrl->questionMsg(tr("Ya existe") +"...", tr("¿La tabla ya existe, quieres usar el siguiente nombre para la tabla?") +"\n"+ tabla_tmp))
						{
							cat.tabla = tabla_tmp;
							ui->txtCatTabla->setText(cat.tabla);
							siguiente = sql->renombrarTabla(tabla_old, cat.tabla);
						} else {
							ui->txtCatTabla->setText(tabla_old);
							ui->txtCatTabla->setFocus();
						}
					} else
						siguiente = sql->renombrarTabla(tabla_old, cat.tabla);
				}
			}

			if (siguiente)
			{
				if (isAdd)
				{
					cat.id = sql->insertaCategoria(cat);
					if (cat.id.isEmpty())
					{
						QMessageBox::information(this, tr("Opciones"), tr("No se ha podido añadir la nueva categoría."));
						siguiente = false;
					} else {
						siguiente = true;
						fGrl->comprobarDirectorio(grlDir.DatosDbGrl + cat.tabla +"/");
					}
				} else {
					if (sql->actualizaCategoria(cat))
					{
						siguiente  = true;
						fGrl->renombrarDirectorio(grlDir.DatosDbGrl + tabla_old +"/", grlDir.DatosDbGrl + cat.tabla +"/");
					} else {
						QMessageBox::information(this, tr("Opciones"), tr("No se ha podido actualizar la categoría."));
						siguiente = false;
					}
				}

				if (siguiente)
				{
					QTreeWidgetItem *item = NULL;
					if (isAdd)
						item = new QTreeWidgetItem;
					else
						item = ui->twCategorias->currentItem();

					if (QFile::exists(fGrl->theme() +"img32/"+ cat.img))
						item->setIcon(0, QIcon(fGrl->theme() +"img32/"+ cat.img));
					else
						item->setIcon(0, QIcon(":/img32/"+ cat.img));

					item->setTextAlignment(2, Qt::AlignCenter);
					item->setText(0, cat.titulo  );
					item->setText(1, cat.tabla   );
					item->setText(2, fGrl->intToStr(cat.total));
					item->setText(3, cat.id      );
					item->setText(4, cat.img     );
					item->setText(5, cat.orden   );
					item->setText(6, cat.emu_show);

					if (isAdd)
						ui->twCategorias->addTopLevelItem(item);

					sql->comit();
					grlCfg.isChangedCategoria = true;
				}
			}
		}
	}
	ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::on_txtCatNombre_editingFinished()
{
	if (ui->txtCatTabla->text().isEmpty())
		ui->txtCatTabla->setText("db_"+ fGrl->eliminar_caracteres(ui->txtCatNombre->text()).toLower());
}

void frmOpciones::on_txtCatNombre_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twCategorias, ui->btnCatUpdate, arg1, 0);
}

void frmOpciones::on_txtCatTabla_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twCategorias, ui->btnCatUpdate, arg1, 1);
}

void frmOpciones::on_cbxCatImg_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twCategorias, ui->btnCatUpdate, ui->cbxCatImg->itemData(index).toString(), 4);
	else
		ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::cbxCatEmuShow_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twCategorias, ui->btnCatUpdate, cbxCatEmuShow->getCheckedItemsUserData(), 6);
	else
		ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::on_btnCatAdd_clicked()
{
	addEditTwCategorias(true);
}

void frmOpciones::on_btnCatUpdate_clicked()
{
	int pos = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if (ui->twCategorias->topLevelItemCount() > 0 && pos != -1)
		addEditTwCategorias(false);
}

void frmOpciones::on_btnCatSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twCategorias);
	sql->actualizaOrdenTreeWidget(ui->twCategorias, "dbgrl_categorias", "id", "orden", 3, 5);
	sql->comit();
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnCatBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twCategorias);
	sql->actualizaOrdenTreeWidget(ui->twCategorias, "dbgrl_categorias", "id", "orden", 3, 5);
	sql->comit();
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnCatEditTheme_clicked()
{
	emit on_btnEditTheme_clicked();
}

void frmOpciones::on_btnCatDelete_clicked()
{
	int pos = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if (ui->twCategorias->topLevelItemCount() > 0 && pos != -1)
	{
		if (ui->twCategorias->currentItem()->text(3).isEmpty())
			QMessageBox::information(this, tr("Opciones"), tr("Por favor selecciona una categoría de la lista para eliminarla"));
		else {
			if (ui->twCategorias->currentItem()->text(1) == "dbgrl")
				QMessageBox::information(this, tr("Opciones"), tr("No se puede eliminar la tabla de la categoría."));
			else {
				if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar esta categoría de la base de datos?")))
				{
					if (sql->eliminarCategoria(ui->twCategorias->currentItem()->text(3)))
					{
						fGrl->deleteItemTree(ui->twCategorias->currentItem());
						sql->actualizaOrdenTreeWidget(ui->twCategorias, "dbgrl_categorias", "id", "orden", 3, 5);
						sql->comit();
						grlCfg.isChangedCategoria = true;
					}
				}
			}
		}
	}
}

void frmOpciones::on_twCategorias_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->btnCatUpdate->setEnabled(false);
		ui->txtCatNombre->setText(item->text(0));
		ui->txtCatTabla->setText(item->text(1));
		ui->cbxCatImg->setCurrentIndex(ui->cbxCatImg->findData(item->text(4), Qt::UserRole, Qt::MatchExactly));
		cbxCatEmuShow->setCheckedItemsUserData(item->text(6));
	}
}

void frmOpciones::on_twCategorias_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		emit on_twCategorias_itemClicked(current, 0);
}

// MenuNav ------------------------------------------
void frmOpciones::addEditTwMnuNav(bool isAdd)
{
	QString id_mnu_nav, titulo, col_value, col_name, sql_query, archivo, img;
	bool expanded, mostrar;
	int orden = 0;

	titulo    = ui->txtMnuNavNombre->text();
	col_value = ui->cbxMnuNavColValue->itemText(ui->cbxMnuNavColValue->currentIndex());
	col_name  = ui->cbxMnuNavColName->itemData(ui->cbxMnuNavColName->currentIndex()).toString();
	sql_query = ui->cbxMnuNavSqlQuery->itemText(ui->cbxMnuNavSqlQuery->currentIndex());
	archivo   = ui->cbxMnuNavArchivo->itemData(ui->cbxMnuNavArchivo->currentIndex()).toString();
	img       = ui->cbxMnuNavImg->itemData(ui->cbxMnuNavImg->currentIndex()).toString();
	orden     = ui->twMnuNav->topLevelItemCount();
	expanded  = ui->chkMnuNavExpanded->isChecked();
	mostrar   = ui->chkMnuNavShow->isChecked();

	QTreeWidgetItem *item = NULL;
	if (isAdd)
	{
		id_mnu_nav = sql->insertaMenuNav(titulo, col_value, col_name, sql_query, archivo, img, orden, mostrar, expanded);
		item = new QTreeWidgetItem;//(ui->twMnuNav)
	} else {
		int pos = ui->twMnuNav->indexOfTopLevelItem(ui->twMnuNav->currentItem());
		if (ui->twMnuNav->topLevelItemCount() > 0 && pos != -1)
		{
			item  = ui->twMnuNav->currentItem();
			orden = fGrl->strToInt(item->text(8));
			sql->actualizaMenuNav(item->text(9), titulo, col_value, col_name, sql_query, archivo, img, orden, mostrar, expanded);
		} else {
			isAdd = true;
			item = new QTreeWidgetItem;//(ui->twMnuNav)
			id_mnu_nav = sql->insertaMenuNav(titulo, col_value, col_name, sql_query, archivo, img, orden, mostrar, expanded);
		}
		ui->btnMnuNavUpdate->setEnabled(false);
	}

	if (QFile::exists(fGrl->theme() +"img16/"+ img))
		item->setIcon(0, QIcon(fGrl->theme() +"img16/"+ img));
	else
		item->setIcon(0, QIcon(fGrl->theme() +"img16/sinimg.png"));
	item->setTextAlignment(1, Qt::AlignCenter);

	item->setText(0, titulo   );
	item->setText(1, col_value);
	item->setText(2, col_name );
	item->setText(3, sql_query);
	item->setText(4, archivo  );
	item->setText(5, fGrl->boolToStr(mostrar));
	item->setText(6, fGrl->boolToStr(expanded));
	item->setText(7, img);
	if (isAdd)
	{
		item->setText(8, fGrl->intToStr(orden));
		item->setText(9, id_mnu_nav);

		ui->twMnuNav->addTopLevelItem(item);
	}

	sql->comit();
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_txtMnuNavNombre_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, arg1, 0);
}

void frmOpciones::on_chkMnuNavExpanded_clicked(bool checked)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, fGrl->boolToStr(checked), 6);
}

void frmOpciones::on_chkMnuNavShow_clicked(bool checked)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, fGrl->boolToStr(checked), 5);
}

void frmOpciones::on_cbxMnuNavColName_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, ui->cbxMnuNavColName->itemData(index).toString(), 2);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNavImg_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, ui->cbxMnuNavImg->itemData(index).toString(), 7);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNavArchivo_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, ui->cbxMnuNavArchivo->itemData(index).toString(), 4);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNavColValue_editTextChanged(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, arg1, 1);
}

void frmOpciones::on_cbxMnuNavSqlQuery_editTextChanged(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuNav, ui->btnMnuNavUpdate, arg1, 3);
}

void frmOpciones::on_btnMnuNavAdd_clicked()
{
	addEditTwMnuNav(true);
}

void frmOpciones::on_btnMnuNavUpdate_clicked()
{
	addEditTwMnuNav(false);
}

void frmOpciones::on_btnMnuNavSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twMnuNav);
	sql->actualizaOrdenTreeWidget(ui->twMnuNav, "dbgrl_mnu_nav", "id", "orden", 9, 8);
	sql->comit();
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnMnuNavBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMnuNav);
	sql->actualizaOrdenTreeWidget(ui->twMnuNav, "dbgrl_mnu_nav", "id", "orden", 9, 8);
	sql->comit();
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnMnuNavDefecto_clicked()
{
	if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas restaurar el menú de navegación por defecto?")))
	{
		const int listSize = ui->twMnuNav->topLevelItemCount();
		if (listSize > 0)
		{
			for (int i = 0; i < listSize; ++i)
				sql->eliminarMenuNav(ui->twMnuNav->topLevelItem(i)->text(9));
		}

		sql->insertaMenuNav(tr("Generos")        , "{value}", "genero"       , "WHERE {col_name} LIKE ('%{col_value}%')", "generos.txt"  , "datos_3.png", 0, true, true);
		sql->insertaMenuNav(tr("Temas")          , "{value}", "tema"         , "WHERE {col_name} LIKE ('%{col_value}%')", "temas.txt"    , "datos_3.png", 1, true, true);
		sql->insertaMenuNav(tr("Desarrolladores"), "{value}", "desarrollador", "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 2, true, true);
		sql->insertaMenuNav(tr("Compañias")      , "{value}", "compania"     , "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 3, true, true);
		sql->insertaMenuNav(tr("Años")           , "{value}", "anno"         , "WHERE {col_name} LIKE ('%{col_value}%')", "fechas.txt"   , "fecha.png"  , 4, true, true);
		sql->insertaMenuNav(tr("Idiomas")        , "{value}", "idioma"       , "WHERE {col_name} LIKE ('%{col_value}%')", "idiomas.txt"  , "idiomas.png", 5, true, true);

		ui->twMnuNav->clear();
		sql->comit();
		sql->cargarMenuNav(ui->twMnuNav, "", true);
		grlCfg.isChangedCategoria = true;
	}
}

void frmOpciones::on_btnMnuNavDelete_clicked()
{
	int pos = ui->twMnuNav->indexOfTopLevelItem(ui->twMnuNav->currentItem());
	if (ui->twMnuNav->topLevelItemCount() > 0 && pos != -1)
	{
		if (ui->twMnuNav->currentItem()->text(9).isEmpty())
			QMessageBox::information(this, tr("Opciones"), tr("Por favor selecciona un menú nav de la lista para eliminarlo"));
		else {
			if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este menú nav de la base de datos?")))
			{
				if (sql->eliminarMenuNav(ui->twMnuNav->currentItem()->text(9)))
				{
					fGrl->deleteItemTree(ui->twMnuNav->currentItem());
					sql->actualizaOrdenTreeWidget(ui->twMnuNav, "dbgrl_mnu_nav", "id", "orden", 9, 8);
					sql->comit();
					grlCfg.isChangedCategoria = true;
				}
			}
		}
	}
}

void frmOpciones::on_twMnuNav_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->btnMnuNavUpdate->setEnabled(false);
		ui->txtMnuNavNombre->setText(item->text(0));
		ui->cbxMnuNavColValue->setCurrentIndex(ui->cbxMnuNavColValue->findText(item->text(1)));
		ui->cbxMnuNavColName->setCurrentIndex(ui->cbxMnuNavColName->findData(item->text(2)));
		ui->cbxMnuNavSqlQuery->setCurrentIndex(ui->cbxMnuNavSqlQuery->findText(item->text(3)));
		ui->cbxMnuNavArchivo->setCurrentIndex(ui->cbxMnuNavArchivo->findData(item->text(4)));
		ui->chkMnuNavShow->setChecked(fGrl->strToBool(item->text(5)));
		ui->chkMnuNavExpanded->setChecked(fGrl->strToBool(item->text(6)));
		ui->cbxMnuNavImg->setCurrentIndex(ui->cbxMnuNavImg->findData(item->text(7)));
	}
}

void frmOpciones::on_twMnuNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		emit on_twMnuNav_itemClicked(current, 0);
}

// MnuShortCut --------------------------------------
void frmOpciones::addEditTwMnuShortCut(bool isAdd)
{
	QString id_mnu_shortcut, titulo, key_sequence, sql_query, img;
	bool separador, mostrar;
	int orden = 0;

	separador = ui->chkMnuShortCutSeparator->isChecked();
	if (separador)
	{
		titulo       = "----";
		sql_query    = "";
		key_sequence = "";
		img          = "sinimg.png";
	} else {
		titulo       = ui->txtMnuShortCutTitulo->text();
		sql_query    = ui->cbxMnuShortCutSqlQuery->currentText();
		key_sequence = ui->txtShortcutKeySequence->text();
		img          = ui->cbxMnuShortCutImg->itemData(ui->cbxMnuShortCutImg->currentIndex()).toString();
	}
	orden   = ui->twMnuShortCut->topLevelItemCount();
	mostrar = ui->chkMnuShortCutShow->isChecked();

	QTreeWidgetItem *item = NULL;
	if (isAdd)
	{
		id_mnu_shortcut = sql->insertaMnuShortcut(titulo, key_sequence, sql_query, img, orden, mostrar, separador);
		item = new QTreeWidgetItem;//(ui->twMnuShortCut)
	} else {
		int pos = ui->twMnuShortCut->indexOfTopLevelItem(ui->twMnuShortCut->currentItem());
		if (ui->twMnuShortCut->topLevelItemCount() > 0 && pos != -1)
		{
			item  = ui->twMnuShortCut->currentItem();
			orden = fGrl->strToInt(item->text(6));
			sql->actualizaMnuShortcut(item->text(5), titulo, key_sequence, sql_query, img, orden, mostrar, separador);
		} else {
			isAdd = true;
			item = new QTreeWidgetItem;
			id_mnu_shortcut = sql->insertaMnuShortcut(titulo, key_sequence, sql_query, img, orden, mostrar, separador);
		}
		ui->btnMnuShortCutUpdate->setEnabled(false);
	}

	if (QFile::exists(fGrl->theme() +"img32/"+ img))
		item->setIcon(0, QIcon(fGrl->theme() +"img32/"+ img));
	else
		item->setIcon(0, QIcon(fGrl->theme() +"img32/sinimg.png"));

	item->setText(0, titulo   );
	item->setText(1, sql_query);
	item->setText(2, img      );
	item->setText(3, fGrl->boolToStr(mostrar));
	item->setText(4, fGrl->boolToStr(separador));
	if (isAdd)
	{
		item->setText(5, id_mnu_shortcut);
		item->setText(6, fGrl->intToStr(orden));
		ui->twMnuShortCut->addTopLevelItem(item);
	}
	item->setText(7, key_sequence);

	sql->comit();
	grlCfg.isChangedShortcut = true;
}

void frmOpciones::on_txtMnuShortCutTitulo_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuShortCut, ui->btnMnuShortCutUpdate, arg1, 0);
}

void frmOpciones::on_btnShortcutKeySequenceReset_clicked()
{
	list_key_sequence.clear();
	ui->txtShortcutKeySequence->clear();
	ui->txtShortcutKeySequence->setFocus();
}

void frmOpciones::on_chkMnuShortCutShow_toggled(bool checked)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuShortCut, ui->btnMnuShortCutUpdate, fGrl->boolToStr(checked), 3);
}

void frmOpciones::on_cbxMnuShortCutImg_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twMnuShortCut, ui->btnMnuShortCutUpdate, ui->cbxMnuShortCutImg->itemData(index).toString(), 2);
	else
		ui->btnMnuShortCutUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuShortCutSqlQuery_editTextChanged(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuShortCut, ui->btnMnuShortCutUpdate, arg1, 1);
}

void frmOpciones::on_txtShortcutKeySequence_textChanged(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twMnuShortCut, ui->btnMnuShortCutUpdate, arg1, 7);
}

void frmOpciones::on_btnMnuShortCutAdd_clicked()
{
	ui->chkMnuShortCutSeparator->setChecked(false);
	addEditTwMnuShortCut(true);
}

void frmOpciones::on_btnMnuShortCutAddSeparator_clicked()
{
	ui->chkMnuShortCutSeparator->setChecked(true);
	addEditTwMnuShortCut(true);
}

void frmOpciones::on_btnMnuShortCutUpdate_clicked()
{
	addEditTwMnuShortCut(false);
}

void frmOpciones::on_btnMnuShortCutSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twMnuShortCut);
	sql->actualizaOrdenTreeWidget(ui->twMnuShortCut, "dbgrl_mnu_shortcut", "id", "orden", 5, 6);
	sql->comit();
	grlCfg.isChangedShortcut = true;
}

void frmOpciones::on_btnMnuShortCutBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMnuShortCut);
	sql->actualizaOrdenTreeWidget(ui->twMnuShortCut, "dbgrl_mnu_shortcut", "id", "orden", 5, 6);
	sql->comit();
	grlCfg.isChangedShortcut = true;
}

void frmOpciones::on_btnMnuShortCutDefecto_clicked()
{
	if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas restaurar el menú de shortcut por defecto?")))
	{
		const int listSize = ui->twMnuShortCut->topLevelItemCount();
		if (listSize > 0)
		{
			for (int i = 0; i < listSize; ++i)
				sql->eliminarMnuShortcut(ui->twMnuShortCut->topLevelItem(i)->text(5));
		}

		QString id_shortcut = sql->insertaMnuShortcut(tr("Todos"), "", "", "gr-lida.png", 0, true, false);
		sql->comit();

		ui->twMnuShortCut->clear();
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->twMnuShortCut);
		item->setIcon(0, QIcon(fGrl->theme() +"img32/gr-lida.png"));
		item->setText(0, tr("Todos"));
		item->setText(1, "");
		item->setText(2, "gr-lida.png");
		item->setText(3, "true");
		item->setText(4, "false");
		item->setText(5, id_shortcut);
		item->setText(6, "0");
		item->setText(7, "");
		grlCfg.isChangedShortcut = true;
	}
}

void frmOpciones::on_btnMnuShortCutDelete_clicked()
{
	int pos = ui->twMnuShortCut->indexOfTopLevelItem(ui->twMnuShortCut->currentItem());
	if (ui->twMnuShortCut->topLevelItemCount() > 0 && pos != -1)
	{
		if (ui->twMnuShortCut->currentItem()->text(5).isEmpty())
			QMessageBox::information(this, tr("Opciones"), tr("Por favor selecciona un menú shortcut de la lista para eliminarlo"));
		else {
			if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este menú shortcut de la base de datos?")))
			{
				if (sql->eliminarMnuShortcut(ui->twMnuShortCut->currentItem()->text(5)))
				{
					fGrl->deleteItemTree(ui->twMnuShortCut->currentItem());
					sql->actualizaOrdenTreeWidget(ui->twMnuShortCut, "dbgrl_mnu_shortcut", "id", "orden", 5, 6);
					sql->comit();
					grlCfg.isChangedShortcut = true;
				}
			}
		}
	}
}

void frmOpciones::on_twMnuShortCut_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		bool separatorChecked = fGrl->strToBool(item->text(4));

		ui->btnMnuShortCutUpdate->setEnabled(false);
		ui->txtMnuShortCutTitulo->setText(item->text(0));
		ui->chkMnuShortCutShow->setChecked(fGrl->strToBool(item->text(3)));
		ui->chkMnuShortCutSeparator->setChecked(separatorChecked);
		ui->cbxMnuShortCutImg->setCurrentIndex(ui->cbxMnuShortCutImg->findData(item->text(2)));
		ui->cbxMnuShortCutSqlQuery->lineEdit()->setText(item->text(1));
		ui->txtShortcutKeySequence->setText(item->text(7));

		ui->txtMnuShortCutTitulo->setEnabled(!separatorChecked);
		ui->cbxMnuShortCutImg->setEnabled(!separatorChecked);
		ui->cbxMnuShortCutSqlQuery->setEnabled(!separatorChecked);

	}
}

void frmOpciones::on_twMnuShortCut_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		emit on_twMnuShortCut_itemClicked(current, 0);
}

// Datos --------------------------------------------
void frmOpciones::addEditDatosTwLista(bool isAdd)
{
	QString archivo, titulo, dato, extra, img, dir_img;
	int col_titulo, col_dato, col_nada;
	QFont m_font;
	m_font.setBold(true);

	archivo = ui->cbxDatArchivo->itemData(ui->cbxDatArchivo->currentIndex()).toString();
	titulo  = ui->txtDatNombre->text();
	dato    = ui->txtDatDato->text();
	extra   = ui->txtDatExtra->text();
	img     = ui->cbxDatImg->itemData(ui->cbxDatImg->currentIndex()).toString();

	col_titulo = 0;
	col_dato   = 1;
	col_nada   = 2;

	QTreeWidgetItem *item = NULL;
	if (isAdd)
		item = new QTreeWidgetItem;//(ui->twDatos)
	else {
		int pos = ui->twDatos->indexOfTopLevelItem(ui->twDatos->currentItem());
		if (ui->twDatos->topLevelItemCount() > 0 && pos != -1)
			item = ui->twDatos->currentItem();
		else {
			isAdd = true;
			item = new QTreeWidgetItem;//(ui->twDatos)
		}
		ui->btnDatUpdate->setEnabled(false);
	}

	if (archivo == "smiles.txt")
		dir_img = grlDir.Smiles;
	else if (archivo == "svm_lista.txt")
	{
		dir_img    = fGrl->theme() +"img_svm/";
		col_nada   = 0;
		col_titulo = 1;
		col_dato   = 2;

		if (titulo.isEmpty() && dato.isEmpty())
		{
			titulo = "";
			dato   = "";
			img    = "";
			item->setIcon(0, QIcon(fGrl->theme() +"img_svm/level/sinimg.png"));
		} else {
			if (dato.isEmpty())
			{
				item->setTextColor(1, QColor(0,0,0));
				item->setFont(1, m_font);
			} else
				extra = ui->cbxDatImgCmpt->itemData(ui->cbxDatImgCmpt->currentIndex()).toString().remove(".png");
		}

		if (extra.isEmpty())
			item->setIcon(0, QIcon(fGrl->theme() +"img_svm/level/sinimg.png"));
		else
			item->setIcon(0, QIcon(fGrl->theme() +"img_svm/level/"+ extra +".png"));
	}
//	else if (archivo == "idiomas.txt" || archivo == "svm_idioma.txt" || archivo == "dbx_keyboardlayout.txt")
//		dir_img = fGrl->theme() +"img16/";
	else
		dir_img = fGrl->theme() +"img16/";

	if (dato.isEmpty())
		extra  = "";

	if (QFile::exists(dir_img + img))
		item->setIcon(col_titulo, QIcon(dir_img + img));
	else
		item->setIcon(col_titulo, QIcon(dir_img +"sinimg.png"));

	item->setText(col_nada, "");
	item->setText(col_titulo, titulo);
	item->setText(col_dato, dato);
	item->setText(3, extra);
	item->setText(4, img);

	if (isAdd)
		ui->twDatos->addTopLevelItem(item);

	guardarDatosTwLista();
}

void frmOpciones::guardarDatosTwLista()
{
	QString archivo, sLng;
	archivo = ui->cbxDatArchivo->itemData(ui->cbxDatArchivo->currentIndex()).toString();
	sLng    = sql->getArchivoIsLng(archivo);

// Tipo de columnas, ScummVM, Smile, 2col, 3col...
	if (sql->getArchivoIs3Col(archivo))
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList3col);
	else if (archivo == "smiles.txt")
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSmile, "\",\"");
	else if (archivo == "dbx_keyboardlayout.txt")
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListEmu);
	else if (archivo == "svm_lista.txt")
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSvm);
	else
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList2col);
	grlCfg.isChangedCategoria    = true;
}

void frmOpciones::on_cbxDatArchivo_activated(int index)
{
	QString archivo, sLng;
	bool isLng = false;

	archivo = ui->cbxDatArchivo->itemData(index).toString();
	sLng    = sql->getArchivoIsLng(archivo);

	ui->lb_Opt_14->setText(tr("Dato") +":");
	ui->lb_Opt_dato->setVisible(false);
	ui->cbxDatImgCmpt->setVisible(false);
	ui->txtDatExtra->setVisible(false);
	ui->txtDatDato->setEnabled(false);
	ui->btnDatAddSmile->setEnabled(false);

	ui->txtDatNombre->setText("");
	ui->txtDatDato->setText("");
	ui->txtDatExtra->setText("");
	ui->cbxDatImg->setIconSize(QSize(16,16));
	ui->cbxDatImg->setMinimumHeight(24);

	ui->cbxDatImg->clear();

	if (archivo == "smiles.txt")
		fGrl->cargarIconosComboBox(ui->cbxDatImg, grlDir.Smiles, "", "", "sinimg.png", grlCfg.FormatsImage.join(";"));
	else if (archivo == "svm_lista.txt")
		fGrl->cargarIconosComboBox(ui->cbxDatImg, fGrl->theme(), "img_svm/", "", "sinimg.png", grlCfg.FormatsImage.join(";"));
	else if (archivo == "idiomas.txt" || archivo == "svm_idioma.txt" || archivo == "dbx_keyboardlayout.txt")
	{
		isLng = true;
		fGrl->cargarIconosComboBox(ui->cbxDatImg, fGrl->theme(), "img16/lng/", "lng/", "sinimg.png", grlCfg.FormatsImage.join(";"));
	} else {
		ui->cbxDatImg->addItemParent("Imágenes Categorías");
		fGrl->cargarIconosGroupComboBox(ui->cbxDatImg, fGrl->theme(), "img16/cat/", "cat/", grlCfg.FormatsImage.join(";"));
		ui->cbxDatImg->addItemParent("Imágenes defecto");
		fGrl->cargarIconosGroupComboBox(ui->cbxDatImg, fGrl->theme(), "img16/", "", grlCfg.FormatsImage.join(";"));
	}
	int row = ui->cbxDatImg->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
	ui->cbxDatImg->setCurrentIndex(row);

	ui->twDatos->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
	ui->twDatos->headerItem()->setIcon(1, QIcon(fGrl->theme() +"img16/bullet_black.png"));
	ui->twDatos->headerItem()->setIcon(2, QIcon(fGrl->theme() +"img16/bullet_black.png"));
	ui->twDatos->headerItem()->setIcon(3, QIcon(fGrl->theme() +"img16/bullet_black.png"));
	ui->twDatos->headerItem()->setText(0, tr("Título"));
	ui->twDatos->headerItem()->setText(1, "Config");
	ui->twDatos->headerItem()->setText(2, "");
	ui->twDatos->headerItem()->setText(3, "");
	ui->twDatos->headerItem()->setTextAlignment(1, Qt::AlignCenter);
	ui->twDatos->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twDatos->header()->setSectionsMovable(false);
	ui->twDatos->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twDatos->header()->setSectionResizeMode(1, QHeaderView::Interactive);
	ui->twDatos->header()->setSectionResizeMode(2, QHeaderView::Fixed);
	ui->twDatos->header()->setSectionResizeMode(3, QHeaderView::Interactive);
#else
	ui->twDatos->header()->setMovable(false);
	ui->twDatos->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twDatos->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDatos->header()->setResizeMode(2, QHeaderView::Fixed);
	ui->twDatos->header()->setResizeMode(3, QHeaderView::Interactive);
#endif
	ui->twDatos->setColumnWidth(0, 150);
	ui->twDatos->setColumnWidth(1,  90);
	ui->twDatos->setColumnWidth(2,  80);
	ui->twDatos->setColumnWidth(3,  80);
	ui->twDatos->setColumnHidden(1, true);
	ui->twDatos->setColumnHidden(2, true);
	ui->twDatos->setColumnHidden(3, true);

	ui->twDatos->clear();
// Tipo de columnas, ScummVM, Smile, 2col, 3col...
	if (sql->getArchivoIs3Col(archivo))
	{
		ui->txtDatDato->setEnabled(true);
		ui->twDatos->setColumnHidden(1, false);

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList3col, "|", isLng);
	}
	else if (archivo == "smiles.txt")
	{
		ui->btnDatAddSmile->setEnabled(true);
		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSmile, "\",\"", isLng);
	}
	else if (archivo == "dbx_keyboardlayout.txt")
	{
		ui->txtDatDato->setEnabled(true);
		ui->lb_Opt_dato->setVisible(true);
		ui->txtDatExtra->setVisible(true);

		ui->twDatos->headerItem()->setTextAlignment(1, Qt::AlignLeft);
		ui->twDatos->setColumnHidden(1, false);
		ui->twDatos->setColumnHidden(3, false);
		ui->twDatos->headerItem()->setText(1, tr("Etiqueta"));
		ui->twDatos->headerItem()->setText(3, tr("Config"));

		ui->lb_Opt_14->setText(tr("Etiqueta") +":");
		ui->lb_Opt_dato->setText(tr("Dato") +":");

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListEmu, "|", isLng);
	}
	else if (archivo == "svm_lista.txt")
	{
		ui->txtDatDato->setEnabled(true);
		ui->cbxDatImg->setIconSize(QSize(24,24));
		ui->cbxDatImg->setMinimumHeight(31);
		ui->cbxDatImgCmpt->setVisible(true);

		ui->twDatos->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/bullet_black.png"));
		ui->twDatos->headerItem()->setIcon(1, QIcon(fGrl->theme() +"img16/tag.png"));
		ui->twDatos->headerItem()->setText(0, "");
		ui->twDatos->headerItem()->setText(1, tr("Título"));
		ui->twDatos->headerItem()->setText(2, tr("Config"));
	#if QT_VERSION >= 0x050000
		ui->twDatos->header()->setSectionResizeMode(0, QHeaderView::Fixed);
		ui->twDatos->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	#else
		ui->twDatos->header()->setResizeMode(0, QHeaderView::Fixed);
		ui->twDatos->header()->setResizeMode(1, QHeaderView::Stretch);
	#endif
		ui->twDatos->setColumnWidth(0, 30);
		ui->twDatos->setColumnHidden(1, false);
		ui->twDatos->setColumnHidden(2, false);

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSvm, "|", isLng);
	} else
		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList2col, "|", isLng);
}

void frmOpciones::on_txtDatNombre_textEdited(const QString &arg1)
{
	QString archivo = ui->cbxDatArchivo->itemData(ui->cbxDatArchivo->currentIndex()).toString();
	int col_titulo = 0;

	if (archivo == "svm_lista.txt")
		col_titulo = 1;

	fGrl->enabledButtonUpdateTwList(ui->twDatos, ui->btnDatUpdate, arg1, col_titulo);
}

void frmOpciones::on_cbxDatImgCmpt_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twDatos, ui->btnDatUpdate, ui->cbxDatImgCmpt->itemData(index).toString().remove(".png"), 3);
	else
		ui->btnDatUpdate->setEnabled(false);
}

void frmOpciones::on_txtDatDato_textEdited(const QString &arg1)
{
	QString archivo = ui->cbxDatArchivo->itemData(ui->cbxDatArchivo->currentIndex()).toString();
	int col_dato = 1;

	if (archivo == "svm_lista.txt")
		col_dato = 2;

	fGrl->enabledButtonUpdateTwList(ui->twDatos, ui->btnDatUpdate, arg1, col_dato);
}

void frmOpciones::on_cbxDatImg_activated(int index)
{
	if (index > -1)
		fGrl->enabledButtonUpdateTwList(ui->twDatos, ui->btnDatUpdate, ui->cbxDatImg->itemData(index).toString(), 4);
	else
		ui->btnDatUpdate->setEnabled(false);
}

void frmOpciones::on_txtDatExtra_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twDatos, ui->btnDatUpdate, arg1, 3);
}

void frmOpciones::on_btnDatAdd_clicked()
{
	addEditDatosTwLista(true);
}

void frmOpciones::on_btnDatUpdate_clicked()
{
	addEditDatosTwLista(false);
}

void frmOpciones::on_btnDatSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twDatos);
	guardarDatosTwLista();
}

void frmOpciones::on_btnDatBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twDatos);
	guardarDatosTwLista();
}

void frmOpciones::on_btnDatAddSmile_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlDir.Smiles, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			if (QFile::exists(grlDir.Smiles + f_info.NameExt))
			{
				QString hora = "_"+ fGrl->horaFechaActual(sql->getTime(), "ddMMyyyy_HHmmss");

				if (QFile::copy(archivo, grlDir.Smiles + f_info.Name + hora + f_info.Ext))
					ui->cbxDatImg->addItem(QIcon(grlDir.Smiles + f_info.Name + hora + f_info.Ext), f_info.Name + hora, f_info.Name + hora + f_info.Ext);
			} else {
				if (QFile::copy(archivo, grlDir.Smiles + f_info.NameExt))
					ui->cbxDatImg->addItem(QIcon(grlDir.Smiles + f_info.NameExt), f_info.Name, f_info.NameExt);
			}
		}
	}
}

void frmOpciones::on_btnDatDelete_clicked()
{
	int pos = ui->twDatos->indexOfTopLevelItem(ui->twDatos->currentItem());
	if (ui->twDatos->topLevelItemCount() > 0 && pos != -1)
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este dato de la lista?")))
		{
			fGrl->deleteItemTree(ui->twDatos->currentItem());
			guardarDatosTwLista();
		}
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione un dato de la lista para eliminarlo"));
}

void frmOpciones::on_twDatos_itemClicked(QTreeWidgetItem *item, int column)
{
	ui->btnDatUpdate->setEnabled(false);

	if (item && column > -1)
	{
		QString archivo = ui->cbxDatArchivo->itemData(ui->cbxDatArchivo->currentIndex()).toString();
		int col_titulo = 0;
		int col_dato   = 1;

		if (sql->getArchivoIs3Col(archivo) || archivo == "svm_lista.txt" || archivo == "dbx_keyboardlayout.txt")
		{
			ui->txtDatDato->setEnabled(true);
			if (archivo == "dbx_keyboardlayout.txt")
				ui->txtDatExtra->setText(item->text(3));

			if (archivo == "svm_lista.txt")
			{
				col_titulo = 1;
				col_dato   = 2;
				QString svm_icon = item->text(3).isEmpty() ? "sinimg.png" : item->text(3) +".png";

				ui->cbxDatImgCmpt->setCurrentIndex(ui->cbxDatImgCmpt->findData(svm_icon, Qt::UserRole, Qt::MatchExactly));
			}
		} else
			ui->txtDatDato->setEnabled(false);

		ui->txtDatNombre->setText(item->text(col_titulo));
		ui->txtDatDato->setText(item->text(col_dato));

		QString dato_icon = item->text(4).isEmpty() ? "sinimg.png" : item->text(4);

		int row = ui->cbxDatImg->findData(dato_icon, Qt::UserRole, Qt::MatchExactly);
		if (row < 0) row = 0;
		ui->cbxDatImg->setCurrentIndex(row);
	}
}

void frmOpciones::on_twDatos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current != previous)
		emit on_twDatos_itemClicked(current, 0);
}

// Base de datos-------------------------------------
void frmOpciones::on_btnDirDB_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.DirBD, "", tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDirBD->setText(fGrl->setDirRelative(archivo));
			grlCfg.DirBD = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmOpciones::on_btnDirDB_clear_clicked()
{
	ui->txtDirBD->clear();
}

// Style themes -------------------------------------
void frmOpciones::on_cbxStyle_activated(int index)
{
	grlCfg.Style = ui->cbxStyle->itemData(index).toString();
	QApplication::setStyle(QStyleFactory::create(grlCfg.Style));

	emit on_chkStylePalette_clicked(ui->chkStylePalette->isChecked());
}

void frmOpciones::on_chkStylePalette_clicked(bool checked)
{
	if (checked)
		QApplication::setPalette(QApplication::style()->standardPalette());
	else
		QApplication::setPalette(QApplication::palette());
}

void frmOpciones::on_cbxIconFav_activated(int index)
{
	if (index > -1)
	{
		if (grlCfg.IconoFav != ui->cbxIconFav->itemData(index).toString())
			grlCfg.isChangedFavorito = true;
		else
			grlCfg.isChangedFavorito = false;
	}
}

void frmOpciones::on_chkUsarTipoFuente_clicked(bool checked)
{
	grlCfg.font_usar = checked;

	QString font_usar = grlCfg.font_usar ? "*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}" : "";
	this->setStyleSheet(fGrl->myStyleSheet("StyleSheet.qss") + font_usar);
}

void frmOpciones::on_cbxFuenteTipo_activated(const QString &arg1)
{
	grlCfg.font_family = arg1;
	emit on_chkUsarTipoFuente_clicked(grlCfg.font_usar);
}

void frmOpciones::on_cbxFontSize_valueChanged(const QString &arg1)
{
	grlCfg.font_size = arg1;
	emit on_chkUsarTipoFuente_clicked(grlCfg.font_usar);
}
void frmOpciones::on_btnEditTheme_clicked()
{
	int id_cat = 1;
	int pos_cat = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if (ui->twCategorias->topLevelItemCount() > 0 && pos_cat != -1)
		id_cat = ui->twCategorias->currentItem()->text(3).toInt();

	frmListIconCfg *ListIconCfg = new frmListIconCfg(sql, grlCfg, id_cat, this);
	ListIconCfg->setWindowFlags(Qt::Window);
	ListIconCfg->exec();
	delete ListIconCfg ;
}

void frmOpciones::on_twThemes_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		grlCfg.NameDirTheme = item->text(0);

		fGrl->setTheme(grlCfg.NameDirTheme);

		if (QFile::exists(fGrl->theme() +"preview.png"))
			ui->lb_theme_example->setPixmap(QPixmap(fGrl->theme() +"preview.png"));
		else
			ui->lb_theme_example->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));

		cargarConfig();
		setTheme();
	} else
		ui->lb_theme_example->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
}

void frmOpciones::on_twThemes_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(current);
	Q_UNUSED(previous);
//	if (current != previous)
//		emit on_twThemes_itemClicked(current, 0);
}

// PictureFlow --------------------------------------
void frmOpciones::on_cbxPicFlowType_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowType = ui->cbxPicFlowType->itemData(index).toString();
		if (grlCfg.PicFlowType.toLower() == "strip")
			grl_picflow->setFlowType(PictureFlow::Strip);
		else if (grlCfg.PicFlowType.toLower() == "stripoverlapped")
			grl_picflow->setFlowType(PictureFlow::StripOverlapped);
		else
			grl_picflow->setFlowType(PictureFlow::CoverFlowLike);
	} else {
		grlCfg.PicFlowType = "CoverFlowLike";
		grl_picflow->setFlowType(PictureFlow::CoverFlowLike);
	}
}

void frmOpciones::on_btnPicFlowBgColor_clicked()
{
	QColor color = QColorDialog::getColor(fGrl->getColor(grlCfg.PicFlowBgColor), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btnPicFlowBgColor, color);
		grlCfg.PicFlowBgColor.clear();
		grlCfg.PicFlowBgColor = fGrl->setColor(color);
		grl_picflow->setBackgroundColor(color);
	}
}

void frmOpciones::on_cbxPicFlowFontFamily_activated(const QString &arg1)
{
	grlCfg.PicFlowFontFamily = arg1;
	grl_picflow->setPicFlowFont(arg1, grlCfg.PicFlowFontSize);
}

void frmOpciones::on_spinBox_PicFlowFontSize_valueChanged(int arg1)
{
	grlCfg.PicFlowFontSize = arg1;
	grl_picflow->setPicFlowFont(grlCfg.PicFlowFontFamily, arg1);
}

void frmOpciones::on_chkPicFlowFontBold_toggled(bool checked)
{
	grlCfg.PicFlowFontBold = checked;
	grl_picflow->setPicFlowFont(grlCfg.PicFlowFontFamily, grlCfg.PicFlowFontSize, checked, grlCfg.PicFlowFontItalic);
}

void frmOpciones::on_chkPicFlowFontItalic_toggled(bool checked)
{
	grlCfg.PicFlowFontItalic = checked;
	grl_picflow->setPicFlowFont(grlCfg.PicFlowFontFamily, grlCfg.PicFlowFontSize, grlCfg.PicFlowFontBold, checked);
}

void frmOpciones::on_btnPicFlowFontColor_clicked()
{
	QColor color = QColorDialog::getColor(fGrl->getColor(grlCfg.PicFlowFontColor), this, tr("Color de texto"),  QColorDialog::DontUseNativeDialog);

	if (color.isValid())
	{
		setColorBtn(ui->btnPicFlowFontColor, color);
		grlCfg.PicFlowFontColor.clear();
		grlCfg.PicFlowFontColor = fGrl->setColor(color);
		grl_picflow->setFontColor(color);
	}
}

void frmOpciones::on_chkPicFlowShowTitle_toggled(bool checked)
{
	grlCfg.PicFlowShowTitle = checked;
	grl_picflow->setShowTitle(checked);
}

void frmOpciones::on_cbxPicFlowTitlePos_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowTitlePos = index;
		grl_picflow->setSlideTitlePos(fGrl->getPosition(index));
	} else {
		grlCfg.PicFlowTitlePos = 0;
		grl_picflow->setSlideTitlePos(Qt::AlignLeft | Qt::AlignTop);
	}
}

void frmOpciones::on_spinBox_PicFlowTitleMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowTitleMargin = QPoint(arg1, grlCfg.PicFlowTitleMargin.y());
	grl_picflow->setSlideTitleMargin(grlCfg.PicFlowTitleMargin);
}

void frmOpciones::on_spinBox_PicFlowTitleMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowTitleMargin = QPoint(grlCfg.PicFlowTitleMargin.x(), arg1);
	grl_picflow->setSlideTitleMargin(grlCfg.PicFlowTitleMargin);
}

void frmOpciones::on_chkPicFlowShowTitleIcon_toggled(bool checked)
{
	grlCfg.PicFlowShowTitleIcon = checked;
	grl_picflow->setShowTitleIcon(checked);
}

void frmOpciones::on_cbxPicFlowTitleIconPos_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowTitleIconPos = index;
		grl_picflow->setSlideTitleIconPos(index);
	} else {
		grlCfg.PicFlowTitleIconPos = 0;
		grl_picflow->setSlideTitleIconPos(0);
	}
}

void frmOpciones::on_spinBox_PicFlowTitleIconMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowTitleIconMargin = QPoint(arg1, grlCfg.PicFlowTitleIconMargin.y());
	grl_picflow->setSlideTitleIconMargin(grlCfg.PicFlowTitleIconMargin);
}

void frmOpciones::on_spinBox_PicFlowTitleIconMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowTitleIconMargin = QPoint(grlCfg.PicFlowTitleIconMargin.x(), arg1);
	grl_picflow->setSlideTitleIconMargin(grlCfg.PicFlowTitleIconMargin);
}

void frmOpciones::on_chkPicFlowShowCaption_toggled(bool checked)
{
	grlCfg.PicFlowShowCaption = checked;
	grl_picflow->setShowCaption(checked);
}

void frmOpciones::on_cbxPicFlowCaptionPos_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowCaptionPos = index;
		grl_picflow->setSlideCaptionPos(fGrl->getPosition(index));
	} else {
		grlCfg.PicFlowCaptionPos = 7;
		grl_picflow->setSlideCaptionPos(Qt::AlignHCenter | Qt::AlignBottom);
	}
}

void frmOpciones::on_spinBox_PicFlowCaptionMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowCaptionMargin = QPoint(arg1, grlCfg.PicFlowCaptionMargin.y());
	grl_picflow->setSlideCaptionMargin(grlCfg.PicFlowCaptionMargin);
}

void frmOpciones::on_spinBox_PicFlowCaptionMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowCaptionMargin = QPoint(grlCfg.PicFlowCaptionMargin.x(), arg1);
	grl_picflow->setSlideCaptionMargin(grlCfg.PicFlowCaptionMargin);
}

void frmOpciones::on_chkPicFlowShowNumber_toggled(bool checked)
{
	grlCfg.PicFlowShowNumber = checked;
	grl_picflow->setShowNumber(checked);
}

void frmOpciones::on_cbxPicFlowNumberPos_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowNumberPos = index;
		grl_picflow->setSlideNumberPos(fGrl->getPosition(index));
	} else {
		grlCfg.PicFlowNumberPos = 2;
		grl_picflow->setSlideNumberPos(Qt::AlignRight | Qt::AlignTop);
	}
}

void frmOpciones::on_spinBox_PicFlowNumberMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowNumberMargin = QPoint(arg1, grlCfg.PicFlowNumberMargin.y());
	grl_picflow->setSlideNumberMargin(grlCfg.PicFlowNumberMargin);
}

void frmOpciones::on_spinBox_PicFlowNumberMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowNumberMargin = QPoint(grlCfg.PicFlowNumberMargin.x(), arg1);
	grl_picflow->setSlideNumberMargin(grlCfg.PicFlowNumberMargin);
}

void frmOpciones::on_txtPicFlowNumberFormat_textChanged(const QString &arg1)
{
	grlCfg.PicFlowNumberFormat = arg1;
	grl_picflow->setSlideNumberFormat(arg1);
}

void frmOpciones::on_chkPicFlowShowIconExtra_toggled(bool checked)
{
	grlCfg.PicFlowShowIconExtra = checked;
	grl_picflow->setShowIconExtra(checked);
}

void frmOpciones::on_cbxPicFlowIconExtraPos_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowIconExtraPos = index;
		grl_picflow->setSlideIconExtraPos(index);
	} else {
		grlCfg.PicFlowIconExtraPos = 2;
		grl_picflow->setSlideIconExtraPos(2);
	}
}

void frmOpciones::on_spinBox_PicFlowIconExtraMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowIconExtraMargin = QPoint(arg1, grlCfg.PicFlowIconExtraMargin.y());
	grl_picflow->setSlideIconExtraMargin(grlCfg.PicFlowIconExtraMargin);
}

void frmOpciones::on_spinBox_PicFlowIconExtraMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowIconExtraMargin = QPoint(grlCfg.PicFlowIconExtraMargin.x(), arg1);
	grl_picflow->setSlideIconExtraMargin(grlCfg.PicFlowIconExtraMargin);
}

void frmOpciones::on_cbxPicFlowIconExtraUse_activated(int index)
{
	if (index > -1)
	{
		grlCfg.PicFlowIconExtraUse = ui->cbxPicFlowIconExtraUse->itemData(index).toString();

		QString iconExtraImg = "datos_3.png";
		ui->cbxPicFlowIconExtraImage->setDisabled(false);

		if (grlCfg.PicFlowIconExtraUse == "favorito")
			iconExtraImg = grlCfg.PicFlowIconExtraImgFavorito;
		else if (grlCfg.PicFlowIconExtraUse == "original")
			iconExtraImg = grlCfg.PicFlowIconExtraImgOriginal;
		else
			ui->cbxPicFlowIconExtraImage->setDisabled(true);

		int row = ui->cbxPicFlowIconExtraImage->findData(iconExtraImg, Qt::UserRole, Qt::MatchContains);
		ui->cbxPicFlowIconExtraImage->setCurrentIndex(row);

		grl_picflow->setSlideIconExtra(fGrl->theme() +"img32/"+ iconExtraImg);
	}
}

void frmOpciones::on_cbxPicFlowIconExtraImage_activated(int index)
{
	if (index > -1)
	{
		QString iconExtraImg =  ui->cbxPicFlowIconExtraImage->itemData(index).toString();

		if (grlCfg.PicFlowIconExtraUse == "favorito")
			grlCfg.PicFlowIconExtraImgFavorito = iconExtraImg;
		if (grlCfg.PicFlowIconExtraUse == "original")
			grlCfg.PicFlowIconExtraImgOriginal = iconExtraImg;

		grl_picflow->setSlideIconExtra(fGrl->theme() +"img32/"+ iconExtraImg);
	}
}

void frmOpciones::on_spinBox_PicFlowMarginX_valueChanged(int arg1)
{
	grlCfg.PicFlowMargin = QPoint(arg1, grlCfg.PicFlowMargin.y());
	grl_picflow->setSlideMargin(grlCfg.PicFlowMargin);
}

void frmOpciones::on_spinBox_PicFlowMarginY_valueChanged(int arg1)
{
	grlCfg.PicFlowMargin = QPoint(grlCfg.PicFlowMargin.x(), arg1);
	grl_picflow->setSlideMargin(grlCfg.PicFlowMargin);
}

void frmOpciones::on_chkPicFlowShowTriangle_toggled(bool checked)
{
	grlCfg.PicFlowShowTriangle = checked;
	grl_picflow->setShowTriangle(checked);
}

//---------------------------------------------------
void frmOpciones::on_btnExtVideoAdd_clicked()
{
	if (!ui->txtExtVideo->text().isEmpty())
	{
		if (ui->txtExtVideo->text().startsWith("*."))
			ui->lwExtVideo->addItem(ui->txtExtVideo->text());
		else
			ui->lwExtVideo->addItem("*."+ ui->txtExtVideo->text());
	}
}

void frmOpciones::on_btnExtVideoDelete_clicked()
{
	if (ui->lwExtVideo->count() > 0 && ui->lwExtVideo->currentItem()->isSelected())
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar la extensión?"))
			delete ui->lwExtVideo->currentItem();
	}
}

void frmOpciones::on_btnExtVideoDefecto_clicked()
{
	grlCfg.FormatsVideo.clear();
	grlCfg.FormatsVideo << "*.avi" << "*.mkv" << "*.mov" << "*.mp4" << "*.mpeg" << "*.mpg" << "*.wmv";

	ui->lwExtVideo->clear();
	ui->lwExtVideo->addItems(grlCfg.FormatsVideo);
	ui->lwExtVideo->setCurrentRow(0);
}

void frmOpciones::on_btnExtMusicAdd_clicked()
{
	if (!ui->txtExtMusic->text().isEmpty())
	{
		if (ui->txtExtMusic->text().startsWith("*."))
			ui->lwExtMusic->addItem(ui->txtExtMusic->text());
		else
			ui->lwExtMusic->addItem("*."+ ui->txtExtMusic->text());
	}
}

void frmOpciones::on_btnExtMusicDelete_clicked()
{
	if (ui->lwExtMusic->count() > 0 && ui->lwExtMusic->currentItem()->isSelected())
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar la extensión?"))
			delete ui->lwExtMusic->currentItem();
	}
}

void frmOpciones::on_btnExtMusicDefecto_clicked()
{
	grlCfg.FormatsMusic.clear();
	grlCfg.FormatsMusic << "*.mp3" << "*.ogg" << "*.wav" << "*.wma" << "*.m4a";

	ui->lwExtMusic->clear();
	ui->lwExtMusic->addItems(grlCfg.FormatsMusic);
	ui->lwExtMusic->setCurrentRow(0);
}

void frmOpciones::on_btnExtImageAdd_clicked()
{
	if (!ui->txtExtImage->text().isEmpty())
	{
		if (ui->txtExtImage->text().startsWith("*."))
			ui->lwExtImage->addItem(ui->txtExtImage->text());
		else
			ui->lwExtImage->addItem("*."+ ui->txtExtImage->text());
	}
}

void frmOpciones::on_btnExtImageDelete_clicked()
{
	if (ui->lwExtImage->count() > 0 && ui->lwExtImage->currentItem()->isSelected())
	{
		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar la extensión?"))
			delete ui->lwExtImage->currentItem();
	}
}

void frmOpciones::on_btnExtImageDefecto_clicked()
{
	grlCfg.FormatsImage.clear();
	grlCfg.FormatsImage << "*.bmp" << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.pbm" << "*.pgm" << "*.ppm" << "*.tiff" << "*.xbm" << "*.xpm" << "*.svg";

	ui->lwExtImage->clear();
	ui->lwExtImage->addItems(grlCfg.FormatsImage);
	ui->lwExtImage->setCurrentRow(0);
}
