/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006  Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#include "grlida_addedit_juego.h"
#include "grlida_addedit_url.h"
#include "grlida_img_viewer.h"
#include "grlida_importar_juego.h"
#include "grlida_ruleta.h"

frmAddEditJuego::frmAddEditJuego(bool EditJuego, QString TipoEmu, QString stIDIndex, QDialog *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
	ui.setupUi(this);

	EditandoJuego = EditJuego;
	TipoEmulador  = TipoEmu;
	stItemIDGrl   = stIDIndex;

	stHomeDir     = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stIconDir     = stHomeDir + "iconos/";	// directorio de iconos para el GR-lida
	stDatosDir    = stHomeDir + "datos/";	// directorio para los distintos datos del GR-lida

	stTheme = fGrl.ThemeGrl();
	setTheme();

// Conecta los distintos botones con las funciones.
	createConnections();

	setUpdatesEnabled( false );

	CargarConfig();

	setUpdatesEnabled( true );

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAddEditJuego::~frmAddEditJuego()
{
	//
}

void frmAddEditJuego::closeEvent( QCloseEvent *e )
{
	e->ignore();
}

void frmAddEditJuego::createConnections()
{
// Conecta los distintos botones con las funciones.
	connect( ui.btnOk           , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.cbxDatos_TipoEmu, SIGNAL( activated(int) ), this, SLOT( on_cbxDatos_TipoEmu_Changed(int) ) );

// Referente al DatosJuego ---------------------------------------
	connect( ui.btnImgAbrir_Thumbs        , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_Thumbs() ) );
	connect( ui.btnImgAbrir_CoverFront    , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_CoverFront() ) );
	connect( ui.btnImgAbrir_CoverBack     , SIGNAL( clicked() ), this, SLOT( on_btnImgAbrir_CoverBack() ) );
	connect( ui.btnImgVer_Thumbs          , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_Thumbs() ) );
	connect( ui.btnImgVer_CoverFront      , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_CoverFront() ) );
	connect( ui.btnImgVer_CoverBack       , SIGNAL( clicked() ), this, SLOT( on_btnImgVer_CoverBack() ) );
	connect( ui.btnImgEliminar_Thumbs     , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_Thumbs() ) );
	connect( ui.btnImgEliminar_CoverFront , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_CoverFront() ) );
	connect( ui.btnImgEliminar_CoverBack  , SIGNAL( clicked() ), this, SLOT( on_btnImgEliminar_CoverBack() ) );
	connect( ui.btnDescargarInfo          , SIGNAL( clicked() ), this, SLOT( on_btnDescargarInfo() ) );
	connect( ui.twDatoSmile , SIGNAL( itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( on_twDatoSmile_Dblclicked(QTreeWidgetItem*) ));

// Conecta los botones para editar los comentarios de los juegos
	connect( ui.btnTool_Cortar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Cortar() ) );
	connect( ui.btnTool_Copiar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Copiar() ) );
	connect( ui.btnTool_Pegar           , SIGNAL( clicked() ), this, SLOT( on_btnTool_Pegar() ) );
	connect( ui.btnTool_SelectAll       , SIGNAL( clicked() ), this, SLOT( on_btnTool_SelectAll() ) );
	connect( ui.btnTool_Deshacer        , SIGNAL( clicked() ), this, SLOT( on_btnTool_Deshacer() ) );
	connect( ui.btnTool_Rehacer         , SIGNAL( clicked() ), this, SLOT( on_btnTool_Rehacer() ) );
	connect( ui.btnTool_TextoNegrita    , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoNegrita() ) );
	connect( ui.btnTool_TextoCursiva    , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoCursiva() ) );
	connect( ui.btnTool_TextoSubrayado  , SIGNAL( clicked() ), this, SLOT( on_btnTool_TextoSubrayado() ) );
	connect( ui.btnTool_InsertarImg     , SIGNAL( clicked() ), this, SLOT( on_btnTool_InsertarImg() ) );
	connect( ui.btnTool_InsertaUrl      , SIGNAL( clicked() ), this, SLOT( on_btnTool_InsertaUrl() ) );
	connect( ui.btnTool_Buscar          , SIGNAL( clicked() ), this, SLOT( on_btnTool_Buscar() ) );
	connect( ui.btnTool_BuscarAnterior  , SIGNAL( clicked() ), this, SLOT( on_btnTool_BuscarAnterior() ) );
	connect( ui.btnTool_BuscarSiguiente , SIGNAL( clicked() ), this, SLOT( on_btnTool_BuscarSiguiente() ) );
	connect( ui.btnTool_Reemplazar      , SIGNAL( clicked() ), this, SLOT( on_btnTool_Reemplazar() ) );
	connect( ui.btnTool_Preview         , SIGNAL( clicked() ), this, SLOT( on_btnTool_Preview() ) );
//----
	connect( ui.btnNuevaUrl    , SIGNAL( clicked() ), this, SLOT( on_btnNuevaUrl()    ) );
	connect( ui.btnEditarUrl   , SIGNAL( clicked() ), this, SLOT( on_btnEditarUrl()   ) );
	connect( ui.btnEliminarUrl , SIGNAL( clicked() ), this, SLOT( on_btnEliminarUrl() ) );
	connect( ui.btnAbrirUrl    , SIGNAL( clicked() ), this, SLOT( on_btnAbrirUrl()    ) );
//----
	connect( ui.btnDatosFiles_PathFile , SIGNAL( clicked() ), this, SLOT( on_btnDatosFiles_PathFile() ) );
	connect( ui.btnAddFile     , SIGNAL( clicked() ), this, SLOT( on_btnAddFile()     ) );
	connect( ui.btnEditFile    , SIGNAL( clicked() ), this, SLOT( on_btnEditFile()    ) );
	connect( ui.btnUpdateFile  , SIGNAL( clicked() ), this, SLOT( on_btnUpdateFile()  ) );
	connect( ui.btnDeleteFile  , SIGNAL( clicked() ), this, SLOT( on_btnDeleteFile()  ) );
	connect( ui.twDatosFiles   , SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT( on_twDatosFiles_Dblclicked(QTreeWidgetItem*) ));
//----

// Referente a VDMSound ------------------------------------------
	connect( ui.btnVdms_FileConfg, SIGNAL( clicked() ), this, SLOT( on_btnVdms_FileConfg() ) );
	connect( ui.btnVdms_ExeJuego , SIGNAL( clicked() ), this, SLOT( on_btnVdms_ExeJuego()  ) );
	connect( ui.btnVdms_Icono    , SIGNAL( clicked() ), this, SLOT( on_btnVdms_Icono()     ) );
}

void frmAddEditJuego::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/datos_1.png") );

// General
	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnDescargarInfo->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.tabWidget_Datos->setTabIcon(0, QIcon(stTheme+"img16/datos_2.png") );
	ui.tabWidget_Datos->setTabIcon(1, QIcon(stTheme+"img16/nuevo.png") );
	ui.tabWidget_Datos->setTabIcon(2, QIcon(stTheme+"img16/importar.png") );
	ui.tabWidget_Datos->setTabIcon(3, QIcon(stTheme+"img16/dosbox.png") );
	ui.tabWidget_Datos->setTabIcon(4, QIcon(stTheme+"img16/scummvm.png") );
	ui.tabWidget_Datos->setTabIcon(5, QIcon(stTheme+"img16/vdmsound.png") );
