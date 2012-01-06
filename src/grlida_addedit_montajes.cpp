/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_addedit_montajes.h"

frmAddEditMontajes::frmAddEditMontajes(QHash<QString, QString> montajes, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	connect( ui.cbxMontaje_type_drive, SIGNAL( activated(int) ), this, SLOT( on_changeTypeDrive(int) ) );
	connect( ui.btnOk       , SIGNAL( clicked() ), this, SLOT( on_btnOk()     ) );
	connect( ui.btnDirFile  , SIGNAL( clicked() ), this, SLOT( on_DirFile()   ) );
	connect( ui.btnSubirIso , SIGNAL( clicked() ), this, SLOT( on_SubirIso()  ) );
	connect( ui.btnBajarIso , SIGNAL( clicked() ), this, SLOT( on_BajarIso()  ) );
	connect( ui.lw_MultiIso , SIGNAL( itemActivated(QListWidgetItem*) ), ui.btnDeleteIso, SLOT(setEnabled(bool)));
	connect( ui.btnDeleteIso, SIGNAL( clicked() ), this, SLOT( on_DeleteIso() ) );

	stHomeDir = fGrl.GRlidaHomePath();		// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	setTheme();

	ui.cbxMontaje_type_drive->clear();
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_hd.png")    , tr("Carpeta como disco duro"), "drive"        );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")       , "cdrom"        );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_floppy.png"), tr("Carpeta como disquete")  , "floppy"       );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/floppy_1.png")    , tr("Imagen de disquete")     , "IMG_floppy"   );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")    , "IMG_iso"      );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")   , "IMG_multi_iso");
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_hd.png")    , tr("Imagen de disco duro")   , "IMG_hdd"      );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/floppy_2.png")    , tr("Imagen Botable")         , "boot"         );
	ui.cbxMontaje_type_drive->setCurrentIndex(0);
	on_changeTypeDrive(0);

	ui.cbxMontaje_mode_cdrom->clear();
	ui.cbxMontaje_mode_cdrom->addItem( tr("No usar nada")    , "");
	ui.cbxMontaje_mode_cdrom->addItem( tr("Usar")+" -aspi"   , "-aspi");
	ui.cbxMontaje_mode_cdrom->addItem( tr("Usar")+" -ioctl"  , "-ioctl");
	ui.cbxMontaje_mode_cdrom->addItem( tr("Usar")+" -noioctl", "-noioctl");
	ui.cbxMontaje_mode_cdrom->setCurrentIndex(0);

	ui.cbxMontaje_cdrom->clear();
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), tr("NO usar CD"), ""  );
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 0"    , "0" );
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 1"    , "1" );
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 2"    , "2" );
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 3"    , "3" );
	ui.cbxMontaje_cdrom->addItem(QIcon(stTheme+"img16/drive_cdrom.png"), "CD Index 4"    , "4" );
	ui.cbxMontaje_cdrom->setCurrentIndex(0);

	ui.lb_Montaje_info->setText( "<b>-aspi</b> -- "+ tr("Fuerza el uso de la capa aspi. Sólo válido si montas un CD-ROM bajo los sistemas Windows con un ASPI-Layer.")+"<br>"+
		"<b>-ioctl</b> -- "+ tr("Fuerza el uso de los comandos ioctl. Sólo válido si montar un CD-ROM bajo un sistema operativo de Windows que lo soporten (Win2000/XP/NT).")+"<br>"+
		"<b>-noioctl</b> -- "+ tr("Fuerza el uso de las SDL para el CD-ROM. Válido para todos los sistemas.")+"<br>"+
		tr("Procura no montar la Raiz del sistema operativo: ejemplo en windows seria")+" <b style=\"color:#FF0000;\">C:\\</b> "+ tr("y en linux seria directamente")+ " <b style=\"color:#FF0000;\">/</b> ");

	if( montajes["tipo_as"] == "IMG_multi_iso" )
	{
		ui.txtMontaje_path->setText("");
		QStringList listaIso = montajes["path"].split("|");
		ui.lw_MultiIso->clear();
		for (int i = 0; i < listaIso.count(); ++i)
		{
			QListWidgetItem *itemIso = new QListWidgetItem( ui.lw_MultiIso );
			itemIso->setIcon( QIcon(stTheme+"img16/cd_iso.png") );
			itemIso->setText( listaIso.value(i) );
		}
	} else
		ui.txtMontaje_path->setText( montajes["path"] );

	ui.txtMontaje_label->setText( montajes["label"] );
	ui.cbxMontaje_type_drive->setCurrentIndex( ui.cbxMontaje_type_drive->findData( montajes["tipo_as"] ) );
	ui.cbxMontaje_letter->setCurrentIndex( ui.cbxMontaje_letter->findText( montajes["letter"] ) );
	ui.cbxMontaje_cdrom->setCurrentIndex( ui.cbxMontaje_cdrom->findData( montajes["indx_cd"] ) );
	ui.txtMontaje_opt_mount->setText( montajes["opt_mount"] );
	ui.cbxMontaje_mode_cdrom->setCurrentIndex( ui.cbxMontaje_mode_cdrom->findData( montajes["io_ctrl"] ) );
	on_changeTypeDrive( ui.cbxMontaje_type_drive->currentIndex() );

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAddEditMontajes::~frmAddEditMontajes()
{
	//
}

