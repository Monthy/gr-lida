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

#include "grlida_addedit_montajes.h"
#include "ui_addedit_montajes.h"

frmAddEditMontajes::frmAddEditMontajes(stGrlCfg m_cfg, bool m_editando, QStringList m_list_letter_uses, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmAddEditMontajes)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg   = m_cfg;
	Editando = m_editando;
	list_letter_uses = m_list_letter_uses;

	grlDir.Home = fGrl->dirApp();

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmAddEditMontajes::~frmAddEditMontajes()
{
	delete fGrl;
	delete ui;
}

void frmAddEditMontajes::cargarConfig()
{
	ui->cbxMontaje_type_drive->clear();
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Carpeta como disco duro")     , "drive"           );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")            , "cdrom"           );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_floppy.png"), tr("Carpeta como disquete")       , "floppy"          );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png")    , tr("Imagen de disquete")          , "IMG_floppy"      );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_1.png")    , tr("Imagen de disquete multiples"), "IMG_multi_floppy");
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")         , "IMG_iso"         );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")        , "IMG_multi_iso"   );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/drive_hd.png")    , tr("Imagen de disco duro")        , "IMG_hdd"         );
	ui->cbxMontaje_type_drive->addItem(QIcon(fGrl->theme() +"img16/floppy_2.png")    , tr("Imagen Botable")              , "boot"            );

	ui->cbxMontaje_mode_cdrom->clear();
	ui->cbxMontaje_mode_cdrom->addItem(tr("No usar nada"), "");
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -aspi "+ tr("Fuerza el uso de la capa ASPI. Sólo en Windows con un ASPI-Layer")          , "-aspi"     );
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -ioctl "+ tr("Selección automática de la interfaz de audio de CD")                       , "-ioctl"    );
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -ioctl_dx "+ tr("Extracción digital de audio utilizado para los CD de audio")            , "-ioctl_dx" );
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -ioctl_dio "+ tr("Llamadas ioctl utiliza para CD de audio")                              , "-ioctl_dio");
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -ioctl_mci "+ tr("MCI utiliza para CD de audio")                                         , "-ioctl_mci");
	ui->cbxMontaje_mode_cdrom->addItem(tr("Usar") +" -noioctl "+ tr("Fuerza el uso de las SDL para el CD-ROM. Válido para todos los sistemas"), "-noioctl"  );
	ui->cbxMontaje_mode_cdrom->setCurrentIndex(0);

	ui->cbxMontaje_opt_mount->clear();
	ui->cbxMontaje_opt_mount->addItem("");
	ui->cbxMontaje_opt_mount->addItem("-fs iso");
	ui->cbxMontaje_opt_mount->addItem("-fs fat");
	ui->cbxMontaje_opt_mount->addItem("-fs none");
	ui->cbxMontaje_opt_mount->setCurrentIndex(0);

	ui->cbxMontaje_cdrom->clear();
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), tr("NO usar CD"), "" );
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 0"    , "0");
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 1"    , "1");
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 2"    , "2");
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 3"    , "3");
	ui->cbxMontaje_cdrom->addItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 4"    , "4");
	ui->cbxMontaje_cdrom->setCurrentIndex(0);

	ui->h_SliderMontaje_freesize->setValue(250);

	list_letter.clear();
	list_letter << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";

	const int count_letter = list_letter_uses.size();
	for (int l = 0; l < count_letter; ++l)
		list_letter.removeAll(list_letter_uses.at(l));

	DatosMontaje.path         = "";
	DatosMontaje.label        = "";
	DatosMontaje.tipo_as      = "drive";
	DatosMontaje.letter       = "C";
	DatosMontaje.indx_cd      = "";
	DatosMontaje.opt_mount    = "";
	DatosMontaje.io_ctrl      = "";
	DatosMontaje.select_mount = "x";
	DatosMontaje.id_lista     = "";
	DatosMontaje.id_dosbox    = "";
	DatosMontaje.opt_size     = "";
	DatosMontaje.opt_freesize = "false";
	DatosMontaje.freesize     = "250";

	setDatosMontaje(DatosMontaje);

	ui->lb_Montaje_info->setText(tr("Procura no montar la Raiz del sistema operativo: ejemplo en windows seria") +" <b style=\"color:#FF0000;\">C:\\</b> "+ tr("y en linux seria directamente") +" <b style=\"color:#FF0000;\">/</b> ");
}

