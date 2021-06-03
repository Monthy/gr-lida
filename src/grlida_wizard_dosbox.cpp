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

#include "grlida_wizard_dosbox.h"
#include "grlida_addedit_montajes.h"
#include "grlida_instalar_juego.h"
#include "ui_wizard_dosbox.h"

frmWizardDosBox::frmWizardDosBox(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmWizardDosBox)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql          = m_sql;
	grlCfg       = m_cfg;
	categoria    = m_categoria;
//	isDbxSVN     = grlCfg.DOSBoxSVN;
	isDbxSVN     = false;
	index_wizard = 0;

	grlDir.Home       = fGrl->homePath();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";
	grlDir.Temp       = grlDir.Home +"temp/";
	grlDir.Templates  = grlDir.Home +"templates/";

	cargarConfig();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmWizardDosBox::~frmWizardDosBox()
{
	delete ui;
}

void frmWizardDosBox::cargarConfig()
{
	DatosJuego  = sql->getDefectDatos("dosbox");
	DatosDosBox = sql->getDefectDOSBox();

	dbx_list.clear();
	ui->cbxDbx_EmuKey->clear();

	QList<stGrlDatos> list = fGrl->cargaListaDatos(grlDir.Datos +"dbx_list.txt", 6, "|");
	const int listSize = list.size();
	if (listSize > 0)
	{
		for (int i = 0; i < listSize; ++i)
		{
			ui->cbxDbx_EmuKey->addItem(QIcon(fGrl->theme() +"img16/"+ list.at(i).icono), list.at(i).titulo, list.at(i).key);
			dbx_list.insert(list.at(i).key, list.at(i));
		}
	}

	fGrl->cargarProfilesComboBox(ui->cbxDbx_Profiles, grlDir.Templates, "img16/", "_defecto_");

	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_memsize  , grlDir.Datos +"dbx_memsize.txt"    , "img16/", "16"         , 2, "|"); // Cantidad de memoria para DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_core        , grlDir.Datos +"dbx_cpu_core.txt"   , "img16/", "auto"       , 3, "|"); // Núcleo de la CPU DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cycles      , grlDir.Datos +"dbx_cycles.txt"     , "img16/", "auto"       , 2, "|"); // Ciclos DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_sbtype , grlDir.Datos +"dbx_sbtype.txt"     , "img16/", "sb16"       , 3, "|", true, true); // Tipo Sound Blaste
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_oplrate, ":/datos/dbx_sb_oplrate.txt"       , "img16/", "44100"      , 2, "|"); // Sample rate of OPL
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_mpu401     , grlDir.Datos +"dbx_mpu401.txt"     , "img16/", "intelligent", 2, "|"); // MPU-401
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_device     , grlDir.Datos +"dbx_midi_device.txt", "img16/", "default"    , 2, "|"); // MIDI Device

	ui->cbxDbx_dos_umb->clear();
	ui->cbxDbx_dos_umb->addItem("true");
	ui->cbxDbx_dos_umb->addItem("false");
	ui->cbxDbx_dos_umb->addItem("max");

	QStringList sonido_frecuencias;
	sonido_frecuencias << "8000" << "11025" << "16000" << "22050" << "32000" << "44100" << "48000" << "49716";

	ui->cbxDbx_mixer_rate->clear();
	ui->cbxDbx_mixer_rate->addItems(sonido_frecuencias);
	ui->cbxDbx_gus_gusrate->clear();
	ui->cbxDbx_gus_gusrate->addItems(sonido_frecuencias);
	ui->cbxDbx_midi_mt32rate->clear();
	ui->cbxDbx_midi_mt32rate->addItems(sonido_frecuencias);
	ui->cbxDbx_speaker_pcrate->clear();
	ui->cbxDbx_speaker_pcrate->addItems(sonido_frecuencias);
	ui->cbxDbx_speaker_tandyrate->clear();
	ui->cbxDbx_speaker_tandyrate->addItems(sonido_frecuencias);

	ui->chkDbx_sdl_fullfixed->setVisible(false); // En desuso

// Configuración del twMontajes
	ui->twMontajes->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twMontajes->header()->setSectionsMovable(false);
	ui->twMontajes->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	ui->twMontajes->header()->setMovable(false);
	ui->twMontajes->header()->setResizeMode(0, QHeaderView::Stretch);
#endif
	ui->twMontajes->setColumnWidth(1, 40);
	ui->twMontajes->setColumnWidth(2, 80);
	ui->twMontajes->setColumnWidth(3, 80);

	setTheme();

	cargarDatosDosBox(DatosDosBox);
}

void frmWizardDosBox::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/cat/dosbox.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
	ui->btnPrevious->setIcon(QIcon(fGrl->theme() +"img16/mp_rebobinar_atras.png"));
	ui->btnNext->setIcon(QIcon(fGrl->theme() +"img16/mp_rebobinar_adelante.png"));
// General
	ui->btnDbx_ExeJuego->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_ExeJuego_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDbx_ExeSetup->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_ExeSetup_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDescargarInfo->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnInstalarJuego->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app_setup.png"));
// Montaje
	ui->btnMount_Add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btnMount_Edit->setIcon(QIcon(fGrl->theme() +"img16/editar.png"));
	ui->btnMount_Delete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnMount_Clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnMount_Subir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnMount_Bajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnMount_AutoCrear->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app.png"));
	ui->btnMount_Primario->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
}

