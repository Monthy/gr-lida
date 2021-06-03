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

#include <QMessageBox>

#include "grlida_addedit_dosbox.h"
#include "grlida_addedit_montajes.h"
#include "grlida_instalar_juego.h"
#include "ui_addedit_dosbox.h"

frmAddEditDosBox::frmAddEditDosBox(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::frmAddEditDosBox)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	IdGame    = id_game;
	Editando  = m_editando;
	isDbxSVN  = false;

	grlDir.Home      = fGrl->homePath();
	grlDir.Datos     = grlDir.Home +"datos/";
	grlDir.Templates = grlDir.Home +"templates/";
}

frmAddEditDosBox::~frmAddEditDosBox()
{
	delete fGrl;
	delete ui;
}

void frmAddEditDosBox::cargarConfig()
{
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

		int index_dbx = ui->cbxDbx_EmuKey->findData(grlCfg.DOSBoxDefault);
		index_dbx = index_dbx < 0 ? 0 : index_dbx;

		ui->cbxDbx_EmuKey->setCurrentIndex(index_dbx);
		isDbxSVN = fGrl->strToBool(dbx_list[ui->cbxDbx_EmuKey->itemData(index_dbx).toString()].issvn);
	} else {
		ui->cbxDbx_EmuKey->addItem(QIcon(fGrl->theme() +"img16/dosbox.png"), "DOSBox", "dosbox");
	}

	fGrl->cargarProfilesComboBox(ui->cbxDbx_Profiles, grlDir.Templates, "img16/", "_defecto_");

	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_hwscale         , grlDir.Datos +"dbx_hwscale.txt"       , "img16/", "1.00"       , 2, "|"); // Escalar por hardware
	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_sensitivity     , grlDir.Datos +"dbx_sensitivity.txt"   , "img16/", "100"        , 2, "|"); // Sensibilidad del ratn
	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_memsize      , grlDir.Datos +"dbx_memsize.txt"       , "img16/", "16"         , 2, "|"); // Cantidad de memoria para DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_core            , grlDir.Datos +"dbx_cpu_core.txt"      , "img16/", "auto"       , 3, "|"); // Núcleo de la CPU DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_render_frameskip    , ":/datos/dbx_frameskip.txt"           , "img16/", "0"          , 2, "|"); // Frameskip DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cycles          , grlDir.Datos +"dbx_cycles.txt"        , "img16/", "auto"       , 2, "|"); // Ciclos DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cycles_realmode , grlDir.Datos +"dbx_cycles.txt"        , "img16/", ""           , 2, "|"); // Ciclos DOSBox realmode
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cycles_limitmode, grlDir.Datos +"dbx_cycles.txt"        , "img16/", ""           , 2, "|"); // Ciclos DOSBox limitmode
	fGrl->cargarDatosComboBox(ui->cbxDbx_joystick_type       , grlDir.Datos +"dbx_joystick.txt"      , "img16/", "auto"       , 3, "|", true, true); // Emulador de joystick
	fGrl->cargarDatosComboBox(ui->cbxDbx_dos_keyboardlayout  , grlDir.Datos +"dbx_keyboardlayout.txt", "img16/", "default"    , 4, "|"); // keyboardlayout
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_sbtype     , grlDir.Datos +"dbx_sbtype.txt"        , "img16/", "sb16"       , 3, "|", true, true); // Tipo Sound Blaste
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_oplmode    , grlDir.Datos +"dbx_sb_oplmode.txt"    , "img16/", "auto"       , 3, "|", true, true); // Sound Blaste Opl mode
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_oplemu     , grlDir.Datos +"dbx_sb_oplemu.txt"     , "img16/", "default"    , 2, "|"); // Provider for the OPL emulation
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_oplrate    , ":/datos/dbx_sb_oplrate.txt"          , "img16/", "44100"      , 2, "|"); // Sample rate of OPL
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_mpu401         , grlDir.Datos +"dbx_mpu401.txt"        , "img16/", "intelligent", 2, "|"); // MPU-401
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_device         , grlDir.Datos +"dbx_midi_device.txt"   , "img16/", "default"    , 2, "|"); // MIDI Device

	ui->cbxDbx_cpu_cycles_realmode->setItemText(0,"");
	ui->cbxDbx_cpu_cycles_realmode->removeItem(1);
	ui->cbxDbx_cpu_cycles_limitmode->setItemText(0,"");
	ui->cbxDbx_cpu_cycles_limitmode->removeItem(1);

	ui->cbxDbx_cpu_cycles_protmode->clear();
	ui->cbxDbx_cpu_cycles_protmode->addItem("");
	ui->cbxDbx_cpu_cycles_protmode->addItem("10%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("20%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("30%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("40%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("50%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("60%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("70%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("80%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("90%");
	ui->cbxDbx_cpu_cycles_protmode->addItem("100%");

	ui->cbxDbx_dos_umb->clear();
	ui->cbxDbx_dos_umb->addItem("true");
	ui->cbxDbx_dos_umb->addItem("false");
	ui->cbxDbx_dos_umb->addItem("max");

	ui->cbxDbx_dos_version->clear();
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), tr("por defecto"), "");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.22", "6 22");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.2" , "6 2");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.0" , "6 0");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "5.0" , "5 0");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "4.0" , "4 0");
	ui->cbxDbx_dos_version->addItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "3.3" , "3 3");

	QStringList sonido_frecuencias, sonido_Address,	sonido_IRQ, sonido_DMA, sonido_HDMA;
	sonido_frecuencias << "8000" << "11025" << "16000" << "22050" << "32000" << "44100" << "48000" << "49716";
	sonido_Address << "210" << "220" << "240" << "260" << "280" << "2a0" << "2c0" << "2e0" << "300";
	sonido_IRQ << "3" << "5" << "7" << "9" << "10" << "11" << "12";
	sonido_DMA << "0" << "1" << "3" << "5" << "6" << "7";
	sonido_HDMA << "0" << "1" << "3" << "5" << "6" << "7";

	ui->cbxDbx_mixer_rate->clear();
	ui->cbxDbx_mixer_rate->addItems(sonido_frecuencias);
	ui->cbxDbx_sblaster_sbbase->clear();
	ui->cbxDbx_sblaster_sbbase->addItems(sonido_Address);
	ui->cbxDbx_sblaster_irq->clear();
	ui->cbxDbx_sblaster_irq->addItems(sonido_IRQ);
	ui->cbxDbx_sblaster_dma->clear();
	ui->cbxDbx_sblaster_dma->addItems(sonido_DMA);
	ui->cbxDbx_sblaster_hdma->clear();
	ui->cbxDbx_sblaster_hdma->addItems(sonido_HDMA);

	ui->cbxDbx_gus_gusrate->clear();
	ui->cbxDbx_gus_gusrate->addItems(sonido_frecuencias);
	ui->cbxDbx_gus_gusbase->clear();
	ui->cbxDbx_gus_gusbase->addItems(sonido_Address);
	ui->cbxDbx_gus_irq1->clear();
	ui->cbxDbx_gus_irq1->addItems(sonido_IRQ);
	ui->cbxDbx_gus_irq2->clear();
	ui->cbxDbx_gus_irq2->addItems(sonido_IRQ);
	ui->cbxDbx_gus_dma1->clear();
	ui->cbxDbx_gus_dma1->addItems(sonido_DMA);
	ui->cbxDbx_gus_dma2->clear();
	ui->cbxDbx_gus_dma2->addItems(sonido_DMA);
	ui->txtDbx_gus_ultradir->setText("C:\\ULTRASND");

	ui->cbxDbx_midi_mt32rate->clear();
	ui->cbxDbx_midi_mt32rate->addItems(sonido_frecuencias);
	ui->cbxDbx_speaker_pcrate->clear();
	ui->cbxDbx_speaker_pcrate->addItems(sonido_frecuencias);
	ui->cbxDbx_speaker_tandyrate->clear();
	ui->cbxDbx_speaker_tandyrate->addItems(sonido_frecuencias);

	ui->cbxDbx_speaker_tandy->clear();
	ui->cbxDbx_speaker_tandy->addItem("auto");
	ui->cbxDbx_speaker_tandy->addItem("on");
	ui->cbxDbx_speaker_tandy->addItem("off");

	ui->cbxDbx_dserial_comport->clear();
	ui->cbxDbx_dserial_comport->addItem("1");
	ui->cbxDbx_dserial_comport->addItem("2");

	ui->cbxDbx_dserial_defaultbps->clear();
	ui->cbxDbx_dserial_defaultbps->addItem("1200");
	ui->cbxDbx_dserial_defaultbps->addItem("1800");
	ui->cbxDbx_dserial_defaultbps->addItem("2400");
	ui->cbxDbx_dserial_defaultbps->addItem("4800");
	ui->cbxDbx_dserial_defaultbps->addItem("7200");
	ui->cbxDbx_dserial_defaultbps->addItem("9600");
	ui->cbxDbx_dserial_defaultbps->addItem("14400");
	ui->cbxDbx_dserial_defaultbps->addItem("19200");
	ui->cbxDbx_dserial_defaultbps->addItem("38400");
	ui->cbxDbx_dserial_defaultbps->addItem("57600");
	ui->cbxDbx_dserial_defaultbps->addItem("115200");
	ui->cbxDbx_dserial_defaultbps->addItem("128000");

	ui->cbxDbx_dserial_realport->clear();
	ui->cbxDbx_dserial_realport->addItem("COM1");
	ui->cbxDbx_dserial_realport->addItem("COM2");
	ui->cbxDbx_dserial_realport->addItem("COM3");
	ui->cbxDbx_dserial_realport->addItem("COM4");

	ui->cbxDbx_modem_comport->clear();
	ui->cbxDbx_modem_comport->addItem("COM1");
	ui->cbxDbx_modem_comport->addItem("COM2");
	ui->cbxDbx_modem_comport->addItem("COM3");
	ui->cbxDbx_modem_comport->addItem("COM4");

	ui->cbxDbx_dserial_bytesize->clear();
	ui->cbxDbx_dserial_bytesize->addItem("5");
	ui->cbxDbx_dserial_bytesize->addItem("8");

	ui->cbxDbx_dserial_parity->clear();
	ui->cbxDbx_dserial_parity->addItem("N");
	ui->cbxDbx_dserial_parity->addItem("E");
	ui->cbxDbx_dserial_parity->addItem("O");

	ui->cbxDbx_dserial_stopbit->clear();
	ui->cbxDbx_dserial_stopbit->addItem("1");
	ui->cbxDbx_dserial_stopbit->addItem("2");

	ui->cbxDbx_select_serial->clear();
	ui->cbxDbx_select_serial->addItem("serial1");
	ui->cbxDbx_select_serial->addItem("serial2");
	ui->cbxDbx_select_serial->addItem("serial3");
	ui->cbxDbx_select_serial->addItem("serial4");

	ui->cbxDbx_type_serial->clear();
	ui->cbxDbx_type_serial->addItem("disabled");
	ui->cbxDbx_type_serial->addItem("dummy");
	ui->cbxDbx_type_serial->addItem("modem");
	ui->cbxDbx_type_serial->addItem("directserial");

	ui->cbxDbx_ipx_type->clear();
	ui->cbxDbx_ipx_type->addItem("Ninguna" , "none");
	ui->cbxDbx_ipx_type->addItem("Cliente" , "client");
	ui->cbxDbx_ipx_type->addItem("Servidor", "server");

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
}

