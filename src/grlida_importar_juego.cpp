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
#include <QMessageBox>

#include "grlida_importar_juego.h"
#include "ui_importar_juego.h"

// INICIO ImportarTwPrototype -----------------------------------------------------------------------------------
ImportarTwPrototype::ImportarTwPrototype(dbSql *m_sql, QString dir_theme, bool m_importMain, QObject *parent)
	: QObject(parent)
{
	sql        = m_sql;
	stTheme    = dir_theme;
	importMain = m_importMain;
	IdGame     = "";

	datos    = sql->getDefectDatos("datos");
	dosbox   = sql->getDefectDOSBox();
	scummvm  = sql->getDefectScummVM();
	vdmsound = sql->getDefectVDMSound();
}

void ImportarTwPrototype::clearListImport()
{
	list_import.clear();
}

void ImportarTwPrototype::addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, const QString &tipo_emu)
{
	QTreeWidget *twPrototype = qscriptvalue_cast<QTreeWidget*>(thisObject());
	twPrototype->setIconSize(QSize(26, 26));

	QTreeWidgetItem *item = new QTreeWidgetItem;
		if (importMain)
			item->setCheckState(icol_titulo, Qt::Unchecked);
		item->setIcon(icol_titulo, QIcon(stTheme +"img24/"+ tipo_emu +".png"));
		item->setTextAlignment(icol_plataforma, Qt::AlignCenter);
		item->setText(icol_titulo    , titulo    );
		item->setText(icol_anno      , anno      );
		item->setText(icol_plataforma, plataforma);
		item->setText(icol_cbx_dosbox, ""        );
		item->setText(icol_url       , url       );
		item->setText(icol_tipo_emu  , tipo_emu  );
	twPrototype->addTopLevelItem(item);

// Datos
	datos.icono          = tipo_emu +".png";
	datos.titulo         = titulo;
	datos.titulo_guiones = sql->removeAccents(datos.titulo);
	datos.anno           = anno;
	datos.sistemaop      = plataforma;
	datos.tipo_emu       = tipo_emu;

	stImport import;
		import.datos     = datos;
		import.datos_def = datos;
		import.dosbox    = dosbox;
		import.scummvm   = scummvm;
		import.vdmsound  = vdmsound;
	// --
		import.solo_datos      = false;
		import.solo_cfg_dosbox = false;
		import.import_capturas = true;
		import.is_importado    = false;
		import.url_all_covers  = "";
	list_import.insert(item, import);
}

void ImportarTwPrototype::addItemPages(const QString &titulo, const QString &url)
{
	QComboBox *cbx = qscriptvalue_cast<QComboBox*>(thisObject());
	cbx->addItem(titulo, url);
}
// FIN ImportarTwPrototype --------------------------------------------------------------------------------------




frmImportarJuego::frmImportarJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, stConfigDOSBox datos_dbx, stConfigScummVM datos_svm, stConfigVDMSound datos_vdms, bool m_import_main, bool m_editando, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmImportarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql        = m_sql;
	grlCfg     = m_cfg;
	categoria  = m_categoria;
	importMain = m_import_main;
	Editando   = m_editando;
	isScriptURLInit     = true;
	isItemChanged       = false;
	isItemDoubleClicked = false;

	DatosJuego    = datos;
	DatosDosBox   = datos_dbx;
	DatosScummVM  = datos_svm;
	DatosVDMSound = datos_vdms;

	grlDir.Home       = fGrl->dirApp();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";
	grlDir.Scripts    = grlDir.Home +"scripts/";
	grlDir.Iconos     = grlDir.Home +"iconos/";
	grlDir.Smiles     = grlDir.Home +"smiles/";
	grlDir.Temp       = grlDir.Home +"temp/";

	createWidgets();
	cargarConfig();
}

frmImportarJuego::~frmImportarJuego()
{
	z_file->closeArchive();
	delete z_file;

	delete ui;
}

void frmImportarJuego::closeEvent(QCloseEvent *event)
{
	grlCfg.import_geometry = this->saveGeometry();
	grlCfg.import_splitter_state = ui->splitter->saveState();

	event->accept();
}

void frmImportarJuego::resizeEvent(QResizeEvent *event)
{
	msgInfo->setGeometry(QRect((this->width()/2)-200, (this->height()/2)-45, 400, 90));

	QDialog::resizeEvent(event);
}

void frmImportarJuego::createWidgets()
{
	QFont msgFont;
	msgFont.setPointSize(28);

	msgInfo = new QFrame(this);
	msgInfo->setWindowTitle(tr("Información"));
	msgInfo->setGeometry(QRect((this->width()/2)-200, (this->height()/2)-45, 400, 90));
	msgInfo->setFrameShape(QFrame::WinPanel);
	msgInfo->setFrameShadow(QFrame::Raised);
	msgInfo->setStyleSheet("background-color: "+ this->palette().window().color().name() +";");
	QVBoxLayout *vLayoutMsg = new QVBoxLayout(msgInfo);
		lb_msgInfo = new QLabel(msgInfo);
		lb_msgInfo->setFont(msgFont);
		lb_msgInfo->setAlignment(Qt::AlignCenter);
	vLayoutMsg->addWidget(lb_msgInfo);
	msgInfo->hide();

	httpdown = new HttpDownload(ui->wInfo);
	ui->hLayout_wInfo->addWidget(httpdown);
	connect(httpdown, SIGNAL(statusBtnEnabled(bool)), this, SLOT(statusBtnEnabled(bool)));
	connect(httpdown, SIGNAL(statusFinished()), this, SLOT(statusFinished()));

	twProto = new ImportarTwPrototype(sql, fGrl->theme(), importMain, this);
	engine  = new QScriptEngine(this);
	engine->setDefaultPrototype(qMetaTypeId<QTreeWidget*>(), engine->newQObject(twProto));
	engine->setDefaultPrototype(qMetaTypeId<QComboBox*>(), engine->newQObject(twProto));
	engine->globalObject().setProperty("twListaBusqueda", engine->newQObject(ui->twListaJuegos));
	engine->globalObject().setProperty("cbxPaginas"     , engine->newQObject(ui->cbxNumPag));

	ui->cbxNumPag->clear();
	ui->cbxNumPag->addItem("1", "");
	total_num_pag = fGrl->intToStr(ui->cbxNumPag->count());
	num_pag = fGrl->intToStr(ui->cbxNumPag->currentIndex() + 1);
	ui->lb_num_paginas->setText(tr("Página") +" "+ num_pag +" "+ tr("de") +" "+ total_num_pag);

	wDat = new frmImportarDatos(grlCfg, categoria, DatosJuego, Editando, this);
	ui->vLayout_wDat->addWidget(wDat);
	connect(wDat, SIGNAL(cfgDatChanged(stDatosJuego)), this, SLOT(onCfgDatChanged(stDatosJuego)));

	wDbx = new frmImportarDosBox(grlCfg, categoria, DatosDosBox, Editando, this);
	ui->vLayout_wDbx->addWidget(wDbx);
	connect(wDbx, SIGNAL(cfgDbxChanged(stConfigDOSBox)), this, SLOT(onCfgDbxChanged(stConfigDOSBox)));

	wSvm = new frmImportarScummVM(grlCfg, categoria, DatosScummVM, Editando, this);
	ui->vLayout_wSvm->addWidget(wSvm);
	connect(wSvm, SIGNAL(cfgSvmChanged(stConfigScummVM)), this, SLOT(onCfgSvmChanged(stConfigScummVM)));

	z_file = new Qt7zip();
	is_load_7zlib = z_file->isLoad7zLib();

	ui->txtInfo->document()->setDocumentMargin(0);
	ui->txtInfoJuego->document()->setDocumentMargin(0);
}

void frmImportarJuego::cargarConfig()
{
	fGrl->comprobarDirectorio(grlDir.Temp +"imagenes/");
	fGrl->comprobarDirectorio(grlDir.Temp +"imagenes/small/");

	if (grlCfg.ProxyEnable)
		httpdown->setHttpProxy(grlCfg.ProxyType, grlCfg.ProxyHost, grlCfg.ProxyPort, grlCfg.ProxyUserName, grlCfg.ProxyPassword);

	cargarTextResource();

	dbx_list = wDbx->getDbxList();

	emu_list.clear();
	emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");

	str_replace_list.clear();
	str_replace_list = fGrl->cargaDatosQHash(":/datos/text_html_convert.txt", 2, "|");

// Lista iconos
	icon_list.clear();
	icon_title_list.clear();
	icon_list = fGrl->getListFiles(grlDir.Iconos, grlCfg.FormatsImage);
	const int listSize = icon_list.size();
	for (int i = 0; i < listSize; ++i)
	{
		icon_title_list.insert(i, icon_list.at(i).fileName());
	}

// Configuración del twListaJuegos
	ui->twListaJuegos->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twListaJuegos->header()->setSectionsMovable(false);
	ui->twListaJuegos->header()->setSectionResizeMode(icol_titulo    , QHeaderView::Interactive);// Interactive
	ui->twListaJuegos->header()->setSectionResizeMode(icol_anno      , QHeaderView::Interactive);
	ui->twListaJuegos->header()->setSectionResizeMode(icol_plataforma, QHeaderView::Interactive);
	ui->twListaJuegos->header()->setSectionResizeMode(icol_cbx_dosbox, QHeaderView::ResizeToContents);
#else
	ui->twListaJuegos->header()->setMovable(false);
	ui->twListaJuegos->header()->setResizeMode(icol_titulo    , QHeaderView::Interactive);// Stretch
	ui->twListaJuegos->header()->setResizeMode(icol_anno      , QHeaderView::Interactive);
	ui->twListaJuegos->header()->setResizeMode(icol_plataforma, QHeaderView::Interactive);
	ui->twListaJuegos->header()->setResizeMode(icol_cbx_dosbox, QHeaderView::ResizeToContents);
#endif
	ui->twListaJuegos->setColumnWidth(icol_titulo    , 200);
	ui->twListaJuegos->setColumnWidth(icol_anno      ,  36);
	ui->twListaJuegos->setColumnWidth(icol_plataforma,  80);
// Común
	ui->twListaJuegos->headerItem()->setText(icol_titulo    , tr("Título")    );
	ui->twListaJuegos->headerItem()->setText(icol_anno      , tr("Año"       ));
	ui->twListaJuegos->headerItem()->setText(icol_plataforma, tr("Plataforma"));
	ui->twListaJuegos->headerItem()->setText(icol_cbx_dosbox,    "DOSBox"     );
	ui->twListaJuegos->setIconSize(QSize(26, 26));
// Ocultamos las columnas
	ui->twListaJuegos->hideColumn(icol_cbx_dosbox);
	ui->twListaJuegos->hideColumn(icol_url);
	ui->twListaJuegos->hideColumn(icol_tipo_emu);

	ui->chkSelectListaJuegos->setVisible(!Editando);

// Configuración del twDatosFiles
	ui->twDatosFiles->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosFiles->header()->setSectionsMovable(false);
	ui->twDatosFiles->header()->setSectionResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setSectionResizeMode(1, QHeaderView::Fixed      );
	ui->twDatosFiles->header()->setSectionResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setSectionResizeMode(3, QHeaderView::Interactive);
#else
	ui->twDatosFiles->header()->setMovable(false);
	ui->twDatosFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui->twDatosFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(3, QHeaderView::Interactive);
#endif
	ui->twDatosFiles->setColumnWidth(0, 200);
	ui->twDatosFiles->setColumnWidth(1, 60 );
	ui->twDatosFiles->setColumnWidth(2, 100);
	ui->twDatosFiles->setColumnWidth(3, 50 );
	ui->twDatosFiles->headerItem()->setText(0, tr("Nombre"     ));
	ui->twDatosFiles->headerItem()->setText(1, tr("CRC"        ));
	ui->twDatosFiles->headerItem()->setText(2, tr("Descripción"));
	ui->twDatosFiles->headerItem()->setText(3, tr("Tamaño"     ));
	ui->twDatosFiles->headerItem()->setText(4, tr("Path"       ));

// Configuración del twDatosURL
	ui->twDatosURL->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosURL->header()->setSectionsMovable(false);
	ui->twDatosURL->header()->setSectionResizeMode(0, QHeaderView::Interactive);
#else
	ui->twDatosURL->header()->setMovable(false);
	ui->twDatosURL->header()->setResizeMode(0, QHeaderView::Interactive);
#endif
	ui->twDatosURL->setColumnWidth(0, 250);
	ui->twDatosURL->headerItem()->setText(0, tr("Url"        ));
	ui->twDatosURL->headerItem()->setText(1, tr("Descripción"));

// Configuración del twDatosImages
	ui->twDatosImages->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twDatosImages->header()->setSectionsMovable(false);
	ui->twDatosImages->header()->setSectionResizeMode(col_img_thumb , QHeaderView::Interactive);
	ui->twDatosImages->header()->setSectionResizeMode(col_img_nombre, QHeaderView::Interactive);
	ui->twDatosImages->header()->setSectionResizeMode(col_img_tipo  , QHeaderView::Interactive);
#else
	ui->twDatosImages->header()->setMovable(false);
	ui->twDatosImages->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosImages->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDatosImages->header()->setResizeMode(2, QHeaderView::Interactive);
#endif
	ui->twDatosImages->setColumnWidth(col_img_thumb, 250);
	ui->twDatosImages->headerItem()->setText(col_img_thumb , tr("Thumbs"));
	ui->twDatosImages->headerItem()->setText(col_img_nombre, tr("Nombre"));
	ui->twDatosImages->headerItem()->setText(col_img_tipo  , tr("Tipo"  ));

// Configuración del twDatosTempImages
	ui->twDatosTempImages->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twDatosTempImages->header()->setSectionsMovable(false);
	ui->twDatosTempImages->header()->setSectionResizeMode(col_img_thumb , QHeaderView::Interactive);
	ui->twDatosTempImages->header()->setSectionResizeMode(col_img_nombre, QHeaderView::Interactive);
	ui->twDatosTempImages->header()->setSectionResizeMode(col_img_tipo  , QHeaderView::Interactive);
#else
	ui->twDatosTempImages->header()->setMovable(false);
	ui->twDatosTempImages->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosTempImages->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDatosTempImages->header()->setResizeMode(2, QHeaderView::Interactive);
#endif
	ui->twDatosTempImages->setColumnWidth(col_img_thumb, 250);
	ui->twDatosTempImages->headerItem()->setText(col_img_thumb , tr("Thumbs"));
	ui->twDatosTempImages->headerItem()->setText(col_img_nombre, tr("Nombre"));
	ui->twDatosTempImages->headerItem()->setText(col_img_tipo  , tr("Tipo"  ));

// Configuración del twMontajes
	ui->twMontajes->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twMontajes->header()->setSectionsMovable(false);
	ui->twMontajes->header()->setSectionResizeMode(0, QHeaderView::Interactive);
#else
	ui->twMontajes->header()->setMovable(false);
	ui->twMontajes->header()->setResizeMode(0, QHeaderView::Interactive);
#endif
	ui->twMontajes->setColumnWidth(1, 40);
	ui->twMontajes->setColumnWidth(2, 80);
	ui->twMontajes->setColumnWidth(3, 80);

	if (!DatosJuego.titulo.isEmpty())
		ui->txtTituloBuscar->setText(DatosJuego.titulo);
	else
		ui->txtTituloBuscar->setText("");
	ui->txtTituloBuscar->setFocus();

	codecFileHtml.clear();
	codecFileHtml.append("UTF-8");

	setTheme();

	fGrl->cargarScriptsComboBox(ui->cbxScriptURL, grlDir.Scripts, "img16/", grlCfg.url_xmldb);
	emit on_cbxScriptURL_activated(ui->cbxScriptURL->currentIndex());

	setTabImportEnabled("datos");

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
	if (!grlCfg.import_geometry.isEmpty())
		this->restoreGeometry(grlCfg.import_geometry);

	ui->splitter->restoreState(grlCfg.import_splitter_state);
}