void frmWizardDosBox::cargarDatosJuego(stDatosJuego datos)
{
	DatosJuego = datos;
	ui->txtDat_titulo->setText(datos.titulo);

	Thumbs      = datos.thumbs;
	CoverFront  = datos.cover_front;
	CoverBack   = datos.cover_back;
	CoverLeft   = datos.cover_left;
	CoverRight  = datos.cover_right;
	CoverTop    = datos.cover_top;
	CoverBottom = datos.cover_bottom;

// Thumbs
	file_thumbs = grlDir.Temp +"imagenes/small/"+ Thumbs;
	if (!QFile::exists(file_thumbs))
	{
		Thumbs = "";
		file_thumbs = "";
	}
// CoverFront
	file_cover_front = grlDir.Temp +"imagenes/"+ CoverFront;
	if (!QFile::exists(file_cover_front))
	{
		CoverFront = "";
		file_cover_front = "";
	}
// CoverBack
	file_cover_back = grlDir.Temp +"imagenes/"+ CoverBack;
	if (!QFile::exists(file_cover_back))
	{
		CoverBack = "";
		file_cover_back = "";
	}
// CoverLeft
	file_cover_left = grlDir.Temp +"imagenes/"+ CoverLeft;
	if (!QFile::exists(file_cover_left))
	{
		CoverLeft = "";
		file_cover_left = "";
	}
// CoverRight
	file_cover_right = grlDir.Temp +"imagenes/"+ CoverRight;
	if (!QFile::exists(file_cover_right))
	{
		CoverRight = "";
		file_cover_right = "";
	}
// CoverTop
	file_cover_top = grlDir.Temp +"imagenes/"+ CoverTop;
	if (!QFile::exists(file_cover_top))
	{
		CoverTop = "";
		file_cover_top = "";
	}
// CoverBottom
	file_cover_bottom = grlDir.Temp +"imagenes/"+ CoverBottom;
	if (!QFile::exists(file_cover_bottom))
	{
		CoverBottom = "";
		file_cover_bottom = "";
	}
}

bool frmWizardDosBox::setDatosJuegos()
{
	bool isOk = false;

	DatosJuego.icono          = "cat/dosbox.png";
	DatosJuego.titulo         = ui->txtDat_titulo->text();
	DatosJuego.titulo_guiones = sql->removeAccents(DatosJuego.titulo);
	DatosJuego.tipo_emu       = "dosbox";
	DatosJuego.fecha          = sql->getTime();
	DatosJuego.idgrl          = sql->insertaDatos(categoria.tabla, DatosJuego);
	isOk = DatosJuego.idgrl.isEmpty() ? false : true;

	DatosJuego.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
	grlDir.DatosGame    = grlDir.DatosDbGrl + DatosJuego.game_dir +"/";

	if (isOk)
	{
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

		if (!Thumbs.isEmpty() && QFile::exists(file_thumbs))
		{
			DatosJuego.thumbs = "thumbs."+ grlCfg.thumb_format.toLower();
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "thumbs", DatosJuego.thumbs);
			fGrl->crearThumbs(file_thumbs, grlDir.DatosGame + DatosJuego.thumbs, grlCfg.thumb_width, grlCfg.thumb_height, grlCfg.thumb_quality, true, grlCfg.thumb_format);
		}

		if (!CoverFront.isEmpty() && QFile::exists(file_cover_front))
		{
			DatosJuego.cover_front = "cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_front", DatosJuego.cover_front);
			fGrl->copiarArchivo(file_cover_front, grlDir.DatosGame +"caja/"+ DatosJuego.cover_front, false, true);
		}

		if (!CoverBack.isEmpty() && QFile::exists(file_cover_back))
		{
			DatosJuego.cover_back = "cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_back", DatosJuego.cover_back);
			fGrl->copiarArchivo(file_cover_back, grlDir.DatosGame +"caja/"+ DatosJuego.cover_back, false, true);
		}

		if (!CoverLeft.isEmpty() && QFile::exists(file_cover_left))
		{
			DatosJuego.cover_left = "cover_left"+ fGrl->getInfoFile(file_cover_left).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_left", DatosJuego.cover_left);
			fGrl->copiarArchivo(file_cover_left, grlDir.DatosGame +"caja/"+ DatosJuego.cover_left, false, true);
		}

		if (!CoverRight.isEmpty() && QFile::exists(file_cover_right))
		{
			DatosJuego.cover_right = "cover_right"+ fGrl->getInfoFile(file_cover_right).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_right", DatosJuego.cover_right);
			fGrl->copiarArchivo(file_cover_right, grlDir.DatosGame +"caja/"+ DatosJuego.cover_right, false, true);
		}

		if (!CoverTop.isEmpty() && QFile::exists(file_cover_top))
		{
			DatosJuego.cover_top = "cover_top"+ fGrl->getInfoFile(file_cover_top).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_top", DatosJuego.cover_top);
			fGrl->copiarArchivo(file_cover_top, grlDir.DatosGame +"caja/"+ DatosJuego.cover_top, false, true);
		}

		if (!CoverBottom.isEmpty() && QFile::exists(file_cover_bottom))
		{
			DatosJuego.cover_bottom = "cover_bottom"+ fGrl->getInfoFile(file_cover_bottom).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_bottom", DatosJuego.cover_bottom);
			fGrl->copiarArchivo(file_cover_bottom, grlDir.DatosGame +"caja/"+ DatosJuego.cover_bottom, false, true);
		}

		if (listImagenesImportadas.size() > 0)
		{
			stDatosImagenes imagen;
			QString dir_out = grlDir.DatosGame +"imagenes/";
			const int numImagenes = listImagenesImportadas.size();
			for (int i = 0; i < numImagenes; ++i)
			{
				imagen = listImagenesImportadas.at(i);

				if (imagen.isImport)
					fGrl->copiarArchivo(imagen.dir_in +"imagenes/"+ imagen.nombre, dir_out + imagen.nombre, false, true);
				else
					fGrl->copiarArchivo(imagen.dir_in + imagen.nombre, dir_out + imagen.nombre, false, true);

				fGrl->crearThumbs(dir_out + imagen.nombre, dir_out +"small/"+ imagen.nombre +".jpg", grlCfg.thumb_img_width, grlCfg.thumb_img_height, grlCfg.thumb_img_quality);
			}
		}
	}

	return isOk;
}


