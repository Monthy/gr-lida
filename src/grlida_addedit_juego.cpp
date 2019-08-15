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

//#include <QDesktopWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QPainter>

#include "grlida_addedit_juego.h"
#include "ui_addedit_juego.h"

frmAddEditJuego::frmAddEditJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, QString tipo_emu, bool m_editando, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmAddEditJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	IdGame    = id_game;
	TipoEmu   = tipo_emu;
	Editando  = m_editando;

	grlDir.Home       = fGrl->dirApp();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";
	grlDir.Iconos     = grlDir.Home +"iconos/";
	grlDir.Smiles     = grlDir.Home +"smiles/";
	grlDir.Temp       = grlDir.Home +"temp/";

	isOpenGrDap = false;
	isOpenPdf   = false;

	createWidgets();
	cargarConfig();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmAddEditJuego::~frmAddEditJuego()
{
	delete cbxDat_genero;
	delete cbxDat_compania;
	delete cbxDat_desarrollador;
	delete cbxDat_tema;
	delete cbxDat_idioma;
	delete cbxDat_idioma_voces;
	delete cbxDat_formato;
	delete cbxDat_perspectiva;
	delete cbxDat_sistemaop;
	delete txtDat_comentario;
	delete fGrl;

	delete ui;
}

void frmAddEditJuego::closeEvent(QCloseEvent *event)
{
	QMessageBox msgBox(this);
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setWindowTitle(tr("¿Cerrar ventana?"));
	msgBox.setText(tr("¿Deseas realmente cerrar la ventana?\nSi son nuevos datos o has echo cambios y no guardas puedes perder los cambios efectuados."));
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	msgBox.setButtonText(QMessageBox::Save  , tr("Guardar"));
	msgBox.setButtonText(QMessageBox::Close , tr("Cerrar"));
	msgBox.setButtonText(QMessageBox::Cancel, tr("Cancelar"));

	switch (msgBox.exec())
	{
		case QMessageBox::Save:
			event->ignore();
			emit on_btnOk_clicked();
		break;
		case QMessageBox::Cancel:
			event->ignore();
		break;
		case QMessageBox::Close:
			event->accept();
		break;
		default:
			event->ignore();
		break;
	}
}

void frmAddEditJuego::createWidgets()
{
	cbxDat_genero        = new QCheckComboBox(this);
	cbxDat_compania      = new QCheckComboBox(this);
	cbxDat_desarrollador = new QCheckComboBox(this);
	cbxDat_tema          = new QCheckComboBox(this);
	cbxDat_grupo         = new QCheckComboBox(this);
	cbxDat_idioma        = new QCheckComboBox(this);
	cbxDat_idioma_voces  = new QCheckComboBox(this);
	cbxDat_formato       = new QCheckComboBox(this);
	cbxDat_perspectiva   = new QCheckComboBox(this);
	cbxDat_sistemaop     = new QCheckComboBox(this);

	cbxDat_genero->setMinimumHeight(24);
	cbxDat_compania->setMinimumHeight(24);
	cbxDat_desarrollador->setMinimumHeight(24);
	cbxDat_tema->setMinimumHeight(24);
	cbxDat_grupo->setMinimumHeight(24);
	cbxDat_idioma->setMinimumHeight(24);
	cbxDat_idioma_voces->setMinimumHeight(24);
	cbxDat_formato->setMinimumHeight(24);
	cbxDat_perspectiva->setMinimumHeight(24);
	cbxDat_sistemaop->setMinimumHeight(24);

	ui->gLayout_datos->addWidget(cbxDat_genero, 2, 1, 1, 5);
	ui->gLayout_datos->addWidget(cbxDat_compania, 3, 1, 1, 5);
	ui->gLayout_datos->addWidget(cbxDat_desarrollador, 4, 1, 1, 5);
	ui->gLayout_datos->addWidget(cbxDat_tema, 5, 1, 1, 1);
	ui->gLayout_datos->addWidget(cbxDat_formato, 5, 4, 1, 2);
	ui->gLayout_datos->addWidget(cbxDat_grupo, 6, 1, 1, 1);
	ui->gLayout_datos->addWidget(cbxDat_idioma, 7, 1, 1, 1);
	ui->gLayout_datos->addWidget(cbxDat_idioma_voces, 8, 1, 1, 1);
	ui->gLayout_datos->addWidget(cbxDat_perspectiva, 9, 1, 1, 1);
	ui->gLayout_datos->addWidget(cbxDat_sistemaop, 10, 1, 1, 1);

	setTabOrder(ui->txtDat_subtitulo, cbxDat_genero       );
	setTabOrder(cbxDat_genero       , cbxDat_compania     );
	setTabOrder(cbxDat_compania     , cbxDat_desarrollador);
	setTabOrder(cbxDat_desarrollador, cbxDat_tema         );
	setTabOrder(cbxDat_tema         , cbxDat_grupo        );
	setTabOrder(cbxDat_grupo        , cbxDat_idioma       );
	setTabOrder(cbxDat_idioma       , cbxDat_idioma_voces );
	setTabOrder(cbxDat_idioma_voces , cbxDat_formato      );
	setTabOrder(cbxDat_formato      , ui->cbxDat_anno     );
	setTabOrder(ui->cbxDat_numdisc  , cbxDat_perspectiva  );
	setTabOrder(cbxDat_perspectiva  , ui->cbxDat_icono    );
	setTabOrder(ui->cbxDat_icono    , cbxDat_sistemaop    );
	setTabOrder(cbxDat_sistemaop    , ui->txtDat_tamano   );

	wDbx = new frmAddEditDosBox(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->vLayout_wDbx->addWidget(wDbx);

	wSvm = new frmAddEditScummVM(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->vLayout_wSvm->addWidget(wSvm);

	wVdms = new frmAddEditVDMSound(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->vLayout_wVdms->addWidget(wVdms);

	txtDat_comentario = new EditorWidget(fGrl->theme(), this);
	ui->vLayout_description->addWidget(txtDat_comentario);
	txtDat_comentario->showSource(false);
	txtDat_comentario->showFindReplace(false);
	txtDat_comentario->showSmiles(true);

	if (QFile::exists(grlDir.Datos +"smiles.txt"))
		txtDat_comentario->setSmileList(grlDir.Datos +"smiles.txt", "\",\"", grlDir.Smiles);
	else
		txtDat_comentario->setSmileList(":/datos/smiles.txt", "\",\"", grlDir.Smiles);

	mediaSound = new GrlMultiMedia(grlDir.DatosGame, fGrl->theme(), grlCfg, true, false, ui->tabSonidos);
	mediaSound->setTemplate(fGrl->tplInfoJuego("tpl_info_media"));
	mediaSound->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(mediaSound, SIGNAL(changeConfig()), this, SLOT(changeMediaConfig()));

	ui->gLayout_mediaSound->addWidget(mediaSound, 0, 0, 3, 1);
	ui->gLayout_mediaSound->addWidget(ui->btnDat_sonidos_add, 0, 1, 1, 1);
	ui->gLayout_mediaSound->addWidget(ui->btnDat_sonidos_eliminar, 1, 1, 1, 1);

	mediaVideo = new GrlMultiMedia(grlDir.DatosGame, fGrl->theme(), grlCfg, false, false, ui->tabVideos);
	mediaVideo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(mediaVideo, SIGNAL(changeConfig()), this, SLOT(changeMediaConfig()));

	ui->gLayout_mediaVideo->addWidget(mediaVideo, 0, 0, 3, 1);
	ui->gLayout_mediaVideo->addWidget(ui->btnDat_videos_add, 0, 1, 1, 1);
	ui->gLayout_mediaVideo->addWidget(ui->btnDat_videos_eliminar, 1, 1, 1, 1);
}

void frmAddEditJuego::cargarConfig()
{
	ui->tabw_Datos->setTabEnabled(tabEjecutabeRom, false);
	ui->tabw_Datos->setTabEnabled(tabDOSBox  , false);
	ui->tabw_Datos->setTabEnabled(tabScummVM , false);
	ui->tabw_Datos->setTabEnabled(tabVDMSound, false);

	ui->cbxDat_tipo_emu->clear();
	QStringList lista = categoria.emu_show.split(";", QString::SkipEmptyParts);
	if (lista.isEmpty() || lista.contains("all") || lista.contains("datos"))
		ui->cbxDat_tipo_emu->addItem(QIcon(fGrl->theme() +"img16/cat/datos.png"), tr("Datos"), "datos");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("dosbox"))
		ui->cbxDat_tipo_emu->addItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), tr("DOSBox"), "dosbox");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("scummvm"))
		ui->cbxDat_tipo_emu->addItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("ScummVM"), "scummvm");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound"))
		ui->cbxDat_tipo_emu->addItem(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"), tr("VDMSound"), "vdmsound");

	QList<stGrlDatos> list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	const int listSize = list_emu.size();
	for (int i = 0; i < listSize; ++i)
	{
		if (lista.isEmpty() || lista.contains("all") || lista.contains(list_emu.at(i).key))
		{
			if (QFile::exists(fGrl->theme() +"img16/"+ list_emu.at(i).icono))
				ui->cbxDat_tipo_emu->addItem(QIcon(fGrl->theme() +"img16/"+ list_emu.at(i).icono), list_emu.at(i).titulo, list_emu.at(i).key);
			else
				ui->cbxDat_tipo_emu->addItem(QIcon(":/img16/sinimg.png"), list_emu.at(i).titulo, list_emu.at(i).key);
		}
	}

	int row_tipo_emu = ui->cbxDat_tipo_emu->findData(TipoEmu);
	if (row_tipo_emu < 0) row_tipo_emu = 0;
	ui->cbxDat_tipo_emu->setCurrentIndex(row_tipo_emu);
	emit on_cbxDat_tipo_emu_activated(row_tipo_emu);

	ui->cbxUrl_url->clear();
	ui->cbxUrl_url->addItem(QIcon(fGrl->theme() +"img16/edit_enlace.png"), "", "");
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT DISTINCT url, descripcion FROM dbgrl_urls");
	if (query.first())
	{
		do {
			ui->cbxUrl_url->addItem(QIcon(fGrl->theme() +"img16/edit_enlace.png"), query.record().value("url").toString(), query.record().value("descripcion").toString());
		} while (query.next());
	}
	query.clear();
	ui->cbxUrl_url->setCurrentIndex(0);
	emit on_cbxUrl_url_activated(0);

// Configuración del twDatosParametrosExe
	ui->twDatosParametrosExe->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosParametrosExe->header()->setSectionsMovable(false);
#else
	ui->twDatosParametrosExe->header()->setMovable(false);
#endif
	ui->twDatosParametrosExe->setColumnWidth(0, 220);
	ui->twDatosParametrosExe->setColumnWidth(1, 220);

// Configuración del twDatosParametrosSetup
	ui->twDatosParametrosSetup->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosParametrosSetup->header()->setSectionsMovable(false);
#else
	ui->twDatosParametrosSetup->header()->setMovable(false);
#endif
	ui->twDatosParametrosSetup->setColumnWidth(0, 220);
	ui->twDatosParametrosSetup->setColumnWidth(1, 220);

// Configuración del twDatosFiles
	ui->twDatosFiles->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosFiles->header()->setSectionsMovable(false);
	ui->twDatosFiles->header()->setSectionResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setSectionResizeMode(1, QHeaderView::Fixed);
	ui->twDatosFiles->header()->setSectionResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setSectionResizeMode(3, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setSectionResizeMode(4, QHeaderView::Interactive);
#else
	ui->twDatosFiles->header()->setMovable(false);
	ui->twDatosFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(1, QHeaderView::Fixed);
	ui->twDatosFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(4, QHeaderView::Interactive);
#endif
	ui->twDatosFiles->setColumnWidth(0, 200);
	ui->twDatosFiles->setColumnWidth(1, 65);
	ui->twDatosFiles->setColumnWidth(2, 100);
	ui->twDatosFiles->setColumnWidth(3, 50);
	ui->twDatosFiles->setColumnWidth(4, 50);

// Configuración del twDatosURL
	ui->twDatosURL->header()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
	ui->twDatosURL->header()->setSectionsMovable(false);
	ui->twDatosURL->header()->setSectionResizeMode(0, QHeaderView::Interactive);
	ui->twDatosURL->header()->setSectionResizeMode(1, QHeaderView::Interactive);
#else
	ui->twDatosURL->header()->setMovable(false);
	ui->twDatosURL->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosURL->header()->setResizeMode(1, QHeaderView::Interactive);
#endif
	ui->twDatosURL->setColumnWidth(0, 250);

// Configuración lista
// Capturas
	ui->lwCapturas->setIconSize(QSize(128, 128));
	ui->lwCapturas->setMovement(QListView::Static);
	ui->lwCapturas->setFlow(QListView::LeftToRight);
	ui->lwCapturas->setResizeMode(QListView::Adjust);
	ui->lwCapturas->setViewMode(QListView::IconMode);
// Imágenes
	ui->lwImagenes->setIconSize(QSize(128, 128));
	ui->lwImagenes->setMovement(QListView::Static);
	ui->lwImagenes->setFlow(QListView::LeftToRight);
	ui->lwImagenes->setResizeMode(QListView::Adjust);
	ui->lwImagenes->setViewMode(QListView::IconMode);

	fGrl->cargarDatosCheckComboBox(cbxDat_genero       , grlDir.Datos + fGrl->idioma() +"/generos.txt"     , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_compania     , grlDir.Datos +"companias.txt"                     , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_desarrollador, grlDir.Datos +"companias.txt"                     , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_tema         , grlDir.Datos + fGrl->idioma() +"/temas.txt"       , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_grupo        , grlDir.Datos +"grupos.txt"                        , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_idioma       , grlDir.Datos + fGrl->idioma() +"/idiomas.txt"     , "img16/", "", 3, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_idioma_voces , grlDir.Datos + fGrl->idioma() +"/idiomas.txt"     , "img16/", "", 3, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_formato      , grlDir.Datos + fGrl->idioma() +"/formatos.txt"    , "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_perspectiva  , grlDir.Datos + fGrl->idioma() +"/perspectivas.txt", "img16/", "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDat_sistemaop    , grlDir.Datos +"sistemas.txt"                      , "img16/", "", 2, "|");

	ui->cbxDat_anno->clear();
	ui->cbxDat_anno->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "", "");

	const QStringList fechas = fGrl->listaFechas();
	const int listFechasSize = fechas.size();
	for (int i = 0; i < listFechasSize; ++i)
		ui->cbxDat_anno->addItem(QIcon(fGrl->theme() +"img16/fecha.png"), fechas.at(i), fechas.at(i));
	ui->cbxDat_anno->setCurrentIndex(0);

	fGrl->cargarDatosComboBox(ui->cbxDat_edad_recomendada, grlDir.Datos + fGrl->idioma() +"/edad_recomendada.txt", "img16/", "nd", 3, "|");
	fGrl->cargarDatosComboBox(ui->cbxDat_numdisc         , grlDir.Datos + fGrl->idioma() +"/numdisc.txt"         , "img16/", ""  , 2, "|");
	fGrl->cargarDatosComboBox(ui->cbxDat_rating          , ":datos/rating.txt"                                   , "img16/", "0" , 2, "|");
	fGrl->cargarDatosComboBox(ui->cbxDat_graficos        , ":datos/puntuacion.txt"                               , "img16/", "0" , 2, "|");
	fGrl->cargarDatosComboBox(ui->cbxDat_sonido          , ":datos/puntuacion.txt"                               , "img16/", "0" , 2, "|");
	fGrl->cargarDatosComboBox(ui->cbxDat_jugabilidad     , ":datos/puntuacion.txt"                               , "img16/", "0" , 2, "|");

	fGrl->cargarIconosComboBox(ui->cbxDat_icono, grlDir.Iconos, ""          , ""    , ""          , grlCfg.FormatsImage.join(";"));
	fGrl->cargarIconosComboBox(ui->cbxDat_icono, fGrl->theme(), "img24/cat/", "cat/", "sinimg.png", grlCfg.FormatsImage.join(";"), false);
	ui->cbxDat_estado->setVisible(false);

	dap_ext.clear();
	dap_ext << "*.dap" << "*.dapz" << "*.conf";
	comic_ext.clear();
	comic_ext << "*.cbz" << "*.cbr" << "*.cb7" << "*.cbt";
	z_ext.clear();
	z_ext << "*.zip" << "*.rar" << "*.7z" << "*.tar";

	QString ext_dap   = dap_ext.join(" ");
	QString ext_comic = comic_ext.join(" ");
	QString ext_z     = z_ext.join(" ");

	ui->cbxFile_tipo->clear();
	ui->cbxFile_tipo->addItem(QIcon(fGrl->theme() +"img16/cat/datos.png"), tr("Documentos - Manuales") +" - ("+ ext_comic +" "+ ext_z +")", "manual");
	ui->cbxFile_tipo->addItem(QIcon(fGrl->theme() +"img16/pdf.png")      , tr("Documentos - Manuales") +" - (pdf)"                        , "pdf"   );
	ui->cbxFile_tipo->addItem(QIcon(fGrl->theme() +"img16/ruleta.png")   , tr("Ruleta de protección") +" - ("+ ext_dap +" "+ ext_z +")"   , "ruleta");
	ui->cbxFile_tipo->addItem(QIcon(fGrl->theme() +"img16/archivos.png") , tr("Cualquier archivos")                                       , ""      );

	ui->cbxDat_imagenes_asignar_como->clear();
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/imagen.png")    , tr("Portada o pantallazo"), 0);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_front.png") , tr("Carátula delantera")  , 1);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_back.png")  , tr("Carátula trasera")    , 2);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_left.png")  , tr("Carátula izquierda")  , 3);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_right.png") , tr("Carátula derecha")    , 4);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_top.png")   , tr("Carátula superior")   , 5);
	ui->cbxDat_imagenes_asignar_como->addItem(QIcon(fGrl->theme() +"img16/box_bottom.png"), tr("Carátula inferior")   , 6);

	listImagenesAdd.clear();
	listVideosAdd.clear();

	if (Editando)
	{
		DatosJuego = sql->show_Datos(categoria.tabla, IdGame);

		sql->cargarDatosUrls(ui->twDatosURL   , IdGame, categoria.id);
		sql->cargarDatosFiles(ui->twDatosFiles, IdGame, categoria.id);

		if (TipoEmu == "dosbox")
		{
			ui->tabw_Datos->setTabEnabled(tabDOSBox, true);
			wDbx->cargarConfig();
			wDbx->cargarDatosDosBox(sql->showConfg_DOSBox(IdGame, categoria.id));
		}
		if (TipoEmu == "scummvm")
		{
			ui->tabw_Datos->setTabEnabled(tabScummVM , true);
			wSvm->cargarConfig();
			wSvm->cargarDatosScummVM(sql->showConfg_ScummVM(IdGame, categoria.id));
		}
		if (TipoEmu == "vdmsound")
		{
			ui->tabw_Datos->setTabEnabled(tabVDMSound, true);
			wVdms->cargarConfig();
			wVdms->cargarDatosVDMSound(sql->showConfg_VDMSound(IdGame, categoria.id));
		}

		ui->cbxDat_tipo_emu->setEnabled(false);
		setWindowTitle(categoria.titulo +" - "+ tr("Editando a") +": "+ DatosJuego.titulo);
	} else {
		DatosJuego = sql->getDefectDatos("datos");

		wDbx->cargarConfig();
		wDbx->cargarDatosDosBox(sql->getDefectDOSBox());

		wSvm->cargarConfig();
		wSvm->cargarDatosScummVM(sql->getDefectScummVM());

		wVdms->cargarConfig();
		wVdms->cargarDatosVDMSound(sql->getDefectVDMSound());

		ui->cbxDat_tipo_emu->setEnabled(true);
		setWindowTitle(categoria.titulo +" - "+ tr("Añadiendo un nuevo juego"));
	}

	grlDir.DatosGame = grlDir.DatosDbGrl + DatosJuego.game_dir +"/";

	cargarDatosJuego(DatosJuego);
	setTheme();

	ui->txtDat_titulo->setFocus();
}