void frmAddEditMontajes::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/applications.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnDirFile->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnClearDirFile->setIcon( QIcon(stTheme+"img16/limpiar.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
}

void frmAddEditMontajes::on_changeTypeDrive(int row)
{
	if( row >= 0)
	{
		QString tipo_montaje = ui.cbxMontaje_type_drive->itemData( row ).toString();
		if( tipo_montaje == "IMG_multi_iso" || tipo_montaje == "boot")
		{
			ui.txtMontaje_path->setVisible( false );
			ui.lw_MultiIso->setVisible( true );
			ui.btnSubirIso->setVisible( true );
			ui.btnBajarIso->setVisible( true );
			ui.btnDeleteIso->setVisible( true );
		} else {
			ui.txtMontaje_path->setVisible( true );
			ui.lw_MultiIso->setVisible( false );
			ui.btnSubirIso->setVisible( false );
			ui.btnBajarIso->setVisible( false );
			ui.btnDeleteIso->setVisible( false );
		}
	}
}

void frmAddEditMontajes::on_btnOk()
{
	DatosMontaje.clear();

	if( ui.cbxMontaje_type_drive->currentText() != "" )
		DatosMontaje["tipo_as"] = ui.cbxMontaje_type_drive->itemData( ui.cbxMontaje_type_drive->currentIndex() ).toString();	// tipo de montaje
	else
		DatosMontaje["tipo_as"] = "";

	if( DatosMontaje["tipo_as"] == "IMG_multi_iso" || DatosMontaje["tipo_as"] == "boot" )
	{
		QStringList listaIso;
		listaIso.clear();
		for (int i = 0; i < ui.lw_MultiIso->count(); ++i)
		{
			listaIso << ui.lw_MultiIso->item(i)->text();
		}
		DatosMontaje["path"]  = ""+listaIso.join("|");
	} else
		DatosMontaje["path"]  = ui.txtMontaje_path->text();					// directorio o iso

	DatosMontaje["label"] = ui.txtMontaje_label->text();					// etiqueta

	if( ui.cbxMontaje_letter->currentText() != "" )
		DatosMontaje["letter"] = ui.cbxMontaje_letter->currentText();		// letra de montaje
	else
		DatosMontaje["letter"] = "";

	DatosMontaje["indx_cd"]   = ui.cbxMontaje_cdrom->itemData( ui.cbxMontaje_cdrom->currentIndex() ).toString();		// index de la unidad de cd-rom
	DatosMontaje["opt_mount"] = ui.txtMontaje_opt_mount->text();			// opciones del cd-rom

	if( ui.cbxMontaje_mode_cdrom->currentText()!="" )
		DatosMontaje["io_ctrl"] = ui.cbxMontaje_mode_cdrom->itemData( ui.cbxMontaje_mode_cdrom->currentIndex() ).toString();	// cd/dvd windows, w9x, linux
	else
		DatosMontaje["io_ctrl"] = "";

	QDialog::accept();
}

void frmAddEditMontajes::on_DirFile()
{
	QString tipo_montaje = ui.cbxMontaje_type_drive->itemData( ui.cbxMontaje_type_drive->currentIndex() ).toString();

	if( tipo_montaje == "drive" || tipo_montaje == "cdrom" || tipo_montaje == "floppy" )
	{
		QString directorio = fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Montaje_path"].toString(), ui.txtMontaje_path->text() );
		if( directorio !="" )
		{
			ui.txtMontaje_path->setText( directorio );

			QDir dir( ui.txtMontaje_path->text() );
			if( dir.exists() )
				GRLConfig["Montaje_path"] = ui.txtMontaje_path->text();
			else
				GRLConfig["Montaje_path"] = "";
		}
	} else {
		QString tipo_archivo;
		if( tipo_montaje == "IMG_floppy" || tipo_montaje == "boot" || tipo_montaje == "IMG_hdd" )
			tipo_archivo = tr("Imagen") +" (*.ima *.img);;";
		else if( tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso" )
			tipo_archivo = tr("Imagen CD") +" (*.iso *.cue);;";
		else
			tipo_archivo = "";

		QString archivo = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Montaje_path"].toString(), ui.txtMontaje_path->text(), tipo_archivo + tr("Todos los archivo") +" (*)", 0, false);
		if( archivo !="" )
		{
			if( tipo_montaje == "IMG_multi_iso" || tipo_montaje == "boot" )
			{
				QListWidgetItem *itemIso = new QListWidgetItem( ui.lw_MultiIso );
				if( tipo_montaje == "boot" )
					itemIso->setIcon( QIcon(stTheme+"img16/floppy_2.png") );
				else
					itemIso->setIcon( QIcon(stTheme+"img16/cd_iso.png") );
				itemIso->setText( archivo );
			} else
				ui.txtMontaje_path->setText( archivo );

			QFileInfo fi( archivo );
			if( fi.exists() )
				GRLConfig["Montaje_path"] = fi.absolutePath();
			else
				GRLConfig["Montaje_path"] = "";
		}
	}

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Montaje_path", GRLConfig["Montaje_path"].toString() );
}

void frmAddEditMontajes::on_SubirIso()
{
	int listIndex = ui.lw_MultiIso->currentRow();
	if(listIndex > 0)
	{
		QListWidgetItem *item = new QListWidgetItem(*ui.lw_MultiIso->currentItem());
		delete ui.lw_MultiIso->currentItem();
		ui.lw_MultiIso->insertItem(listIndex - 1, item);
		ui.lw_MultiIso->setCurrentItem(item);
	}
}

void frmAddEditMontajes::on_BajarIso()
{
	int listIndex = ui.lw_MultiIso->currentRow();
	if(listIndex < (ui.lw_MultiIso->count() - 1))
	{
		QListWidgetItem *item = new QListWidgetItem(*ui.lw_MultiIso->currentItem());
		delete ui.lw_MultiIso->currentItem();
		ui.lw_MultiIso->insertItem(listIndex + 1, item);
		ui.lw_MultiIso->setCurrentItem(item);
	}
}

void frmAddEditMontajes::on_DeleteIso()
{
	QListWidgetItem *item = ui.lw_MultiIso->currentItem();
	if(item != NULL)
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Quieres eliminar la ISO de la lista?"), tr("Si"), tr("Cancelar"), 0 , 1);
		if(respuesta == 0)
		{
			delete item;
		}
	}
}

void frmAddEditMontajes::on_lw_MultiIso_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if(previous != NULL)
	{
		//
	}

	if(current != NULL)
	{
		int listIndex = ui.lw_MultiIso->currentRow();
		if(listIndex == 0)
			ui.btnSubirIso->setEnabled(false);
		else
			ui.btnSubirIso->setEnabled(true);

		if(listIndex == (ui.lw_MultiIso->count() - 1))
			ui.btnBajarIso->setEnabled(false);
		else
			ui.btnBajarIso->setEnabled(true);

		ui.btnDeleteIso->setEnabled(true);
	}
	else
	{
		ui.btnDeleteIso->setEnabled(false);
		ui.btnSubirIso->setEnabled(false);
		ui.btnBajarIso->setEnabled(false);
	}
}
