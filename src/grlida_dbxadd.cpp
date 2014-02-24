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

#include "grlida_dbxadd.h"
#include "grlida_addedit_montajes.h"
#include "grlida_importar_juego.h"
#include "grlida_instalar_juego.h"
#include "ui_dbxadd.h"

frmDbxAdd::frmDbxAdd(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmDbxAdd)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql          = m_sql;
	grlCfg       = m_cfg;
	categoria    = m_categoria;
	index_wizard = 0;

	grlDir.Home      = fGrl->GRlidaHomePath();
	grlDir.Confdbx   = grlDir.Home +"confdbx/"+ categoria.tabla +"/";
	grlDir.Datos     = grlDir.Home +"datos/";
	grlDir.Templates = grlDir.Home +"templates/";

	cargarConfig();
	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmDbxAdd::~frmDbxAdd()
{
	delete ui;
}

void frmDbxAdd::cargarConfig()
{
	QRegExp regexp;
	regexp.setPatternSyntax(QRegExp::RegExp);
	regexp.setPattern("[A-Za-z-_0-9]+");

	fGrl->setIdioma(grlCfg.IdiomaSelect);

	DatosJuego  = fGrl->getDefectDatos("dosbox");
	DatosDosBox = fGrl->getDefectDOSBox();

	fGrl->cargarListaArchivosComboBox(ui->cbxDbx_Profiles, grlDir.Templates, "_defecto_", CbxListProf, "*.conf;*.prof", ";");

	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution, grlDir.Datos +"dbx_resolution.txt" , fGrl->Theme() +"img16/", "original"   , 2, "|"); // Resolución pantalla
	fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output        , grlDir.Datos +"dbx_output.txt"     , fGrl->Theme() +"img16/", "surface"    , 3, "|"); // Modo de Renderizado
	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine    , grlDir.Datos +"dbx_machine.txt"    , fGrl->Theme() +"img16/", "svga_s3"    , 3, "|", true, true); // Tarjeta de Video
	fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler     , grlDir.Datos +"dbx_scaler.txt"     , fGrl->Theme() +"img16/", "normal2x"   , 3, "|", true, true); // Escalar y Filtros
	fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_memsize    , grlDir.Datos +"dbx_memsize.txt"    , fGrl->Theme() +"img16/", "16"         , 2, "|"); // Cantidad de memoria para DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_core          , grlDir.Datos +"dbx_cpu_core.txt"   , fGrl->Theme() +"img16/", "auto"       , 3, "|"); // Núcleo de la CPU DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_cpu_cycles        , grlDir.Datos +"dbx_cycles.txt"     , fGrl->Theme() +"img16/", "auto"       , 2, "|"); // Ciclos DOSBox
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_sbtype   , grlDir.Datos +"dbx_sbtype.txt"     , fGrl->Theme() +"img16/", "sb16"       , 3, "|", true, true); // Tipo Sound Blaste
	fGrl->cargarDatosComboBox(ui->cbxDbx_sblaster_oplrate  , grlDir.Datos +"dbx_sb_oplrate.txt" , fGrl->Theme() +"img16/", "44100"      , 2, "|"); // Sample rate of OPL
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_mpu401       , grlDir.Datos +"dbx_mpu401.txt"     , fGrl->Theme() +"img16/", "intelligent", 2, "|"); // MPU-401
	fGrl->cargarDatosComboBox(ui->cbxDbx_midi_device       , grlDir.Datos +"dbx_midi_device.txt", fGrl->Theme() +"img16/", "default"    , 2, "|"); // MIDI Device

	ui->txtDbx_path_conf->setValidator(new QRegExpValidator(regexp, ui->txtDbx_path_conf));
	ui->cbxDbx_sdl_fullresolution->addItem(QIcon(fGrl->Theme() +"img16/sinimg.png"), "0x0", "0x0");

	if( grlCfg.DOSBoxSVN )
	{
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_fullresolution, grlDir.Datos +"dbx_resolution_svn.txt", fGrl->Theme() +"img16/", "", 2, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_sdl_output        , grlDir.Datos +"dbx_output_svn.txt"    , fGrl->Theme() +"img16/", "", 3, "|", false);
		fGrl->cargarDatosComboBox(ui->cbxDbx_dosbox_machine    , grlDir.Datos +"dbx_machine_svn.txt"   , fGrl->Theme() +"img16/", "", 3, "|", false, true);
		fGrl->cargarDatosComboBox(ui->cbxDbx_render_scaler     , grlDir.Datos +"dbx_scaler_svn.txt"    , fGrl->Theme() +"img16/", "", 3, "|", false, true);
	}

	ui->cbxDbx_dos_umb->clear();
	ui->cbxDbx_dos_umb->addItem("true");
	ui->cbxDbx_dos_umb->addItem("false");
	ui->cbxDbx_dos_umb->addItem("max");

	QStringList sonido_frecuencias;
	sonido_frecuencias << "8000" << "11025" << "16000" << "22050" << "32000" << "44100" << "48000" << "49716";

	ui->cbxDbx_mixer_rate->clear();
	ui->cbxDbx_mixer_rate->addItems( sonido_frecuencias );
	ui->cbxDbx_gus_gusrate->clear();
	ui->cbxDbx_gus_gusrate->addItems( sonido_frecuencias );
	ui->cbxDbx_midi_mt32rate->clear();
	ui->cbxDbx_midi_mt32rate->addItems( sonido_frecuencias );
	ui->cbxDbx_speaker_pcrate->clear();
	ui->cbxDbx_speaker_pcrate->addItems( sonido_frecuencias );
	ui->cbxDbx_speaker_tandyrate->clear();
	ui->cbxDbx_speaker_tandyrate->addItems( sonido_frecuencias );

	ui->chkDbx_sdl_fullfixed->setVisible(false); // En desuso