void frmAddEditJuego::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/datos_1.png"));

// General
	ui->tabw_Datos->setTabIcon(tabDatos       , QIcon(fGrl->theme() +"img16/cat/datos.png"));
	ui->tabw_Datos->setTabIcon(tabDescripcion , QIcon(fGrl->theme() +"img16/nuevo.png"));
	ui->tabw_Datos->setTabIcon(tabMultimedia  , QIcon(fGrl->theme() +"img16/multimedia.png"));
	ui->tabw_Datos->setTabIcon(tabEjecutabeRom, QIcon(fGrl->theme() +"img16/ejecutar_app.png"));
	ui->tabw_Datos->setTabIcon(tabOtrosDatos  , QIcon(fGrl->theme() +"img16/importar.png"));
	ui->tabw_Datos->setTabIcon(tabDOSBox      , QIcon(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->tabw_Datos->setTabIcon(tabScummVM     , QIcon(fGrl->theme() +"img16/cat/scummvm.png"));
	ui->tabw_Datos->setTabIcon(tabVDMSound    , QIcon(fGrl->theme() +"img16/cat/vdmsound.png"));

	ui->tabw_OtrosDatos->setTabIcon(0, QIcon(fGrl->theme() +"img16/archivos.png"));
	ui->tabw_OtrosDatos->setTabIcon(1, QIcon(fGrl->theme() +"img16/html.png"));

	ui->btnDescargarInfo->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
// Datos
	ui->btnDat_grupo_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_thumbs_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_thumbs_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_thumbs_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_thumbs_crear->setIcon(QIcon(fGrl->theme() +"img16/cover_create.png"));
// Multimedia
	ui->tabw_Multimedia->setTabIcon(0, QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->tabw_Multimedia->setTabIcon(1, QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->tabw_Multimedia->setTabIcon(2, QIcon(fGrl->theme() +"img16/capturas.png"));
	ui->tabw_Multimedia->setTabIcon(3, QIcon(fGrl->theme() +"img16/video.png"));
	ui->tabw_Multimedia->setTabIcon(4, QIcon(fGrl->theme() +"img16/audio.png"));
//	ui->btnDatos_PathCapturas->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
//	ui->btnDatos_PathCapturas_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_cover_front_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_front_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_front_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_thumbs_crear_->setIcon(QIcon(fGrl->theme() +"img16/cover_create.png"));
	ui->btnDat_cover_back_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_back_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_back_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_cover_left_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_left_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_left_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_cover_right_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_right_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_right_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_cover_top_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_top_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_top_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_cover_bottom_abrir->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_cover_bottom_ver->setIcon(QIcon(fGrl->theme() +"img16/imagen.png"));
	ui->btnDat_cover_bottom_eliminar->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
// Imágenes
	ui->btnDat_imagenes_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_imagenes_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDat_imagenes_asignar->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
// Capturas
	ui->btnDat_capturas_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_capturas_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Videos
	ui->btnDat_videos_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_videos_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Sonidos
	ui->btnDat_sonidos_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_sonidos_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Otros Datos
// Archivos
	ui->btnFile_path->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnFile_path_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnFile_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnFile_update->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnFile_delete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Dirección URLs
	ui->btnUrl_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnUrl_update->setIcon(QIcon(fGrl->theme() +"img16/actualizar.png"));
	ui->btnUrl_abrir->setIcon(QIcon(fGrl->theme() +"img16/edit_enlace.png"));
	ui->btnUrl_delete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
// Usuarios
	ui->btnDat_usuario_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
// Ejecutable / Rom
	ui->btnDat_path_exe->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_path_exe_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_parametros_exe_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));

	ui->btnDat_parametros_exe_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_parametros_exe_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDat_parametros_exe_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
// Setup
	ui->btnDat_path_setup->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDat_path_setup_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDat_parametros_setup_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));

	ui->btnDat_parametros_setup_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnDat_parametros_setup_eliminar->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDat_parametros_setup_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
}