void frmImportarJuego::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/importar.png"));

	ui->btnAbrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnAbrirUrl->setIcon(QIcon(fGrl->theme() +"img16/edit_enlace.png"));
	ui->btnBuscar->setIcon(QIcon(fGrl->theme() +"img16/buscar.png"));
	ui->btnAbortar->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));

	ui->btnDownInfo->setIcon(QIcon(fGrl->theme() +"img16/datos_2.png"));
	ui->btnToggleInfo->setIcon(QIcon(fGrl->theme() +"img16/go-next.png"));
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancelar->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));

	ui->tabwImport->setTabIcon(tabInfo    , QIcon(fGrl->theme() +"img16/datos_3.png"    ));
	ui->tabwImport->setTabIcon(tabDatos   , QIcon(fGrl->theme() +"img16/cat/datos.png"  ));
	ui->tabwImport->setTabIcon(tabDOSBox  , QIcon(fGrl->theme() +"img16/cat/dosbox.png" ));
	ui->tabwImport->setTabIcon(tabScummVM , QIcon(fGrl->theme() +"img16/cat/scummvm.png"));
	ui->tabwImport->setTabIcon(tabOpciones, QIcon(fGrl->theme() +"img16/opciones.png"   ));

	ui->tabwDatos->setTabIcon(0, QIcon(fGrl->theme() +"img16/cat/datos.png"));
	ui->tabwDatos->setTabIcon(1, QIcon(fGrl->theme() +"img16/archivos.png" ));
	ui->tabwDatos->setTabIcon(2, QIcon(fGrl->theme() +"img16/html.png"     ));
	ui->tabwDatos->setTabIcon(3, QIcon(fGrl->theme() +"img16/imagen.png"   ));

	ui->tabwDOSBox->setTabIcon(0, QIcon(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->tabwDOSBox->setTabIcon(1, QIcon(fGrl->theme() +"img16/drive_hd.png"  ));

	tpl_info_old            = fGrl->tplInfoJuego("tpl_info");
	tpl_info_game_old       = fGrl->tplInfoJuego("tpl_info_juego");
	tpl_info_game_empty_old = fGrl->tplInfoJuego("tpl_info_juego_no");

	QString tpl_info_new = tpl_info_old;
	tpl_info_new.replace("{info_contenido}", "");
	ui->txtInfo->setHtml(tpl_info_new);

	QString tpl_info_game_empty_new = tpl_info_game_empty_old;
	tpl_info_game_empty_new.replace("{info_contenido}", tr("Información no disponible"));
	ui->txtInfoJuego->setHtml(tpl_info_game_empty_new);
}

void frmImportarJuego::cargarTextResource()
{
	ui->txtInfo->clear();
	ui->txtInfoJuego->clear();

	QHash<QString, QImage> list_resource;
	list_resource.insert("img_rs_star_on.png"     , QImage(fGrl->theme() +"img16/star_on.png")                      );
	list_resource.insert("img_rs_star_off.png"    , QImage(fGrl->theme() +"img16/star_off.png")                     );
	list_resource.insert("img_rs_original.png"    , QImage(fGrl->theme() +"img16/original.png")                     );
	list_resource.insert("img_rs_original_off.png", fGrl->imagenToDisabled(fGrl->theme() +"img16/original.png")     );
	list_resource.insert("img_rs_favorito.png"    , QImage(fGrl->theme() +"img16/"+ grlCfg.IconoFav)                );
	list_resource.insert("img_rs_favorito_off.png", fGrl->imagenToDisabled(fGrl->theme() +"img16/"+ grlCfg.IconoFav));
	list_resource.insert("img_rs_gamepad.png"     , QImage(fGrl->theme() +"img16/gamepad.png")                      );
	list_resource.insert("img_rs_gamepad_off.png" , QImage(fGrl->theme() +"img16/gamepad_off.png")                  );

	edades_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->idioma() +"/edad_recomendada.txt", 3, "|");
	foreach (const stGrlDatos &edades, edades_list)
		list_resource.insert("edad_rs_"+ edades.icono, QImage(fGrl->theme() +"img16/"+ edades.icono));

	idiomas_list = fGrl->cargaDatosQHash(grlDir.Datos + fGrl->idioma() +"/idiomas.txt", 3, "|");
	foreach (const stGrlDatos &idioma, idiomas_list)
		list_resource.insert("idioma_rs_"+ idioma.icono, QImage(fGrl->theme() +"img16/"+ idioma.icono));

	smiles_list = fGrl->cargaDatosQHash(grlDir.Datos +"smiles.txt", 2, "\",\"");
	foreach (const stGrlDatos &smile, smiles_list)
	{
		if (fGrl->getInfoFile(grlDir.Smiles + smile.icono).Ext == ".gif")
		{
			ui->txtInfo->addAnimation(QUrl("smile_rs_"+ smile.key +"_"+ smile.icono), grlDir.Smiles + smile.icono);
			ui->txtInfoJuego->addAnimation(QUrl("smile_rs_"+ smile.key +"_"+ smile.icono), grlDir.Smiles + smile.icono);
		} else
			list_resource.insert("smile_rs_"+ smile.key +"_"+ smile.icono, QImage(grlDir.Smiles + smile.icono));
	}

	foreach (const stGrlDatos &dbx, dbx_list)
		list_resource.insert("emu_rs_"+ dbx.key, QImage(fGrl->theme() +"img16/"+ dbx.icono));

	QList<stGrlDatos> list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	foreach (const stGrlDatos &emu, list_emu)
		list_resource.insert("emu_rs_"+ emu.key, QImage(fGrl->theme() +"img16/"+ emu.icono));

	list_resource.insert("emu_rs_datos"     , QImage(fGrl->theme() +"img16/datos.png")     );
	list_resource.insert("emu_rs_dosbox"    , QImage(fGrl->theme() +"img16/dosbox.png")    );
	list_resource.insert("emu_rs_scummvm"   , QImage(fGrl->theme() +"img16/scummvm.png")   );
	list_resource.insert("emu_rs_vdmsound"  , QImage(fGrl->theme() +"img16/vdmsound.png")  );
	list_resource.insert("emu_rs_pc"        , QImage(fGrl->theme() +"img16/pc.png")        );
	list_resource.insert("emu_rs_emuladores", QImage(fGrl->theme() +"img16/emuladores.png"));

	QHash<QString, QImage>::const_iterator idxr = list_resource.constBegin();
	while (idxr != list_resource.constEnd())
	{
		ui->txtInfo->document()->addResource(QTextDocument::ImageResource, QUrl(idxr.key()), idxr.value());
		ui->txtInfoJuego->document()->addResource(QTextDocument::ImageResource, QUrl(idxr.key()), idxr.value());
		++idxr;
	}
}

void frmImportarJuego::cargarScript(QString fileScript)
{
// Cargamos el script
	engine->evaluate(fGrl->leerArchivo(fileScript));

	codecFileHtml.clear();
	if (engine->evaluate("url_charset").isValid())
		codecFileHtml.append(engine->evaluate("url_charset").toString());
	else
		codecFileHtml.append("UTF-8");

	if (engine->evaluate("url_site").isValid())
		str_url_web = engine->evaluate("url_site").toString();
	else
		str_url_web = "";
}

QString frmImportarJuego::leerArchivoHTML(QString file_html)
{
	QString text_html = fGrl->leerArchivo(file_html, codecFileHtml);
	if (text_html.isEmpty())
		text_html = "";
	else {
		foreach (const stGrlDatos &str, str_replace_list)
			text_html.replace(str.key, str.icono, Qt::CaseInsensitive);
	}
	return text_html;
}

