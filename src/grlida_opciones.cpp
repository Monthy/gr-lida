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

#include "grlida_opciones.h"
#include "grlida_list_icon_cfg.h"
#include "ui_opciones.h"

frmOpciones::frmOpciones(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmOpciones)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql    = m_sql;
	grlCfg = m_cfg;

	grlDir.Home     = fGrl->GRlidaHomePath();
	grlDir.Capturas = grlDir.Home +"capturas/";
	grlDir.Confdbx  = grlDir.Home +"confdbx/";
	grlDir.Confvdms = grlDir.Home +"confvdms/";
	grlDir.Covers   = grlDir.Home +"covers/";
	grlDir.Datos    = grlDir.Home +"datos/";
	grlDir.Idiomas  = grlDir.Home +"idiomas/";
	grlDir.Scripts  = grlDir.Home +"scripts/";
	grlDir.Smiles   = grlDir.Home +"smiles/";
	grlDir.Themes   = grlDir.Home +"themes/";
	grlDir.Thumbs   = grlDir.Home +"thumbs/";

	IdiomaSelect = grlCfg.IdiomaSelect;

	fGrl->setTheme(grlCfg.NameDirTheme);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	createWidgets();
	cargarConfig();
	setTheme();
	cargarListaThemes();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmOpciones::~frmOpciones()
{
	delete cbxCat_EmuShow;
	delete fGrl;
	delete ui;
}

void frmOpciones::closeEvent(QCloseEvent *event)
{
	emit on_btnOk_clicked();
	event->accept();
}

void frmOpciones::createWidgets()
{
	cbxCat_EmuShow = new QCheckComboBox(ui->gBox_categorias);
	cbxCat_EmuShow->setMinimumHeight(24);

	ui->gridLayout_categorias->addWidget(cbxCat_EmuShow, 2, 1, 1, 1);
}

void frmOpciones::cargarConfig()
{
	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);

// Opciones Generales ----------------
	ui->lwOpciones->setCurrentRow( grlCfg.LastTabOptSelect );

// DOSBox ----------------------------
	ui->chkDOSBoxDisp->setChecked( grlCfg.DOSBoxDisp );
	regexp.setPattern("[a-z-_0-9]+");
	ui->txtDbxDato->setValidator(new QRegExpValidator(regexp, ui->txtDbxDato));

	ui->cbxDbxVersion->clear();
	ui->cbxDbxVersion->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png"), "0.72", "0.72");
	ui->cbxDbxVersion->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png"), "0.73", "0.73");
	ui->cbxDbxVersion->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png"), "0.74", "0.74");

	ui->twDOSBox->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png") );
	ui->twDOSBox->headerItem()->setIcon(1, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
	ui->twDOSBox->headerItem()->setIcon(4, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
	ui->twDOSBox->headerItem()->setTextAlignment(2, Qt::AlignCenter );
	ui->twDOSBox->headerItem()->setTextAlignment(3, Qt::AlignCenter );
	ui->twDOSBox->header()->setMovable(false);
	ui->twDOSBox->header()->setStretchLastSection(false);
	ui->twDOSBox->header()->setResizeMode(0, QHeaderView::Stretch     );
	ui->twDOSBox->header()->setResizeMode(1, QHeaderView::Interactive );
	ui->twDOSBox->header()->setResizeMode(2, QHeaderView::Interactive );
	ui->twDOSBox->header()->setResizeMode(3, QHeaderView::Interactive );
	ui->twDOSBox->header()->setResizeMode(4, QHeaderView::Interactive );
	ui->twDOSBox->setColumnWidth(1, 90);
	ui->twDOSBox->setColumnWidth(2, 50);
	ui->twDOSBox->setColumnWidth(3, 50);
	ui->twDOSBox->setColumnWidth(4, 50);

	fGrl->cargarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx, "|");

	emit on_btnDbxCancel_clicked();

// ScummVM ---------------------------
	ui->chkScummVMDisp->setChecked( grlCfg.ScummVMDisp );
	ui->txtSvmPath->setText( grlCfg.DirScummVM );

// VDMSound --------------------------
	ui->chkVDMSoundDisp->setChecked( grlCfg.VDMSoundDisp );

// Otras Opciones --------------------
	ui->txtDirBaseGames->setText( grlCfg.DirBaseGames );
	fGrl->cargarScriptsComboBox(ui->cbxScriptURL, grlDir.Scripts, grlCfg.url_xmldb);
	ui->txtFormatoFecha->setText( grlCfg.FormatoFecha );
	ui->chkComprobarVersionGrl->setChecked( grlCfg.chkVersion );
	ui->chkShowNext->setChecked( grlCfg.Primeravez );
	ui->chkOpenPdfExternal->setChecked( grlCfg.OpenPdfExternal );

// Crear, editar categorías ----------
	ui->twCategorias->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png") );
	ui->twCategorias->headerItem()->setTextAlignment(2, Qt::AlignCenter );
	ui->twCategorias->header()->setMovable(false);
	ui->twCategorias->header()->setStretchLastSection(false);
	ui->twCategorias->header()->setResizeMode(0, QHeaderView::Stretch     );
	ui->twCategorias->header()->setResizeMode(1, QHeaderView::Interactive );
	ui->twCategorias->header()->setResizeMode(2, QHeaderView::Fixed       );
	ui->twCategorias->setColumnWidth(1, 90);
	ui->twCategorias->setColumnWidth(2, 50);

	regexp.setPattern("[A-Za-z-_0-9]+");
	ui->txtCat_Tabla->setValidator(new QRegExpValidator(regexp, ui->txtCat_Tabla));

	fGrl->cargarIconosComboBox(ui->cbxCat_Img, fGrl->ThemeApp() +"img16_cat/", "sinimg.png", grlCfg.FormatsImage.join(";"));
	sql->cargarCategorias(ui->twCategorias);

	emu_list.clear();
	emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");

	connect(cbxCat_EmuShow, SIGNAL(activated(int)), this, SLOT(cbxCat_EmuShow_activated(int)));

// Crear, editar menú nav ------------
	ui->cbxMnuNav_ColValue->clear();
	ui->cbxMnuNav_ColValue->addItem(QIcon(fGrl->Theme() +"img16/basedatos.png"), "{value}");
	ui->cbxMnuNav_ColValue->addItem(QIcon(fGrl->Theme() +"img16/basedatos.png"), "true"   );
	ui->cbxMnuNav_ColValue->addItem(QIcon(fGrl->Theme() +"img16/basedatos.png"), "false"  );
	ui->cbxMnuNav_ColValue->setCurrentIndex(0);

	ui->cbxMnuNav_SqlQuery->clear();
	ui->cbxMnuNav_SqlQuery->addItem(QIcon(fGrl->Theme() +"img16/basedatos.png"), "WHERE {col_name} LIKE ('%{col_value}%')");
	ui->cbxMnuNav_SqlQuery->addItem(QIcon(fGrl->Theme() +"img16/basedatos.png"), "WHERE {col_name}='{col_value}'"     );
	ui->cbxMnuNav_SqlQuery->setCurrentIndex(0);

	fGrl->cargarIconosComboBox(ui->cbxMnuNav_Img, fGrl->Theme() +"img16/", "sinimg.png", grlCfg.FormatsImage.join(";"));

// Editar datos, configuraciones -----
	fGrl->cargarIconosComboBox(ui->cbxDat_Img    , fGrl->Theme() +"img16/"  , "sinimg.png", grlCfg.FormatsImage.join(";"));
	fGrl->cargarIconosComboBox(ui->cbxDat_ImgCmpt, fGrl->Theme() +"img_svm/", "sinimg.png", grlCfg.FormatsImage.join(";"));

	regexp.setPattern("[A-Za-z-_0-9]+");
	ui->txtDat_Dato->setValidator(new QRegExpValidator(regexp, ui->txtDat_Dato));

// Opciones de base de datos ---------
	ui->txtDirBD->setText( fGrl->setDirRelative(grlCfg.db_host) );
	ui->txt_dbserver->setText( grlCfg.db_server );
	ui->txt_dbname->setText( grlCfg.db_name );
	ui->txt_dbusername->setText( grlCfg.db_username );
	ui->txt_dbpassword->setText( grlCfg.db_password );
	ui->txt_dbport->setText( grlCfg.db_port );
	ui->txt_dbport->setValidator(new QIntValidator(ui->txt_dbport));

// Opciones de Estilos y apariencia --
	ui->cbxStyle->clear();
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style Default"     , "Default"      );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/win9x.png"), "Style Windows"     , "Windows"      );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/winxp.png"), "Style WindowsXP"   , "WindowsXP"    );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style WindowsVista", "WindowsVista" );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style Motif"       , "Motif"        );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style CDE"         , "CDE"          );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style Plastique"   , "Plastique"    );
	ui->cbxStyle->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "Style Cleanlooks"  , "Cleanlooks"   );

	int index_style = ui->cbxStyle->findData(grlCfg.Style);
	if( index_style < 0 ) index_style = 0;
	ui->cbxStyle->setCurrentIndex( index_style );
	emit on_cbxStyle_activated( index_style );

	ui->chkStylePalette->setChecked( grlCfg.StylePalette );
	ui->chkToolbarBigIcon->setChecked( grlCfg.ToolbarBigIcon );

// Opciones fuente -------------------
	ui->chkUsarTipoFuente->setChecked( grlCfg.font_usar );
	ui->cbxFuenteTipo->setCurrentIndex( ui->cbxFuenteTipo->findText(grlCfg.font_family, Qt::MatchContains) );
	ui->sbxFontSize->setValue( grlCfg.font_size.toInt() );

// PictureFlow -----------------------
	ui->spinBox_SkipPicFlow->setValue( grlCfg.Skip_PicFlow );
	ui->spinBox_MinHeightPicFlow->setValue( grlCfg.PicFlowMinHeight  );

	setColorBtn( fGrl->getColor(grlCfg.PicFlowBgColor) );
	ui->chkPicFlowShowNumPos->setChecked(grlCfg.PicFlowShowNumPos);
	ui->chkPicFlowShowTriangle->setChecked(grlCfg.PicFlowShowTriangle);

// Apariencia ------------------------
	ui->twThemes->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png"));
	ui->twThemes->headerItem()->setTextAlignment(1, Qt::AlignCenter);
	ui->twThemes->headerItem()->setTextAlignment(2, Qt::AlignCenter);
	ui->twThemes->header()->setMovable(false);
	ui->twThemes->header()->setStretchLastSection(false);
	ui->twThemes->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twThemes->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twThemes->header()->setResizeMode(2, QHeaderView::Fixed);
	ui->twThemes->setColumnWidth(1, 90);
	ui->twThemes->setColumnWidth(2, 50);

// Opciones del Servidor Proxy -------
	ui->chkEnableProxy->setChecked( grlCfg.ProxyEnable );
	ui->txt_px_host->setText( grlCfg.ProxyHost );
	ui->txt_px_puerto->setText( grlCfg.ProxyPort );
	ui->txt_px_puerto->setValidator(new QIntValidator(ui->txt_px_puerto));
	ui->chkLogin->setChecked( grlCfg.ProxyLogin );
	ui->txt_px_username->setText( grlCfg.ProxyUserName );
	ui->txt_px_password->setText( grlCfg.ProxyPassword );