// Configuración del twMontajes
	ui->twMontajes->header()->setMovable(false);
	ui->twMontajes->header()->setStretchLastSection(false);
	ui->twMontajes->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twMontajes->setColumnWidth(1, 40);
	ui->twMontajes->setColumnWidth(2, 80);
	ui->twMontajes->setColumnWidth(3, 80);

	cargarDatosDosBox(DatosDosBox);
}

void frmDbxAdd::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/dosbox.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnPrevious->setIcon( QIcon(fGrl->Theme() +"img16/mp_rebobinar_atras.png") );
	ui->btnNext->setIcon( QIcon(fGrl->Theme() +"img16/mp_rebobinar_adelante.png") );

	ui->btnDbx_FileConfg->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_FileConfg_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_ExeJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_ExeJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_ExeSetup->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_ExeSetup_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDescargarInfo->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnInstalarJuego->setIcon( QIcon(fGrl->Theme() +"img16/ejecutar_app_setup.png") );

	ui->btnMount_Add->setIcon( QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->btnMount_Edit->setIcon( QIcon(fGrl->Theme() +"img16/editar.png") );
	ui->btnMount_Delete->setIcon( QIcon(fGrl->Theme() +"img16/eliminar.png") );
	ui->btnMount_Clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnMount_Subir->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnMount_Bajar->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnMount_AutoCrear->setIcon( QIcon(fGrl->Theme() +"img16/ejecutar_app.png") );
	ui->btnMount_Primario->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
}

void frmDbxAdd::previerMontajes()
{
	if( ui->twMontajes->topLevelItemCount() > 0 )
	{
		stConfigDOSBox datos_montaje;
		datos_montaje.path_exe        = ui->txtDbx_path_exe->text();
		datos_montaje.parametros_exe  = ui->txtDbx_parametros_exe->text();
		datos_montaje.opt_loadfix_mem = DatosDosBox.opt_loadfix_mem;
		datos_montaje.opt_loadfix     = DatosDosBox.opt_loadfix;
		datos_montaje.opt_cerrar_dbox = DatosDosBox.opt_cerrar_dbox;
		QStringList list_mount = fGrl->creaConfigMontajes(ui->twMontajes, datos_montaje);

		ui->previer_mount->clear();
		const int listSize = list_mount.size();
		for(int i= 0; i < listSize; ++i)
			ui->previer_mount->insertPlainText(list_mount.at(i) +"\n");
	} else
		ui->previer_mount->clear();
}

