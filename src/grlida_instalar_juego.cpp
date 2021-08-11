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
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QDir>

#include "grlida_instalar_juego.h"
#include "grlida_addedit_virtual_drive.h"
#include "ui_instalar_juego.h"

frmInstalarJuego::frmInstalarJuego(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmInstalarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql    = m_sql;
	grlCfg = m_cfg;

	grlDir.Home  = fGrl->dirApp();
	grlDir.Datos = grlDir.Home +"datos/";
	grlDir.Temp  = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

void frmInstalarJuego::closeEvent(QCloseEvent *event)
{
	isCancel = true;

	if (!isCloseEvent && isMountImage)
	{
		isCloseEvent = true;
		emit on_btn_virtual_drive_montar_clicked();

		event->ignore();
	} else
		event->accept();
}

frmInstalarJuego::~frmInstalarJuego()
{
	if (grlProcess->isOpen())
	{
		grlProcess->terminate();
		grlProcess->close();
	}

	if (grlProcessMount->isOpen())
	{
		grlProcessMount->terminate();
		grlProcessMount->close();
	}

	delete grlProcess;
	delete grlProcessMount;

	delete fGrl;
	delete ui;
}

void frmInstalarJuego::cargarConfig()
{
	grlProcess = new QProcess(this);
	connect(grlProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(fin_Proceso(int, QProcess::ExitStatus)));
	connect(grlProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error_Proceso(QProcess::ProcessError)));

	grlProcessMount = new QProcess(this);
	connect(grlProcessMount, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(fin_ProcesoMount(int, QProcess::ExitStatus)));
	connect(grlProcessMount, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error_ProcesoMount(QProcess::ProcessError)));

	isCloseEvent = false;
	isCancel     = false;

	img_floppy.clear();
	img_floppy << ".ima" << ".imz" << ".img";

	img_iso.clear();
	img_iso << ".iso" << ".cue" << ".bin";

	img_hdd.clear();
	img_hdd << "*.vhd" << "*.img";

// DOSBox
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
	}

	ui->cbxMontaje_type_drive->clear();
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Carpeta como disco duro")     , "drive"           );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")            , "cdrom"           );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_floppy.png"), tr("Carpeta como disquete")       , "floppy"          );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png")    , tr("Imagen de disquete")          , "IMG_floppy"      );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png")    , tr("Imagen de disquete multiples"), "IMG_multi_floppy");
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")         , "IMG_iso"         );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")        , "IMG_multi_iso"   );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Imagen de disco duro")        , "IMG_hdd"         );
	ui->cbxMontaje_type_drive->setCurrentIndex(1);
	emit on_cbxMontaje_type_drive_activated(1);

	ui->txtDestinoPath->setText("./DosGames/");
	ui->btnEjecutarDbx->setEnabled(true);

// Montaje ISO, CUE/BIN
	isMountImage = false;
	idx_virtual_drive = 0;
	cargarVirtualDriveList();

	ui->btnOk->setEnabled(false);
}

void frmInstalarJuego::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/applications.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));

	ui->btnDirFile->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDirFile_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSubirIso->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnBajarIso->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDeleteIso->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDestino->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDestinoClear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnEjecutarDbx->setIcon(QIcon(fGrl->theme() +"img16/cat/dosbox.png"));

	ui->btn_virtual_drive_edit->setIcon(QIcon(fGrl->theme() +"img16/editar.png"));
	ui->btn_vd_image_list_add->setIcon(QIcon(fGrl->theme() +"img16/list_add.png"));
	ui->btn_vd_image_list_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btn_vd_image_list_subir->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btn_vd_image_list_bajar->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btn_vd_image_list_delete->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btn_virtual_drive_montar->setIcon(QIcon(fGrl->theme() +"img16/virtual_drive.png"));
}