// Otras Opciones --------------------
	ui->lwExtVideo->clear();
	ui->lwExtVideo->addItems(grlCfg.FormatsVideo);
	ui->lwExtVideo->setCurrentRow(0);
	ui->lwExtMusic->clear();
	ui->lwExtMusic->addItems(grlCfg.FormatsMusic);
	ui->lwExtMusic->setCurrentRow(0);
	ui->lwExtImage->clear();
	ui->lwExtImage->addItems(grlCfg.FormatsImage);
	ui->lwExtImage->setCurrentRow(0);

// Idioma ----------------------------
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, ":/idiomas/");
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, grlDir.Idiomas, false);

	int idx = ui->cbxIdioma->findData( grlCfg.IdiomaSelect +".qm", Qt::UserRole, Qt::MatchEndsWith);
	ui->cbxIdioma->setCurrentIndex( idx );
	emit on_cbxIdioma_activated( idx );
}

void frmOpciones::guardarConfig()
{
	if( ui->btnDbxCancel->isEnabled() )
		emit on_btnDbxCancel_clicked();
// SqlDatabase
	grlCfg.db_type     = ui->cbxMotorDataBase->itemData( ui->cbxMotorDataBase->currentIndex() ).toString();
	grlCfg.db_server   = ui->txt_dbserver->text();
	grlCfg.db_host     = ui->txtDirBD->text();
	grlCfg.db_name     = ui->txt_dbname->text();
	grlCfg.db_username = ui->txt_dbusername->text();
	grlCfg.db_password = ui->txt_dbpassword->text();
	grlCfg.db_port     = ui->txt_dbport->text();
// OpcGeneral
	grlCfg.Primeravez      = ui->chkShowNext->isChecked();
	grlCfg.DirDOSBox       = ui->txtDbxPath->text();
	grlCfg.DirScummVM      = ui->txtSvmPath->text();
	grlCfg.DirBaseGames    = ui->txtDirBaseGames->text();
	grlCfg.DOSBoxDisp      = ui->chkDOSBoxDisp->isChecked();
	grlCfg.ScummVMDisp     = ui->chkScummVMDisp->isChecked();
	grlCfg.VDMSoundDisp    = ui->chkVDMSoundDisp->isChecked();
	grlCfg.OpenPdfExternal = ui->chkOpenPdfExternal->isChecked();
	grlCfg.url_xmldb       = ui->cbxScriptURL->itemData( ui->cbxScriptURL->currentIndex() ).toString();
	grlCfg.FormatoFecha    = ui->txtFormatoFecha->text().isEmpty() ? "dd/MM/yyyy HH:mm:ss" : ui->txtFormatoFecha->text();
	grlCfg.StylePalette    = ui->chkStylePalette->isChecked();

	if( IdiomaSelect != grlCfg.IdiomaSelect )
		grlCfg.isChangedIdioma = true;

	if( ui->chkToolbarBigIcon->isChecked() != grlCfg.ToolbarBigIcon )
		grlCfg.isChangedCategoria = true;

	grlCfg.ToolbarBigIcon      = ui->chkToolbarBigIcon->isChecked();
	grlCfg.IconoFav            = ui->cbxIconFav->itemData(ui->cbxIconFav->currentIndex()).toString();
	grlCfg.PicFlowType         = ui->cbxPicFlowType->itemData(ui->cbxPicFlowType->currentIndex()).toString();
	grlCfg.PicFlowShowNumPos   = ui->chkPicFlowShowNumPos->isChecked();
	grlCfg.PicFlowShowTriangle = ui->chkPicFlowShowTriangle->isChecked();
	grlCfg.PicFlowMinHeight    = ui->spinBox_MinHeightPicFlow->value();
	grlCfg.Skip_PicFlow        = ui->spinBox_SkipPicFlow->value();
	grlCfg.VersionDBx          = ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString();
	grlCfg.DOSBoxSVN           = ui->chkDbxSVN->isChecked();
	grlCfg.LastTabOptSelect    = ui->lwOpciones->currentRow();
// OpcFuente
	grlCfg.font_usar   = ui->chkUsarTipoFuente->isChecked();
	grlCfg.font_family = ui->cbxFuenteTipo->currentText();
	grlCfg.font_size   = fGrl->IntToStr( ui->sbxFontSize->value() );
// HttpProxy
	grlCfg.ProxyEnable   = ui->chkEnableProxy->isChecked();
	grlCfg.ProxyType     = ui->cbxTypeProxy->itemData(ui->cbxTypeProxy->currentIndex()).toInt();
	grlCfg.ProxyHost     = ui->txt_px_host->text();
	grlCfg.ProxyPort     = ui->txt_px_puerto->text();
	grlCfg.ProxyLogin    = ui->chkLogin->isChecked();
	grlCfg.ProxyUserName = ui->txt_px_username->text();
	grlCfg.ProxyPassword = ui->txt_px_password->text();
// OpcMultimedia
	grlCfg.FormatsVideo = fGrl->lwToStrList( ui->lwExtVideo );
	grlCfg.FormatsMusic = fGrl->lwToStrList( ui->lwExtMusic );
	grlCfg.FormatsImage = fGrl->lwToStrList( ui->lwExtImage );
// Updates
	grlCfg.chkVersion = ui->chkComprobarVersionGrl->isChecked();

	fGrl->guardarGRLConfig(grlDir.Home +"GR-lida.conf", grlCfg);
}

void frmOpciones::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/barra_herramintas.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );

	ui->lwOpciones->item(0)->setIcon( QIcon(fGrl->Theme() +"img24/opciones.png") );
	ui->lwOpciones->item(1)->setIcon( QIcon(fGrl->Theme() +"img24/dosbox.png") );
	ui->lwOpciones->item(2)->setIcon( QIcon(fGrl->Theme() +"img24/categorias.png") );
	ui->lwOpciones->item(3)->setIcon( QIcon(fGrl->Theme() +"img24/menu_nav.png") );
	ui->lwOpciones->item(4)->setIcon( QIcon(fGrl->Theme() +"img24/datos_1.png") );
	ui->lwOpciones->item(5)->setIcon( QIcon(fGrl->Theme() +"img24/basedatos.png") );
	ui->lwOpciones->item(6)->setIcon( QIcon(fGrl->Theme() +"img24/style.png") );
	ui->lwOpciones->item(7)->setIcon( QIcon(fGrl->Theme() +"img24/html.png") );
	ui->lwOpciones->item(8)->setIcon( QIcon(fGrl->Theme() +"img24/ejecutar_app_setup.png") );

	ui->btnSowEmus->setIcon( QIcon(fGrl->Theme() +"img24/dosbox.png") );
	ui->btnDirBaseGames->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnDirBaseGames_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnOpenUrl->setIcon( QIcon(fGrl->Theme() +"img16/edit_enlace.png") );
	ui->btnInfoFormatoFecha->setIcon( QIcon(fGrl->Theme() +"img16/informacion.png") );

	ui->twEmuladores->setTabIcon(0, QIcon(fGrl->Theme() +"img16/dosbox-scummvm.png") );
	ui->twEmuladores->setTabIcon(1, QIcon(fGrl->Theme() +"img16/cartucho.png") );
	ui->btnSvmPath->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnSvmPath_find->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->btnSvmPath_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbxDefault->setIcon( QIcon(fGrl->Theme() +"img16/go-back.png") );
	ui->btnDbxPath->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbxPath_find->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->btnDbxPath_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->btnDbxNew->setIcon( QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->btnDbxAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnDbxEdit->setIcon( QIcon(fGrl->Theme() +"img16/editar.png") );
	ui->btnDbxUpdate->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->btnDbxDefaultList->setIcon( QIcon(fGrl->Theme() +"img16/go-back.png") );
	ui->btnDbxSubir->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnDbxBajar->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnDbxDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );
	ui->btnDbxCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );

	ui->btnCatAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnCatUpdate->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->btnCatSubir->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnCatBajar->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnCatEditTheme->setIcon( QIcon(fGrl->Theme() +"img16/style.png") );
	ui->btnCatDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );

	ui->btnMnuNavAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnMnuNavUpdate->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->btnMnuNavSubir->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnMnuNavBajar->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnMnuNavDefecto->setIcon( QIcon(fGrl->Theme() +"img16/categorias.png") );
	ui->btnMnuNavDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );

	ui->btnDatPath->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDatPath_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDatAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnDatUpdate->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->btnDatSubir->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnDatBajar->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnDatAddSmile->setIcon( QIcon(fGrl->Theme() +"img16/smile.png") );
	ui->btnDatDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );

	ui->btnDirDB->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDirDB_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->tabApariencia->setTabIcon( 0, QIcon(fGrl->Theme() +"img16/archivo_config.png") );
	ui->tabApariencia->setTabIcon( 1, QIcon(fGrl->Theme() +"img16/style.png") );
	ui->btnEditTheme->setIcon( QIcon(fGrl->Theme() +"img16/style.png") );

	ui->btnExtVideoAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnExtVideoDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );
	ui->btnExtMusicAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnExtMusicDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );
	ui->btnExtImageAdd->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnExtImageDelete->setIcon( QIcon(fGrl->Theme() +"img16/list_remove.png") );

	emit on_chkUsarTipoFuente_clicked( grlCfg.font_usar );
}

void frmOpciones::cargarListaThemes()
{
	QString autor, version;
	QDir dir( grlDir.Themes );
	QStringList list_themes = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);

	ui->twThemes->clear();
	QTreeWidgetItem *item = new QTreeWidgetItem( ui->twThemes );
	item->setText( 0 , "defecto" );
	item->setIcon( 0 , QIcon(fGrl->Theme() +"img16/style.png") );
	item->setText( 1 , "-" );
	item->setText( 2 , "-" );
	item->setTextAlignment(1, Qt::AlignCenter);
	item->setTextAlignment(2, Qt::AlignCenter);
	item->setSelected(true);
	ui->lb_theme_example->setPixmap( QPixmap(fGrl->ThemeApp() +"images/juego_sin_imagen.png") );

	const int list_themesSize = list_themes.size();
	for (int i = 0; i < list_themesSize; ++i)
	{
		if( dir.exists(grlDir.Themes + list_themes[i]) && list_themes[i] != "defecto" )
		{
			QSettings settings(grlDir.Themes + list_themes[i] +"/info.ini", QSettings::IniFormat);
			settings.beginGroup("info");
				autor   = settings.value("autor"  , "-").toString();
				version = settings.value("version", "-").toString();
			settings.endGroup();

			QTreeWidgetItem *item = new QTreeWidgetItem( ui->twThemes );
			item->setText( 0 , list_themes[i] );
			item->setIcon( 0 , QIcon(fGrl->Theme() +"img16/style.png") );
			item->setText( 1 , autor );
			item->setText( 2 , "v"+ version );
			item->setTextAlignment(1, Qt::AlignCenter);
			item->setTextAlignment(2, Qt::AlignCenter);

			if( list_themes[i] == grlCfg.NameDirTheme )
			{
				ui->twThemes->clearSelection();
				ui->twThemes->setCurrentItem( item );
				emit on_twThemes_itemClicked(item, 0);
			}
		}
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
	if( currentRow > -1 )
		ui->swConfigOpciones->setCurrentIndex( currentRow );
	else
		ui->swConfigOpciones->setCurrentIndex( 0 );

	grlCfg.LastTabOptSelect = currentRow;
}

