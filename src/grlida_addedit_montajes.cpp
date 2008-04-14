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

#include "grlida_addedit_montajes.h"

frmAddEditMontajes::frmAddEditMontajes(bool EditMount, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	connect( ui.btnOk      , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.btnDirFile , SIGNAL( clicked() ), this, SLOT( on_DirFile() ) );

	stHomeDir = QDir::homePath()+"/.gr-lida/";		// directorio de trabajo del GR-lida
//	stHomeDir = QDir::currentPath()+"/";			// directorio de trabajo del GR-lida
//	stHomeDir = "./";								// directorio de trabajo del GR-lida

	ui.lb_Montaje_info->setText( "<b>-aspi</b> -- "+ tr("Forces use of the aspi layer. Only valid if mounting a CD-ROM under Windows systems with an ASPI-Layer.")+"<br>"+
		"<b>-ioctl</b> -- "+ tr("Forces use of ioctl commands. Only valid if mounting a CD-ROM under a Windows OS which support them (Win2000/XP/NT).")+"<br>"+
		"<b>-noioctl</b> -- "+ tr("Forces use of the SDL CD-ROM layer. Valid on all systems.")+"<br>"+
		tr("Procura no montar la Raiz del sistema operativo: ejemplo en windows seria")+" <b style=\"color:#FF0000;\">C:\\</b> "+ tr("y en linux seria directamente")+ " <b style=\"color:#FF0000;\">/</b> ");

	if(EditMount==true)
	{
		ui.txtMontaje_path->setText( DatosMontaje["path"] );
		ui.txtMontaje_label->setText( DatosMontaje["label"] );
		ui.cbxMontaje_type_drive->setCurrentIndex( ui.cbxMontaje_type_drive->findText( DatosMontaje["tipo_as"] ) );
		ui.cbxMontaje_letter->setCurrentIndex( ui.cbxMontaje_letter->findText( DatosMontaje["letter"] ) );
		ui.cbxMontaje_cdrom->setCurrentIndex( ui.cbxMontaje_cdrom->findText( DatosMontaje["indx_cd"] ) );
		ui.txtMontaje_opt_mount->setText( DatosMontaje["opt_mount"] );
		ui.cbxMontaje_mode_cdrom->setCurrentIndex( ui.cbxMontaje_mode_cdrom->findText( DatosMontaje["io_ctrl"], Qt::MatchContains ) );
	}

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAddEditMontajes::~frmAddEditMontajes(){}
		
void frmAddEditMontajes::on_btnOk()
{
	DatosMontaje.clear();
	DatosMontaje["path"]  = ui.txtMontaje_path->text();						// directorio o iso
	DatosMontaje["label"] = ui.txtMontaje_label->text();					// etiqueta

	if ( ui.cbxMontaje_type_drive->currentText()!="" )
		DatosMontaje["tipo_as"] = ui.cbxMontaje_type_drive->currentText();	// tipo de montaje
	else DatosMontaje["tipo_as"] = "";

	if ( ui.cbxMontaje_letter->currentText()!="" )
		DatosMontaje["letter"] = ui.cbxMontaje_letter->currentText();		// letra de montaje
	else DatosMontaje["letter"] = "";

	if ( ui.cbxMontaje_cdrom->currentText()!="" )
		DatosMontaje["indx_cd"] = ui.cbxMontaje_cdrom->currentText();		// index de la unidad de cd-rom
	else DatosMontaje["indx_cd"] = "";

	DatosMontaje["opt_mount"] = ui.txtMontaje_opt_mount->text();			// opciones del cd-rom

	if ( ui.cbxMontaje_mode_cdrom->currentText()!="" )
		DatosMontaje["io_ctrl"] = ui.cbxMontaje_mode_cdrom->currentText();	// cd/dvd windows, w9x, linux
	else DatosMontaje["io_ctrl"] = "-noioctl";								// Forces use of the SDL

	QDialog::accept();
}

void frmAddEditMontajes::on_DirFile()
{
	int tipomontaje = ui.cbxMontaje_type_drive->currentIndex();
	if((tipomontaje==0)||(tipomontaje==1)||(tipomontaje==2))
		ui.txtMontaje_path->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtMontaje_path->text() ));
	else
		ui.txtMontaje_path->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtMontaje_path->text(), tr("Todos los archivo") + " (*)", 0, false) );
}
