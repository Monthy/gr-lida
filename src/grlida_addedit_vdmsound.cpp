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

#include "grlida_addedit_vdmsound.h"
#include "ui_addedit_vdmsound.h"

frmAddEditVDMSound::frmAddEditVDMSound(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent) :
	QTabWidget(parent),
	ui(new Ui::frmAddEditVDMSound)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;
	IdGame    = id_game;
	Editando  = m_editando;

	grlDir.Home     = fGrl->GRlidaHomePath();
	grlDir.Confvdms = grlDir.Home +"confvdms/"+ categoria.tabla +"/";
}

frmAddEditVDMSound::~frmAddEditVDMSound()
{
	delete fGrl;
	delete ui;
}

void frmAddEditVDMSound::cargarConfig()
{
	ui->txtVdms_path_conf->clear();
	ui->txtVdms_path_exe->clear();
	ui->txtVdms_params->clear();
	ui->txtVdms_icon->clear();
	ui->txtVdms_autoexec->clear();
	ui->txtVdms_customCfg->clear();
	ui->chkVdms_useAutoexec->setChecked(false);
	ui->chkVdms_useCustomCfg->setChecked(false);
	ui->chkVdms_exitclose->setChecked(false);
	ui->chkVdms_exitWarn->setChecked(false);
	ui->chkVdms_fastPaste->setChecked(false);
	ui->chkVdms_useCDROM->setChecked(false);
	ui->chkVdms_useNetware->setChecked(false);
}

void frmAddEditVDMSound::setTheme()
{
	ui->btnVdms_FileConfg->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_FileConfg_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_ExeJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_ExeJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_params_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_Icono->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_Icono_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
}

bool frmAddEditVDMSound::isCorrectNext()
{
	bool siguiente = false;

	if( ui->txtVdms_path_conf->text().isEmpty() )
	{
		siguiente = false;
		QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el archivo de configuración para el VDMSound"));
	} else {
		siguiente = true;
		if( ui->txtVdms_path_exe->text().isEmpty() )
		{
			siguiente=false;
			QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego para el VDMSound"));
		} else {
			siguiente = true;

			if( !Editando )
			{
				QFile appConfg( grlDir.Confvdms + ui->txtVdms_path_conf->text() );
				if( appConfg.exists() )
				{
					siguiente = false;
					QMessageBox::information(this, titulo_ventana(), tr("El archivo de configuración para el VDMSound ya esixte"));
				} else
					siguiente = true;
			}
		}
	}

	return siguiente;
}

void frmAddEditVDMSound::cargarDatosVDMSound(stConfigVDMSound cfgVdms)
{
	ui->txtVdms_path_conf->setText( cfgVdms.path_conf );
	ui->txtVdms_path_exe->setText( cfgVdms.path_exe );

	ui->txtVdms_params->setText( cfgVdms.program_1 );
	ui->txtVdms_icon->setText( cfgVdms.program_2 );

	ui->chkVdms_useAutoexec->setChecked( fGrl->StrToBool( cfgVdms.winnt_dos_1 ) );
	ui->txtVdms_autoexec->setPlainText( cfgVdms.winnt_dos_2.replace("%000d%000a", "\n") );

	ui->chkVdms_useCustomCfg->setChecked( fGrl->StrToBool( cfgVdms.vdms_debug_1 ) );
	ui->txtVdms_customCfg->setPlainText( cfgVdms.vdms_debug_2.replace("%000d%000a", "\n") );

	ui->chkVdms_exitclose->setChecked( fGrl->StrToBool( cfgVdms.winnt_dosbox_1 ) );
	ui->chkVdms_exitWarn->setChecked( fGrl->StrToBool( cfgVdms.winnt_dosbox_2 ) );
	ui->chkVdms_fastPaste->setChecked( fGrl->StrToBool( cfgVdms.winnt_dosbox_3 ) );

	ui->chkVdms_useCDROM->setChecked( fGrl->StrToBool( cfgVdms.winnt_storage_1 ) );
	ui->chkVdms_useNetware->setChecked( fGrl->StrToBool( cfgVdms.winnt_storage_2 ) );

	if( Editando && !IdGame.isEmpty() )
		IdVdms = cfgVdms.id;
	else
		IdVdms = "";
}