void frmDbxAdd::cargarDatosDosBox(stConfigDOSBox cfgDbx)
{
// [sdl]
	ui->chkDbx_sdl_fullscreen->setChecked( fGrl->StrToBool( cfgDbx.sdl_fullscreen ) );
	ui->chkDbx_sdl_fullfixed->setChecked( fGrl->StrToBool( cfgDbx.sdl_fullfixed ) );
	ui->cbxDbx_sdl_fullresolution->setCurrentIndex( ui->cbxDbx_sdl_fullresolution->findData( cfgDbx.sdl_fullresolution ) );
	ui->cbxDbx_sdl_output->setCurrentIndex( ui->cbxDbx_sdl_output->findData( cfgDbx.sdl_output ) );
	ui->chkDbx_sdl_autolock->setChecked( fGrl->StrToBool( cfgDbx.sdl_autolock ) );
// [dosbox]
	ui->cbxDbx_dosbox_machine->setCurrentIndex( ui->cbxDbx_dosbox_machine->findData( cfgDbx.dosbox_machine ) );
	ui->cbxDbx_dosbox_memsize->setEditText( cfgDbx.dosbox_memsize );
// [render]
	ui->chkDbx_render_aspect->setChecked( fGrl->StrToBool( cfgDbx.render_aspect ) );
	ui->cbxDbx_render_scaler->setCurrentIndex( ui->cbxDbx_render_scaler->findData( cfgDbx.render_scaler ) );
// [cpu]
	ui->cbxDbx_cpu_core->setCurrentIndex( ui->cbxDbx_cpu_core->findData( cfgDbx.cpu_core ) );
	ui->cbxDbx_cpu_cycles->setCurrentIndex( ui->cbxDbx_cpu_cycles->findText( cfgDbx.cpu_cycles ) );
// [mixer]
	ui->chkDbx_mixer_nosound->setChecked( !fGrl->StrToBool( cfgDbx.mixer_nosound ) );
	ui->cbxDbx_mixer_rate->setCurrentIndex( ui->cbxDbx_mixer_rate->findText( cfgDbx.mixer_rate ) );
// [midi]
	ui->cbxDbx_midi_mpu401->setCurrentIndex( ui->cbxDbx_midi_mpu401->findText( cfgDbx.midi_mpu401 ) );
	ui->chkDbx_midi_intelligent->setChecked( fGrl->StrToBool( cfgDbx.midi_intelligent ) );
	ui->cbxDbx_midi_device->setCurrentIndex( ui->cbxDbx_midi_device->findText( cfgDbx.midi_device ) );
	ui->cbxDbx_midi_mt32rate->setCurrentIndex( ui->cbxDbx_midi_mt32rate->findText( cfgDbx.midi_mt32rate ) );
// [sblaster]
	ui->cbxDbx_sblaster_sbtype->setCurrentIndex( ui->cbxDbx_sblaster_sbtype->findData( cfgDbx.sblaster_sbtype ) );
	ui->cbxDbx_sblaster_oplrate->setCurrentIndex( ui->cbxDbx_sblaster_oplrate->findText( cfgDbx.sblaster_oplrate ) );
// [gus]
	ui->chkDbx_gus_gus->setChecked( fGrl->StrToBool( cfgDbx.gus_gus ) );
	ui->cbxDbx_gus_gusrate->setCurrentIndex( ui->cbxDbx_gus_gusrate->findText( cfgDbx.gus_gusrate ) );
// [speaker]
	ui->chkDbx_speaker_pcspeaker->setChecked( fGrl->StrToBool( cfgDbx.speaker_pcspeaker ) );
	ui->cbxDbx_speaker_pcrate->setCurrentIndex( ui->cbxDbx_speaker_pcrate->findText( cfgDbx.speaker_pcrate ) );
	ui->cbxDbx_speaker_tandyrate->setCurrentIndex( ui->cbxDbx_speaker_tandyrate->findText( cfgDbx.speaker_tandyrate ) );
	ui->chkDbx_speaker_disney->setChecked( fGrl->StrToBool( cfgDbx.speaker_disney ) );
// [dos]
	ui->chkDbx_dos_xms->setChecked( fGrl->StrToBool( cfgDbx.dos_xms ) );
	ui->chkDbx_dos_ems->setChecked( fGrl->StrToBool( cfgDbx.dos_ems ) );
	ui->cbxDbx_dos_umb->setCurrentIndex( ui->cbxDbx_dos_umb->findText( cfgDbx.dos_umb ) );
// Opciones
	ui->chkDbx_loadfix->setChecked( fGrl->StrToBool( cfgDbx.opt_loadfix ) );
	ui->chkDbx_consola_dbox->setChecked( fGrl->StrToBool( cfgDbx.opt_consola_dbox ) );
	ui->chkDbx_cerrar_dbox->setChecked( fGrl->StrToBool( cfgDbx.opt_cerrar_dbox ) );
// path_exe
	if( !cfgDbx.path_exe.isEmpty() )
		ui->txtDbx_path_exe->setText( cfgDbx.path_exe );
// path_setup
	if( !cfgDbx.path_setup.isEmpty() )
		ui->txtDbx_path_setup->setText( cfgDbx.path_setup );
// parametros_exe
	ui->txtDbx_parametros_exe->setText( cfgDbx.parametros_exe );
// parametros_setup
	ui->txtDbx_parametros_setup->setText( cfgDbx.parametros_setup );
}

