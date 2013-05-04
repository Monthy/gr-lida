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

#include "grlida_addedit_juego.h"
#include "grlida_importar_juego.h"
#include "grlida_img_viewer.h"
#include "grlida_pdf_viewer.h"
#include "grlida_ruleta.h"
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

	grlDir.Home     = fGrl->GRlidaHomePath();
	grlDir.Capturas = grlDir.Home +"capturas/"+ categoria.tabla +"/";
	grlDir.Confdbx  = grlDir.Home +"confdbx/"+ categoria.tabla +"/";
	grlDir.Confvdms = grlDir.Home +"confvdms/"+ categoria.tabla +"/";
	grlDir.Covers   = grlDir.Home +"covers/"+ categoria.tabla +"/";
	grlDir.Datos    = grlDir.Home +"datos/";
	grlDir.Iconos   = grlDir.Home +"iconos/";
	grlDir.Smiles   = grlDir.Home +"smiles/";
	grlDir.Temp     = grlDir.Home +"temp/";
	grlDir.Thumbs   = grlDir.Home +"thumbs/"+ categoria.tabla +"/";

	createWidgets();
	cargarConfig();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmAddEditJuego::~frmAddEditJuego()
{
	delete cbxDatos_Genero;
	delete cbxDatos_Compania;
	delete cbxDatos_Desarrollador;
	delete cbxDatos_Tema;
	delete cbxDatos_Idioma;
	delete cbxDatos_IdiomaVoces;
	delete cbxDatos_Formato;
	delete cbxDatos_Perspectiva;
	delete cbxDatos_SistemaOp;

	delete fGrl;
	delete ui;
}

void frmAddEditJuego::closeEvent(QCloseEvent *event)
{
	QMessageBox msgBox(this);
	msgBox.setIcon( QMessageBox::Question );
	msgBox.setWindowTitle( tr("¿Cerrar ventana?") );
	msgBox.setText( tr("¿Deseas realmente cerrar la ventana?\nSi son nuevos datos o has echo cambios y no guardas puedes perder los cambios efectuados.") );
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
	msgBox.setDefaultButton( QMessageBox::Save );
	msgBox.setButtonText( QMessageBox::Save  , tr("Guardar")  );
	msgBox.setButtonText( QMessageBox::Close , tr("Cerrar")   );
	msgBox.setButtonText( QMessageBox::Cancel, tr("Cancelar") );

	switch ( msgBox.exec() )
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
	cbxDatos_Genero = new QCheckComboBox(this);
	cbxDatos_Compania = new QCheckComboBox(this);
	cbxDatos_Desarrollador = new QCheckComboBox(this);
	cbxDatos_Tema = new QCheckComboBox(this);
	cbxDatos_Grupo = new QCheckComboBox(this);
	cbxDatos_Idioma = new QCheckComboBox(this);
	cbxDatos_IdiomaVoces = new QCheckComboBox(this);
	cbxDatos_Formato = new QCheckComboBox(this);
	cbxDatos_Perspectiva = new QCheckComboBox(this);
	cbxDatos_SistemaOp = new QCheckComboBox(this);

	cbxDatos_Genero->setMinimumHeight(24);
	cbxDatos_Compania->setMinimumHeight(24);
	cbxDatos_Desarrollador->setMinimumHeight(24);
	cbxDatos_Tema->setMinimumHeight(24);
	cbxDatos_Grupo->setMinimumHeight(24);
	cbxDatos_Idioma->setMinimumHeight(24);
	cbxDatos_IdiomaVoces->setMinimumHeight(24);
	cbxDatos_Formato->setMinimumHeight(24);
	cbxDatos_Perspectiva->setMinimumHeight(24);
	cbxDatos_SistemaOp->setMinimumHeight(24);

	ui->gridLayout->addWidget(cbxDatos_Genero, 2, 1, 1, 7);
	ui->gridLayout->addWidget(cbxDatos_Compania, 3, 1, 1, 7);
	ui->gridLayout->addWidget(cbxDatos_Desarrollador, 4, 1, 1, 7);
	ui->gridLayout->addWidget(cbxDatos_Tema, 5, 1, 1, 4);
	ui->gridLayout->addWidget(cbxDatos_Formato, 5, 6, 1, 2);
	ui->gridLayout->addWidget(cbxDatos_Grupo, 6, 1, 1, 4);
	ui->gridLayout->addWidget(cbxDatos_Idioma, 7, 1, 1, 4);
	ui->gridLayout->addWidget(cbxDatos_IdiomaVoces, 8, 1, 1, 4);
	ui->gridLayout->addWidget(cbxDatos_Perspectiva, 9, 1, 1, 4);
	ui->gridLayout->addWidget(cbxDatos_SistemaOp, 11, 1, 1, 4);

	setTabOrder(ui->txtDatos_Subtitulo, cbxDatos_Genero       );
	setTabOrder(cbxDatos_Genero       , cbxDatos_Compania     );
	setTabOrder(cbxDatos_Compania     , cbxDatos_Desarrollador);
	setTabOrder(cbxDatos_Desarrollador, cbxDatos_Tema         );
	setTabOrder(cbxDatos_Tema         , cbxDatos_Grupo        );
	setTabOrder(cbxDatos_Grupo        , cbxDatos_Idioma       );
	setTabOrder(cbxDatos_Idioma       , cbxDatos_IdiomaVoces  );
	setTabOrder(cbxDatos_IdiomaVoces  , cbxDatos_Formato      );
	setTabOrder(cbxDatos_Formato      , ui->cbxDatos_Anno     );
	setTabOrder(ui->cbxDatos_NumDisc  , cbxDatos_Perspectiva  );
	setTabOrder(cbxDatos_Perspectiva  , ui->cbxDatos_Icono    );
	setTabOrder(ui->cbxDatos_Icono    , cbxDatos_SistemaOp    );
	setTabOrder(cbxDatos_SistemaOp    , ui->txtDatos_Tamano   );

	wDbx = new frmAddEditDosBox(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->verticalLayout_wDbx->addWidget( wDbx );

	wSvm = new frmAddEditScummVM(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->verticalLayout_wSvm->addWidget( wSvm );

	wVdms = new frmAddEditVDMSound(sql, grlCfg, categoria, IdGame, Editando, this);
	ui->verticalLayout_wVdms->addWidget( wVdms );
}

void frmAddEditJuego::createConnections()
{
	//
}

void frmAddEditJuego::cargarConfig()
{
setUpdatesEnabled(false);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	ui->tabWidget_Datos->setTabEnabled(tabDOSBox  , false);
	ui->tabWidget_Datos->setTabEnabled(tabScummVM , false);
	ui->tabWidget_Datos->setTabEnabled(tabVDMSound, false);

	emu_list.clear();
	smiles_list.clear();
	emu_list    = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|"    );
	smiles_list = fGrl->cargaDatosQHash(grlDir.Datos +"smiles.txt"  , 2, "\",\"");
	fGrl->cargarDatosTwLista(ui->twDatoSmile, grlDir.Datos +"smiles.txt", TwListSmile, "\",\"");

	ui->cbxDatos_TipoEmu->clear();
	QStringList lista = categoria.emu_show.split(";", QString::SkipEmptyParts);
	if( lista.isEmpty() || lista.contains("all") || lista.contains("datos") )
		ui->cbxDatos_TipoEmu->addItem(QIcon(fGrl->Theme() +"img16/datos.png")   , tr("Datos")   , "datos"   );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("dosbox") )
		ui->cbxDatos_TipoEmu->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("scummvm") )
		ui->cbxDatos_TipoEmu->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("ScummVM") , "scummvm" );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound") )
		ui->cbxDatos_TipoEmu->addItem(QIcon(fGrl->Theme() +"img16/vdmsound.png"), tr("VDMSound"), "vdmsound");

	foreach (const stGrlDatos &dat, emu_list)
	{
		if( lista.isEmpty() || lista.contains("all") || lista.contains(dat.key) )
		{
			if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ dat.icono) )
				ui->cbxDatos_TipoEmu->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ dat.icono), dat.titulo, dat.key);
			else
				ui->cbxDatos_TipoEmu->addItem(QIcon(":/img16_cat/sinimg.png"), dat.titulo, dat.key);
		}
	}

	int row_tipo_emu = ui->cbxDatos_TipoEmu->findData( TipoEmu );
	if( row_tipo_emu < 0 ) row_tipo_emu = 0;
	ui->cbxDatos_TipoEmu->setCurrentIndex( row_tipo_emu );
	emit on_cbxDatos_TipoEmu_activated( row_tipo_emu );

	ui->cbxUrl_url->clear();
	ui->cbxUrl_url->addItem(QIcon(fGrl->Theme() +"img16/edit_enlace.png"), "", "");
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT DISTINCT url, descripcion FROM dbgrl_urls");
	if( query.first() )
	{
		do {
			ui->cbxUrl_url->addItem(QIcon(fGrl->Theme() +"img16/edit_enlace.png"), query.record().value("url").toString(), query.record().value("descripcion").toString());
		} while ( query.next() );
	}
	query.clear();
	ui->cbxUrl_url->setCurrentIndex(0);
	emit on_cbxUrl_url_activated(0);