void frmAddEditVDMSound::setDatosVDMSound()
{
	DatosVDMSound.id    = IdVdms;
	DatosVDMSound.idgrl = IdGame;
	DatosVDMSound.idcat = categoria.id;

	DatosVDMSound.path_conf = ui->txtVdms_path_conf->text();
	DatosVDMSound.path_exe  = ui->txtVdms_path_exe->text();
	DatosVDMSound.program_1 = ui->txtVdms_params->text();
	DatosVDMSound.program_2 = ui->txtVdms_icon->text();

	DatosVDMSound.winnt_dos_1 = fGrl->BoolToStr( ui->chkVdms_useAutoexec->isChecked(), true);
	DatosVDMSound.winnt_dos_2 = ui->txtVdms_autoexec->toPlainText().replace("\n", "%000d%000a");

	DatosVDMSound.vdms_debug_1 = fGrl->BoolToStr( ui->chkVdms_useCustomCfg->isChecked(), true);
	DatosVDMSound.vdms_debug_2 = ui->txtVdms_customCfg->toPlainText().replace("\n", "%000d%000a");

	DatosVDMSound.winnt_dosbox_1 = fGrl->BoolToStr( ui->chkVdms_exitclose->isChecked(), true);
	DatosVDMSound.winnt_dosbox_2 = fGrl->BoolToStr( ui->chkVdms_exitWarn->isChecked() , true);
	DatosVDMSound.winnt_dosbox_3 = fGrl->BoolToStr( ui->chkVdms_fastPaste->isChecked(), true);

	DatosVDMSound.winnt_storage_1 = fGrl->BoolToStr( ui->chkVdms_useCDROM->isChecked()  , true);
	DatosVDMSound.winnt_storage_2 = fGrl->BoolToStr( ui->chkVdms_useNetware->isChecked(), true);
}

QString frmAddEditVDMSound::getPathConf()
{
	return ui->txtVdms_path_conf->text();
}

void frmAddEditVDMSound::setPathConf(QString config)
{
	ui->txtVdms_path_conf->setText( config );
}

void frmAddEditVDMSound::on_btnVdms_FileConfg_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Guardar archivo como..."), grlDir.Confvdms, "", tr("Configuraciones") +" VDMSound (*.vlp);;"+ tr("Todos los archivo") +" (*)", 0, true);

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists && !Editando )
			ui->txtVdms_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".vlp");
		else
			ui->txtVdms_path_conf->setText(f_info.Name +".vlp");
	}

}

void frmAddEditVDMSound::on_btnVdms_FileConfg_clear_clicked()
{
	ui->txtVdms_path_conf->clear();
}

void frmAddEditVDMSound::on_btnVdms_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Vdms_path_exe, "DosGames", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtVdms_path_exe->setText( fGrl->setDirRelative(archivo, "DosGames") );
			grlCfg.Vdms_path_exe = fGrl->setDirRelative(f_info.Path, "DosGames");

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Vdms_path_exe", grlCfg.Vdms_path_exe);
		}
	}
}

void frmAddEditVDMSound::on_btnVdms_ExeJuego_clear_clicked()
{
	ui->txtVdms_path_exe->clear();
}

void frmAddEditVDMSound::on_btnVdms_params_clear_clicked()
{
	ui->txtVdms_params->clear();
}

void frmAddEditVDMSound::on_btnVdms_Icono_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Vdms_icon, "", tr("Iconos") +" (*.ico);;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			ui->txtVdms_icon->setText( fGrl->setDirRelative(archivo) );
			grlCfg.Vdms_icon = fGrl->setDirRelative(f_info.Path);

			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Vdms_icon", grlCfg.Vdms_icon);
		}
	}

}

void frmAddEditVDMSound::on_btnVdms_Icono_clear_clicked()
{
	ui->txtVdms_icon->clear();
}