void frmAddEditJuego::cargarDatosJuego(stDatosJuego datos, bool isImport)
{
	if (Editando)
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

	int row_icon = ui->cbxDat_icono->findData(datos.icono);
	if (row_icon < 0) row_icon = 0;

	oldTitulo = datos.titulo;

	ui->cbxDat_icono->setCurrentIndex(row_icon);
	ui->txtDat_titulo->setText(datos.titulo);
//	TituloGuiones = datos.titulo_guiones.isEmpty() ? sql->removeAccents(datos.titulo) : datos.titulo_guiones;
	ui->txtDat_subtitulo->setText(datos.subtitulo);
	cbxDat_genero->setCheckedItems(datos.genero);
	cbxDat_compania->setCheckedItems(datos.compania);
	cbxDat_desarrollador->setCheckedItems(datos.desarrollador);
	cbxDat_tema->setCheckedItems(datos.tema);
	cbxDat_grupo->setCheckedItems(datos.grupo);
	cbxDat_perspectiva->setCheckedItems(datos.perspectiva);
	cbxDat_idioma->setCheckedItems(datos.idioma);
	cbxDat_idioma_voces->setCheckedItems(datos.idioma_voces);
	cbxDat_formato->setCheckedItems(datos.formato);
	ui->cbxDat_anno->setCurrentIndex(ui->cbxDat_anno->findText(datos.anno));
	ui->cbxDat_numdisc->setCurrentIndex(ui->cbxDat_numdisc->findData(datos.numdisc));
	cbxDat_sistemaop->setCheckedItems(datos.sistemaop);
	ui->txtDat_tamano->setText(datos.tamano);
	ui->cbxDat_graficos->setCurrentIndex(ui->cbxDat_graficos->findText(datos.graficos));
	ui->cbxDat_sonido->setCurrentIndex(ui->cbxDat_sonido->findText(datos.sonido));
	ui->cbxDat_jugabilidad->setCurrentIndex(ui->cbxDat_jugabilidad->findText(datos.jugabilidad));
	ui->chkDat_original->setChecked(fGrl->strToBool(datos.original));
	ui->cbxDat_estado->setCurrentIndex(ui->cbxDat_estado->findText(datos.estado));
	Thumbs      = datos.thumbs;
	oldThumbs   = datos.thumbs;
	CoverFront  = datos.cover_front;
	CoverBack   = datos.cover_back;
	CoverLeft   = datos.cover_left;
	CoverRight  = datos.cover_right;
	CoverTop    = datos.cover_top;
	CoverBottom = datos.cover_bottom;
	ui->lb_fechahora->setText((Editando) ? fGrl->horaFechaActual(datos.fecha, grlCfg.FormatoFecha) : fGrl->horaFechaActual(sql->getTime(), grlCfg.FormatoFecha));
//	TipoEmu = datos.tipo_emu;
	if (!datos.comentario.isEmpty())
		txtDat_comentario->setText(datos.comentario);
	ui->chkDat_favorito->setChecked(fGrl->strToBool(datos.favorito));
	ui->chkDat_gamepad->setChecked(fGrl->strToBool(datos.gamepad));
	ui->cbxDat_rating->setCurrentIndex(ui->cbxDat_rating->findData(datos.rating));
	ui->cbxDat_edad_recomendada->setCurrentIndex(ui->cbxDat_edad_recomendada->findData(datos.edad_recomendada));
	ui->txtDat_usuario->setText(datos.usuario);
	ui->txtDat_path_exe->setText(datos.path_exe);
	ui->txtDat_path_setup->setText(datos.path_setup);

	cargarParametrosTwList(ui->twDatosParametrosExe, datos.parametros_exe);
	cargarParametrosTwList(ui->twDatosParametrosSetup, datos.parametros_setup);

// Thumbs
	QString dir_game = grlDir.DatosGame;
	if (!Editando && isImport)
		dir_game = grlDir.Temp;

	if (!Editando && isImport)
		file_thumbs = dir_game +"imagenes/"+ Thumbs;
	else
		file_thumbs = dir_game + Thumbs;

	if (!Thumbs.isEmpty() && QFile::exists(file_thumbs))
	{
		ui->lbDat_thumbs->setPixmap(QPixmap(file_thumbs));
		ui->btnDat_thumbs_ver->setEnabled(true);
		ui->btnDat_thumbs_eliminar->setEnabled(true);
	} else
		emit on_btnDat_thumbs_eliminar_clicked();

// CoverFront
	if (!Editando && isImport)
		file_cover_front = dir_game +"imagenes/"+ CoverFront;
	else
		file_cover_front = dir_game +"caja/"+ CoverFront;

	if (!CoverFront.isEmpty() && QFile::exists(file_cover_front))
	{
		ui->lbDat_cover_front->setPixmap(QPixmap(file_cover_front));
		ui->btnDat_cover_front_ver->setEnabled(true);
		ui->btnDat_cover_front_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_front_eliminar_clicked();

// CoverBack
	if (!Editando && isImport)
		file_cover_back = dir_game +"imagenes/"+ CoverBack;
	else
		file_cover_back = dir_game +"caja/"+ CoverBack;

	if (!CoverBack.isEmpty() && QFile::exists(file_cover_back))
	{
		ui->lbDat_cover_back->setPixmap(QPixmap(file_cover_back));
		ui->btnDat_cover_back_ver->setEnabled(true);
		ui->btnDat_cover_back_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_back_eliminar_clicked();

// CoverLeft
	if (!Editando && isImport)
		file_cover_left = dir_game +"imagenes/"+ CoverLeft;
	else
		file_cover_left = dir_game +"caja/"+ CoverLeft;

	if (!CoverLeft.isEmpty() && QFile::exists(file_cover_left))
	{
		ui->lbDat_cover_left->setPixmap(QPixmap(file_cover_left));
		ui->btnDat_cover_left_ver->setEnabled(true);
		ui->btnDat_cover_left_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_left_eliminar_clicked();

// CoverRight
	if (!Editando && isImport)
		file_cover_right = dir_game +"imagenes/"+ CoverRight;
	else
		file_cover_right = dir_game +"caja/"+ CoverRight;

	if (!CoverRight.isEmpty() && QFile::exists(file_cover_right))
	{
		ui->lbDat_cover_right->setPixmap(QPixmap(file_cover_right));
		ui->btnDat_cover_right_ver->setEnabled(true);
		ui->btnDat_cover_right_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_right_eliminar_clicked();

// CoverTop
	if (!Editando && isImport)
		file_cover_top = dir_game +"imagenes/"+ CoverTop;
	else
		file_cover_top = dir_game +"caja/"+ CoverTop;

	if (!CoverTop.isEmpty() && QFile::exists(file_cover_top))
	{
		ui->lbDat_cover_top->setPixmap(QPixmap(file_cover_top));
		ui->btnDat_cover_top_ver->setEnabled(true);
		ui->btnDat_cover_top_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_top_eliminar_clicked();

// CoverBottom
	if (!Editando && isImport)
		file_cover_bottom = dir_game +"imagenes/"+ CoverBottom;
	else
		file_cover_bottom = dir_game +"caja/"+ CoverBottom;

	if (!CoverBottom.isEmpty() && QFile::exists(file_cover_bottom))
	{
		ui->lbDat_cover_bottom->setPixmap(QPixmap(file_cover_bottom));
		ui->btnDat_cover_bottom_ver->setEnabled(true);
		ui->btnDat_cover_bottom_eliminar->setEnabled(true);
	} else
		emit on_btnDat_cover_bottom_eliminar_clicked();

// Capturas, videos y sonidos.
	if (Editando || isImport)
	{
		if (isImport)
		{
			QString archivo, thumbs;
			stDatosImagenes imagen;
			const int numImagenes = listImagenesImportadas.size();
			for (int i = 0; i < numImagenes; ++i)
			{
				imagen = listImagenesImportadas.at(i);
				if (imagen.isImport)
				{
					archivo = imagen.dir_in +"imagenes/"+ imagen.nombre;
					thumbs  = imagen.dir_in +"imagenes/small/"+ imagen.nombre;

					QListWidgetItem *item = new QListWidgetItem;
					item->setData(Qt::UserRole, archivo);
					if (QFile::exists(thumbs))
						item->setIcon(QIcon(thumbs));
					else
						item->setIcon(QIcon(fGrl->theme() +"images/juego_sin_imagen.png"));
					ui->lwImagenes->addItem(item);
				}
			}
		} else {
			fGrl->cargarArchivosLwLista(ui->lwCapturas, dir_game +"capturas/", grlCfg.FormatsImage.join(";"), ";", "capturas.png", true, true);
			fGrl->cargarArchivosLwLista(ui->lwImagenes, dir_game +"imagenes/", grlCfg.FormatsImage.join(";"), ";", "capturas.png", true, true, true);
		}

		mediaVideo->setDirPlaylist(grlDir.DatosGame +"capturas/", grlCfg.FormatsVideo, true);
		mediaVideo->setDirPlaylist(grlDir.DatosGame +"videos/"  , grlCfg.FormatsVideo);
		mediaSound->setDirPlaylist(grlDir.DatosGame +"capturas/", grlCfg.FormatsMusic, true);
		mediaSound->setDirPlaylist(grlDir.DatosGame +"sonidos/" , grlCfg.FormatsMusic);
	}

/*
// Si el tipo de emu es distinto actualizar infomación
	if (Editando)
	{
//		IdGame = datos.idgrl;
		if (TipoEmu != datos.tipo_emu)
		{
			if (fGrl->questionMsg(tr("Tipo emulador distintos"),
								tr("El tipo de mulador no corresponde al seleccionado.") +"\n"+
								tr("¿Deseas crear la configuración por defecto?")))
			{
//				DatosJuego.tipo_emu = TipoEmu;
//				sql->actualizaDatosItem(categoria.tabla, IdGame, "tipo_emu", TipoEmu);

//				QString img_tmp_name = "id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(DatosJuego.titulo) +"_"+ TipoEmu;

//				if (!datos.thumbs.isEmpty())
//				{
//					datos.thumbs = img_tmp_name +"_thumbs"+ fGrl->getInfoFile(Thumbs).Ext;
//					QFile::rename(grlDir.Thumbs + Thumbs, grlDir.Thumbs + datos.thumbs);
//					sql->actualizaDatosItem(categoria.tabla, IdGame, "thumbs", datos.thumbs);
//				}

//				if (!datos.cover_front.isEmpty())
//				{
//					datos.cover_front = img_tmp_name +"_cover_front"+ fGrl->getInfoFile(CoverFront).Ext;
//					QFile::rename(grlDir.Covers + CoverFront, grlDir.Covers + datos.cover_front);
//					sql->actualizaDatosItem(categoria.tabla, IdGame, "cover_front", datos.cover_front);
//				}

//				if (!datos.cover_back.isEmpty())
//				{
//					datos.cover_back = img_tmp_name +"_cover_back"+ fGrl->getInfoFile(CoverBack).Ext;
//					QFile::rename(grlDir.Covers + CoverBack, grlDir.Covers + datos.cover_back);
//					sql->actualizaDatosItem(categoria.tabla, IdGame, "cover_back", datos.cover_back);
//				}

//				if (TipoEmu == "dosbox")
//				{
//					ui->tabw_Datos->setTabEnabled(tabDOSBox, true);
//					wDbx->cargarConfig();

//					DatosDosBox = sql->showConfg_DOSBox(IdGame, categoria.id);
//					DatosDosBox.idgrl = IdGame;
//					DatosDosBox.idcat = categoria.id;

//					if (DatosDosBox.id.isEmpty())
//						DatosDosBox.id = sql->insertaDbx(DatosDosBox);

//					wDbx->cargarDatosDosBox(DatosDosBox);
//				}

//				if (TipoEmu == "scummvm")
//				{
//					ui->tabw_Datos->setTabEnabled(tabScummVM , true);
//					wSvm->cargarConfig();

//					DatosScummVM = sql->showConfg_ScummVM(IdGame, categoria.id);
//					DatosScummVM.idgrl = IdGame;
//					DatosScummVM.idcat = categoria.id;

//					if (DatosScummVM.id.isEmpty())
//						DatosScummVM.id = sql->insertaSvm(DatosScummVM);

//					wSvm->cargarDatosScummVM(DatosScummVM);
//				}

//				if (TipoEmu == "vdmsound")
//				{
//					ui->tabw_Datos->setTabEnabled(tabVDMSound, true);
//					wVdms->cargarConfig();

//					DatosVDMSound = sql->showConfg_VDMSound(IdGame, categoria.id);
//					DatosVDMSound.idgrl = IdGame;
//					DatosVDMSound.idcat = categoria.id;

//					if (DatosVDMSound.id.isEmpty())
//						DatosVDMSound.id = sql->insertaVdms(DatosVDMSound);

//					wVdms->cargarDatosVDMSound(DatosVDMSound);
//				}
			}
		}
	}*/
}

bool frmAddEditJuego::setDatosJuegos(bool isSoloDatos)
{
	QString str;
	bool isOk = false;

	oldGameDir = DatosJuego.game_dir;

	str = ui->cbxDat_icono->itemData(ui->cbxDat_icono->currentIndex()).toString();
	DatosJuego.icono          = str.isEmpty() ? "cat/datos.png" : str;
	DatosJuego.titulo         = ui->txtDat_titulo->text();
	DatosJuego.titulo_guiones = sql->removeAccents(DatosJuego.titulo);
	DatosJuego.subtitulo      = ui->txtDat_subtitulo->text();
	DatosJuego.genero         = cbxDat_genero->getCheckedItems();
	DatosJuego.compania       = cbxDat_compania->getCheckedItems();
	DatosJuego.desarrollador  = cbxDat_desarrollador->getCheckedItems();
	DatosJuego.tema           = cbxDat_tema->getCheckedItems();
	DatosJuego.grupo          = cbxDat_grupo->getCheckedItems();
	DatosJuego.perspectiva    = cbxDat_perspectiva->getCheckedItems();
	DatosJuego.idioma         = cbxDat_idioma->getCheckedItems();
	DatosJuego.idioma_voces   = cbxDat_idioma_voces->getCheckedItems();
	DatosJuego.formato        = cbxDat_formato->getCheckedItems();
	DatosJuego.anno           = ui->cbxDat_anno->currentText().isEmpty() ? " " : ui->cbxDat_anno->currentText();
	str = ui->cbxDat_numdisc->itemData(ui->cbxDat_numdisc->currentIndex()).toString();
	DatosJuego.numdisc      = str.isEmpty() ? "" : str;
	DatosJuego.sistemaop    = cbxDat_sistemaop->getCheckedItems();
	DatosJuego.tamano       = ui->txtDat_tamano->text();
	DatosJuego.graficos     = ui->cbxDat_graficos->currentText().isEmpty() ? "0" : ui->cbxDat_graficos->currentText();
	DatosJuego.sonido       = ui->cbxDat_sonido->currentText().isEmpty() ? "0" : ui->cbxDat_sonido->currentText();
	DatosJuego.jugabilidad  = ui->cbxDat_jugabilidad->currentText().isEmpty() ? "0" : ui->cbxDat_jugabilidad->currentText();
	DatosJuego.original     = fGrl->boolToStr(ui->chkDat_original->isChecked());
	DatosJuego.estado       = ui->cbxDat_estado->currentText().isEmpty() ? "" : ui->cbxDat_estado->currentText();
	DatosJuego.thumbs       = Thumbs;
	DatosJuego.cover_front  = CoverFront;
	DatosJuego.cover_back   = CoverBack;
	DatosJuego.cover_left   = CoverLeft;
	DatosJuego.cover_right  = CoverRight;
	DatosJuego.cover_top    = CoverTop;
	DatosJuego.cover_bottom = CoverBottom;
	if (!Editando)
		DatosJuego.fecha = sql->getTime();
	DatosJuego.comentario = txtDat_comentario->toPlainText();
	DatosJuego.favorito   = fGrl->boolToStr(ui->chkDat_favorito->isChecked());
	DatosJuego.gamepad    = fGrl->boolToStr(ui->chkDat_gamepad->isChecked());
	str = ui->cbxDat_rating->itemData(ui->cbxDat_rating->currentIndex()).toString();
	DatosJuego.rating = ui->cbxDat_rating->currentIndex() > -1 ? str : "0";
	str =  ui->cbxDat_edad_recomendada->itemData(ui->cbxDat_edad_recomendada->currentIndex()).toString();
	DatosJuego.edad_recomendada = str.isEmpty() ? "nd" : str;
	DatosJuego.usuario          = ui->txtDat_usuario->text();
	DatosJuego.path_exe         = ui->txtDat_path_exe->text();
	DatosJuego.path_setup       = ui->txtDat_path_setup->text();
	DatosJuego.parametros_exe   = getParametrosTwList(ui->twDatosParametrosExe);
	DatosJuego.parametros_setup = getParametrosTwList(ui->twDatosParametrosSetup);

	if (isSoloDatos)
	{
		if (!Editando)
			DatosJuego.tipo_emu = TipoEmu.isEmpty() ? "datos" : TipoEmu;
	} else {
		if (Editando)
			isOk = sql->actualizaDatos(categoria.tabla, DatosJuego);
		else {
			DatosJuego.idgrl = sql->insertaDatos(categoria.tabla, DatosJuego);
			IdGame = DatosJuego.idgrl;
			isOk = DatosJuego.idgrl.isEmpty() ? false : true;
		}

		DatosJuego.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
		grlDir.DatosGame    = grlDir.DatosDbGrl + DatosJuego.game_dir +"/";

		if (isOk)
		{
			bool correcto = true;
			if (oldGameDir == DatosJuego.game_dir)
			{
				// Iguales....
			} else {
			// Distintos y renombramos.
				if (Editando)
				{
					mediaSound->closeMedia();
					mediaVideo->closeMedia();
				// Renombramos....
					correcto = fGrl->renombrarDirectorio(grlDir.DatosDbGrl + oldGameDir, grlDir.DatosGame);
				}
			}

			if (correcto)
			{
				if (!Editando)
				{
				//	No Editando
					DatosJuego.thumbs       = "";
					DatosJuego.cover_front  = "";
					DatosJuego.cover_back   = "";
					DatosJuego.cover_left   = "";
					DatosJuego.cover_right  = "";
					DatosJuego.cover_top    = "";
					DatosJuego.cover_bottom = "";

					fGrl->comprobarDirectorio(grlDir.DatosGame);
					fGrl->comprobarDirectorio(grlDir.DatosGame +"archivos/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"caja/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"capturas/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/small/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"sonidos/");
					fGrl->comprobarDirectorio(grlDir.DatosGame +"videos/");
				}

				if (!Thumbs.isEmpty() && QFile::exists(file_thumbs))
				{
					DatosJuego.thumbs = "thumbs." + grlCfg.thumb_format.toLower();
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "thumbs", DatosJuego.thumbs);
					if (!Editando)
						fGrl->crearThumbs(file_thumbs, grlDir.DatosGame + DatosJuego.thumbs, grlCfg.thumb_width, grlCfg.thumb_height, grlCfg.thumb_quality, true, grlCfg.thumb_format);
				}

				if (!CoverFront.isEmpty() && QFile::exists(file_cover_front))
				{
					DatosJuego.cover_front = "cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_front", DatosJuego.cover_front);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_front, grlDir.DatosGame +"caja/"+ DatosJuego.cover_front, true);
				}

				if (!CoverBack.isEmpty() && QFile::exists(file_cover_back))
				{
					DatosJuego.cover_back = "cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_back", DatosJuego.cover_back);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_back, grlDir.DatosGame +"caja/"+ DatosJuego.cover_back, true);
				}

				if (!CoverLeft.isEmpty() && QFile::exists(file_cover_left))
				{
					DatosJuego.cover_left = "cover_left"+ fGrl->getInfoFile(file_cover_left).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_left", DatosJuego.cover_left);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_left, grlDir.DatosGame +"caja/"+ DatosJuego.cover_left, true);
				}

				if (!CoverRight.isEmpty() && QFile::exists(file_cover_right))
				{
					DatosJuego.cover_right = "cover_right"+ fGrl->getInfoFile(file_cover_right).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_right", DatosJuego.cover_right);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_right, grlDir.DatosGame +"caja/"+ DatosJuego.cover_right, true);
				}

				if (!CoverTop.isEmpty() && QFile::exists(file_cover_top))
				{
					DatosJuego.cover_top = "cover_top"+ fGrl->getInfoFile(file_cover_top).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_top", DatosJuego.cover_top);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_top, grlDir.DatosGame +"caja/"+ DatosJuego.cover_top, true);
				}

				if (!CoverBottom.isEmpty() && QFile::exists(file_cover_bottom))
				{
					DatosJuego.cover_bottom = "cover_bottom"+ fGrl->getInfoFile(file_cover_bottom).Ext;
					sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_bottom", DatosJuego.cover_bottom);
					if (!Editando)
						fGrl->copiarArchivo(file_cover_bottom, grlDir.DatosGame +"caja/"+ DatosJuego.cover_bottom, true);
				}

				if (!Editando)
				{
					listImagenesImportadas << listImagenesAdd;

				// Copiamos las imagenes si se ha importado
					if (listImagenesImportadas.size() > 0)
					{
						stDatosImagenes imagen;
						const int numImagenes = listImagenesImportadas.size();
						for (int i = 0; i < numImagenes; ++i)
						{
							imagen = listImagenesImportadas.at(i);

							if (imagen.isImport)
								fGrl->copiarArchivo(imagen.dir_in +"imagenes/"+ imagen.nombre, grlDir.DatosGame + imagen.dir_out + imagen.nombre, true);
							else
								fGrl->copiarArchivo(imagen.dir_in + imagen.nombre, grlDir.DatosGame + imagen.dir_out + imagen.nombre, true);

							if (imagen.crearThumbs)
								fGrl->crearThumbs(grlDir.DatosGame + imagen.dir_out + imagen.nombre, grlDir.DatosGame + imagen.dir_out +"small/"+ imagen.nombre +".jpg", grlCfg.thumb_img_width, grlCfg.thumb_img_height, grlCfg.thumb_img_quality);
						}
					}

					sql->insertaFiles(ui->twDatosFiles, IdGame, categoria.id);
					sql->insertaURL(ui->twDatosURL, IdGame, categoria.id);
				}
			} else {
				QMessageBox::information(this, categoria.titulo, tr("No se ha renombrado correctamente"));
				ui->txtDat_titulo->setText(oldTitulo);
				isOk = false;
			}
		}
	}

	return isOk;
}

void frmAddEditJuego::asignarImagen(stFileInfo f_info, tipoImagen tipo)
{
	switch (tipo)
	{
		case t_thumb:
			file_thumbs       = f_info.FilePath;
			Thumbs            = "thumbs." + grlCfg.thumb_format.toLower();
			grlCfg.Img_Thumbs = fGrl->setDirRelative(f_info.Path);

			ui->lbDat_thumbs->setPixmap(QPixmap(file_thumbs));
			ui->btnDat_thumbs_ver->setEnabled(true);
			ui->btnDat_thumbs_eliminar->setEnabled(true);

			if (Editando)
			{
				if (file_thumbs != grlDir.DatosGame + Thumbs)
				{
					if (oldThumbs != Thumbs)
						fGrl->eliminarArchivo(grlDir.DatosGame + oldThumbs);
					fGrl->crearThumbs(file_thumbs, grlDir.DatosGame + Thumbs, grlCfg.thumb_width, grlCfg.thumb_height, grlCfg.thumb_quality, true, grlCfg.thumb_format);
					oldThumbs = Thumbs;
				}
			}
			break;
		case t_cover_front:
			file_cover_front      = f_info.FilePath;
			CoverFront            = "cover_front"+ f_info.Ext;
			grlCfg.Img_CoverFront = fGrl->setDirRelative(f_info.Path);

			ui->lbDat_cover_front->setPixmap(QPixmap(file_cover_front));
			ui->btnDat_cover_front_ver->setEnabled(true);
			ui->btnDat_cover_front_eliminar->setEnabled(true);

			if (Editando)
				fGrl->copiarArchivo(file_cover_front, grlDir.DatosGame +"caja/"+ CoverFront, false, true);
			break;
		case t_cover_back:
			file_cover_back      = f_info.FilePath;
			CoverBack            = "cover_back"+ f_info.Ext;
			grlCfg.Img_CoverBack = fGrl->setDirRelative(f_info.Path);

			ui->lbDat_cover_back->setPixmap(QPixmap(file_cover_back));
			ui->btnDat_cover_back_ver->setEnabled(true);
			ui->btnDat_cover_back_eliminar->setEnabled(true);

			if (Editando)
				fGrl->copiarArchivo(file_cover_back, grlDir.DatosGame +"caja/"+ CoverBack, false, true);
			break;
		case t_cover_left:
				file_cover_left      = f_info.FilePath;
				CoverLeft            = "cover_left"+ f_info.Ext;
				grlCfg.Img_CoverLeft = fGrl->setDirRelative(f_info.Path);

				ui->lbDat_cover_left->setPixmap(QPixmap(file_cover_left));
				ui->btnDat_cover_left_ver->setEnabled(true);
				ui->btnDat_cover_left_eliminar->setEnabled(true);

				if (Editando)
					fGrl->copiarArchivo(file_cover_left, grlDir.DatosGame +"caja/"+ CoverLeft, false, true);
			break;
		case t_cover_right:
				file_cover_right      = f_info.FilePath;
				CoverRight            = "cover_right"+ f_info.Ext;
				grlCfg.Img_CoverRight = fGrl->setDirRelative(f_info.Path);

				ui->lbDat_cover_right->setPixmap(QPixmap(file_cover_right));
				ui->btnDat_cover_right_ver->setEnabled(true);
				ui->btnDat_cover_right_eliminar->setEnabled(true);

				if (Editando)
					fGrl->copiarArchivo(file_cover_right, grlDir.DatosGame +"caja/"+ CoverRight, false, true);
			break;
		case t_cover_top:
			file_cover_top      = f_info.FilePath;
			CoverTop            = "cover_top"+ f_info.Ext;
			grlCfg.Img_CoverTop = fGrl->setDirRelative(f_info.Path);

			ui->lbDat_cover_top->setPixmap(QPixmap(file_cover_top));
			ui->btnDat_cover_top_ver->setEnabled(true);
			ui->btnDat_cover_top_eliminar->setEnabled(true);

			if (Editando)
				fGrl->copiarArchivo(file_cover_top, grlDir.DatosGame +"caja/"+ CoverTop, false, true);
			break;
		case t_cover_bottom:
			file_cover_bottom      = f_info.FilePath;
			CoverBottom            = "cover_bottom"+ f_info.Ext;
			grlCfg.Img_CoverBottom = fGrl->setDirRelative(f_info.Path);

			ui->lbDat_cover_bottom->setPixmap(QPixmap(file_cover_bottom));
			ui->btnDat_cover_bottom_ver->setEnabled(true);
			ui->btnDat_cover_bottom_eliminar->setEnabled(true);

			if (Editando)
				fGrl->copiarArchivo(file_cover_bottom, grlDir.DatosGame +"caja/"+ CoverBottom, false, true);
			break;
		case t_imagenes:
		case t_capturas:
			stDatosImagenes imagen;
			imagen.isImport    = false;
			imagen.crearThumbs = tipo == t_capturas ? false : true;
			imagen.nombre      = f_info.NameExt;
			imagen.dir_in      = f_info.FilePath;
			imagen.dir_out     = tipo == t_capturas ? "capturas/" : "imagenes/";

			QString img_mini, img_out;
			if (Editando)
			{
				fGrl->copiarArchivo(f_info.FilePath, grlDir.DatosGame + imagen.dir_out + imagen.nombre, false, true);

				img_out  = grlDir.DatosGame + imagen.dir_out + imagen.nombre;

				if (tipo == t_capturas)
					img_mini = img_out;
				else
					img_mini = grlDir.DatosGame + imagen.dir_out +"small/"+ imagen.nombre +".jpg";

				if (imagen.crearThumbs)
					fGrl->crearThumbs(f_info.FilePath, img_mini, grlCfg.thumb_img_width, grlCfg.thumb_img_height, grlCfg.thumb_img_quality);
			} else {
				img_out  = imagen.dir_in;
				img_mini = imagen.dir_in;
				listImagenesAdd.append(imagen);
			}

			QListWidgetItem *item = new QListWidgetItem;
			item->setData(Qt::UserRole, img_out);
			if (QFile::exists(img_mini))
				item->setIcon(QIcon(img_mini));
			else
				item->setIcon(QIcon(fGrl->theme() +"images/juego_sin_imagen.png"));

			if (tipo == t_capturas)
				ui->lwCapturas->addItem(item);
			else
				ui->lwImagenes->addItem(item);
		break;
	}
}

void frmAddEditJuego::on_btnOk_clicked()
{
	bool siguiente = true;

	if (ui->txtDat_titulo->text().isEmpty())
	{
		siguiente = false;
		QMessageBox::information(this, categoria.titulo, tr("Debes poner un titulo al juego"));
	} else {
		DatosJuego.tipo_emu = TipoEmu.isEmpty() ? "datos" : TipoEmu;

		if (TipoEmu == "dosbox")
		{
			wDbx->setDatosDosBox();
			siguiente = wDbx->isCorrectNext();
		}

		if (TipoEmu == "scummvm")
		{
			wSvm->setDatosScummVM();
			siguiente = wSvm->isCorrectNext();
		}

		if (TipoEmu == "vdmsound")
		{
			wVdms->setDatosVDMSound();
			siguiente = wVdms->isCorrectNext();
		}

		if (siguiente)
		{
			siguiente = setDatosJuegos();

			if (siguiente)
			{
				if (TipoEmu == "dosbox")
				{
					stConfigDOSBox DatosDosBox = wDbx->getDatosDosBox();

					grlCfg.Dbx_path_exe        = wDbx->getGrlCfg().Dbx_path_exe;
					grlCfg.Dbx_path_setup      = wDbx->getGrlCfg().Dbx_path_setup;
					grlCfg.Montaje_path        = wDbx->getGrlCfg().Montaje_path;
					grlCfg.Dbx_sdl_mapperfile  = wDbx->getGrlCfg().Dbx_sdl_mapperfile;
					grlCfg.Dbx_dosbox_language = wDbx->getGrlCfg().Dbx_dosbox_language;

					QList<stConfigDOSBoxMount> list_mount = wDbx->getListMount();

					if (Editando)
					{
						sql->actualizaDbx(DatosDosBox);
					} else {
						DatosDosBox.idgrl = DatosJuego.idgrl;
						DatosDosBox.idcat = categoria.id;
						DatosDosBox.id    = sql->insertaDbx(DatosDosBox);
						sql->insertaMontajesDbx(list_mount, DatosDosBox.id);
					}

					fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, list_mount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox.conf");
					fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, list_mount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox-setup.conf", true);
				}

				if (TipoEmu == "scummvm")
				{
					stConfigScummVM DatosScummVM = wSvm->getDatosScummVM();

					grlCfg.Svm_path       = wSvm->getGrlCfg().Svm_path;
					grlCfg.Svm_path_save  = wSvm->getGrlCfg().Svm_path_save;
					grlCfg.Svm_path_extra = wSvm->getGrlCfg().Svm_path_extra;
					grlCfg.Svm_soundfont  = wSvm->getGrlCfg().Svm_soundfont;

					if (Editando)
						sql->actualizaSvm(DatosScummVM);
					else {
						DatosScummVM.idgrl = DatosJuego.idgrl;
						DatosScummVM.idcat = categoria.id;
						DatosScummVM.id    = sql->insertaSvm(DatosScummVM);
					}

					DatosScummVM.emu_svmpath = fGrl->getInfoFile(fGrl->getDirRelative(grlCfg.DirScummVM)).Path;
					fGrl->creaIniScummVM(DatosScummVM, grlDir.DatosGame +"scummvm.ini");
				}

				if (TipoEmu == "vdmsound")
				{
					stConfigVDMSound DatosVDMSound = wVdms->getDatosVDMSound();

					grlCfg.Vdms_path_exe = wVdms->getGrlCfg().Vdms_path_exe;
					grlCfg.Vdms_icon     = wVdms->getGrlCfg().Vdms_icon;

					if (Editando)
						sql->actualizaVdms(DatosVDMSound);
					else {
						DatosVDMSound.idgrl = DatosJuego.idgrl;
						DatosVDMSound.idcat = categoria.id;
						DatosVDMSound.id    = sql->insertaVdms(DatosVDMSound);
					}

					fGrl->crearArchivoConfigVdmS(DatosVDMSound, grlDir.DatosGame +"vdmsound.vlp");
				}

				sql->comit();
				QDialog::accept();
			}
		}
	}
}

void frmAddEditJuego::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmAddEditJuego::on_btnDescargarInfo_clicked()
{
	setDatosJuegos(true);
	wDbx->setDatosDosBox();
	wSvm->setDatosScummVM();
	wVdms->setDatosVDMSound();

	frmImportarJuego *importarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, wDbx->getDatosDosBox(), wSvm->getDatosScummVM(), wVdms->getDatosVDMSound(), false, Editando, this);
	importarJuego->setWindowFlags(Qt::Window);

	if (importarJuego->exec() == QDialog::Accepted)
	{
		grlCfg = importarJuego->getGrlCfg();

		if (importarJuego->getDatosImport().size() > 0)
		{
			stImport import = importarJuego->getDatosImport().at(0);
			stDatosJuego datos = import.datos;
			listImagenesImportadas = import.imagenes;

			if (!Editando)
			{
				int row_tipo_emu = ui->cbxDat_tipo_emu->findData(datos.tipo_emu);
				if (row_tipo_emu < 0) row_tipo_emu = 0;
				ui->cbxDat_tipo_emu->setCurrentIndex(row_tipo_emu);
				emit on_cbxDat_tipo_emu_activated(row_tipo_emu);
			}

			cargarDatosJuego(datos, true);

			if (!import.solo_datos && TipoEmu == "dosbox")
			{
				wDbx->cargarDatosDosBox(import.dosbox);
				wDbx->cargarDatosDBxMontaje(import.dbxMount);
			}

			if (!import.solo_datos && TipoEmu == "scummvm")
			{
				wSvm->cargarDatosScummVM(import.scummvm);
			}

			if (!import.solo_datos && TipoEmu == "vdmsound")
			{
				wVdms->cargarDatosVDMSound(import.vdmsound);
			}
		}
	} else {
		grlCfg = importarJuego->getGrlCfg();
	}

	delete importarJuego;
}