void frmImportarJuego::mostrarFichaHtml(QTreeWidgetItem *item)
{
	if (item != NULL)
	{
		stImport i = list_import_temp[item];

		QString tipo_emu      = i.datos.tipo_emu;
		QString img_tipo_emu  = "<img src=\"emu_rs_"+ i.datos.tipo_emu +"\"> ";
		QString dbx_usado     = "";
		QString dbx_usado_img = "";

		if (i.datos.tipo_emu == "datos")
			tipo_emu = tr("Datos");
		else if (i.datos.tipo_emu == "scummvm")
			tipo_emu = "ScummVM";
		else if (i.datos.tipo_emu == "vdmsound")
			tipo_emu = "VDMSound";
		else if (i.datos.tipo_emu == "dosbox")
		{
			stGrlDatos dbx_dato;
			if (dbx_list.contains(i.dosbox.dosbox_emu_key))
			{
				dbx_dato = dbx_list[i.dosbox.dosbox_emu_key];
			} else {
				const int num_dbx_count = dbx_list.count(grlCfg.DOSBoxDefault);
				if (num_dbx_count > 0)
					dbx_dato = dbx_list[grlCfg.DOSBoxDefault];
				else {
					QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
					dbx_dato = i_dbx.value();
				}
			}

			dbx_usado     = dbx_dato.titulo;
			dbx_usado_img = dbx_dato.icono;
			tipo_emu      = "DOSBox";
		} else {
			if (emu_list.contains(i.datos.tipo_emu))
			{
				tipo_emu     = emu_list[i.datos.tipo_emu].titulo;
				img_tipo_emu = "<img src=\"emu_rs_"+ emu_list[i.datos.tipo_emu].key +"\"> ";
			} else {
				tipo_emu     = tr("Otro emulador");
				img_tipo_emu = "<img src=\"emu_rs_emuladores\"> ";
			}
		}

	// Datos
		QString dat_idioma          = fGrl->getImgDatos(idiomas_list, i.datos.idioma.split(";")      , true);
		QString dat_idioma_voces    = fGrl->getImgDatos(idiomas_list, i.datos.idioma_voces.split(";"), true);
		QString dat_img_edad_titulo = edades_list[i.datos.edad_recomendada].titulo;
		QString dat_img_edad        = "<img src=\"edad_rs_"+ edades_list[i.datos.edad_recomendada].icono +"\" alt=\""+ dat_img_edad_titulo +"\" title=\""+ dat_img_edad_titulo +"\"> ";
		QString dat_comentario      = fGrl->reemplazaTextoSmiles(i.datos.comentario, smiles_list);

		QString dat_thumbs = fGrl->theme() +"images/juego_sin_imagen.png";
		if (!i.datos.thumbs.isEmpty() && QFile::exists(grlDir.Temp +"imagenes/small/"+ i.datos.thumbs))
			dat_thumbs = grlDir.Temp +"imagenes/small/"+ i.datos.thumbs;

		QString dat_icono = ":/img24/sinimg.png";
		if (!i.datos.icono.isEmpty() && QFile::exists(fGrl->theme() +"img24/"+ i.datos.icono))
			dat_icono = fGrl->theme() +"img24/"+ i.datos.icono;

		QString dat_rating = "";
		for (int d=0; d < 5; ++d)
		{
			if (d < i.datos.rating.toInt())
				dat_rating.append("<img src=\"img_rs_star_on.png\" alt=\""+ fGrl->intToStr(d+1) +"\" title=\""+ fGrl->intToStr(d+1) +"\"> ");
			else
				dat_rating.append("<img src=\"img_rs_star_off.png\"> ");
		}

		QString img_original = "<img src=\"img_rs_original_off.png\"> ";
		if (fGrl->strToBool(i.datos.original))
			img_original = "<img src=\"img_rs_original.png\"> ";

		QString img_favorito = "<img src=\"img_rs_favorito_off.png\"> ";
		if (fGrl->strToBool(i.datos.favorito))
			img_favorito = "<img src=\"img_rs_favorito.png\"> ";

		QString img_gamepad = "<img src=\"img_rs_gamepad_off.png\"> ";
		if (fGrl->strToBool(i.datos.gamepad))
			img_gamepad = "<img src=\"img_rs_gamepad.png\"> ";

		QString tpl_info_game_new = tpl_info_game_old;
		tpl_info_game_new.replace("{info_icono}"               , "<img src="+ QUrl::fromLocalFile(dat_icono).path() +">"); //  width=\"24\" height=\"24\"
		tpl_info_game_new.replace("{info_titulo}"              , i.datos.titulo       );
		tpl_info_game_new.replace("{info_subtitulo}"           , i.datos.subtitulo    );
		tpl_info_game_new.replace("{info_genero}"              , i.datos.genero       );
		tpl_info_game_new.replace("{info_compania}"            , i.datos.compania     );
		tpl_info_game_new.replace("{info_desarrollador}"       , i.datos.desarrollador);
		tpl_info_game_new.replace("{info_tema}"                , i.datos.tema         );
		tpl_info_game_new.replace("{info_grupo}"               , i.datos.grupo        );
		tpl_info_game_new.replace("{info_perspectiva}"         , i.datos.perspectiva  );
		tpl_info_game_new.replace("{info_idioma}"              , dat_idioma           );
		tpl_info_game_new.replace("{info_idioma_voces}"        , dat_idioma_voces     );
		tpl_info_game_new.replace("{info_formato}"             , i.datos.formato      );
		tpl_info_game_new.replace("{info_anno}"                , i.datos.anno         );
		tpl_info_game_new.replace("{info_numdisc}"             , i.datos.numdisc      );
		tpl_info_game_new.replace("{info_img_edad_recomendada}", dat_img_edad         );
		tpl_info_game_new.replace("{info_edad_recomendada}"    , dat_img_edad_titulo  );
		tpl_info_game_new.replace("{info_sistemaop}"           , i.datos.sistemaop    );
		tpl_info_game_new.replace("{info_tamano}"              , i.datos.tamano       );
		tpl_info_game_new.replace("{info_graficos}"            , i.datos.graficos     );
		tpl_info_game_new.replace("{info_sonido}"              , i.datos.sonido       );
		tpl_info_game_new.replace("{info_jugabilidad}"         , i.datos.jugabilidad  );
		tpl_info_game_new.replace("{info_original}"            , fGrl->strToBool(i.datos.original) ? tr("Si") : tr("No"));
		tpl_info_game_new.replace("{img_original}"             , img_original         );
		tpl_info_game_new.replace("{info_estado}"              , i.datos.estado       );
		tpl_info_game_new.replace("{info_thumbs}"              , QUrl::fromLocalFile(dat_thumbs).path());
		tpl_info_game_new.replace("{info_fecha}"               , fGrl->horaFechaActual(i.datos.fecha, grlCfg.FormatoFecha));
		tpl_info_game_new.replace("{info_tipo_emu}"            , tipo_emu             );
		tpl_info_game_new.replace("{img_tipo_emu}"             , img_tipo_emu         );
		tpl_info_game_new.replace("{info_favorito}"            , fGrl->strToBool(i.datos.favorito) ? tr("Si") : tr("No"));
		tpl_info_game_new.replace("{img_favorito}"             , img_favorito         );
		tpl_info_game_new.replace("{info_gamepad}"             , fGrl->strToBool(i.datos.gamepad) ? tr("Si") : tr("No"));
		tpl_info_game_new.replace("{img_gamepad}"              , img_gamepad          );
		tpl_info_game_new.replace("{info_rating}"              , dat_rating           );
		tpl_info_game_new.replace("{info_usuario}"             , i.datos.usuario      );
		tpl_info_game_new.replace("{info_comentario}"          , sql->removeHtmlTag(dat_comentario));
		tpl_info_game_new.replace("<dir_scripts>"              , grlDir.Scripts       );
		tpl_info_game_new.replace("<theme>"                    , fGrl->theme()        );
		tpl_info_game_new.replace("{dbx_usado}"                , dbx_usado            );
		tpl_info_game_new.replace("{dbx_usado_img}"            , dbx_usado_img        );

		ui->txtInfoJuego->setHtml(tpl_info_game_new);
		wDat->setEditorDatos(i.datos, i.datos_def);

		setTabImportEnabled(i.datos.tipo_emu);

		mostrarURLs(i.urls);
		mostrarFiles(i.files);

		setScriptImagenes(ui->twDatosImages, i.imagenes);
		setScriptImagenes(ui->twDatosTempImages, i.imagenes_tmp, true);

	// DOSBox
		if (i.datos.tipo_emu == "dosbox")
		{
			wDbx->setEditorDOSBox(i.dosbox);
			wDbx->mountAdd(ui->twMontajes, i.dbxMount);
		}

	// ScummVM
		if (i.datos.tipo_emu == "scummvm")
		{
			wSvm->setEditorScummVM(i.scummvm);
		}

	// VDMSound
	//	if (i.datos.tipo_emu == "vdmsound")
	//	{
	//		wVdms->setEditorVDMSound(i.vdmsound);
	//	}

		ui->chkSoloDatos->setChecked(i.solo_datos);
		ui->chkSoloCfgDOSBox->setChecked(i.solo_cfg_dosbox);
		ui->chkImportarCapturas->setChecked(i.import_capturas);
	} else {
		ui->txtInfoJuego->setHtml(tpl_info_game_old);
		ui->twDatosFiles->clear();
		ui->twDatosURL->clear();
		ui->twDatosImages->clear();
		ui->twDatosTempImages->clear();
		ui->twMontajes->clear();

		stDatosJuego datos = sql->getDefectDatos("datos");
		wDat->setEditorDatos(datos, datos);
		wDbx->setEditorDOSBox(sql->getDefectDOSBox());
		wSvm->setEditorScummVM(sql->getDefectScummVM());
//		wVdms->setEditorVDMSound(sql->getDefectVDMSound());
	}
}

void frmImportarJuego::mostrarURLs(QList<stDatosUrls> urls)
{
	ui->twDatosURL->clear();
	const int num_urls = urls.size();
	if (num_urls > 0)
	{
		QList<QTreeWidgetItem *> items;
		for (int i = 0; i < num_urls; ++i)
		{
			QTreeWidgetItem *item_url = new QTreeWidgetItem;
				item_url->setIcon(0, QIcon(fGrl->theme() +"img16/html.png"));
				item_url->setText(0, urls.at(i).url        );
				item_url->setText(1, urls.at(i).descripcion);
				item_url->setText(2, urls.at(i).id         );
				item_url->setText(3, urls.at(i).idgrl      );
				item_url->setText(4, urls.at(i).idcat      );
			items << item_url;
		}
		ui->twDatosURL->addTopLevelItems(items);
	}
}

void frmImportarJuego::mostrarFiles(QList<stDatosFiles> files)
{
	ui->twDatosFiles->clear();
	const int num_files = files.size();
	if (num_files > 0)
	{
		QList<QTreeWidgetItem *> items;
		for (int i = 0; i < num_files; ++i)
		{
			QTreeWidgetItem *item_files = new QTreeWidgetItem;
				QString file_ico = files.at(i).tipo;
				if (file_ico == "manual")
					item_files->setIcon(0, QIcon(fGrl->theme() +"img16/cat/datos.png"));
				else if (file_ico == "pdf")
					item_files->setIcon(0, QIcon(fGrl->theme() +"img16/pdf.png"));
				else if (file_ico == "ruleta")
					item_files->setIcon(0, QIcon(fGrl->theme() +"img16/ruleta.png"));
				else if (file_ico == "archivo")
					item_files->setIcon(0, QIcon(fGrl->theme() +"img16/archivos.png"));
				else
					item_files->setIcon(0, QIcon(fGrl->theme() +"img16/archivos.png"));
				item_files->setTextAlignment(1, Qt::AlignCenter);
				item_files->setTextAlignment(3, Qt::AlignCenter);
				item_files->setText(0, files.at(i).nombre      );
				item_files->setText(1, files.at(i).crc         );
				item_files->setText(2, files.at(i).descripcion );
				item_files->setText(3, sql->covertir_bytes(files.at(i).size.toDouble()));
				item_files->setText(4, files.at(i).path        );
				item_files->setText(5, files.at(i).tipo        );
				item_files->setText(6, files.at(i).id          );
				item_files->setText(7, files.at(i).idgrl       );
				item_files->setText(8, files.at(i).idcat       );
				item_files->setText(9, files.at(i).size        );
			items << item_files;
		}
		ui->twDatosFiles->addTopLevelItems(items);
	}
}

void frmImportarJuego::setTabImportEnabled(QString tipo_emu)
{
	if (tipo_emu == "dosbox")
		ui->tabwImport->setTabEnabled(tabDOSBox, true);
	else {
		if (ui->tabwImport->currentIndex() == tabDOSBox && ui->tabwImport->isTabEnabled(tabDOSBox))
			ui->tabwImport->setCurrentIndex(tabInfo);
		ui->tabwImport->setTabEnabled(tabDOSBox, false);
	}

	if (tipo_emu == "scummvm")
		ui->tabwImport->setTabEnabled(tabScummVM, true);
	else {
		if (ui->tabwImport->currentIndex() == tabScummVM && ui->tabwImport->isTabEnabled(tabScummVM))
			ui->tabwImport->setCurrentIndex(tabInfo);
		ui->tabwImport->setTabEnabled(tabScummVM, false);
	}
}

QString frmImportarJuego::correctSlash(QString str, bool remplace)
{
	if (remplace)
		str.replace("\\", "/");
	return str;
}

void frmImportarJuego::defectoLimpiar()
{
	QString tpl_info_new = tpl_info_old;
	tpl_info_new.replace("{info_contenido}", "");

	QString tpl_info_game_empty_new = tpl_info_game_empty_old;
	tpl_info_game_empty_new.replace("{info_contenido}", tr("Información no disponible"));

	ui->txtInfo->setHtml(tpl_info_new);
	ui->txtInfoJuego->setHtml(tpl_info_game_empty_new);

	ui->twListaJuegos->clear();
	ui->twDatosFiles->clear();
	ui->twDatosURL->clear();
	ui->twDatosImages->clear();
	ui->twDatosTempImages->clear();
	ui->twMontajes->clear();
	twProto->clearListImport();
	list_import_temp.clear();
}

bool frmImportarJuego::isSelectedItem(QTreeWidgetItem *item)
{
	int pos = ui->twListaJuegos->indexOfTopLevelItem(item);
	if (ui->twListaJuegos->topLevelItemCount() > 0 && pos != -1)
		return true;
	else
		return false;
}

// Slots
// --------------------------------------------------------------------------------------------------------------
void frmImportarJuego::onCfgDatChanged(stDatosJuego cfgDat)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].datos = cfgDat;
//		list_import_temp[item].datos_chk = wDat->getCheckedDatos();
	}
}

void frmImportarJuego::onCfgDbxChanged(stConfigDOSBox cfgDbx)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].dosbox = cfgDbx;

		QString emu_key = cfgDbx.dosbox_emu_key;
		item->setText(icol_cbx_dosbox, emu_key);

		if (type_import == t_DBGL)
		{
			QComboBox *cmb = qobject_cast<QComboBox*>(ui->twListaJuegos->itemWidget(item, icol_cbx_dosbox));
			int index = cmb->findData(emu_key);
			index = (index > -1) ? index : 0;
			cmb->setCurrentIndex(index);
		}
	}
}

void frmImportarJuego::onCfgSvmChanged(stConfigScummVM cfgSvm)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].scummvm = cfgSvm;
	}
}

void frmImportarJuego::onCbxDbx_activated(QTreeWidgetItem *item)
{
	QString emu_key = item->text(icol_cbx_dosbox);
	list_import_temp[item].dosbox.dosbox_emu_key = emu_key;
	wDbx->setEmuKeyDOSBox(emu_key);
}

void frmImportarJuego::on_cbxScriptURL_activated(int index)
{
	if (index > -1)
	{
		ui->btnToggleInfo->setEnabled(false);
		ui->btnOk->setEnabled(false);

		ui->twListaJuegos->hideColumn(icol_cbx_dosbox);
	//	ui->twListaJuegos->headerItem()->setText(icol_plataforma, "Plataforma");
		bool url_changed = false;

		if (grlCfg.url_xmldb != ui->cbxScriptURL->itemData(index).toString() || isScriptURLInit)
		{
			isScriptURLInit = false;

			grlCfg.url_xmldb = ui->cbxScriptURL->itemData(index).toString();
			url_changed = true;

			defectoLimpiar();

			ui->w_barra_busqueda->setVisible(false);
			ui->btnAbrir->setEnabled(true);
			ui->btnAbrirUrl->setEnabled(false);
			ui->btnBuscar->setEnabled(true);
			ui->btnAbortar->setEnabled(false);
			ui->btnDownAllCovers->setEnabled(false);
			ui->btnToggleInfo->setVisible(true);

			httpdown->defaultInfo();
			setTabImportEnabled("datos");
		}

		if (grlCfg.url_xmldb == "_desde_archivo_")
		{
			type_import = t_File;
		}
		else if (grlCfg.url_xmldb == "_desde_dbgl_")
		{
			type_import = t_DBGL;
			ui->twListaJuegos->showColumn(icol_cbx_dosbox);
		//	ui->twListaJuegos->headerItem()->setText(icol_plataforma, "DOSBox");
		} else {
			type_import = t_Script;

			ui->w_barra_busqueda->setVisible(true);
			ui->btnAbrir->setEnabled(false);
			ui->btnAbrirUrl->setEnabled(true);
			ui->btnToggleInfo->setVisible(false);

			cargarScript(grlDir.Scripts + grlCfg.url_xmldb);
		}

		if (url_changed)
		{
			if (type_import == t_File || type_import == t_DBGL)
				ui->swImport->setCurrentIndex(0);
			else {
				ui->swImport->setCurrentIndex(1);
				httpdown->defaultInfo();
			}
		}

		ui->btnToggleInfo->setText(ui->swImport->currentIndex() > 0 ? tr("Anterior") : tr("Siguiente"));
		ui->btnToggleInfo->setIcon(QIcon(fGrl->theme() +"img16/"+ (ui->swImport->currentIndex() > 0 ? "go-back.png" : "go-next.png")));
	}
}

void frmImportarJuego::on_txtTituloBuscar_returnPressed()
{
//	emit on_btnBuscar_clicked();
}

