/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_instalar_juego.h"
#include "ui_instalar_juego.h"

frmInstalarJuego::frmInstalarJuego(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmInstalarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home = fGrl->GRlidaHomePath();
	grlDir.Temp = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmInstalarJuego::~frmInstalarJuego()
{
	delete fGrl;
	delete ui;
}

void frmInstalarJuego::cargarConfig()
{
	ui->cbxMontaje_type_drive->clear();
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/drive_hd.png")    , tr("Carpeta como disco duro"), "drive"        );
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")       , "cdrom"        );
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/drive_floppy.png"), tr("Carpeta como disquete")  , "floppy"       );
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/floppy_1.png")    , tr("Imagen de disquete")     , "IMG_floppy"   );
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")    , "IMG_iso"      );
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")   , "IMG_multi_iso");
	ui->cbxMontaje_type_drive->addItem( QIcon(fGrl->Theme() +"img16/drive_hd.png")    , tr("Imagen de disco duro")   , "IMG_hdd"      );
	ui->cbxMontaje_type_drive->setCurrentIndex(1);
	emit on_cbxMontaje_type_drive_activated(1);
}

void frmInstalarJuego::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/applications.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnDirFile->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnClearDirFile->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSubirIso->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btnBajarIso->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btnDeleteIso->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnDestino->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnDestinoClear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
}

void frmInstalarJuego::ejecutar(const QString &bin, const QString &parametros, const QString &dirWorking)
{
	QStringList stl_param;
	QProcess *grlProcess = new QProcess( this );
	QFile appBin( bin );
	if( appBin.exists() )
	{
		if( !dirWorking.isEmpty() )
			grlProcess->setWorkingDirectory( dirWorking );

		if( !parametros.isEmpty())
		{
			stl_param << parametros.split("|", QString::SkipEmptyParts);
			grlProcess->start( bin , stl_param );
			stl_param.clear();
		} else
			grlProcess->start( bin );
	} else
		QMessageBox::information(this, "", tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}

void frmInstalarJuego::on_cbxMontaje_type_drive_activated(int index)
{
	if( index > -1 )
	{
		if( ui->cbxMontaje_type_drive->itemData( index ).toString() == "IMG_multi_iso" )
		{
			ui->txtMontaje_path->setEnabled( false );
			ui->lw_MultiIso->setEnabled( true );
			ui->btnSubirIso->setEnabled( true );
			ui->btnBajarIso->setEnabled( true );
			ui->btnDeleteIso->setEnabled( true );
		} else {
			ui->txtMontaje_path->setEnabled( true );
			ui->lw_MultiIso->setEnabled( false );
			ui->btnSubirIso->setEnabled( false );
			ui->btnBajarIso->setEnabled( false );
			ui->btnDeleteIso->setEnabled( false );
		}
	}
}

void frmInstalarJuego::on_btnDirFile_clicked()
{
	QString tipo_montaje = ui->cbxMontaje_type_drive->itemData( ui->cbxMontaje_type_drive->currentIndex() ).toString();

	if( tipo_montaje == "drive" || tipo_montaje == "cdrom" || tipo_montaje == "floppy" )
	{
		QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Montaje_path, "DosGames");

		if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
		{
			ui->txtMontaje_path->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Montaje_path = ui->txtMontaje_path->text();

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Montaje_path", grlCfg.Montaje_path);
		}
	} else {
		QString tipo_archivo;
		if( tipo_montaje == "IMG_floppy" || tipo_montaje == "boot" || tipo_montaje == "IMG_hdd" )
			tipo_archivo = tr("Imagen") +" (*.ima *.img);;";
		else if( tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso" )
			tipo_archivo = tr("Imagen CD") +" (*.iso *.cue);;";
		else
			tipo_archivo = "";

		QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Montaje_path, "DosGames", tipo_archivo + tr("Todos los archivo") +" (*)", 0, false);

		if( !archivo.isEmpty() )
		{
			stFileInfo f_info = fGrl->getInfoFile( archivo );
			if( f_info.Exists )
			{
				if( tipo_montaje == "IMG_multi_iso" || tipo_montaje == "boot" )
				{
					QListWidgetItem *itemIso = new QListWidgetItem( ui->lw_MultiIso );
					if( tipo_montaje == "boot" )
						itemIso->setIcon( QIcon(fGrl->Theme() +"img16/floppy_2.png") );
					else
						itemIso->setIcon( QIcon(fGrl->Theme() +"img16/cd_iso.png") );
					itemIso->setText( fGrl->setDirRelative(archivo, "DosGames") );
				} else
					ui->txtMontaje_path->setText( fGrl->setDirRelative(archivo, "DosGames") );

				grlCfg.Montaje_path = fGrl->setDirRelative(f_info.Path, "DosGames");

				fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Montaje_path", grlCfg.Montaje_path);
			}
		}
	}
}