void frmDbxAdd::cargarDatosDBxMontaje(QTreeWidget *twMontajesDbx)
{
	const int count_mount = twMontajesDbx->topLevelItemCount();
	if( count_mount > 0 )
	{
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QTreeWidgetItem *itemDbx = twMontajesDbx->topLevelItem( num_mount );

			QTreeWidgetItem *item = new QTreeWidgetItem( ui->twMontajes );
			item->setTextAlignment(1, Qt::AlignCenter);
			item->setTextAlignment(2, Qt::AlignCenter);
			item->setIcon( 0, itemDbx->icon(0) );
			item->setText( 0, itemDbx->text(0) );	// Directorio o iso
			item->setText( 1, itemDbx->text(1) );	// Letra de montaje
			item->setText( 2, itemDbx->text(2) );	// Tipo de montaje
			item->setText( 3, itemDbx->text(3) );	// Etiqueta
			item->setText( 4, itemDbx->text(4) );	// Index de la unidad de cd-rom
			item->setText( 5, itemDbx->text(5) );	// Opciones del cd-rom
			item->setText( 6, itemDbx->text(6) );	// Cd/dvd
			item->setText( 7, itemDbx->text(7) );	// Primer montaje
			item->setText( 8, itemDbx->text(8) );	// Id
			item->setText( 9, itemDbx->text(9) );	// Id_lista
			item->setText(10, itemDbx->text(10));
			item->setText(11, itemDbx->text(11));
			item->setText(12, itemDbx->text(12));
		}
	}
	previerMontajes();
}

void frmDbxAdd::setDatosDosBox()
{
	QString str;
// [sdl]
	DatosDosBox.sdl_fullscreen     = fGrl->BoolToStr( ui->chkDbx_sdl_fullscreen->isChecked() );
	DatosDosBox.sdl_fullfixed      = fGrl->BoolToStr( ui->chkDbx_sdl_fullfixed->isChecked() );
	str = ui->cbxDbx_sdl_fullresolution->itemData( ui->cbxDbx_sdl_fullresolution->currentIndex() ).toString();
	DatosDosBox.sdl_fullresolution = str.isEmpty() ? "original" : str;
	str = ui->cbxDbx_sdl_output->itemData( ui->cbxDbx_sdl_output->currentIndex() ).toString();
	DatosDosBox.sdl_output         = str.isEmpty() ? "surface" : str;
	DatosDosBox.sdl_autolock       = fGrl->BoolToStr( ui->chkDbx_sdl_autolock->isChecked() );
// [dosbox]
	str = ui->cbxDbx_dosbox_machine->itemData( ui->cbxDbx_dosbox_machine->currentIndex() ).toString();
	DatosDosBox.dosbox_machine = str.isEmpty() ? "svga_s3" : str;
	DatosDosBox.dosbox_memsize = ui->cbxDbx_dosbox_memsize->currentText().isEmpty() ? "16" : ui->cbxDbx_dosbox_memsize->currentText();
// [render]
	DatosDosBox.render_aspect = fGrl->BoolToStr( ui->chkDbx_render_aspect->isChecked() );
	str = ui->cbxDbx_render_scaler->itemData( ui->cbxDbx_render_scaler->currentIndex() ).toString();
	DatosDosBox.render_scaler = str.isEmpty() ? "normal2x" : str;
// [cpu]
	str = ui->cbxDbx_cpu_core->itemData( ui->cbxDbx_cpu_core->currentIndex() ).toString();
	DatosDosBox.cpu_core   = str.isEmpty() ? "auto" : str;
	DatosDosBox.cpu_cycles = ui->cbxDbx_cpu_cycles->currentText().isEmpty() ? "auto" : ui->cbxDbx_cpu_cycles->currentText();
// [mixer]
	DatosDosBox.mixer_nosound = fGrl->BoolToStr( !ui->chkDbx_mixer_nosound->isChecked() );
	DatosDosBox.mixer_rate    = ui->cbxDbx_mixer_rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_mixer_rate->currentText();
// [midi]
	DatosDosBox.midi_mpu401      = ui->cbxDbx_midi_mpu401->currentText().isEmpty() ? "intelligent" : ui->cbxDbx_midi_mpu401->currentText();
	DatosDosBox.midi_intelligent = fGrl->BoolToStr( ui->chkDbx_midi_intelligent->isChecked() );
	DatosDosBox.midi_device      = ui->cbxDbx_midi_device->currentText().isEmpty() ? "default" : ui->cbxDbx_midi_device->currentText();
	DatosDosBox.midi_mt32rate    = ui->cbxDbx_midi_mt32rate->currentText().isEmpty() ? "44100" : ui->cbxDbx_midi_mt32rate->currentText();
// [sblaster]
	str = ui->cbxDbx_sblaster_sbtype->itemData( ui->cbxDbx_sblaster_sbtype->currentIndex() ).toString();
	DatosDosBox.sblaster_sbtype  = str.isEmpty() ? "sb16" : str;
// [gus]
	DatosDosBox.gus_gus     = fGrl->BoolToStr( ui->chkDbx_gus_gus->isChecked() );
	DatosDosBox.gus_gusrate = ui->cbxDbx_gus_gusrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_gus_gusrate->currentText();
// [speaker]
	DatosDosBox.speaker_pcspeaker = fGrl->BoolToStr( ui->chkDbx_speaker_pcspeaker->isChecked() );
	DatosDosBox.speaker_pcrate    = ui->cbxDbx_speaker_pcrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_pcrate->currentText();
	DatosDosBox.speaker_tandyrate = ui->cbxDbx_speaker_tandyrate->currentText().isEmpty() ? "44100" : ui->cbxDbx_speaker_tandyrate->currentText();
	DatosDosBox.speaker_disney    = fGrl->BoolToStr( ui->chkDbx_speaker_disney->isChecked() );
// [joystick]
// [modem]
// [dserial]
// [serial]
// [dos]
	DatosDosBox.dos_xms = fGrl->BoolToStr( ui->chkDbx_dos_xms->isChecked() );
	DatosDosBox.dos_ems = fGrl->BoolToStr( ui->chkDbx_dos_ems->isChecked() );
	DatosDosBox.dos_umb = ui->cbxDbx_dos_umb->currentText().isEmpty() ? "true" : ui->cbxDbx_dos_umb->currentText();
// [ipx]
// [autoexec]
// Opciones
	DatosDosBox.opt_loadfix      = fGrl->BoolToStr( ui->chkDbx_loadfix->isChecked() );
	DatosDosBox.opt_consola_dbox = fGrl->BoolToStr( ui->chkDbx_consola_dbox->isChecked() );
	DatosDosBox.opt_cerrar_dbox  = fGrl->BoolToStr( ui->chkDbx_cerrar_dbox->isChecked() );
// Path
	DatosDosBox.path_conf        = ui->txtDbx_path_conf->text().isEmpty() ? "" : ui->txtDbx_path_conf->text();
	DatosDosBox.path_exe         = ui->txtDbx_path_exe->text().isEmpty() ? "" : ui->txtDbx_path_exe->text();
	DatosDosBox.path_setup       = ui->txtDbx_path_setup->text().isEmpty() ? "" : ui->txtDbx_path_setup->text();
	DatosDosBox.parametros_exe   = ui->txtDbx_parametros_exe->text().isEmpty() ? "" : ui->txtDbx_parametros_exe->text();
	DatosDosBox.parametros_setup = ui->txtDbx_parametros_setup->text().isEmpty() ? "" : ui->txtDbx_parametros_setup->text();
}