void frmAddEditJuego::on_cbxDat_tipo_emu_activated(int index)
{
	TipoEmu = ui->cbxDat_tipo_emu->itemData(index).toString();
	ui->tabw_Datos->setTabEnabled(tabEjecutabeRom, true);

	if (TipoEmu.isEmpty())
		TipoEmu = "datos";

	if (TipoEmu == "dosbox")
	{
		ui->tabw_Datos->setTabEnabled(tabEjecutabeRom, false);
		ui->tabw_Datos->setTabEnabled(tabDOSBox  , true);
		ui->tabw_Datos->setTabEnabled(tabScummVM , false);
		ui->tabw_Datos->setTabEnabled(tabVDMSound, false);
	}
	else if (TipoEmu == "scummvm")
	{
		ui->tabw_Datos->setTabEnabled(tabEjecutabeRom, false);
		ui->tabw_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabw_Datos->setTabEnabled(tabScummVM , true);
		ui->tabw_Datos->setTabEnabled(tabVDMSound, false);
	}
	else if (TipoEmu == "vdmsound")
	{
		ui->tabw_Datos->setTabEnabled(tabEjecutabeRom, false);
		ui->tabw_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabw_Datos->setTabEnabled(tabScummVM , false);
		ui->tabw_Datos->setTabEnabled(tabVDMSound, true);
	} else {
		ui->tabw_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabw_Datos->setTabEnabled(tabScummVM , false);
		ui->tabw_Datos->setTabEnabled(tabVDMSound, false);
	}

	int row_icono = ui->cbxDat_icono->findData(TipoEmu, Qt::UserRole, Qt::MatchContains);
	if (row_icono < 0) row_icono = 0;
	ui->cbxDat_icono->setCurrentIndex(row_icono);
}