void frmImportarJuego::on_cbxNumPag_activated(int index)
{
	if (index > -1)
		index_page = index;
	else
		index_page = 0;
}

void frmImportarJuego::on_txtInfo_anchorClicked(const QUrl &arg1)
{
	fGrl->abrirArchivo(arg1.toString());
}

void frmImportarJuego::on_txtInfoJuego_anchorClicked(const QUrl &arg1)
{
	fGrl->abrirArchivo(arg1.toString());
}

void frmImportarJuego::on_btnAbrir_clicked()
{
	defectoLimpiar();

	QString filter = tr("Soportados");
	if (type_import == t_File)
	{
		filter.append(" (*.xml *.prof)");
		filter.append(";;DB GR-lida (*.xml)");
		filter.append(";;D-Fend Reloaded (*.prof)");
	}
	else if (type_import == t_DBGL)
	{
		filter.append(" (*.dbgl.zip *.xml)");
		filter.append(";;DOSBox Game Launcher (DBGL) (*.dbgl.zip *.xml)");
	} else
		filter.append(" (*)");

	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo para importarlo"), grlCfg.DirImportar, "", filter);
	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			bool siguiente = false;
			grlCfg.DirImportar = fGrl->setDirRelative(f_info.Path);

			ui->swImport->setCurrentIndex(0);
			ui->btnToggleInfo->setEnabled(false);
			ui->btnToggleInfo->setText(ui->swImport->currentIndex() > 0 ? tr("Anterior") : tr("Siguiente"));
			ui->btnToggleInfo->setIcon(QIcon(fGrl->theme() +"img16/"+ (ui->swImport->currentIndex() > 0 ? "go-back.png" : "go-next.png")));

			isZip = false;
			if (z_is_open)
				z_file->closeArchive();
			z_is_open = false;

			if (type_import == t_DBGL)
			{
				if (f_info.Ext == ".xml")
					siguiente = leerDbglXml(archivo);
				else if (f_info.Ext == ".zip")
				{
					if (leerDbglZip(archivo))
						siguiente = leerDbglXml(grlDir.Temp +"profiles.xml");
				} else
					siguiente = false;
			} else {
				if (f_info.Ext == ".xml")
				{
					cargarScript(grlDir.Scripts +"gr-lida.js");
					analyzePage(archivo, true);
					siguiente = true;
				}
				else if (f_info.Ext == ".prof")
				{
					analyzePage(archivo, true, true); // Indicamos que es tipo DFend
					siguiente = true;
				} else
					siguiente = false;
			}

			ui->btnToggleInfo->setEnabled(siguiente);
			ui->btnOk->setEnabled(siguiente);
		}
	}
}

void frmImportarJuego::on_btnAbrirUrl_clicked()
{
// Abre la URL con el navegador por defecto
	if (!str_url_web.isEmpty())
		fGrl->abrirArchivo(str_url_web);
}

void frmImportarJuego::on_btnBuscar_clicked()
{
	if (!ui->txtTituloBuscar->text().isEmpty() && ui->cbxScriptURL->count() > -1)
	{
		QHash<QString, QString> url_filed;

		defectoLimpiar();

		ui->btnOk->setEnabled(true);

		QString page = "";
		if (ui->cbxNumPag->count() > 0)
		{
			if (index_page > ui->cbxNumPag->count())
				index_page = 0;
			page = ui->cbxNumPag->itemData(index_page).toString();
		}

		args.clear();
		ctor = engine->evaluate("UrlWebBusqueda");
		args << QScriptValue(engine, ui->txtTituloBuscar->text());
		args << QScriptValue(engine, page);

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		url_filed.clear();
		while (it.hasNext())
		{
			it.next();
			url_filed.insert(it.name(), it.value().toString());
		}

		index_fin_descarga = AnalizarPaginaBusqueda;
		httpdown->downloadFile(url_filed["url"], grlDir.Temp +"busqueda.html", url_filed["metodo"], url_filed["c_post"]);
	}
}

void frmImportarJuego::on_btnAbortar_clicked()
{
	emit httpdown->cancelDownload();
}

void frmImportarJuego::on_twListaJuegos_itemClicked(QTreeWidgetItem *item, int column)
{
	if (!isItemDoubleClicked)
	{
		if (item && column > -1)
		{
			if (!isItemChanged)
				mostrarFichaHtml(item);
			isItemChanged = false;
		}
	}
	isItemDoubleClicked = false;
}

void frmImportarJuego::on_twListaJuegos_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		emit on_btnDownInfo_clicked();
		isItemDoubleClicked = true;
	}
}

void frmImportarJuego::on_twListaJuegos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == NULL)
		return;

	if (current != previous)
	{
		isItemChanged = true;
		mostrarFichaHtml(current);
	}
}

void frmImportarJuego::on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
		fGrl->abrirArchivo(item->text(0));
}

void frmImportarJuego::on_btnEliminarImagen_clicked()
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
	// Eliminamos la imagen de la lista twDatosImages
		QTreeWidgetItem *item_img = ui->twDatosImages->currentItem();
		int pos = ui->twDatosImages->indexOfTopLevelItem(item_img);
		if (ui->twDatosImages->topLevelItemCount() > 0 && pos != -1)
		{
			int id = fGrl->strToInt(item_img->text(col_img_id));
			list_import_temp[item].imagenes.removeAt(id);
			fGrl->deleteItemTree(item_img);
		}
	}
}

void frmImportarJuego::on_btnAddImagen_clicked()
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		const int count = ui->twDatosTempImages->topLevelItemCount();
		if (count > 0)
		{
			int total_img = list_import_temp[item].imagenes.size();
			QList<QTreeWidgetItem *> items_checked;
			for (int i = 0; i < count; ++i)
			{
				QTreeWidgetItem *item_chk = ui->twDatosTempImages->topLevelItem(i);
				if (item_chk->checkState(col_img_thumb) == Qt::Checked)
				{
					QTreeWidgetItem *item_new = new QTreeWidgetItem;
						item_new->setIcon(col_img_thumb    , item_chk->icon(col_img_thumb)    );
						item_new->setText(col_img_nombre   , item_chk->text(col_img_nombre)   );
						item_new->setText(col_img_tipo     , item_chk->text(col_img_tipo)     );
						item_new->setText(col_img_url_full , item_chk->text(col_img_url_full) );
						item_new->setText(col_img_url_small, item_chk->text(col_img_url_small));
						item_new->setText(col_img_id       , fGrl->intToStr(total_img)        );
					items_checked << item_new;

					stDatosImagenes imagen;
						imagen.nombre    = item_chk->text(col_img_nombre);
						imagen.type      = item_chk->text(col_img_tipo);
						imagen.url_full  = item_chk->text(col_img_url_full);
						imagen.url_small = item_chk->text(col_img_url_small);
					list_import_temp[item].imagenes << imagen;

					total_img++;
				}
			}
			ui->twDatosImages->addTopLevelItems(items_checked);
		}
	}
}

void frmImportarJuego::on_chkSelectDatosTempImages_toggled(bool checked)
{
	fGrl->checkStateAllItemTw(ui->twDatosTempImages, col_img_thumb, checked);
}

void frmImportarJuego::on_btnDownAllCovers_clicked()
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		index_fin_descarga = AnalizarPaginaMoreCovers;
		httpdown->downloadFile(list_import_temp[item].url_all_covers, grlDir.Temp +"more_covers.html");
	}
}

void frmImportarJuego::on_chkSelectListaJuegos_toggled(bool checked)
{
	fGrl->checkStateAllItemTw(ui->twListaJuegos, icol_titulo, checked);
}

void frmImportarJuego::on_btnDownInfo_clicked()
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		fGrl->comprobarDirectorio(grlDir.Temp +"imagenes/small/");

		if (type_import == t_DBGL)
			mostrarFichaHtml(item);

		if (type_import == t_Script)
		{
			index_fin_descarga = AnalizarPaginaFicha;
			httpdown->downloadFile(item->text(icol_url), grlDir.Temp +"ficha.html");
		}
	}
}

void frmImportarJuego::on_btnToggleInfo_clicked()
{
	if (ui->swImport->currentIndex() == 0)
	{
		ui->swImport->setCurrentIndex(1);
		ui->btnToggleInfo->setText(tr("Anterior"));
		ui->btnToggleInfo->setIcon(QIcon(fGrl->theme() +"img16/go-back.png"));

	} else {
		ui->swImport->setCurrentIndex(0);
		ui->btnToggleInfo->setText(tr("Siguiente"));
		ui->btnToggleInfo->setIcon(QIcon(fGrl->theme() +"img16/go-next.png"));
	}
}

void frmImportarJuego::on_chkSoloDatos_toggled(bool checked)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].solo_datos = checked;
	}
}

void frmImportarJuego::on_chkSoloCfgDOSBox_toggled(bool checked)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].solo_cfg_dosbox = checked;
	}
}

void frmImportarJuego::on_chkImportarCapturas_toggled(bool checked)
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].import_capturas = checked;
	}
}

void frmImportarJuego::on_chkSoloDatos_all_toggled(bool checked)
{
	const int count = ui->twListaJuegos->topLevelItemCount();
	for (int i = 0; i < count; ++i)
	{
		list_import_temp[ui->twListaJuegos->topLevelItem(i)].solo_datos = checked;
	}
}

void frmImportarJuego::on_chkSoloCfgDOSBox_all_toggled(bool checked)
{
	const int count = ui->twListaJuegos->topLevelItemCount();
	for (int i = 0; i < count; ++i)
	{
		list_import_temp[ui->twListaJuegos->topLevelItem(i)].solo_cfg_dosbox = checked;
	}
}

void frmImportarJuego::on_chkImportarCapturas_all_toggled(bool checked)
{
	const int count = ui->twListaJuegos->topLevelItemCount();
	for (int i = 0; i < count; ++i)
	{
		list_import_temp[ui->twListaJuegos->topLevelItem(i)].import_capturas = checked;
	}
}

// INI Importar btnOK y btnCancelar -----------------------------------------------------------------------------
void frmImportarJuego::on_btnOk_clicked()
{
	if (ui->twListaJuegos->topLevelItemCount() > 0)
	{
		emit statusWidgetEnabled(false);
		QList<stDownItem> listDown;
		bool siguiente = false;

		z_dir_out = fGrl->getDirRelative(grlCfg.DirBaseGames, "DosGames");
		z_list.clear();
		if (isZip && z_is_open)
			z_list = z_file->entryList();
		const int z_listSize = z_list.size();

		list_import.clear();

		if (importMain)
		{
			const int count = ui->twListaJuegos->topLevelItemCount();

			httpdown->defaultInfo();
			httpdown->cfgProgressBar(0, count);
			httpdown->setProgressBarValue(1, tr("Revisando") +": 1 "+ tr("de") +" "+ QString::number(count));

			for (int n = 0; n < count; ++n)
			{
				httpdown->setProgressBarValue(n+1, tr("Revisando") +": "+ QString::number(n+1) +" "+ tr("de") +" "+ QString::number(count));

				QTreeWidgetItem *item = ui->twListaJuegos->topLevelItem(n);
				if (item->checkState(icol_titulo) == Qt::Checked)
				{
					createListImagesDownload(item, listDown);
					createListZipFiles(item, z_listSize);

					list_import << list_import_temp[item];
				}
			}
			siguiente = true;
		} else {
			QTreeWidgetItem *current = ui->twListaJuegos->currentItem();

			if (current != NULL)
			{
				httpdown->defaultInfo();
				httpdown->cfgProgressBar(0, 1);
				httpdown->setProgressBarValue(1, tr("Revisando") +": 1 "+ tr("de") +" 1");

				createListImagesDownload(current, listDown);
				createListZipFiles(current, z_listSize);

				list_import << list_import_temp[current];

				siguiente = true;
			} else {
				QMessageBox::information(this, tr("Importar juego"), tr("Por favor selecciona al menos un juego de la lista para importarlo"));
				emit statusWidgetEnabled(true);
			}
		}

		if (siguiente)
		{
			index_fin_descarga = ImportarJuegos;
			if (type_import == t_Script)
			{
			// Descargamos primero todas las imagenes.
				httpdown->defaultInfo();
				httpdown->downloadMultiFiles(listDown);
			} else
				emit statusFinished();
		}
	}
}

void frmImportarJuego::on_btnCancelar_clicked()
{
	QDialog::reject();
}
// FIN Importar btnOK y btnCancelar -----------------------------------------------------------------------------

// INI HttpDownload ---------------------------------------------------------------------------------------------
void frmImportarJuego::statusBtnEnabled(bool estado)
{
	ui->wTop->setEnabled(estado);

	ui->txtTituloBuscar->setEnabled(estado);
	ui->cbxNumPag->setEnabled(estado);
	ui->btnBuscar->setEnabled(estado);
	ui->btnAbortar->setEnabled(!estado);

	ui->twListaJuegos->setEnabled(estado);
	ui->chkSelectListaJuegos->setEnabled(estado);
	ui->tabwImport->setEnabled(estado);

	ui->wBottom->setEnabled(estado);
}

void frmImportarJuego::statusWidgetEnabled(bool estado)
{
	ui->wTop->setEnabled(estado);
	ui->swImport->setEnabled(estado);
	ui->wBottom->setEnabled(estado);

	lb_msgInfo->setText(tr("Importando") +"...");

	if (estado)
		msgInfo->hide();
	else
		msgInfo->show();
}