// General ------------------------------------------
void frmOpciones::on_cbxIdioma_activated(int index)
{
	if( index > -1 )
	{
		idioma = ui->cbxIdioma->itemData(index).toString();
		translator.load( idioma );

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
	cbxCat_EmuShow->clear();
	cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/tag.png")     , tr("Todos")   , "all"     );
	cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/datos.png")   , tr("Datos")   , "datos"   );
	cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
	cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("ScummVM") , "scummvm" );
	cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/vdmsound.png"), tr("VDMSound"), "vdmsound");

	foreach (const stGrlDatos &dat, emu_list)
		cbxCat_EmuShow->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ dat.icono), dat.titulo, dat.key);

// Crear, editar menú nav ------------
	ui->cbxMnuNav_ColName->clear();
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Genero")          , "genero"          );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Compañia")        , "compania"        );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Desarrollador")   , "desarrollador"   );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Tema")            , "tema"            );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Grupo")           , "grupo"           );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Perspectiva")     , "perspectiva"     );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/idiomas.png") , tr("Idioma")          , "idioma"          );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/idiomas.png") , tr("Idioma voces")    , "idioma_voces"    );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Formato")         , "formato"         );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/fecha.png")   , tr("Año")             , "anno"            );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/cd_iso.png")  , tr("Nº discos")       , "numdisc"         );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/pc.png")      , tr("Sistema")         , "sistemaop"       );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/grafica.png") , tr("Gráficos")        , "graficos"        );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/grafica.png") , tr("Sonido")          , "sonido"          );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/grafica.png") , tr("Jugabilidad")     , "jugabilidad"     );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/original.png"), tr("Original")        , "original"        );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , tr("Estado")          , "estado"          );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/fecha.png")   , tr("Fecha")           , "fecha"           );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/cartucho.png"), tr("Tipo Emu")        , "tipo_emu"        );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/fav_0.png")   , tr("Favorito")        , "favorito"        );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"images/star_on.png"), tr("Calificación")    , "rating"          );
	ui->cbxMnuNav_ColName->addItem(QIcon(fGrl->Theme() +"img16/edad_nd.png") , tr("Edad recomendada"), "edad_recomendada");
	ui->cbxMnuNav_ColName->setCurrentIndex(0);

	ui->cbxMnuNav_Archivo->clear();
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_3.png") , "<"+ tr("Defecto") +">"  , "defecto"             );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Generos")            , "generos.txt"         );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Compañias")          , "companias.txt"       );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Temas")              , "temas.txt"           );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Grupos")             , "grupos.txt"          );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Perspectivas")       , "perspectivas.txt"    );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/idiomas.png") , tr("Idiomas")            , "idiomas.txt"         );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Formatos")           , "formatos.txt"        );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/fecha.png")   , tr("Años")               , "fechas.txt"          );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/cd_iso.png")  , tr("Nº discos")          , "numdisc.txt"         );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/pc.png")      , tr("Sistemas")           , "sistemas.txt"        );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/grafica.png") , tr("Puntuación")         , "puntuacion.txt"      );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"images/star_on.png"), tr("Calificación")       , "rating.txt"          );
	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/edad_nd.png") , tr("Edades recomendadas"), "edad_recomendada.txt");
//	ui->cbxMnuNav_Archivo->addItem(QIcon(fGrl->Theme() +"img16/cartucho.png"), tr("Emuladores")         , "emu_list.txt"        );
	ui->cbxMnuNav_Archivo->setCurrentIndex(0);

	ui->twMnuNav->clear();
	sql->cargarMenuNav(ui->twMnuNav, "", true);

// Editar datos, configuraciones -----
	ui->cbxDat_Archivo->clear();
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Generos")            , "generos.txt"           );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Compañias")          , "companias.txt"         );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Temas")              , "temas.txt"             );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Perspectivas")       , "perspectivas.txt"      );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/idiomas.png") , tr("Modificar archivo") +" - "+ tr("Idiomas")            , "idiomas.txt"           );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Formatos")           , "formatos.txt"          );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/datos_1.png") , tr("Modificar archivo") +" - "+ tr("Grupos")             , "grupos.txt"            );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/fecha.png"  ) , tr("Modificar archivo") +" - "+ tr("Años")               , "fechas.txt"            );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/cd_iso.png" ) , tr("Modificar archivo") +" - "+ tr("Nº discos")          , "numdisc.txt"           );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/pc.png"     ) , tr("Modificar archivo") +" - "+ tr("Sistemas")           , "sistemas.txt"          );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/edad_nd.png") , tr("Modificar archivo") +" - "+ tr("Edades recomendadas"), "edad_recomendada.txt"  );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/smile.png"  ) , tr("Modificar archivo") +" - "+ tr("Smiles")             , "smiles.txt"            );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_cpu_core"        , "dbx_cpu_core.txt"      );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_cpu_type"        , "dbx_cpu_type.txt"      );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox SVN - dbx_cpu_type_svn", "dbx_cpu_type_svn.txt"  );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_cycles"          , "dbx_cycles.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_hwscale"         , "dbx_hwscale.txt"       );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_joystick"        , "dbx_joystick.txt"      );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_keyboardlayout"  , "dbx_keyboardlayout.txt");
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_machine"         , "dbx_machine.txt"       );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox SVN - dbx_machine_svn" , "dbx_machine_svn.txt"   );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_memsize"         , "dbx_memsize.txt"       );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_midi_device"     , "dbx_midi_device.txt"   );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_mpu401"          , "dbx_mpu401.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_output"          , "dbx_output.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox SVN - dbx_output_svn"  , "dbx_output_svn.txt"    );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_resolution"      , "dbx_resolution.txt"    );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox SVN - dbx_resolution_svn", "dbx_resolution_svn.txt");
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_sb_oplemu"       , "dbx_sb_oplemu.txt"     );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_sb_oplmode"      , "dbx_sb_oplmode.txt"    );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_sbtype"          , "dbx_sbtype.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_scaler"          , "dbx_scaler.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox SVN - dbx_scaler_svn"  , "dbx_scaler_svn.txt"    );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png" ) , tr("Modificar archivo") +" DOSBox - dbx_sensitivity"     , "dbx_sensitivity.txt"   );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_gfxmode"        , "svm_gfxmode.txt"       );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_idioma"         , "svm_idioma.txt"        );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_lista"          , "svm_lista.txt"         );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_music_driver"   , "svm_music_driver.txt"  );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_platform"       , "svm_platform.txt"      );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("Modificar archivo") +" ScummVM - svm_render_mode"    , "svm_render_mode.txt"   );
	ui->cbxDat_Archivo->addItem(QIcon(fGrl->Theme() +"img16/cartucho.png"), tr("Otros emuladores") +" - emu_list"                    , "emu_list.txt"          );
	ui->cbxDat_Archivo->setCurrentIndex(0);
	emit on_cbxDat_Archivo_activated( ui->cbxDat_Archivo->currentIndex() );

// Opciones de base de datos ---------
	ui->cbxMotorDataBase->clear();
	ui->cbxMotorDataBase->addItem( QIcon(fGrl->Theme() +"img16/basedatos.png"), tr("Base de datos") +" SQLite"    , "QSQLITE");
	ui->cbxMotorDataBase->addItem( QIcon(fGrl->Theme() +"img16/basedatos.png"), tr("Base de datos") +" MySQL"     , "QMYSQL" );
	ui->cbxMotorDataBase->addItem( QIcon(fGrl->Theme() +"img16/basedatos.png"), tr("Base de datos") +" PostgreSQL", "QPSQL"  );
	ui->cbxMotorDataBase->setCurrentIndex( ui->cbxMotorDataBase->findData( grlCfg.db_type ) );

// Icono favorito --------------------
	ui->cbxIconFav->clear();
	ui->cbxIconFav->addItem( QIcon(fGrl->Theme() +"img16/fav_0.png"), tr("Favorito") +": 0", "fav_0.png");
	ui->cbxIconFav->addItem( QIcon(fGrl->Theme() +"img16/fav_1.png"), tr("Favorito") +": 1", "fav_1.png");
	ui->cbxIconFav->addItem( QIcon(fGrl->Theme() +"img16/fav_2.png"), tr("Favorito") +": 2", "fav_2.png");
	ui->cbxIconFav->addItem( QIcon(fGrl->Theme() +"img16/fav_3.png"), tr("Favorito") +": 3", "fav_3.png");
	ui->cbxIconFav->setCurrentIndex( ui->cbxIconFav->findData(grlCfg.IconoFav) );

// PictureFlow -----------------------
	ui->cbxPicFlowType->clear();
	ui->cbxPicFlowType->addItem( QIcon(fGrl->Theme() +"img16/capturas.png"), tr("Tipo CoverFlow")    , "CoverFlowLike"  );
	ui->cbxPicFlowType->addItem( QIcon(fGrl->Theme() +"img16/capturas.png"), tr("Tipo tira")         , "Strip"          );
	ui->cbxPicFlowType->addItem( QIcon(fGrl->Theme() +"img16/capturas.png"), tr("Tipo tira solapada"), "StripOverlapped");

	int index_picflowtype = ui->cbxPicFlowType->findData(grlCfg.PicFlowType);
	if( index_picflowtype < 0 ) index_picflowtype = 0;
	ui->cbxPicFlowType->setCurrentIndex( index_picflowtype );
	emit on_cbxPicFlowType_activated( index_picflowtype );

// Opciones del servidor proxy -------
	ui->cbxTypeProxy->clear();
	ui->cbxTypeProxy->addItem( QIcon(fGrl->Theme() +"img16/html.png"), tr("No usar proxy"), "2");
	ui->cbxTypeProxy->addItem( QIcon(fGrl->Theme() +"img16/html.png"), tr("El proxy se determina sobre la base de la aplicación"), "0");
	ui->cbxTypeProxy->addItem( QIcon(fGrl->Theme() +"img16/html.png"), tr("Socks5 proxy se utilizara")           , "1");
	ui->cbxTypeProxy->addItem( QIcon(fGrl->Theme() +"img16/html.png"), tr("HTTP se utiliza proxy transparente")  , "3");
	ui->cbxTypeProxy->addItem( QIcon(fGrl->Theme() +"img16/html.png"), tr("HTTP proxy sólo para las solicitudes"), "4");
	ui->cbxTypeProxy->setCurrentIndex( ui->cbxTypeProxy->findData(grlCfg.ProxyType) );
}

void frmOpciones::on_btnSowEmus_clicked()
{
	ui->lwOpciones->setCurrentRow(1);
}

void frmOpciones::on_btnDirBaseGames_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.DirBaseGames, "DosGames");

	if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
	{
		ui->txtDirBaseGames->setText( fGrl->setDirRelative(directorio, "DosGames") );
		grlCfg.DirBaseGames = ui->txtDirBaseGames->text();
	}
}

void frmOpciones::on_btnDirBaseGames_clear_clicked()
{
	ui->txtDirBaseGames->setText( QDir::toNativeSeparators("./DosGames/") );
}