void frmAddEditJuego::on_btnDat_grupo_add_clicked()
{
	QDialog *grupoAdd = new QDialog(this);
	grupoAdd->resize(340, 100);
	grupoAdd->setMinimumSize(QSize(340, 100));
	grupoAdd->setWindowIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	grupoAdd->setWindowTitle(tr("Añadir grupo"));

	QVBoxLayout *vLayout = new QVBoxLayout(grupoAdd);
	vLayout->setContentsMargins(4, 4, 4, 4);
		QHBoxLayout *hLayout_1 = new QHBoxLayout();
			QLabel *lb_grupo_titulo = new QLabel(grupoAdd);
			lb_grupo_titulo->setText(tr("Grupo") +":");
		hLayout_1->addWidget(lb_grupo_titulo);
			QLineEdit *txt_grupo_titulo = new QLineEdit(grupoAdd);
		hLayout_1->addWidget(txt_grupo_titulo);
	vLayout->addLayout(hLayout_1);
		QHBoxLayout *hLayout_2 = new QHBoxLayout();
			QLabel *lb_grupo_imagen = new QLabel(grupoAdd);
			lb_grupo_imagen->setText(tr("Imagen") +":");
		hLayout_2->addWidget(lb_grupo_imagen);
		//	QGroupComboBox *cbx_grupo_img = new QGroupComboBox(grupoAdd);
			QComboBox *cbx_grupo_img = new QComboBox(grupoAdd);

		//	cbx_grupo_img->clear();
		//	cbx_grupo_img->addItemParent("Imágenes Categorías");
			fGrl->cargarIconosComboBox(cbx_grupo_img, fGrl->theme(), "img16/cat/", "cat/", "", grlCfg.FormatsImage.join(";"));
		//	cbx_grupo_img->addItemParent("Imágenes defecto");
			fGrl->cargarIconosComboBox(cbx_grupo_img, fGrl->theme(), "img16/", "", "sinimg.png", grlCfg.FormatsImage.join(";"), false);

		//	int row = cbx_grupo_img->findData("sinimg.png", Qt::UserRole, Qt::MatchContains);
		//	cbx_grupo_img->setCurrentIndex(row);

			hLayout_2->addWidget(cbx_grupo_img);
			QSpacerItem *hSpacer_grupo_0 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hLayout_2->addItem(hSpacer_grupo_0);
	vLayout->addLayout(hLayout_2);
		QHBoxLayout *hLayout_3 = new QHBoxLayout();
			QSpacerItem *hSpacer_grupo_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hLayout_3->addItem(hSpacer_grupo_1);
			QPushButton *btn_grupo_add = new QPushButton(grupoAdd);
			btn_grupo_add->setText(tr("Añadir"));
		hLayout_3->addWidget(btn_grupo_add);
			QPushButton *btn_grupo_cancel = new QPushButton(grupoAdd);
			btn_grupo_cancel->setText(tr("Cancelar"));
		hLayout_3->addWidget(btn_grupo_cancel);
			QSpacerItem *hSpacer_grupo_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hLayout_3->addItem(hSpacer_grupo_2);
	vLayout->addLayout(hLayout_3);

	connect(btn_grupo_add, SIGNAL(clicked()), grupoAdd, SLOT(accept()));
	connect(btn_grupo_cancel , SIGNAL(clicked()), grupoAdd, SLOT(reject()));
	grupoAdd->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, grupoAdd->size(), qApp->desktop()->availableGeometry()));

	if (grupoAdd->exec() == QDialog::Accepted)
	{
		QString imagen = cbx_grupo_img->itemData(cbx_grupo_img->currentIndex()).toString();
		cbxDat_grupo->addItem(QIcon(fGrl->theme() +"img16/"+ imagen), txt_grupo_titulo->text(), txt_grupo_titulo->text());

		QString txt_in = fGrl->leerArchivo(grlDir.Datos +"grupos.txt", "UTF-8");
		QFile file_out(grlDir.Datos +"grupos.txt");
		if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QTextStream out(&file_out);
			out.setCodec("UTF-8");
			out << txt_in;
			out << txt_grupo_titulo->text() << "|" << imagen << endl;
			out.flush();
		}
		file_out.close();
	}
}