// Datos
	ui.btnImgAbrir_Thumbs->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnImgAbrir_CoverFront->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnImgAbrir_CoverBack->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnImgVer_Thumbs->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnImgVer_CoverFront->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnImgVer_CoverBack->setIcon( QIcon(stTheme+"img16/capturas.png") );
	ui.btnImgEliminar_Thumbs->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnImgEliminar_CoverFront->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnImgEliminar_CoverBack->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnTool_Cortar->setIcon( QIcon(stTheme+"img16/edit_cut.png") );
	ui.btnTool_Copiar->setIcon( QIcon(stTheme+"img16/edit_copy.png") );
	ui.btnTool_Pegar->setIcon( QIcon(stTheme+"img16/edit_paste.png") );
	ui.btnTool_SelectAll->setIcon( QIcon(stTheme+"img16/edit_select_all.png") );
	ui.btnTool_Deshacer->setIcon( QIcon(stTheme+"img16/edit_deshacer.png") );
	ui.btnTool_Rehacer->setIcon( QIcon(stTheme+"img16/edit_rehacer.png") );
	ui.btnTool_TextoNegrita->setIcon( QIcon(stTheme+"img16/edit_negrita.png") );
	ui.btnTool_TextoCursiva->setIcon( QIcon(stTheme+"img16/edit_cursiva.png") );
	ui.btnTool_TextoSubrayado->setIcon( QIcon(stTheme+"img16/edit_subrayada.png") );
	ui.btnTool_InsertarImg->setIcon( QIcon(stTheme+"img16/edit_imagen.png") );
	ui.btnTool_InsertaUrl->setIcon( QIcon(stTheme+"img16/edit_enlace.png") );
	ui.btnTool_Buscar->setIcon( QIcon(stTheme+"img16/edit_buscar.png") );
	ui.btnTool_BuscarAnterior->setIcon( QIcon(stTheme+"img16/edit_buscar_anterior.png") );
	ui.btnTool_BuscarSiguiente->setIcon( QIcon(stTheme+"img16/edit_buscar_siguiente.png") );
	ui.btnTool_Reemplazar->setIcon( QIcon(stTheme+"img16/edit_reemplazar.png") );
	ui.btnTool_Preview->setIcon( QIcon(stTheme+"img16/edit_preview.png") );
	ui.btnNuevaUrl->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.btnEditarUrl->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.btnEliminarUrl->setIcon( QIcon(stTheme+"img16/eliminar.png") );
	ui.btnAbrirUrl->setIcon( QIcon(stTheme+"img16/edit_enlace.png") );
	ui.btnDatosFiles_PathFile->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnDatosFiles_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnAddFile->setIcon( QIcon(stTheme+"img16/nuevo.png") );
	ui.btnEditFile->setIcon( QIcon(stTheme+"img16/editar.png") );
	ui.btnUpdateFile->setIcon( QIcon(stTheme+"img16/actualizar.png") );
	ui.btnDeleteFile->setIcon( QIcon(stTheme+"img16/eliminar.png") );

// VDMSound
	ui.btnVdms_FileConfg->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_ExeJuego->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_Icono->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_FileConfg_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_ExeJuego_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnVdms_Icono_clear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
}

void frmAddEditJuego::CargarConfig()
{
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	ui.cbxDatos_TipoEmu->clear();
	ui.cbxDatos_TipoEmu->addItem(QIcon(stTheme+"img16/datos_1.png") , tr("Datos")   , "datos"   );
	ui.cbxDatos_TipoEmu->addItem(QIcon(stTheme+"img16/dosbox.png")  , tr("DOSBox")  , "dosbox"  );
	ui.cbxDatos_TipoEmu->addItem(QIcon(stTheme+"img16/scummvm.png") , tr("ScummVM") , "scummvm" );
	ui.cbxDatos_TipoEmu->addItem(QIcon(stTheme+"img16/vdmsound.png"), tr("VDMSound"), "vdmsound");

	int rowTipoEmu = ui.cbxDatos_TipoEmu->findData( TipoEmulador );
	if( rowTipoEmu < 0 ) rowTipoEmu = 0;
	ui.cbxDatos_TipoEmu->setCurrentIndex( rowTipoEmu );
	on_cbxDatos_TipoEmu_Changed( rowTipoEmu );

	ui.tabWidget_Datos->setTabEnabled (3, false ); // tab DOSBox
	ui.tabWidget_Datos->setTabEnabled (4, false ); // tab ScummVM
	ui.tabWidget_Datos->setTabEnabled (5, false ); // tab vdmsound

	ui.wDbx->ui.twMontajes->header()->setStretchLastSection(false);
	ui.wDbx->ui.twMontajes->header()->setMovable(false);

	ui.twDatosFiles->header()->setStretchLastSection(true);
	ui.twDatosFiles->header()->setMovable(false);
	ui.twDatosFiles->header()->setResizeMode(0, QHeaderView::Interactive);
	ui.twDatosFiles->header()->setResizeMode(1, QHeaderView::Fixed      );
	ui.twDatosFiles->header()->setResizeMode(2, QHeaderView::Interactive);
	ui.twDatosFiles->header()->setResizeMode(3, QHeaderView::Fixed      );
	ui.twDatosFiles->header()->setResizeMode(4, QHeaderView::Interactive);
	ui.twDatosFiles->setColumnWidth(0, 200 );// nombre
	ui.twDatosFiles->setColumnWidth(1, 50  );// crc32
	ui.twDatosFiles->setColumnWidth(2, 80  );// descripcion
	ui.twDatosFiles->setColumnWidth(3, 50  );// size
	ui.twDatosFiles->setColumnWidth(4, 50  );//

	ui.twDatosURL->header()->setStretchLastSection(true);
	ui.twDatosURL->header()->setMovable(false);
	ui.twDatosURL->header()->setResizeMode(QHeaderView::Interactive);
	ui.twDatosURL->setColumnWidth(0, 100 );

// Referente al DatosJuego ---------------------------------------
	filters.clear();
	filters << "*.bmp" << "*.jpg" << "*.png" << "*.gif"; // Imagenes soportadas
	fGrl.CargarIconosComboBox(stIconDir, ui.cbxDatos_Icono, filters);

	fGrl.CargarDatosComboBox(stDatosDir + "generos.txt"  , ui.cbxDatos_Genero       , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_Compania     , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "companias.txt", ui.cbxDatos_Desarrollador, 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "tema.txt"     , ui.cbxDatos_Tema         , 1, false);
	fGrl.CargarDatosComboBox(":/datos/svm_idioma.txt"    , ui.cbxDatos_Idioma       , 1, true );
	fGrl.CargarDatosComboBox(stDatosDir + "formatos.txt" , ui.cbxDatos_Formato      , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "fechas.txt"   , ui.cbxDatos_Anno         , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "numdisc.txt"  , ui.cbxDatos_NumDisc      , 1, false);
	fGrl.CargarDatosComboBox(stDatosDir + "sistemaop.txt", ui.cbxDatos_SistemaOp    , 1, false);

	for(int n = 1; n < 11; n++)
		ui.cbxDatos_Graficos->addItem(QIcon(stTheme+"img16/grafica.png"),fGrl.IntToStr(n));

	for(int n = 1; n < 11; n++)
		ui.cbxDatos_Sonido->addItem(QIcon(stTheme+"img16/grafica.png"),fGrl.IntToStr(n));

	for(int n = 1; n < 11; n++)
		ui.cbxDatos_Jugabilidad->addItem(QIcon(stTheme+"img16/grafica.png"),fGrl.IntToStr(n));

	listSmailes.clear();
	listSmailes = fGrl.Cargar_Smiles( stDatosDir + "smiles.txt", ui.twDatoSmile);

	ui.html_preview->setVisible(false);
	m_rating = 0;

	ui.cbxDatos_TipoArchivo->clear();
	ui.cbxDatos_TipoArchivo->addItem(QIcon(stTheme+"img16/datos_1.png") , tr("Documentos - Manuales")+" - (cbz - zip)", "manual");
	ui.cbxDatos_TipoArchivo->addItem(QIcon(stTheme+"img16/ruleta.png")  , tr("Ruleta de protección")+" - (conf - zip)", "ruleta");
	ui.cbxDatos_TipoArchivo->addItem(QIcon(stTheme+"img16/archivos.png"), tr("Cualquier archivos"), "");

	if( EditandoJuego == true )
		CargarDatosJuego( stItemIDGrl );
	else {
		ui.lb_fechahora->setText( fGrl.HoraFechaActual(GRLConfig["FormatoFecha"].toString()) );
		stThumbs     = "";
		stCoverFront = "";
		stCoverBack  = "";
		stUsuario    = "";
		ui.lbImg_Thumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.lbImg_CoverFront->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.lbImg_CoverBack->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.btnImgVer_Thumbs->setEnabled( false );
		ui.btnImgVer_CoverFront->setEnabled( false );
		ui.btnImgVer_CoverBack->setEnabled( false );
		ui.btnImgEliminar_Thumbs->setEnabled( false );
		ui.btnImgEliminar_CoverFront->setEnabled( false );
		ui.btnImgEliminar_CoverBack->setEnabled( false );
		ui.cbxDatos_Genero->setCurrentIndex(0);
		ui.cbxDatos_Compania->setCurrentIndex(0);
		ui.cbxDatos_Desarrollador->setCurrentIndex(0);
		ui.cbxDatos_Tema->setCurrentIndex(0);
		ui.cbxDatos_Idioma->setCurrentIndex(0);
		ui.cbxDatos_Formato->setCurrentIndex(0);
		ui.cbxDatos_Anno->setCurrentIndex(0);
		ui.cbxDatos_NumDisc->setCurrentIndex(0);
		ui.cbxDatos_Icono->setCurrentIndex(0);
		ui.cbxDatos_SistemaOp->setCurrentIndex(0);
		ui.cbxDatos_Graficos->setCurrentIndex(0);
		ui.cbxDatos_Sonido->setCurrentIndex(0);
		ui.cbxDatos_Jugabilidad->setCurrentIndex(0);
		ui.cbxDatos_Estado->setCurrentIndex(0);
	}
// Fin del DatosJuego --------------------------------------------

// Referente al ScummVM ------------------------------------------
	if( EditandoJuego == false || (EditandoJuego == true && TipoEmulador == "scummvm") )
	{
		ui.wSvm->CargarConfigScummVM();
		ui.wSvm->setEditandoJuego( EditandoJuego );
		if( EditandoJuego == true )
		{
			ui.tabWidget_Datos->setTabEnabled(4, true );	// tab ScummVM
			ui.wSvm->CargarDatosScummVM( stItemIDGrl );		// Carga los Datos del ScummVM
		} else
			ui.wSvm->setConfigDefecto();
	}
// Fin del ScummVM -----------------------------------------------

// Referente a DOSBox --------------------------------------------
	if( EditandoJuego == false || (EditandoJuego == true && TipoEmulador == "dosbox") )
	{
		ui.wDbx->CargarConfigDosBox();
		ui.wDbx->setEditandoJuego( EditandoJuego );
		if( EditandoJuego == true )
		{
			ui.tabWidget_Datos->setTabEnabled(3, true );	// tab DOSBox
			ui.wDbx->CargarDatosDosBox( stItemIDGrl );		// Carga los Datos del DOSBox
		} else
			ui.wDbx->setConfigDefecto();
	}
// Fin del DOSBox ------------------------------------------------

// Referente a VDMSound ------------------------------------------
	if( EditandoJuego == false || (EditandoJuego == true && TipoEmulador == "vdmsound") )
	{
		if( EditandoJuego == true )
		{
			ui.tabWidget_Datos->setTabEnabled(5, true );						// tab VDMSound
			stItemIDVdms = sql->ItemIDIndex("dbgrl_emu_vdmsound", stItemIDGrl);	// Obtenemos el Index del VDMSound
			CargarDatosVDMSound( stItemIDGrl );									// Carga los Datos del VDMSound
		}
	}
// Fin del VDMSound ----------------------------------------------

	if( EditandoJuego == true )
	{
		ui.cbxDatos_TipoEmu->setEnabled( false );
		setWindowTitle( tr("Editando a") + ": " + ui.txtDatos_Titulo->text() );
	} else {
		ui.cbxDatos_TipoEmu->setEnabled( true );
		setWindowTitle( tr("Añadiendo un nuevo juego") );
	}
}