void frmWizardDosBox::previerMontajes()
{
	if (ui->twMontajes->topLevelItemCount() > 0)
	{
		stConfigDOSBox datos_montaje;
		datos_montaje.path_exe        = ui->txtDbx_path_exe->text();
		datos_montaje.parametros_exe  = ui->txtDbx_parametros_exe->text();
		datos_montaje.opt_loadfix_mem = DatosDosBox.opt_loadfix_mem;
		datos_montaje.opt_loadfix     = DatosDosBox.opt_loadfix;
		datos_montaje.opt_cerrar_dbox = DatosDosBox.opt_cerrar_dbox;
		QStringList list_mount = fGrl->creaConfigMontajes(getListMount(), datos_montaje);

		ui->previer_mount->clear();
		const int listSize = list_mount.size();
		for (int i= 0; i < listSize; ++i)
			ui->previer_mount->insertPlainText(list_mount.at(i) +"\n");
	} else
		ui->previer_mount->clear();
}

void frmWizardDosBox::cargarDatosDosBox(stConfigDOSBox cfgDbx)
{
	DatosDosBox = cfgDbx;
// DOSBox version a usar
	if (cfgDbx.dosbox_emu_key.isEmpty() || !dbx_list.contains(cfgDbx.dosbox_emu_key))
	{
		int num_dbx = dbx_list.count(grlCfg.DOSBoxDefault);
		if (num_dbx > 0)
			cfgDbx.dosbox_emu_key = grlCfg.DOSBoxDefault;
		else {
			QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
			cfgDbx.dosbox_emu_key = i_dbx.key();
		}
	}

	isDbxSVN = fGrl->strToBool(dbx_list[cfgDbx.dosbox_emu_key].issvn);
	int index_dbx = ui->cbxDbx_EmuKey->findData(cfgDbx.dosbox_emu_key);
	ui->cbxDbx_EmuKey->setCurrentIndex((index_dbx < 0 ? 0 : index_dbx));

	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution, grlDir.Datos +"dbx_resolution.txt", "img16/", "original", 2, "|"); // Resolución pantalla
	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output        , grlDir.Datos +"dbx_output.txt"    , "img16/", "surface" , 3, "|"); // Modo de Renderizado
	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine    , grlDir.Datos +"dbx_machine.txt"   , "img16/", "svga_s3" , 3, "|", true, true); // Tarjeta de Video
	fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler     , grlDir.Datos +"dbx_scaler.txt"    , "img16/", "normal2x", 3, "|", true, true); // Escalar y Filtros

	if (isDbxSVN)
	{
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution, grlDir.Datos +"dbx_resolution_svn.txt", "img16/", "", 2, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output        , grlDir.Datos +"dbx_output_svn.txt"    , "img16/", "", 3, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine    , grlDir.Datos +"dbx_machine_svn.txt"   , "img16/", "", 3, "|", false, true);
		fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler     , grlDir.Datos +"dbx_scaler_svn.txt"    , "img16/", "", 3, "|", false, true);
	}

	ui->cbxDbx_sdl_fullresolution->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "0x0", "0x0");

// [sdl]
	ui->chkDbx_sdl_fullscreen->setChecked(fGrl->strToBool(cfgDbx.sdl_fullscreen));
	ui->chkDbx_sdl_fullfixed->setChecked(fGrl->strToBool(cfgDbx.sdl_fullfixed));
	ui->cbxDbx_sdl_fullresolution->setCurrentIndex(ui->cbxDbx_sdl_fullresolution->findData(cfgDbx.sdl_fullresolution));
	ui->cbxDbx_sdl_output->setCurrentIndex(ui->cbxDbx_sdl_output->findData(cfgDbx.sdl_output));
	ui->chkDbx_sdl_autolock->setChecked(fGrl->strToBool(cfgDbx.sdl_autolock));
// [dosbox]
	ui->cbxDbx_dosbox_machine->setCurrentIndex(ui->cbxDbx_dosbox_machine->findData(cfgDbx.dosbox_machine));
	ui->cbxDbx_dosbox_memsize->setEditText(cfgDbx.dosbox_memsize);
// [render]
	ui->chkDbx_render_aspect->setChecked(fGrl->strToBool(cfgDbx.render_aspect));
	ui->cbxDbx_render_scaler->setCurrentIndex(ui->cbxDbx_render_scaler->findData(cfgDbx.render_scaler));
// [cpu]
	ui->cbxDbx_cpu_core->setCurrentIndex(ui->cbxDbx_cpu_core->findData(cfgDbx.cpu_core));
	ui->cbxDbx_cpu_cycles->setCurrentIndex(ui->cbxDbx_cpu_cycles->findText(cfgDbx.cpu_cycles));
// [mixer]
	ui->chkDbx_mixer_nosound->setChecked(!fGrl->strToBool(cfgDbx.mixer_nosound));
	ui->cbxDbx_mixer_rate->setCurrentIndex(ui->cbxDbx_mixer_rate->findText(cfgDbx.mixer_rate));