void frmAddEditMontajes::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/applications.png"));

// General
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
	ui->btnDirFile->setIcon(QIcon(fGrl->theme() +"img16/carpeta_0.png"));
	ui->btnDirFile_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSubirIso->setIcon(QIcon(fGrl->theme() +"img16/go-up.png"));
	ui->btnBajarIso->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
	ui->btnDeleteIso->setIcon(QIcon(fGrl->theme() +"img16/list_remove.png"));
	ui->btnMontaje_opt_size_mount_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
}

void frmAddEditMontajes::setDatosMontaje(stConfigDOSBoxMount montaje)
{
	DatosMontaje = montaje;
	if (montaje.tipo_as == "IMG_multi_iso" || montaje.tipo_as == "IMG_multi_floppy" || montaje.tipo_as == "boot")
	{
		ui->txtMontaje_path->setText("");
		QString img_tipo_as   = montaje.tipo_as == "IMG_multi_iso" ? "cd_iso.png" : "floppy_2.png";
		QStringList listaIsos = montaje.path.split("|");
		ui->lw_MultiIso->clear();
		const int listSize = listaIsos.size();
		for (int i = 0; i < listSize; ++i)
		{
			QListWidgetItem *itemIso = new QListWidgetItem;
			itemIso->setIcon(QIcon(fGrl->theme() +"img16/"+ img_tipo_as));
			itemIso->setText(listaIsos.at(i));
			ui->lw_MultiIso->addItem(itemIso);
		}
	} else
		ui->txtMontaje_path->setText(montaje.path);

	ui->txtMontaje_label->setText(montaje.label);
	ui->cbxMontaje_type_drive->setCurrentIndex(ui->cbxMontaje_type_drive->findData(montaje.tipo_as));
	ui->cbxMontaje_cdrom->setCurrentIndex(ui->cbxMontaje_cdrom->findData(montaje.indx_cd));
	ui->cbxMontaje_opt_mount->setEditText(montaje.opt_mount);
	ui->cbxMontaje_mode_cdrom->setCurrentIndex(ui->cbxMontaje_mode_cdrom->findData(montaje.io_ctrl));
	ui->txtMontaje_opt_size_mount->setText(DatosMontaje.opt_size);
	ui->chkMontaje_opt_freesize->setChecked(fGrl->strToBool(montaje.opt_freesize));
	ui->h_SliderMontaje_freesize->setEnabled(ui->chkMontaje_opt_freesize->isChecked());
	ui->h_SliderMontaje_freesize->setValue(montaje.freesize.toInt());
	emit on_h_SliderMontaje_freesize_valueChanged(montaje.freesize.toInt());
	emit on_cbxMontaje_type_drive_activated(ui->cbxMontaje_type_drive->currentIndex());
}

void frmAddEditMontajes::on_btnOk_clicked()
{
	QString str;

	str = ui->cbxMontaje_type_drive->itemData(ui->cbxMontaje_type_drive->currentIndex()).toString();
	DatosMontaje.tipo_as = str.isEmpty() ? "" : str;

	if (DatosMontaje.tipo_as == "IMG_multi_iso" || DatosMontaje.tipo_as == "IMG_multi_floppy" || DatosMontaje.tipo_as == "boot")
	{
		QStringList listaIsos;
		listaIsos.clear();
		const int count_isos = ui->lw_MultiIso->count();
		for (int i = 0; i < count_isos; ++i)
			listaIsos << ui->lw_MultiIso->item(i)->text();

		DatosMontaje.path = ""+ listaIsos.join("|");
	} else
		DatosMontaje.path = ui->txtMontaje_path->text();

	DatosMontaje.label        = ui->txtMontaje_label->text();
	DatosMontaje.letter       = ui->cbxMontaje_letter->currentText().isEmpty() ? "" : ui->cbxMontaje_letter->currentText();
	DatosMontaje.indx_cd      = ui->cbxMontaje_cdrom->itemData(ui->cbxMontaje_cdrom->currentIndex()).toString();
	DatosMontaje.opt_mount    = ui->cbxMontaje_opt_mount->currentText().isEmpty() ? "" : ui->cbxMontaje_opt_mount->currentText();
	DatosMontaje.opt_size     = ui->txtMontaje_opt_size_mount->text();
	DatosMontaje.opt_freesize = fGrl->boolToStr(ui->chkMontaje_opt_freesize->isChecked());
	DatosMontaje.freesize     = fGrl->intToStr(ui->h_SliderMontaje_freesize->value());

	str = ui->cbxMontaje_mode_cdrom->itemData(ui->cbxMontaje_mode_cdrom->currentIndex()).toString();
	DatosMontaje.io_ctrl = str.isEmpty() ? "" : str;

	QDialog::accept();
}