// Thumbs
void frmAddEditJuego::on_btnDat_thumbs_crear_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(file_cover_front);

	if (f_info.Exists)
		asignarImagen(f_info, t_thumb);
}

void frmAddEditJuego::on_btnDat_thumbs_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_Thumbs, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_thumb);
}

void frmAddEditJuego::on_btnDat_thumbs_ver_clicked()
{
	if (!Thumbs.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_thumbs);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_thumbs_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame + Thumbs);

	Thumbs = "";
	file_thumbs = "";
	ui->lbDat_thumbs->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_thumbs_ver->setEnabled(false);
	ui->btnDat_thumbs_eliminar->setEnabled(false);
}

void frmAddEditJuego::on_btnDat_thumbs_crear__clicked()
{
	emit on_btnDat_thumbs_crear_clicked();
}

// CoverFront
void frmAddEditJuego::on_btnDat_cover_front_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverFront, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_front);
}

void frmAddEditJuego::on_btnDat_cover_front_ver_clicked()
{
	if (!CoverFront.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_front);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_front_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverFront);

	CoverFront = "";
	file_cover_front = "";
	ui->lbDat_cover_front->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_front_ver->setEnabled(false);
	ui->btnDat_cover_front_eliminar->setEnabled(false);
}

// CoverBack
void frmAddEditJuego::on_btnDat_cover_back_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverBack, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_back);
}

void frmAddEditJuego::on_btnDat_cover_back_ver_clicked()
{
	if (!CoverBack.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_back);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_back_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverBack);

	CoverBack = "";
	file_cover_back = "";
	ui->lbDat_cover_back->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_back_ver->setEnabled(false);
	ui->btnDat_cover_back_eliminar->setEnabled(false);
}

// CoverLeft
void frmAddEditJuego::on_btnDat_cover_left_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverLeft, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_left);
}

void frmAddEditJuego::on_btnDat_cover_left_ver_clicked()
{
	if (!CoverLeft.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_left);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_left_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverLeft);

	CoverLeft = "";
	file_cover_left = "";
	ui->lbDat_cover_left->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_left_ver->setEnabled(false);
	ui->btnDat_cover_left_eliminar->setEnabled(false);
}

// CoverRight
void frmAddEditJuego::on_btnDat_cover_right_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverRight, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_right);
}

void frmAddEditJuego::on_btnDat_cover_right_ver_clicked()
{
	if (!CoverRight.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_right);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_right_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverRight);

	CoverRight = "";
	file_cover_right = "";
	ui->lbDat_cover_right->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_right_ver->setEnabled(false);
	ui->btnDat_cover_right_eliminar->setEnabled(false);
}

// CoverTop
void frmAddEditJuego::on_btnDat_cover_top_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverTop, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_top);
}

void frmAddEditJuego::on_btnDat_cover_top_ver_clicked()
{
	if (!CoverTop.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_top);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_top_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverTop);

	CoverTop = "";
	file_cover_top = "";
	ui->lbDat_cover_top->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_top_ver->setEnabled(false);
	ui->btnDat_cover_top_eliminar->setEnabled(false);
}

// CoverBottom
void frmAddEditJuego::on_btnDat_cover_bottom_abrir_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_CoverBottom, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_cover_bottom);
}

void frmAddEditJuego::on_btnDat_cover_bottom_ver_clicked()
{
	if (!CoverBottom.isEmpty())
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		grdap->cargarImagen(file_cover_bottom);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_cover_bottom_eliminar_clicked()
{
	if (Editando)
		fGrl->eliminarArchivo(grlDir.DatosGame +"caja/"+ CoverBottom);

	CoverBottom = "";
	file_cover_bottom = "";
	ui->lbDat_cover_bottom->setPixmap(QPixmap(fGrl->theme() +"images/juego_sin_imagen.png"));
	ui->btnDat_cover_bottom_ver->setEnabled(false);
	ui->btnDat_cover_bottom_eliminar->setEnabled(false);
}

// Multimedia
void frmAddEditJuego::on_lwImagenes_itemDoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		QStringList lista;
		const int listSize = ui->lwImagenes->count();
		for (int i = 0; i < listSize; ++i)
			lista << ui->lwImagenes->item(i)->data(Qt::UserRole).toString();
		int id_imagen = lista.indexOf(item->data(Qt::UserRole).toString());

		grdap->cargarImagenes(lista, (id_imagen != -1 ? id_imagen : 0), true);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_imagenes_add_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_Imagenes, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_imagenes);
}

void frmAddEditJuego::on_btnDat_imagenes_eliminar_clicked()
{
	if (ui->lwImagenes->count() > 0 && ui->lwImagenes->currentRow() != -1)
	{
		stFileInfo f_info = fGrl->getInfoFile(ui->lwImagenes->currentItem()->data(Qt::UserRole).toString());

		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar la imagen seleccionada?"))
		{
			if (Editando)
			{
				if (fGrl->eliminarArchivo(grlDir.DatosGame +"imagenes/"+ f_info.NameExt))
				{
					if (fGrl->eliminarArchivo(grlDir.DatosGame +"imagenes/small/"+ f_info.NameExt +".jpg"))
						delete ui->lwImagenes->currentItem();
				}
			} else {
				const int numImgAdd = listImagenesAdd.size();
				bool encontrado = false;
				int i = 0;
				int idx_del = -1;
				while (i < numImgAdd && !encontrado)
				{
					if (listImagenesAdd.at(i).dir_in == f_info.FilePath)
					{
						idx_del = i;
						encontrado = true;
					}
					if (!encontrado)
						++i;
				}

				if (idx_del > -1)
				{
					listImagenesAdd.removeAt(idx_del);
					delete ui->lwImagenes->currentItem();
				}
			}
		}
	}
}

void frmAddEditJuego::on_btnDat_imagenes_asignar_clicked()
{
	if (ui->lwImagenes->count() > 0 && ui->lwImagenes->currentRow() != -1)
	{
		stFileInfo f_info = fGrl->getInfoFile(ui->lwImagenes->currentItem()->data(Qt::UserRole).toString());
		if (f_info.Exists)
		{
			switch (ui->cbxDat_imagenes_asignar_como->itemData(ui->cbxDat_imagenes_asignar_como->currentIndex()).toInt())
			{
				case 0: asignarImagen(f_info, t_thumb); break;
				case 1: asignarImagen(f_info, t_cover_front); break;
				case 2: asignarImagen(f_info, t_cover_back); break;
				case 3: asignarImagen(f_info, t_cover_left); break;
				case 4: asignarImagen(f_info, t_cover_right); break;
				case 5: asignarImagen(f_info, t_cover_top); break;
				case 6: asignarImagen(f_info, t_cover_bottom); break;
			}
		}
	}
}

void frmAddEditJuego::on_lwCapturas_itemDoubleClicked(QListWidgetItem *item)
{
	if (item)
	{
		if (!isOpenGrDap)
		{
			isOpenGrDap = true;
			grdap = new GrDap(this);
		}

		QStringList lista;
		const int listSize = ui->lwCapturas->count();
		for (int i = 0; i < listSize; ++i)
			lista << ui->lwCapturas->item(i)->data(Qt::UserRole).toString();
		int id_imagen = lista.indexOf(item->data(Qt::UserRole).toString());

		grdap->cargarImagenes(lista, (id_imagen != -1 ? id_imagen : 0), true);
		grdap->show();
	}
}

void frmAddEditJuego::on_btnDat_capturas_add_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Img_Imagenes, "", tr("Imágenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
		asignarImagen(f_info, t_capturas);
}

void frmAddEditJuego::on_btnDat_capturas_eliminar_clicked()
{
	int pos = ui->lwCapturas->currentRow();
	if (ui->lwCapturas->count() > 0 && pos > -1 && ui->lwCapturas->currentItem()->isSelected())
	{
		stFileInfo f_info = fGrl->getInfoFile(ui->lwCapturas->currentItem()->data(Qt::UserRole).toString());

		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar la captura seleccionada?"))
		{
			if (Editando)
			{
				if (fGrl->eliminarArchivo(grlDir.DatosGame +"capturas/"+ f_info.NameExt))
					delete ui->lwCapturas->currentItem();
			} else {
/*				const int numImgAdd = listImagenesAdd.size();
				bool encontrado = false;
				int i = 0;
				int idx_del = -1;
				while (i < numImgAdd && !encontrado)
				{
//qDebug() << "dir_in  : " << listImagenesAdd.at(i).dir_in;
//qDebug() << "FilePath: " << f_info.FilePath;
//qDebug() << "--------------------";
					if (listImagenesAdd.at(i).dir_in == f_info.FilePath)
					{
//qDebug() << "Encontrado : " << i;
						idx_del = i;
						encontrado = true;
					}
					if (!encontrado)
						++i;
				}
//qDebug() << "idx_del: " << idx_del;
				if (idx_del > -1)
				{
					listImagenesAdd.removeAt(idx_del);
					delete ui->lwImagenes->currentItem();
				}*/
			}
		}
	}
}

void frmAddEditJuego::on_btnDat_videos_add_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Datos_Videos, "", tr("Videos soportados") +" ("+ grlCfg.FormatsVideo.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		if (Editando)
		{
			if (fGrl->copiarArchivo(f_info.FilePath, grlDir.DatosGame +"videos/"+ f_info.NameExt, false, false, true))
				mediaVideo->addPlaylist(f_info.FilePath);
		} else
			listVideosAdd << f_info.FilePath;
	}
}