void frmInstalarJuego::ejecutar(const QString &bin, const QString &parametros, const QString &dirWorking)
{
	//if (!grlProcess->isOpen())
	//{
		QStringList stl_param;
		QFile appBin(bin);
		if (appBin.exists())
		{
			if (!dirWorking.isEmpty())
			{
				if (fGrl->comprobarDirectorio(dirWorking, true))
				{
					QFileInfo fi(dirWorking);
					if (fi.isWritable())
						grlProcess->setWorkingDirectory(dirWorking);
					else
						grlProcess->setWorkingDirectory(grlDir.Temp);
				}
			}

			if (!parametros.isEmpty())
				stl_param << parametros.split("|", QString::SkipEmptyParts);

			grlProcess->start(bin, stl_param);
		} else
			QMessageBox::information(this, windowTitle(), tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
	//} else
	//	QMessageBox::information(this, windowTitle(), tr("Un proceso de ejecucion ya esta en marcha."));
}

void frmInstalarJuego::montaje(QString bin, QString parametros, QString working_dir)
{
	if (QFile::exists(bin))
	{
		if (!working_dir.isEmpty())
		{
			if (fGrl->comprobarDirectorio(working_dir, true))
			{
				QFileInfo fi(working_dir);
				if (fi.isWritable())
					grlProcessMount->setWorkingDirectory(working_dir);
				else
					grlProcessMount->setWorkingDirectory(grlDir.Temp);
			}
		}

		QStringList stl_param;
		if (!parametros.isEmpty())
			stl_param << parametros.split("|", QString::SkipEmptyParts);

		grlProcessMount->start(bin, stl_param);
		grlProcessMount->waitForFinished(1200);
	} else
		QMessageBox::information(this, windowTitle(), tr("No esta disponible el script para ejecutar el montaje de la imagen."));
}

void frmInstalarJuego::cargarVirtualDriveList()
{
	ui->cbx_virtual_drive->clear();
	ui->cbx_virtual_drive->addItem(QIcon(fGrl->theme() +"img16/archivo_config.png"), tr("Seleccionar Unidad Virtual"), "NO_VIRTUAL_DRIVE");

	virtual_drive_list.clear();
	virtual_drive_list = fGrl->cargarListVirtualDrive(grlDir.Datos +"virtual_driver_list.txt");
	QHashIterator<QString, stVirtualDrive> vd(virtual_drive_list);
	while (vd.hasNext())
	{
		vd.next();
		if (vd.value().etiqueta != "NO_VIRTUAL_DRIVE")
			ui->cbx_virtual_drive->addItem(QIcon(fGrl->theme() +"img16/"+ vd.value().icono), vd.value().titulo, vd.value().etiqueta);
	}
	ui->cbx_virtual_drive->setCurrentIndex(idx_virtual_drive);
}

void frmInstalarJuego::setConfigDatosMontaje()
{
	list_mount.clear();
// Montaje C
	stConfigDOSBoxMount mountC;
	mountC.path         = fGrl->setDirRelative(ui->txtDestinoPath->text(), "DosGames");
	mountC.label        = "";
	mountC.tipo_as      = "drive";
	mountC.letter       = "C";
	mountC.indx_cd      = "";
	mountC.opt_mount    = "";
	mountC.io_ctrl      = "";
	mountC.select_mount = "v";
	mountC.id_lista     = "";
	mountC.id_dosbox    = "";
	mountC.opt_size     = "";
	mountC.opt_freesize = "false";
	mountC.freesize     = "250";
	list_mount.append(mountC);

// Montaje
	DatosMontaje.label        = "";
	DatosMontaje.indx_cd      = "";
	DatosMontaje.opt_mount    = "";
	DatosMontaje.io_ctrl      = "";
	DatosMontaje.select_mount = "x";
	DatosMontaje.id_lista     = "";
	DatosMontaje.id_dosbox    = "";
	DatosMontaje.opt_size     = "";
	DatosMontaje.opt_freesize = "false";
	DatosMontaje.freesize     = "250";
	list_mount.append(DatosMontaje);
}

void frmInstalarJuego::on_btnOk_clicked()
{
	isCancel     = false;
	isCloseEvent = true;

	ui->gBoxDbx->setEnabled(false);
	ui->gBoxMount->setEnabled(false);

	if (isMountImage)
		emit on_btn_virtual_drive_montar_clicked();
	else {
		setConfigDatosMontaje();

		QDialog::accept();
	}
}

void frmInstalarJuego::on_btnCancel_clicked()
{
	isCancel     = true;
	isCloseEvent = true;

	ui->gBoxDbx->setEnabled(false);
//	ui->gBoxMount->setEnabled(false);

	if (isMountImage)
		emit on_btn_virtual_drive_montar_clicked();
	else
		QDialog::reject();
}

// DOSBox
void frmInstalarJuego::on_cbxMontaje_type_drive_activated(int index)
{
	if (index > -1)
	{
		QString tipo_montaje = ui->cbxMontaje_type_drive->itemData(index).toString();

		if (tipo_montaje == "IMG_multi_iso" || tipo_montaje == "IMG_multi_floppy" || tipo_montaje == "boot")
		{
			ui->txtMontaje_path->setVisible(false);
			ui->lw_MultiIso->setVisible(true);
			ui->btnSubirIso->setVisible(true);
			ui->btnBajarIso->setVisible(true);
			ui->btnDeleteIso->setVisible(true);
			ui->gridLayout_list_mount->addWidget(ui->txtMontaje_path, 3, 1, 1, 2);
			ui->gridLayout_list_mount->addWidget(ui->lw_MultiIso, 0, 0, 4, 1);
			ui->frame_list_mount->setMaximumHeight(this->maximumHeight());
		} else {
			ui->txtMontaje_path->setVisible(true);
			ui->lw_MultiIso->setVisible(false);
			ui->btnSubirIso->setVisible(false);
			ui->btnBajarIso->setVisible(false);
			ui->btnDeleteIso->setVisible(false);
			ui->gridLayout_list_mount->addWidget(ui->txtMontaje_path, 0, 0, 1, 1);
			ui->gridLayout_list_mount->addWidget(ui->lw_MultiIso, 1, 0, 3, 1);
			ui->frame_list_mount->setMaximumHeight(26);
		}
	}
}

void frmInstalarJuego::on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous != NULL)
	{
		//
	}

	if (current != NULL)
	{
		int listIndex = ui->lw_MultiIso->currentRow();
		if (listIndex == 0)
			ui->btnSubirIso->setEnabled(false);
		else
			ui->btnSubirIso->setEnabled(true);

		if (listIndex == (ui->lw_MultiIso->count() - 1))
			ui->btnBajarIso->setEnabled(false);
		else
			ui->btnBajarIso->setEnabled(true);

		ui->btnDeleteIso->setEnabled(true);
	} else {
		ui->btnDeleteIso->setEnabled(false);
		ui->btnSubirIso->setEnabled(false);
		ui->btnBajarIso->setEnabled(false);
	}
}