void frmImportarJuego::statusFinished()
{
	switch (index_fin_descarga)
	{
		case NohacerNada:
		// No Hacer Nada
		break;
		case MostrarFicha:
			isItemDoubleClicked = false;
			mostrarFichaHtml(ui->twListaJuegos->currentItem());
		break;
		case AnalizarPaginaBusqueda:
			addTitles(grlDir.Temp +"busqueda.html");
		break;
		case AnalizarPaginaFicha:
			analyzePage(grlDir.Temp +"ficha.html");
		break;
		case AnalizarPaginaCovers:
			analyzePageCovers(grlDir.Temp +"covers.html");
		break;
		case AnalizarPaginaMoreCovers:
			analyzePageMoreCovers(grlDir.Temp +"more_covers.html");
		break;
		case CargarThumb:
			index_fin_descarga = MostrarFicha;
			httpdown->downloadFile(datosImportar["Dat_thumbs_url"], grlDir.Temp +"imagenes/small/"+ datosImportar["Dat_thumbs"]);
		break;
		case CargarCovers:
			index_fin_descarga = AnalizarPaginaCovers;
			httpdown->downloadFile(datosImportar["Dat_url_all_covers"], grlDir.Temp +"covers.html");
		break;
		case CargarTwTempImages:
			setScriptImagenes(ui->twDatosTempImages, list_import_temp[ui->twListaJuegos->currentItem()].imagenes_tmp, true);
		break;
		case ImportarJuegos:
			importarJuegos();
		break;
	}
}

void frmImportarJuego::addTitles(QString filename)
{
	ui->cbxNumPag->clear();

	args.clear();
	ctor = engine->evaluate("AnalyzeFindPage");
	args << QScriptValue(engine, leerArchivoHTML(filename));
	ctor.call(QScriptValue(), args);

	list_import_temp = twProto->getListImport();

	int count_cbx = ui->cbxNumPag->count();
	if (index_page >= count_cbx)
		index_page = 0;
	ui->cbxNumPag->setCurrentIndex(index_page);

	total_num_pag = fGrl->intToStr(count_cbx);
	num_pag = fGrl->intToStr(ui->cbxNumPag->currentIndex() + 1);
	ui->lb_num_paginas->setText(tr("Página") +" "+ num_pag +" "+ tr("de") +" "+ total_num_pag);
}

void frmImportarJuego::analyzePage(QString filename, bool local, bool tipoDFend)
{
	datosImportar.clear();
	datosImportar["tipoDFend"] = fGrl->boolToStr(tipoDFend);
	datosImportar["isLocal"]   = fGrl->boolToStr(local);
	datosImportar["filename"]  = fGrl->getInfoFile(filename).Path;

	if (local)
	{
		datosImportar["Dat_thumbs_url"]      = datosImportar["filename"] + datosImportar["Dat_thumbs"];
		datosImportar["Dat_cover_front_url"] = datosImportar["filename"] + datosImportar["Dat_cover_front"];
		datosImportar["Dat_cover_back_url"]  = datosImportar["filename"] + datosImportar["Dat_cover_back"];
	}

	if (tipoDFend)
		datosImportar = fGrl->importarProfileDFend(grlDir.Home, filename);
	else {
		args.clear();
		ctor = engine->evaluate("AnalyzeGamePage");
		args << QScriptValue(engine, leerArchivoHTML(filename));
		args << QScriptValue(engine, local);

		QScriptValue m_array = ctor.call(QScriptValue(), args);
		QScriptValueIterator it(m_array);

		while (it.hasNext())
		{
			it.next();
			datosImportar.insert(it.name(), ""+ it.value().toString());
		}
	}

	setScriptDatos();

	ui->btnDownAllCovers->setEnabled(!datosImportar["Dat_url_all_covers"].isEmpty());

	if (fGrl->strToBool(datosImportar["Dat_covers_other_page"]))
		index_fin_descarga = CargarCovers;
	else
		index_fin_descarga = CargarThumb;
	emit statusFinished();
}

void frmImportarJuego::analyzePageCovers(QString filename)
{
	QHash<QString, QString> tmpDatosImportar;

	args.clear();
	ctor = engine->evaluate("AnalyzeGameCoverPage");
	args << QScriptValue(engine, leerArchivoHTML(filename));

	QScriptValue m_array = ctor.call(QScriptValue(), args);
	QScriptValueIterator it(m_array);

	while (it.hasNext())
	{
		it.next();
		tmpDatosImportar.insert(it.name(), ""+ it.value().toString());
	}

	index_fin_descarga = MostrarFicha;

	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		if (!list_import_temp[item].datos.thumbs_url.isEmpty())
		{
			list_import_temp[item].datos.thumbs         = tmpDatosImportar["Dat_thumbs"];
			list_import_temp[item].datos.thumbs_url     = tmpDatosImportar["Dat_thumbs_url"];
			list_import_temp[item].datos_def.thumbs     = list_import_temp[item].datos.thumbs;
			list_import_temp[item].datos_def.thumbs_url = list_import_temp[item].datos.thumbs_url;
		}

		if (!list_import_temp[item].datos.cover_front_url_small.isEmpty())
		{
			list_import_temp[item].datos.cover_front               = tmpDatosImportar["Dat_cover_front"];
			list_import_temp[item].datos.cover_front_url           = tmpDatosImportar["Dat_cover_front_url"];
			list_import_temp[item].datos.cover_front_url_small     = tmpDatosImportar["Dat_cover_front_url_small"];
			list_import_temp[item].datos_def.cover_front           = list_import_temp[item].datos.cover_front;
			list_import_temp[item].datos_def.cover_front_url       = list_import_temp[item].datos.cover_front_url;
			list_import_temp[item].datos_def.cover_front_url_small = list_import_temp[item].datos.cover_front_url_small;
		}

		if (datosImportar["Dat_cover_back"].isEmpty() && !tmpDatosImportar["Dat_cover_back"].isEmpty())
		{
			list_import_temp[item].datos.cover_back               = tmpDatosImportar["Dat_cover_back"];
			list_import_temp[item].datos.cover_back_url           = tmpDatosImportar["Dat_cover_back_url"];
			list_import_temp[item].datos.cover_back_url_small     = tmpDatosImportar["Dat_cover_back_url_small"];
			list_import_temp[item].datos_def.cover_back           = list_import_temp[item].datos.cover_back;
			list_import_temp[item].datos_def.cover_back_url       = list_import_temp[item].datos.cover_back_url;
			list_import_temp[item].datos_def.cover_back_url_small = list_import_temp[item].datos.cover_back_url_small;
		}

		if (datosImportar["Dat_cover_left"].isEmpty() && !tmpDatosImportar["Dat_cover_left"].isEmpty())
		{
			list_import_temp[item].datos.cover_left               = tmpDatosImportar["Dat_cover_left"];
			list_import_temp[item].datos.cover_left_url           = tmpDatosImportar["Dat_cover_left_url"];
			list_import_temp[item].datos.cover_left_url_small     = tmpDatosImportar["Dat_cover_left_url_small"];
			list_import_temp[item].datos_def.cover_left           = list_import_temp[item].datos.cover_left;
			list_import_temp[item].datos_def.cover_left_url       = list_import_temp[item].datos.cover_left_url;
			list_import_temp[item].datos_def.cover_left_url_small = list_import_temp[item].datos.cover_left_url_small;
		}

		if (datosImportar["Dat_cover_right"].isEmpty() && !tmpDatosImportar["Dat_cover_right"].isEmpty())
		{
			list_import_temp[item].datos.cover_right               = tmpDatosImportar["Dat_cover_right"];
			list_import_temp[item].datos.cover_right_url           = tmpDatosImportar["Dat_cover_right_url"];
			list_import_temp[item].datos.cover_right_url_small     = tmpDatosImportar["Dat_cover_right_url_small"];
			list_import_temp[item].datos_def.cover_right           = list_import_temp[item].datos.cover_right;
			list_import_temp[item].datos_def.cover_right_url       = list_import_temp[item].datos.cover_right_url;
			list_import_temp[item].datos_def.cover_right_url_small = list_import_temp[item].datos.cover_right_url_small;
		}

		if (datosImportar["Dat_cover_top"].isEmpty() && !tmpDatosImportar["Dat_cover_top"].isEmpty())
		{
			list_import_temp[item].datos.cover_top               = tmpDatosImportar["Dat_cover_top"];
			list_import_temp[item].datos.cover_top_url           = tmpDatosImportar["Dat_cover_top_url"];
			list_import_temp[item].datos.cover_top_url_small     = tmpDatosImportar["Dat_cover_top_url_small"];
			list_import_temp[item].datos_def.cover_top           = list_import_temp[item].datos.cover_top;
			list_import_temp[item].datos_def.cover_top_url       = list_import_temp[item].datos.cover_top_url;
			list_import_temp[item].datos_def.cover_top_url_small = list_import_temp[item].datos.cover_top_url_small;
		}

		if (datosImportar["Dat_cover_bottom"].isEmpty() && !tmpDatosImportar["Dat_cover_bottom"].isEmpty())
		{
			list_import_temp[item].datos.cover_bottom               = tmpDatosImportar["Dat_cover_bottom"];
			list_import_temp[item].datos.cover_bottom_url           = tmpDatosImportar["Dat_cover_bottom_url"];
			list_import_temp[item].datos.cover_bottom_url_small     = tmpDatosImportar["Dat_cover_bottom_url_small"];
			list_import_temp[item].datos_def.cover_bottom           = list_import_temp[item].datos.cover_bottom;
			list_import_temp[item].datos_def.cover_bottom_url       = list_import_temp[item].datos.cover_bottom_url;
			list_import_temp[item].datos_def.cover_bottom_url_small = list_import_temp[item].datos.cover_bottom_url_small;
		}

		list_import_temp[item].imagenes.clear();
		datosImportar["Dat_all_covers"] = tmpDatosImportar["Dat_all_covers"];

		analyzeImportImagenes(datosImportar["Dat_all_covers"], list_import_temp[item].imagenes);
	} else
		emit statusFinished();
}

void frmImportarJuego::analyzePageMoreCovers(QString filename)
{
	QHash<QString, QString> tmpDatosImportar;

	args.clear();
	ctor = engine->evaluate("AnalyzeGameMoreCoverPage");
	args << QScriptValue(engine, leerArchivoHTML(filename));

	QScriptValue m_array = ctor.call(QScriptValue(), args);
	QScriptValueIterator it(m_array);

	while (it.hasNext())
	{
		it.next();
		tmpDatosImportar.insert(it.name(), ""+ it.value().toString());
	}

	index_fin_descarga = CargarTwTempImages;

	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		list_import_temp[item].imagenes_tmp.clear();
		datosImportar["Dat_all_covers"] = tmpDatosImportar["Dat_all_covers"];

		analyzeImportImagenes(datosImportar["Dat_all_covers"], list_import_temp[item].imagenes_tmp);
	} else
		emit statusFinished();
}

void frmImportarJuego::analyzeImportImagenes(QString all_covers, QList<stDatosImagenes> &imagenes)
{
	if (!all_covers.isEmpty())
	{
		QList<stDownItem> listDown;
		stDownItem itemdown;
		itemdown.tipo        = "GET";
		itemdown.contentPost = "";

		QString linea = "";
		QStringList linea_cover;
		QTextStream in(&all_covers, QIODevice::ReadOnly | QIODevice::Text);
		in.setCodec("UTF-8");
		while (!in.atEnd())
		{
			linea = in.readLine();
			if (!linea.isEmpty())
			{
			// 0 = name, 1 = url full, 2 = type, 3 = url small
				linea_cover = linea.split("|");
				if (linea_cover.size() == 4)
				{
					stDatosImagenes imagen;
					imagen.type      = linea_cover.at(2);
					imagen.nombre    = linea_cover.at(0);
					imagen.url_full  = linea_cover.at(1);
					imagen.url_small = linea_cover.at(3);

					itemdown.urlfile  = imagen.url_small;
					itemdown.fileName = grlDir.Temp +"imagenes/small/"+ imagen.nombre;
					listDown << itemdown;

					imagenes << imagen;
				}
			}
		}

		if (listDown.size() > 0)
			httpdown->downloadMultiFiles(listDown);
		else
			emit statusFinished();
	} else
		emit statusFinished();
}

void frmImportarJuego::setScriptDatos()
{
	QTreeWidgetItem *item = ui->twListaJuegos->currentItem();
	if (isSelectedItem(item))
	{
		stImport import;

		import.solo_datos      = false;
		import.solo_cfg_dosbox = false;
		import.import_capturas = true;
		import.is_importado    = false;
		import.url_all_covers  = datosImportar["Dat_url_all_covers"];

	// Datos Juego --------------------------------------------------------------
		stDatosJuego datos = sql->getDefectDatos(datosImportar["Dat_icono"], datosImportar);
		datos.idgrl = DatosJuego.idgrl;

		if (!icon_list.isEmpty())
		{
			int index = icon_title_list.indexOf(datos.icono);
			index = index > -1 ? index : 0;
			datos.icono = icon_list.at(index).fileName();
		} else
			datos.icono = "datos.png";

		import.datos     = datos;
		import.datos_def = datos;

	// import.files
	// import.urls
	// --------------------------------------------------------------------------

	// Datos DOSBox -------------------------------------------------------------
		if (datos.tipo_emu == "dosbox")
		{
			import.dosbox = sql->getDefectDOSBox(datosImportar);

			QList<stConfigDOSBoxMount> list_mount;
			const int num_mount = datosImportar["Dbx_mount_count"].toInt();
			for (int i_dbx = 0; i_dbx < num_mount; i_dbx++)
			{
				QString linea = datosImportar["Dbx_mount_"+ fGrl->intToStr(i_dbx)];
				QStringList linea_mount = linea.split("@|");
				if (linea_mount.size() == 11)
				{
					stConfigDOSBoxMount mount;
						mount.path         = fGrl->setDirRelative(linea_mount.at(0), "DosGames");
						mount.label        = linea_mount.at(1);
						mount.tipo_as      = linea_mount.at(2);
						mount.letter       = linea_mount.at(3);
						mount.indx_cd      = linea_mount.at(4);
						mount.opt_mount    = linea_mount.at(5);
						mount.io_ctrl      = linea_mount.at(6);
						mount.select_mount = linea_mount.at(7);
						mount.id_lista     = fGrl->intToStr(i_dbx);
						mount.id_dosbox    = "";
						mount.opt_size     = linea_mount.at(8);
						mount.opt_freesize = linea_mount.at(9);
						mount.freesize     = linea_mount.at(10);
					list_mount << mount;
				}
			}
			wDbx->mountAdd(ui->twMontajes, list_mount);
			import.dbxMount = list_mount;
		} else {
			import.dosbox = sql->getDefectDOSBox();
			import.dbxMount.clear();
		}
	// --------------------------------------------------------------------------

	// Datos ScummVM ------------------------------------------------------------
		if (datos.tipo_emu == "scummvm")
			import.scummvm = sql->getDefectScummVM(datosImportar);
		else
			import.scummvm = sql->getDefectScummVM();
	// --------------------------------------------------------------------------

	// Datos VDMSound -----------------------------------------------------------
		if (datos.tipo_emu == "vdmsound")
			import.vdmsound = sql->getDefectVDMSound(datosImportar);
		else
			import.vdmsound = sql->getDefectVDMSound();
	// --------------------------------------------------------------------------

		list_import_temp[item] = import;
	}
}