void frmAddEditJuego::on_btnOk()
{
	bool siguiente;
	QString currentTipoEmu = ui.cbxDatos_TipoEmu->itemData( ui.cbxDatos_TipoEmu->currentIndex() ).toString();

//DOSBox minimo archivo conf, ejecutable
	if( ui.txtDatos_Titulo->text() != "" )
	{
		siguiente=true;
		if( currentTipoEmu == "dosbox")
		{
			if( ui.wDbx->ui.txtDbx_path_conf->text() != "" )
			{
				siguiente = true;
				if( ui.wDbx->ui.txtDbx_path_exe->text() == "")
				{
					siguiente = false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " DOSBox");
				} else {
					siguiente = true;
					if( EditandoJuego != true )
					{
						QFile appConfg( stHomeDir + "confdbx/"+ ui.wDbx->ui.txtDbx_path_conf->text() );
						if( appConfg.exists() )
						{
							siguiente = false;
							ui.wDbx->ui.txtDbx_path_conf->setText("");
							QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el DOSBox ya esixte"));
						} else
							siguiente = true;
					}
				}
			} else {
				siguiente = false;
				QMessageBox::information( this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " DOSBox");
			}
		}
		if( currentTipoEmu == "scummvm" )
		{//IF de scummvm
			if( ui.wSvm->ui.txtSvm_game_label->text() == "" )
			{
				siguiente=false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner la etiqueta del juego de")+ " ScummVM" );
			} else {
				siguiente=true;
				if( ui.wSvm->ui.txtSvm_path->text() == "" )
				{
					siguiente=false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Directorio del juego de")+ " ScummVM" );
				} else {
					siguiente=true;
					QDir tmpdirsvm( ui.wSvm->ui.txtSvm_path->text() );
					if(!tmpdirsvm.exists())
					{
						siguiente=false;
						QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar un Directorio que exista") );
					} else
						siguiente=true;
				}
			}
		}//fin IF de scummvm
		if( currentTipoEmu == "vdmsound")
		{
			if( ui.txtVdms_path_conf->text() != "" )
			{
				siguiente = true;
				if( ui.txtVdms_path_exe->text() == "" )
				{
					siguiente=false;
					QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el Ejecutable del juego para el")+ " VDMSound");
				} else {
					siguiente = true;
					if( EditandoJuego != true )
					{
						QFile appConfg( stHomeDir + "confvdms/"+ ui.txtVdms_path_conf->text() );
						if( appConfg.exists() )
						{
							siguiente = false;
							ui.txtVdms_path_conf->setText("");
							QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
						} else
							siguiente = true;
					}
				}
			} else {
				siguiente = false;
				QMessageBox::information(this, stTituloAddEdit(), tr("Debes indicar el archivo de Configuración para el")+ " VDMSound");
			}
		}
	} else {
		siguiente = false;
		QMessageBox::information(this, stTituloAddEdit(), tr("Debes poner un Titulo al juego"));
	}
	if( siguiente == true )
	{
		setDatosJuegos();

		if( currentTipoEmu == "dosbox" )
		{
			DatosDosBox.clear();
			DatosDosBox = ui.wDbx->setDatosDosBox();
		}

		if( currentTipoEmu == "scummvm" )
		{
			DatosScummVM.clear();
			DatosScummVM = ui.wSvm->setDatosScummVM();
		}

		if( currentTipoEmu == "vdmsound" )
			setDatosVDMSound();

		QDialog::accept();
	}
}