void frmOpciones::on_btnOpenUrl_clicked()
{
	QString archivo = ui->cbxScriptURL->itemData( ui->cbxScriptURL->currentIndex() ).toString();
	if( archivo != "_desde_archivo_" )
	{
	// Cargamos el script
		QScriptEngine engine;
		engine.evaluate( fGrl->leerArchivo(grlDir.Scripts + archivo, "UTF-8") );

	// Abre la URL con el navegador por defecto
		if( engine.evaluate("url_site").isValid() )
			QDesktopServices::openUrl( QUrl(engine.evaluate("url_site").toString()) );
	}
}

void frmOpciones::on_btnInfoFormatoFecha_clicked()
{
	fGrl->ventanaInfo( tr("Información sobre el formato de la fecha y hora"),
		fGrl->Theme() +"img16/informacion.png", 500, 400,
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
		"</table>"
	);
}

// Emuladores ---------------------------------------
void frmOpciones::on_chkScummVMDisp_toggled(bool checked)
{
	ui->txtSvmPath->setEnabled( checked );
	ui->btnSvmPath->setEnabled( checked );
	ui->btnSvmPath_clear->setEnabled( checked );
}

void frmOpciones::on_btnSvmPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona el ejecutable del ScummVM"), grlCfg.Svm_path, "", "ScummVM (scummvm.exe scummvm);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtSvmPath->setText( fGrl->setDirRelative(archivo) );
			grlCfg.Svm_path = fGrl->setDirRelative(f_info.Path);

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path", grlCfg.Svm_path);
		}
	}
}

void frmOpciones::on_btnSvmPath_find_clicked()
{
#ifdef Q_OS_WIN32
	QStringList lista_drivers;
	QFileInfoList list_drives = QDir::drives();
	lista_drivers.clear();
	for (int i = 0; i < list_drives.size(); ++i)
		lista_drivers.insert(i, QDir::toNativeSeparators(list_drives.at(i).absoluteFilePath()));

	bool ok = false;
	QString letra_drive = QInputDialog::getItem(this, tr("Buscar") +" ScummVM", tr("Selecciona la letra de la unidad:"), lista_drivers, 0, false, &ok);
	if( ok && !letra_drive.isEmpty() )
		ui->txtSvmPath->setText( fGrl->setDirRelative( fGrl->getFindFile(letra_drive, "scummvm.exe") ) );
#else
	#ifdef Q_OS_MAC
		ui->txtSvmPath->setText( fGrl->getFindFile("/", "scummvm") );
	#else
		ui->txtSvmPath->setText( fGrl->getFindFile("/usr/games/", "scummvm") );
	#endif
#endif
}

void frmOpciones::on_btnSvmPath_clear_clicked()
{
	ui->txtSvmPath->clear();
}

void frmOpciones::on_chkDOSBoxDisp_toggled(bool checked)
{
	ui->txtDbxPath->setEnabled( checked );
	ui->btnDbxPath->setEnabled( checked );
	ui->btnDbxPath_clear->setEnabled( checked );
}

void frmOpciones::textEditedTxtDbxDato()
{
	if( ui->btnDbxAdd->isVisible() || ui->btnDbxUpdate->isVisible() )
	{
		QString text = fGrl->eliminar_caracteres(ui->txtDbxTitulo->text());
		text.append("_"+ fGrl->eliminar_caracteres(ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString()));
		text.append(ui->chkDbxSVN->isChecked()?"_svn":"");
		ui->txtDbxDato->setText( text.toLower() );
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
	if( index > -1 )
		textEditedTxtDbxDato();
}

void frmOpciones::on_btnDbxPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona el ejecutable del DOSBox"), grlCfg.Dbx_path, "", "DOSBox (dosbox.exe dosbox);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtDbxPath->setText( fGrl->setDirRelative(archivo) );
			grlCfg.Dbx_path = fGrl->setDirRelative(f_info.Path);

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path", grlCfg.Dbx_path);
		}
	}
}

void frmOpciones::on_btnDbxPath_find_clicked()
{
#ifdef Q_OS_WIN32
	QStringList lista_drivers;
	QFileInfoList list_drives = QDir::drives();
	lista_drivers.clear();
	for (int i = 0; i < list_drives.size(); ++i)
		lista_drivers.insert(i, QDir::toNativeSeparators(list_drives.at(i).absoluteFilePath()));

	bool ok = false;
	QString letra_drive = QInputDialog::getItem(this, tr("Buscar") +" DOSBox", tr("Selecciona la letra de la unidad:"), lista_drivers, 0, false, &ok);
	if( ok && !letra_drive.isEmpty() )
		ui->txtDbxPath->setText( fGrl->setDirRelative( fGrl->getFindFile(letra_drive, "dosbox.exe") ) );
#else
	#ifdef Q_OS_MAC
		ui->txtDbxPath->setText( fGrl->getFindFile("/", "dosbox") );
	#else
		ui->txtDbxPath->setText( fGrl->getFindFile("/usr/bin/", "dosbox") );
	#endif
#endif
}

void frmOpciones::on_btnDbxPath_clear_clicked()
{
	ui->txtDbxPath->clear();
}

void frmOpciones::on_btnDbxDefault_clicked()
{
	grlCfg.DOSBoxDefault = "dosbox";
}

void frmOpciones::on_btnDbxDefaultList_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if( ui->twDOSBox->topLevelItemCount() > 0 && pos != -1 )
	{
		grlCfg.DOSBoxDefault = ui->twDOSBox->currentItem()->text(1);
	} else
		emit on_btnDbxDefault_clicked();
}

void frmOpciones::on_btnDbxNew_clicked()
{
	ui->txtDbxTitulo->setEnabled(true);
	ui->txtDbxDato->setEnabled(true);
	ui->btnDbxDefault->setEnabled(false);
	ui->btnDbxDefaultList->setEnabled(false);
	ui->btnDbxSubir->setEnabled(false);
	ui->btnDbxBajar->setEnabled(false);
	ui->btnDbxNew->setVisible(false);
	ui->btnDbxAdd->setVisible(true);
	ui->btnDbxEdit->setEnabled(false);
	ui->btnDbxUpdate->setVisible(false);
	ui->btnDbxDelete->setEnabled(false);
	ui->btnDbxCancel->setEnabled(true);

	ui->txtDbxTitulo->setText("");
	ui->txtDbxDato->setText("");
	ui->txtDbxPath->setText("");
	ui->chkDbxSVN->setChecked(false);
	ui->cbxDbxVersion->setCurrentIndex(2);
	ui->txtDbxTitulo->setFocus();
}

void frmOpciones::on_btnDbxAdd_clicked()
{
	bool siguiente = false;
	if( ui->txtDbxDato->text().isEmpty() )
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner una etiqueta."));
	else if( ui->txtDbxPath->text().isEmpty() )
		QMessageBox::information(this, tr("Opciones"), tr("Debes indicar el ejecutable del DOSBox."));
	else {
		int total_dbx_find_cfg = 0;
		if( ui->txtDbxDato->text() == "dosbox" )
			total_dbx_find_cfg = 1;
		else {
			QList<QTreeWidgetItem *> dbx_find_cfg = ui->twDOSBox->findItems(ui->txtDbxDato->text(), Qt::MatchExactly, 1);
			total_dbx_find_cfg = dbx_find_cfg.size();
		}

		if( total_dbx_find_cfg > 0 )
			QMessageBox::information(this, tr("Opciones"), tr("Ya esiste la misma etiqueta para el ejecutable del DOSBox."));
		else {
			siguiente = true;
			QTreeWidgetItem *item = new QTreeWidgetItem( ui->twDOSBox );
			item->setTextAlignment(2, Qt::AlignCenter);
			item->setTextAlignment(3, Qt::AlignCenter);
			item->setIcon( 0, QIcon(fGrl->Theme() +"img16/dosbox.png") );
			item->setText( 0, ui->txtDbxTitulo->text() );
			item->setText( 1, ui->txtDbxDato->text()   );
			item->setText( 2, ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString() );
			item->setText( 3, fGrl->BoolToStr(ui->chkDbxSVN->isChecked(),true)  );
			item->setText( 4, ui->txtDbxPath->text() );

			fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
			grlCfg.isChangedListDOSBox = true;
		}
	}

	if( siguiente )
		emit on_btnDbxCancel_clicked();
}

void frmOpciones::on_btnDbxEdit_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if( ui->twDOSBox->topLevelItemCount() > 0 && pos != -1 )
	{
		QTreeWidgetItem *item = ui->twDOSBox->currentItem();

		ui->txtDbxTitulo->setText( item->text(0) );
		ui->txtDbxDato->setText( item->text(1) );
		ui->txtDbxPath->setText( item->text(4) );
		ui->chkDbxSVN->setChecked( fGrl->StrToBool( item->text(3) ) );
		int row = ui->cbxDbxVersion->findData(item->text(2), Qt::UserRole, Qt::MatchExactly);
		if( row < 0 ) row = 0;
		ui->cbxDbxVersion->setCurrentIndex( row );

		ui->twDOSBox->setEnabled(false);
		ui->txtDbxTitulo->setEnabled(true);
		ui->txtDbxDato->setEnabled(true);
		ui->btnDbxDefault->setEnabled(false);
		ui->btnDbxDefaultList->setEnabled(false);
		ui->btnDbxSubir->setEnabled(false);
		ui->btnDbxBajar->setEnabled(false);
		ui->btnDbxNew->setEnabled(false);
		ui->btnDbxAdd->setVisible(false);
		ui->btnDbxEdit->setVisible(false);
		ui->btnDbxUpdate->setVisible(true);
		ui->btnDbxDelete->setEnabled(false);
		ui->btnDbxCancel->setEnabled(true);
	}
}