void frmDbxAdd::on_btnOk_clicked()
{
	if( ui->txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el título."));
	else {
		setDatosDosBox();

		DatosJuego.titulo   = ui->txtDatos_Titulo->text();
		DatosJuego.tipo_emu = "dosbox";
		DatosJuego.fecha    = fGrl->getTime();
		DatosJuego.idgrl    = sql->insertaDatos(categoria.tabla, DatosJuego);

		DatosDosBox.idgrl = DatosJuego.idgrl;
		DatosDosBox.idcat = categoria.id;
		DatosDosBox.id    = sql->insertaDbx(DatosDosBox);

		QList<QString> url_list = sql->getDatosUrls(DatosJuego.idgrl, categoria.id);
		sql->insertaMontajesDbx(ui->twMontajes, DatosDosBox.id);
		fGrl->crearArchivoConfigDbx(DatosJuego, url_list, DatosDosBox, ui->twMontajes, grlDir.Home, categoria.tabla, grlDir.Confdbx + DatosDosBox.path_conf);

		QDialog::accept();
	}
}

void frmDbxAdd::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmDbxAdd::on_btnNext_clicked()
{
	bool siguiente = false;
	if( !ui->txtDatos_Titulo->text().isEmpty() )
	{
		if( !ui->txtDbx_path_conf->text().isEmpty() )
		{
			stFileInfo f_info = fGrl->getInfoFile( grlDir.Confdbx + ui->txtDbx_path_conf->text() );
			if( f_info.Exists )
				ui->txtDbx_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".conf");
			else
				ui->txtDbx_path_conf->setText(f_info.Name +".conf");
			siguiente = true;
			if( ui->txtDbx_path_exe->text().isEmpty() )
			{
				QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego"));
				siguiente = false;
				ui->txtDbx_path_exe->setFocus();
			} else
				siguiente = true;
		} else {
			QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el archivo donde guardara la configuración del juego"));
			siguiente = false;
			ui->txtDbx_path_conf->setFocus();
		}
	} else {
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner un título al juego"));
		siguiente = false;
		ui->txtDatos_Titulo->setFocus();
	}

	if( siguiente )
	{
		ui->btnPrevious->setEnabled(true);
		ui->wizardDbx->setCurrentIndex(ui->wizardDbx->currentIndex()+1);
	}
	if( ui->wizardDbx->currentIndex() == 1 ) ui->chkDbx_sdl_fullscreen->setFocus();
	if( ui->wizardDbx->currentIndex() == 2 ) ui->cbxDbx_dosbox_memsize->setFocus();
	if( ui->wizardDbx->currentIndex() == 3 ) ui->chkDbx_mixer_nosound->setFocus();
	if( ui->wizardDbx->currentIndex() == 4 )
	{
//		if( isImportDbx == false )
//			emit on_btnMount_AutoCrear();
		ui->twMontajes->setFocus();
	}

	if( ui->wizardDbx->currentIndex() >= 5 )
	{
		ui->btnOk->setEnabled( true );
		ui->btnOk->setFocus();
		ui->btnNext->setEnabled( false );
	}
}