// Configuración del twDatosFiles
	ui->twDatosFiles->header()->setStretchLastSection(true);
	ui->twDatosFiles->header()->setMovable(false);
	ui->twDatosFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui->twDatosFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(3, QHeaderView::Interactive);
	ui->twDatosFiles->header()->setResizeMode(4, QHeaderView::Interactive);
	ui->twDatosFiles->setColumnWidth(0, 200 );
	ui->twDatosFiles->setColumnWidth(1, 65  );
	ui->twDatosFiles->setColumnWidth(2, 100 );
	ui->twDatosFiles->setColumnWidth(3, 50  );
	ui->twDatosFiles->setColumnWidth(4, 50  );

// Configuración del twDatosURL
	ui->twDatosURL->header()->setStretchLastSection(true);
	ui->twDatosURL->header()->setMovable(false);
	ui->twDatosURL->header()->setResizeMode(0, QHeaderView::Interactive);
	ui->twDatosURL->header()->setResizeMode(1, QHeaderView::Interactive);
	ui->twDatosURL->setColumnWidth(0, 250 );

	fGrl->cargarDatosCheckComboBox(cbxDatos_Genero       , grlDir.Datos + fGrl->Idioma() +"/generos.txt"     , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Compania     , grlDir.Datos +"companias.txt"                     , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Desarrollador, grlDir.Datos +"companias.txt"                     , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Tema         , grlDir.Datos + fGrl->Idioma() +"/temas.txt"       , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Grupo        , grlDir.Datos +"grupos.txt"                        , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Idioma       , grlDir.Datos + fGrl->Idioma() +"/idiomas.txt"     , fGrl->Theme() +"img_lng/", "", 3, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_IdiomaVoces  , grlDir.Datos + fGrl->Idioma() +"/idiomas.txt"     , fGrl->Theme() +"img_lng/", "", 3, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Formato      , grlDir.Datos + fGrl->Idioma() +"/formatos.txt"    , fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_Perspectiva  , grlDir.Datos + fGrl->Idioma() +"/perspectivas.txt", fGrl->Theme() +"img16/"  , "", 2, "|");
	fGrl->cargarDatosCheckComboBox(cbxDatos_SistemaOp    , grlDir.Datos +"sistemas.txt"                      , fGrl->Theme() +"img16/"  , "", 2, "|");

	fGrl->cargarDatosComboBox( ui->cbxDatos_Anno           , grlDir.Datos +"fechas.txt", fGrl->Theme() +"img16/", "", 2, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_EdadRecomendada, grlDir.Datos + fGrl->Idioma() +"/edad_recomendada.txt", fGrl->Theme() +"img16/", "nd", 3, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_NumDisc        , grlDir.Datos + fGrl->Idioma() +"/numdisc.txt"         , fGrl->Theme() +"img16/", "", 2, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_Rating         , ":datos/rating.txt"    , fGrl->Theme() +"images/", "0", 2, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_Graficos       , ":datos/puntuacion.txt", fGrl->Theme() +"img16/" , "0", 2, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_Sonido         , ":datos/puntuacion.txt", fGrl->Theme() +"img16/" , "0", 2, "|");
	fGrl->cargarDatosComboBox( ui->cbxDatos_Jugabilidad    , ":datos/puntuacion.txt", fGrl->Theme() +"img16/" , "0", 2, "|");

	fGrl->cargarIconosComboBox(ui->cbxDatos_Icono, grlDir.Iconos, "sinimg.png", grlCfg.FormatsImage.join(";"));
	ui->cbxDatos_Estado->setVisible(false);
	ui->html_preview->setVisible(false);
	ui->frame_find->setVisible(false);

	ui->cbxDatos_TipoArchivo->clear();
	ui->cbxDatos_TipoArchivo->addItem(QIcon(fGrl->Theme() +"img16/datos.png")   , tr("Documentos - Manuales") +" - (cbz - zip)", "manual");
	ui->cbxDatos_TipoArchivo->addItem(QIcon(fGrl->Theme() +"img16/pdf.png")     , tr("Documentos - Manuales") +" - (pdf)"      , "pdf"   );
	ui->cbxDatos_TipoArchivo->addItem(QIcon(fGrl->Theme() +"img16/ruleta.png")  , tr("Ruleta de protección") +" - (conf - zip)", "ruleta");
	ui->cbxDatos_TipoArchivo->addItem(QIcon(fGrl->Theme() +"img16/archivos.png"), tr("Cualquier archivos"), "");

	if( Editando )
	{
		cargarDatosJuego( sql->show_Datos(categoria.tabla, IdGame) );
		sql->cargarDatosUrls(ui->twDatosURL   , IdGame, categoria.id);
		sql->cargarDatosFiles(ui->twDatosFiles, IdGame, categoria.id);

		if( TipoEmu == "dosbox" )
		{
			ui->tabWidget_Datos->setTabEnabled(tabDOSBox, true);
			wDbx->cargarConfig();
			wDbx->cargarDatosDosBox( sql->showConfg_DOSBox(IdGame, categoria.id) );
		}

		if( TipoEmu == "scummvm" )
		{
			ui->tabWidget_Datos->setTabEnabled(tabScummVM , true);
			wSvm->cargarConfig();
			wSvm->cargarDatosScummVM( sql->showConfg_ScummVM(IdGame, categoria.id) );
		}

		if( TipoEmu == "vdmsound" )
		{
			ui->tabWidget_Datos->setTabEnabled(tabVDMSound, true);
			wVdms->cargarConfig();
			wVdms->cargarDatosVDMSound( sql->showConfg_VDMSound(IdGame, categoria.id) );
		}

		ui->cbxDatos_TipoEmu->setEnabled( false );
		setWindowTitle(categoria.titulo +" - "+ tr("Editando a") +": "+ ui->txtDatos_Titulo->text() );
	} else {
		cargarDatosJuego( fGrl->getDefectDatos() );

		wDbx->cargarConfig();
		wDbx->cargarDatosDosBox( fGrl->getDefectDOSBox() );

		wSvm->cargarConfig();
		wSvm->cargarDatosScummVM( fGrl->getDefectScummVM() );

		wVdms->cargarConfig();
		wVdms->cargarDatosVDMSound( fGrl->getDefectVDMSound() );

		ui->cbxDatos_TipoEmu->setEnabled( true );
		setWindowTitle(categoria.titulo +" - "+ tr("Añadiendo un nuevo juego") );
	}

	setTheme();
setUpdatesEnabled(true);

	ui->txtDatos_Titulo->setFocus();
}

void frmAddEditJuego::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/datos_1.png") );