// [midi]
	ui->cbxDbx_midi_mpu401->setCurrentIndex(ui->cbxDbx_midi_mpu401->findText(cfgDbx.midi_mpu401));
	ui->chkDbx_midi_intelligent->setChecked(fGrl->strToBool(cfgDbx.midi_intelligent));
	ui->cbxDbx_midi_device->setCurrentIndex(ui->cbxDbx_midi_device->findText(cfgDbx.midi_device));
	ui->cbxDbx_midi_mt32rate->setCurrentIndex(ui->cbxDbx_midi_mt32rate->findText(cfgDbx.midi_mt32rate));
// [sblaster]
	ui->cbxDbx_sblaster_sbtype->setCurrentIndex(ui->cbxDbx_sblaster_sbtype->findData(cfgDbx.sblaster_sbtype));
	ui->cbxDbx_sblaster_oplrate->setCurrentIndex(ui->cbxDbx_sblaster_oplrate->findText(cfgDbx.sblaster_oplrate));
// [gus]
	ui->chkDbx_gus_gus->setChecked(fGrl->strToBool(cfgDbx.gus_gus));
	ui->cbxDbx_gus_gusrate->setCurrentIndex(ui->cbxDbx_gus_gusrate->findText(cfgDbx.gus_gusrate));
// [speaker]
	ui->chkDbx_speaker_pcspeaker->setChecked(fGrl->strToBool(cfgDbx.speaker_pcspeaker));
	ui->cbxDbx_speaker_pcrate->setCurrentIndex(ui->cbxDbx_speaker_pcrate->findText(cfgDbx.speaker_pcrate));
	ui->cbxDbx_speaker_tandyrate->setCurrentIndex(ui->cbxDbx_speaker_tandyrate->findText(cfgDbx.speaker_tandyrate));
	ui->chkDbx_speaker_disney->setChecked(fGrl->strToBool(cfgDbx.speaker_disney));
// [dos]
	ui->chkDbx_dos_xms->setChecked(fGrl->strToBool(cfgDbx.dos_xms));
	ui->chkDbx_dos_ems->setChecked(fGrl->strToBool(cfgDbx.dos_ems));
	ui->cbxDbx_dos_umb->setCurrentIndex(ui->cbxDbx_dos_umb->findText(cfgDbx.dos_umb));
// Opciones
	ui->chkDbx_loadfix->setChecked(fGrl->strToBool(cfgDbx.opt_loadfix));
	ui->chkDbx_consola_dbox->setChecked(fGrl->strToBool(cfgDbx.opt_consola_dbox));
	ui->chkDbx_cerrar_dbox->setChecked(fGrl->strToBool(cfgDbx.opt_cerrar_dbox));
// path_exe
	if (!cfgDbx.path_exe.isEmpty())
		ui->txtDbx_path_exe->setText(cfgDbx.path_exe);
// path_setup
	if (!cfgDbx.path_setup.isEmpty())
		ui->txtDbx_path_setup->setText(cfgDbx.path_setup);
// parametros_exe
	ui->txtDbx_parametros_exe->setText(cfgDbx.parametros_exe);
// parametros_setup
	ui->txtDbx_parametros_setup->setText(cfgDbx.parametros_setup);
}

void frmWizardDosBox::cargarDatosDBxMontaje(QList<stConfigDOSBoxMount> listMount)
{
	QList<QTreeWidgetItem *> items;
	const int listMountSize = listMount.size();
	for (int i = 0; i < listMountSize; ++i)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setTextAlignment(1, Qt::AlignCenter);
		item->setTextAlignment(2, Qt::AlignCenter);
		item->setIcon(0, QIcon(fGrl->getIconMount(listMount.at(i).tipo_as, listMount.at(i).select_mount == "v" ? "s_" : "")));
		item->setText( 0, listMount.at(i).path        ); // Directorio o iso
		item->setText( 1, listMount.at(i).letter      ); // Letra de montaje
		item->setText( 2, listMount.at(i).tipo_as     ); // Tipo de montaje
		item->setText( 3, listMount.at(i).label       ); // Etiqueta
		item->setText( 4, listMount.at(i).indx_cd     ); // Index de la unidad de cd-rom
		item->setText( 5, listMount.at(i).opt_mount   ); // Opciones del cd-rom
		item->setText( 6, listMount.at(i).io_ctrl     ); // Cd/dvd
		item->setText( 7, listMount.at(i).select_mount); // Primer montaje
		item->setText( 8, listMount.at(i).id          ); // Id
		item->setText( 9, listMount.at(i).id_lista    ); // Id_lista
		item->setText(10, listMount.at(i).opt_size    );
		item->setText(11, listMount.at(i).opt_freesize);
		item->setText(12, listMount.at(i).freesize    );
		items << item;
	}
	ui->twMontajes->addTopLevelItems(items);
	previerMontajes();
}

QList<stConfigDOSBoxMount> frmWizardDosBox::getListMount()
{
	return fGrl->getListCfgMount(ui->twMontajes);
}