void frmImportarJuego::setScriptImagenes(QTreeWidget *twImagen, QList<stDatosImagenes> imagenes, bool isCheck)
{
	twImagen->clear();
	const int num_img = imagenes.size();
	if (num_img > 0)
	{
		QList<QTreeWidgetItem *> items_cover;
		for (int i = 0; i < num_img; ++i)
		{
			QTreeWidgetItem *item_cover = new QTreeWidgetItem;
				item_cover->setTextAlignment(0, Qt::AlignCenter);
				if (isCheck)
					item_cover->setCheckState(icol_titulo, Qt::Unchecked);
				QString icon = grlDir.Temp +"imagenes/small/"+ imagenes.at(i).nombre;
				if (!QFile::exists(icon))
					icon = fGrl->theme() +"images/juego_sin_imagen.png";
				item_cover->setIcon(0, QIcon(icon)             );
				item_cover->setText(1, imagenes.at(i).nombre   );
				item_cover->setText(2, imagenes.at(i).type     );
				item_cover->setText(3, imagenes.at(i).url_full );
				item_cover->setText(4, imagenes.at(i).url_small);
				item_cover->setText(5, fGrl->intToStr(i)       );
			items_cover << item_cover;
		}
		twImagen->addTopLevelItems(items_cover);
		twImagen->header()->resizeSections(QHeaderView::ResizeToContents);
	}
}
// FIN HttpDownload ---------------------------------------------------------------------------------------------

// INI DBGL -----------------------------------------------------------------------------------------------------
bool frmImportarJuego::leerDbglZip(QString filename)
{
	bool isOk = false;

	if (is_load_7zlib)
	{
		isZip = true;
		z_is_open = z_file->open(filename);

		if (z_is_open)
			isOk = z_file->extract("profiles.xml", grlDir.Temp, "profiles.xml");
	} else {
		isZip = false;
		z_is_open = false;
	}

	return isOk;
}

bool frmImportarJuego::leerDbglXml(QString filename)
{
	bool isOk = false;

	items_dbgl.clear();

	const QString data = fGrl->leerArchivo(filename, "UTF-8");
	xml.addData(data);
	if (xml.readNextStartElement())
	{
		isOk = true;
		if (xml.name() == "document")
		{
			while (xml.readNextStartElement())
			{
				if (xml.name() == "export")
					leerDbglExport();
				else if (xml.name() == "profile")
					leerDbglProfile();
				else
					xml.skipCurrentElement();
			}
		} else {
			isOk = false;
			xml.raiseError(tr("The file is not an XBEL version 1.0 file."));
		}
	}
	xml.clear();

	ui->twListaJuegos->addTopLevelItems(items_dbgl);

	const int listSize = items_dbgl.size();
	httpdown->defaultInfo();
	httpdown->cfgProgressBar(0, listSize);
	httpdown->setProgressBarValue(1, tr("Añadiendo a la lista") +": 1 "+ tr("de") +" "+ QString::number(listSize));

	for (int i = 0; i < listSize; ++i)
	{
		QTreeWidgetItem *item = ui->twListaJuegos->topLevelItem(i);

		ComboBoxTwItem *cmb = new ComboBoxTwItem(item, icol_cbx_dosbox);
		foreach (const stGrlDatos &dbx, dbx_list)
			cmb->addItem(QIcon(fGrl->theme() +"img16/"+ dbx.icono), dbx.titulo, dbx.key);
		connect(cmb, SIGNAL(itemSelect(QTreeWidgetItem*)), this, SLOT(onCbxDbx_activated(QTreeWidgetItem*)));

		ui->twListaJuegos->setItemWidget(item, icol_cbx_dosbox, cmb);

		int index = cmb->findText(list_import_temp[item].dbgl.dosbox_title, Qt::MatchStartsWith);
		index = (index > -1) ? index : 0;
		cmb->setCurrentIndex(index);

		QString emu_key = cmb->itemData(index).toString();
		item->setText(icol_cbx_dosbox, emu_key);
		list_import_temp[item].dosbox.dosbox_emu_key = emu_key;

		httpdown->setProgressBarValue(i+1, tr("Añadiendo a la lista") +": "+ QString::number(i+1) +" "+ tr("de") +" "+ QString::number(listSize));
	}

	return isOk;
}

QString frmImportarJuego::leerDbglSubItem(QString key, bool isDir)
{
	QString text = "";
	xml.readNextStartElement();
	if (xml.name() == key)
		text = correctSlash(""+ xml.readElementText(), isDir);
	xml.skipCurrentElement();
	return text;
}

QHash<QString, QString> frmImportarJuego::leerDbglSubItemDouble(QString key1, QString key2)
{
	QHash<QString, QString> value;
	while (xml.readNextStartElement())
	{
		if (xml.name() == key1)
			value[key1] =  ""+ xml.readElementText();
		else if (xml.name() == key2)
			value[key2] =  ""+ xml.readElementText();
		else
			xml.skipCurrentElement();
	}
	return value;
}

void frmImportarJuego::leerDbglExport()
{
	info_dbgl_export.clear();
	while (xml.readNextStartElement())
	{
		if (xml.name() == "format-version")
			info_dbgl_export["format-version"] = ""+ xml.readElementText();
		else if (xml.name() == "title")
			info_dbgl_export["title"] = ""+ xml.readElementText();
		else if (xml.name() == "author")
			info_dbgl_export["author"] = ""+ xml.readElementText();
		else if (xml.name() == "notes")
			info_dbgl_export["notes"] = ""+ xml.readElementText().replace("\n", "<br/>");
		else if (xml.name() == "creationdatetime")
			info_dbgl_export["creationdatetime"] = ""+ xml.readElementText();
		else if (xml.name() == "generator-title")
			info_dbgl_export["generator-title"] = ""+ xml.readElementText();
		else if (xml.name() == "generator-version")
			info_dbgl_export["generator-version"] = ""+ xml.readElementText();
		else if (xml.name() == "captures-available")
			info_dbgl_export["captures-available"] = ""+ xml.readElementText();
		else if (xml.name() == "keymapperfiles-available")
			info_dbgl_export["keymapperfiles-available"] = ""+ xml.readElementText();
		else if (xml.name() == "nativecommands-available")
			info_dbgl_export["nativecommands-available"] = ""+ xml.readElementText();
		else if (xml.name() == "gamedata-available")
			info_dbgl_export["gamedata-available"] = ""+ xml.readElementText();
		else if (xml.name() == "custom1")
			info_dbgl_export["custom1"] = ""+ xml.readElementText();
		else if (xml.name() == "custom2")
			info_dbgl_export["custom2"] = ""+ xml.readElementText();
		else if (xml.name() == "custom3")
			info_dbgl_export["custom3"] = ""+ xml.readElementText();
		else if (xml.name() == "custom4")
			info_dbgl_export["custom4"] = ""+ xml.readElementText();
		else if (xml.name() == "custom5")
			info_dbgl_export["custom5"] = ""+ xml.readElementText();
		else if (xml.name() == "custom6")
			info_dbgl_export["custom6"] = ""+ xml.readElementText();
		else if (xml.name() == "custom7")
			info_dbgl_export["custom7"] = ""+ xml.readElementText();
		else if (xml.name() == "custom8")
			info_dbgl_export["custom8"] = ""+ xml.readElementText();
		else if (xml.name() == "custom9")
			info_dbgl_export["custom9"] = ""+ xml.readElementText();
		else if (xml.name() == "custom10")
			info_dbgl_export["custom10"] = ""+ xml.readElementText();
		else if (xml.name() == "custom11")
			info_dbgl_export["custom11"] = ""+ xml.readElementText();
		else if (xml.name() == "custom12")
			info_dbgl_export["custom12"] = ""+ xml.readElementText();
		else if (xml.name() == "custom13")
			info_dbgl_export["custom13"] = ""+ xml.readElementText();
		else if (xml.name() == "custom14")
			info_dbgl_export["custom14"] = ""+ xml.readElementText();
		else
			xml.skipCurrentElement();
	}

// Defecto
//	ui->fullGames->setEnabled(true);
//	ui->importCapturesButton->setEnabled(true);
//	ui->importMapperfilesButton->setEnabled(true);

//--
//	if (fGrl->strToBool(info_dbgl_export["gamedata-available"]))
//		ui->fullGames->setChecked(true);
//	else {
//		ui->fullGames->setEnabled(false);
//		ui->settingsOnly->setChecked(true);
//	}

//	ui->incrementalSettingsButton->setChecked(true);

//	if (fGrl->strToBool(info_dbgl_export["captures-available"]))
//		ui->importCapturesButton->setChecked(true);
//	else {
//		ui->importCapturesButton->setChecked(false);
//		ui->importCapturesButton->setEnabled(false);
//	}
//
//	if (fGrl->strToBool(info_dbgl_export["keymapperfiles-available"]))
//		ui->importMapperfilesButton->setChecked(true);
//	else {
//		ui->importMapperfilesButton->setChecked(false);
//		ui->importMapperfilesButton->setEnabled(false);
//	}
//
//	ui->importNativeCommandsButton->setEnabled(fGrl->strToBool(info_dbgl_export["nativecommands-available"]));

//--
	QString tpl_info_new = tpl_info_old;
	QString tpl_contenido = "format-version          : "+ info_dbgl_export["format-version"] +"<br/>"
						"title                   : "+ info_dbgl_export["title"] +"<br/>"
						"author                  : "+ info_dbgl_export["author"] +"<br/>"
						"notes                   : "+ info_dbgl_export["notes"] +"<br/>"
						"creationdatetime        : "+ info_dbgl_export["creationdatetime"] +"<br/>"
						"generator-title         : "+ info_dbgl_export["generator-title"] +"<br/>"
						"generator-version       : "+ info_dbgl_export["generator-version"] +"<br/><br/>"
						"gamedata-available      : "+ info_dbgl_export["gamedata-available"] +"<br/>"
						"captures-available      : "+ info_dbgl_export["captures-available"] +"<br/>"
						"keymapperfiles-available: "+ info_dbgl_export["keymapperfiles-available"] +"<br/>"
						"nativecommands-available: "+ info_dbgl_export["nativecommands-available"] +"<br/><br/>";

	for (int i = 1; i < 15; ++i)
		tpl_contenido += "custom"+ fGrl->intToStr(i) +" : "+ info_dbgl_export["custom"+ fGrl->intToStr(i)] +"<br/>";

	tpl_info_new.replace("{info_contenido}", "<pre>"+ tpl_contenido +"</pre>");
	ui->txtInfo->setHtml(tpl_info_new);
}

void frmImportarJuego::leerDbglProfile()
{
	stImport import;
	QTreeWidgetItem *item = new QTreeWidgetItem;

	if (importMain)
		item->setCheckState(icol_titulo, Qt::Unchecked);

	item->setIcon(icol_titulo, QIcon(fGrl->theme() +"img24/cat/dosbox.png"));
	item->setTextAlignment(icol_plataforma, Qt::AlignCenter);
	item->setText(icol_plataforma, "MS-Dos");
	item->setText(icol_tipo_emu, "dosbox");

	import.datos           = sql->getDefectDatos("datos");
	import.datos.icono     = "dosbox.png";
	import.datos.fecha     = sql->getTime();
	import.datos.tipo_emu  = "dosbox";
	import.dosbox          = sql->getDefectDOSBox();
	import.scummvm         = sql->getDefectScummVM();
	import.vdmsound        = sql->getDefectVDMSound();
	import.solo_datos      = false;
	import.solo_cfg_dosbox = false;
	import.import_capturas = info_dbgl_export["captures-available"].isEmpty() ? true : fGrl->strToBool(info_dbgl_export["captures-available"]);
	import.is_importado    = false;
	import.url_all_covers  = "";

	QString tmp = "";
	while (xml.readNextStartElement())
	{
		if (xml.name() == "title")
		{
			import.datos.titulo         = ""+ xml.readElementText();
			import.datos.titulo_guiones = sql->removeAccents(import.datos.titulo);
			item->setText(icol_titulo, import.datos.titulo);
		} else if (xml.name() == "id")
			import.dbgl.id_dbgl = ""+ xml.readElementText();
		else if (xml.name() == "captures")
			import.datos.path_capturas = leerDbglSubItem("raw", true);
		else if (xml.name() == "config-file")
			tmp = leerDbglSubItem("raw", true);
		else if (xml.name() == "game-dir")
			import.dbgl.game_dir = leerDbglSubItem("raw", true);
		else if (xml.name() == "setup")
			import.dosbox.path_setup = correctSlash(""+ xml.readElementText());
		else if (xml.name() == "altexe1")
			import.dbgl.altexe1 = ""+ xml.readElementText();
		else if (xml.name() == "altexe2")
			import.dbgl.altexe2 = ""+ xml.readElementText();
		else if (xml.name() == "setup-parameters")
			import.dosbox.parametros_setup = ""+ xml.readElementText();
		else if (xml.name() == "altexe1-parameters")
			import.dbgl.altexe1_param = ""+ xml.readElementText();
		else if (xml.name() == "altexe2-parameters")
			import.dbgl.altexe2_param = ""+ xml.readElementText();
		else if (xml.name() == "meta-info")
			leerDbglMetaInfo(item, import);
		else if (xml.name() == "full-configuration")
			leerDbglCfgDOSBox(import, ""+ xml.readElementText());
		else if (xml.name() == "incremental-configuration")
			tmp = ""+ xml.readElementText();
		else if (xml.name() == "dosbox")
		{
			QHash<QString, QString> value = leerDbglSubItemDouble("title", "version");
			import.dbgl.dosbox_title   = value["title"];
			import.dbgl.dosbox_version = value["version"];
		} else if (xml.name() == "native-commands")
			tmp = ""+ xml.readElementText();
//			<native-command>
//				<command/> // string
//				<parameters/> // string
//				<cwd/> // string
//				<waitfor/> // bool
//				<ordernr/> // int
//			</native-command>
		else
			xml.skipCurrentElement();
	}

	import.datos_def = import.datos;

	list_import_temp.insert(item, import);

	items_dbgl << item;
}