// Referente al DatosJuego ---------------------------------------
void frmAddEditJuego::CargarDatosJuego(QString stIDIndex)
{
	QHash<QString, QString> strDatosJuego;
	QSqlQuery query;

	strDatosJuego = sql->show_Datos( stIDIndex );

	stUsuario = strDatosJuego["Dat_usuario"];
	m_rating = fGrl.StrToInt( strDatosJuego["Dat_rating"] );
	ui.lb_fechahora->setText( strDatosJuego["Dat_fecha"] );						// fecha
	ui.txtDatos_Titulo->setText( strDatosJuego["Dat_titulo"] );					// titulo
	ui.txtDatos_Subtitulo->setText( strDatosJuego["Dat_subtitulo"] );			// subtitulo
	ui.txtDatos_Tamano->setText( strDatosJuego["Dat_tamano"] );					// tamano
	ui.txtDatos_Comentario->setPlainText( strDatosJuego["Dat_comentario"] );	// comentario

	ui.cbxDatos_Genero->addItem( QIcon(stTheme+"img16/datos_3.png"), strDatosJuego["Dat_genero"] );					// genero
	ui.cbxDatos_Compania->addItem( QIcon(stTheme+"img16/datos_3.png"), strDatosJuego["Dat_compania"] );				// compania
	ui.cbxDatos_Desarrollador->addItem( QIcon(stTheme+"img16/datos_3.png"), strDatosJuego["Dat_desarrollador"] );	// desarrollador
	ui.cbxDatos_Tema->addItem( QIcon(stTheme+"img16/datos_3.png"), strDatosJuego["Dat_tema"] );						// tema
	ui.cbxDatos_Idioma->addItem( QIcon(stTheme+"img16/sinimg.png"), strDatosJuego["Dat_idioma"] );					// idioma
	ui.cbxDatos_Formato->addItem( QIcon(stTheme+"img16/sinimg.png"), strDatosJuego["Dat_formato"] );				// formato
	ui.cbxDatos_Anno->addItem( QIcon(stTheme+"img16/fecha.png"), strDatosJuego["Dat_anno"] );						// anno
	ui.cbxDatos_NumDisc->addItem( QIcon(stTheme+"img16/sinimg.png"), strDatosJuego["Dat_numdisc"] );				// numdisc
	ui.cbxDatos_SistemaOp->addItem( QIcon(stTheme+"img16/sinimg.png"), strDatosJuego["Dat_sistemaop"] );			// sistemaop
	ui.cbxDatos_Estado->addItem( QIcon(stTheme+"img16/sinimg.png"), strDatosJuego["Dat_estado"] );					// estado

// Selecciona el dato correspondiente.
	ui.cbxDatos_Genero->setCurrentIndex( ui.cbxDatos_Genero->findText( strDatosJuego["Dat_genero"] ) );						// genero
	ui.cbxDatos_Compania->setCurrentIndex( ui.cbxDatos_Compania->findText( strDatosJuego["Dat_compania"] ) );				// compania
	ui.cbxDatos_Desarrollador->setCurrentIndex( ui.cbxDatos_Desarrollador->findText( strDatosJuego["Dat_desarrollador"] ) );// desarrollador
	ui.cbxDatos_Tema->setCurrentIndex( ui.cbxDatos_Tema->findText( strDatosJuego["Dat_tema"] ) );							// tema
	ui.cbxDatos_Idioma->setCurrentIndex( ui.cbxDatos_Idioma->findText( strDatosJuego["Dat_idioma"] ) );						// idioma
	ui.cbxDatos_Formato->setCurrentIndex( ui.cbxDatos_Formato->findText( strDatosJuego["Dat_formato"] ) );					// formato
	ui.cbxDatos_Anno->setCurrentIndex( ui.cbxDatos_Anno->findText( strDatosJuego["Dat_anno"] ) );							// anno
	ui.cbxDatos_NumDisc->setCurrentIndex( ui.cbxDatos_NumDisc->findText( strDatosJuego["Dat_numdisc"] ) );					// numdisc
	ui.cbxDatos_SistemaOp->setCurrentIndex( ui.cbxDatos_SistemaOp->findText( strDatosJuego["Dat_sistemaop"] ) );			// sistemaop
	ui.cbxDatos_Graficos->setCurrentIndex( ui.cbxDatos_Graficos->findText( strDatosJuego["Dat_graficos"] ) );				// graficos
	ui.cbxDatos_Sonido->setCurrentIndex( ui.cbxDatos_Sonido->findText( strDatosJuego["Dat_sonido"] ) );						// sonido
	ui.cbxDatos_Jugabilidad->setCurrentIndex( ui.cbxDatos_Jugabilidad->findText( strDatosJuego["Dat_jugabilidad"] ) );		// jugabilidad
	ui.cbxDatos_Estado->setCurrentIndex( ui.cbxDatos_Estado->findText( strDatosJuego["Dat_estado"] ) );						// estado
	ui.chkDatos_Original->setChecked( fGrl.StrToBool( strDatosJuego["Dat_original"] ) );									// original
	ui.chkDatos_Favorito->setChecked( fGrl.StrToBool( strDatosJuego["Dat_favorito"] ) );									// favorito

// icono
	if( strDatosJuego["Dat_icono"] == "datos" || strDatosJuego["Dat_icono"] == "" )
		ui.cbxDatos_Icono->setCurrentIndex(0);
	else if( strDatosJuego["Dat_icono"] == "dosbox" )
		ui.cbxDatos_Icono->setCurrentIndex(1);
	else if( strDatosJuego["Dat_icono"] == "scummvm" )
		ui.cbxDatos_Icono->setCurrentIndex(2);
	else if( strDatosJuego["Dat_icono"] =="vdmsound" )
		ui.cbxDatos_Icono->setCurrentIndex(3);
	else
		ui.cbxDatos_Icono->setCurrentIndex( ui.cbxDatos_Icono->findData( strDatosJuego["Dat_icono"] ) );	// icono

	stThumbs.clear();
	stCoverFront.clear();
	stCoverBack.clear();
	stThumbs     = strDatosJuego["Dat_thumbs"];			// thumbs
	stCoverFront = strDatosJuego["Dat_cover_front"];	// cover_front
	stCoverBack  = strDatosJuego["Dat_cover_back"];		// cover_back

	if( file_thumbs.exists(stThumbs) )
	{
		ui.lbImg_Thumbs->setPixmap( QPixmap(stThumbs) );
		ui.btnImgVer_Thumbs->setEnabled( true );
		ui.btnImgEliminar_Thumbs->setEnabled( true );
	} else {
		ui.lbImg_Thumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.btnImgVer_Thumbs->setEnabled( false );
		ui.btnImgEliminar_Thumbs->setEnabled( false );
	}

	if( file_cover_front.exists(stCoverFront) )
	{
		ui.lbImg_CoverFront->setPixmap( QPixmap(stCoverFront) );
		ui.btnImgVer_CoverFront->setEnabled( true );
		ui.btnImgEliminar_CoverFront->setEnabled( true );
	} else {
		ui.lbImg_CoverFront->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.btnImgVer_CoverFront->setEnabled( false );
		ui.btnImgEliminar_CoverFront->setEnabled( false );
	}

	if( file_cover_back.exists(stCoverBack) )
	{
		ui.lbImg_CoverBack->setPixmap( QPixmap(stCoverBack) );
		ui.btnImgVer_CoverBack->setEnabled( true );
		ui.btnImgEliminar_CoverBack->setEnabled( true );
	} else {
		ui.lbImg_CoverBack->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
		ui.btnImgVer_CoverBack->setEnabled( false );
		ui.btnImgEliminar_CoverBack->setEnabled( false );
	}

	ui.twDatosURL->clear();
	query.exec("SELECT * FROM dbgrl_url WHERE idgrl="+stIDIndex);
	if( query.first() )
	{
		do {
			QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
			item_url->setText( 0, query.record().value("url").toString()         ); // url
			item_url->setIcon( 0, QIcon(stTheme+"img16/edit_enlace.png")         ); // icono
			item_url->setText( 1, query.record().value("descripcion").toString() ); // descripcion
			item_url->setText( 2, query.record().value("id").toString()          ); // id
			item_url->setText( 3, query.record().value("idgrl").toString()       ); // idgrl
		} while (query.next());
		ui.twDatosURL->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}

	ui.twDatosFiles->clear();
	query.exec("SELECT * FROM dbgrl_file WHERE idgrl="+stIDIndex);
	if( query.first() )
	{
		do {
			QTreeWidgetItem *item_files = new QTreeWidgetItem( ui.twDatosFiles );

			QString file_ico = query.record().value("tipo").toString();
			if( file_ico == "manual" )
				item_files->setIcon( 0, QIcon(stTheme+"img16/datos_1.png") ); // icono
			else if( file_ico == "ruleta" )
				item_files->setIcon( 0, QIcon(stTheme+"img16/ruleta.png") ); // icono
			else if( file_ico == "archivo" )
				item_files->setIcon( 0, QIcon(stTheme+"img16/archivos.png") ); // icono
			else
				item_files->setIcon( 0, QIcon(stTheme+"img16/archivos.png") ); // icono

			item_files->setText( 0, query.record().value("nombre").toString()      ); // nombre
			item_files->setText( 1, query.record().value("crc").toString()         ); // crc
			item_files->setText( 2, query.record().value("descripcion").toString() ); // descripcion
			item_files->setText( 3, query.record().value("size").toString()        ); // size
			item_files->setText( 4, query.record().value("path").toString()        ); // path
			item_files->setText( 5, query.record().value("id").toString()          ); // id
			item_files->setText( 6, query.record().value("idgrl").toString()       ); // idgrl
			item_files->setText( 7, query.record().value("tipo").toString()        ); // tipo
		} while (query.next());
		ui.twDatosFiles->sortItems( 0, Qt::AscendingOrder ); // Qt::DescendingOrder
	}
}