void frmWizardDosBox::setDatosDosBox()
{
	QString str;
// [sdl]
	DatosDosBox.sdl_fullscreen     = fGrl->boolToStr(ui->chkDbx_sdl_fullscreen->isChecked());
	DatosDosBox.sdl_fullfixed      = fGrl->boolToStr(ui->chkDbx_sdl_fullfixed->isChecked());
	str = ui->cbxDbx_sdl_fullresolution->itemData(ui->cbxDbx_sdl_fullresolution->currentIndex()).toString();
	DatosDosBox.sdl_fullresolution = str.isEmpty() ? "original" : str;
	str = ui->cbxDbx_sdl_output->itemData(ui->cbxDbx_sdl_output->currentIndex()).toString();
	DatosDosBox.sdl_output         = str.isEmpty() ? "surface" : str;
	DatosDosBox.sdl_autolock       = fGrl->boolToStr(ui->chkDbx_sdl_autolock->isChecked());
// [dosbox]
	str = ui->cbxDbx_dosbox_machine->itemData(ui->cbxDbx_dosbox_machine->currentIndex()).toString();
	DatosDosBox.dosbox_machine     = str.isEmpty() ? "svga_s3" : str;
	DatosDosBox.dosbox_memsize     = ui->cbxDbx_dosbox_memsize->currentText().isEmpty() ? "16" : ui->cbxDbx_dosbox_memsize->currentText();
	str = ui->cbxDbx_EmuKey->itemData(ui->cbxDbx_EmuKey->currentIndex()).toString();
	DatosDosBox.dosbox_emu_key     = str.isEmpty() ? "dosbox" : str;
// [render]
	DatosDosBox.render_aspect      = fGrl->boolToStr(ui->chkDbx_render_aspect->isChecked());
	str = ui->cbxDbx_render_scaler->itemData(ui->cbxDbx_render_scaler->currentIndex()).toString();
	DatosDosBox.render_scaler      = str.isEmpty() ? "normal2x" : str;
// [cpu]
	str = ui->cbxDbx_cpu_core->itemData(ui->cbxDbx_cpu_core->currentIndex()).toString();
	DatosDosBox.cpu_core           = str.isEmpty() ? "auto" : str;
	DatosDosBox.cpu_cycles         = ui->cbxDbx_cpu_cycles->currentText().isEmpty() ? "auto" : ui->cbxDbx_cpu_cycles->currentText();
// [mixer]
	DatosDosBox.mixer_nosound      = fGrl->boolToStr(!ui->chkDbx_mixer_nosound->isChecked());
	DatosDosBox.mixer_rate         = ui->cbxDbx_mixer_rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_mixer_rate->currentText();
// [midi]
	DatosDosBox.midi_mpu401        = ui->cbxDbx_midi_mpu401->currentText().isEmpty() ? "intelligent" : ui->cbxDbx_midi_mpu401->currentText();
	DatosDosBox.midi_intelligent   = fGrl->boolToStr(ui->chkDbx_midi_intelligent->isChecked());
	DatosDosBox.midi_device        = ui->cbxDbx_midi_device->currentText().isEmpty() ? "default" : ui->cbxDbx_midi_device->currentText();
	DatosDosBox.midi_mt32rate      = ui->cbxDbx_midi_mt32rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_midi_mt32rate->currentText();
// [sblaster]
	str = ui->cbxDbx_sblaster_sbtype->itemData(ui->cbxDbx_sblaster_sbtype->currentIndex()).toString();
	DatosDosBox.sblaster_sbtype    = str.isEmpty() ? "sb16" : str;
// [gus]
	DatosDosBox.gus_gus            = fGrl->boolToStr(ui->chkDbx_gus_gus->isChecked());
	DatosDosBox.gus_gusrate        = ui->cbxDbx_gus_gusrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_gus_gusrate->currentText();
// [speaker]
	DatosDosBox.speaker_pcspeaker  = fGrl->boolToStr(ui->chkDbx_speaker_pcspeaker->isChecked());
	DatosDosBox.speaker_pcrate     = ui->cbxDbx_speaker_pcrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_pcrate->currentText();
	DatosDosBox.speaker_tandyrate  = ui->cbxDbx_speaker_tandyrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_tandyrate->currentText();
	DatosDosBox.speaker_disney     = fGrl->boolToStr(ui->chkDbx_speaker_disney->isChecked());
// [dos]
	DatosDosBox.dos_xms            = fGrl->boolToStr(ui->chkDbx_dos_xms->isChecked());
	DatosDosBox.dos_ems            = fGrl->boolToStr(ui->chkDbx_dos_ems->isChecked());
	DatosDosBox.dos_umb            = ui->cbxDbx_dos_umb->currentText().isEmpty() ? "true" : ui->cbxDbx_dos_umb->currentText();
// Opciones
	DatosDosBox.opt_loadfix        = fGrl->boolToStr(ui->chkDbx_loadfix->isChecked());
	DatosDosBox.opt_consola_dbox   = fGrl->boolToStr(ui->chkDbx_consola_dbox->isChecked());
	DatosDosBox.opt_cerrar_dbox    = fGrl->boolToStr(ui->chkDbx_cerrar_dbox->isChecked());
// Path
	DatosDosBox.path_exe           = ui->txtDbx_path_exe->text().isEmpty() ? "" : ui->txtDbx_path_exe->text();
	DatosDosBox.path_setup         = ui->txtDbx_path_setup->text().isEmpty() ? "" : ui->txtDbx_path_setup->text();
	DatosDosBox.parametros_exe     = ui->txtDbx_parametros_exe->text().isEmpty() ? "" : ui->txtDbx_parametros_exe->text();
	DatosDosBox.parametros_setup   = ui->txtDbx_parametros_setup->text().isEmpty() ? "" : ui->txtDbx_parametros_setup->text();
}

void frmWizardDosBox::on_btnOk_clicked()
{
	if (ui->txtDat_titulo->text().isEmpty())
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el título."));
	else {
		if (setDatosJuegos())
		{
			setDatosDosBox();
			QList<stConfigDOSBoxMount> list_mount = getListMount();

			DatosDosBox.idgrl = DatosJuego.idgrl;
			DatosDosBox.idcat = categoria.id;
			DatosDosBox.id    = sql->insertaDbx(DatosDosBox);
			sql->insertaMontajesDbx(list_mount, DatosDosBox.id);

		//	if (grlCfg.DOSBoxSaveConfFile)
			fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, list_mount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox.conf");
			fGrl->crearArchivoConfigDbx(DatosJuego, DatosDosBox, list_mount, grlDir.Home, categoria.tabla, grlDir.DatosGame +"dosbox-setup.conf", true);

		// Guardamos los cambios en la base de datos.
			sql->comit();
			QDialog::accept();
		}
	}
}