void frmAddEditJuego::on_btnDat_videos_eliminar_clicked()
{
	int pos = mediaVideo->currentRow();
	if (mediaVideo->count() > 0 && pos > -1)
	{
		stFileInfo f_info = fGrl->getInfoFile(mediaVideo->getFileName());

		qDebug() << "f_info Path    : " << f_info.Path;
		qDebug() << "f_info NameExt : " << f_info.NameExt;
		qDebug() << "f_info FilePath: " << f_info.FilePath;

		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar el video seleccionado?"))
		{
			if (Editando)
			{
				mediaVideo->removeSelectMedia();
				fGrl->eliminarArchivo(f_info.FilePath);
			} else {
/*				const int numVideoAdd = listVideosAdd.size();
				bool encontrado = false;
				int i = 0;
				int idx_del = -1;
				while (i < numVideoAdd && !encontrado)
				{
//qDebug() << "dir_in  : " << listVideosAdd.at(i);
//qDebug() << "FilePath: " << f_info.FilePath;
//qDebug() << "--------------------";
					if (listVideosAdd.at(i) == f_info.FilePath)
					{
//qDebug() << "Encontrado : " << i;
						idx_del = i;
						encontrado = true;
					}
					if (!encontrado)
						++i;
				}
//qDebug() << "idx_del: " << idx_del;
				if (idx_del > -1)
				{
					listVideosAdd.removeAt(idx_del);
					mediaVideo->removeSelectMedia();
				}
*/
			}
		}
	}
}

void frmAddEditJuego::on_btnDat_sonidos_add_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Datos_Sonidos, "", tr("Sonidos soportados") +" ("+ grlCfg.FormatsMusic.join(" ") +");;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		if (Editando)
		{
			if (fGrl->copiarArchivo(f_info.FilePath, grlDir.DatosGame +"sonidos/"+ f_info.NameExt, false, false, true))
				mediaSound->addPlaylist(f_info.FilePath);
		} else
			listSonidosAdd << f_info.FilePath;
	}
}

void frmAddEditJuego::on_btnDat_sonidos_eliminar_clicked()
{
	int pos = mediaSound->currentRow();
	if (mediaSound->count() > 0 && pos > -1)
	{
		stFileInfo f_info = fGrl->getInfoFile(mediaSound->getFileName());

		qDebug() << "f_info Path    : " << f_info.Path;
		qDebug() << "f_info NameExt : " << f_info.NameExt;
		qDebug() << "f_info FilePath: " << f_info.FilePath;

		if (fGrl->questionMsg(tr("¿Eliminar...?"), "¿Deseas eliminar el sonido seleccionado?"))
		{
			if (Editando)
			{
				mediaSound->removeSelectMedia();
				fGrl->eliminarArchivo(f_info.FilePath);
			} else {
				//
			}
		}
	}
}

void frmAddEditJuego::changeMediaConfig()
{
	grlCfg.VerPlaylistVideo = mediaVideo->getGrlCfg().VerPlaylistVideo;
	grlCfg.VerPlaylistSound = mediaSound->getGrlCfg().VerPlaylistSound;
	grlCfg.VerInfoMedia     = mediaSound->getGrlCfg().VerInfoMedia;
}

// Otros Datos
void frmAddEditJuego::cargarParametrosTwList(QTreeWidget *twList, QString parametros)
{
	QStringList list_parametros = parametros.split("|:|", QString::SkipEmptyParts);
	const int listParametrosSize = list_parametros.size();

	for (int i = 0; i < listParametrosSize; ++i)
	{
		QStringList parametro = list_parametros.at(i).split("|", QString::SkipEmptyParts);
		const int num_parm = parametro.size();

		QTreeWidgetItem *item = new QTreeWidgetItem;

		if (num_parm > 0)
			item->setText(0, parametro.at(0));
		else
			item->setText(0, "");

		if (num_parm > 1)
			item->setText(1, parametro.at(1));
		else
			item->setText(1, "");

		twList->addTopLevelItem(item);
	}
}

QString frmAddEditJuego::getParametrosTwList(QTreeWidget *twList)
{
	QStringList parametros;

	const int count = twList->topLevelItemCount();
	if (count > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			QTreeWidgetItem *item = twList->topLevelItem(i);
			parametros << item->text(0) +"|"+ item->text(1);
		}
	}

	return ""+ parametros.join("|:|");
}

void frmAddEditJuego::on_btnDat_path_exe_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.DatosFiles_PathExe, "", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		ui->txtDat_path_exe->setText(fGrl->setDirRelative(f_info.FilePath));
		grlCfg.DatosFiles_PathExe = fGrl->setDirRelative(f_info.Path);
	}
}

void frmAddEditJuego::on_btnDat_path_exe_clear_clicked()
{
	ui->txtDat_path_exe->clear();
}

void frmAddEditJuego::on_txtDat_parametros_exe_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twDatosParametrosExe, ui->btnDat_parametros_exe_update, arg1, 0);
}

void frmAddEditJuego::on_txtDat_parametros_exe_valor_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twDatosParametrosExe, ui->btnDat_parametros_exe_update, arg1, 1);
}

void frmAddEditJuego::on_btnDat_parametros_exe_add_clicked()
{
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setText(0, ui->txtDat_parametros_exe->text());
	item->setText(1, ui->txtDat_parametros_exe_valor->text());
	ui->twDatosParametrosExe->addTopLevelItem(item);

	ui->txtDat_parametros_exe->setText("");
	ui->txtDat_parametros_exe_valor->setText("");
}

void frmAddEditJuego::on_btnDat_parametros_exe_update_clicked()
{
	int pos = ui->twDatosParametrosExe->indexOfTopLevelItem(ui->twDatosParametrosExe->currentItem());
	if (ui->twDatosParametrosExe->topLevelItemCount() > 0 && pos != -1)
	{
		QTreeWidgetItem *item_parametros_exe = ui->twDatosParametrosExe->currentItem();
		item_parametros_exe->setText(0, ui->txtDat_parametros_exe->text());
		item_parametros_exe->setText(1, ui->txtDat_parametros_exe_valor->text());

		ui->txtDat_parametros_exe->setText("");
		ui->txtDat_parametros_exe_valor->setText("");
		ui->btnDat_parametros_exe_update->setEnabled(false);
	}
}

void frmAddEditJuego::on_btnDat_parametros_exe_eliminar_clicked()
{
	int pos = ui->twDatosParametrosExe->indexOfTopLevelItem(ui->twDatosParametrosExe->currentItem());
	if (ui->twDatosParametrosExe->topLevelItemCount() > 0 && pos != -1)
	{
		fGrl->deleteItemTree(ui->twDatosParametrosExe->currentItem());

		ui->txtDat_parametros_exe->setText("");
		ui->txtDat_parametros_exe_valor->setText("");
		ui->btnDat_parametros_exe_update->setEnabled(false);
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione elemento de la lista para eliminarlo"));
}

void frmAddEditJuego::on_btnDat_parametros_exe_clear_clicked()
{
	ui->twDatosParametrosExe->clear();
	ui->txtDat_parametros_exe->clear();
	ui->txtDat_parametros_exe->setText("");
	ui->txtDat_parametros_exe_valor->setText("");
	ui->btnDat_parametros_exe_update->setEnabled(false);
}

void frmAddEditJuego::on_twDatosParametrosExe_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->txtDat_parametros_exe->setText(item->text(0));
		ui->txtDat_parametros_exe_valor->setText(item->text(1));
	}
}

void frmAddEditJuego::on_btnDat_path_setup_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.DatosFiles_PathSetup, "", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		ui->txtDat_path_setup->setText(fGrl->setDirRelative(f_info.FilePath));
		grlCfg.DatosFiles_PathSetup = fGrl->setDirRelative(f_info.Path);
	}
}

void frmAddEditJuego::on_btnDat_path_setup_clear_clicked()
{
	ui->txtDat_path_setup->clear();
}

void frmAddEditJuego::on_txtDat_parametros_setup_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twDatosParametrosSetup, ui->btnDat_parametros_setup_update, arg1, 0);
}

void frmAddEditJuego::on_txtDat_parametros_setup_valor_textEdited(const QString &arg1)
{
	fGrl->enabledButtonUpdateTwList(ui->twDatosParametrosSetup, ui->btnDat_parametros_setup_update, arg1, 1);
}

void frmAddEditJuego::on_btnDat_parametros_setup_add_clicked()
{
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setText(0, ui->txtDat_parametros_setup->text());
	item->setText(1, ui->txtDat_parametros_setup_valor->text());
	ui->twDatosParametrosSetup->addTopLevelItem(item);

	ui->txtDat_parametros_setup->setText("");
	ui->txtDat_parametros_setup_valor->setText("");
}

void frmAddEditJuego::on_btnDat_parametros_setup_update_clicked()
{
	int pos = ui->twDatosParametrosSetup->indexOfTopLevelItem(ui->twDatosParametrosSetup->currentItem());
	if (ui->twDatosParametrosSetup->topLevelItemCount() > 0 && pos != -1)
	{
		int pos = ui->twDatosParametrosSetup->indexOfTopLevelItem(ui->twDatosParametrosSetup->currentItem());
		if (ui->twDatosParametrosSetup->topLevelItemCount() > 0 && pos != -1)
		{
			QTreeWidgetItem *item_parametros_setup = ui->twDatosParametrosSetup->currentItem();
			item_parametros_setup->setText(0, ui->txtDat_parametros_setup->text());
			item_parametros_setup->setText(1, ui->txtDat_parametros_setup_valor->text());

			ui->txtDat_parametros_setup->setText("");
			ui->txtDat_parametros_setup_valor->setText("");
			ui->btnDat_parametros_setup_update->setEnabled(false);
		}
	}
}

void frmAddEditJuego::on_btnDat_parametros_setup_eliminar_clicked()
{
	int pos = ui->twDatosParametrosSetup->indexOfTopLevelItem(ui->twDatosParametrosSetup->currentItem());
	if (ui->twDatosParametrosSetup->topLevelItemCount() > 0 && pos != -1)
	{
		fGrl->deleteItemTree(ui->twDatosParametrosSetup->currentItem());

		ui->txtDat_parametros_setup->setText("");
		ui->txtDat_parametros_setup_valor->setText("");
		ui->btnDat_parametros_setup_update->setEnabled(false);
	} else
		QMessageBox::information(this, tr("Opciones"), tr("Por favor seleccione elemento de la lista para eliminarlo"));
}

void frmAddEditJuego::on_btnDat_parametros_setup_clear_clicked()
{
	ui->twDatosParametrosSetup->clear();
	ui->txtDat_parametros_setup->clear();
	ui->txtDat_parametros_setup->setText("");
	ui->txtDat_parametros_setup_valor->setText("");
	ui->btnDat_parametros_setup_update->setEnabled(false);
}

void frmAddEditJuego::on_twDatosParametrosSetup_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->txtDat_parametros_setup->setText(item->text(0));
		ui->txtDat_parametros_setup_valor->setText(item->text(1));
	}
}

// Direcciones url
void frmAddEditJuego::enabledUrlUpdate(QString texto, int col)
{
	int pos = ui->twDatosURL->indexOfTopLevelItem(ui->twDatosURL->currentItem());
	if (ui->twDatosURL->topLevelItemCount() > 0 && pos != -1)
	{
		if (texto != ui->twDatosURL->currentItem()->text(col))
			ui->btnUrl_update->setEnabled(true);
		else
			ui->btnUrl_update->setEnabled(false);
	} else
		ui->btnUrl_update->setEnabled(false);
}