void frmAddEditDosBox::setTheme()
{
// General
	ui->btnDbx_ExeJuego->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_ExeJuego_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDbx_ExeSetup->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_ExeSetup_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
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
// Comunicación
	ui->btnDbx_AddSerial->setIcon(QIcon(fGrl->theme() +"img16/applications.png"));
// Autoexec
// Menu herramientas de montaje en Autoexec
	ui->btnToolDbx_mount_HD->setIcon(QIcon(fGrl->theme() +"img16/drive_hd.png"));
	ui->btnToolDbx_mount_CD->setIcon(QIcon(fGrl->theme() +"img16/drive_cdrom.png"));
	ui->btnToolDbx_mount_Floppy->setIcon(QIcon(fGrl->theme() +"img16/drive_floppy.png"));
	ui->btnToolDbx_mount_ImgFloppy->setIcon(QIcon(fGrl->theme() +"img16/floppy_1.png"));
	ui->btnToolDbx_mount_ImgCD->setIcon(QIcon(fGrl->theme() +"img16/cd_iso.png"));
	ui->btnToolDbx_mount_ImgHD->setIcon(QIcon(fGrl->theme() +"img16/drive_hd.png"));
// Miscelánea
	ui->btnDbx_mapperfile->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_mapperfile_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDbx_language->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbx_language_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
}

bool frmAddEditDosBox::isCorrectNext()
{
	bool siguiente = false;
	bool isBoot = false;

	const int count = ui->twMontajes->topLevelItemCount();
	if (count > 0)
	{
		for (int i = 0; i < count; ++i)
		{
			QTreeWidgetItem *item = ui->twMontajes->topLevelItem(i);
			if (item->text(2) == "boot")
				isBoot = true;
		}
	}

	if (ui->txtDbx_path_exe->text().isEmpty() && !isBoot)
		QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego para el DOSBox"));
	else
		siguiente = true;

	return siguiente;
}

void frmAddEditDosBox::previerMontajes()
{
	if (ui->twMontajes->topLevelItemCount() > 0)
	{
		stConfigDOSBox datos_montaje;
		datos_montaje.path_exe         = ui->txtDbx_path_exe->text();
		datos_montaje.parametros_exe   = ui->txtDbx_parametros_exe->text();
		datos_montaje.opt_loadfix_mem  = fGrl->intToStr(ui->spinDbx_loadfix_mem->value());
		datos_montaje.opt_loadfix      = fGrl->boolToStr(ui->chkDbx_loadfix->isChecked());
		datos_montaje.opt_cerrar_dbox  = fGrl->boolToStr(ui->chkDbx_cerrar_dbox->isChecked());
		datos_montaje.autoexec_ini_exe = ui->txtDbx_Autoexec_ini_exe->toPlainText();
		datos_montaje.autoexec_fin_exe = ui->txtDbx_Autoexec_fin_exe->toPlainText();
		QStringList list_mount = fGrl->creaConfigMontajes(getListMount(), datos_montaje);

		ui->previer_mount->clear();
		const int listSize = list_mount.size();
		for (int i= 0; i < listSize; ++i)
			ui->previer_mount->insertPlainText(list_mount.at(i) +"\n");
	} else
		ui->previer_mount->clear();
}

void frmAddEditDosBox::cargarDatosDosBox(stConfigDOSBox cfgDbx)
{
	DatosDosBox = cfgDbx;
// DOSBox version a usar
	if (cfgDbx.dosbox_emu_key.isEmpty() || !dbx_list.contains(cfgDbx.dosbox_emu_key))
	{
		const int num_dbx_count = dbx_list.count(grlCfg.DOSBoxDefault);
		const int num_dbx_size  = dbx_list.size();
		if (num_dbx_count > 0 || num_dbx_size > 0)
		{
			QHash<QString, stGrlDatos>::iterator i_dbx = dbx_list.begin();
			cfgDbx.dosbox_emu_key = i_dbx.key();
		} else
			cfgDbx.dosbox_emu_key = grlCfg.DOSBoxDefault;
	}

	isDbxSVN = fGrl->strToBool(dbx_list[cfgDbx.dosbox_emu_key].issvn);
	int index_dbx = ui->cbxDbx_EmuKey->findData(cfgDbx.dosbox_emu_key);
	ui->cbxDbx_EmuKey->setCurrentIndex((index_dbx < 0 ? 0 : index_dbx));

	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution  , grlDir.Datos +"dbx_resolution.txt", "img16/", "original", 2, "|"); // Resolución pantalla
	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_windowresolution, grlDir.Datos +"dbx_resolution.txt", "img16/", "original", 2, "|"); // windowresolution
	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output          , grlDir.Datos +"dbx_output.txt"    , "img16/", "surface" , 3, "|"); // Modo de Renderizado
	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine      , grlDir.Datos +"dbx_machine.txt"   , "img16/", "svga_s3" , 3, "|", true, true); // Tarjeta de Video
	fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler       , grlDir.Datos +"dbx_scaler.txt"    , "img16/", "normal2x", 3, "|", true, true); // Escalar y Filtros
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cputype         , grlDir.Datos +"dbx_cpu_type.txt"  , "img16/", "auto"    , 3, "|"); // Tipo de CPU para usar en la emulación

	if (isDbxSVN)
	{
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution  , grlDir.Datos +"dbx_resolution_svn.txt", "img16/", "", 2, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_windowresolution, grlDir.Datos +"dbx_resolution_svn.txt", "img16/", "", 2, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output          , grlDir.Datos +"dbx_output_svn.txt"    , "img16/", "", 3, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine      , grlDir.Datos +"dbx_machine_svn.txt"   , "img16/", "", 3, "|", false, true);
		fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler       , grlDir.Datos +"dbx_scaler_svn.txt"    , "img16/", "", 3, "|", false, true);
		fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cputype         , grlDir.Datos +"dbx_cpu_type_svn.txt"  , "img16/", "", 3, "|", false);
	}

	ui->cbxDbx_sdl_fullresolution->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "0x0", "0x0");