void frmWizardDosBox::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmWizardDosBox::on_btnNext_clicked()
{
	bool siguiente = false;

	if (!ui->txtDat_titulo->text().isEmpty())
	{
		if (ui->txtDbx_path_exe->text().isEmpty())
		{
			QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego"));
			ui->txtDbx_path_exe->setFocus();
		} else
			siguiente = true;
	} else {
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner un título al juego"));
		ui->txtDat_titulo->setFocus();
	}

	if (siguiente)
	{
		ui->btnPrevious->setEnabled(true);
		ui->wizardDbx->setCurrentIndex(ui->wizardDbx->currentIndex()+1);
	}
	if (ui->wizardDbx->currentIndex() == 1) ui->chkDbx_sdl_fullscreen->setFocus();
	if (ui->wizardDbx->currentIndex() == 2) ui->cbxDbx_dosbox_memsize->setFocus();
	if (ui->wizardDbx->currentIndex() == 3) ui->chkDbx_mixer_nosound->setFocus();
	if (ui->wizardDbx->currentIndex() == 4)
	{
//		if (isImportDbx == false)
//			emit on_btnMount_AutoCrear();
		ui->twMontajes->setFocus();
	}

	if (ui->wizardDbx->currentIndex() >= 5)
	{
		ui->btnOk->setEnabled(true);
		ui->btnOk->setFocus();
		ui->btnNext->setEnabled(false);
	}
}

void frmWizardDosBox::on_btnPrevious_clicked()
{
	ui->wizardDbx->setCurrentIndex(ui->wizardDbx->currentIndex()-1);
	ui->btnNext->setEnabled(true);
	if (ui->wizardDbx->currentIndex() < 1)
		ui->btnPrevious->setEnabled(false);
}

void frmWizardDosBox::on_btnDbx_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Dbx_path_exe, "DosGames", tr("Ejecutables") +" (*.exe; *.bat; *.com);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbx_path_exe->setText(fGrl->setDirRelative(archivo, "DosGames"));
			grlCfg.Dbx_path_exe = fGrl->setDirRelative(f_info.Path, "DosGames");

			if (ui->twMontajes->topLevelItemCount() < 1)
				emit on_btnMount_AutoCrear_clicked();
		}
	}
}

void frmWizardDosBox::on_btnDbx_ExeJuego_clear_clicked()
{
	ui->txtDbx_path_exe->clear();
}

void frmWizardDosBox::on_btnDbx_ExeSetup_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Dbx_path_setup, "DosGames", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbx_path_setup->setText(fGrl->setDirRelative(archivo, "DosGames"));
			grlCfg.Dbx_path_setup = fGrl->setDirRelative(f_info.Path, "DosGames");
		}
	}
}

void frmWizardDosBox::on_btnDbx_ExeSetup_clear_clicked()
{
	ui->txtDbx_path_setup->clear();
}

void frmWizardDosBox::on_btnDescargarInfo_clicked()
{
	stConfigScummVM  DatosScummVM  = sql->getDefectScummVM();
	stConfigVDMSound DatosVDMSound = sql->getDefectVDMSound();

	DatosJuego.titulo = ui->txtDat_titulo->text();

	frmImportarJuego *importarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, false, this);
	importarJuego->setWindowFlags(Qt::Window);

	if (importarJuego->exec() == QDialog::Accepted)
	{
		grlCfg = importarJuego->getGrlCfg();

		if (importarJuego->getDatosImport().size() > 0)
		{
			stImport import = importarJuego->getDatosImport().at(0);
			DatosJuego = import.datos;
			listImagenesImportadas = import.imagenes;

			cargarDatosJuego(DatosJuego);

			if (!import.solo_datos && DatosJuego.tipo_emu == "dosbox")
			{
				cargarDatosDosBox(import.dosbox);
				cargarDatosDBxMontaje(import.dbxMount);
			}
		}
	} else {
		grlCfg = importarJuego->getGrlCfg();
	}

	delete importarJuego;
}

void frmWizardDosBox::on_btnInstalarJuego_clicked()
{
	frmInstalarJuego *instalarJuego = new frmInstalarJuego(sql, grlCfg, this);
	if (instalarJuego->exec() == QDialog::Accepted)
	{
		if (fGrl->questionMsg(tr("¿Usar montajes?"), tr("¿Deseas añadir los montajes usados?")))
		{
			QList<stConfigDOSBoxMount> list_mount = instalarJuego->getDatosMontajes();
			const int count_mounts = list_mount.size();
			if (count_mounts > 0)
			{
				for (int m = 0; m < count_mounts; ++m)
				{
					list_mount[m].id_lista = fGrl->intToStr(ui->twMontajes->topLevelItemCount());

					QTreeWidgetItem *item = new QTreeWidgetItem;
					item->setTextAlignment(1, Qt::AlignCenter);
					item->setTextAlignment(2, Qt::AlignCenter);
					item->setIcon( 0, QIcon(fGrl->getIconMount(list_mount.at(m).tipo_as)));
					item->setText( 0, list_mount.at(m).path        ); // directorio o iso
					item->setText( 1, list_mount.at(m).letter      ); // letra de montaje
					item->setText( 2, list_mount.at(m).tipo_as     ); // tipo de montaje
					item->setText( 3, list_mount.at(m).label       ); // etiqueta
					item->setText( 4, list_mount.at(m).indx_cd     ); // index de la unidad de cd-rom
					item->setText( 5, list_mount.at(m).opt_mount   ); // opciones del cd-rom
					item->setText( 6, list_mount.at(m).io_ctrl     ); // cd/dvd
					item->setText( 7, list_mount.at(m).select_mount); // primer montaje
					item->setText( 8, ""                           );
					item->setText( 9, list_mount.at(m).id_lista    );
					item->setText(10, list_mount.at(m).opt_size    );
					item->setText(11, list_mount.at(m).opt_freesize);
					item->setText(12, list_mount.at(m).freesize    );
					ui->twMontajes->addTopLevelItem(item);
				}
			}
		}
	}
	delete instalarJuego;
}