void frmImportarJuego::leerDbglMetaInfo(QTreeWidgetItem *item, stImport &import)
{
	QHash<QString, QString> value;
	while (xml.readNextStartElement())
	{
		if (xml.name() == "developer")
			import.datos.desarrollador = ""+ xml.readElementText();
		else if (xml.name() == "publisher")
			import.datos.compania = ""+ xml.readElementText();
		else if (xml.name() == "year")
		{
			import.datos.anno = ""+ xml.readElementText();
			item->setText(icol_anno, import.datos.anno);
		} else if (xml.name() == "genre")
			import.datos.genero = ""+ xml.readElementText();
		else if (xml.name() == "status")
			import.datos.estado = ""+ xml.readElementText();
		else if (xml.name() == "favorite")
			import.datos.favorito = ""+ xml.readElementText();
		else if (xml.name() == "gamepad")
			import.datos.gamepad = ""+ xml.readElementText();
		else if (xml.name() == "notes")
			import.datos.comentario = ""+ xml.readElementText().replace("\n", "<br/>");
		else if (xml.name() == "custom1")
			import.dbgl.custom1 = ""+ xml.readElementText();
		else if (xml.name() == "custom2")
			import.dbgl.custom2 = ""+ xml.readElementText();
		else if (xml.name() == "custom3")
			import.dbgl.custom3 = ""+ xml.readElementText();
		else if (xml.name() == "custom4")
			import.dbgl.custom4 = ""+ xml.readElementText();
		else if (xml.name() == "custom5")
			import.dbgl.custom5 = ""+ xml.readElementText();
		else if (xml.name() == "custom6")
			import.dbgl.custom6 = ""+ xml.readElementText();
		else if (xml.name() == "custom7")
			import.dbgl.custom7 = ""+ xml.readElementText();
		else if (xml.name() == "custom8")
			import.dbgl.custom8 = ""+ xml.readElementText();
		else if (xml.name() == "custom9")
			import.dbgl.custom9 = ""+ xml.readElementText();
		else if (xml.name() == "custom10")
			import.dbgl.custom10 = ""+ xml.readElementText();
		else if (xml.name() == "custom11")
			import.dbgl.custom11 = ""+ xml.readElementText();
		else if (xml.name() == "custom12")
			import.dbgl.custom12 = ""+ xml.readElementText();
		else if (xml.name() == "custom13")
			import.dbgl.custom13 = ""+ xml.readElementText();
		else if (xml.name() == "custom14")
			import.dbgl.custom14 = ""+ xml.readElementText();
		else if (xml.name() == "link1") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link2") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link3") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link4") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link5") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link6") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link7") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else if (xml.name() == "link8") {
			value = leerDbglSubItemDouble("raw", "title");
			leerDbglFilesURL(import, value["raw"], value["title"]);
		} else
			xml.skipCurrentElement();
	}
}

void frmImportarJuego::leerDbglCfgDOSBox(stImport &import, QString cfg)
{
// Importamos la configuración
	QHash<QString, QString> tmp_cfgDbx = fGrl->importarProfileDFend(grlDir.Home, cfg, true);
	tmp_cfgDbx["Dbx_id"]    = DatosDosBox.id;
	tmp_cfgDbx["Dbx_idgrl"] = DatosDosBox.idgrl;
	tmp_cfgDbx["Dbx_idcat"] = categoria.id;
	tmp_cfgDbx["Dbx_path_setup"]       = import.dosbox.path_setup;
	tmp_cfgDbx["Dbx_parametros_setup"] = import.dosbox.parametros_setup;

	import.dosbox = sql->getDefectDOSBox(tmp_cfgDbx);

	bool ini_autoexec = false;
	bool ini_exe      = false;
	QString linea     = "";
	QString dir_game  = "";
	stConfigDOSBoxMount montaje;

	QTextStream in(&cfg, QIODevice::ReadOnly | QIODevice::Text);
	in.setCodec("UTF-8");
	while (!in.atEnd())
	{
		linea = in.readLine();
		linea.replace("\\", "/");

		if (ini_autoexec && linea.startsWith("[", Qt::CaseInsensitive))
			ini_autoexec = false;
		if (linea.startsWith("[autoexec]", Qt::CaseInsensitive))
			ini_autoexec = true;
		if (ini_autoexec)
		{
			if (linea.startsWith("mount", Qt::CaseInsensitive))
			{
				montaje = getDbglMount(linea, "mount");
				import.dbxMount << montaje;
			}
			if (linea.startsWith("imgmount", Qt::CaseInsensitive))
			{
				montaje = getDbglMount(linea, "imgmount");
				import.dbxMount << montaje;
			}
			if (ini_exe)
			{
				QString exe_dir = fGrl->setDirRelative(grlCfg.DirBaseGames + import.dbgl.game_dir +"/"+ dir_game, "DosGames");
				if (!exe_dir.endsWith("/"))
					exe_dir.append("/");
				import.dosbox.path_exe = exe_dir + linea.remove("call ");
				ini_exe = false;
			}
			if (linea.startsWith("cd \\", Qt::CaseInsensitive) || linea.startsWith("cd /", Qt::CaseInsensitive))
			{
				ini_exe = true;
				dir_game = linea.remove("cd /");
			}
			if (linea.startsWith("exit", Qt::CaseInsensitive))
			{
				import.dosbox.opt_cerrar_dbox = "true";
			}
		}
	}
}

void frmImportarJuego::leerDbglFilesURL(stImport &import, QString raw, QString title)
{
	if (!raw.isEmpty())
	{
		szEntryInfo z_info;
		z_info.crc32 = "00000000";
		z_info.size  = 0;

		if (raw.toLower().startsWith("dosroot", Qt::CaseInsensitive))
		{
		// Files
			if (isZip && z_is_open)
			{
				raw.replace("\\","/").insert(8, import.dbgl.id_dbgl +"/");
				z_info = z_file->getEntryInfo(raw);
			}

			raw.replace("dosroot/"+ import.dbgl.id_dbgl +"/", grlCfg.DirBaseGames);
			stFileInfo f_info = fGrl->getInfoFile(fGrl->getDirRelative(raw, "DosGames"));

			stDatosFiles file;
				file.nombre      = title;
				file.crc         = z_info.crc32;
				file.descripcion = "";
				file.path        = fGrl->setDirRelative(f_info.FilePath, "DosGames");
				file.tipo        = f_info.Ext == ".pdf" ? "pdf" : "";
				file.id          = "";
				file.idgrl       = DatosJuego.idgrl;
				file.idcat       = categoria.id;
				file.size        = fGrl->intToStr(z_info.size);
			import.files << file;
		} else {
		// Urls
			stDatosUrls url;
				url.id    = "";
				url.idgrl = DatosJuego.idgrl;
				url.idcat = categoria.id;
				url.url   = raw;
				url.descripcion = title;
			import.urls << url;
		}
	}
}

stConfigDOSBoxMount frmImportarJuego::getDbglMount(QString linea, QString type)
{
	stConfigDOSBoxMount montaje;
	QStringList l_path;
	bool isQuote = false;
	bool isLetra = false;
	bool isType = false;
	QString str = "";

	montaje.path         = "";
	montaje.label        = "";
	montaje.tipo_as      = "drive";
	montaje.letter       = "C";
	montaje.indx_cd      = "";
	montaje.opt_mount    = "";
	montaje.io_ctrl      = "";
	montaje.select_mount = "x";
	montaje.id_lista     = "";
	montaje.id_dosbox    = "";
	montaje.opt_size     = "";
	montaje.opt_freesize = "false";
	montaje.freesize     = "250";

	linea.remove(0, type.length());
	int j = -1;
	while (j < linea.length())
	{
		++j;
		if (!isLetra && !linea.at(j).isSpace())
		{
			montaje.letter = linea.left(j+1).trimmed();
			isLetra = true;
			++j;
		}
		if (isQuote && linea.at(j) == '"')
		{
			isQuote = false;
			l_path.append(grlCfg.DirBaseGames + str.remove(0, str.indexOf("\"", 0) +1));
			str.clear();
			++j;
		}
		if (linea.at(j) == '"')
			isQuote = true;
		if (isQuote)
			str.append(linea.at(j));
		if (isType && (str.contains("dir") || str.contains("floppy") ||
						str.contains("cdrom") || str.contains("iso") ||
						str.contains("hdd")))
		{
			isType = false;
			montaje.tipo_as = str.remove(0, str.indexOf("\"", 0) +1).remove("-t").trimmed();
			str.clear();
		}
		if (linea.at(j) == '-' && linea.at(j+1) == 't')
			isType = true;
		if (isType)
			str.append(linea.at(j));
	}

	if (l_path.size() > 0)
	{
		montaje.path = l_path.join("|");
	// drive, cdrom, floppy, IMG_floppy, IMG_iso, IMG_multi_iso, IMG_hdd, boot
		if (montaje.tipo_as == "iso")
		{
			if (l_path.size() > 1)
				montaje.tipo_as = "IMG_multi_iso";
			else
				montaje.tipo_as = "IMG_iso";
		}
	} else {
		montaje.path = l_path.at(0);
	}

	if (montaje.letter.toLower() == "c")
		montaje.select_mount = "v";

	return montaje;

}
// FIN DBGL -----------------------------------------------------------------------------------------------------

// INI Importar -------------------------------------------------------------------------------------------------
void frmImportarJuego::setDefaultDatosIsChecked(stImport &import)
{
	if (!import.datos.isChecked.icono)
		import.datos.icono = import.datos_def.icono;
	if (!import.datos.isChecked.titulo)
	{
		import.datos.titulo = import.datos_def.titulo;
		import.datos.titulo_guiones = sql->removeAccents(import.datos.titulo);
	}
	if (!import.datos.isChecked.subtitulo)
		import.datos.subtitulo = import.datos_def.subtitulo;
	if (!import.datos.isChecked.genero)
		import.datos.genero = import.datos_def.genero;
	if (!import.datos.isChecked.compania)
		import.datos.compania = import.datos_def.compania;
	if (!import.datos.isChecked.desarrollador)
		import.datos.desarrollador = import.datos_def.desarrollador;
	if (!import.datos.isChecked.tema)
		import.datos.tema = import.datos_def.tema;
	if (!import.datos.isChecked.grupo)
		import.datos.grupo = import.datos_def.grupo;
	if (!import.datos.isChecked.perspectiva)
		import.datos.perspectiva = import.datos_def.perspectiva;
	if (!import.datos.isChecked.idioma)
		import.datos.idioma = import.datos_def.idioma;
	if (!import.datos.isChecked.idioma_voces)
		import.datos.idioma_voces = import.datos_def.idioma_voces;
	if (!import.datos.isChecked.formato)
		import.datos.formato = import.datos_def.formato;
	if (!import.datos.isChecked.anno)
		import.datos.anno = import.datos_def.anno;
	if (!import.datos.isChecked.numdisc)
		import.datos.numdisc = import.datos_def.numdisc;
	if (!import.datos.isChecked.sistemaop)
		import.datos.sistemaop = import.datos_def.sistemaop;
	if (!import.datos.isChecked.tamano)
		import.datos.tamano = import.datos_def.tamano;
	if (!import.datos.isChecked.graficos)
		import.datos.graficos = import.datos_def.graficos;
	if (!import.datos.isChecked.sonido)
		import.datos.sonido = import.datos_def.sonido;
	if (!import.datos.isChecked.jugabilidad)
		import.datos.jugabilidad = import.datos_def.jugabilidad;
	if (!import.datos.isChecked.original)
		import.datos.original = import.datos_def.original;
	if (!import.datos.isChecked.estado)
		import.datos.estado = import.datos_def.estado;
	if (!import.datos.isChecked.thumbs)
		import.datos.thumbs = import.datos_def.thumbs;
	if (!import.datos.isChecked.cover_front)
		import.datos.cover_front = import.datos_def.cover_front;
	if (!import.datos.isChecked.cover_back)
		import.datos.cover_back = import.datos_def.cover_back;
	if (!import.datos.isChecked.cover_left)
		import.datos.cover_left = import.datos_def.cover_left;
	if (!import.datos.isChecked.cover_right)
		import.datos.cover_right = import.datos_def.cover_right;
	if (!import.datos.isChecked.cover_top)
		import.datos.cover_top = import.datos_def.cover_top;
	if (!import.datos.isChecked.cover_bottom)
		import.datos.cover_bottom = import.datos_def.cover_bottom;
//	if (!import.datos.isChecked.tipo_emu)
//		import.datos.tipo_emu = import.datos_def.tipo_emu;
	if (!import.datos.isChecked.comentario)
		import.datos.comentario = import.datos_def.comentario;
	if (!import.datos.isChecked.favorito)
		import.datos.favorito = import.datos_def.favorito;
	if (!import.datos.isChecked.gamepad)
		import.datos.gamepad = import.datos_def.gamepad;
	if (!import.datos.isChecked.rating)
		import.datos.rating = import.datos_def.rating;
	if (!import.datos.isChecked.edad_recomendada)
		import.datos.edad_recomendada = import.datos_def.edad_recomendada;
	if (!import.datos.isChecked.usuario)
		import.datos.usuario = import.datos_def.usuario;
	if (!import.datos.isChecked.path_exe)
		import.datos.path_exe = import.datos_def.path_exe;
	if (!import.datos.isChecked.parametros_exe)
		import.datos.parametros_exe = import.datos_def.parametros_exe;
	if (!import.datos.isChecked.path_setup)
		import.datos.path_setup = import.datos_def.path_setup;
	if (!import.datos.isChecked.parametros_setup)
		import.datos.parametros_setup = import.datos_def.parametros_setup;
}