// [sdl]
	ui->chkDbx_sdl_fullscreen->setChecked(fGrl->strToBool(cfgDbx.sdl_fullscreen));
	ui->chkDbx_sdl_fulldouble->setChecked(fGrl->strToBool(cfgDbx.sdl_fulldouble));
	ui->chkDbx_sdl_fullfixed->setChecked(fGrl->strToBool(cfgDbx.sdl_fullfixed));
	ui->cbxDbx_sdl_fullresolution->setCurrentIndex(ui->cbxDbx_sdl_fullresolution->findData(cfgDbx.sdl_fullresolution));
	ui->cbxDbx_sdl_windowresolution->setCurrentIndex(ui->cbxDbx_sdl_windowresolution->findData(cfgDbx.sdl_windowresolution));
	ui->cbxDbx_sdl_output->setCurrentIndex(ui->cbxDbx_sdl_output->findData(cfgDbx.sdl_output));
	ui->cbxDbx_sdl_hwscale->setCurrentIndex(ui->cbxDbx_sdl_hwscale->findData(cfgDbx.sdl_hwscale));
	ui->chkDbx_sdl_autolock->setChecked(fGrl->strToBool(cfgDbx.sdl_autolock));
	ui->cbxDbx_sdl_sensitivity->setCurrentIndex(ui->cbxDbx_sdl_sensitivity->findData(cfgDbx.sdl_sensitivity));
	ui->chkDbx_sdl_waitonerror->setChecked(fGrl->strToBool(cfgDbx.sdl_waitonerror));

	QStringList sdl_priority_templist;
	sdl_priority_templist.clear();
	sdl_priority_templist = cfgDbx.sdl_priority.split(",");
	if (sdl_priority_templist.at(0) == "lower") ui->rbtPrioridadBaja->setChecked(true);
	if (sdl_priority_templist.at(0) == "normal") ui->rbtPrioridadNormal->setChecked(true);
	if (sdl_priority_templist.at(0) == "higher") ui->rbtPrioridadAlta->setChecked(true);
	if (sdl_priority_templist.at(0) == "highest") ui->rbtPrioridadMuyAlt->setChecked(true);
	if (sdl_priority_templist.at(1) == "lower") ui->rbtNoPrioridadBaja->setChecked(true);
	if (sdl_priority_templist.at(1) == "normal") ui->rbtNoPrioridadNormal->setChecked(true);
	if (sdl_priority_templist.at(1) == "higher") ui->rbtNoPrioridadAlta->setChecked(true);
	if (sdl_priority_templist.at(1) == "highest") ui->rbtNoPrioridadMuyAlt->setChecked(true);

	ui->txtDbx_sdl_mapperfile->setText(cfgDbx.sdl_mapperfile);
	ui->chkDbx_sdl_usescancodes->setChecked(fGrl->strToBool(cfgDbx.sdl_usescancodes));
// [dosbox]
	ui->txtDbx_dosbox_language->setText(cfgDbx.dosbox_language);
	ui->cbxDbx_dosbox_machine->setCurrentIndex(ui->cbxDbx_dosbox_machine->findData(cfgDbx.dosbox_machine));
	ui->cbxDbx_dosbox_memsize->setEditText(cfgDbx.dosbox_memsize);
// [render]
	ui->cbxDbx_render_frameskip->setCurrentIndex(ui->cbxDbx_render_frameskip->findData(cfgDbx.render_frameskip));
	ui->chkDbx_render_aspect->setChecked(fGrl->strToBool(cfgDbx.render_aspect));
	ui->cbxDbx_render_scaler->setCurrentIndex(ui->cbxDbx_render_scaler->findData(cfgDbx.render_scaler));
// [cpu]
	ui->cbxDbx_cpu_core->setCurrentIndex(ui->cbxDbx_cpu_core->findData(cfgDbx.cpu_core));
	ui->cbxDbx_cpu_cputype->setCurrentIndex(ui->cbxDbx_cpu_cputype->findData(cfgDbx.cpu_cputype));
	ui->cbxDbx_cpu_cycles->setCurrentIndex(ui->cbxDbx_cpu_cycles->findText(cfgDbx.cpu_cycles));
	ui->cbxDbx_cpu_cycles_realmode->setCurrentIndex(ui->cbxDbx_cpu_cycles_realmode->findText(cfgDbx.cpu_cycles_realmode));
	ui->cbxDbx_cpu_cycles_protmode->setCurrentIndex(ui->cbxDbx_cpu_cycles_protmode->findText(cfgDbx.cpu_cycles_protmode));
	ui->cbxDbx_cpu_cycles_limitmode->setCurrentIndex(ui->cbxDbx_cpu_cycles_limitmode->findText(cfgDbx.cpu_cycles_limitmode));
	ui->spinDbx_cpu_cycleup->setValue(fGrl->strToInt(cfgDbx.cpu_cycleup));
	ui->spinDbx_cpu_cycledown->setValue(fGrl->strToInt(cfgDbx.cpu_cycledown));
// [mixer]
	ui->chkDbx_mixer_nosound->setChecked(!fGrl->strToBool(cfgDbx.mixer_nosound));
	ui->cbxDbx_mixer_rate->setCurrentIndex(ui->cbxDbx_mixer_rate->findText(cfgDbx.mixer_rate));
	ui->txtDbx_mixer_blocksize->setText(cfgDbx.mixer_blocksize);
	ui->txtDbx_mixer_prebuffer->setText(cfgDbx.mixer_prebuffer);
// [midi]
	ui->cbxDbx_midi_mpu401->setCurrentIndex(ui->cbxDbx_midi_mpu401->findText(cfgDbx.midi_mpu401));
	ui->chkDbx_midi_intelligent->setChecked(fGrl->strToBool(cfgDbx.midi_intelligent));
	ui->cbxDbx_midi_device->setCurrentIndex(ui->cbxDbx_midi_device->findText(cfgDbx.midi_device));
	ui->txtDbx_midi_config->setText(cfgDbx.midi_config);
	ui->cbxDbx_midi_mt32rate->setCurrentIndex(ui->cbxDbx_midi_mt32rate->findText(cfgDbx.midi_mt32rate));
// [sblaster]
	ui->cbxDbx_sblaster_sbtype->setCurrentIndex(ui->cbxDbx_sblaster_sbtype->findData(cfgDbx.sblaster_sbtype));
	ui->cbxDbx_sblaster_sbbase->setCurrentIndex(ui->cbxDbx_sblaster_sbbase->findText(cfgDbx.sblaster_sbbase));
	ui->cbxDbx_sblaster_irq->setCurrentIndex(ui->cbxDbx_sblaster_irq->findText(cfgDbx.sblaster_irq));
	ui->cbxDbx_sblaster_dma->setCurrentIndex(ui->cbxDbx_sblaster_dma->findText(cfgDbx.sblaster_dma));
	ui->cbxDbx_sblaster_hdma->setCurrentIndex(ui->cbxDbx_sblaster_hdma->findText(cfgDbx.sblaster_hdma));
	ui->chkDbx_sblaster_mixer->setChecked(fGrl->strToBool(cfgDbx.sblaster_mixer));
	ui->cbxDbx_sblaster_oplmode->setCurrentIndex(ui->cbxDbx_sblaster_oplmode->findData(cfgDbx.sblaster_oplmode));
	ui->cbxDbx_sblaster_oplemu->setCurrentIndex(ui->cbxDbx_sblaster_oplemu->findText(cfgDbx.sblaster_oplemu));
	ui->cbxDbx_sblaster_oplrate->setCurrentIndex(ui->cbxDbx_sblaster_oplrate->findText(cfgDbx.sblaster_oplrate));