void frmDbxAdd::on_btnPrevious_clicked()
{
	ui->wizardDbx->setCurrentIndex(ui->wizardDbx->currentIndex()-1);
	ui->btnNext->setEnabled( true );
	if(ui->wizardDbx->currentIndex() < 1 )
		ui->btnPrevious->setEnabled(false);
}

void frmDbxAdd::on_btnDbx_FileConfg_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Guardar archivo como..."), grlDir.Confdbx, "", tr("Configuraciones") +" DOSBox (*.conf);;"+ tr("Todos los archivo") +" (*)", 0, true);

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
			ui->txtDbx_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".conf");
		else
			ui->txtDbx_path_conf->setText(f_info.Name +".conf");
	}
}

void frmDbxAdd::on_btnDbx_FileConfg_clear_clicked()
{
	ui->txtDbx_path_conf->clear();
}

void frmDbxAdd::on_btnDbx_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_path_exe, "DosGames", tr("Ejecutables") +" (*.exe; *.bat; *.com);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtDbx_path_exe->setText( fGrl->setDirRelative(archivo, "DosGames") );
			grlCfg.Dbx_path_exe = fGrl->setDirRelative(f_info.Path, "DosGames");

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path_exe", grlCfg.Dbx_path_exe);
		}
	}
}

void frmDbxAdd::on_btnDbx_ExeJuego_clear_clicked()
{
	ui->txtDbx_path_exe->clear();
}

void frmDbxAdd::on_btnDbx_ExeSetup_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_path_setup, "DosGames", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtDbx_path_setup->setText( fGrl->setDirRelative(archivo, "DosGames") );
			grlCfg.Dbx_path_setup = fGrl->setDirRelative(f_info.Path, "DosGames");

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path_setup", grlCfg.Dbx_path_setup);
		}
	}
}

void frmDbxAdd::on_btnDbx_ExeSetup_clear_clicked()
{
	ui->txtDbx_path_setup->clear();
}

void frmDbxAdd::on_btnDescargarInfo_clicked()
{
	stConfigScummVM  DatosScummVM  = fGrl->getDefectScummVM();
	stConfigVDMSound DatosVDMSound = fGrl->getDefectVDMSound();

	DatosJuego.titulo = ui->txtDatos_Titulo->text();

	frmImportarJuego *ImportarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, this);
	ImportarJuego->setWindowFlags(Qt::Window);

	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		DatosJuego = ImportarJuego->getDatosJuegos();
		ui->txtDatos_Titulo->setText( DatosJuego.titulo );

		if( DatosJuego.tipo_emu == "dosbox" && !ImportarJuego->isSoloDatos )
		{
			if( !ImportarJuego->getDatosDosBox().path_conf.isEmpty() )
				ui->txtDbx_path_conf->setText( ImportarJuego->getDatosDosBox().path_conf );

			cargarDatosDosBox(ImportarJuego->getDatosDosBox());
			cargarDatosDBxMontaje(ImportarJuego->ui->twMontajes);
		}
	}

	delete ImportarJuego;
}

void frmDbxAdd::on_btnInstalarJuego_clicked()
{
	frmInstalarJuego *NewInstalarJuego = new frmInstalarJuego(grlCfg, this);
	NewInstalarJuego->exec();
	delete NewInstalarJuego;
}

void frmDbxAdd::on_btnMount_Add_clicked()
{
	frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes(grlCfg, false, this);
	if( AddEditMontajes->exec() == QDialog::Accepted )
	{
		grlCfg.Montaje_path = AddEditMontajes->getGrlCfg().Montaje_path;
		stConfigDOSBoxMount DatosMontaje = AddEditMontajes->getDatosMontaje();

		QTreeWidgetItem *item = new QTreeWidgetItem( ui->twMontajes );
		DatosMontaje.id_lista = fGrl->IntToStr( item->childCount() );
		DatosMontaje.select_mount = "x";

		item->setTextAlignment(1, Qt::AlignCenter);
		item->setTextAlignment(2, Qt::AlignCenter);
		item->setIcon( 0, QIcon( fGrl->getIconMount(fGrl->Theme(), DatosMontaje.tipo_as) ) );
		item->setText( 0, DatosMontaje.path         );	// directorio o iso
		item->setText( 1, DatosMontaje.letter       );	// letra de montaje
		item->setText( 2, DatosMontaje.tipo_as      );	// tipo de montaje
		item->setText( 3, DatosMontaje.label        );	// etiqueta
		item->setText( 4, DatosMontaje.indx_cd      );	// index de la unidad de cd-rom
		item->setText( 5, DatosMontaje.opt_mount    );	// opciones del cd-rom
		item->setText( 6, DatosMontaje.io_ctrl      );	// cd/dvd
		item->setText( 7, DatosMontaje.select_mount );	// primer montaje
		item->setText( 8, ""                        );
		item->setText( 9, DatosMontaje.id_lista     );
		item->setText(10, DatosMontaje.opt_size     );
		item->setText(11, DatosMontaje.opt_freesize );
		item->setText(12, DatosMontaje.freesize     );
	}
	previerMontajes();
}