void frmOpciones::on_btnDbxUpdate_clicked()
{
	int pos = ui->twDOSBox->indexOfTopLevelItem(ui->twDOSBox->currentItem());
	if( ui->twDOSBox->topLevelItemCount() > 0 && pos != -1 )
	{
		bool siguiente = false;
		if( ui->txtDbxTitulo->text().isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Debes poner un título."));
		else if( ui->txtDbxDato->text().isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Debes poner una etiqueta."));
		else if( ui->txtDbxPath->text().isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Debes indicar el ejecutable del DOSBox."));
		else {
			QTreeWidgetItem *item = ui->twDOSBox->currentItem();
			int total_dbx_find_cfg = 0;
			if( ui->txtDbxDato->text() != item->text(1) )
			{
				if( ui->txtDbxDato->text() == "dosbox" )
					total_dbx_find_cfg = 1;
				else {
					QList<QTreeWidgetItem *> dbx_find_cfg = ui->twDOSBox->findItems(ui->txtDbxDato->text(), Qt::MatchExactly, 1);
					total_dbx_find_cfg = dbx_find_cfg.size();
				}
			}

			if( total_dbx_find_cfg > 0 )
				QMessageBox::information(this, tr("Opciones"), tr("Ya esiste la misma etiqueta para el ejecutable del DOSBox."));
			else {
				siguiente = true;

				item->setIcon( 0, QIcon(fGrl->Theme() +"img16/dosbox.png") );
				item->setText( 0, ui->txtDbxTitulo->text() );
				item->setText( 1, ui->txtDbxDato->text()   );
				item->setText( 2, ui->cbxDbxVersion->itemData(ui->cbxDbxVersion->currentIndex()).toString() );
				item->setText( 3, fGrl->BoolToStr(ui->chkDbxSVN->isChecked(),true)  );
				item->setText( 4, ui->txtDbxPath->text() );

				fGrl->guardarDatosTwLista(ui->twDOSBox, grlDir.Datos +"dbx_list.txt", TwListDbx);
				grlCfg.isChangedListDOSBox = true;
			}
		}

		if( siguiente )
			emit on_btnDbxCancel_clicked();
	}

	emit on_btnDbxCancel_clicked();
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
	int pos = ui->twDOSBox->indexOfTopLevelItem( ui->twDOSBox->currentItem() );
	if( ui->twDOSBox->topLevelItemCount() > 0 && pos != -1 )
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este DOSBox de la lista?") +"\n"+ tr("Se actualizaran los juegos al emulador por defecto."), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
		{
		// Actualizamos el DOSBox que se usaba a uno por defecto.
			QSqlQuery query(sql->getSqlDB());
			QString strSQL = "UPDATE dbgrl_emu_dosbox SET dbx_emu_use = :new_dbx_emu_use WHERE dbx_emu_use = :old_dbx_emu_use;";
			query.prepare( strSQL );
			query.bindValue(":new_dbx_emu_use", grlCfg.DOSBoxDefault );
			query.bindValue(":old_dbx_emu_use", ui->twDOSBox->currentItem()->text(1) );
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
	ui->twDOSBox->setEnabled(true);
	ui->txtDbxTitulo->setEnabled(false);
	ui->txtDbxDato->setEnabled(false);
	ui->btnDbxDefault->setEnabled(true);
	ui->btnDbxDefaultList->setEnabled(true);
	ui->btnDbxSubir->setEnabled(true);
	ui->btnDbxBajar->setEnabled(true);
	ui->btnDbxNew->setEnabled(true);
	ui->btnDbxEdit->setEnabled(true);
	ui->btnDbxDelete->setEnabled(true);
	ui->btnDbxCancel->setEnabled(false);

	ui->btnDbxNew->setVisible(true);
	ui->btnDbxAdd->setVisible(false);
	ui->btnDbxEdit->setVisible(true);
	ui->btnDbxUpdate->setVisible(false);

	ui->txtDbxTitulo->setText("DOSBox");
	ui->txtDbxDato->setText("dosbox");
	ui->txtDbxPath->setText( grlCfg.DirDOSBox );
	ui->cbxDbxVersion->setCurrentIndex( ui->cbxDbxVersion->findData( grlCfg.VersionDBx ) );
	ui->chkDbxSVN->setChecked( grlCfg.DOSBoxSVN );
}

// Categoría ----------------------------------------
void frmOpciones::enabledCatUpdate(QString texto, int col)
{
	int pos = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if( ui->twCategorias->topLevelItemCount() > 0 && pos != -1 )
	{
		if( texto != ui->twCategorias->currentItem()->text(col) )
			ui->btnCatUpdate->setEnabled(true);
		else
			ui->btnCatUpdate->setEnabled(false);
	} else
		ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::on_txtCat_Nombre_editingFinished()
{
	if( ui->txtCat_Tabla->text().isEmpty() )
		ui->txtCat_Tabla->setText( "db_"+ fGrl->eliminar_caracteres(ui->txtCat_Nombre->text()).toLower() );
}

void frmOpciones::on_txtCat_Nombre_textEdited(const QString &arg1)
{
	enabledCatUpdate(arg1, 0);
}

void frmOpciones::on_txtCat_Tabla_textEdited(const QString &arg1)
{
	enabledCatUpdate(arg1, 1);
}

void frmOpciones::on_cbxCat_Img_activated(int index)
{
	if( index > -1 )
		enabledCatUpdate(ui->cbxCat_Img->itemData(index).toString(), 4);
	else
		ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::cbxCat_EmuShow_activated(int index)
{
	if( index > -1 )
		enabledCatUpdate(cbxCat_EmuShow->getCheckedItemsUserData(), 6);
	else
		ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::on_btnCatAdd_clicked()
{
	QString id_cat, titulo, tabla, tabla_tmp, img, emu_show;
	bool siguiente = true;
	int orden = 0;

	titulo   = ui->txtCat_Nombre->text();
	tabla    = ui->txtCat_Tabla->text();
	img      = ui->cbxCat_Img->itemData( ui->cbxCat_Img->currentIndex() ).toString();
	orden    = ui->twCategorias->topLevelItemCount();
	emu_show = cbxCat_EmuShow->getCheckedItemsUserData().isEmpty() ? "all" : cbxCat_EmuShow->getCheckedItemsUserData();

	if( tabla.isEmpty() )
		QMessageBox::information(this, tr("Opciones"), tr("Debes poner el nombre de la tabla para la nueva categoría."));
	else {
		if( titulo.isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Debes poner el título de la nueva categoría."));
		else {
			QStringList lista_tablas = sql->listarTablas();

			if( lista_tablas.contains(tabla) )
			{
				tabla_tmp = tabla +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss");

				int respuesta = QMessageBox::question(this, tr("Ya existe"), tr("¿La tabla ya existe, quieres usar el siguiente nombre para la tabla?") +"\n"+ tabla_tmp, tr("Si"), tr("No"), 0, 1);
				if( respuesta == 0 )
				{
					tabla = tabla_tmp;
					siguiente = true;
				} else
					siguiente = false;
			} else
				siguiente = true;

			if( siguiente )
			{
				id_cat = sql->insertaCategoria(tabla, titulo, img, orden, emu_show);

				if( id_cat.isEmpty() )
					QMessageBox::information(this, tr("Opciones"), tr("No se ha podido añadir la nueva categoría."));
				else {
					QTreeWidgetItem *item = new QTreeWidgetItem( ui->twCategorias );
					item->setTextAlignment(2, Qt::AlignCenter);

					if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ img) )
						item->setIcon( 0, QIcon(fGrl->ThemeApp() +"img32_cat/"+ img) );
					else
						item->setIcon( 0, QIcon(":/img32_cat/"+ img) );

					item->setText( 0, titulo );
					item->setText( 1, tabla  );
					item->setText( 2, "0"    );
					item->setText( 3, id_cat );
					item->setText( 4, img    );
					item->setText( 5, fGrl->IntToStr(orden) );
					item->setText( 6, emu_show );

					grlCfg.isChangedCategoria = true;
				}
			}
		}
	}
}

void frmOpciones::on_btnCatUpdate_clicked()
{
	QString id_cat, titulo, tabla, tabla_old, tabla_tmp, img, emu_show;
	bool siguiente = true;
	int orden = 0;

	int pos = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if( ui->twCategorias->topLevelItemCount() > 0 && pos != -1 )
	{
		titulo    = ui->txtCat_Nombre->text();
		tabla     = ui->txtCat_Tabla->text().toLower();
		tabla_old = ui->twCategorias->currentItem()->text(1);
		img       = ui->cbxCat_Img->itemData( ui->cbxCat_Img->currentIndex() ).toString();
		id_cat    = ui->twCategorias->currentItem()->text(3);
		orden     = fGrl->StrToInt(ui->twCategorias->currentItem()->text(5));
		emu_show  = cbxCat_EmuShow->getCheckedItemsUserData().isEmpty() ? "all" : cbxCat_EmuShow->getCheckedItemsUserData();

		if( tabla_old == "dbgrl" )
		{
			tabla = "dbgrl";
			ui->txtCat_Tabla->setText(tabla);
		}

		if( tabla.isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Debes poner el nombre de la tabla para actualizar la categoría."));
		else {
			if( titulo.isEmpty() )
				QMessageBox::information(this, tr("Opciones"), tr("Debes poner el título para actualizar la categoría."));
			else {
				if( QString::compare(tabla_old, tabla) == 0 )
					siguiente  = true;
				else {
					QStringList lista_tablas = sql->listarTablas();

					if( lista_tablas.contains(tabla) )
					{
						tabla_tmp = tabla +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss");

						int respuesta = QMessageBox::question(this, tr("Ya existe"), tr("¿La tabla ya existe, quieres usar el siguiente nombre para la tabla?") +"\n"+ tabla_tmp, tr("Si"), tr("No"), 0, 1);
						if( respuesta == 0 )
						{
							if( sql->renombrarTabla(tabla_old, tabla_tmp) )
							{
								tabla = tabla_tmp;
								ui->txtCat_Tabla->setText(tabla);

								fGrl->renombrarDirectorio(grlDir.Capturas + tabla_old +"/", grlDir.Capturas + tabla +"/");
								fGrl->renombrarDirectorio(grlDir.Confdbx  + tabla_old +"/", grlDir.Confdbx  + tabla +"/");
								fGrl->renombrarDirectorio(grlDir.Confvdms + tabla_old +"/", grlDir.Confvdms + tabla +"/");
								fGrl->renombrarDirectorio(grlDir.Covers   + tabla_old +"/", grlDir.Covers   + tabla +"/");
								fGrl->renombrarDirectorio(grlDir.Thumbs   + tabla_old +"/", grlDir.Thumbs   + tabla +"/");

								siguiente  = true;
							} else {
								siguiente = false;
								ui->txtCat_Tabla->setText(tabla);
							}
						} else
							siguiente  = false;
					} else {
						if( sql->renombrarTabla(tabla_old, tabla) )
							siguiente  = true;
						else
							siguiente = false;

						fGrl->renombrarDirectorio(grlDir.Capturas + tabla_old +"/", grlDir.Capturas + tabla +"/");
						fGrl->renombrarDirectorio(grlDir.Confdbx  + tabla_old +"/", grlDir.Confdbx  + tabla +"/");
						fGrl->renombrarDirectorio(grlDir.Confvdms + tabla_old +"/", grlDir.Confvdms + tabla +"/");
						fGrl->renombrarDirectorio(grlDir.Covers   + tabla_old +"/", grlDir.Covers   + tabla +"/");
						fGrl->renombrarDirectorio(grlDir.Thumbs   + tabla_old +"/", grlDir.Thumbs   + tabla +"/");
					}
				}

				if( siguiente )
				{
					if( sql->actualizaCategoria(id_cat, tabla, titulo, img, emu_show) )
					{
						QTreeWidgetItem *item = ui->twCategorias->currentItem();
						item->setTextAlignment(2, Qt::AlignCenter);

						if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ img) )
							item->setIcon( 0, QIcon(fGrl->ThemeApp() +"img32_cat/"+ img) );
						else
							item->setIcon( 0, QIcon(":/img32_cat/"+ img) );

						item->setText( 0, titulo );
						item->setText( 1, tabla  );
						item->setText( 4, img    );
						item->setText( 5, fGrl->IntToStr(orden) );
						item->setText( 6, emu_show );

						grlCfg.isChangedCategoria = true;
					} else
						QMessageBox::information(this, tr("Opciones"), tr("No se ha podido actualizar la categoría."));
				}
			}
		}
	} else
		QMessageBox::information(this, tr("Opciones"), tr("No se ha podido actualizar la categoría."));
	ui->btnCatUpdate->setEnabled(false);
}

void frmOpciones::on_btnCatSubir_clicked()
{
	fGrl->moveUpItemTw(ui->twCategorias);
	sql->actualizaOrdenCategoria(ui->twCategorias);
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnCatBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twCategorias);
	sql->actualizaOrdenCategoria(ui->twCategorias);
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnCatEditTheme_clicked()
{
	emit on_btnEditTheme_clicked();
}

void frmOpciones::on_btnCatDelete_clicked()
{
	int pos = ui->twCategorias->indexOfTopLevelItem( ui->twCategorias->currentItem() );
	if( ui->twCategorias->topLevelItemCount() > 0 && pos != -1 )
	{
		if( ui->twCategorias->currentItem()->text(3).isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Por favor selecciona una categoría de la lista para eliminarla"));
		else {
			if( ui->twCategorias->currentItem()->text(1) == "dbgrl" )
				QMessageBox::information(this, tr("Opciones"), tr("No se puede eliminar la tabla de la categoría."));
			else {
				int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar esta categoría de la base de datos?"), tr("Si"), tr("No"), 0, 1);
				if( respuesta == 0 )
				{
					if( sql->eliminarCategoria( ui->twCategorias->currentItem()->text(3) ) )
					{
						fGrl->deleteItemTree( ui->twCategorias->currentItem() );
						sql->actualizaOrdenCategoria( ui->twCategorias );
						grlCfg.isChangedCategoria = true;
					}
				}
			}
		}
	}
}

void frmOpciones::on_twCategorias_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->btnCatUpdate->setEnabled(false);
		ui->txtCat_Nombre->setText( item->text(0) );
		ui->txtCat_Tabla->setText( item->text(1) );
		ui->cbxCat_Img->setCurrentIndex( ui->cbxCat_Img->findData(item->text(4), Qt::UserRole, Qt::MatchExactly) );
		cbxCat_EmuShow->setCheckedItemsUserData( item->text(6) );
	}
}

void frmOpciones::on_twCategorias_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twCategorias_itemClicked(current, 0);
}

// MenuNav ------------------------------------------
void frmOpciones::addEditTwMnuNav(bool isNew)
{
	QString id_mnu_nav, titulo, col_value, col_name, sql_query, archivo, img;
	bool expanded, show;
	int orden = 0;

	titulo    = ui->txtMnuNav_Nombre->text();
	col_value = ui->cbxMnuNav_ColValue->itemText( ui->cbxMnuNav_ColValue->currentIndex() );
	col_name  = ui->cbxMnuNav_ColName->itemData( ui->cbxMnuNav_ColName->currentIndex() ).toString();
	sql_query = ui->cbxMnuNav_SqlQuery->itemText( ui->cbxMnuNav_SqlQuery->currentIndex() );
	archivo   = ui->cbxMnuNav_Archivo->itemData( ui->cbxMnuNav_Archivo->currentIndex() ).toString();
	img       = ui->cbxMnuNav_Img->itemData( ui->cbxMnuNav_Img->currentIndex() ).toString();
	orden     = ui->twMnuNav->topLevelItemCount();
	expanded  = ui->chkMnuNav_Expanded->isChecked();
	show      = ui->chkMnuNav_Show->isChecked();

	QTreeWidgetItem *item;
	if( isNew )
	{
		id_mnu_nav = sql->insertaMenuNav(titulo, col_value, col_name, sql_query, archivo, img, orden, show, expanded);
		item = new QTreeWidgetItem( ui->twMnuNav );
	} else {
		int pos = ui->twMnuNav->indexOfTopLevelItem(ui->twMnuNav->currentItem());
		if( ui->twMnuNav->topLevelItemCount() > 0 && pos != -1 )
		{
			item  = ui->twMnuNav->currentItem();
			orden = fGrl->StrToInt(item->text(8));
			sql->actualizaMenuNav(item->text(9), titulo, col_value, col_name, sql_query, archivo, img, orden, show, expanded);
		} else {
			item = new QTreeWidgetItem( ui->twMnuNav );
			id_mnu_nav = sql->insertaMenuNav(titulo, col_value, col_name, sql_query, archivo, img, orden, show, expanded);
		}
		ui->btnMnuNavUpdate->setEnabled(false);
	}

	if( QFile::exists(fGrl->Theme() +"img16/"+ img) )
		item->setIcon( 0, QIcon(fGrl->Theme() +"img16/"+ img) );
	else
		item->setIcon( 0, QIcon(fGrl->Theme() +"img16/sinimg.png") );

	item->setText( 0, titulo    );
	item->setText( 1, col_value );
	item->setText( 2, col_name  );
	item->setText( 3, sql_query );
	item->setText( 4, archivo   );
	item->setText( 5, fGrl->BoolToStr(show)     );
	item->setText( 6, fGrl->BoolToStr(expanded) );
	item->setText( 7, img       );
	if( isNew )
	{
		item->setText( 8, fGrl->IntToStr(orden) );
		item->setText( 9, id_mnu_nav           );
	}
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::enabledMnuNavUpdate(QString texto, int col)
{
	int pos = ui->twMnuNav->indexOfTopLevelItem(ui->twMnuNav->currentItem());
	if( ui->twMnuNav->topLevelItemCount() > 0 && pos != -1 )
	{
		if( texto != ui->twMnuNav->currentItem()->text(col) )
			ui->btnMnuNavUpdate->setEnabled(true);
		else
			ui->btnMnuNavUpdate->setEnabled(false);
	} else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_txtMnuNav_Nombre_textEdited(const QString &arg1)
{
	enabledMnuNavUpdate(arg1, 0);
}

void frmOpciones::on_chkMnuNav_Expanded_clicked(bool checked)
{
	enabledMnuNavUpdate(fGrl->BoolToStr(checked), 6);
}

void frmOpciones::on_chkMnuNav_Show_clicked(bool checked)
{
	enabledMnuNavUpdate(fGrl->BoolToStr(checked), 5);
}

void frmOpciones::on_cbxMnuNav_ColName_activated(int index)
{
	if( index > -1 )
		enabledMnuNavUpdate(ui->cbxMnuNav_ColName->itemData(index).toString(), 2);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNav_Img_activated(int index)
{
	if( index > -1 )
		enabledMnuNavUpdate(ui->cbxMnuNav_Img->itemData(index).toString(), 7);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNav_Archivo_activated(int index)
{
	if( index > -1 )
		enabledMnuNavUpdate(ui->cbxMnuNav_Archivo->itemData(index).toString(), 4);
	else
		ui->btnMnuNavUpdate->setEnabled(false);
}

void frmOpciones::on_cbxMnuNav_ColValue_editTextChanged(const QString &arg1)
{
	enabledMnuNavUpdate(arg1, 1);
}

void frmOpciones::on_cbxMnuNav_SqlQuery_editTextChanged(const QString &arg1)
{
	enabledMnuNavUpdate(arg1, 3);
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
	sql->actualizaOrdenMenuNav(ui->twMnuNav);
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnMnuNavBajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMnuNav);
	sql->actualizaOrdenMenuNav(ui->twMnuNav);
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::on_btnMnuNavDefecto_clicked()
{
	int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Deseas restaurar el menú de navegación por defecto?"), tr("Si"), tr("No"), 0, 1);
	if( respuesta == 0 )
	{
		const int listSize = ui->twMnuNav->topLevelItemCount();
		if( listSize > 0 )
		{
			for(int i = 0; i < listSize; ++i)
				sql->eliminarMenuNav( ui->twMnuNav->topLevelItem(i)->text(9) );
		}

		sql->insertaMenuNav( tr("Generos")        , "{value}", "genero"       , "WHERE {col_name} LIKE ('%{col_value}%')", "generos.txt"  , "datos_3.png", 0, true, true);
		sql->insertaMenuNav( tr("Temas")          , "{value}", "tema"         , "WHERE {col_name} LIKE ('%{col_value}%')", "temas.txt"    , "datos_3.png", 1, true, true);
		sql->insertaMenuNav( tr("Desarrolladores"), "{value}", "desarrollador", "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 2, true, true);
		sql->insertaMenuNav( tr("Compañias")      , "{value}", "compania"     , "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 3, true, true);
		sql->insertaMenuNav( tr("Años")           , "{value}", "anno"         , "WHERE {col_name} LIKE ('%{col_value}%')", "fechas.txt"   , "fecha.png"  , 4, true, true);
		sql->insertaMenuNav( tr("Idiomas")        , "{value}", "idioma"       , "WHERE {col_name} LIKE ('%{col_value}%')", "idiomas.txt"  , "idiomas.png", 5, true, true);

		ui->twMnuNav->clear();
		sql->cargarMenuNav(ui->twMnuNav, "", true);
		grlCfg.isChangedCategoria = true;
	}
}

void frmOpciones::on_btnMnuNavDelete_clicked()
{
	int pos = ui->twMnuNav->indexOfTopLevelItem( ui->twMnuNav->currentItem() );
	if( ui->twMnuNav->topLevelItemCount() > 0 && pos != -1 )
	{
		if( ui->twMnuNav->currentItem()->text(9).isEmpty() )
			QMessageBox::information(this, tr("Opciones"), tr("Por favor selecciona un menú nav de la lista para eliminarlo"));
		else {
			int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este menú nav de la base de datos?"), tr("Si"), tr("No"), 0, 1);
			if( respuesta == 0 )
			{
				if( sql->eliminarMenuNav( ui->twMnuNav->currentItem()->text(9) ) )
				{
					fGrl->deleteItemTree( ui->twMnuNav->currentItem() );
					sql->actualizaOrdenMenuNav( ui->twMnuNav );
					grlCfg.isChangedCategoria = true;
				}
			}
		}
	}
}

void frmOpciones::on_twMnuNav_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->btnMnuNavUpdate->setEnabled(false);
		ui->txtMnuNav_Nombre->setText( item->text(0) );
		ui->cbxMnuNav_ColValue->setCurrentIndex( ui->cbxMnuNav_ColValue->findText( item->text(1) ) );
		ui->cbxMnuNav_ColName->setCurrentIndex( ui->cbxMnuNav_ColName->findData( item->text(2) ) );
		ui->cbxMnuNav_SqlQuery->setCurrentIndex( ui->cbxMnuNav_SqlQuery->findText( item->text(3) ) );
		ui->cbxMnuNav_Archivo->setCurrentIndex( ui->cbxMnuNav_Archivo->findData( item->text(4) ) );
		ui->chkMnuNav_Show->setChecked( fGrl->StrToBool( item->text(5) ) );
		ui->chkMnuNav_Expanded->setChecked( fGrl->StrToBool( item->text(6) ) );
		ui->cbxMnuNav_Img->setCurrentIndex( ui->cbxMnuNav_Img->findData( item->text(7) ) );
	}
}

void frmOpciones::on_twMnuNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current != previous)
		emit on_twMnuNav_itemClicked(current, 0);
}

// Datos --------------------------------------------
void frmOpciones::addEditDatosTwLista(bool isNew)
{
	QString archivo, titulo, dato, extra, img, dir_img;
	int col_titulo, col_dato, col_nada;
	QFont m_font;
	m_font.setBold(true);

	archivo = ui->cbxDat_Archivo->itemData( ui->cbxDat_Archivo->currentIndex() ).toString();
	titulo  = ui->txtDat_Nombre->text();
	dato    = ui->txtDat_Dato->text();
	extra   = ui->txtDat_Extra->text();
	img     = ui->cbxDat_Img->itemData( ui->cbxDat_Img->currentIndex() ).toString();

	col_titulo = 0;
	col_dato   = 1;
	col_nada   = 2;

	QTreeWidgetItem *item;
	if( isNew )
		item = new QTreeWidgetItem( ui->twDatos );
	else {
		int pos = ui->twDatos->indexOfTopLevelItem(ui->twDatos->currentItem());
		if( ui->twDatos->topLevelItemCount() > 0 && pos != -1 )
			item = ui->twDatos->currentItem();
		else
			item = new QTreeWidgetItem( ui->twDatos );
		ui->btnDatUpdate->setEnabled(false);
	}

	if( archivo == "smiles.txt" )
		dir_img = grlDir.Smiles;
	else if( archivo == "emu_list.txt" )
		dir_img = fGrl->ThemeApp() +"img16_cat/";
	else if( archivo == "svm_lista.txt" )
	{
		dir_img    = fGrl->Theme() +"icon_svm/";
		col_nada   = 0;
		col_titulo = 1;
		col_dato   = 2;

		if( titulo.isEmpty() && dato.isEmpty() )
		{
			titulo = "";
			dato   = "";
			img    = "";
			item->setIcon( 0, QIcon( fGrl->Theme() +"img_svm/sinimg.png" ) );
		} else {
			if( dato.isEmpty() )
			{
				item->setTextColor( 1, QColor(0,0,0) );
				item->setFont( 1, m_font);
			} else
				extra = ui->cbxDat_ImgCmpt->itemData( ui->cbxDat_ImgCmpt->currentIndex() ).toString().remove(".png");
		}

		if( extra.isEmpty() )
			item->setIcon( 0, QIcon(fGrl->Theme() +"img_svm/sinimg.png") );
		else
			item->setIcon( 0, QIcon(fGrl->Theme() +"img_svm/"+ extra +".png") );
	}
	else if( archivo == "idiomas.txt" || archivo == "svm_idioma.txt" || archivo == "dbx_keyboardlayout.txt" )
		dir_img = fGrl->Theme() +"img_lng/";
	else
		dir_img = fGrl->Theme() +"img16/";

	if( dato.isEmpty() )
		extra  = "";

	if( QFile::exists(dir_img + img) )
		item->setIcon( col_titulo, QIcon(dir_img + img) );
	else
		item->setIcon( col_titulo, QIcon(dir_img +"sinimg.png") );

	item->setText( col_nada  , ""     );
	item->setText( col_titulo, titulo );
	item->setText( col_dato  , dato   );
	item->setText( 3, extra  );
	item->setText( 4, img    );

	guardarDatosTwLista();
}

void frmOpciones::guardarDatosTwLista()
{
	QString archivo, sLng;
	archivo = ui->cbxDat_Archivo->itemData( ui->cbxDat_Archivo->currentIndex() ).toString();
	sLng    = fGrl->getArchivoIsLng(archivo);

// Tipo de columnas, ScummVM, Smile, 2col, 3col...
	if( fGrl->getArchivoIs3Col(archivo) )
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList3col);
	else if( archivo == "smiles.txt" )
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSmile, "\",\"");
	else if( archivo == "emu_list.txt" || archivo == "dbx_keyboardlayout.txt" )
	{
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListEmu);
		if( archivo == "emu_list.txt" )
		{
			grlCfg.isChangedEmuList = true;

			cbxCat_EmuShow->clear();
			cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/tag.png")     , tr("Todos")   , "all"     );
			cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/datos.png")   , tr("Datos")   , "datos"   );
			cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
			cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("ScummVM") , "scummvm" );
			cbxCat_EmuShow->addItem(QIcon(fGrl->Theme() +"img16/vdmsound.png"), tr("VDMSound"), "vdmsound");

			emu_list.clear();
			emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");
			foreach (const stGrlDatos &dat, emu_list)
				cbxCat_EmuShow->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ dat.icono), dat.titulo, dat.key);
		}
	} else if( archivo == "svm_lista.txt" )
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSvm);
	else
		fGrl->guardarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList2col);
	grlCfg.isChangedCategoria = true;
}