// [gus]
	ui->chkDbx_gus_gus->setChecked(fGrl->strToBool(cfgDbx.gus_gus));
	ui->cbxDbx_gus_gusrate->setCurrentIndex(ui->cbxDbx_gus_gusrate->findText(cfgDbx.gus_gusrate));
	ui->cbxDbx_gus_gusbase->setCurrentIndex(ui->cbxDbx_gus_gusbase->findText(cfgDbx.gus_gusbase));
	ui->cbxDbx_gus_irq1->setCurrentIndex(ui->cbxDbx_gus_irq1->findText(cfgDbx.gus_irq1));
	ui->cbxDbx_gus_irq2->setCurrentIndex(ui->cbxDbx_gus_irq2->findText(cfgDbx.gus_irq2));
	ui->cbxDbx_gus_dma1->setCurrentIndex(ui->cbxDbx_gus_dma1->findText(cfgDbx.gus_dma1));
	ui->cbxDbx_gus_dma2->setCurrentIndex(ui->cbxDbx_gus_dma2->findText(cfgDbx.gus_dma2));
	ui->txtDbx_gus_ultradir->setText(cfgDbx.gus_ultradir.replace("/","\\"));
// [speaker]
	ui->chkDbx_speaker_pcspeaker->setChecked(fGrl->strToBool(cfgDbx.speaker_pcspeaker));
	ui->cbxDbx_speaker_pcrate->setCurrentIndex(ui->cbxDbx_speaker_pcrate->findText(cfgDbx.speaker_pcrate));
	ui->cbxDbx_speaker_tandy->setCurrentIndex(ui->cbxDbx_speaker_tandy->findText(cfgDbx.speaker_tandy));
	ui->cbxDbx_speaker_tandyrate->setCurrentIndex(ui->cbxDbx_speaker_tandyrate->findText(cfgDbx.speaker_tandyrate));
	ui->chkDbx_speaker_disney->setChecked(fGrl->strToBool(cfgDbx.speaker_disney));
// [joystick]
	ui->cbxDbx_joystick_type->setCurrentIndex(ui->cbxDbx_joystick_type->findData(cfgDbx.joystick_type));
	ui->chkDbx_joystick_timed->setChecked(fGrl->strToBool(cfgDbx.joystick_timed));
	ui->chkDbx_joystick_autofire->setChecked(fGrl->strToBool(cfgDbx.joystick_autofire));
	ui->chkDbx_joystick_swap34->setChecked(fGrl->strToBool(cfgDbx.joystick_swap34));
	ui->chkDbx_joystick_buttonwrap->setChecked(fGrl->strToBool(cfgDbx.joystick_buttonwrap));
// [modem]
	ui->chkDbx_modem_modem->setChecked(fGrl->strToBool(cfgDbx.modem_modem));
	ui->cbxDbx_modem_comport->setCurrentIndex(fGrl->strToInt(cfgDbx.modem_comport)-1);
	ui->txtDbx_modem_listenport->setText(cfgDbx.modem_listenport);
// [dserial]
	ui->chkDbx_dserial_directserial->setChecked(fGrl->strToBool(cfgDbx.dserial_directserial));
	ui->cbxDbx_dserial_comport->setCurrentIndex(ui->cbxDbx_dserial_comport->findText(cfgDbx.dserial_comport));
	ui->cbxDbx_dserial_realport->setCurrentIndex(ui->cbxDbx_dserial_realport->findText(cfgDbx.dserial_realport));
	ui->cbxDbx_dserial_defaultbps->setCurrentIndex(ui->cbxDbx_dserial_defaultbps->findText(cfgDbx.dserial_defaultbps));
	ui->cbxDbx_dserial_parity->setCurrentIndex(ui->cbxDbx_dserial_parity->findText(cfgDbx.dserial_parity));
	ui->cbxDbx_dserial_bytesize->setCurrentIndex(ui->cbxDbx_dserial_bytesize->findText(cfgDbx.dserial_bytesize));
	ui->cbxDbx_dserial_stopbit->setCurrentIndex(ui->cbxDbx_dserial_stopbit->findText(cfgDbx.dserial_stopbit));
// [serial]
	ui->txtDbx_serial_1->setText(cfgDbx.serial_1);
	ui->txtDbx_serial_2->setText(cfgDbx.serial_2);
	ui->txtDbx_serial_3->setText(cfgDbx.serial_3);
	ui->txtDbx_serial_4->setText(cfgDbx.serial_4);
// [dos]
	ui->chkDbx_dos_xms->setChecked(fGrl->strToBool(cfgDbx.dos_xms));
	ui->chkDbx_dos_ems->setChecked(fGrl->strToBool(cfgDbx.dos_ems));
	ui->cbxDbx_dos_umb->setCurrentIndex(ui->cbxDbx_dos_umb->findText(cfgDbx.dos_umb));
	ui->cbxDbx_dos_keyboardlayout->setCurrentIndex(ui->cbxDbx_dos_keyboardlayout->findData(cfgDbx.dos_keyboardlayout));
	ui->cbxDbx_dos_version->setCurrentIndex(ui->cbxDbx_dos_version->findData(cfgDbx.dos_version));
// [ipx]
	ui->chkDbx_ipx_ipx->setChecked(fGrl->strToBool(cfgDbx.ipx_ipx));
	ui->cbxDbx_ipx_type->setCurrentIndex(ui->cbxDbx_ipx_type->findData(cfgDbx.ipx_type));
	ui->spinDbx_ipx_port->setValue(cfgDbx.ipx_port.toInt());
	ui->txtDbx_ipx_ip->setText(cfgDbx.ipx_ip);
// [autoexec]
	ui->txtDbx_Autoexec->setPlainText(cfgDbx.autoexec);
	ui->txtDbx_Autoexec_ini->setPlainText(cfgDbx.autoexec_ini);
	ui->txtDbx_Autoexec_fin->setPlainText(cfgDbx.autoexec_fin);
	ui->txtDbx_Autoexec_ini_exe->setPlainText(cfgDbx.autoexec_ini_exe);
	ui->txtDbx_Autoexec_fin_exe->setPlainText(cfgDbx.autoexec_fin_exe);
// Opciones
	ui->chkDbx_autoexec->setChecked(fGrl->strToBool(cfgDbx.opt_autoexec));
	ui->chkDbx_loadfix->setChecked(fGrl->strToBool(cfgDbx.opt_loadfix));
	ui->spinDbx_loadfix_mem->setValue(cfgDbx.opt_loadfix_mem.toInt());
	emit on_spinDbx_loadfix_mem_valueChanged(cfgDbx.opt_loadfix_mem.toInt());

	ui->chkDbx_consola_dbox->setChecked(fGrl->strToBool(cfgDbx.opt_consola_dbox));
	ui->chkDbx_cerrar_dbox->setChecked(fGrl->strToBool(cfgDbx.opt_cerrar_dbox));
	ui->chkDbx_cycle_sincronizar->setChecked(fGrl->strToBool(cfgDbx.opt_cycle_sincronizar));
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

	if (Editando && !IdGame.isEmpty())//&& !isProfileGame
	{
		IdDbx = cfgDbx.id;
		cargarDatosDBxMontaje(IdDbx);
	} else
		IdDbx = "";
}

void frmAddEditDosBox::cargarDatosDBxMontaje(QString IDdbx)
{
	QString tipoDrive, select_mount;
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT id, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdbx +" ORDER BY id_lista");
	if (query.first())
	{
		ui->twMontajes->clear();
		QList<QTreeWidgetItem *> items;
		do {
			QTreeWidgetItem *item = new QTreeWidgetItem;
			tipoDrive    = query.record().value("tipo_as").toString();
			select_mount = query.record().value("select_mount").toString() == "v" ? "s_" : "";

			item->setTextAlignment(1, Qt::AlignCenter);
			item->setTextAlignment(2, Qt::AlignCenter);
			item->setIcon( 0, QIcon(fGrl->getIconMount(tipoDrive, select_mount)));
			item->setText( 0, query.record().value("path").toString()        );	// Directorio o iso
			item->setText( 1, query.record().value("letter").toString()      );	// Letra de montaje
			item->setText( 2, query.record().value("tipo_as").toString()     );	// Tipo de montaje
			item->setText( 3, query.record().value("label").toString()       );	// Etiqueta
			item->setText( 4, query.record().value("indx_cd").toString()     );	// Index de la unidad de cd-rom
			item->setText( 5, query.record().value("opt_mount").toString()   );	// Opciones del cd-rom
			item->setText( 6, query.record().value("io_ctrl").toString()     );	// Cd/dvd
			item->setText( 7, query.record().value("select_mount").toString());	// Primer montaje
			item->setText( 8, query.record().value("id").toString()          );	// Id
			item->setText( 9, query.record().value("id_lista").toString()    );	// Id_lista
			item->setText(10, query.record().value("opt_size").toString()    );
			item->setText(11, query.record().value("opt_freesize").toString());
			item->setText(12, query.record().value("freesize").toString()    );
			items << item;
		} while (query.next());
		ui->twMontajes->addTopLevelItems(items);
	}
	previerMontajes();
}