void frmAddEditJuego::setDatosJuegos()
{
	DatosJuego.clear();

	if( ui.cbxDatos_Icono->currentText() != "" )
		DatosJuego["Dat_icono"] = ""+ui.cbxDatos_Icono->itemData( ui.cbxDatos_Icono->currentIndex() ).toString();	// icono
	else
		DatosJuego["Dat_icono"] = ""+TipoEmulador;

	DatosJuego["Dat_titulo"]    = ui.txtDatos_Titulo->text();						// titulo
	DatosJuego["Dat_subtitulo"] = ui.txtDatos_Subtitulo->text();					// subtitulo

	if( ui.cbxDatos_Genero->currentText() != "" )
		DatosJuego["Dat_genero"] = ui.cbxDatos_Genero->currentText();				// genero
	else
		DatosJuego["Dat_genero"] = "";

	if( ui.cbxDatos_Compania->currentText() != "" )
		DatosJuego["Dat_compania"] = ui.cbxDatos_Compania->currentText();			// compania
	else
		DatosJuego["Dat_compania"] = "";

	if( ui.cbxDatos_Desarrollador->currentText() != "" )
		DatosJuego["Dat_desarrollador"] = ui.cbxDatos_Desarrollador->currentText();	// desarrollador
	else
		DatosJuego["Dat_desarrollador"] = "";

	if( ui.cbxDatos_Tema->currentText() !="" )
		DatosJuego["Dat_tema"] = ui.cbxDatos_Tema->currentText();					// tema
	else
		DatosJuego["Dat_tema"] = "";

	if( ui.cbxDatos_Idioma->currentText() != "" )
		DatosJuego["Dat_idioma"] = ui.cbxDatos_Idioma->currentText();				// idioma
	else
		DatosJuego["Dat_idioma"] = "";

	if( ui.cbxDatos_Formato->currentText() != "" )
		DatosJuego["Dat_formato"] = ui.cbxDatos_Formato->currentText();				// formato
	else
		DatosJuego["Dat_formato"] = "";

	if( ui.cbxDatos_Anno->currentText() != "" )
		DatosJuego["Dat_anno"] = ui.cbxDatos_Anno->currentText();					// anno
	else
		DatosJuego["Dat_anno"] = "";

	if( ui.cbxDatos_NumDisc->currentText() != "" )
		DatosJuego["Dat_numdisc"] = ui.cbxDatos_NumDisc->currentText();				// numdisc
	else
		DatosJuego["Dat_numdisc"] = "";

	if( ui.cbxDatos_SistemaOp->currentText() != "" )
		DatosJuego["Dat_sistemaop"] = ui.cbxDatos_SistemaOp->currentText();			// sistemaop
	else
		DatosJuego["Dat_sistemaop"] = "";

	DatosJuego["Dat_tamano"] = ui.txtDatos_Tamano->text();							// tamano

	if( ui.cbxDatos_Graficos->currentText() != "" )
		DatosJuego["Dat_graficos"] = ui.cbxDatos_Graficos->currentText();			// graficos
	else
		DatosJuego["Dat_graficos"] = "1";

	if( ui.cbxDatos_Sonido->currentText() != "" )
		DatosJuego["Dat_sonido"] = ui.cbxDatos_Sonido->currentText();				// sonido
	else
		DatosJuego["Dat_sonido"] = "1";

	if( ui.cbxDatos_Jugabilidad->currentText() != "" )
		DatosJuego["Dat_jugabilidad"] = ui.cbxDatos_Jugabilidad->currentText();		// jugabilidad
	else
		DatosJuego["Dat_jugabilidad"] = "1";

	DatosJuego["Dat_original"] = fGrl.BoolToStr( ui.chkDatos_Original->isChecked() ); // original

	if( ui.cbxDatos_Estado->currentText() != "" )
		DatosJuego["Dat_estado"] = ui.cbxDatos_Estado->currentText();	// estado
	else
		DatosJuego["Dat_estado"] = "";

	DatosJuego["Dat_thumbs"]      = stThumbs;						// thumbs
	DatosJuego["Dat_cover_front"] = stCoverFront;					// cover_front
	DatosJuego["Dat_cover_back"]  = stCoverBack;					// cover_back
	DatosJuego["Dat_fecha"]       = ui.lb_fechahora->text();		// fecha d/m/a h:m:s

	if( ui.cbxDatos_TipoEmu->currentText() != "" )
		DatosJuego["Dat_tipo_emu"] = ui.cbxDatos_TipoEmu->itemData( ui.cbxDatos_TipoEmu->currentIndex() ).toString();	// tipo_emu
	else
		DatosJuego["Dat_tipo_emu"] = ""+TipoEmulador;

	DatosJuego["Dat_comentario"] = ui.txtDatos_Comentario->toPlainText();	// comentario
	DatosJuego["Dat_favorito"]   = fGrl.BoolToStr( ui.chkDatos_Favorito->isChecked() ); // favorito
	DatosJuego["Dat_rating"]     = fGrl.IntToStr( m_rating );
	DatosJuego["Dat_usuario"]    = stUsuario;
}

void frmAddEditJuego::on_btnImgAbrir_Thumbs()
{
	stThumbs = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Img_Thumbs"].toString(), stThumbs,
										  tr("Imagenes Soportadas")+" (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;"+
										  tr("Todos los archivo") + " (*)", 0, false);
	if( file_thumbs.exists(stThumbs) )
	{
		ui.lbImg_Thumbs->setPixmap( QPixmap(stThumbs) );
		ui.btnImgVer_Thumbs->setEnabled( true );
		ui.btnImgEliminar_Thumbs->setEnabled( true );

		QFileInfo fi( stThumbs );
		GRLConfig["Img_Thumbs"] = fi.absolutePath();
	} else {
		on_btnImgEliminar_Thumbs();
		GRLConfig["Img_Thumbs"] = "";
	}

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Img_Thumbs", GRLConfig["Img_Thumbs"].toString() );
}

void frmAddEditJuego::on_btnImgAbrir_CoverFront()
{
	stCoverFront = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Img_CoverFront"].toString(), stCoverFront,
											  tr("Imagenes Soportadas")+" (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;"+
											  tr("Todos los archivo") + " (*)", 0, false);
	if( file_cover_front.exists(stCoverFront) )
	{
		ui.lbImg_CoverFront->setPixmap( QPixmap(stCoverFront) );
		ui.btnImgVer_CoverFront->setEnabled( true );
		ui.btnImgEliminar_CoverFront->setEnabled( true );

		QFileInfo fi( stCoverFront );
		GRLConfig["Img_CoverFront"] = fi.absolutePath();
	} else {
		on_btnImgEliminar_CoverFront();
		GRLConfig["Img_CoverFront"] = "";
	}

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Img_CoverFront", GRLConfig["Img_CoverFront"].toString() );
}

void frmAddEditJuego::on_btnImgAbrir_CoverBack()
{
	stCoverBack = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Img_CoverBack"].toString(), stCoverBack,
											 tr("Imagenes Soportadas")+" (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;"+
											 tr("Todos los archivo") + " (*)", 0, false);
	if( file_cover_back.exists(stCoverBack) )
	{
		ui.lbImg_CoverBack->setPixmap( QPixmap(stCoverBack) );
		ui.btnImgVer_CoverBack->setEnabled( true );
		ui.btnImgEliminar_CoverBack->setEnabled( true );

		QFileInfo fi( stCoverBack );
		GRLConfig["Img_CoverBack"] = fi.absolutePath();
	} else {
		on_btnImgEliminar_CoverBack();
		GRLConfig["Img_CoverBack"] = "";
	}

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Img_CoverBack", GRLConfig["Img_CoverBack"].toString() );
}