void frmAddEditJuego::addEditTwDatosURL(bool isNew)
{
	if (!ui->cbxUrl_url->currentText().isEmpty())
	{
		stDatosUrls datos_url;
		datos_url.id          = "";
		datos_url.idgrl       = IdGame;
		datos_url.idcat       = categoria.id;
		datos_url.url         = ui->cbxUrl_url->currentText();
		datos_url.descripcion = ui->txtUrl_descripcion->toPlainText();

		QTreeWidgetItem *item_url = NULL;
		if (isNew)
		{
			item_url = new QTreeWidgetItem;

			if (Editando)
				datos_url.id = sql->insertaUnURL(datos_url);

			ui->cbxUrl_url->addItem(QIcon(fGrl->theme() +"img16/edit_enlace.png"), datos_url.url, datos_url.descripcion);
		} else {
			int pos = ui->twDatosURL->indexOfTopLevelItem(ui->twDatosURL->currentItem());
			if (ui->twDatosURL->topLevelItemCount() > 0 && pos != -1)
			{
				item_url = ui->twDatosURL->currentItem();
				datos_url.id = item_url->text(2);

				if (Editando)
					sql->actualizaURL(datos_url);
			}
			ui->btnUrl_update->setEnabled(false);
		}

		item_url->setIcon(0, QIcon(fGrl->theme() +"img16/html.png"));
		item_url->setText(0, datos_url.url);
		item_url->setText(1, datos_url.descripcion);
		item_url->setText(2, datos_url.id);
		item_url->setText(3, datos_url.idgrl);
		item_url->setText(4, datos_url.idcat);
		ui->twDatosURL->addTopLevelItem(item_url);
	}
}

void frmAddEditJuego::on_cbxUrl_url_editTextChanged(const QString &arg1)
{
	enabledUrlUpdate(arg1, 0);
}

void frmAddEditJuego::on_txtUrl_descripcion_textChanged()
{
	enabledUrlUpdate(ui->txtUrl_descripcion->toPlainText(), 1);
}

void frmAddEditJuego::on_cbxUrl_url_activated(int index)
{
	if (index > -1)
		ui->txtUrl_descripcion->setPlainText(ui->cbxUrl_url->itemData(index).toString());
	else
		ui->txtUrl_descripcion->setPlainText("");
}

void frmAddEditJuego::on_btnUrl_add_clicked()
{
	addEditTwDatosURL(true);
}

void frmAddEditJuego::on_btnUrl_update_clicked()
{
	addEditTwDatosURL(false);
}

void frmAddEditJuego::on_btnUrl_abrir_clicked()
{
// Abre la url con el navegador por defecto
	QTreeWidgetItem *item = NULL;
	item = ui->twDatosURL->currentItem();
	if (item)
		fGrl->abrirArchivo(item->text(0));
}

void frmAddEditJuego::on_btnUrl_delete_clicked()
{
	int pos = ui->twDatosURL->indexOfTopLevelItem(ui->twDatosURL->currentItem());
	if (ui->twDatosURL->topLevelItemCount() > 0 && pos != -1)
	{
		if (fGrl->questionMsg(tr("¿Eliminar url...?"), tr("¿Deseas eliminar esta url?")))
		{
			if (Editando)
				sql->eliminarURL(ui->twDatosURL->currentItem()->text(2));
			fGrl->deleteItemTree(ui->twDatosURL->currentItem());

			ui->cbxUrl_url->clear();
			ui->cbxUrl_url->addItem(QIcon(fGrl->theme() +"img16/edit_enlace.png"), "", "");

			QSqlQuery query(sql->getSqlDB());
			query.exec("SELECT DISTINCT url, descripcion FROM dbgrl_urls");
			if (query.first())
			{
				do {
					ui->cbxUrl_url->addItem(QIcon(fGrl->theme() +"img16/edit_enlace.png"), query.record().value("url").toString(), query.record().value("descripcion").toString());
				} while (query.next());
			}
			query.clear();

			ui->cbxUrl_url->setCurrentIndex(0);
			emit on_cbxUrl_url_activated(0);

			ui->txtUrl_descripcion->setPlainText("");
		}
	}
}

void frmAddEditJuego::on_twDatosURL_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->cbxUrl_url->setEditText(item->text(0));
		ui->txtUrl_descripcion->setPlainText(item->text(1));
	}
}

void frmAddEditJuego::on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
		emit on_btnUrl_abrir_clicked();
}

// Datos usuario
void frmAddEditJuego::on_btnDat_usuario_clear_clicked()
{
	ui->txtDat_usuario->clear();
}

// Datos archivos
void frmAddEditJuego::enabledDatosUpdate(QString texto, int col)
{
	int pos = ui->twDatosFiles->indexOfTopLevelItem(ui->twDatosFiles->currentItem());
	if (ui->twDatosFiles->topLevelItemCount() > 0 && pos != -1)
	{
		if (texto != ui->twDatosFiles->currentItem()->text(col))
			ui->btnFile_update->setEnabled(true);
		else
			ui->btnFile_update->setEnabled(false);
	} else
		ui->btnFile_update->setEnabled(false);
}

void frmAddEditJuego::addEditTwDatosFiles(bool isNew)
{
	if (!ui->txtFile_path->text().isEmpty())
	{
		stDatosFiles datos_file;
		datos_file.id          = "";
		datos_file.idgrl       = IdGame;
		datos_file.idcat       = categoria.id;
		datos_file.nombre      = ui->txtFile_nombre->text();
		datos_file.crc         = ui->txtFile_crc->text();
		datos_file.descripcion = ui->txtFile_descripcion->toPlainText();
		datos_file.path        = ui->txtFile_path->text();
		datos_file.size        = ui->txtFile_size->text();
		datos_file.tipo        = ui->cbxFile_tipo->itemData(ui->cbxFile_tipo->currentIndex()).toString();

		QTreeWidgetItem *item_file = NULL;
		if (isNew)
		{
			item_file = new QTreeWidgetItem;

			if (Editando)
				datos_file.id = sql->insertaUnFiles(datos_file);
		} else {
			int pos = ui->twDatosFiles->indexOfTopLevelItem(ui->twDatosFiles->currentItem());
			if (ui->twDatosFiles->topLevelItemCount() > 0 && pos != -1)
			{
				item_file = ui->twDatosFiles->currentItem();
				datos_file.id = item_file->text(6);

				if (Editando)
					sql->actualizaFiles(datos_file);
			}
			ui->btnFile_update->setEnabled(false);
		}

		QString file_ico;
		if (datos_file.tipo == "manual")
			file_ico = fGrl->theme() +"img16/datos.png";
		else if (datos_file.tipo == "pdf")
			file_ico = fGrl->theme() +"img16/pdf.png";
		else if (datos_file.tipo == "ruleta")
			file_ico = fGrl->theme() +"img16/ruleta.png";
		else if (datos_file.tipo == "archivo")
			file_ico = fGrl->theme() +"img16/archivos.png";
		else
			file_ico = fGrl->theme() +"img16/archivos.png";

		item_file->setIcon(0, QIcon(file_ico));
		item_file->setTextAlignment(1, Qt::AlignCenter);
		item_file->setTextAlignment(3, Qt::AlignCenter);
		item_file->setText(0, datos_file.nombre);
		item_file->setText(1, datos_file.crc);
		item_file->setText(2, datos_file.descripcion);
		item_file->setText(3, sql->covertir_bytes(datos_file.size.toDouble()));
		item_file->setText(4, datos_file.path);
		item_file->setText(5, datos_file.tipo);
		item_file->setText(6, datos_file.id);
		item_file->setText(7, datos_file.idgrl);
		item_file->setText(8, datos_file.idcat);
		item_file->setText(9, datos_file.size);
		ui->twDatosFiles->addTopLevelItem(item_file);
	}
}

void frmAddEditJuego::on_txtFile_path_textEdited(const QString &arg1)
{
	enabledDatosUpdate(arg1, 4);
}

void frmAddEditJuego::on_txtFile_nombre_textEdited(const QString &arg1)
{
	enabledDatosUpdate(arg1, 0);
}

void frmAddEditJuego::on_txtFile_descripcion_textChanged()
{
	enabledDatosUpdate(ui->txtFile_descripcion->toPlainText(), 2);
}

void frmAddEditJuego::on_btnFile_path_clicked()
{
	QString tipo_archivo, ext_dap, ext_comic, ext_z;
	ext_dap   = dap_ext.join(" ");
	ext_comic = comic_ext.join(" ");
	ext_z     = z_ext.join(" ");

	switch (ui->cbxFile_tipo->currentIndex())
	{
		case 0:
			tipo_archivo = tr("Documentos - Manuales") +" - ("+ ext_comic +" "+ ext_z +");;"+ tr("Todos los archivos") +" (*)";
		break;
		case 1:
			tipo_archivo = tr("Documentos - Manuales") +" - (*.pdf);;"+ tr("Todos los archivos") +" (*)";
		break;
		case 2:
			tipo_archivo = tr("Ruleta de protección") +" - ("+ ext_dap +" "+ ext_z +");;"+ tr("Todos los archivos") +" (*)";
		break;
		case 3:
			tipo_archivo = tr("Todos los archivos") +" (*)";
		break;
		default:
			tipo_archivo = tr("Todos los archivos") +" (*)";
		break;
	}

	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.DatosFiles_PathFile, "", tipo_archivo), hashCrc32);
	if (f_info.Exists)
	{
		grlCfg.DatosFiles_PathFile = fGrl->setDirRelative(f_info.Path);
		ui->txtFile_path->setText(fGrl->setDirRelative(f_info.FilePath));
		ui->txtFile_nombre->setText(f_info.Name);
		ui->txtFile_crc->setText(f_info.Crc32);
		ui->txtFile_size->setText(f_info.Size);
		ui->txtFile_descripcion->setText("");
		ui->lb_files_size->setText(sql->covertir_bytes(f_info.Size.toDouble()));

		enabledDatosUpdate(grlCfg.DatosFiles_PathFile, 4);
	} else
		emit on_btnFile_path_clear_clicked();
}

void frmAddEditJuego::on_btnFile_path_clear_clicked()
{
	ui->txtFile_path->clear();
	ui->txtFile_nombre->clear();
	ui->txtFile_crc->clear();
	ui->txtFile_size->clear();
	ui->txtFile_descripcion->clear();
}

void frmAddEditJuego::on_btnFile_add_clicked()
{
	addEditTwDatosFiles(true);
}

void frmAddEditJuego::on_btnFile_update_clicked()
{
	addEditTwDatosFiles(false);
}

void frmAddEditJuego::on_btnFile_delete_clicked()
{
	if (ui->twDatosFiles->topLevelItemCount() > 0)
	{
		if (fGrl->questionMsg(tr("¿Eliminar archivo...?"), tr("¿Deseas eliminar este archivo?")))
		{
			if (Editando)
				sql->eliminarFiles(ui->twDatosFiles->currentItem()->text(6));
			fGrl->deleteItemTree(ui->twDatosFiles->currentItem());

			emit on_btnFile_path_clear_clicked();
		}
	}
}

void frmAddEditJuego::on_twDatosFiles_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		ui->txtFile_nombre->setText(item->text(0));
		ui->txtFile_crc->setText(item->text(1));
		ui->txtFile_descripcion->setPlainText(item->text(2));
		ui->txtFile_path->setText(item->text(4));
		ui->txtFile_size->setText(item->text(9));
		ui->cbxFile_tipo->setCurrentIndex(ui->cbxFile_tipo->findData(item->text(5)));
		ui->lb_files_size->setText(sql->covertir_bytes(item->text(9).toDouble()));
	}
}

void frmAddEditJuego::on_twDatosFiles_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		stFileInfo archivo = fGrl->getInfoFile(fGrl->getDirRelative(item->text(4)));

		if (item->text(5) == "ruleta" || item->text(5) == "manual")
		{
			QStringList list_ext;
			list_ext << dap_ext << z_ext << comic_ext;
			if (list_ext.contains("*"+ archivo.Ext))
			{
				if (!isOpenGrDap)
				{
					isOpenGrDap = true;
					grdap = new GrDap(this);
				}

				grdap->cargarArchivo(archivo.FilePath);
				grdap->show();
			} else
				fGrl->abrirArchivo(archivo.FilePath);
		}
		else if (item->text(5) == "pdf")
		{
			if (grlCfg.OpenPdfExternal || archivo.Ext != ".pdf")
				fGrl->abrirArchivo(archivo.FilePath);
			else {
				if (!isOpenPdf)
				{
					isOpenPdf = true;
					pdfViewer = new frmPdfViewer(this);
					pdfViewer->setWindowFlags(Qt::Window);
				}

				pdfViewer->openPdf(archivo.FilePath);
				pdfViewer->show();
			}
		} else
			fGrl->abrirArchivo(archivo.FilePath);
	}
}

void frmAddEditJuego::on_txtDat_titulo_editingFinished()
{
//qDebug() << sql->removeAccents(ui->txtDat_titulo->text());
}