void frmDbxAdd::on_btnMount_Edit_clicked()
{
	int pos = ui->twMontajes->indexOfTopLevelItem( ui->twMontajes->currentItem() );
	if( ui->twMontajes->topLevelItemCount() > 0 && pos != -1 )
	{
		stConfigDOSBoxMount DatosMontaje;
		DatosMontaje.path         = ui->twMontajes->currentItem()->text(0);	// directorio o iso
		DatosMontaje.letter       = ui->twMontajes->currentItem()->text(1);	// letra de montaje
		DatosMontaje.tipo_as      = ui->twMontajes->currentItem()->text(2);	// tipo de montaje
		DatosMontaje.label        = ui->twMontajes->currentItem()->text(3);	// etiqueta
		DatosMontaje.indx_cd      = ui->twMontajes->currentItem()->text(4);	// index de la unidad de cd-rom
		DatosMontaje.opt_mount    = ui->twMontajes->currentItem()->text(5);	// opciones del cd-rom
		DatosMontaje.io_ctrl      = ui->twMontajes->currentItem()->text(6);	// cd/dvd
		DatosMontaje.select_mount = ui->twMontajes->currentItem()->text(7);	// primer montaje
		DatosMontaje.id_lista     = fGrl->IntToStr( ui->twMontajes->currentItem()->childCount() );
		DatosMontaje.opt_size     = ui->twMontajes->currentItem()->text(10);
		DatosMontaje.opt_freesize = ui->twMontajes->currentItem()->text(11);
		DatosMontaje.freesize     = ui->twMontajes->currentItem()->text(12);

		frmAddEditMontajes *AddEditMontajes = new frmAddEditMontajes(grlCfg, true, this);
		AddEditMontajes->setDatosMontaje( DatosMontaje );
		if( AddEditMontajes->exec() == QDialog::Accepted )
		{
			grlCfg.Montaje_path = AddEditMontajes->getGrlCfg().Montaje_path;
			DatosMontaje = AddEditMontajes->getDatosMontaje();
			QString select_mount = ui->twMontajes->currentItem()->text(7) == "v" ? "s_" : "";

			ui->twMontajes->currentItem()->setIcon( 0, QIcon( fGrl->getIconMount(fGrl->Theme(), DatosMontaje.tipo_as, select_mount) ) );
			ui->twMontajes->currentItem()->setText( 0, DatosMontaje.path         );	// directorio o iso
			ui->twMontajes->currentItem()->setText( 1, DatosMontaje.letter       );	// letra de montaje
			ui->twMontajes->currentItem()->setText( 2, DatosMontaje.tipo_as      );	// tipo de montaje
			ui->twMontajes->currentItem()->setText( 3, DatosMontaje.label        );	// etiqueta
			ui->twMontajes->currentItem()->setText( 4, DatosMontaje.indx_cd      );	// index de la unidad de cd-rom
			ui->twMontajes->currentItem()->setText( 5, DatosMontaje.opt_mount    );	// opciones del cd-rom
			ui->twMontajes->currentItem()->setText( 6, DatosMontaje.io_ctrl      );	// cd/dvd
			ui->twMontajes->currentItem()->setText( 7, DatosMontaje.select_mount );	// primer montaje
			ui->twMontajes->currentItem()->setText( 9, DatosMontaje.id_lista     );
			ui->twMontajes->currentItem()->setText(10, DatosMontaje.opt_size     );
			ui->twMontajes->currentItem()->setText(11, DatosMontaje.opt_freesize );
			ui->twMontajes->currentItem()->setText(12, DatosMontaje.freesize     );
		}
		previerMontajes();
	}
}

void frmDbxAdd::on_btnMount_Delete_clicked()
{
	if( ui->twMontajes->topLevelItemCount() > 0 )
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar montaje...?"), tr("¿Deseas eliminar este montaje?"), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
		{
			fGrl->deleteItemTree( ui->twMontajes->currentItem() );
			previerMontajes();
		}
	}
}