void frmAddEditJuego::on_btnImgVer_Thumbs()
{
	// Abre la imagen para verla en su tamaño original
	if( file_thumbs.exists(stThumbs) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stThumbs );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgVer_CoverFront()
{
	// Abre la imagen para verla en su tamaño original
	if( file_cover_front.exists(stCoverFront) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCoverFront );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgVer_CoverBack()
{
	// Abre la imagen para verla en su tamaño original
	if( file_cover_back.exists(stCoverBack) )
	{
		ImageViewer *imgViewer = new ImageViewer(this);
		imgViewer->setWindowModality(Qt::WindowModal);
		imgViewer->open( stCoverBack );
		imgViewer->show();
	}
}

void frmAddEditJuego::on_btnImgEliminar_Thumbs()
{
	stThumbs = "";
	ui.lbImg_Thumbs->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
	ui.btnImgVer_Thumbs->setEnabled( false );
	ui.btnImgEliminar_Thumbs->setEnabled( false );
}

void frmAddEditJuego::on_btnImgEliminar_CoverFront()
{
	stCoverFront = "";
	ui.lbImg_CoverFront->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
	ui.btnImgVer_CoverFront->setEnabled( false );
	ui.btnImgEliminar_CoverFront->setEnabled( false );
}

void frmAddEditJuego::on_btnImgEliminar_CoverBack()
{
	stCoverBack = "";
	ui.lbImg_CoverBack->setPixmap( QPixmap(stTheme+"images/juego_sin_imagen.png") );
	ui.btnImgVer_CoverBack->setEnabled( false );
	ui.btnImgEliminar_CoverBack->setEnabled( false );
}

void frmAddEditJuego::on_btnDescargarInfo()
{
	frmImportarJuego * ImportarJuego = new frmImportarJuego(ui.txtDatos_Titulo->text(), 0, Qt::Window);
	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		ui.txtDatos_Titulo->setText( ImportarJuego->DatosJuego["Dat_titulo"] );					// titulo
		ui.txtDatos_Subtitulo->setText( ImportarJuego->DatosJuego["Dat_subtitulo"] );			// subtitulo
		ui.cbxDatos_Genero->setEditText( ImportarJuego->DatosJuego["Dat_genero"] );
		ui.cbxDatos_Compania->setEditText( ImportarJuego->DatosJuego["Dat_compania"] );
		ui.cbxDatos_Desarrollador->setEditText( ImportarJuego->DatosJuego["Dat_desarrollador"] );
		ui.cbxDatos_Tema->setEditText( ImportarJuego->DatosJuego["Dat_tema"] );
		ui.cbxDatos_Idioma->setEditText( ImportarJuego->DatosJuego["Dat_idioma"] );
		ui.cbxDatos_Formato->setEditText( ImportarJuego->DatosJuego["Dat_formato"] );
		ui.cbxDatos_Anno->setEditText( ImportarJuego->DatosJuego["Dat_anno"] );
		ui.cbxDatos_NumDisc->setEditText( ImportarJuego->DatosJuego["Dat_numdisc"] );
		ui.cbxDatos_SistemaOp->setEditText( ImportarJuego->DatosJuego["Dat_sistemaop"] );
		ui.txtDatos_Tamano->setText( ImportarJuego->DatosJuego["Dat_tamano"] );					// tamano
		ui.cbxDatos_Graficos->setCurrentIndex( ui.cbxDatos_Graficos->findText( ImportarJuego->DatosJuego["Dat_graficos"] ) );
		ui.cbxDatos_Sonido->setCurrentIndex( ui.cbxDatos_Sonido->findText( ImportarJuego->DatosJuego["Dat_sonido"] ) );
		ui.cbxDatos_Jugabilidad->setCurrentIndex( ui.cbxDatos_Jugabilidad->findText( ImportarJuego->DatosJuego["Dat_jugabilidad"] ) );
		ui.chkDatos_Original->setChecked( fGrl.StrToBool( ImportarJuego->DatosJuego["Dat_original"] ) );
		ui.cbxDatos_Estado->setEditText( ImportarJuego->DatosJuego["Dat_estado"] );
		stThumbs     = ImportarJuego->DatosJuego["Dat_thumbs"];
		stCoverFront = ImportarJuego->DatosJuego["Dat_cover_front"];
		stCoverBack  = ImportarJuego->DatosJuego["Dat_cover_back"];
	//	fecha
	//	tipo_emu
		ui.txtDatos_Comentario->setPlainText( ImportarJuego->DatosJuego["Dat_comentario"] );	// comentario
		ui.chkDatos_Favorito->setChecked( fGrl.StrToBool( ImportarJuego->DatosJuego["Dat_favorito"] ) );
		m_rating  = fGrl.StrToInt( ImportarJuego->DatosJuego["Dat_rating"] );
		stUsuario = ImportarJuego->DatosJuego["Dat_usuario"];

		if( file_thumbs.exists(stThumbs) )
		{
			ui.lbImg_Thumbs->setPixmap( QPixmap(stThumbs) );
			ui.btnImgVer_Thumbs->setEnabled( true );
			ui.btnImgEliminar_Thumbs->setEnabled( true );
		} else
			on_btnImgEliminar_Thumbs();

		if( file_cover_front.exists(stCoverFront) )
		{
			ui.lbImg_CoverFront->setPixmap( QPixmap(stCoverFront) );
			ui.btnImgVer_CoverFront->setEnabled( true );
			ui.btnImgEliminar_CoverFront->setEnabled( true );
		} else
			on_btnImgEliminar_CoverFront();

		if( file_cover_back.exists(stCoverBack) )
		{
			ui.lbImg_CoverBack->setPixmap( QPixmap(stCoverBack) );
			ui.btnImgVer_CoverBack->setEnabled( true );
			ui.btnImgEliminar_CoverBack->setEnabled( true );
		} else
			on_btnImgEliminar_CoverBack();
	}
}

void frmAddEditJuego::on_cbxDatos_TipoEmu_Changed(int row)
{
	QString texto = ui.cbxDatos_TipoEmu->itemData( row ).toString();

	if( texto == "datos" )
		ui.cbxDatos_Icono->setCurrentIndex(0);

	if( texto == "dosbox" )
	{
		ui.tabWidget_Datos->setTabEnabled (3, true );	// tab dosbox
		ui.cbxDatos_Icono->setCurrentIndex(1);
	} else
		ui.tabWidget_Datos->setTabEnabled (3, false );

	if( texto == "scummvm" )
	{
		ui.tabWidget_Datos->setTabEnabled (4, true );	// tab scummvm
		ui.cbxDatos_Icono->setCurrentIndex(2);
	} else
		ui.tabWidget_Datos->setTabEnabled (4, false );

	if( texto == "vdmsound" )
	{
		ui.tabWidget_Datos->setTabEnabled (5, true );	// tab vdmsound
		ui.cbxDatos_Icono->setCurrentIndex(3);
	} else
		ui.tabWidget_Datos->setTabEnabled (5, false );
}

void frmAddEditJuego::on_twDatoSmile_Dblclicked(QTreeWidgetItem *item)
{
// preview texto-html
	if( item )
	{
		ui.txtDatos_Comentario->textCursor().insertText( " " + item->text(0) + " " );
		if( ui.btnTool_Preview->isChecked() )
			ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_Comentario->toPlainText(), listSmailes ) );
	} else
		return;
}

void frmAddEditJuego::on_btnTool_Cortar()
{
// cortar texto
	ui.txtDatos_Comentario->cut();
}

void frmAddEditJuego::on_btnTool_Copiar()
{
// copiar texto
	ui.txtDatos_Comentario->copy();
}

void frmAddEditJuego::on_btnTool_Pegar()
{
// pegar texto
	ui.txtDatos_Comentario->paste();
}

void frmAddEditJuego::on_btnTool_SelectAll()
{
// seleccionar el texto
	ui.txtDatos_Comentario->selectAll();
}

void frmAddEditJuego::on_btnTool_Deshacer()
{
// deshacer texto
	ui.txtDatos_Comentario->undo();
}

void frmAddEditJuego::on_btnTool_Rehacer()
{
// rehacer texto
	ui.txtDatos_Comentario->redo();
}

void frmAddEditJuego::on_btnTool_TextoNegrita()
{
// texto en negrita
	ui.txtDatos_Comentario->textCursor().insertText("<strong>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</strong>");
}

void frmAddEditJuego::on_btnTool_TextoCursiva()
{
// texto en cursiva
	ui.txtDatos_Comentario->textCursor().insertText("<em>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</em>");
}

void frmAddEditJuego::on_btnTool_TextoSubrayado()
{
// texto subrayado
	ui.txtDatos_Comentario->textCursor().insertText("<u>"+ui.txtDatos_Comentario->textCursor().selectedText()+"</u>");
}

void frmAddEditJuego::on_btnTool_InsertarImg()
{
// insertar una imagen en el texto
	ui.txtDatos_Comentario->textCursor().insertText("<img src=\"direccion_imagen\" />");
}

void frmAddEditJuego::on_btnTool_InsertaUrl()
{
// insertar una url en el texto
	ui.txtDatos_Comentario->textCursor().insertText("<a href=\""+ui.txtDatos_Comentario->textCursor().selectedText()+"\">"+ui.txtDatos_Comentario->textCursor().selectedText()+"</a>");
}

void frmAddEditJuego::on_btnTool_Buscar()
{
// buscar texto

}

void frmAddEditJuego::on_btnTool_BuscarAnterior()
{
// buscar texto anterior

}

void frmAddEditJuego::on_btnTool_BuscarSiguiente()
{
// buscar texto siguiente

}

void frmAddEditJuego::on_btnTool_Reemplazar()
{
// reemlazar texto

}

void frmAddEditJuego::on_btnTool_Preview()
{
// preview texto-html
	if( ui.btnTool_Preview->isChecked() )
	{
		ui.txtDatos_Comentario->setVisible(false);
		ui.html_preview->setVisible(true);
		ui.html_preview->setHtml( fGrl.ReemplazaTextoSmiles( ui.txtDatos_Comentario->toPlainText(), listSmailes ) );
	} else {
		ui.txtDatos_Comentario->setVisible(true);
		ui.html_preview->setVisible(false);
	}
}