// General
	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnDescargarInfo->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->tabWidget_Datos->setTabIcon(0, QIcon(fGrl->Theme() +"img16/datos.png") );
	ui->tabWidget_Datos->setTabIcon(1, QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->tabWidget_Datos->setTabIcon(2, QIcon(fGrl->Theme() +"img16/importar.png") );
	ui->tabWidget_Datos->setTabIcon(3, QIcon(fGrl->Theme() +"img16/archivos.png") );
	ui->tabWidget_Datos->setTabIcon(4, QIcon(fGrl->Theme() +"img16/dosbox.png") );
	ui->tabWidget_Datos->setTabIcon(5, QIcon(fGrl->Theme() +"img16/scummvm.png") );
	ui->tabWidget_Datos->setTabIcon(6, QIcon(fGrl->Theme() +"img16/vdmsound.png") );
// Datos
//	ui->btnDatosAdd_Grupo->setIcon( QIcon(fGrl->Theme() +"img16/list_add.png") );
	ui->btnImgAbrir_Thumbs->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnImgAbrir_CoverFront->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnImgAbrir_CoverBack->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnImgVer_Thumbs->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->btnImgVer_CoverFront->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->btnImgVer_CoverBack->setIcon( QIcon(fGrl->Theme() +"img16/capturas.png") );
	ui->btnImgEliminar_Thumbs->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnImgEliminar_CoverFront->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnImgEliminar_CoverBack->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->btnTool_Cortar->setIcon( QIcon(fGrl->Theme() +"img16/edit_cut.png") );
	ui->btnTool_Copiar->setIcon( QIcon(fGrl->Theme() +"img16/edit_copy.png") );
	ui->btnTool_Pegar->setIcon( QIcon(fGrl->Theme() +"img16/edit_paste.png") );
	ui->btnTool_SelectAll->setIcon( QIcon(fGrl->Theme() +"img16/edit_select_all.png") );
	ui->btnTool_Deshacer->setIcon( QIcon(fGrl->Theme() +"img16/edit_deshacer.png") );
	ui->btnTool_Rehacer->setIcon( QIcon(fGrl->Theme() +"img16/edit_rehacer.png") );
	ui->btnTool_TextoNegrita->setIcon( QIcon(fGrl->Theme() +"img16/edit_negrita.png") );
	ui->btnTool_TextoCursiva->setIcon( QIcon(fGrl->Theme() +"img16/edit_cursiva.png") );
	ui->btnTool_TextoSubrayado->setIcon( QIcon(fGrl->Theme() +"img16/edit_subrayada.png") );
	ui->btnTool_InsertarImg->setIcon( QIcon(fGrl->Theme() +"img16/edit_imagen.png") );
	ui->btnTool_InsertaUrl->setIcon( QIcon(fGrl->Theme() +"img16/edit_enlace.png") );
	ui->btnTool_Buscar->setIcon( QIcon(fGrl->Theme() +"img16/edit_buscar.png") );
	ui->btnTool_Reemplazar->setIcon( QIcon(fGrl->Theme() +"img16/edit_reemplazar.png") );
	ui->btnTool_Preview->setIcon( QIcon(fGrl->Theme() +"img16/edit_preview.png") );
	ui->btnTool_ReemplazarTexto->setIcon( QIcon(fGrl->Theme() +"img16/edit_reemplazar.png") );
	ui->btnTool_BuscarAnterior->setIcon( QIcon(fGrl->Theme() +"img16/edit_buscar_anterior.png") );
	ui->btnTool_BuscarSiguiente->setIcon( QIcon(fGrl->Theme() +"img16/edit_buscar_siguiente.png") );

	ui->btnDatos_ExeJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDatos_ExeJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDatos_ParametrosExe_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDatos_SetupJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDatos_SetupJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDatos_ParametrosSetup_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->btnAddUrl->setIcon( QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->btnUpdateUrl->setIcon( QIcon(fGrl->Theme() +"img16/editar.png") );
	ui->btnDeleteUrl->setIcon( QIcon(fGrl->Theme() +"img16/eliminar.png") );
	ui->btnAbrirUrl->setIcon( QIcon(fGrl->Theme() +"img16/edit_enlace.png") );

	ui->btnDatos_Usuario_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	ui->btnDatosFiles_PathFile->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDatosFiles_PathFile_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnAddFile->setIcon( QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->btnUpdateFile->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->btnDeleteFile->setIcon( QIcon(fGrl->Theme() +"img16/eliminar.png") );
}

void frmAddEditJuego::cargarDatosJuego(stDatosJuego datos, bool isImport)
{
setUpdatesEnabled(false);
	if( !isImport )
		DatosJuego = datos;

	if( Editando )
		IdGame = datos.idgrl;

	int row_icon = ui->cbxDatos_Icono->findData( datos.icono );
	if( row_icon < 0 ) row_icon = 0;

	ui->cbxDatos_Icono->setCurrentIndex( row_icon );
	ui->txtDatos_Titulo->setText( datos.titulo );
	ui->txtDatos_Subtitulo->setText( datos.subtitulo );
	cbxDatos_Genero->setCheckedItems( datos.genero );
	cbxDatos_Compania->setCheckedItems( datos.compania );
	cbxDatos_Desarrollador->setCheckedItems( datos.desarrollador );
	cbxDatos_Tema->setCheckedItems( datos.tema );
	cbxDatos_Grupo->setCheckedItems( datos.grupo );
	cbxDatos_Perspectiva->setCheckedItems( datos.perspectiva );
	cbxDatos_Idioma->setCheckedItems( datos.idioma );
	cbxDatos_IdiomaVoces->setCheckedItems( datos.idioma_voces );
	cbxDatos_Formato->setCheckedItems( datos.formato );
	ui->cbxDatos_Anno->setCurrentIndex( ui->cbxDatos_Anno->findText( datos.anno ) );
	ui->cbxDatos_NumDisc->setCurrentIndex( ui->cbxDatos_NumDisc->findData( datos.numdisc ) );
	cbxDatos_SistemaOp->setCheckedItems( datos.sistemaop );
	ui->txtDatos_Tamano->setText( datos.tamano );
	ui->cbxDatos_Graficos->setCurrentIndex( ui->cbxDatos_Graficos->findText( datos.graficos ) );
	ui->cbxDatos_Sonido->setCurrentIndex( ui->cbxDatos_Sonido->findText( datos.sonido ) );
	ui->cbxDatos_Jugabilidad->setCurrentIndex( ui->cbxDatos_Jugabilidad->findText( datos.jugabilidad ) );
	ui->chkDatos_Original->setChecked( fGrl->StrToBool( datos.original ) );
	ui->cbxDatos_Estado->setCurrentIndex( ui->cbxDatos_Estado->findText( datos.estado ) );
	ui->lb_fechahora->setText( (Editando) ? fGrl->HoraFechaActual(datos.fecha, grlCfg.FormatoFecha) : fGrl->HoraFechaActual(fGrl->getTime(), grlCfg.FormatoFecha) );

	if( Editando )
	{
		if( TipoEmu != datos.tipo_emu )
		{
			int respuesta = QMessageBox::question(this, tr("Tipo emulador distintos"), 
												  tr("El tipo de mulador no corresponde al seleccionado.") +"\n"+
												  tr("¿Deseas crear la configuración por defecto?"), tr("Si"), tr("No"), 0, 1);
			if( respuesta == 0 )
			{
				DatosJuego.tipo_emu = TipoEmu;

				if( TipoEmu == "dosbox" )
				{
					ui->tabWidget_Datos->setTabEnabled(tabDOSBox, true);
					wDbx->cargarConfig();

					DatosDosBox = sql->showConfg_DOSBox(IdGame, categoria.id);
					DatosDosBox.idgrl = IdGame;
					DatosDosBox.idcat = categoria.id;

					if( DatosDosBox.id.isEmpty() )
						DatosDosBox.id = sql->insertaDbx( DatosDosBox );

					wDbx->cargarDatosDosBox( DatosDosBox );
				}

				if( TipoEmu == "scummvm" )
				{
					ui->tabWidget_Datos->setTabEnabled(tabScummVM , true);
					wSvm->cargarConfig();

					DatosScummVM = sql->showConfg_ScummVM(IdGame, categoria.id);
					DatosScummVM.idgrl = IdGame;
					DatosScummVM.idcat = categoria.id;

					if( DatosScummVM.id.isEmpty() )
						DatosScummVM.id = sql->insertaSvm( DatosScummVM );

					wSvm->cargarDatosScummVM( DatosScummVM );
				}
				
				if( TipoEmu == "vdmsound" )
				{
					ui->tabWidget_Datos->setTabEnabled(tabVDMSound, true);
					wVdms->cargarConfig();

					DatosVDMSound = sql->showConfg_VDMSound(IdGame, categoria.id);
					DatosVDMSound.idgrl = IdGame;
					DatosVDMSound.idcat = categoria.id;

					if( DatosVDMSound.id.isEmpty() )
						DatosVDMSound.id = sql->insertaVdms( DatosVDMSound );

					wVdms->cargarDatosVDMSound( DatosVDMSound );
				}
			}
		}
	}

	ui->txtDatos_Comentario->setPlainText( datos.comentario );
	ui->chkDatos_Favorito->setChecked( fGrl->StrToBool( datos.favorito ) );
	ui->cbxDatos_Rating->setCurrentIndex( ui->cbxDatos_Rating->findData( datos.rating ) );
	ui->cbxDatos_EdadRecomendada->setCurrentIndex( ui->cbxDatos_EdadRecomendada->findData( datos.edad_recomendada ) );
	ui->txtDatos_Usuario->setText( datos.usuario );
	ui->txtDatos_path_exe->setText( datos.path_exe );
	ui->txtDatos_parametros_exe->setText( datos.parametros_exe );
	ui->txtDatos_path_capturas->setText( datos.path_capturas );
	ui->txtDatos_path_setup->setText( datos.path_setup );
	ui->txtDatos_parametros_setup->setText( datos.parametros_setup );

	Thumbs     = datos.thumbs;
	CoverFront = datos.cover_front;
	CoverBack  = datos.cover_back;

	file_thumbs.clear();
	if( isImport && datos.thumbs_new )
		file_thumbs = grlDir.Temp + Thumbs;
	else
		file_thumbs = grlDir.Thumbs + Thumbs;

	if( !Thumbs.isEmpty() && QFile::exists(file_thumbs) )
	{
		ui->lbImg_Thumbs->setPixmap( QPixmap(file_thumbs) );
		ui->btnImgVer_Thumbs->setEnabled( true );
		ui->btnImgEliminar_Thumbs->setEnabled( true );
	} else {
		if( !datos.thumbs_new )
			emit on_btnImgEliminar_Thumbs_clicked();
	}

	file_cover_front.clear();
	if( isImport && datos.cover_front_new )
		file_cover_front = grlDir.Temp + CoverFront;
	else
		file_cover_front = grlDir.Covers + CoverFront;

	if( !CoverFront.isEmpty() && QFile::exists(file_cover_front) )
	{
		ui->lbImg_CoverFront->setPixmap( QPixmap(file_cover_front) );
		ui->btnImgVer_CoverFront->setEnabled( true );
		ui->btnImgEliminar_CoverFront->setEnabled( true );
	} else {
		if( !datos.thumbs_new )
			emit on_btnImgEliminar_CoverFront_clicked();
	}

	file_cover_back.clear();
	if( isImport && datos.cover_back_new )
		file_cover_back = grlDir.Temp + CoverBack;
	else
		file_cover_back = grlDir.Covers + CoverBack;

	if( !CoverBack.isEmpty() && QFile::exists(file_cover_back) )
	{
		ui->lbImg_CoverBack->setPixmap( QPixmap(file_cover_back) );
		ui->btnImgVer_CoverBack->setEnabled( true );
		ui->btnImgEliminar_CoverBack->setEnabled( true );
	} else {
		if( !datos.thumbs_new )
			emit on_btnImgEliminar_CoverBack_clicked();
	}
setUpdatesEnabled(true);
}

bool frmAddEditJuego::setDatosJuegos(bool isSoloDatos)
{
	QString str, img_tmp_name;
	bool isOk = false;

	oldTitulo = DatosJuego.titulo;

	str = ui->cbxDatos_Icono->itemData( ui->cbxDatos_Icono->currentIndex() ).toString();
	DatosJuego.icono            = str.isEmpty() ? "datos.png" : str;
	DatosJuego.titulo           = ui->txtDatos_Titulo->text();
	DatosJuego.subtitulo        = ui->txtDatos_Subtitulo->text();
	DatosJuego.genero           = cbxDatos_Genero->getCheckedItems();
	DatosJuego.compania         = cbxDatos_Compania->getCheckedItems();
	DatosJuego.desarrollador    = cbxDatos_Desarrollador->getCheckedItems();
	DatosJuego.tema             = cbxDatos_Tema->getCheckedItems();
	DatosJuego.grupo            = cbxDatos_Grupo->getCheckedItems();
	DatosJuego.perspectiva      = cbxDatos_Perspectiva->getCheckedItems();
	DatosJuego.idioma           = cbxDatos_Idioma->getCheckedItems();
	DatosJuego.idioma_voces     = cbxDatos_IdiomaVoces->getCheckedItems();
	DatosJuego.formato          = cbxDatos_Formato->getCheckedItems();
	DatosJuego.anno             = ui->cbxDatos_Anno->currentText().isEmpty() ? "2012" : ui->cbxDatos_Anno->currentText();

	str = ui->cbxDatos_NumDisc->itemData( ui->cbxDatos_NumDisc->currentIndex() ).toString();
	DatosJuego.numdisc          = str.isEmpty() ? "" : str;
	DatosJuego.sistemaop        = cbxDatos_SistemaOp->getCheckedItems();
	DatosJuego.tamano           = ui->txtDatos_Tamano->text();
	DatosJuego.graficos         = ui->cbxDatos_Graficos->currentText().isEmpty() ? "0" : ui->cbxDatos_Graficos->currentText();
	DatosJuego.sonido           = ui->cbxDatos_Sonido->currentText().isEmpty() ? "0" : ui->cbxDatos_Sonido->currentText();
	DatosJuego.jugabilidad      = ui->cbxDatos_Jugabilidad->currentText().isEmpty() ? "0" : ui->cbxDatos_Jugabilidad->currentText();
	DatosJuego.original         = fGrl->BoolToStr( ui->chkDatos_Original->isChecked() );
	DatosJuego.estado           = ui->cbxDatos_Estado->currentText().isEmpty() ? "" : ui->cbxDatos_Estado->currentText();

	if( !Editando )
		DatosJuego.fecha = fGrl->getTime();

//	DatosJuego.tipo_emu   = TipoEmu.isEmpty() ? "datos" : TipoEmu;
	DatosJuego.comentario = ui->txtDatos_Comentario->toPlainText();
	DatosJuego.favorito   = fGrl->BoolToStr( ui->chkDatos_Favorito->isChecked() );

	str = ui->cbxDatos_Rating->itemData( ui->cbxDatos_Rating->currentIndex() ).toString();
	DatosJuego.rating = ui->cbxDatos_Rating->currentIndex() > -1 ? str : "0";

	str =  ui->cbxDatos_EdadRecomendada->itemData( ui->cbxDatos_EdadRecomendada->currentIndex() ).toString();
	DatosJuego.edad_recomendada = str.isEmpty() ? "nd" : str;
	DatosJuego.usuario          = ui->txtDatos_Usuario->text();
	DatosJuego.path_exe         = ui->txtDatos_path_exe->text();
	DatosJuego.path_setup       = ui->txtDatos_path_setup->text();
	DatosJuego.parametros_exe   = ui->txtDatos_parametros_exe->text();
	DatosJuego.parametros_setup = ui->txtDatos_parametros_setup->text();
	DatosJuego.path_capturas    = ui->txtDatos_path_capturas->text();

	if( isSoloDatos )
	{
		DatosJuego.thumbs      = Thumbs;
		DatosJuego.cover_front = CoverFront;
		DatosJuego.cover_back  = CoverBack;

		if( Editando )
			DatosJuego.idgrl = IdGame;
	//	else
	//		DatosJuego.idgrl = "";
	} else {
		if( Editando )
		{
			QString old_name_thumbs, old_name_cover_front, old_name_cover_back;
			old_name_thumbs      = DatosJuego.thumbs;
			old_name_cover_front = DatosJuego.cover_front;
			old_name_cover_back  = DatosJuego.cover_back;

			DatosJuego.idgrl = IdGame;
			img_tmp_name = "id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(DatosJuego.titulo) +"_"+ TipoEmu;

		// Actualizamos los nombres de las imagenes
			if( Thumbs.isEmpty() )
				DatosJuego.thumbs = "";
			else {
				QString new_thumbs = img_tmp_name +"_thumbs"+ fGrl->getInfoFile(file_thumbs).Ext;
				if( DatosJuego.thumbs != Thumbs || DatosJuego.thumbs_new )
				{
					QFile::remove(grlDir.Thumbs + old_name_thumbs);
					QFile::copy(file_thumbs, grlDir.Thumbs + new_thumbs);
				}
				if( DatosJuego.thumbs == Thumbs && oldTitulo != DatosJuego.titulo )
					QFile::rename(grlDir.Thumbs + old_name_thumbs, grlDir.Thumbs + new_thumbs);

				DatosJuego.thumbs = new_thumbs;
			}

			if( CoverFront.isEmpty() )
				DatosJuego.cover_front = "";
			else {
				QString new_cover_front = img_tmp_name +"_cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
				if( DatosJuego.cover_front != CoverFront || DatosJuego.cover_front_new )
				{
					QFile::remove(grlDir.Covers + old_name_cover_front);
					QFile::copy(file_cover_front, grlDir.Covers + new_cover_front);
				}
				if( DatosJuego.cover_front == CoverFront && oldTitulo != DatosJuego.titulo )
					QFile::rename(grlDir.Covers + old_name_cover_front, grlDir.Covers + new_cover_front);

				DatosJuego.cover_front = new_cover_front;
			}

			if( CoverBack.isEmpty() )
				DatosJuego.cover_back = "";
			else {
				QString new_cover_back = img_tmp_name +"_cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
				if( DatosJuego.cover_back != CoverBack || DatosJuego.cover_back_new  )
				{
					QFile::remove(grlDir.Covers + old_name_cover_back);
					QFile::copy(file_cover_back, grlDir.Covers + new_cover_back);
				}
				if( DatosJuego.cover_back == CoverBack && oldTitulo != DatosJuego.titulo )
					QFile::rename(grlDir.Covers + old_name_cover_back , grlDir.Covers + new_cover_back);

				DatosJuego.cover_back = new_cover_back;
			}

			isOk = sql->actualizaDatos(categoria.tabla, DatosJuego);

			comprobarDirCapturas(DatosJuego.path_capturas, oldTitulo, DatosJuego.titulo, TipoEmu);
		} else {
			DatosJuego.thumbs      = "";
			DatosJuego.cover_front = "";
			DatosJuego.cover_back  = "";

			IdGame = sql->insertaDatos(categoria.tabla, DatosJuego);
			DatosJuego.idgrl = IdGame;
			isOk = IdGame.isEmpty() ? false : true;

			if( isOk )
			{
				img_tmp_name = "id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(DatosJuego.titulo) +"_"+ TipoEmu;

			// Actualizamos las imagenes
				if( !Thumbs.isEmpty() )
				{
					DatosJuego.thumbs = img_tmp_name +"_thumbs"+ fGrl->getInfoFile(file_thumbs).Ext;
					sql->actualizaDatosItem(categoria.tabla, IdGame, "thumbs", DatosJuego.thumbs);
					QFile::copy(file_thumbs, grlDir.Thumbs + DatosJuego.thumbs);
				}

				if( !CoverFront.isEmpty() )
				{
					DatosJuego.cover_front = img_tmp_name +"_cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
					sql->actualizaDatosItem(categoria.tabla, IdGame, "cover_front", DatosJuego.cover_front);
					QFile::copy(file_cover_front, grlDir.Covers + DatosJuego.cover_front);
				}

				if( !CoverBack.isEmpty() )
				{
					DatosJuego.cover_back = img_tmp_name +"_cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
					sql->actualizaDatosItem(categoria.tabla, IdGame, "cover_back", DatosJuego.cover_back);
					QFile::copy(file_cover_back, grlDir.Covers + DatosJuego.cover_back);
				}

				sql->insertaFiles(ui->twDatosFiles, IdGame, categoria.id);
				sql->insertaURL(ui->twDatosURL, IdGame, categoria.id);

				comprobarDirCapturas(DatosJuego.path_capturas, DatosJuego.titulo, DatosJuego.titulo, TipoEmu);
			}
		}
	}
	return isOk;
}

void frmAddEditJuego::comprobarDirCapturas(QString dir_capturas, QString old_titulo, QString new_titulo, QString tipo_emu)
{
	if( dir_capturas == "" || dir_capturas == "capture" )
	{
		QDir captureDir;
		if( captureDir.exists(grlDir.Capturas +"id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(old_titulo) +"_"+ tipo_emu) && old_titulo != new_titulo)
			captureDir.rename(grlDir.Capturas +"id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(old_titulo) +"_"+ tipo_emu, grlDir.Capturas +"id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(new_titulo) +"_"+ tipo_emu);
		else
			fGrl->comprobarDirectorio(grlDir.Capturas +"id-"+ IdGame +"_"+ fGrl->eliminar_caracteres(new_titulo) +"_"+ tipo_emu);
	}
}

void frmAddEditJuego::on_btnOk_clicked()
{
	bool siguiente = true;

	if( ui->txtDatos_Titulo->text().isEmpty() )
	{
		siguiente = false;
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner un titulo al juego"));
	} else {
		DatosJuego.tipo_emu = TipoEmu.isEmpty() ? "datos" : TipoEmu;

		if( TipoEmu == "dosbox" )
		{
			wDbx->setDatosDosBox();
			siguiente = wDbx->isCorrectNext();
		}

		if( TipoEmu == "scummvm" )
		{
			wSvm->setDatosScummVM();
			siguiente = wSvm->isCorrectNext();
		}

		if( TipoEmu == "vdmsound" )
		{
			wVdms->setDatosVDMSound();
			siguiente = wVdms->isCorrectNext();
		}

		if( siguiente )
		{
			siguiente = setDatosJuegos();

			if( siguiente )
			{
				if( TipoEmu == "dosbox" )
				{
					DatosDosBox = wDbx->getDatosDosBox();
					grlCfg.Dbx_path_exe        = wDbx->getGrlCfg().Dbx_path_exe;
					grlCfg.Dbx_path_setup      = wDbx->getGrlCfg().Dbx_path_setup;
					grlCfg.Montaje_path        = wDbx->getGrlCfg().Montaje_path;
					grlCfg.Dbx_sdl_mapperfile  = wDbx->getGrlCfg().Dbx_sdl_mapperfile;
					grlCfg.Dbx_dosbox_language = wDbx->getGrlCfg().Dbx_dosbox_language;
					grlCfg.Dbx_dosbox_captures = wDbx->getGrlCfg().Dbx_dosbox_captures;
					grlCfg.Dbx_path_sonido     = wDbx->getGrlCfg().Dbx_path_sonido;

					QList<QString> url_list = sql->getDatosUrls(DatosJuego.idgrl, categoria.id);

					if( Editando )
					{
						sql->actualizaDbx( DatosDosBox );
						sql->actualizaMontajeDbx( wDbx->ui->twMontajes );
						fGrl->crearArchivoConfigDbx(DatosJuego, url_list, DatosDosBox, wDbx->ui->twMontajes, grlDir.Home, categoria.tabla, grlDir.Confdbx + DatosDosBox.path_conf);

						comprobarDirCapturas(DatosDosBox.dosbox_captures, oldTitulo, DatosJuego.titulo, TipoEmu);
					} else {
						DatosDosBox.idgrl = DatosJuego.idgrl;
						DatosDosBox.idcat = categoria.id;
						DatosDosBox.id    = sql->insertaDbx( DatosDosBox );
						sql->insertaMontajesDbx(wDbx->ui->twMontajes, DatosDosBox.id);
						fGrl->crearArchivoConfigDbx(DatosJuego, url_list, DatosDosBox, wDbx->ui->twMontajes, grlDir.Home, categoria.tabla, grlDir.Confdbx + DatosDosBox.path_conf);

						comprobarDirCapturas(DatosDosBox.dosbox_captures, DatosJuego.titulo, DatosJuego.titulo, TipoEmu);
					}
				}

				if( TipoEmu == "scummvm" )
				{
					DatosScummVM = wSvm->getDatosScummVM();

					if( Editando )
					{
						sql->actualizaSvm( DatosScummVM );

						comprobarDirCapturas(DatosScummVM.path_capturas, oldTitulo, DatosJuego.titulo, TipoEmu);
					} else {
						DatosScummVM.idgrl = DatosJuego.idgrl;
						DatosScummVM.idcat = categoria.id;
						DatosScummVM.id    = sql->insertaSvm( DatosScummVM );

						comprobarDirCapturas(DatosScummVM.path_capturas, DatosJuego.titulo, DatosJuego.titulo, TipoEmu);
					}
				}

				if( TipoEmu == "vdmsound" )
				{
					DatosVDMSound = wVdms->getDatosVDMSound();

					if( Editando )
					{
						sql->actualizaVdms( DatosVDMSound );
						fGrl->crearArchivoConfigVdmS(DatosVDMSound, grlDir.Confvdms + DatosVDMSound.path_conf);

	//					comprobarDirCapturas(DatosVDMSound.path_capturas, oldTitulo, DatosJuego.titulo, TipoEmu);
					} else {
						DatosVDMSound.idgrl = DatosJuego.idgrl;
						DatosVDMSound.idcat = categoria.id;
						DatosVDMSound.id    = sql->insertaVdms( DatosVDMSound );
						fGrl->crearArchivoConfigVdmS(DatosVDMSound, grlDir.Confvdms + DatosVDMSound.path_conf);

	//					comprobarDirCapturas(DatosVDMSound.path_capturas, DatosJuego.titulo, DatosJuego.titulo, TipoEmu);
					}
				}

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
	DatosDosBox   = wDbx->getDatosDosBox();
	DatosScummVM  = wSvm->getDatosScummVM();
	DatosVDMSound = wVdms->getDatosVDMSound();

	frmImportarJuego *ImportarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, Editando, this);
	ImportarJuego->setWindowFlags(Qt::Window);

	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		grlCfg = ImportarJuego->getGrlCfg();
		cargarDatosJuego(ImportarJuego->getDatosJuegos(), true);

		if( TipoEmu == "dosbox" && DatosJuego.tipo_emu == "dosbox" )
		{
			wDbx->cargarDatosDosBox(ImportarJuego->getDatosDosBox());
			wDbx->cargarDatosDBxMontaje(ImportarJuego->ui->twMontajes);
		}

		if( TipoEmu == "scummvm" && DatosJuego.tipo_emu == "scummvm" )
		{
			wSvm->cargarDatosScummVM(ImportarJuego->getDatosScummVM());
		}

		if( TipoEmu == "vdmsound" && DatosJuego.tipo_emu == "vdmsound" )
		{
			wVdms->cargarDatosVDMSound(ImportarJuego->getDatosVDMSound());
		}
	} else
		grlCfg = ImportarJuego->getGrlCfg();

	delete ImportarJuego;
}

void frmAddEditJuego::on_txtDatos_Titulo_editingFinished()
{
	QString titulo, conf_file;
	stFileInfo f_info;
	titulo = fGrl->eliminar_caracteres( ui->txtDatos_Titulo->text() );
	titulo = titulo.isEmpty() ? "config_"+ TipoEmu : titulo;

	if( TipoEmu == "dosbox" )
	{
		conf_file = grlDir.Confdbx + titulo +".conf";
		f_info    = fGrl->getInfoFile(conf_file);
		if( f_info.Exists )
			conf_file = f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") + f_info.Ext;
		else
			conf_file = f_info.NameExt;

		if( Editando && wDbx->ui->txtDbx_path_conf->text().isEmpty() )
			wDbx->ui->txtDbx_path_conf->setText( conf_file );
		else
			wDbx->ui->txtDbx_path_conf->setText( conf_file );
	}

	if( TipoEmu == "vdmsound" )
	{
		conf_file = grlDir.Confvdms + titulo +".vlp";
		f_info    = fGrl->getInfoFile(conf_file);
		if( f_info.Exists )
			conf_file = f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") + f_info.Ext;
		else
			conf_file = f_info.NameExt;

		if( Editando && wVdms->getPathConf().isEmpty() )
			wVdms->setPathConf( conf_file );
		else
			wVdms->setPathConf( conf_file );
	}
}

void frmAddEditJuego::on_cbxDatos_TipoEmu_activated(int index)
{
	TipoEmu = ui->cbxDatos_TipoEmu->itemData( index ).toString();
	ui->gBoxDatos_Exe->setEnabled(true);

	if( TipoEmu.isEmpty() )
		TipoEmu = "datos";

	if( TipoEmu == "dosbox" )
	{
		ui->gBoxDatos_Exe->setEnabled(false);
		ui->tabWidget_Datos->setTabEnabled(tabDOSBox  , true );
		ui->tabWidget_Datos->setTabEnabled(tabScummVM , false);
		ui->tabWidget_Datos->setTabEnabled(tabVDMSound, false);
		emit on_txtDatos_Titulo_editingFinished();
	}
	else if( TipoEmu == "scummvm" )
	{
		ui->gBoxDatos_Exe->setEnabled(false);
		ui->tabWidget_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabWidget_Datos->setTabEnabled(tabScummVM , true );
		ui->tabWidget_Datos->setTabEnabled(tabVDMSound, false);
	}
	else if( TipoEmu == "vdmsound" )
	{
		ui->gBoxDatos_Exe->setEnabled(false);
		ui->tabWidget_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabWidget_Datos->setTabEnabled(tabScummVM , false);
		ui->tabWidget_Datos->setTabEnabled(tabVDMSound, true );
		emit on_txtDatos_Titulo_editingFinished();
	} else {
		ui->tabWidget_Datos->setTabEnabled(tabDOSBox  , false);
		ui->tabWidget_Datos->setTabEnabled(tabScummVM , false);
		ui->tabWidget_Datos->setTabEnabled(tabVDMSound, false);
	}

	int row_icono = ui->cbxDatos_Icono->findData(TipoEmu, Qt::UserRole, Qt::MatchContains);
	if( row_icono < 0 ) row_icono = 0;
	ui->cbxDatos_Icono->setCurrentIndex( row_icono );
}

void frmAddEditJuego::on_btnImgAbrir_Thumbs_clicked()
{
	file_thumbs = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Img_Thumbs, Thumbs, tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( file_thumbs );
	if( f_info.Exists )
	{
		Thumbs = f_info.NameExt;
		ui->lbImg_Thumbs->setPixmap( QPixmap(file_thumbs) );
		ui->btnImgVer_Thumbs->setEnabled( true );
		ui->btnImgEliminar_Thumbs->setEnabled( true );

		grlCfg.Img_Thumbs = f_info.Path;
		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Img_Thumbs", grlCfg.Img_Thumbs);
	} else
		emit on_btnImgEliminar_Thumbs_clicked();
}

void frmAddEditJuego::on_btnImgVer_Thumbs_clicked()
{
	if( !Thumbs.isEmpty() )
	{
		frmImgViewer *ImgViewer = new frmImgViewer(grlCfg, this);
		ImgViewer->setWindowFlags(Qt::Window);
		ImgViewer->open(grlDir.Thumbs + Thumbs);
		ImgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgEliminar_Thumbs_clicked()
{
	Thumbs = "";
	ui->lbImg_Thumbs->setPixmap( QPixmap(fGrl->Theme() +"images/juego_sin_imagen.png") );
	ui->btnImgVer_Thumbs->setEnabled( false );
	ui->btnImgEliminar_Thumbs->setEnabled( false );
}

void frmAddEditJuego::on_btnImgAbrir_CoverFront_clicked()
{
	file_cover_front = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Img_CoverFront, CoverFront, tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( file_cover_front );
	if( f_info.Exists )
	{
		CoverFront = f_info.NameExt;
		ui->lbImg_CoverFront->setPixmap( QPixmap(file_cover_front) );
		ui->btnImgVer_CoverFront->setEnabled( true );
		ui->btnImgEliminar_CoverFront->setEnabled( true );

		grlCfg.Img_CoverFront = f_info.Path;
		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Img_CoverFront", grlCfg.Img_CoverFront);
	} else
		emit on_btnImgEliminar_CoverFront_clicked();
}

void frmAddEditJuego::on_btnImgVer_CoverFront_clicked()
{
	if( !CoverFront.isEmpty() )
	{
		frmImgViewer *ImgViewer = new frmImgViewer(grlCfg, this);
		ImgViewer->setWindowFlags(Qt::Window);
		ImgViewer->open(grlDir.Covers + CoverFront);
		ImgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgEliminar_CoverFront_clicked()
{
	CoverFront = "";
	ui->lbImg_CoverFront->setPixmap( QPixmap(fGrl->Theme() +"images/juego_sin_imagen.png") );
	ui->btnImgVer_CoverFront->setEnabled( false );
	ui->btnImgEliminar_CoverFront->setEnabled( false );
}

void frmAddEditJuego::on_btnImgAbrir_CoverBack_clicked()
{
	file_cover_back = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Img_CoverBack, CoverBack, tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( file_cover_back );
	if( f_info.Exists )
	{
		CoverBack = f_info.NameExt;
		ui->lbImg_CoverBack->setPixmap( QPixmap(file_cover_back) );
		ui->btnImgVer_CoverBack->setEnabled( true );
		ui->btnImgEliminar_CoverBack->setEnabled( true );

		grlCfg.Img_CoverBack = f_info.Path;
		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Img_CoverBack", grlCfg.Img_CoverBack);
	} else
		emit on_btnImgEliminar_CoverBack_clicked();
}

void frmAddEditJuego::on_btnImgVer_CoverBack_clicked()
{
	if( !CoverBack.isEmpty() )
	{
		frmImgViewer *ImgViewer = new frmImgViewer(grlCfg, this);
		ImgViewer->setWindowFlags(Qt::Window);
		ImgViewer->open(grlDir.Covers + CoverBack);
		ImgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgEliminar_CoverBack_clicked()
{
	CoverBack = "";
	ui->lbImg_CoverBack->setPixmap( QPixmap(fGrl->Theme() +"images/juego_sin_imagen.png") );
	ui->btnImgVer_CoverBack->setEnabled( false );
	ui->btnImgEliminar_CoverBack->setEnabled( false );
}

// Descripción
void frmAddEditJuego::on_btnTool_Cortar_clicked()
{
// Cortar texto
	ui->txtDatos_Comentario->cut();
}

void frmAddEditJuego::on_btnTool_Copiar_clicked()
{
// Copiar texto
	ui->txtDatos_Comentario->copy();
}

void frmAddEditJuego::on_btnTool_Pegar_clicked()
{
// Pegar texto
	ui->txtDatos_Comentario->paste();
}

void frmAddEditJuego::on_btnTool_SelectAll_clicked()
{
// Seleccionar el texto
	ui->txtDatos_Comentario->selectAll();
}

void frmAddEditJuego::on_btnTool_Deshacer_clicked()
{
// Deshacer texto
	ui->txtDatos_Comentario->undo();
}

void frmAddEditJuego::on_btnTool_Rehacer_clicked()
{
// Rehacer texto
	ui->txtDatos_Comentario->redo();
}

void frmAddEditJuego::on_btnTool_TextoNegrita_clicked()
{
// Texto en negrita
	ui->txtDatos_Comentario->textCursor().insertText("<strong>"+ ui->txtDatos_Comentario->textCursor().selectedText() +"</strong>");
}

void frmAddEditJuego::on_btnTool_TextoCursiva_clicked()
{
// Texto en cursiva
	ui->txtDatos_Comentario->textCursor().insertText("<em>"+ ui->txtDatos_Comentario->textCursor().selectedText() +"</em>");
}

void frmAddEditJuego::on_btnTool_TextoSubrayado_clicked()
{
// Texto subrayado
	ui->txtDatos_Comentario->textCursor().insertText("<u>"+ ui->txtDatos_Comentario->textCursor().selectedText() +"</u>");
}

void frmAddEditJuego::on_btnTool_InsertarImg_clicked()
{
// Insertar una imagen en el texto
	ui->txtDatos_Comentario->textCursor().insertText("<img src=\"direccion_imagen\" />");
}

void frmAddEditJuego::on_btnTool_InsertaUrl_clicked()
{
// Insertar una url en el texto
	ui->txtDatos_Comentario->textCursor().insertText("<a href=\""+ ui->txtDatos_Comentario->textCursor().selectedText() +"\">"+ ui->txtDatos_Comentario->textCursor().selectedText() +"</a>");
}

void frmAddEditJuego::on_btnTool_Buscar_clicked(bool checked)
{
// Buscar texto
	ui->btnTool_Buscar->setChecked(checked);

	if( ui->btnTool_Reemplazar->isChecked() && checked )
		ui->btnTool_Reemplazar->setChecked(!checked);

	ui->lb_reemplazar_por->setVisible(!checked);
	ui->txtDatos_Reeplazar->setVisible(!checked);
	ui->btnTool_ReemplazarTexto->setVisible(!checked);

	ui->frame_find->setVisible(checked);
}

void frmAddEditJuego::on_btnTool_Reemplazar_clicked(bool checked)
{
// Reemlazar texto
	if( ui->btnTool_Buscar->isChecked() && checked )
		ui->btnTool_Buscar->setChecked(!checked);

	ui->btnTool_Reemplazar->setChecked(checked);
	ui->lb_reemplazar_por->setVisible(checked);
	ui->txtDatos_Reeplazar->setVisible(checked);
	ui->btnTool_ReemplazarTexto->setVisible(checked);

	ui->frame_find->setVisible(checked);
}

void frmAddEditJuego::on_btnTool_Preview_clicked()
{
// Preview texto-html
	if( ui->btnTool_Preview->isChecked() )
	{
		ui->html_preview->document()->clear();
		ui->html_preview->clear();
		foreach (const stGrlDatos &smile, smiles_list)
			ui->html_preview->document()->addResource(QTextDocument::ImageResource, QUrl("smile_rs_"+ smile.icono), QImage(grlDir.Smiles + smile.icono));
		ui->txtDatos_Comentario->setVisible(false);
		ui->html_preview->setVisible(true);
		ui->html_preview->setHtml( fGrl->reemplazaTextoSmiles(ui->txtDatos_Comentario->toPlainText(), smiles_list) );
	} else {
		ui->txtDatos_Comentario->setVisible(true);
		ui->html_preview->setVisible(false);
	}
}

void frmAddEditJuego::on_btnTool_ReemplazarTexto_clicked()
{
// Reemplaza el texto seleccionado
	if( ui->txtDatos_Comentario->textCursor().selectedText() == ui->txtDatos_Buscar->text() )
		ui->txtDatos_Comentario->insertPlainText( ui->txtDatos_Reeplazar->text() );
}

void frmAddEditJuego::buscarTexto(QString texto, bool anterior)
{
	QTextDocument::FindFlags flags;

	ui->txtDatos_Comentario->setFocus(Qt::ShortcutFocusReason);

	if ( anterior )
		flags |= QTextDocument::FindBackward;
	if ( ui->chkDatos_SoloPalabrasCompletas->isChecked() )
		flags |= QTextDocument::FindWholeWords;
	if ( ui->chkDatos_CoincideMayusculas->isChecked() )
		flags |= QTextDocument::FindCaseSensitively;

	ui->txtDatos_Comentario->find(texto, flags);
}

void frmAddEditJuego::on_btnTool_BuscarAnterior_clicked()
{
// Buscar texto anterior
	buscarTexto(ui->txtDatos_Buscar->text(), true);
}

void frmAddEditJuego::on_btnTool_BuscarSiguiente_clicked()
{
// Buscar texto siguiente
	buscarTexto(ui->txtDatos_Buscar->text(), false);
}

void frmAddEditJuego::on_twDatoSmile_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->txtDatos_Comentario->textCursor().insertText(" "+ item->text(0) +" ");
		if( ui->btnTool_Preview->isChecked() )
			ui->html_preview->setHtml( fGrl->reemplazaTextoSmiles(ui->txtDatos_Comentario->toPlainText(), smiles_list) );
	}
}

// Otros Datos
void frmAddEditJuego::on_btnDatos_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.DatosFiles_PathExe, fGrl->getDirRelative(ui->txtDatos_path_exe->text()), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDatos_path_exe->setText( fGrl->setDirRelative(archivo) );
		grlCfg.DatosFiles_PathExe = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DatosFiles_PathExe", grlCfg.DatosFiles_PathExe);
	}
}

void frmAddEditJuego::on_btnDatos_ExeJuego_clear_clicked()
{
	ui->txtDatos_path_exe->clear();
}

void frmAddEditJuego::on_btnDatos_ParametrosExe_clear_clicked()
{
	ui->txtDatos_parametros_exe->clear();
}

void frmAddEditJuego::on_btnDatos_SetupJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.DatosFiles_PathSetup, fGrl->getDirRelative(ui->txtDatos_path_setup->text()), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDatos_path_setup->setText( fGrl->setDirRelative(archivo) );
		grlCfg.DatosFiles_PathSetup = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DatosFiles_PathSetup", grlCfg.DatosFiles_PathSetup);
	}
}

void frmAddEditJuego::on_btnDatos_SetupJuego_clear_clicked()
{
	ui->txtDatos_path_setup->clear();
}

void frmAddEditJuego::on_btnDatos_ParametrosSetup_clear_clicked()
{
	ui->txtDatos_parametros_setup->clear();
}

void frmAddEditJuego::on_btnDatos_PathCapturas_clicked()
{
	QDir dir;
	QString directorio = fGrl->ventanaDirectorios(tr("Selecciona un archivo"), grlCfg.DatosFiles_PathCapturas, fGrl->getDirRelative(ui->txtDatos_path_capturas->text()) );

	if( dir.exists(directorio) )
	{
		ui->txtDatos_path_capturas->setText( fGrl->setDirRelative(directorio) );
		grlCfg.DatosFiles_PathCapturas = ui->txtDatos_path_capturas->text();

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DatosFiles_PathCapturas", grlCfg.DatosFiles_PathCapturas);
	}
}

void frmAddEditJuego::on_btnDatos_PathCapturas_clear_clicked()
{
	ui->txtDatos_path_capturas->clear();
}

// Direcciones url
void frmAddEditJuego::enabledUrlUpdate(QString texto, int col)
{
	int pos = ui->twDatosURL->indexOfTopLevelItem(ui->twDatosURL->currentItem());
	if( ui->twDatosURL->topLevelItemCount() > 0 && pos != -1 )
	{
		if( texto != ui->twDatosURL->currentItem()->text(col) )
			ui->btnUpdateUrl->setEnabled(true);
		else
			ui->btnUpdateUrl->setEnabled(false);
	} else
		ui->btnUpdateUrl->setEnabled(false);
}

void frmAddEditJuego::addEditTwDatosURL(bool isNew)
{
	if( !ui->cbxUrl_url->currentText().isEmpty() )
	{
		stDatosUrls datos_url;
		datos_url.idgrl       = IdGame;
		datos_url.idcat       = categoria.id;
		datos_url.url         = ui->cbxUrl_url->currentText();
		datos_url.descripcion = ui->txtUrl_descripcion->toPlainText();

		QTreeWidgetItem *item_url = NULL;
		if( isNew )
		{
			item_url = new QTreeWidgetItem( ui->twDatosURL );

			if( Editando )
				datos_url.id = sql->insertaUnURL( datos_url );
			else
				datos_url.id = "";

			ui->cbxUrl_url->addItem(QIcon(fGrl->Theme() +"img16/edit_enlace.png"), datos_url.url, datos_url.descripcion);
		} else {
			int pos = ui->twDatosURL->indexOfTopLevelItem(ui->twDatosURL->currentItem());
			if( ui->twDatosURL->topLevelItemCount() > 0 && pos != -1 )
				item_url = ui->twDatosURL->currentItem();
			else
				item_url = new QTreeWidgetItem( ui->twDatosURL );

			if( Editando )
			{
				datos_url.id = item_url->text(2);
				sql->actualizaURL( datos_url );
			}

			ui->btnUpdateUrl->setEnabled(false);
		}

		item_url->setIcon( 0, QIcon(fGrl->Theme() +"img16/html.png") );
		item_url->setText( 0, datos_url.url         );
		item_url->setText( 1, datos_url.descripcion );
		item_url->setText( 2, datos_url.id          );
		item_url->setText( 3, datos_url.idgrl       );
		item_url->setText( 4, datos_url.idcat       );
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
	if( index > -1 )
		ui->txtUrl_descripcion->setPlainText( ui->cbxUrl_url->itemData( index ).toString() );
	else
		ui->txtUrl_descripcion->setPlainText("");
}

void frmAddEditJuego::on_btnAddUrl_clicked()
{
	addEditTwDatosURL(true);
}

void frmAddEditJuego::on_btnUpdateUrl_clicked()
{
	addEditTwDatosURL(false);
}

void frmAddEditJuego::on_btnAbrirUrl_clicked()
{
// Abre la url con el navegador por defecto
	QTreeWidgetItem * item = NULL;
	item = ui->twDatosURL->currentItem();
	if( item )
		QDesktopServices::openUrl( QUrl( item->text(0) ) );
}

void frmAddEditJuego::on_btnDeleteUrl_clicked()
{
	if( ui->twDatosURL->topLevelItemCount() > 0 )
	{
		QString id_url = ui->twDatosURL->currentItem()->text(2);
		int respuesta = QMessageBox::question(this, tr("¿Eliminar url...?"), tr("¿Deseas eliminar esta url?"), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
		{
			fGrl->deleteItemTree( ui->twDatosURL->currentItem() );
			if( Editando && !id_url.isEmpty() )
				sql->eliminarURL( id_url );

			ui->cbxUrl_url->clear();
			ui->cbxUrl_url->addItem(QIcon(fGrl->Theme() +"img16/edit_enlace.png"), "", "");
			QSqlQuery query(sql->getSqlDB());
			query.exec("SELECT DISTINCT url, descripcion FROM dbgrl_urls");
			if( query.first() )
			{
				do {
					ui->cbxUrl_url->addItem(QIcon(fGrl->Theme() +"img16/edit_enlace.png"), query.record().value("url").toString(), query.record().value("descripcion").toString());
				} while ( query.next() );
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
	if( item && column > -1 )
	{
		ui->cbxUrl_url->setEditText( item->text(0) );
		ui->txtUrl_descripcion->setPlainText( item->text(1) );
	}
}

void frmAddEditJuego::on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
		emit on_btnAbrirUrl_clicked();
}

// Datos usuario
void frmAddEditJuego::on_btnDatos_Usuario_clear_clicked()
{
	ui->txtDatos_Usuario->clear();
}

// Datos archivos
void frmAddEditJuego::enabledDatosUpdate(QString texto, int col)
{
	int pos = ui->twDatosFiles->indexOfTopLevelItem(ui->twDatosFiles->currentItem());
	if( ui->twDatosFiles->topLevelItemCount() > 0 && pos != -1 )
	{
		if( texto != ui->twDatosFiles->currentItem()->text(col) )
			ui->btnUpdateFile->setEnabled(true);
		else
			ui->btnUpdateFile->setEnabled(false);
	} else
		ui->btnUpdateFile->setEnabled(false);
}

void frmAddEditJuego::addEditTwDatosFiles(bool isNew)
{
	if( !ui->txtDatosFiles_PathFile->text().isEmpty() )
	{
		stDatosFiles datos_file;
		datos_file.idgrl       = IdGame;
		datos_file.idcat       = categoria.id;
		datos_file.nombre      = ui->txtDatosFiles_FileName->text();
		datos_file.crc         = ui->txtDatosFiles_Crc32->text();
		datos_file.descripcion = ui->txtDatosFiles_Comentario->toPlainText();
		datos_file.path        = ui->txtDatosFiles_PathFile->text();
		datos_file.size        = ui->txtDatosFiles_Size->text();
		datos_file.tipo        = ui->cbxDatos_TipoArchivo->itemData( ui->cbxDatos_TipoArchivo->currentIndex() ).toString();

		QTreeWidgetItem *item_file = NULL;
		if( isNew )
		{
			item_file = new QTreeWidgetItem( ui->twDatosFiles );

			if( Editando )
				datos_file.id = sql->insertaUnFiles( datos_file );
			else
				datos_file.id = "";
		} else {
			int pos = ui->twDatosFiles->indexOfTopLevelItem(ui->twDatosFiles->currentItem());
			if( ui->twDatosFiles->topLevelItemCount() > 0 && pos != -1 )
				item_file = ui->twDatosFiles->currentItem();
			else
				item_file = new QTreeWidgetItem( ui->twDatosFiles );

			if( Editando )
			{
				datos_file.id = item_file->text(6);
				sql->actualizaFiles( datos_file );
			}
			ui->btnUpdateFile->setEnabled(false);
		}

		QString file_ico;
		if( datos_file.tipo == "manual" )
			file_ico = fGrl->Theme() +"img16/datos.png";
		else if( datos_file.tipo == "pdf" )
			file_ico = fGrl->Theme() +"img16/pdf.png";
		else if( datos_file.tipo == "ruleta" )
			file_ico = fGrl->Theme() +"img16/ruleta.png";
		else if( datos_file.tipo == "archivo" )
			file_ico = fGrl->Theme() +"img16/archivos.png";
		else
			file_ico = fGrl->Theme() +"img16/archivos.png";

		item_file->setTextAlignment(1, Qt::AlignCenter);
		item_file->setTextAlignment(3, Qt::AlignCenter);
		item_file->setIcon( 0, QIcon(file_ico)        );
		item_file->setText( 0, datos_file.nombre      );
		item_file->setText( 1, datos_file.crc         );
		item_file->setText( 2, datos_file.descripcion );
		item_file->setText( 3, fGrl->covertir_bytes( datos_file.size.toFloat() ) );
		item_file->setText( 4, datos_file.path        );
		item_file->setText( 5, datos_file.tipo        );
		item_file->setText( 6, datos_file.id          );
		item_file->setText( 7, datos_file.idgrl       );
		item_file->setText( 8, datos_file.idcat       );
		item_file->setText( 9, datos_file.size        );
	}
}

void frmAddEditJuego::on_txtDatosFiles_PathFile_textEdited(const QString &arg1)
{
	enabledDatosUpdate(arg1, 4);
}

void frmAddEditJuego::on_txtDatosFiles_FileName_textEdited(const QString &arg1)
{
	enabledDatosUpdate(arg1, 0);
}

void frmAddEditJuego::on_txtDatosFiles_Comentario_textChanged()
{
	enabledDatosUpdate(ui->txtDatosFiles_Comentario->toPlainText(), 2);
}

void frmAddEditJuego::on_btnDatosFiles_PathFile_clicked()
{
	QString archivo, tipo_archivo;
	switch ( ui->cbxDatos_TipoArchivo->currentIndex() )
	{
		case 0:
			tipo_archivo = tr("Documentos - Manuales") +" - (*.cbz *.zip);;"+ tr("Todos los archivo") +" (*)";
		break;
		case 1:
			tipo_archivo = tr("Documentos - Manuales") +" - (*.pdf);;"+ tr("Todos los archivo") +" (*)";
		break;
		case 2:
			tipo_archivo = tr("Ruleta de protección") +" - (*.conf *.zip);;"+ tr("Todos los archivo") +" (*)";
		break;
		case 3:
			tipo_archivo = tr("Todos los archivo") +" (*)";
		break;
		default:
			tipo_archivo = tr("Todos los archivo") +" (*)";
		break;
	}

	archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.DatosFiles_PathFile, fGrl->getDirRelative(ui->txtDatosFiles_PathFile->text()), tipo_archivo);

	stFileInfo f_info = fGrl->getInfoFile(archivo, hashCrc32);
	if( f_info.Exists )
	{
		grlCfg.DatosFiles_PathFile = f_info.Path;
		ui->txtDatosFiles_PathFile->setText( fGrl->setDirRelative(archivo) );
		ui->txtDatosFiles_FileName->setText( f_info.NameExt  );
		ui->txtDatosFiles_Crc32->setText( f_info.Crc32 );
		ui->txtDatosFiles_Size->setText( f_info.Size );
		ui->txtDatosFiles_Comentario->setText("");
		ui->lb_files_size->setText( fGrl->covertir_bytes( f_info.Size.toFloat() ) );
		enabledDatosUpdate(fGrl->setDirRelative(archivo), 4);

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DatosFiles_PathFile", grlCfg.DatosFiles_PathFile);
	} else
		emit on_btnDatosFiles_PathFile_clear_clicked();
}

void frmAddEditJuego::on_btnDatosFiles_PathFile_clear_clicked()
{
	ui->txtDatosFiles_PathFile->clear();
	ui->txtDatosFiles_FileName->clear();
	ui->txtDatosFiles_Crc32->clear();
	ui->txtDatosFiles_Size->clear();
	ui->txtDatosFiles_Comentario->clear();
}

void frmAddEditJuego::on_btnAddFile_clicked()
{
	addEditTwDatosFiles(true);
}

void frmAddEditJuego::on_btnUpdateFile_clicked()
{
	addEditTwDatosFiles(false);
}

void frmAddEditJuego::on_btnDeleteFile_clicked()
{
	if( ui->twDatosFiles->topLevelItemCount() > 0 )
	{
		QString id_file = ui->twDatosFiles->currentItem()->text(6);
		int respuesta = QMessageBox::question(this, tr("¿Eliminar archivo...?"), tr("¿Deseas eliminar este archivo?"), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
		{
			fGrl->deleteItemTree( ui->twDatosFiles->currentItem() );
			if( Editando && !id_file.isEmpty() )
				sql->eliminarFiles( id_file );

			emit on_btnDatosFiles_PathFile_clear_clicked();
		}
	}
}

void frmAddEditJuego::on_twDatosFiles_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		ui->txtDatosFiles_FileName->setText( item->text(0) );
		ui->txtDatosFiles_Crc32->setText( item->text(1) );
		ui->txtDatosFiles_Comentario->setPlainText( item->text(2) );
		ui->txtDatosFiles_PathFile->setText( item->text(4));
		ui->txtDatosFiles_Size->setText( item->text(9) );
		ui->cbxDatos_TipoArchivo->setCurrentIndex( ui->cbxDatos_TipoArchivo->findData( item->text(5) ) );
		ui->lb_files_size->setText( fGrl->covertir_bytes( item->text(9).toFloat() ) );
	}
}

void frmAddEditJuego::on_twDatosFiles_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		stFileInfo archivo = fGrl->getInfoFile( fGrl->getDirRelative(item->text(4)) );
		if( item->text(5) == "manual" )
		{
			if( archivo.Ext == ".cbz" || archivo.Ext == ".zip")
			{
				frmImgViewer *ImgViewer = new frmImgViewer(grlCfg, this);
				ImgViewer->setWindowFlags(Qt::Window);
				ImgViewer->openZip( archivo.FilePath );
				ImgViewer->show();
			} else
				fGrl->abrirArchivo( archivo.FilePath );
		}
		else if( item->text(5) == "pdf" )
		{
			if( grlCfg.OpenPdfExternal || archivo.Ext != ".pdf")
				fGrl->abrirArchivo( archivo.FilePath );
			else {
				frmPdfViewer *PdfViewer = new frmPdfViewer(this);
				PdfViewer->setWindowFlags(Qt::Window);
				PdfViewer->openPdf( archivo.FilePath );
				PdfViewer->show();
			}
		}
		else if( item->text(5) == "ruleta" )
		{
			if( archivo.Ext == ".conf" || archivo.Ext == ".zip")
			{
				frmRuleta *Ruleta = new frmRuleta(grlCfg, this);
				Ruleta->setWindowFlags(Qt::Window);
				Ruleta->cargarRuleta(archivo.FilePath);
				Ruleta->show();
			} else
				fGrl->abrirArchivo( archivo.FilePath );
		} else
			fGrl->abrirArchivo( archivo.FilePath );
	}
}
