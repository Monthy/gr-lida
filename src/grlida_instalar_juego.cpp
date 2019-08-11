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
#include "ui_instalar_juego.h"

frmInstalarJuego::frmInstalarJuego(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmInstalarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home  = fGrl->dirApp();
	grlDir.Datos = grlDir.Home +"datos/";
	grlDir.Temp  = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmInstalarJuego::~frmInstalarJuego()
{
	delete fGrl;
	delete ui;
}

void frmInstalarJuego::cargarConfig()
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
	}

	ui->cbxMontaje_type_drive->clear();
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Carpeta como disco duro"), "drive"        );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")       , "cdrom"        );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_floppy.png"), tr("Carpeta como disquete")  , "floppy"       );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png")    , tr("Imagen de disquete")     , "IMG_floppy"   );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")    , "IMG_iso"      );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")   , "IMG_multi_iso");
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Imagen de disco duro")   , "IMG_hdd"      );
	ui->cbxMontaje_type_drive->setCurrentIndex(1);
	emit on_cbxMontaje_type_drive_activated(1);

	ui->btnOk->setEnabled(false);
	ui->txtDestinoPath->setText("./DosGames/");
}

void frmInstalarJuego::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/applications.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnEjecutar->setIcon(QIcon(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));

	ui->btnDirFile->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDirFile_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSubirIso->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnBajarIso->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDeleteIso->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnDestino->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDestinoClear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
}

void frmInstalarJuego::ejecutar(const QString &bin, const QString &parametros, const QString &dirWorking)
{
	QStringList stl_param;
	QProcess *grlProcess = new QProcess(this);
	QFile appBin(bin);
	if (appBin.exists())
	{
		if (!dirWorking.isEmpty())
			grlProcess->setWorkingDirectory(dirWorking);

		if (!parametros.isEmpty())
		{
			stl_param << parametros.split("|", QString::SkipEmptyParts);
			grlProcess->start(bin , stl_param);
			stl_param.clear();
		} else
			grlProcess->start(bin);
	} else
		QMessageBox::information(this, "", tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void frmInstalarJuego::on_cbxMontaje_type_drive_activated(int index)
{
	if (index > -1)
	{
		QString tipo_montaje = ui->cbxMontaje_type_drive->itemData(index).toString();
		if (tipo_montaje == "IMG_multi_iso")
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
		if (tipo_montaje == "IMG_floppy" || tipo_montaje == "boot" || tipo_montaje == "IMG_hdd")
			tipo_archivo = tr("Imagen") +" (*.ima *.img);;";
		else if (tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso")
			tipo_archivo = tr("Imagen CD") +" (*.iso *.cue);;";
		else
			tipo_archivo = "";

		QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivos") +" (*)", 0, false);

		if (!archivo.isEmpty())
		{
			stFileInfo f_info = fGrl->getInfoFile(archivo);
			if (f_info.Exists)
			{
				if (tipo_montaje == "IMG_multi_iso" || tipo_montaje == "boot")
				{
					QListWidgetItem *itemIso = new QListWidgetItem;
					if (tipo_montaje == "boot")
						itemIso->setIcon(QIcon(fGrl->theme() +"img16/floppy_2.png"));
					else
						itemIso->setIcon(QIcon(fGrl->theme() +"img16/cd_iso.png"));
					itemIso->setText(fGrl->setDirRelative(archivo, "DosGames"));
					ui->lw_MultiIso->addItem(itemIso);
				} else
					ui->txtMontaje_path->setText(fGrl->setDirRelative(archivo, "DosGames"));

				grlCfg.Montaje_path = fGrl->setDirRelative(f_info.Path, "DosGames");
			}
		}
	}
}

void frmInstalarJuego::on_btnClearDirFile_clicked()
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
		if (fGrl->questionMsg(tr("¿Eliminar...?"), tr("¿Quieres eliminar la ISO de la lista?")))
		{
			delete item;
		}
	}
}

void frmInstalarJuego::on_btnDestino_clicked()
{
	QString directorio = fGrl->ventanaDirectorios(this, tr("Seleccionar un directorio"), grlCfg.Montaje_path, "DosGames");

	if (!directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true))
	{
		ui->btnOk->setEnabled(true);
		ui->txtDestinoPath->setText(fGrl->setDirRelative(directorio, "DosGames"));
	} else {
		ui->btnOk->setEnabled(false);
		ui->txtDestinoPath->setText("");
	}
}

void frmInstalarJuego::on_btnDestinoClear_clicked()
{
	ui->txtDestinoPath->clear();
}

void frmInstalarJuego::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmInstalarJuego::on_btnEjecutar_clicked()
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
			} else if (tipo_origen == "IMG_multi_iso") {
				listaIsosDirRelative.clear();
				listaIsos.clear();
				for (int i = 0; i < ui->lw_MultiIso->count(); ++i)
				{
					listaIsosDirRelative << ui->lw_MultiIso->item(i)->text();
					listaIsos << "\""+ QDir::toNativeSeparators(fGrl->getDirRelative(ui->lw_MultiIso->item(i)->text())) +"\"";
				}
				out << "imgmount D " << listaIsos.join(" ") << " -t cdrom" << endl;
				DatosMontaje.path   = ""+ listaIsosDirRelative.join("|");
				DatosMontaje.letter = "D";
			} else if (tipo_origen == "IMG_hdd") {
				out << "imgmount D \"" << dir_montaje << "\" -t hdd" << endl;
				DatosMontaje.letter = "D";
			}

			out << "mount C \"" << QDir::toNativeSeparators(fGrl->getDirRelative(ui->txtDestinoPath->text(), "DosGames")) << "\"" << endl;

			out << "echo " << install_info << endl;

			if (tipo_origen == "floppy" || tipo_origen == "IMG_floppy")
				out << "A:" << endl;
			else
				out << "D:" << endl;

			out.flush();
			file_out.close();
		}

		ejecutar(dosbox_path, "-conf|"+ grlDir.Temp +"dosbox.conf", grlDir.Temp);
		ui->btnOk->setEnabled(true);
	} else
		QMessageBox::information(this, "", tr("Puede que no este disponible el ejecutable del emulador.\nO el directorio de destino."));
}

void frmInstalarJuego::on_btnOk_clicked()
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

	QDialog::accept();
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