void frmWizardDosBox::on_btnMount_Add_clicked()
{
	QStringList list_letter;
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if (count_mount > 0)
	{
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
			list_letter << ui->twMontajes->topLevelItem(num_mount)->text(1);
	}

	frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes(grlCfg, false, list_letter, this);
	if (AddEditMontajes->exec() == QDialog::Accepted)
	{
		grlCfg.Montaje_path = AddEditMontajes->getGrlCfg().Montaje_path;
		stConfigDOSBoxMount DatosMontaje = AddEditMontajes->getDatosMontaje();
		DatosMontaje.id_lista = fGrl->intToStr(ui->twMontajes->topLevelItemCount());
		DatosMontaje.select_mount = "x";

		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setTextAlignment(1, Qt::AlignCenter);
		item->setTextAlignment(2, Qt::AlignCenter);
		item->setIcon( 0, QIcon(fGrl->getIconMount(DatosMontaje.tipo_as)));
		item->setText( 0, DatosMontaje.path        ); // directorio o iso
		item->setText( 1, DatosMontaje.letter      ); // letra de montaje
		item->setText( 2, DatosMontaje.tipo_as     ); // tipo de montaje
		item->setText( 3, DatosMontaje.label       ); // etiqueta
		item->setText( 4, DatosMontaje.indx_cd     ); // index de la unidad de cd-rom
		item->setText( 5, DatosMontaje.opt_mount   ); // opciones del cd-rom
		item->setText( 6, DatosMontaje.io_ctrl     ); // cd/dvd
		item->setText( 7, DatosMontaje.select_mount); // primer montaje
		item->setText( 8, ""                       );
		item->setText( 9, DatosMontaje.id_lista    );
		item->setText(10, DatosMontaje.opt_size    );
		item->setText(11, DatosMontaje.opt_freesize);
		item->setText(12, DatosMontaje.freesize    );
		ui->twMontajes->addTopLevelItem(item);
	}
	previerMontajes();
}

void frmWizardDosBox::on_btnMount_Edit_clicked()
{
	int pos = ui->twMontajes->indexOfTopLevelItem(ui->twMontajes->currentItem());
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if (count_mount > 0 && pos != -1)
	{
		stConfigDOSBoxMount DatosMontaje;
		DatosMontaje.id_lista     = ui->twMontajes->currentItem()->text(9);
		DatosMontaje.path         = ui->twMontajes->currentItem()->text(0);	// directorio o iso
		DatosMontaje.label        = ui->twMontajes->currentItem()->text(3);	// etiqueta
		DatosMontaje.tipo_as      = ui->twMontajes->currentItem()->text(2);	// tipo de montaje
		DatosMontaje.letter       = ui->twMontajes->currentItem()->text(1);	// letra de montaje
		DatosMontaje.indx_cd      = ui->twMontajes->currentItem()->text(4);	// index de la unidad de cd-rom
		DatosMontaje.opt_mount    = ui->twMontajes->currentItem()->text(5);	// opciones del cd-rom
		DatosMontaje.io_ctrl      = ui->twMontajes->currentItem()->text(6);	// cd/dvd
		DatosMontaje.select_mount = ui->twMontajes->currentItem()->text(7);	// primer montaje
		DatosMontaje.opt_size     = ui->twMontajes->currentItem()->text(10);
		DatosMontaje.opt_freesize = ui->twMontajes->currentItem()->text(11);
		DatosMontaje.freesize     = ui->twMontajes->currentItem()->text(12);

		QStringList list_letter;
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QString letter = ui->twMontajes->topLevelItem(num_mount)->text(1);
			if (DatosMontaje.letter != letter)
				list_letter << letter;
		}

		frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes(grlCfg, true, list_letter, this);
		AddEditMontajes->setDatosMontaje(DatosMontaje);
		if (AddEditMontajes->exec() == QDialog::Accepted)
		{
			grlCfg.Montaje_path = AddEditMontajes->getGrlCfg().Montaje_path;
			DatosMontaje = AddEditMontajes->getDatosMontaje();
			QString select_mount = ui->twMontajes->currentItem()->text(7) == "v" ? "s_" : "";

			ui->twMontajes->currentItem()->setIcon( 0, QIcon(fGrl->getIconMount(DatosMontaje.tipo_as, select_mount)));
			ui->twMontajes->currentItem()->setText( 0, DatosMontaje.path        ); // directorio o iso
			ui->twMontajes->currentItem()->setText( 1, DatosMontaje.letter      ); // letra de montaje
			ui->twMontajes->currentItem()->setText( 2, DatosMontaje.tipo_as     ); // tipo de montaje
			ui->twMontajes->currentItem()->setText( 3, DatosMontaje.label       ); // etiqueta
			ui->twMontajes->currentItem()->setText( 4, DatosMontaje.indx_cd     ); // index de la unidad de cd-rom
			ui->twMontajes->currentItem()->setText( 5, DatosMontaje.opt_mount   ); // opciones del cd-rom
			ui->twMontajes->currentItem()->setText( 6, DatosMontaje.io_ctrl     ); // cd/dvd
			ui->twMontajes->currentItem()->setText( 7, DatosMontaje.select_mount); // primer montaje
			ui->twMontajes->currentItem()->setText( 9, DatosMontaje.id_lista    );
			ui->twMontajes->currentItem()->setText(10, DatosMontaje.opt_size    );
			ui->twMontajes->currentItem()->setText(11, DatosMontaje.opt_freesize);
			ui->twMontajes->currentItem()->setText(12, DatosMontaje.freesize    );
		}
		previerMontajes();
	}
}