void frmAddEditDosBox::cargarDatosDBxMontaje(QList<stConfigDOSBoxMount> listMount)
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

QList<stConfigDOSBoxMount> frmAddEditDosBox::getListMount()
{
	return fGrl->getListCfgMount(ui->twMontajes);
}

void frmAddEditDosBox::setDatosDosBox()
{
	QString str, prioritySelect, priorityNoSelect;

	if (ui->rbtPrioridadBaja->isChecked()) prioritySelect = "lower";
	if (ui->rbtPrioridadNormal->isChecked()) prioritySelect = "normal";
	if (ui->rbtPrioridadAlta->isChecked()) prioritySelect = "higher";
	if (ui->rbtPrioridadMuyAlt->isChecked()) prioritySelect = "highest";
	if (ui->rbtNoPrioridadBaja->isChecked()) priorityNoSelect = "lower";
	if (ui->rbtNoPrioridadNormal->isChecked()) priorityNoSelect = "normal";
	if (ui->rbtNoPrioridadAlta->isChecked()) priorityNoSelect = "higher";
	if (ui->rbtNoPrioridadMuyAlt->isChecked()) priorityNoSelect = "highest";

	DatosDosBox.id    = IdDbx;
	DatosDosBox.idgrl = IdGame;
	DatosDosBox.idcat = categoria.id;
// [sdl]
	DatosDosBox.sdl_fullscreen        = fGrl->boolToStr(ui->chkDbx_sdl_fullscreen->isChecked());
	DatosDosBox.sdl_fulldouble        = fGrl->boolToStr(ui->chkDbx_sdl_fulldouble->isChecked());
	DatosDosBox.sdl_fullfixed         = fGrl->boolToStr(ui->chkDbx_sdl_fullfixed->isChecked());
	str = ui->cbxDbx_sdl_fullresolution->itemData(ui->cbxDbx_sdl_fullresolution->currentIndex()).toString();
	DatosDosBox.sdl_fullresolution    = str.isEmpty() ? "original" : str;
	str = ui->cbxDbx_sdl_windowresolution->itemData(ui->cbxDbx_sdl_windowresolution->currentIndex()).toString();
	DatosDosBox.sdl_windowresolution  = str.isEmpty() ? "original" : str;
	str = ui->cbxDbx_sdl_output->itemData(ui->cbxDbx_sdl_output->currentIndex()).toString();
	DatosDosBox.sdl_output            = str.isEmpty() ? "surface" : str;
	str = ui->cbxDbx_sdl_hwscale->itemData(ui->cbxDbx_sdl_hwscale->currentIndex()).toString();
	DatosDosBox.sdl_hwscale           = str.isEmpty() ? "1.00" : str;
	DatosDosBox.sdl_autolock          = fGrl->boolToStr(ui->chkDbx_sdl_autolock->isChecked());
	str = ui->cbxDbx_sdl_sensitivity->itemData(ui->cbxDbx_sdl_sensitivity->currentIndex()).toString();
	DatosDosBox.sdl_sensitivity       = str.isEmpty() ? "100" : str;
	DatosDosBox.sdl_waitonerror       = fGrl->boolToStr(ui->chkDbx_sdl_waitonerror->isChecked());
	DatosDosBox.sdl_priority          = (!prioritySelect.isEmpty() && !priorityNoSelect.isEmpty()) ? prioritySelect +","+ priorityNoSelect : "higher,normal";
	DatosDosBox.sdl_mapperfile        = ui->txtDbx_sdl_mapperfile->text().isEmpty() ? "mapper.txt" : ui->txtDbx_sdl_mapperfile->text();
	DatosDosBox.sdl_usescancodes      = fGrl->boolToStr(ui->chkDbx_sdl_usescancodes->isChecked());
// [dosbox]
	DatosDosBox.dosbox_language       = ui->txtDbx_dosbox_language->text().isEmpty() ? "" : ui->txtDbx_dosbox_language->text();
	str = ui->cbxDbx_dosbox_machine->itemData(ui->cbxDbx_dosbox_machine->currentIndex()).toString();
	DatosDosBox.dosbox_machine        = str.isEmpty() ? "svga_s3" : str;
	DatosDosBox.dosbox_memsize        = ui->cbxDbx_dosbox_memsize->currentText().isEmpty() ? "16" : ui->cbxDbx_dosbox_memsize->currentText();
	str = ui->cbxDbx_EmuKey->itemData(ui->cbxDbx_EmuKey->currentIndex()).toString();
	DatosDosBox.dosbox_emu_key        = str.isEmpty() ? "dosbox" : str;
// [render]
	str = ui->cbxDbx_render_frameskip->itemData(ui->cbxDbx_render_frameskip->currentIndex()).toString();
	DatosDosBox.render_frameskip      = str.isEmpty() ? "0" : str;
	DatosDosBox.render_aspect         = fGrl->boolToStr(ui->chkDbx_render_aspect->isChecked());
	str = ui->cbxDbx_render_scaler->itemData(ui->cbxDbx_render_scaler->currentIndex()).toString();
	DatosDosBox.render_scaler         = str.isEmpty() ? "normal2x" : str;
// [cpu]
	str = ui->cbxDbx_cpu_core->itemData(ui->cbxDbx_cpu_core->currentIndex()).toString();
	DatosDosBox.cpu_core              = str.isEmpty() ? "auto" : str;
	str = ui->cbxDbx_cpu_cputype->itemData(ui->cbxDbx_cpu_cputype->currentIndex()).toString();
	DatosDosBox.cpu_cputype           = str.isEmpty() ? "auto" : str;
	DatosDosBox.cpu_cycles            = ui->cbxDbx_cpu_cycles->currentText().isEmpty() ? "auto" : ui->cbxDbx_cpu_cycles->currentText();
	DatosDosBox.cpu_cycles_realmode   = ui->cbxDbx_cpu_cycles_realmode->currentText().isEmpty() ? "" : ui->cbxDbx_cpu_cycles_realmode->currentText();
	DatosDosBox.cpu_cycles_protmode   = ui->cbxDbx_cpu_cycles_protmode->currentText().isEmpty() ? "" : ui->cbxDbx_cpu_cycles_protmode->currentText();
	DatosDosBox.cpu_cycles_limitmode  = ui->cbxDbx_cpu_cycles_limitmode->currentText().isEmpty() ? "" : ui->cbxDbx_cpu_cycles_limitmode->currentText();
	DatosDosBox.cpu_cycleup           = ui->spinDbx_cpu_cycleup->value() >= 0 ? fGrl->intToStr(ui->spinDbx_cpu_cycleup->value()) : "10";
	DatosDosBox.cpu_cycledown         = ui->spinDbx_cpu_cycledown->value() >= 0 ? fGrl->intToStr(ui->spinDbx_cpu_cycledown->value()) : "20";
// [mixer]
	DatosDosBox.mixer_nosound         = fGrl->boolToStr(!ui->chkDbx_mixer_nosound->isChecked());
	DatosDosBox.mixer_rate            = ui->cbxDbx_mixer_rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_mixer_rate->currentText();
	DatosDosBox.mixer_blocksize       = ui->txtDbx_mixer_blocksize->text().isEmpty() ? "1024" : ui->txtDbx_mixer_blocksize->text();
	DatosDosBox.mixer_prebuffer       = ui->txtDbx_mixer_prebuffer->text().isEmpty() ? "20" : ui->txtDbx_mixer_prebuffer->text();
// [midi]
	DatosDosBox.midi_mpu401           = ui->cbxDbx_midi_mpu401->currentText().isEmpty() ? "intelligent" : ui->cbxDbx_midi_mpu401->currentText();
	DatosDosBox.midi_intelligent      = fGrl->boolToStr(ui->chkDbx_midi_intelligent->isChecked());
	DatosDosBox.midi_device           = ui->cbxDbx_midi_device->currentText().isEmpty() ? "default" : ui->cbxDbx_midi_device->currentText();
	DatosDosBox.midi_config           = ui->txtDbx_midi_config->text().isEmpty() ? "" : ui->txtDbx_midi_config->text();
	DatosDosBox.midi_mt32rate         = ui->cbxDbx_midi_mt32rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_midi_mt32rate->currentText();
// [sblaster]
	str = ui->cbxDbx_sblaster_sbtype->itemData(ui->cbxDbx_sblaster_sbtype->currentIndex()).toString();
	DatosDosBox.sblaster_sbtype       = str.isEmpty() ? "sb16" : str;
	DatosDosBox.sblaster_sbbase       = ui->cbxDbx_sblaster_sbbase->currentText().isEmpty() ? "220" : ui->cbxDbx_sblaster_sbbase->currentText();
	DatosDosBox.sblaster_irq          = ui->cbxDbx_sblaster_irq->currentText().isEmpty() ? "7" : ui->cbxDbx_sblaster_irq->currentText();
	DatosDosBox.sblaster_dma          = ui->cbxDbx_sblaster_dma->currentText().isEmpty() ? "1" : ui->cbxDbx_sblaster_dma->currentText();
	DatosDosBox.sblaster_hdma         = ui->cbxDbx_sblaster_hdma->currentText().isEmpty() ? "5" : ui->cbxDbx_sblaster_hdma->currentText();
	DatosDosBox.sblaster_mixer        = fGrl->boolToStr(ui->chkDbx_sblaster_mixer->isChecked());
	str = ui->cbxDbx_sblaster_oplmode->itemData(ui->cbxDbx_sblaster_oplmode->currentIndex()).toString();
	DatosDosBox.sblaster_oplmode      = str.isEmpty() ? "auto" : str;
	DatosDosBox.sblaster_oplemu       = ui->cbxDbx_sblaster_oplemu->currentText().isEmpty() ? "default" : ui->cbxDbx_sblaster_oplemu->currentText();
	DatosDosBox.sblaster_oplrate      = ui->cbxDbx_sblaster_oplrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_sblaster_oplrate->currentText();
// [gus]
	DatosDosBox.gus_gus               = fGrl->boolToStr(ui->chkDbx_gus_gus->isChecked());
	DatosDosBox.gus_gusrate           = ui->cbxDbx_gus_gusrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_gus_gusrate->currentText();
	DatosDosBox.gus_gusbase           = ui->cbxDbx_gus_gusbase->currentText().isEmpty() ? "240" : ui->cbxDbx_gus_gusbase->currentText();
	DatosDosBox.gus_irq1              = ui->cbxDbx_gus_irq1->currentText().isEmpty() ? "5" : ui->cbxDbx_gus_irq1->currentText();
	DatosDosBox.gus_irq2              = ui->cbxDbx_gus_irq2->currentText().isEmpty() ? "5" : ui->cbxDbx_gus_irq2->currentText();
	DatosDosBox.gus_dma1              = ui->cbxDbx_gus_dma1->currentText().isEmpty() ? "3" : ui->cbxDbx_gus_dma1->currentText();
	DatosDosBox.gus_dma2              = ui->cbxDbx_gus_dma2->currentText().isEmpty() ? "3" : ui->cbxDbx_gus_dma2->currentText();
	DatosDosBox.gus_ultradir          = ui->txtDbx_gus_ultradir->text().isEmpty() ? "C:\\ULTRASND" : ui->txtDbx_gus_ultradir->text();
// [speaker]
	DatosDosBox.speaker_pcspeaker     = fGrl->boolToStr(ui->chkDbx_speaker_pcspeaker->isChecked());
	DatosDosBox.speaker_pcrate        = ui->cbxDbx_speaker_pcrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_pcrate->currentText();
	DatosDosBox.speaker_tandy         = ui->cbxDbx_speaker_tandy->currentText().isEmpty() ? "auto" : ui->cbxDbx_speaker_tandy->currentText();
	DatosDosBox.speaker_tandyrate     = ui->cbxDbx_speaker_tandyrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_tandyrate->currentText();
	DatosDosBox.speaker_disney        = fGrl->boolToStr(ui->chkDbx_speaker_disney->isChecked());
// [joystick]
	str = ui->cbxDbx_joystick_type->itemData(ui->cbxDbx_joystick_type->currentIndex()).toString();
	DatosDosBox.joystick_type         = str.isEmpty() ? "auto" : str;
	DatosDosBox.joystick_timed        = fGrl->boolToStr(ui->chkDbx_joystick_timed->isChecked());
	DatosDosBox.joystick_autofire     = fGrl->boolToStr(ui->chkDbx_joystick_autofire->isChecked());
	DatosDosBox.joystick_swap34       = fGrl->boolToStr(ui->chkDbx_joystick_swap34->isChecked());
	DatosDosBox.joystick_buttonwrap   = fGrl->boolToStr(ui->chkDbx_joystick_buttonwrap->isChecked());
// [modem]
	DatosDosBox.modem_modem           = fGrl->boolToStr(ui->chkDbx_modem_modem->isChecked());
	DatosDosBox.modem_comport         = ui->cbxDbx_modem_comport->currentText().isEmpty() ? "2" : fGrl->intToStr(ui->cbxDbx_modem_comport->currentIndex()+1);
	DatosDosBox.modem_listenport      = ui->txtDbx_modem_listenport->text().isEmpty() ? "" : ui->txtDbx_modem_listenport->text();
// [dserial]
	DatosDosBox.dserial_directserial  = fGrl->boolToStr(ui->chkDbx_dserial_directserial->isChecked());// dserial_directserial
	DatosDosBox.dserial_comport       = ui->cbxDbx_dserial_comport->currentText().isEmpty() ? "1" : ui->cbxDbx_dserial_comport->currentText();
	DatosDosBox.dserial_realport      = ui->cbxDbx_dserial_realport->currentText().isEmpty() ? "COM1" : ui->cbxDbx_dserial_realport->currentText();
	DatosDosBox.dserial_defaultbps    = ui->cbxDbx_dserial_defaultbps->currentText().isEmpty() ? "1200" : ui->cbxDbx_dserial_defaultbps->currentText();
	DatosDosBox.dserial_parity        = ui->cbxDbx_dserial_parity->currentText().isEmpty() ? "N" : ui->cbxDbx_dserial_parity->currentText();
	DatosDosBox.dserial_bytesize      = ui->cbxDbx_dserial_bytesize->currentText().isEmpty() ? "8" : ui->cbxDbx_dserial_bytesize->currentText();
	DatosDosBox.dserial_stopbit       = ui->cbxDbx_dserial_stopbit->currentText().isEmpty() ? "1" : ui->cbxDbx_dserial_stopbit->currentText();
	DatosDosBox.serial_1              = ui->txtDbx_serial_1->text().isEmpty() ? "dummy" : ui->txtDbx_serial_1->text();
	DatosDosBox.serial_2              = ui->txtDbx_serial_2->text().isEmpty() ? "dummy" : ui->txtDbx_serial_2->text();
	DatosDosBox.serial_3              = ui->txtDbx_serial_3->text().isEmpty() ? "disabled" : ui->txtDbx_serial_3->text();
	DatosDosBox.serial_4              = ui->txtDbx_serial_4->text().isEmpty() ? "disabled" : ui->txtDbx_serial_4->text();
// [dos]
	DatosDosBox.dos_xms               = fGrl->boolToStr(ui->chkDbx_dos_xms->isChecked());
	DatosDosBox.dos_ems               = fGrl->boolToStr(ui->chkDbx_dos_ems->isChecked());
	DatosDosBox.dos_umb               = ui->cbxDbx_dos_umb->currentText().isEmpty() ? "true" : ui->cbxDbx_dos_umb->currentText();
	str = ui->cbxDbx_dos_keyboardlayout->itemData(ui->cbxDbx_dos_keyboardlayout->currentIndex()).toString();
	DatosDosBox.dos_keyboardlayout    = str.isEmpty() ? "auto" : str;
	str = ui->cbxDbx_dos_version->itemData(ui->cbxDbx_dos_version->currentIndex()).toString();
	DatosDosBox.dos_version           = str.isEmpty() ? "" : str;
// [ipx]
	DatosDosBox.ipx_ipx               = fGrl->boolToStr(ui->chkDbx_ipx_ipx->isChecked());
	str = ui->cbxDbx_ipx_type->itemData(ui->cbxDbx_ipx_type->currentIndex()).toString();
	DatosDosBox.ipx_type              = str.isEmpty() ? "ipx_none" : str;
	DatosDosBox.ipx_port              = ui->spinDbx_ipx_port->value() > 0 ? fGrl->intToStr(ui->spinDbx_ipx_port->value()) : "213";
	DatosDosBox.ipx_ip                = ui->txtDbx_ipx_ip->text();
// [autoexec]
	DatosDosBox.autoexec              = ui->txtDbx_Autoexec->toPlainText().isEmpty() ? "" : ui->txtDbx_Autoexec->toPlainText();
	DatosDosBox.autoexec_ini          = ui->txtDbx_Autoexec_ini->toPlainText().isEmpty() ? "" : ui->txtDbx_Autoexec_ini->toPlainText();
	DatosDosBox.autoexec_fin          = ui->txtDbx_Autoexec_fin->toPlainText().isEmpty() ? "" : ui->txtDbx_Autoexec_fin->toPlainText();
	DatosDosBox.autoexec_ini_exe      = ui->txtDbx_Autoexec_ini_exe->toPlainText().isEmpty() ? "" : ui->txtDbx_Autoexec_ini_exe->toPlainText();
	DatosDosBox.autoexec_fin_exe      = ui->txtDbx_Autoexec_fin_exe->toPlainText().isEmpty() ? "" : ui->txtDbx_Autoexec_fin_exe->toPlainText();
// Opciones
	DatosDosBox.opt_autoexec          = fGrl->boolToStr(ui->chkDbx_autoexec->isChecked());
	DatosDosBox.opt_loadfix           = fGrl->boolToStr(ui->chkDbx_loadfix->isChecked());
	DatosDosBox.opt_loadfix_mem       = fGrl->intToStr(ui->spinDbx_loadfix_mem->value());
	DatosDosBox.opt_consola_dbox      = fGrl->boolToStr(ui->chkDbx_consola_dbox->isChecked());
	DatosDosBox.opt_cerrar_dbox       = fGrl->boolToStr(ui->chkDbx_cerrar_dbox->isChecked());
	DatosDosBox.opt_cycle_sincronizar = fGrl->boolToStr(ui->chkDbx_cycle_sincronizar->isChecked());
// Otras opciones
	DatosDosBox.path_exe              = ui->txtDbx_path_exe->text().isEmpty() ? "" : ui->txtDbx_path_exe->text();
	DatosDosBox.path_setup            = ui->txtDbx_path_setup->text().isEmpty() ? "" : ui->txtDbx_path_setup->text();
	DatosDosBox.parametros_exe        = ui->txtDbx_parametros_exe->text().isEmpty() ? "" : ui->txtDbx_parametros_exe->text();
	DatosDosBox.parametros_setup      = ui->txtDbx_parametros_setup->text().isEmpty() ? "" : ui->txtDbx_parametros_setup->text();
}