void frmInstalarJuego::on_btnDirFile_clicked()
{
	QString tipo_montaje = ui->cbxMontaje_type_drive->itemData(ui->cbxMontaje_type_drive->currentIndex()).toString();

	if (tipo_montaje == "drive" || tipo_montaje == "cdrom" || tipo_montaje == "floppy")
	{
		QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Montaje_path, "DosGames");
		if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
		{
			ui->txtMontaje_path->setText(fGrl->setDirRelative(directorio, "DosGames"));
			grlCfg.Montaje_path = ui->txtMontaje_path->text();
		}
	} else {
		QString tipo_archivo;
		if (tipo_montaje == "IMG_floppy" || tipo_montaje == "IMG_multi_floppy" || tipo_montaje == "boot" || tipo_montaje == "IMG_hdd")
			tipo_archivo = tr("Imagen") +" (*.ima *.img);;";
		else if (tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso")
			tipo_archivo = tr("Imagen CD") +" (*.iso *.cue);;";
		else
			tipo_archivo = "";

		stFileInfo f_info;

		if (tipo_montaje == "IMG_multi_iso" || tipo_montaje == "IMG_multi_floppy" || tipo_montaje == "boot")
		{
			QStringList archivo_lista = fGrl->ventanaAbrirMultiArchivos(this, tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivos") +" (*)", 0);

			if (!archivo_lista.isEmpty())
			{
				foreach (QString archivo, archivo_lista)
				{
					f_info = fGrl->getInfoFile(archivo);
					if (f_info.Exists)
					{
						QListWidgetItem *itemIso = new QListWidgetItem;
						if (tipo_montaje == "boot" || tipo_montaje == "IMG_multi_floppy")
							itemIso->setIcon(QIcon(fGrl->theme() +"img16/floppy_2.png"));
						else
							itemIso->setIcon(QIcon(fGrl->theme() +"img16/cd_iso.png"));
						itemIso->setText(fGrl->setDirRelative(archivo, "DosGames"));
						ui->lw_MultiIso->addItem(itemIso);
					}
				}

				grlCfg.Montaje_path = fGrl->setDirRelative(archivo_lista.at(0), "DosGames");
			}
		} else {
			QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivos") +" (*)", 0, false);

			if (!archivo.isEmpty())
			{
				f_info = fGrl->getInfoFile(archivo);
				if (f_info.Exists)
				{
					ui->txtMontaje_path->setText(fGrl->setDirRelative(archivo, "DosGames"));

					grlCfg.Montaje_path = fGrl->setDirRelative(f_info.Path, "DosGames");
				}
			}
		}
	}
}

void frmInstalarJuego::on_btnDirFile_clear_clicked()
{
	ui->txtMontaje_path->clear();
	ui->lw_MultiIso->clear();
}

void frmInstalarJuego::on_btnSubirIso_clicked()
{
	int index = ui->lw_MultiIso->currentRow();
	if (index > 0)
	{
		QListWidgetItem *item = new QListWidgetItem(*ui->lw_MultiIso->currentItem());
		delete ui->lw_MultiIso->currentItem();
		ui->lw_MultiIso->insertItem(index - 1, item);
		ui->lw_MultiIso->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btnBajarIso_clicked()
{
	int index = ui->lw_MultiIso->currentRow();
	if (index < (ui->lw_MultiIso->count() - 1))
	{
		QListWidgetItem *item = new QListWidgetItem(*ui->lw_MultiIso->currentItem());
		delete ui->lw_MultiIso->currentItem();
		ui->lw_MultiIso->insertItem(index + 1, item);
		ui->lw_MultiIso->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btnDeleteIso_clicked()
{
	QListWidgetItem *item = ui->lw_MultiIso->currentItem();
	if (item != NULL)
	{
		if (fGrl->questionMsg(tr("¿Eliminar?"), tr("¿Quieres eliminar la ISO/IMA/IMG de la lista?")))
		{
			delete item;
		}
	}
}

void frmInstalarJuego::on_btnDestino_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Montaje_path, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
		ui->txtDestinoPath->setText(fGrl->setDirRelative(directorio, "DosGames"));
	else
		ui->txtDestinoPath->setText("");
}

void frmInstalarJuego::on_btnDestinoClear_clicked()
{
	ui->txtDestinoPath->clear();
}

void frmInstalarJuego::on_btnEjecutarDbx_clicked()
{
	QDir dir;
	QString dosbox_emu_key = ui->cbxDbx_EmuKey->itemData(ui->cbxDbx_EmuKey->currentIndex()).toString();
	QString dosbox_path    = fGrl->getDirRelative(dbx_list[dosbox_emu_key].extra);

	if (QFile::exists(dosbox_path) && dir.exists(fGrl->getDirRelative(ui->txtDestinoPath->text(), "DosGames")))
	{
		QString install_info = tr("El ejecutable suele ser: INSTALL, si no esta seguro teclee DIR y pulsa INTRO");

		QStringList listaIsos, listaIsosDirRelative;
		QString tipo_origen = ui->cbxMontaje_type_drive->itemData(ui->cbxMontaje_type_drive->currentIndex()).toString();
		QFile file_out(grlDir.Temp +"dosbox.conf");
		if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QString dir_montaje = QDir::toNativeSeparators(fGrl->getDirRelative(ui->txtMontaje_path->text(), "DosGames"));

			DatosMontaje.path    = dir_montaje;
			DatosMontaje.tipo_as = tipo_origen;

			QTextStream out(&file_out);
//			out.setCodec("UTF-8");
			out << "[autoexec]" << endl;
			out << "@echo off" << endl;

			if (tipo_origen == "drive")
			{
				out << "mount D \"" << dir_montaje << "\"" << endl;
				DatosMontaje.letter = "D";
			} else if (tipo_origen == "cdrom") {
				out << "mount D \"" << dir_montaje << "\" -t cdrom" << endl;
				DatosMontaje.letter = "D";
			} else if (tipo_origen == "floppy") {
				out << "mount A \"" << dir_montaje << "\" -t floppy" << endl;
				DatosMontaje.letter = "A";
			} else if (tipo_origen == "IMG_floppy") {
				out << "imgmount A \"" << dir_montaje << "\" -t floppy" << endl;
				DatosMontaje.letter = "A";
			} else if (tipo_origen == "IMG_iso") {
				out << "imgmount D \"" << dir_montaje << "\" -t cdrom" << endl;
				DatosMontaje.letter = "D";
			} else if (tipo_origen == "IMG_multi_iso" || tipo_origen == "IMG_multi_floppy") {
				listaIsosDirRelative.clear();
				listaIsos.clear();
				for (int i = 0; i < ui->lw_MultiIso->count(); ++i)
				{
					listaIsosDirRelative << ui->lw_MultiIso->item(i)->text();
					listaIsos << "\""+ QDir::toNativeSeparators(fGrl->getDirRelative(ui->lw_MultiIso->item(i)->text())) +"\"";
				}
				if (tipo_origen == "IMG_multi_floppy")
				{
					out << "imgmount A " << listaIsos.join(" ") << " -t floppy" << endl;
					DatosMontaje.letter = "A";
				} else {
					out << "imgmount D " << listaIsos.join(" ") << " -t cdrom" << endl;
					DatosMontaje.letter = "D";
				}

				DatosMontaje.path = ""+ listaIsosDirRelative.join("|");
			} else if (tipo_origen == "IMG_hdd") {
				out << "imgmount D \"" << dir_montaje << "\" -t hdd" << endl;
				DatosMontaje.letter = "D";
			}

			out << "mount C \"" << QDir::toNativeSeparators(fGrl->getDirRelative(ui->txtDestinoPath->text(), "DosGames")) << "\"" << endl;

			out << "echo " << install_info << endl;

			if (tipo_origen == "floppy" || tipo_origen == "IMG_floppy" || tipo_origen == "IMG_multi_floppy")
				out << "A:" << endl;
			else
				out << "D:" << endl;

			out.flush();
			file_out.close();
		}

		ui->gBoxDbx->setEnabled(false);
	//	ui->gBoxMount->setEnabled(false);
		ui->btnOk->setEnabled(true);

		ejecutar(dosbox_path, "-conf|"+ grlDir.Temp +"dosbox.conf", grlDir.Temp);

	} else
		QMessageBox::information(this, windowTitle(), tr("Puede que no este disponible el ejecutable del emulador.\nO el directorio de destino."));
}


// Montaje ISO, CUE/BIN
void frmInstalarJuego::on_lw_vd_images_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous != NULL)
	{
		//
	}

	if (current != NULL)
	{
		int listIndex = ui->lw_vd_images->currentRow();
		if (listIndex == 0)
			ui->btn_vd_image_list_subir->setEnabled(false);
		else
			ui->btn_vd_image_list_subir->setEnabled(true);

		if (listIndex == (ui->lw_vd_images->count() - 1))
			ui->btn_vd_image_list_bajar->setEnabled(false);
		else
			ui->btn_vd_image_list_bajar->setEnabled(true);

		ui->btn_vd_image_list_delete->setEnabled(true);
	} else {
		ui->btn_vd_image_list_delete->setEnabled(false);
		ui->btn_vd_image_list_subir->setEnabled(false);
		ui->btn_vd_image_list_bajar->setEnabled(false);
	}
}

void frmInstalarJuego::on_btn_virtual_drive_edit_clicked()
{
	frmAddEditVirtualDrive *addeditVirtualDrive = new frmAddEditVirtualDrive(sql, grlCfg, this);
	addeditVirtualDrive->setWindowFlags(Qt::Window);
	idx_virtual_drive = ui->cbx_virtual_drive->currentIndex();

	if (addeditVirtualDrive->exec() == QDialog::Accepted)
	{
		grlCfg = addeditVirtualDrive->getGrlCfg();

		cargarVirtualDriveList();
	} else
		grlCfg = addeditVirtualDrive->getGrlCfg();

	delete addeditVirtualDrive;
}

void frmInstalarJuego::on_btn_vd_image_list_add_clicked()
{
	QString tipo_archivo = tr("Imagenes soportadas")+" (*.ima *.img *.iso *.cue);;"+ tr("Imagen") +" (*.ima *.img);;"+ tr("Imagen CD") +" (*.iso *.cue);;";
	QStringList archivo_lista = fGrl->ventanaAbrirMultiArchivos(this, tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivos") +" (*)", 0);

	const int listSize = archivo_lista.size();
	if (listSize > 0)
	{
		stFileInfo f_info;
		for (int i = 0; i < listSize; ++i)
		{
			QString archivo = archivo_lista.at(i);
			f_info = fGrl->getInfoFile(archivo);
			if (f_info.Exists)
			{
				QListWidgetItem *item = new QListWidgetItem;

				QString extension = f_info.Ext.toLower();
				if (img_floppy.contains(extension))
					item->setIcon(QIcon(fGrl->theme() +"img16/floppy_2.png"));
				else if (img_iso.contains(extension))
					item->setIcon(QIcon(fGrl->theme() +"img16/cd_iso.png"));
				else if (img_hdd.contains(extension))
					item->setIcon(QIcon(fGrl->theme() +"img16/drive_hd.png"));
				else
					item->setIcon(QIcon(fGrl->theme() +"img16/ejecutar_app.png"));

				item->setText(fGrl->setDirRelative(archivo, "DosGames"));

				if (i == 0)
					item->setSelected(true);

				ui->lw_vd_images->addItem(item);
			}
		}

		ui->lw_vd_images->setCurrentRow(0);
		grlCfg.Montaje_path = fGrl->setDirRelative(ui->lw_vd_images->currentItem()->text(), "DosGames");
	}
}

void frmInstalarJuego::on_btn_vd_image_list_clear_clicked()
{
	ui->lw_vd_images->clear();
}

void frmInstalarJuego::on_btn_vd_image_list_subir_clicked()
{
	int index = ui->lw_vd_images->currentRow();
	if (index > 0)
	{
		QListWidgetItem *item = new QListWidgetItem(*ui->lw_vd_images->currentItem());
		delete ui->lw_vd_images->currentItem();
		ui->lw_vd_images->insertItem(index - 1, item);
		ui->lw_vd_images->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btn_vd_image_list_bajar_clicked()
{
	int index = ui->lw_vd_images->currentRow();
	if (index < (ui->lw_vd_images->count() - 1))
	{
		QListWidgetItem *item = new QListWidgetItem(*ui->lw_vd_images->currentItem());
		delete ui->lw_vd_images->currentItem();
		ui->lw_vd_images->insertItem(index + 1, item);
		ui->lw_vd_images->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btn_vd_image_list_delete_clicked()
{
	QListWidgetItem *item = ui->lw_vd_images->currentItem();
	if (item != NULL)
	{
		if (fGrl->questionMsg(tr("¿Eliminar?"), tr("¿Quieres eliminar de la lista?")))
		{
			delete item;
		}
	}
}

void frmInstalarJuego::on_btn_virtual_drive_montar_clicked()
{
	QString virtual_drive = ui->cbx_virtual_drive->itemData(ui->cbx_virtual_drive->currentIndex()).toString();
	if (virtual_drive != "NO_VIRTUAL_DRIVE")
	{
		int index = ui->lw_vd_images->currentRow();
		if (index > -1)
		{
			QString vd_path_image = ui->lw_vd_images->currentItem()->text();

			stVirtualDrive virtualDrive = virtual_drive_list[virtual_drive];
			virtualDrive.path_image     = vd_path_image;
/*
			qDebug() << "isMountImage           : " << isMountImage;
			qDebug() << "vd.titulo              : " << virtualDrive.titulo;
			qDebug() << "vd.etiqueta            : " << virtualDrive.etiqueta;
			qDebug() << "vd.icono               : " << virtualDrive.icono;
			qDebug() << "vd.path_exe            : " << virtualDrive.path_exe;
			qDebug() << "vd.path_image          : " << virtualDrive.path_image;
			qDebug() << "vd.letter              : " << virtualDrive.letter;
			qDebug() << "vd.param_mount         : " << virtualDrive.param_mount;
			qDebug() << "vd.param_unmount       : " << virtualDrive.param_unmount;
			qDebug() << "vd.param_extra_1       : " << virtualDrive.param_extra_1;
			qDebug() << "vd.param_extra_2       : " << virtualDrive.param_extra_2;
			qDebug() << "vd.command_line_mount  : " << virtualDrive.command_line_mount;
			qDebug() << "vd.command_line_unmount: " << virtualDrive.command_line_unmount;
			qDebug() << "----------------------------------------------------";
*/
			if (virtualDrive.etiqueta == virtual_drive)
			{
				QString mount_unmount_image = "";

				if (isMountImage)
				{
					isMountImage = false;

					ui->btn_virtual_drive_montar->setText(tr("Montar"));

					mount_unmount_image = fGrl->getCommandLineMount(virtualDrive, false);
				} else {
					isMountImage = true;

					ui->btn_virtual_drive_montar->setText(tr("Desmontar"));

					mount_unmount_image = fGrl->getCommandLineMount(virtualDrive, true);
				}

				ui->cbx_virtual_drive->setEnabled(!isMountImage);
				ui->lw_vd_images->setEnabled(!isMountImage);
				ui->frame_btns_vd->setEnabled(!isMountImage);

				fGrl->guardarArchivo(grlDir.Temp +"montaje.bat", "@echo off\n"+ mount_unmount_image);
				montaje(grlDir.Temp +"montaje.bat", mount_unmount_image, grlDir.Temp);
			} else
				QMessageBox::information(this, windowTitle(), tr("Debes indicar una unidad virtual."));
		} else
			QMessageBox::information(this, windowTitle(), tr("Comprueba que la Imagen exista."));
	} else
		QMessageBox::information(this, windowTitle(), tr("Debes seleccionar una unidad virtual."));
}

// Procesos
void frmInstalarJuego::fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus)
{
	switch (exitStatus)
	{
		case QProcess::NormalExit:
			//
		break;
		case QProcess::CrashExit:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error al salir del proceso") +"\nError: "+ fGrl->intToStr(exitCode), QMessageBox::Ok);
		break;
	}

	grlProcess->terminate();
	grlProcess->close();

	if (isCloseEvent)
	{
		if (isCancel)
			emit on_btnCancel_clicked();
		else
			emit on_btnOk_clicked();
	} else
		ui->gBoxDbx->setEnabled(true);
}

void frmInstalarJuego::fin_ProcesoMount(int exitCode, QProcess::ExitStatus exitStatus)
{
	switch (exitStatus)
	{
		case QProcess::NormalExit:
		//	if (isMountImage)
		//		isMountImage = false;
		//	else
		//		isMountImage = true;
		break;
		case QProcess::CrashExit:
		//	isMountImage = false;
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error al salir del proceso") +"\nError: "+ fGrl->intToStr(exitCode), QMessageBox::Ok);
		break;
	}

	grlProcessMount->terminate();
	grlProcessMount->close();
}

void frmInstalarJuego::error_msg_Proceso(QProcess::ProcessError error)
{
	switch (error)
	{
		case QProcess::FailedToStart:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error al iniciar el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::Crashed:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error en el proceso, tiempo después de empezar con éxito"), QMessageBox::Ok);
		break;
		case QProcess::Timedout:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error, waitFor...() última función el tiempo de espera"), QMessageBox::Ok);
		break;
		case QProcess::WriteError:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error al intentar escribir en el proceso de inicio"), QMessageBox::Ok);
		break;
		case QProcess::ReadError:
			QMessageBox::critical(this, windowTitle(), tr("Se ha producido un error al intentar leer en el proceso"), QMessageBox::Ok);
		break;
		case QProcess::UnknownError:
			QMessageBox::critical(this, windowTitle(), tr("Ha ocurrido un error desconocido"), QMessageBox::Ok);
		break;
	}
}

void frmInstalarJuego::error_Proceso(QProcess::ProcessError error)
{
	error_msg_Proceso(error);

	grlProcess->terminate();
	grlProcess->close();
}

void frmInstalarJuego::error_ProcesoMount(QProcess::ProcessError error)
{
	error_msg_Proceso(error);

//	isMountImage = false;

	grlProcessMount->terminate();
	grlProcessMount->close();
}