void frmAddEditJuego::on_btnNuevaUrl()
{
	QString id_url;
	frmAddEditURL *AddEditURL = new frmAddEditURL();
	if( AddEditURL->exec() == QDialog::Accepted )
	{
		QHash<QString, QString> datos_url;
		datos_url.clear();
		datos_url["Url_url"]         = AddEditURL->ui.txt_addedit_url_1->currentText();
		datos_url["Url_descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText();

		if( stItemIDGrl != "")
			id_url = sql->ItemInsertaUnURL( datos_url , stItemIDGrl );
		else
			id_url = "";

		QTreeWidgetItem *item_url = new QTreeWidgetItem( ui.twDatosURL );
		item_url->setText( 0, datos_url["Url_url"]                   ); // url
		item_url->setIcon( 0, QIcon(stTheme+"img16/edit_enlace.png") ); // icono
		item_url->setText( 1, datos_url["Url_descripcion"]           ); // descripcion
		item_url->setText( 2, id_url                                 ); // id
		item_url->setText( 3, stItemIDGrl                            ); // idgrl
	}
	delete AddEditURL;
}

void frmAddEditJuego::on_btnEditarUrl()
{
	if( ui.twDatosURL->topLevelItemCount() > 0 && ui.twDatosURL->isItemSelected( ui.twDatosURL->currentItem() ) )
	{
		QString id_url;
		frmAddEditURL *AddEditURL = new frmAddEditURL();

		AddEditURL->ui.txt_addedit_url_1->setEditText(  ui.twDatosURL->currentItem()->text(0) );
		AddEditURL->ui.txt_addedit_url_2->setPlainText( ui.twDatosURL->currentItem()->text(1) );
		id_url = ui.twDatosURL->currentItem()->text(2);

		if( AddEditURL->exec() == QDialog::Accepted )
		{
			QHash<QString, QString> datos_url;
			datos_url.clear();
			datos_url["Url_url"]         = AddEditURL->ui.txt_addedit_url_1->currentText();
			datos_url["Url_descripcion"] = AddEditURL->ui.txt_addedit_url_2->toPlainText();

			ui.twDatosURL->currentItem()->setText( 0, datos_url["Url_url"]                   ); // url
			ui.twDatosURL->currentItem()->setIcon( 0, QIcon(stTheme+"img16/edit_enlace.png") ); // icono
			ui.twDatosURL->currentItem()->setText( 1, datos_url["Url_descripcion"]           ); // descripcion
			ui.twDatosURL->currentItem()->setText( 2, id_url                                 ); // id
			ui.twDatosURL->currentItem()->setText( 3, stItemIDGrl                            ); // idgrl

			if(id_url!="")
				sql->ItemActualizaURL( datos_url, id_url);
		}
		delete AddEditURL;
	}
}

void frmAddEditJuego::on_btnEliminarUrl()
{
	QString id_url;
	if( ui.twDatosURL->topLevelItemCount() > 0 )
	{
		id_url = ui.twDatosURL->currentItem()->text(2); // id
		int respuesta = QMessageBox::question( this, tr("¿Eliminar url...?"), tr("¿Deseas eliminar esta url?"), tr("Si"), tr("No"), 0, 1 );
		if( respuesta == 0 )
		{
			fGrl.DeleteItemTree( ui.twDatosURL->currentItem() );
			if( id_url != "" )
				sql->ItemEliminarURL( id_url );
		}
	}
}

void frmAddEditJuego::on_btnAbrirUrl()
{
// Abre la URL con el navegador por defecto
	QTreeWidgetItem * item = 0;
	item = ui.twDatosURL->currentItem();
	if( item )
	{
		my_url.clear();
		my_url = ui.twDatosURL->currentItem()->text(0);
		QDesktopServices::openUrl( my_url );
	}
}

void frmAddEditJuego::on_btnDatosFiles_PathFile()
{
	QString archivo, tipo_archivo;
	switch ( ui.cbxDatos_TipoArchivo->currentIndex() )
	{
		case 0:
			tipo_archivo = tr("Documentos - Manuales")+" - (*.cbz *.zip)";
		break;
		case 1:
			tipo_archivo = tr("Ruleta de protección")+" - (*.conf *.zip)";
		break;
		case 2:
			tipo_archivo = tr("Todos los archivo")+" (*)";
		break;
		default:
			tipo_archivo = tr("Todos los archivo")+" (*)";
		break;
	}
	archivo = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["DatosFiles_PathFile"].toString(), ui.txtDatosFiles_PathFile->text(), tipo_archivo, 0, false);
	QFileInfo fi( archivo );
	if( fi.exists() )
	{
		GRLConfig["DatosFiles_PathFile"] = fi.absolutePath();
		ui.txtDatosFiles_PathFile->setText(archivo);
		ui.txtDatosFiles_FileName->setText(fi.fileName());
		ui.txtDatosFiles_Crc32->setText("");
		ui.txtDatosFiles_Size->setText(QVariant(fi.size()).toString());
		ui.txtDatosFiles_Comentario->setText("");
	} else {
		GRLConfig["DatosFiles_PathFile"] = "";
		ui.txtDatosFiles_PathFile->setText("");
		ui.txtDatosFiles_FileName->setText("");
		ui.txtDatosFiles_Crc32->setText("");
		ui.txtDatosFiles_Size->setText("");
		ui.txtDatosFiles_Comentario->setText("");
	}
	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","DatosFiles_PathFile", GRLConfig["DatosFiles_PathFile"].toString() );
}

void frmAddEditJuego::on_btnAddFile()
{
	QString id_file;

	if( ui.txtDatosFiles_PathFile->text() != "" )
	{
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["Fil_nombre"]      = ui.txtDatosFiles_FileName->text();			// nombre
		datos_file["Fil_crc"]         = ui.txtDatosFiles_Crc32->text();				// crc32
		datos_file["Fil_descripcion"] = ui.txtDatosFiles_Comentario->toPlainText();	// descripcion
		datos_file["Fil_path"]        = ui.txtDatosFiles_PathFile->text();			// path
		datos_file["Fil_size"]        = ui.txtDatosFiles_Size->text();				// size
		datos_file["Fil_tipo"]        = ui.cbxDatos_TipoArchivo->itemData( ui.cbxDatos_TipoArchivo->currentIndex() ).toString(); // tipo

		if(stItemIDGrl != "")
			id_file = sql->ItemInsertaUnFiles( datos_file , stItemIDGrl );
		else
			id_file = "";

		QTreeWidgetItem *item_file = new QTreeWidgetItem( ui.twDatosFiles );

		QString file_ico;
		if( datos_file["Fil_tipo"] == "manual" )
			file_ico = stTheme+"img16/datos_1.png";
		else if( datos_file["Fil_tipo"] == "ruleta" )
			file_ico = stTheme+"img16/ruleta.png";
		else if( datos_file["Fil_tipo"] == "archivo" )
			file_ico = stTheme+"img16/archivos.png";
		else
			file_ico = 	stTheme+"img16/archivos.png";

		item_file->setIcon( 0, QIcon(file_ico)                     ); // icono
		item_file->setText( 0, datos_file["Fil_nombre"]            ); // nombre
		item_file->setText( 1, datos_file["Fil_crc"]               ); // crc32
		item_file->setText( 2, datos_file["Fil_descripcion"]       ); // descripcion
		item_file->setText( 3, datos_file["Fil_size"]              ); // size
		item_file->setText( 4, datos_file["Fil_path"]              ); // path
		item_file->setText( 5, id_file                             ); // id
		item_file->setText( 6, stItemIDGrl                         ); // idgrl
		item_file->setText( 7, datos_file["Fil_tipo"]              ); // tipo
	}
}

void frmAddEditJuego:: on_btnEditFile()
{
	if( ui.twDatosFiles->topLevelItemCount() > 0 && ui.twDatosFiles->isItemSelected( ui.twDatosFiles->currentItem() ) )
	{
		ui.btnAddFile->setEnabled( false );
		ui.btnEditFile->setEnabled( false );
		ui.btnUpdateFile->setEnabled( true );
		ui.btnDeleteFile->setEnabled( false );
		ui.twDatosFiles->setEnabled( false );
		ui.txtDatosFiles_FileName->setText( ui.twDatosFiles->currentItem()->text(0)   ); // nombre
		ui.txtDatosFiles_Crc32->setText( ui.twDatosFiles->currentItem()->text(1)      ); // crc32
		ui.txtDatosFiles_Comentario->setText( ui.twDatosFiles->currentItem()->text(2) ); // descripcion
		ui.txtDatosFiles_PathFile->setText( ui.twDatosFiles->currentItem()->text(4)   ); // path
		ui.txtDatosFiles_Size->setText( ui.twDatosFiles->currentItem()->text(3)       ); // size
		ui.cbxDatos_TipoArchivo->setCurrentIndex( ui.cbxDatos_TipoArchivo->findData( ui.twDatosFiles->currentItem()->text(7) ) ); // tipo
	}
}

void frmAddEditJuego::on_btnUpdateFile()
{
	if( ui.twDatosFiles->isItemSelected( ui.twDatosFiles->currentItem() ) )
	{
		QString id_file;
		QHash<QString, QString> datos_file;
		datos_file.clear();
		datos_file["Fil_nombre"]      = ui.txtDatosFiles_FileName->text();			// nombre
		datos_file["Fil_crc"]         = ui.txtDatosFiles_Crc32->text();				// crc32
		datos_file["Fil_descripcion"] = ui.txtDatosFiles_Comentario->toPlainText();	// descripcion
		datos_file["Fil_path"]        = ui.txtDatosFiles_PathFile->text();			// path
		datos_file["Fil_size"]        = ui.txtDatosFiles_Size->text();				// size
		datos_file["Fil_tipo"]        = ui.cbxDatos_TipoArchivo->itemData( ui.cbxDatos_TipoArchivo->currentIndex() ).toString(); // tipo

		id_file = ui.twDatosFiles->currentItem()->text(5); // id

		if(id_file!="")
			sql->ItemActualizaFiles( datos_file, id_file);

		QString file_ico;
		if( datos_file["Fil_tipo"] == "manual" )
			file_ico = stTheme+"img16/datos_1.png";
		else if( datos_file["Fil_tipo"] == "ruleta" )
			file_ico = stTheme+"img16/ruleta.png";
		else if( datos_file["Fil_tipo"] == "archivo" )
			file_ico = stTheme+"img16/archivos.png";
		else
			file_ico = 	stTheme+"img16/archivos.png";

		ui.twDatosFiles->currentItem()->setIcon(0, QIcon(file_ico)               ); // icono
		ui.twDatosFiles->currentItem()->setText(0, datos_file["Fil_nombre"]      ); // nombre
		ui.twDatosFiles->currentItem()->setText(1, datos_file["Fil_crc"]         ); // crc32
		ui.twDatosFiles->currentItem()->setText(2, datos_file["Fil_descripcion"] ); // descripcion
		ui.twDatosFiles->currentItem()->setText(3, datos_file["Fil_size"]        ); // size
		ui.twDatosFiles->currentItem()->setText(4, datos_file["Fil_path"]        ); // path
		ui.twDatosFiles->currentItem()->setText(7, datos_file["Fil_tipo"]        ); // tipo

	// Limpiamos la cajas de texto
		ui.txtDatosFiles_FileName->clear();
		ui.txtDatosFiles_Crc32->clear();
		ui.txtDatosFiles_Comentario->clear();
		ui.txtDatosFiles_PathFile->clear();
		ui.txtDatosFiles_Size->clear();

		ui.btnAddFile->setEnabled( true );
		ui.btnEditFile->setEnabled( true );
		ui.btnUpdateFile->setEnabled( false );
		ui.btnDeleteFile->setEnabled( true );
		ui.twDatosFiles->setEnabled( true );
	}
}