void frmWizardDosBox::on_btnMount_Delete_clicked()
{
	if (ui->twMontajes->topLevelItemCount() > 0)
	{
		if (fGrl->questionMsg(tr("¿Eliminar montaje...?"), tr("¿Deseas eliminar este montaje?")))
		{
			fGrl->deleteItemTree(ui->twMontajes->currentItem());
			previerMontajes();
		}
	}
}

void frmWizardDosBox::on_btnMount_Clear_clicked()
{
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if (count_mount > 0)
	{
		ui->twMontajes->clear();
		previerMontajes();
	}
}

void frmWizardDosBox::on_btnMount_Subir_clicked()
{
	fGrl->moveUpItemTw(ui->twMontajes);
	previerMontajes();
}

void frmWizardDosBox::on_btnMount_Bajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMontajes);
	previerMontajes();
}

void frmWizardDosBox::on_btnMount_AutoCrear_clicked()
{
	QString id_lista = fGrl->intToStr(ui->twMontajes->topLevelItemCount());
	QString path     = ".\\DosGames\\";

	if (!ui->txtDbx_path_exe->text().isEmpty())
		path = fGrl->getInfoFile(fGrl->getDirRelative(ui->txtDbx_path_exe->text(), "DosGames")).Path;

	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setTextAlignment(1, Qt::AlignCenter);
	item->setTextAlignment(2, Qt::AlignCenter);
	item->setIcon( 0, QIcon(fGrl->theme() +"img16/drive_hd.png"));
	item->setText( 0, path    ); // directorio o iso
	item->setText( 1, "C"     ); // letra de montaje
	item->setText( 2, "drive" ); // tipo de montaje
	item->setText( 3, ""      ); // etiqueta
	item->setText( 4, ""      ); // index de la unidad de cd-rom
	item->setText( 5, ""      ); // opciones del cd-rom
	item->setText( 6, ""      ); // cd/dvd IOCtrl
	item->setText( 7, "x"     ); // primer montaje
	item->setText( 8, ""      );
	item->setText( 9, id_lista);
	item->setText(10, ""      );
	item->setText(11, "false" );
	item->setText(12, "250"   );
	ui->twMontajes->addTopLevelItem(item);

	previerMontajes();
}

void frmWizardDosBox::on_btnMount_Primario_clicked()
{
	int pos = ui->twMontajes->indexOfTopLevelItem(ui->twMontajes->currentItem());
	if (ui->twMontajes->topLevelItemCount() > 0 && pos != -1)
	{
		int indx_mount = 0;
		QString tipoDrive, isPrimario;

		indx_mount = ui->twMontajes->indexOfTopLevelItem(ui->twMontajes->currentItem());
		tipoDrive  = ui->twMontajes->topLevelItem(indx_mount)->text(2);
		isPrimario = ui->twMontajes->topLevelItem(indx_mount)->text(7);

		const int count_mount = ui->twMontajes->topLevelItemCount();
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QString tipoDrive_mount = ui->twMontajes->topLevelItem(num_mount)->text(2);

			ui->twMontajes->topLevelItem(num_mount)->setIcon(0, QIcon(fGrl->getIconMount(tipoDrive_mount)));
			ui->twMontajes->topLevelItem(num_mount)->setText(7, "x");
		}

		ui->twMontajes->topLevelItem(indx_mount)->setIcon(0, QIcon(fGrl->getIconMount(tipoDrive, "s_")));
		ui->twMontajes->topLevelItem(indx_mount)->setText(7, "v");

		previerMontajes();
	}
}

void frmWizardDosBox::on_cbxDbx_EmuKey_activated(int index)
{
	if (index > -1)
	{
		isDbxSVN = fGrl->strToBool(dbx_list[ui->cbxDbx_EmuKey->itemData(index).toString()].issvn);
		setDatosDosBox();
		cargarDatosDosBox(DatosDosBox);
	}
}

void frmWizardDosBox::on_cbxDbx_Profiles_activated(int index)
{
	if (index > -1)
	{
		QString profileGame = ui->cbxDbx_Profiles->itemData(index).toString();
		if (profileGame != "_defecto_")
		{
			QHash<QString, QString> tmp_cfgDbx = fGrl->importarProfileDFend(grlDir.Home, grlDir.Templates + profileGame);
			tmp_cfgDbx["Dbx_id"]    = DatosDosBox.id;
			tmp_cfgDbx["Dbx_idgrl"] = DatosDosBox.idgrl;
			tmp_cfgDbx["Dbx_idcat"] = DatosDosBox.idcat;
			cargarDatosDosBox(sql->getDefectDOSBox(tmp_cfgDbx));
		} else {
			DatosDosBox = sql->getDefectDOSBox();
			cargarDatosDosBox(DatosDosBox);
		}
	}
}