void frmImportarJuego::createListImagesDownload(QTreeWidgetItem *item, QList<stDownItem> &listDown)
{
	if (type_import == t_Script)
	{
		stImport i = list_import_temp[item];
	// Comprobamos las imágenes a descargar
		stDownItem itemdown;
		itemdown.tipo        = "GET";
		itemdown.contentPost = "";

		if (!i.datos.cover_front.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_front_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_front;
			listDown << itemdown;
		}

		if (!i.datos.cover_back.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_back_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_back;
			listDown << itemdown;
		}

		if (!i.datos.cover_left.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_left_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_left;
			listDown << itemdown;
		}

		if (!i.datos.cover_right.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_right_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_right;
			listDown << itemdown;
		}

		if (!i.datos.cover_top.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_top_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_top;
			listDown << itemdown;
		}

		if (!i.datos.cover_bottom.isEmpty())
		{
			itemdown.urlfile  = i.datos.cover_bottom_url;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.datos.cover_bottom;
			listDown << itemdown;
		}

		int num_images = i.imagenes.size();
		for (int m = 0; m < num_images; ++m)
		{
			itemdown.urlfile  = i.imagenes.at(m).url_full;
			itemdown.fileName = grlDir.Temp +"imagenes/"+ i.imagenes.at(m).nombre;
			listDown << itemdown;
		}
	}
}

void frmImportarJuego::createListZipFiles(QTreeWidgetItem *item, const int z_listSize)
{
	if (isZip && z_is_open && type_import == t_DBGL)
	{
		szEntryExtract z_item;
		stImport i = list_import_temp[item];

		QString z_captures = "captures/"+ i.dbgl.id_dbgl +"/";
		QString z_dosroot  = "dosroot/"+ i.dbgl.id_dbgl +"/";

		i.z_list.clear();
		for (int z = 0; z < z_listSize; ++z)
		{
			QString z_file_out = z_list.at(z);

			if (i.import_capturas)
			{
				if (z_file_out.startsWith(z_captures))
				{
					z_item.index   = z;
					z_item.fileOut = z_file_out;
					i.z_list << z_item;
				}
			}

			if (!i.solo_cfg_dosbox)
			{
				if (z_file_out.startsWith(z_dosroot))
				{
					z_item.index   = z;
					z_item.fileOut = z_file_out;
					i.z_list << z_item;
				}
			}
		}

		list_import_temp[item] = i;
	}
}

void frmImportarJuego::importarJuegos()
{
	const int num_import = list_import.size();
	if (num_import > 0)
	{
		bool isOk;
		stImport i;
		QString emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;

		httpdown->defaultInfo();
		httpdown->cfgProgressBar(0, num_import);
		httpdown->setProgressBarValue(1, tr("Importando") +": 1 "+ tr("de") +" "+ QString::number(num_import));

	// Fichas
		for (int n = 0; n < num_import; ++n)
		{
			httpdown->setProgressBarValue(n + 1, tr("Importando") +": "+ QString::number(n+1) +" "+ tr("de") +" "+ QString::number(num_import));

			i = list_import.at(n);

			setDefaultDatosIsChecked(i);

			if (i.datos.titulo_guiones.isEmpty())
				i.datos.titulo_guiones = sql->removeAccents(i.datos.titulo);

		// Datos Juego
			isOk = false;
			if (importMain)
			{
				i.datos.idgrl = sql->insertaDatos(categoria.tabla, i.datos);
				isOk = i.datos.idgrl.isEmpty() ? false : true;
			} else {
				i.datos.idgrl = DatosJuego.idgrl;
				isOk = true;
			}

			if (Editando || importMain)
			{
				if (Editando)
					i.datos.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
				else
					i.datos.game_dir = "id-"+ i.datos.idgrl +"_"+ i.datos.titulo_guiones +"_"+ i.datos.tipo_emu;

				grlDir.DatosGame = grlDir.DatosDbGrl + i.datos.game_dir +"/";

				if (importMain)
				{
					fGrl->comprobarDirectorio(grlDir.DatosGame);
					fGrl->comprobarDirectorio(grlDir.DatosGame +"archivos/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"caja/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"capturas/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/small/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"sonidos/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"videos/");
				}
			} else {
				i.datos.game_dir = "";
				grlDir.DatosGame = grlDir.Temp;
			}

			if (isOk)
			{
				i.is_importado = true;

				if (Editando || importMain)
				{
					stFileInfo f_info;
				// Thumbs
					if (!i.datos.thumbs.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.thumbs);
						isOk   = fGrl->crearThumbs(grlDir.Temp +"imagenes/small/"+ i.datos.thumbs, grlDir.DatosGame +"thumbs."+ grlCfg.thumb_format.toLower(), grlCfg.thumb_width, grlCfg.thumb_height, grlCfg.thumb_quality, false, grlCfg.thumb_format);
						if (isOk)
						{
							i.datos.thumbs = "thumbs"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "thumbs", i.datos.thumbs);
						}
					}
				// CoverFront
					if (!i.datos.cover_front.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_front);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_front, grlDir.DatosGame +"caja/cover_front"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_front = "cover_front"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_front", i.datos.cover_front);
						}
					}
				// CoverBack
					if (!i.datos.cover_back.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_back);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_back, grlDir.DatosGame +"caja/cover_back"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_back = "cover_back"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_back", i.datos.cover_back);
						}
					}
				// CoverLeft
					if (!i.datos.cover_left.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_left);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_left, grlDir.DatosGame +"caja/cover_left"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_left = "cover_left"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_left", i.datos.cover_left);
						}
					}
				// CoverRight
					if (!i.datos.cover_right.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_right);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_right, grlDir.DatosGame +"caja/cover_right"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_right = "cover_right"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_right", i.datos.cover_right);
						}
					}
				// CoverTop
					if (!i.datos.cover_top.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_top);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_top, grlDir.DatosGame +"caja/cover_top"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_top = "cover_top"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_top", i.datos.cover_top);
						}
					}
				// CoverBottom
					if (!i.datos.cover_bottom.isEmpty())
					{
						f_info = fGrl->getInfoFile(i.datos.cover_bottom);
						isOk   = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.datos.cover_bottom, grlDir.DatosGame +"caja/cover_bottom"+ f_info.Ext, false, true);
						if (isOk)
						{
							i.datos.cover_bottom = "cover_bottom"+ f_info.Ext;
							sql->actualizaDatosItem(categoria.tabla, i.datos.idgrl, "cover_bottom", i.datos.cover_bottom);
						}
					}
				}

			// Datos Archivos
				const int num_files = i.files.size();
				for (int f = 0; f < num_files; ++f)
				{
					i.files[f].idgrl = i.datos.idgrl;
					i.files[f].idcat = categoria.id;

					if (importMain)
						sql->insertaUnFiles(i.files.at(f));
				}

			// Datos Url
				const int num_urls = i.urls.size();
				for (int u = 0; u < num_urls; ++u)
				{
					i.urls[u].idgrl = i.datos.idgrl;
					i.urls[u].idcat = categoria.id;

					if (importMain)
						sql->insertaUnURL(i.urls.at(u));
				}


			// Datos Emuladores
				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "dosbox")
				{
				// DOSBox
					i.dosbox.idgrl = i.datos.idgrl;
					i.dosbox.idcat = categoria.id;
				//	i.dosbox.path_exe   = i.dosbox.path_exe.replace("{DirBaseGames}", grlCfg.DirBaseGames);
				//	i.dosbox.path_setup = i.dosbox.path_setup.replace("{DirBaseGames}", grlCfg.DirBaseGames);

					if (importMain)
					{
						i.dosbox.id = sql->insertaDbx(i.dosbox);
						sql->insertaMontajesDbx(i.dbxMount, i.dosbox.id);
					}
				}

				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "scummvm")
				{
				// ScummVM
					i.scummvm.idgrl = i.datos.idgrl;
					i.scummvm.idcat = categoria.id;

				//	i.scummvm.path       = i.scummvm.path.replace("{DirBaseGames}", grlCfg.DirBaseGames);
				//	i.scummvm.path_save  = i.scummvm.path_save.replace("{DirBaseGames}", grlCfg.DirBaseGames);
				//	i.scummvm.path_extra = i.scummvm.path_extra.replace("{DirBaseGames}", grlCfg.DirBaseGames);

					if (importMain)
					{
						i.scummvm.id = sql->insertaSvm(i.scummvm);
					}

					i.scummvm.emu_svmpath = emu_svmpath;
				}

				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "vdmsound")
				{
				// VDMSound
					i.vdmsound.idgrl = i.datos.idgrl;
					i.vdmsound.idcat = categoria.id;
					i.vdmsound.id    = sql->insertaVdms(i.vdmsound);
				}

			// Imágenes
				if (!i.solo_datos && !i.imagenes.isEmpty())
				{
					isOk = false;
					int num_images = i.imagenes.size();

					httpdown->defaultInfo();
					httpdown->cfgProgressBar(0, num_images);
					httpdown->setProgressBarValue(1, tr("Importando imágenes, juego") +": 1 "+ tr("de") +" "+ QString::number(num_images));

					for (int n = 0; n < num_images; ++n)
					{
						httpdown->setProgressBarValue(n + 1, tr("Importando imágenes, juego") +": "+ QString::number(n+1) +" "+ tr("de") +" "+ QString::number(num_images));

						if (Editando || importMain)
						{
							isOk = fGrl->copiarArchivo(grlDir.Temp +"imagenes/"+ i.imagenes.at(n).nombre, grlDir.DatosGame +"imagenes/"+ i.imagenes.at(n).nombre, false, false, true);
							if (isOk)
								fGrl->crearThumbs(grlDir.Temp +"imagenes/"+ i.imagenes.at(n).nombre, grlDir.DatosGame +"imagenes/small/"+ i.imagenes.at(n).nombre +".jpg", grlCfg.thumb_img_width, grlCfg.thumb_img_height, grlCfg.thumb_img_quality);
						} else {
							i.imagenes[n].isImport    = true;
							i.imagenes[n].crearThumbs = true;
							i.imagenes[n].dir_in      = grlDir.Temp;
							i.imagenes[n].dir_out     = "imagenes/";
						}
					}
				}
			} // Fin isOk

		// Actualizamos la información de la importación.
			list_import[n] = i;
		} // Fin for Ficha

	// Descomprimir
		if (isZip && z_is_open && type_import == t_DBGL)
		{
			httpdown->defaultInfo();
			httpdown->cfgProgressBar(0, num_import);
			httpdown->setProgressBarValue(1, tr("Descomprimiendo juego") +": 1 "+ tr("de") +" "+ QString::number(num_import));

			QList<szEntryExtract> z_list_extract;
			szEntryExtract z_item;
			for (int n = 0; n < num_import; ++n)
			{
				httpdown->setProgressBarValue(n+1, tr("Descomprimiendo juego") +": "+ QString::number(n+1) +" "+ tr("de") +" "+ QString::number(num_import));

				i = list_import.at(n);

				grlDir.DatosGame = grlDir.DatosDbGrl + i.datos.game_dir +"/";

				QString z_captures = "captures/"+ i.dbgl.id_dbgl +"/";
				QString z_dosroot  = "dosroot/"+ i.dbgl.id_dbgl +"/";

				const int z_listSize = i.z_list.size();
				for (int z = 0; z < z_listSize; ++z)
				{
					z_item.index = i.z_list.at(z).index;

					QString z_file_out = i.z_list.at(z).fileOut;

					if (z_file_out.startsWith(z_captures))
					{
						z_file_out.remove(z_captures);
						z_item.fileOut = z_file_out;
						z_item.dirOut  = grlDir.DatosGame +"capturas/";
					}

					if (z_file_out.startsWith(z_dosroot))
					{
						z_file_out.remove(z_dosroot);
						z_item.fileOut = z_file_out;
						z_item.dirOut  = z_dir_out;
					}

					z_list_extract << z_item;
				}
			}

			z_file->extract(z_list_extract);
		}

	// Creamos los config de DOSBox, ScumnmVM, VDMSound.
		if (Editando || importMain)
		{
			for (int n = 0; n < num_import; ++n)
			{
				i = list_import.at(n);

				grlDir.DatosGame = grlDir.DatosDbGrl + i.datos.game_dir +"/";


				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "dosbox")
				{
					fGrl->crearArchivoConfigDbx(i.datos, i.dosbox, i.dbxMount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox.conf");
					fGrl->crearArchivoConfigDbx(i.datos, i.dosbox, i.dbxMount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox-setup.conf", true);
				}

				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "scummvm")
				{
					fGrl->creaIniScummVM(i.scummvm, grlDir.DatosGame +"scummvm.ini");
				}

				if (!i.solo_datos && i.datos.tipo_emu.toLower() == "vdmsound")
				{
					fGrl->crearArchivoConfigVdmS(i.vdmsound, grlDir.DatosGame +"vdmsound.vlp");
				}
			}

		// Guardamos en la Base de Datos y Aceptamos
			sql->comit();
		}

		emit statusWidgetEnabled(true);
		QDialog::accept();
	} else {// Fin if num_import > 0
		QMessageBox::information(this, tr("Importar juego"), tr("Por favor selecciona al menos un juego de la lista para importarlo"));
		emit statusWidgetEnabled(true);
	}
}
// FIN Importar -------------------------------------------------------------------------------------------------