// Menu herramintas de montaje en Autoexec
void frmAddEditDosBox::addMontajeAutoexec(QString tipo, QString dir_file)
{
// Montando disquetera, disco duro, CDs, imagenes de discos, disquetes y CDs
	if (!dir_file.isEmpty())
	{
		if (tipo == "floppy")
			ui->txtDbx_Autoexec->insertPlainText("mount A \""+ dir_file +"\" -t floppy\n");
		if (tipo == "driver")
			ui->txtDbx_Autoexec->insertPlainText("mount C \""+ dir_file +"\"\n");
		if (tipo == "cdrom")
			ui->txtDbx_Autoexec->insertPlainText("mount D \""+ dir_file +"\" -t cdrom\n");
		if (tipo == "IMG_floppy")
			ui->txtDbx_Autoexec->insertPlainText("imgmount A \""+ dir_file +"\" -t floppy\n");
		if (tipo == "IMG_iso")
			ui->txtDbx_Autoexec->insertPlainText("imgmount D \""+ dir_file +"\" -t cdrom\n");
		if (tipo == "IMG_hdd")
			ui->txtDbx_Autoexec->insertPlainText("imgmount C \""+ dir_file +"\" -t hdd\n");
	}
}

QString frmAddEditDosBox::setOpcionesSerial()
{
	QString Serial_Dbx;
	switch (ui->cbxDbx_type_serial->currentIndex())
	{
		case 0: // diasabled
			Serial_Dbx = ui->cbxDbx_type_serial->currentText();
		break;
		case 1: // dummy
			Serial_Dbx = ui->cbxDbx_type_serial->currentText();
		break;
		case 2: // modem
			Serial_Dbx = ui->cbxDbx_type_serial->currentText() +" "+
						"listenport:"+ ui->txtDbx_modem_listenport->text() +" "+
						"realport:"+ ui->cbxDbx_dserial_realport->currentText() +" "+
						"startbps:"+ ui->cbxDbx_dserial_defaultbps->currentText() +" "+
						"parity:"+ ui->cbxDbx_dserial_parity->currentText() +" "+
						"bytesize:"+ ui->cbxDbx_dserial_bytesize->currentText() +" "+
						"stopbits:"+ ui->cbxDbx_dserial_stopbit->currentText() +" "+
						"irq:"+ ui->txtDbx_modem_irq->text();
		break;
		case 3: // directserial
			Serial_Dbx = ui->cbxDbx_type_serial->currentText() +" "+
						"realport:"+ ui->cbxDbx_dserial_realport->currentText() +" "+
						"startbps:"+ ui->cbxDbx_dserial_defaultbps->currentText() +" "+
						"parity:"+ ui->cbxDbx_dserial_parity->currentText() +" "+
						"bytesize:"+ ui->cbxDbx_dserial_bytesize->currentText() +" "+
						"stopbits:"+ ui->cbxDbx_dserial_stopbit->currentText() +" "+
						"irq:"+ ui->txtDbx_modem_irq->text();
		break;
	}
	return Serial_Dbx;
}

