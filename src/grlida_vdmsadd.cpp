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

#include "grlida_vdmsadd.h"
#include "grlida_importar_juego.h"
#include "ui_vdmsound.h"

frmVdmsAdd::frmVdmsAdd(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmVdmsAdd)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;

	grlDir.Home     = fGrl->GRlidaHomePath();
	grlDir.Confvdms = grlDir.Home +"confvdms/"+ categoria.tabla +"/";

	cargarConfig();
	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmVdmsAdd::~frmVdmsAdd()
{
	delete ui;
}

void frmVdmsAdd::cargarConfig()
{
//setUpdatesEnabled(false);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	DatosJuego    = fGrl->getDefectDatos("vdmsound");
	DatosVDMSound = fGrl->getDefectVDMSound();

	cargarDatosVDMSound(DatosVDMSound);
//setUpdatesEnabled(true);
}

void frmVdmsAdd::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/vdmsound.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );

	ui->btnVdms_FileConfg->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_FileConfg_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_ExeJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_ExeJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_params_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_Icono->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_Icono_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDescargarInfo->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
}

void frmVdmsAdd::cargarDatosVDMSound(stConfigVDMSound cfgVdms)
{
	ui->txtVdms_path_conf->setText( cfgVdms.path_conf );
	ui->txtVdms_path_exe->setText( cfgVdms.path_exe );

	ui->txtVdms_params->setText( cfgVdms.program_1 );
	ui->txtVdms_icon->setText( cfgVdms.program_2 );
}

void frmVdmsAdd::setDatosVDMSound()
{
	DatosVDMSound.path_conf = ui->txtVdms_path_conf->text();
	DatosVDMSound.path_exe  = ui->txtVdms_path_exe->text();
	DatosVDMSound.program_1 = ui->txtVdms_params->text();
	DatosVDMSound.program_2 = ui->txtVdms_icon->text();
}

void frmVdmsAdd::on_btnOk_clicked()
{
	if( ui->txtDatos_Titulo->text().isEmpty() )
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el título."));
	else {
		if( ui->txtVdms_path_conf->text().isEmpty() )
			QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el archivo de configuración para el VDMSound"));
		else {
			QFile appConfg( grlDir.Confvdms + ui->txtVdms_path_conf->text() );
			if( appConfg.exists() )
				QMessageBox::information( this, titulo_ventana(), tr("El archivo de configuración para el VDMSound ya esixte"));
			else {
				if( ui->txtVdms_path_exe->text().isEmpty() )
					QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego"));
				else {
					setDatosVDMSound();

					DatosJuego.titulo   = ui->txtDatos_Titulo->text();
					DatosJuego.tipo_emu = "vdmsound";
					DatosJuego.idgrl    = sql->insertaDatos(categoria.tabla, DatosJuego);

					DatosVDMSound.idgrl = DatosJuego.idgrl;
					DatosVDMSound.idcat = categoria.id;
					DatosVDMSound.id    = sql->insertaVdms(DatosVDMSound);

					QDialog::accept();
				}
			}
		}
	}
}

void frmVdmsAdd::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmVdmsAdd::on_btnVdms_FileConfg_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Guardar archivo como..."), grlDir.Confvdms, ui->txtVdms_path_conf->text(), tr("Configuraciones") +" VDMSound (*.vlp);;"+ tr("Todos los archivo") +" (*)", 0, true);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
		ui->txtVdms_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".vlp");
	else
		ui->txtVdms_path_conf->setText(f_info.Name +".vlp");
}

void frmVdmsAdd::on_btnVdms_FileConfg_clear_clicked()
{
	ui->txtVdms_path_conf->clear();
}

void frmVdmsAdd::on_btnVdms_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Vdms_path_exe, ui->txtVdms_path_exe->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtVdms_path_exe->setText( archivo );
		grlCfg.Vdms_path_exe = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Vdms_path_exe", grlCfg.Vdms_path_exe);
	}
}

void frmVdmsAdd::on_btnVdms_ExeJuego_clear_clicked()
{
	ui->txtVdms_path_exe->clear();
}

void frmVdmsAdd::on_btnVdms_params_clear_clicked()
{
	ui->txtVdms_params->clear();
}

void frmVdmsAdd::on_btnVdms_Icono_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Vdms_icon, ui->txtVdms_icon->text(), tr("Iconos") +" (*.ico);;"+ tr("Todos los archivo") +" (*)", 0, false);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtVdms_icon->setText( archivo );
		grlCfg.Vdms_icon = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Vdms_icon", grlCfg.Vdms_icon);
	}
}

void frmVdmsAdd::on_btnVdms_Icono_clear_clicked()
{
	ui->txtVdms_icon->clear();
}

void frmVdmsAdd::on_btnDescargarInfo_clicked()
{
	stConfigDOSBox  DatosDosBox   = fGrl->getDefectDOSBox();
	stConfigScummVM  DatosScummVM = fGrl->getDefectScummVM();

	DatosJuego.titulo = ui->txtDatos_Titulo->text();

	frmImportarJuego *ImportarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, this);
	ImportarJuego->setWindowFlags(Qt::Window);

	if( ImportarJuego->exec() == QDialog::Accepted )
	{
		DatosJuego = ImportarJuego->getDatosJuegos();
		ui->txtDatos_Titulo->setText( DatosJuego.titulo );

		if( DatosJuego.tipo_emu == "vdmsound" )
		{
			cargarDatosVDMSound( ImportarJuego->getDatosVDMSound() );
		}
	}

	delete ImportarJuego;
}

void frmVdmsAdd::on_txtDatos_Titulo_textEdited(const QString &arg1)
{
	if( !arg1.isEmpty() )
	{
		QString str = fGrl->eliminar_caracteres( arg1 );
		bool str_ok = str.endsWith(".vlp");
		if(str_ok == false)
			str.append(".vlp");

		ui->txtVdms_path_conf->setText( str );
	} else
		ui->txtVdms_path_conf->setText("");
}