void frmAddEditMontajes::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmAddEditMontajes::on_cbxMontaje_type_drive_activated(int index)
{
	int id_letra = 0;
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

		ui->cbxMontaje_letter->clear();
		for (int i = 0; i < list_letter.size(); ++i)
			ui->cbxMontaje_letter->addItem(list_letter.at(i));

		QString letra = "C";
		if (tipo_montaje == "cdrom" || tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso")
			letra = "D";
		else if (tipo_montaje == "drive" || tipo_montaje == "IMG_hdd")
		{
			letra = "C";
			if (tipo_montaje == "IMG_hdd")
			{
				ui->cbxMontaje_letter->addItem("2");
				ui->cbxMontaje_letter->addItem("3");
			}
		}
		else if (tipo_montaje == "floppy" || tipo_montaje == "IMG_floppy" || tipo_montaje == "IMG_multi_floppy" || tipo_montaje == "boot")
		{
			letra = "A";
			if (tipo_montaje == "IMG_floppy")
			{
				ui->cbxMontaje_letter->addItem("0");
				ui->cbxMontaje_letter->addItem("1");
			}
		}
		id_letra = ui->cbxMontaje_letter->findText(Editando ? DatosMontaje.letter : letra);
	}
	ui->cbxMontaje_letter->setCurrentIndex(id_letra > -1 ? id_letra : 0);
}

void frmAddEditMontajes::on_btnDirFile_clicked()
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

		QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivos") +" (*)");

		if (!archivo.isEmpty())
		{
			stFileInfo f_info = fGrl->getInfoFile(archivo);
			if (f_info.Exists)
			{
				if (tipo_montaje == "IMG_multi_iso" || tipo_montaje == "IMG_multi_floppy" || tipo_montaje == "boot")
				{
					QListWidgetItem *itemIso = new QListWidgetItem;
					if (tipo_montaje == "boot" || tipo_montaje == "IMG_multi_floppy")
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

void frmAddEditMontajes::on_btnDirFile_clear_clicked()
{
	ui->txtMontaje_path->clear();
	ui->lw_MultiIso->clear();
}

void frmAddEditMontajes::on_btnSubirIso_clicked()
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

void frmAddEditMontajes::on_btnBajarIso_clicked()
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

void frmAddEditMontajes::on_btnDeleteIso_clicked()
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

void frmAddEditMontajes::on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous != NULL)
	{
		//
	}

	if (current != NULL)
	{
		int index = ui->lw_MultiIso->currentRow();
		if (index == 0)
			ui->btnSubirIso->setEnabled(false);
		else
			ui->btnSubirIso->setEnabled(true);

		if (index == (ui->lw_MultiIso->count() - 1))
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

void frmAddEditMontajes::on_chkMontaje_opt_freesize_clicked(bool checked)
{
	ui->h_SliderMontaje_freesize->setEnabled(checked);
}

void frmAddEditMontajes::on_h_SliderMontaje_freesize_valueChanged(int value)
{
	QString tipo_montaje, tipo_size;
	tipo_montaje = ui->cbxMontaje_type_drive->itemData(ui->cbxMontaje_type_drive->currentIndex()).toString();

	if (tipo_montaje == "floppy")
		tipo_size = "KB";
	else
		tipo_size = "MB";

	ui->chkMontaje_opt_freesize->setTitle(tr("Espacio libre virtual (%1 %2)").arg(value).arg(tipo_size));
}

void frmAddEditMontajes::on_btnMontaje_opt_size_mount_clear_clicked()
{
	ui->txtMontaje_opt_size_mount->clear();
}