void frmAddEditJuego::on_btnDeleteFile()
{
	if( ui.twDatosFiles->topLevelItemCount() > 0 )
	{
		QString id_file;
		id_file = ui.twDatosFiles->currentItem()->text(5); // id
		int respuesta = QMessageBox::question( this, tr("¿Eliminar Archivo...?"), tr("¿Deseas eliminar esta Archivo?"), tr("Si"), tr("No"), 0, 1 );
		if( respuesta == 0 )
		{
			fGrl.DeleteItemTree( ui.twDatosFiles->currentItem() );
			if(id_file!="")
				sql->ItemEliminarFiles( id_file );
		}
	}
}

void frmAddEditJuego::on_twDatosFiles_Dblclicked(QTreeWidgetItem *item)
{
	if( item )
	{
		if( item->text(7) == "manual" )
		{
			ImageViewer *imgViewer = new ImageViewer(this);
			imgViewer->setWindowModality(Qt::WindowModal);
			imgViewer->show();
			imgViewer->openZip( item->text(4) );
		}
		else if( item->text(7) == "ruleta" )
		{
			frmRuleta *m_Ruleta = new frmRuleta(0, Qt::Window);
			m_Ruleta->setWindowModality(Qt::WindowModal);
			m_Ruleta->CargarRuleta(item->text(4));
			m_Ruleta->show();
		} else
			fGrl.abrirArchivo( item->text(4) );
	}
}

// ---------------------------------------------------------------
// Referente al VDMSound -----------------------------------------
void frmAddEditJuego::CargarDatosVDMSound(QString stIDvdms)
{
	QHash<QString, QString> tempVdmSound;

	tempVdmSound.clear();
	tempVdmSound = sql->showConfg_VDMSound(stIDvdms);

	ui.txtVdms_path_conf->setText( tempVdmSound["Vdms_path_conf"] );
	ui.txtVdms_path_exe->setText( tempVdmSound["Vdms_path_exe"] );

	QStringList list_program       = tempVdmSound["Vdms_program"].split("|");
	QStringList list_winnt_dos     = tempVdmSound["Vdms_winnt_dos"].split("|");
	QStringList list_vdms_debug    = tempVdmSound["Vdms_vdms_debug"].split("|");
	QStringList list_winnt_dosbox  = tempVdmSound["Vdms_winnt_dosbox"].split("|");
	QStringList list_winnt_storage = tempVdmSound["Vdms_winnt_storage"].split("|");

	ui.txtVdms_params->setText( list_program[0] );
	ui.txtVdms_icon->setText( list_program[1] );

	ui.chkVdms_useAutoexec->setChecked( fGrl.StrToBool( list_winnt_dos[0] ) );
	ui.txtVdms_autoexec->setPlainText( list_winnt_dos[1].replace("%000d%000a", "\n") );

	ui.chkVdms_useCustomCfg->setChecked( fGrl.StrToBool( list_vdms_debug[0] ) );
	ui.txtVdms_customCfg->setPlainText( list_vdms_debug[1].replace("%000d%000a", "\n") );

	ui.chkVdms_exitclose->setChecked( fGrl.StrToBool( list_winnt_dosbox[0] ) );
	ui.chkVdms_exitWarn->setChecked( fGrl.StrToBool( list_winnt_dosbox[1] ) );
	ui.chkVdms_fastPaste->setChecked( fGrl.StrToBool( list_winnt_dosbox[2] ) );

	ui.chkVdms_useCDROM->setChecked( fGrl.StrToBool( list_winnt_storage[0] ) );
	ui.chkVdms_useNetware->setChecked( fGrl.StrToBool( list_winnt_storage[1] ) );
}

void frmAddEditJuego::setDatosVDMSound()
{
	QString strTemp;
	DatosVDMSound.clear();
	DatosVDMSound["Vdms_path_conf"] = ui.txtVdms_path_conf->text();
	DatosVDMSound["Vdms_path_exe"]  = ui.txtVdms_path_exe->text();
	DatosVDMSound["Vdms_program_1"] = ui.txtVdms_params->text();
	DatosVDMSound["Vdms_program_2"] = ui.txtVdms_icon->text();

	DatosVDMSound["Vdms_winnt_dos_1"] = fGrl.BoolToStr( ui.chkVdms_useAutoexec->isChecked() ,true);
	DatosVDMSound["Vdms_winnt_dos_2"] = ui.txtVdms_autoexec->toPlainText().replace("\n", "%000d%000a");

	DatosVDMSound["Vdms_vdms_debug_1"] = fGrl.BoolToStr( ui.chkVdms_useCustomCfg->isChecked() ,true);
	DatosVDMSound["Vdms_vdms_debug_2"] = ui.txtVdms_customCfg->toPlainText().replace("\n", "%000d%000a");

	DatosVDMSound["Vdms_winnt_dosbox_1"] = fGrl.BoolToStr( ui.chkVdms_exitclose->isChecked() ,true);
	DatosVDMSound["Vdms_winnt_dosbox_2"] = fGrl.BoolToStr( ui.chkVdms_exitWarn->isChecked() ,true);
	DatosVDMSound["Vdms_winnt_dosbox_3"] = fGrl.BoolToStr( ui.chkVdms_fastPaste->isChecked() ,true);

	DatosVDMSound["Vdms_winnt_storage_1"] = fGrl.BoolToStr( ui.chkVdms_useCDROM->isChecked() ,true);
	DatosVDMSound["Vdms_winnt_storage_2"] = fGrl.BoolToStr( ui.chkVdms_useNetware->isChecked() ,true);
}

void frmAddEditJuego::on_btnVdms_FileConfg()
{
	bool str_ok;
	QString str, archivo;

	archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir + "confvdms/", ui.txtVdms_path_conf->text(), "Config VDMSound (*.vlp);;"+tr("Todos los archivo") + " (*)", 0, true);
	if(archivo != "")
	{
		QFile appConfg( archivo );
		if( EditandoJuego != true )
		{
			if( appConfg.exists() )
				QMessageBox::information( this, stTituloAddEdit(), tr("El archivo de Configuración para el VDMSound ya esixte"));
		}
		QFileInfo fi( archivo );
		str = fi.fileName();

		str = fGrl.eliminar_caracteres( str );
		str_ok = str.endsWith(".vlp");
		if(str_ok == false)
			str.append(".vlp");
		ui.txtVdms_path_conf->setText( str );
	} else
		ui.txtVdms_path_conf->setText( "" );
}

void frmAddEditJuego::on_btnVdms_ExeJuego()
{
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_path_exe"].toString(), ui.txtVdms_path_exe->text(), tr("Todos los archivo") + " (*)", 0, false) );

	QFileInfo fi( ui.txtVdms_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Vdms_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Vdms_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_path_exe", GRLConfig["Vdms_path_exe"].toString() );
}

void frmAddEditJuego::on_btnVdms_Icono()
{
	ui.txtVdms_icon->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_icon"].toString(), ui.txtVdms_icon->text(),
														tr("Iconos")+" (*.ico *.exe *.dll);;"+
														tr("Todos los archivo") + " (*)", 0, false)  );//+ ",0"

	QFileInfo fi( ui.txtVdms_icon->text() );
	if( fi.exists() )
		GRLConfig["Vdms_icon"] = fi.absolutePath();
	else
		GRLConfig["Vdms_icon"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_icon", GRLConfig["Vdms_icon"].toString() );
}
// ---------------------------------------------------------------
