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

frmAddEditMontajes::frmAddEditMontajes(bool EditMount,const QVector<QString> datos_montaje, QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
	
	connect( ui.btnOk      , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.btnDirFile , SIGNAL( clicked() ), this, SLOT( on_DirFile() ) );

	stHomeDir     = QDir::homePath()+"/.gr-lida/";		// directorio de trabajo del GR-lida
//	stHomeDir     = QDir::currentPath()+"/";			// directorio de trabajo del GR-lida
//	stHomeDir     = "./";								// directorio de trabajo del GR-lida

	ui.lb_Montaje_6->setText( "<b>-aspi</b> -- "+ tr("Forces use of the aspi layer. Only valid if mounting a CD-ROM under Windows systems with an ASPI-Layer.")+"<br>"+
		"<b>-ioctl</b> -- "+ tr("Forces use of ioctl commands. Only valid if mounting a CD-ROM under a Windows OS which support them (Win2000/XP/NT).")+"<br>"+
		"<b>-noioctl</b> -- "+ tr("Forces use of the SDL CD-ROM layer. Valid on all systems.")+"<br>"+
		tr("Procura no montar la Raiz del sistema operativo: ejemplo en windows seria")+" <b style=\"color:#FF0000;\">C:\\</b> "+ tr("y en linux seria directamente")+ " <b style=\"color:#FF0000;\">/</b> ");

	if(EditMount==true)
	{
		ui.txtMontaje_1->setText( datos_montaje[0] );
		ui.txtMontaje_2->setText( datos_montaje[1] );
		ui.cbxMontaje_1->setCurrentIndex( ui.cbxMontaje_1->findText( datos_montaje[2] ) );
		ui.cbxMontaje_2->setCurrentIndex( ui.cbxMontaje_2->findText( datos_montaje[3] ) );
		ui.cbxMontaje_3->setCurrentIndex( ui.cbxMontaje_3->findText( datos_montaje[4] ) );
		ui.txtMontaje_3->setText( datos_montaje[5] );
		ui.cbxMontaje_4->setCurrentIndex( ui.cbxMontaje_4->findText( datos_montaje[6], Qt::MatchContains ) )	; // platform
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
	DatosMontaje << ui.txtMontaje_1->text();			//0 directorio o iso
	DatosMontaje << ui.txtMontaje_2->text();			//1 etiqueta
	if ( ui.cbxMontaje_1->currentText()!="" )
		DatosMontaje << ui.cbxMontaje_1->currentText();	//2 tipo de montaje
	else DatosMontaje << "";
	if ( ui.cbxMontaje_2->currentText()!="" )
		DatosMontaje << ui.cbxMontaje_2->currentText();	//3 letra de montaje
	else DatosMontaje << "";
	if ( ui.cbxMontaje_3->currentText()!="" )
		DatosMontaje << ui.cbxMontaje_3->currentText();	//4 index de la unidad de cd-rom
	else DatosMontaje << "";
	DatosMontaje << ui.txtMontaje_3->text();			//5 opciones del cd-rom
	if ( ui.cbxMontaje_4->currentText()!="" )
		DatosMontaje << ui.cbxMontaje_4->currentText();	//6 cd/dvd windows, w9x, linux 
	else DatosMontaje << "-noioctl"; // Forces use of the SDL

	QDialog::accept();
}

void frmAddEditMontajes::on_DirFile()
{
	int tipomontaje = ui.cbxMontaje_1->currentIndex();
	if((tipomontaje==0)||(tipomontaje==1)||(tipomontaje==2))
		ui.txtMontaje_1->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), stHomeDir, ui.txtMontaje_1->text() ));
	else
		ui.txtMontaje_1->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), stHomeDir, ui.txtMontaje_1->text(), tr("Todos los archivo (*)"), 0, false) );
}