void frmOpciones::enabledDatosUpdate(QString texto, int col)
{
	int pos = ui->twDatos->indexOfTopLevelItem(ui->twDatos->currentItem());
	if( ui->twDatos->topLevelItemCount() > 0 && pos != -1 )
	{
		if( texto != ui->twDatos->currentItem()->text(col) )
			ui->btnDatUpdate->setEnabled(true);
		else
			ui->btnDatUpdate->setEnabled(false);
	} else
		ui->btnDatUpdate->setEnabled(false);
}

void frmOpciones::on_cbxDat_Archivo_activated(int index)
{
	QString archivo, sLng, dir_img;
	bool isLng = false;

	archivo = ui->cbxDat_Archivo->itemData(index).toString();
	sLng    = fGrl->getArchivoIsLng(archivo);

	ui->lb_Opt_14->setText(tr("Dato") +":");
	ui->lb_Opt_datpath->setVisible(false);
	ui->cbxDat_ImgCmpt->setVisible(false);
	ui->txtDat_Extra->setVisible(false);
	ui->btnDatPath->setVisible(false);
	ui->btnDatPath_clear->setVisible(false);
	ui->txtDat_Dato->setEnabled(false);
	ui->btnDatAddSmile->setEnabled(false);

	ui->txtDat_Nombre->setText("");
	ui->txtDat_Dato->setText("");
	ui->txtDat_Extra->setText("");
	ui->cbxDat_Img->setIconSize(QSize(16,16));
	ui->cbxDat_Img->setMinimumHeight(24);

	if( archivo == "smiles.txt" )
		dir_img = grlDir.Smiles;
	else if( archivo == "emu_list.txt" )
		dir_img = fGrl->ThemeApp() +"img16_cat/";
	else if( archivo == "svm_lista.txt" )
		dir_img = fGrl->Theme() +"icon_svm/";
	else if( archivo == "idiomas.txt" || archivo == "svm_idioma.txt" || archivo == "dbx_keyboardlayout.txt" )
	{
		isLng = true;
		dir_img = fGrl->Theme() +"img_lng/";
	} else
		dir_img = fGrl->Theme() +"img16/";

	fGrl->cargarIconosComboBox(ui->cbxDat_Img, dir_img, "sinimg.png", grlCfg.FormatsImage.join(";"));

	ui->twDatos->headerItem()->setIcon( 0, QIcon(fGrl->Theme() +"img16/tag.png") );
	ui->twDatos->headerItem()->setIcon( 1, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
	ui->twDatos->headerItem()->setIcon( 2, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
	ui->twDatos->headerItem()->setIcon( 3, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
	ui->twDatos->headerItem()->setText( 0, tr("Título") );
	ui->twDatos->headerItem()->setText( 1, "Config"     );
	ui->twDatos->headerItem()->setText( 2, ""           );
	ui->twDatos->headerItem()->setText( 3, ""           );
	ui->twDatos->headerItem()->setTextAlignment(1, Qt::AlignCenter);
	ui->twDatos->header()->setMovable(false);
	ui->twDatos->header()->setStretchLastSection(false);
	ui->twDatos->header()->setResizeMode( 0, QHeaderView::Stretch     );
	ui->twDatos->header()->setResizeMode( 1, QHeaderView::Interactive );
	ui->twDatos->header()->setResizeMode( 2, QHeaderView::Fixed       );
	ui->twDatos->header()->setResizeMode( 3, QHeaderView::Interactive );
	ui->twDatos->setColumnWidth( 0, 150 );
	ui->twDatos->setColumnWidth( 1,  90 );
	ui->twDatos->setColumnWidth( 2,  80 );
	ui->twDatos->setColumnWidth( 3,  80 );
	ui->twDatos->setColumnHidden( 1, true );
	ui->twDatos->setColumnHidden( 2, true );
	ui->twDatos->setColumnHidden( 3, true );

	ui->twDatos->clear();
// Tipo de columnas, ScummVM, Smile, 2col, 3col...
	if( fGrl->getArchivoIs3Col(archivo) )
	{
		ui->txtDat_Dato->setEnabled(true);
		ui->twDatos->setColumnHidden( 1, false );

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList3col, "|", isLng);
	}
	else if( archivo == "smiles.txt" )
	{
		ui->btnDatAddSmile->setEnabled(true);
		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSmile, "\",\"", isLng);
	}
	else if( archivo == "emu_list.txt" || archivo == "dbx_keyboardlayout.txt" )
	{
		ui->txtDat_Dato->setEnabled(true);
		ui->lb_Opt_datpath->setVisible(true);
		ui->txtDat_Extra->setVisible(true);
		ui->btnDatPath->setVisible(true);
		ui->btnDatPath_clear->setVisible(true);
		ui->twDatos->headerItem()->setTextAlignment(1, Qt::AlignLeft);
		ui->twDatos->setColumnHidden( 1, false );
		ui->twDatos->setColumnHidden( 3, false );

		if( archivo == "dbx_keyboardlayout.txt" )
		{
			ui->twDatos->headerItem()->setText( 1, tr("Etiqueta") );
			ui->twDatos->headerItem()->setText( 3, tr("Config") );

			ui->lb_Opt_14->setText(tr("Etiqueta") +":");
			ui->lb_Opt_datpath->setText(tr("Dato") +":");
			ui->btnDatPath->setVisible(false);
			ui->btnDatPath_clear->setVisible(false);
		} else {
			ui->twDatos->headerItem()->setText( 1, tr("Etiqueta") );
			ui->twDatos->headerItem()->setText( 3, "Dir" );

			ui->lb_Opt_14->setText(tr("Etiqueta") +":");
			ui->lb_Opt_datpath->setText(tr("Dirección") +":");
			ui->btnDatPath->setVisible(true);
			ui->btnDatPath_clear->setVisible(true);
		}

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListEmu, "|", isLng);
	}
	else if( archivo == "svm_lista.txt" )
	{
		ui->txtDat_Dato->setEnabled(true);
		ui->cbxDat_Img->setIconSize(QSize(24,24));
		ui->cbxDat_Img->setMinimumHeight(31);
		ui->cbxDat_ImgCmpt->setVisible(true);

		ui->twDatos->headerItem()->setIcon( 0, QIcon(fGrl->Theme() +"img16/bullet_black.png") );
		ui->twDatos->headerItem()->setIcon( 1, QIcon(fGrl->Theme() +"img16/tag.png") );
		ui->twDatos->headerItem()->setText( 0, ""           );
		ui->twDatos->headerItem()->setText( 1, tr("Título") );
		ui->twDatos->headerItem()->setText( 2, tr("Config") );

		ui->twDatos->header()->setResizeMode( 0, QHeaderView::Fixed   );
		ui->twDatos->header()->setResizeMode( 1, QHeaderView::Stretch );
		ui->twDatos->setColumnWidth( 0, 30 );
		ui->twDatos->setColumnHidden( 1, false );
		ui->twDatos->setColumnHidden( 2, false );

		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwListSvm, "|", isLng);
	} else
		fGrl->cargarDatosTwLista(ui->twDatos, grlDir.Datos + sLng + archivo, TwList2col, "|", isLng);
}