void frmAddEditDosBox::on_cbxDbx_EmuKey_activated(int index)
{
	if (index > -1)
	{
		isDbxSVN = fGrl->strToBool(dbx_list[ui->cbxDbx_EmuKey->itemData(index).toString()].issvn);

		setDatosDosBox();

		cargarDatosDosBox(DatosDosBox);
	}
}

void frmAddEditDosBox::on_cbxDbx_Profiles_activated(int index)
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
		}/* else {
			DatosDosBox = sql->getDefectDOSBox();
			cargarDatosDosBox(DatosDosBox);
		}*/
	}
}

void frmAddEditDosBox::on_chkDbx_loadfix_clicked()
{
	emit on_spinDbx_loadfix_mem_valueChanged(DatosDosBox.opt_loadfix_mem.toInt());
}

void frmAddEditDosBox::on_spinDbx_loadfix_mem_valueChanged(int arg1)
{
	int total = ui->chkDbx_loadfix->isChecked() ? 632-arg1 : 632;
	ui->lb_Dbx_70->setText(tr("%1K memoria convencional restante").arg(total));
}

void frmAddEditDosBox::on_cbxDbx_dos_keyboardlayout_activated(int index)
{
	if (index > -1)
		dos_keyboardlayout = ui->cbxDbx_dos_keyboardlayout->itemData(index).toString();
}

void frmAddEditDosBox::on_btnDbx_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Dbx_path_exe, "DosGames", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbx_path_exe->setText(fGrl->setDirRelative(archivo, "DosGames"));
			grlCfg.Dbx_path_exe = fGrl->setDirRelative(f_info.Path, "DosGames");

			if (ui->twMontajes->topLevelItemCount() < 1 && !Editando)
				emit on_btnMount_AutoCrear_clicked();
			else
				previerMontajes();
		}
	}
}

void frmAddEditDosBox::on_btnDbx_ExeJuego_clear_clicked()
{
	ui->txtDbx_path_exe->clear();
}

void frmAddEditDosBox::on_btnDbx_ExeSetup_clicked()
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

void frmAddEditDosBox::on_btnDbx_ExeSetup_clear_clicked()
{
	ui->txtDbx_path_setup->clear();
}

void frmAddEditDosBox::on_btnInstalarJuego_clicked()
{
	frmInstalarJuego *instalarJuego = new frmInstalarJuego(grlCfg, this);
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
					item->setText( 9, list_mount.at(m).id_lista    );
					item->setText(10, list_mount.at(m).opt_size    );
					item->setText(11, list_mount.at(m).opt_freesize);
					item->setText(12, list_mount.at(m).freesize    );

					if (Editando)
					{
						list_mount[m].id_dosbox = IdDbx;
						item->setText(8, sql->insertaUnMontajeDbx(list_mount.at(m)));
					} else
						item->setText(8, "");

					ui->twMontajes->addTopLevelItem(item);
					sql->comit();
				}
			}
		}
	}
	delete instalarJuego;
}