void frmInstalarJuego::on_btnClearDirFile_clicked()
{
	ui->txtMontaje_path->clear();
}

void frmInstalarJuego::on_btnSubirIso_clicked()
{
	int index = ui->lw_MultiIso->currentRow();
	if( index > 0 )
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
	if( index < (ui->lw_MultiIso->count() - 1) )
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
	if( item != NULL )
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Quieres eliminar la ISO de la lista?"), tr("Si"), tr("No"), 0, 1);
		if( respuesta == 0 )
		{
			delete item;
		}
	}
}

void frmInstalarJuego::on_btnDestino_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Montaje_path, "DosGames");

	if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
	{
		ui->btnOk->setEnabled(true);
		ui->txtDestinoPath->setText( fGrl->setDirRelative(directorio, "DosGames") );
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

void frmInstalarJuego::on_btnOk_clicked()
{
	QDir dir;
	QString install_info = tr("El ejecutable suele ser: INSTALL, si no esta seguro teclee DIR y pulsa INTRO");
	if( QFile::exists(grlCfg.DirDOSBox) && dir.exists(ui->txtDestinoPath->text()) )
	{
		QStringList lista_multiple_iso;
		QString tipo_origen = ui->cbxMontaje_type_drive->itemData( ui->cbxMontaje_type_drive->currentIndex() ).toString();
		QFile file_out(grlDir.Temp +"dosbox.conf");
		if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
		{
			QString dir_montaje = fGrl->getDirRelative(ui->txtMontaje_path->text(), "DosGames");

			QTextStream out(&file_out);
			out.setCodec("UTF-8");
			out << "[autoexec]" << endl;
			out << "@echo off" << endl;

			if( tipo_origen == "drive" )
				out << "mount D \"" << dir_montaje << "\"" << endl;
			else if( tipo_origen == "cdrom" )
				out << "mount D \"" << dir_montaje << "\" -t cdrom" << endl;
			else if( tipo_origen == "floppy" )
				out << "mount A \"" << dir_montaje << "\" -t floppy" << endl;
			else if( tipo_origen == "IMG_floppy" )
				out << "imgmount A \"" << dir_montaje << "\" -t floppy" << endl;
			else if( tipo_origen == "IMG_iso" )
				out << "imgmount D \"" << dir_montaje << "\" -t iso" << endl;
			else if( tipo_origen == "IMG_multi_iso" )
			{
				lista_multiple_iso.clear();
				for( int i = 0; i < ui->lw_MultiIso->count(); ++i )
				{
					if(i == 0)
						lista_multiple_iso << "\""+ fGrl->getShortPathName( fGrl->getDirRelative(ui->lw_MultiIso->item(i)->text()) ) +"\"";
					else
						lista_multiple_iso << "\""+ fGrl->getDirRelative(ui->lw_MultiIso->item(i)->text()) +"\"";
				}
				out << "imgmount D " << lista_multiple_iso.join(" ") << " -t iso" << endl;
			}
			else if( tipo_origen == "IMG_hdd" )
				out << "imgmount D \"" << dir_montaje << "\" -t hdd" << endl;

			out << "mount C \"" << fGrl->getDirRelative(ui->txtDestinoPath->text(), "DosGames") << "\"" << endl;
			out << "echo " << install_info << endl;

			if(tipo_origen == "floppy" || tipo_origen == "IMG_floppy")
				out << "A:" << endl;
			else
				out << "D:" << endl;

			out.flush();
			file_out.close();
		}
		ejecutar( grlCfg.DirDOSBox, "-conf|"+grlDir.Temp +"dosbox.conf", grlDir.Temp);
	} else
		QMessageBox::information( this, "",tr("Puede que no este disponible el ejecutable del emulador.\nO el directorio de destino."));

}

void frmInstalarJuego::on_lw_MultiIso_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if(previous != NULL)
	{
		//
	}

	if(current != NULL)
	{
		int listIndex = ui->lw_MultiIso->currentRow();
		if(listIndex == 0)
			ui->btnSubirIso->setEnabled(false);
		else
			ui->btnSubirIso->setEnabled(true);

		if(listIndex == (ui->lw_MultiIso->count() - 1))
			ui->btnBajarIso->setEnabled(false);
		else
			ui->btnBajarIso->setEnabled(true);

		ui->btnDeleteIso->setEnabled(true);
	}
	else
	{
		ui->btnDeleteIso->setEnabled(false);
		ui->btnSubirIso->setEnabled(false);
		ui->btnBajarIso->setEnabled(false);
	}
}