void frmOpciones::on_txtDat_Nombre_textEdited(const QString &arg1)
{
	QString archivo = ui->cbxDat_Archivo->itemData( ui->cbxDat_Archivo->currentIndex() ).toString();
	int col_titulo = 0;

	if( archivo == "svm_lista.txt" )
		col_titulo = 1;

	enabledDatosUpdate(arg1, col_titulo);
}

void frmOpciones::on_cbxDat_ImgCmpt_activated(int index)
{
	if( index > -1 )
		enabledDatosUpdate(ui->cbxDat_ImgCmpt->itemData(index).toString().remove(".png"), 3);
	else
		ui->btnDatUpdate->setEnabled(false);
}

void frmOpciones::on_txtDat_Dato_textEdited(const QString &arg1)
{
	QString archivo = ui->cbxDat_Archivo->itemData( ui->cbxDat_Archivo->currentIndex() ).toString();
	int col_dato = 1;

	if( archivo == "svm_lista.txt" )
		col_dato = 2;

	enabledDatosUpdate(arg1, col_dato);
}

void frmOpciones::on_cbxDat_Img_activated(int index)
{
	if( index > -1 )
		enabledDatosUpdate(ui->cbxDat_Img->itemData(index).toString(), 4);
	else
		ui->btnDatUpdate->setEnabled(false);
}