void frmAddEditDosBox::on_btnMount_Add_clicked()
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
		item->setText( 9, DatosMontaje.id_lista    );
		item->setText(10, DatosMontaje.opt_size    );
		item->setText(11, DatosMontaje.opt_freesize);
		item->setText(12, DatosMontaje.freesize    );

		if (Editando)
		{
			DatosMontaje.id_dosbox = IdDbx;
			item->setText(8, sql->insertaUnMontajeDbx(DatosMontaje));
		} else
			item->setText(8, "");

		ui->twMontajes->addTopLevelItem(item);
		sql->comit();
	}
	previerMontajes();
}

void frmAddEditDosBox::on_btnMount_Edit_clicked()
{
	int pos = ui->twMontajes->indexOfTopLevelItem(ui->twMontajes->currentItem());
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if (count_mount > 0 && pos != -1)
	{
		stConfigDOSBoxMount DatosMontaje;
		DatosMontaje.id           = ui->twMontajes->currentItem()->text(8);
		DatosMontaje.id_dosbox    = IdDbx;
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

			if (Editando)
			{
				sql->actualizaMontajeDbx(DatosMontaje);
				sql->comit();
			}
		}
		previerMontajes();
	}
}

void frmAddEditDosBox::on_btnMount_Delete_clicked()
{
	if (ui->twMontajes->topLevelItemCount() > 0)
	{
		if (fGrl->questionMsg(tr("¿Eliminar montaje...?"), tr("¿Deseas eliminar este montaje?")))
		{
			if (Editando)
				sql->eliminarMontajeDbx(ui->twMontajes->currentItem()->text(8));
			fGrl->deleteItemTree(ui->twMontajes->currentItem());
			if (Editando)
			{
				sql->actualizaOrdenTreeWidget(ui->twMontajes, "dbgrl_emu_dosbox_mount", "id", "id_lista", 8, 9);
				sql->comit();
			}
			previerMontajes();
		}
	}
}

void frmAddEditDosBox::on_btnMount_Clear_clicked()
{
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if (count_mount > 0)
	{
		if (Editando)
		{
			for (int num_mount = 0; num_mount < count_mount; ++num_mount)
			{
				QTreeWidgetItem *item = ui->twMontajes->topLevelItem(num_mount);
				sql->eliminarMontajeDbx(item->text(8));
				sql->comit();
			}
		}
		ui->twMontajes->clear();
		previerMontajes();
	}
}

void frmAddEditDosBox::on_btnMount_Subir_clicked()
{
	fGrl->moveUpItemTw(ui->twMontajes);
	if (Editando)
	{
		sql->actualizaOrdenTreeWidget(ui->twMontajes, "dbgrl_emu_dosbox_mount", "id", "id_lista", 8, 9);
		sql->comit();
	}
	previerMontajes();
}

void frmAddEditDosBox::on_btnMount_Bajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMontajes);
	if (Editando)
	{
		sql->actualizaOrdenTreeWidget(ui->twMontajes, "dbgrl_emu_dosbox_mount", "id", "id_lista", 8, 9);
		sql->comit();
	}
	previerMontajes();
}

void frmAddEditDosBox::on_btnMount_AutoCrear_clicked()
{
	QString id_lista = fGrl->intToStr(ui->twMontajes->topLevelItemCount());
	QString path     = ".\\DosGames\\";

	if (!ui->txtDbx_path_exe->text().isEmpty())
		path = fGrl->getInfoFile(fGrl->getDirRelative(ui->txtDbx_path_exe->text(), "DosGames")).Path;

	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setTextAlignment(1, Qt::AlignCenter);
	item->setTextAlignment(2, Qt::AlignCenter);
	item->setIcon(0, QIcon(fGrl->theme() +"img16/drive_hd.png"));
	item->setText(0, path   ); // directorio o iso
	item->setText(1, "C"    ); // letra de montaje
	item->setText(2, "drive"); // tipo de montaje
	item->setText(3, ""     ); // etiqueta
	item->setText(4, ""     ); // index de la unidad de cd-rom
	item->setText(5, ""     ); // opciones del cd-rom
	item->setText(6, ""     ); // cd/dvd IOCtrl
	item->setText(7, "x"    ); // primer montaje

	if (Editando)
	{
		stConfigDOSBoxMount DatosMontaje;
		DatosMontaje.path         = path;
		DatosMontaje.label        = "";
		DatosMontaje.tipo_as      = "drive";
		DatosMontaje.letter       = "C";
		DatosMontaje.indx_cd      = "";
		DatosMontaje.opt_mount    = "";
		DatosMontaje.io_ctrl      = "";
		DatosMontaje.select_mount = "x";
		DatosMontaje.id_lista     = id_lista;
		DatosMontaje.id_dosbox    = IdDbx;
		DatosMontaje.opt_size     = "";
		DatosMontaje.opt_freesize = "false";
		DatosMontaje.freesize     = "250";

		item->setText(8, sql->insertaUnMontajeDbx(DatosMontaje));
		sql->comit();
	} else
		item->setText(8, "");

	item->setText( 9, id_lista);
	item->setText(10, ""      );
	item->setText(11, "false" );
	item->setText(12, "250"   );
	ui->twMontajes->addTopLevelItem(item);

	previerMontajes();
}

void frmAddEditDosBox::on_btnMount_Primario_clicked()
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

		if (Editando)
		{
			sql->actualizaDatosItem("dbgrl_emu_dosbox_mount", IdDbx, "select_mount", "x", "id_dosbox");
			sql->actualizaDatosItem("dbgrl_emu_dosbox_mount", ui->twMontajes->topLevelItem(indx_mount)->text(8), "select_mount", "v", "id");
			sql->comit();
		}

		previerMontajes();
	}
}

void frmAddEditDosBox::on_btnDbx_AddSerial_clicked()
{
	switch (ui->cbxDbx_select_serial->currentIndex())
	{
		case 0: // serial 1
			ui->txtDbx_serial_1->setText(setOpcionesSerial());
		break;
		case 1: // serial 2
			ui->txtDbx_serial_2->setText(setOpcionesSerial());
		break;
		case 2: // serial 3
			ui->txtDbx_serial_3->setText(setOpcionesSerial());
		break;
		case 3: // serial 4
			ui->txtDbx_serial_4->setText(setOpcionesSerial());
		break;
	}
}

void frmAddEditDosBox::on_btnToolDbx_mount_HD_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlDir.Home);
	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
		addMontajeAutoexec("driver", directorio);
}

void frmAddEditDosBox::on_btnToolDbx_mount_CD_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlDir.Home);
	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
		addMontajeAutoexec("cdrom", directorio);
}

void frmAddEditDosBox::on_btnToolDbx_mount_Floppy_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlDir.Home);
	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
		addMontajeAutoexec("floppy", directorio);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgFloppy_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlDir.Home, "", tr("Imagen") +" (*.ima *.img);;"+ tr("Todos los archivos") +" (*)");
	if (!archivo.isEmpty())
		addMontajeAutoexec("IMG_floppy", archivo);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgCD_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlDir.Home, "", tr("Imagen CD") +" (*.iso *.cue);;"+ tr("Todos los archivos") +" (*)");
	if (!archivo.isEmpty())
		addMontajeAutoexec("IMG_iso", archivo);
}

void frmAddEditDosBox::on_btnToolDbx_mount_ImgHD_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlDir.Home, "", tr("Imagen HD") +" (*.ima *.img);;"+ tr("Todos los archivos") +" (*)");
	if (!archivo.isEmpty())
		addMontajeAutoexec("IMG_hdd", archivo);
}

void frmAddEditDosBox::on_btnDbx_mapperfile_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Dbx_sdl_mapperfile, "", "KeyMapper (*.map *.txt);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbx_sdl_mapperfile->setText(fGrl->setDirRelative(archivo));
			grlCfg.Dbx_sdl_mapperfile = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmAddEditDosBox::on_btnDbx_mapperfile_clear_clicked()
{
	ui->txtDbx_sdl_mapperfile->clear();
}

void frmAddEditDosBox::on_btnDbx_language_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Dbx_dosbox_language, "", tr("Idioma") +" (*.lng *.txt);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtDbx_dosbox_language->setText(fGrl->setDirRelative(archivo));
			grlCfg.Dbx_dosbox_language = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmAddEditDosBox::on_btnDbx_language_clear_clicked()
{
	ui->txtDbx_dosbox_language->clear();
}