void frmDbxAdd::on_btnMount_Clear_clicked()
{
	const int count_mount = ui->twMontajes->topLevelItemCount();
	if( count_mount > 0 )
	{
		ui->twMontajes->clear();
		previerMontajes();
	}
}

void frmDbxAdd::on_btnMount_Subir_clicked()
{
	fGrl->moveUpItemTw(ui->twMontajes);
	previerMontajes();
}

void frmDbxAdd::on_btnMount_Bajar_clicked()
{
	fGrl->moveDownItemTw(ui->twMontajes);
	previerMontajes();
}

void frmDbxAdd::on_btnMount_AutoCrear_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile( fGrl->getDirRelative(ui->txtDbx_path_exe->text(), "DosGames") );
	QTreeWidgetItem *item = new QTreeWidgetItem( ui->twMontajes );
	item->setIcon( 0, QIcon(fGrl->Theme() +"img16/drive_hd.png") );
	item->setText( 0, f_info.Path );	// directorio o iso
	item->setText( 1, "C"         );	// letra de montaje
	item->setText( 2, "drive"     );	// tipo de montaje
	item->setText( 3, ""          );	// etiqueta
	item->setText( 4, ""          );	// index de la unidad de cd-rom
	item->setText( 5, ""          );	// opciones del cd-rom
	item->setText( 6, ""          );	// cd/dvd IOCtrl
	item->setText( 7, "x"         );	// primer montaje
	item->setText( 8, "" );
	item->setText( 9, fGrl->IntToStr( item->childCount() ) );
	item->setText(10, ""          );
	item->setText(11, "false"     );
	item->setText(12, "250"       );

	previerMontajes();
}

void frmDbxAdd::on_btnMount_Primario_clicked()
{
	int indx_mount = 0;
	QString tipoDrive, isPrimario;

	int pos = ui->twMontajes->indexOfTopLevelItem( ui->twMontajes->currentItem() );
	if( ui->twMontajes->topLevelItemCount() > 0 && pos != -1 )
	{
		indx_mount = ui->twMontajes->indexOfTopLevelItem( ui->twMontajes->currentItem() );
		isPrimario = ui->twMontajes->topLevelItem( indx_mount )->text(7);

		const int count_mount = ui->twMontajes->topLevelItemCount();
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			tipoDrive = ui->twMontajes->topLevelItem( num_mount )->text(2);

			ui->twMontajes->topLevelItem( num_mount )->setIcon(0, QIcon( fGrl->getIconMount(fGrl->Theme(), tipoDrive) ) );
			ui->twMontajes->topLevelItem( num_mount )->setText(7, "x");
		}

		tipoDrive = ui->twMontajes->topLevelItem( indx_mount )->text(2);

		if( isPrimario == "v" )
		{
			ui->twMontajes->topLevelItem( indx_mount )->setIcon( 0, QIcon( fGrl->getIconMount(fGrl->Theme(), tipoDrive) ) );
			ui->twMontajes->topLevelItem( indx_mount )->setText( 7, "x");
		} else {
			ui->twMontajes->topLevelItem( indx_mount )->setIcon( 0, QIcon( fGrl->getIconMount(fGrl->Theme(), tipoDrive, "s_") ) );
			ui->twMontajes->topLevelItem( indx_mount )->setText( 7, "v");
		}

		previerMontajes();
	}
}

void frmDbxAdd::on_cbxDbx_Profiles_activated(int index)
{
	if( index > -1 )
	{
		QString profileGame = ui->cbxDbx_Profiles->itemData( index ).toString();
		if( profileGame != "_defecto_" )
		{
			QHash<QString, QString> tmp_cfgDbx = fGrl->importarProfileDFend(grlDir.Home, categoria.tabla, grlDir.Templates + profileGame);
			tmp_cfgDbx["Dbx_id"]    = DatosDosBox.id;
			tmp_cfgDbx["Dbx_idgrl"] = DatosDosBox.idgrl;
			tmp_cfgDbx["Dbx_idcat"] = DatosDosBox.idcat;
			cargarDatosDosBox( fGrl->getDefectDOSBox( tmp_cfgDbx ) );
		} else {
			DatosDosBox = fGrl->getDefectDOSBox();
			cargarDatosDosBox(DatosDosBox);
		}
	}
}

void frmDbxAdd::on_txtDatos_Titulo_textEdited(const QString &arg1)
{
	if( !arg1.isEmpty() )
	{
		QString str = fGrl->eliminar_caracteres( arg1 );
		bool str_ok = str.endsWith(".conf");
		if(str_ok == false)
			str.append(".conf");

		ui->txtDbx_path_conf->setText( str );
	} else
		ui->txtDbx_path_conf->setText("");
}