void frmOpciones::on_txtDat_Extra_textEdited(const QString &arg1)
{
	enabledDatosUpdate(arg1, 3);
}

void frmOpciones::on_btnDatPath_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona el ejecutable"), grlCfg.DirOtherEmus, "", tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtDat_Extra->setText( fGrl->setDirRelative(archivo) );
			grlCfg.DirOtherEmus = fGrl->setDirRelative(f_info.Path);

			enabledDatosUpdate(ui->txtDat_Extra->text(), 3);

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DirOtherEmus", grlCfg.DirOtherEmus);
		}
	}
}

void frmOpciones::on_btnDatPath_clear_clicked()
{
	ui->txtDat_Extra->clear();
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
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlDir.Smiles, "", tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if( f_info.Exists )
		{
			if( QFile::exists(grlDir.Smiles + f_info.NameExt) )
			{
				QString hora = "_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss");

				if( QFile::copy(archivo, grlDir.Smiles + f_info.Name + hora + f_info.Ext) )
					ui->cbxDat_Img->addItem(QIcon(grlDir.Smiles + f_info.Name + hora + f_info.Ext), f_info.Name + hora, f_info.Name + hora + f_info.Ext);
			} else {
				if( QFile::copy(archivo, grlDir.Smiles + f_info.NameExt) )
					ui->cbxDat_Img->addItem(QIcon(grlDir.Smiles + f_info.NameExt), f_info.Name, f_info.NameExt);
			}
		}
	}
}

void frmOpciones::on_btnDatDelete_clicked()
{
	int pos = ui->twDatos->indexOfTopLevelItem( ui->twDatos->currentItem() );
	if( ui->twDatos->topLevelItemCount() > 0 && pos != -1 )
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Deseas realmente eliminar este dato de la lista?"), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
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

	if( item && column > -1 )
	{
		QString archivo = ui->cbxDat_Archivo->itemData( ui->cbxDat_Archivo->currentIndex() ).toString();
		int col_titulo = 0;
		int col_dato   = 1;

		if( fGrl->getArchivoIs3Col(archivo) || archivo == "emu_list.txt" || archivo == "svm_lista.txt" || archivo == "dbx_keyboardlayout.txt" )
		{
			ui->txtDat_Dato->setEnabled(true);
			if( archivo == "emu_list.txt" || archivo == "dbx_keyboardlayout.txt" )
				ui->txtDat_Extra->setText( item->text(3) );

			if( archivo == "svm_lista.txt" )
			{
				col_titulo = 1;
				col_dato   = 2;
				ui->cbxDat_ImgCmpt->setCurrentIndex(ui->cbxDat_ImgCmpt->findData(item->text(3) +".png", Qt::UserRole, Qt::MatchExactly));
			}
		} else
			ui->txtDat_Dato->setEnabled(false);

		ui->txtDat_Nombre->setText( item->text(col_titulo) );
		ui->txtDat_Dato->setText( item->text(col_dato) );

		int row = ui->cbxDat_Img->findData(item->text(4), Qt::UserRole, Qt::MatchExactly);
		if( row < 0 ) row = 0;
		ui->cbxDat_Img->setCurrentIndex( row );
	}
}

void frmOpciones::on_twDatos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current != previous)
		emit on_twDatos_itemClicked(current, 0);
}

// Base de Datos-------------------------------------
void frmOpciones::on_btnDirDB_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.DirBD, "", tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtDirBD->setText( fGrl->setDirRelative(archivo) );
			grlCfg.DirBD = fGrl->setDirRelative(f_info.Path);

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DirBD", grlCfg.DirBD);
		}
	}
}

void frmOpciones::on_btnDirDB_clear_clicked()
{
	ui->txtDirBD->clear();
}

// Style themes -------------------------------------
void frmOpciones::setColorBtn(QColor color)
{
	int w = 40;
	int h = 16;
	QPixmap img(w, h);
	QPainter painter(&img);
	painter.fillRect(img.rect(), color);
	painter.setPen(Qt::black);
	painter.drawRect(0, 0, w-1, h-1);
	painter.end();
	ui->btnPicFlowBgColor->setIconSize(QSize(w, h));
	ui->btnPicFlowBgColor->setIcon(QIcon(img));
	ui->lb_picflowtype->setStyleSheet("background-color: rgb("+ fGrl->setColorStr(color) +");");
}

void frmOpciones::on_cbxStyle_activated(int index)
{
	grlCfg.Style = ui->cbxStyle->itemData(index).toString();
	QApplication::setStyle(QStyleFactory::create(grlCfg.Style));

	emit on_chkStylePalette_clicked(ui->chkStylePalette->isChecked());
}

void frmOpciones::on_chkStylePalette_clicked(bool checked)
{
	if( checked )
		QApplication::setPalette(QApplication::style()->standardPalette());
	else
		QApplication::setPalette(QApplication::palette());
}

void frmOpciones::on_chkUsarTipoFuente_clicked(bool checked)
{
	grlCfg.font_usar = checked;
	if( checked )
		this->setStyleSheet(fGrl->myStyleSheet() +"*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}");
	else
		this->setStyleSheet(fGrl->myStyleSheet());
}

void frmOpciones::on_cbxFuenteTipo_activated(const QString &arg1)
{
	grlCfg.font_family = arg1;
	emit on_chkUsarTipoFuente_clicked( grlCfg.font_usar );
}

void frmOpciones::on_sbxFontSize_valueChanged(const QString &arg1)
{
	grlCfg.font_size = arg1;
	emit on_chkUsarTipoFuente_clicked( grlCfg.font_usar );
}

void frmOpciones::on_cbxPicFlowType_activated(int index)
{
	if( index > -1 )
		ui->lb_picflowtype->setPixmap(QPixmap(":/images/flow"+ fGrl->IntToStr(index) +".png"));
	else
		ui->lb_picflowtype->setPixmap(QPixmap(":/images/flow0.png"));
}

void frmOpciones::on_btnPicFlowBgColor_clicked()
{
	QColor color = QColorDialog::getColor( fGrl->getColor(grlCfg.PicFlowBgColor), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if( color.isValid() )
	{
		setColorBtn(color);

		grlCfg.PicFlowBgColor.clear();
		grlCfg.PicFlowBgColor = fGrl->setColor(color);
	}
}

void frmOpciones::on_btnEditTheme_clicked()
{
	int id_cat = 1;
	int pos_cat = ui->twCategorias->indexOfTopLevelItem(ui->twCategorias->currentItem());
	if( ui->twCategorias->topLevelItemCount() > 0 && pos_cat != -1 )
		id_cat = ui->twCategorias->currentItem()->text(3).toInt();

	frmListIconCfg *ListIconCfg = new frmListIconCfg(sql, grlCfg, id_cat, grlCfg.NameDirTheme, this);
	ListIconCfg->setWindowFlags(Qt::Window);
	ListIconCfg->exec();
	delete ListIconCfg ;
}

void frmOpciones::on_twThemes_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		grlCfg.NameDirTheme = item->text(0);

		fGrl->setTheme( grlCfg.NameDirTheme );

		if( QFile::exists(fGrl->Theme() + item->text(0) +"/preview.png") )
			ui->lb_theme_example->setPixmap( QPixmap(fGrl->Theme() + item->text(0) +"/preview.png") );
		else
			ui->lb_theme_example->setPixmap( QPixmap(fGrl->ThemeApp() +"images/juego_sin_imagen.png") );

		cargarConfig();
		setTheme();
	} else
		ui->lb_theme_example->setPixmap( QPixmap(fGrl->ThemeApp() +"images/juego_sin_imagen.png") );
}

void frmOpciones::on_twThemes_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if( current != previous )
		emit on_twThemes_itemClicked(current, 0);
}

//---------------------------------------------------
void frmOpciones::on_btnExtVideoAdd_clicked()
{
	if( !ui->txtExtVideo->text().isEmpty() )
	{
		if( ui->txtExtVideo->text().startsWith("*.") )
			ui->lwExtVideo->addItem(ui->txtExtVideo->text());
		else
			ui->lwExtVideo->addItem("*."+ ui->txtExtVideo->text());
	}
}

void frmOpciones::on_btnExtVideoDelete_clicked()
{
	if( ui->lwExtVideo->count() > 0 && ui->lwExtVideo->currentItem()->isSelected() )
	{
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
	if( !ui->txtExtMusic->text().isEmpty() )
	{
		if( ui->txtExtMusic->text().startsWith("*.") )
			ui->lwExtMusic->addItem(ui->txtExtMusic->text());
		else
			ui->lwExtMusic->addItem("*."+ ui->txtExtMusic->text());
	}
}

void frmOpciones::on_btnExtMusicDelete_clicked()
{
	if( ui->lwExtMusic->count() > 0 && ui->lwExtMusic->currentItem()->isSelected() )
	{
		 delete ui->lwExtMusic->currentItem();
	}
}

void frmOpciones::on_btnExtMusicDefecto_clicked()
{
	grlCfg.FormatsMusic.clear();
	grlCfg.FormatsMusic << "*.mp3" << "*.ogg" << "*.wav" << "*.wma";

	ui->lwExtMusic->clear();
	ui->lwExtMusic->addItems(grlCfg.FormatsMusic);
	ui->lwExtMusic->setCurrentRow(0);

}

void frmOpciones::on_btnExtImageAdd_clicked()
{
	if( !ui->txtExtImage->text().isEmpty() )
	{
		if( ui->txtExtImage->text().startsWith("*.") )
			ui->lwExtImage->addItem(ui->txtExtImage->text());
		else
			ui->lwExtImage->addItem("*."+ ui->txtExtImage->text());
	}
}

void frmOpciones::on_btnExtImageDelete_clicked()
{
	if( ui->lwExtImage->count() > 0 && ui->lwExtImage->currentItem()->isSelected() )
	{
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
