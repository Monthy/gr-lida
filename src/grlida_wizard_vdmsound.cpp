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

#include "grlida_wizard_vdmsound.h"
#include "ui_wizard_vdmsound.h"

frmWizardVdmSound::frmWizardVdmSound(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmWizardVdmSound)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql       = m_sql;
	grlCfg    = m_cfg;
	categoria = m_categoria;

	grlDir.Home       = fGrl->homePath();
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";
	grlDir.Temp       = grlDir.Home +"temp/";

	cargarConfig();
	setTheme();

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmWizardVdmSound::~frmWizardVdmSound()
{
	delete ui;
}

void frmWizardVdmSound::cargarConfig()
{
	DatosJuego    = sql->getDefectDatos("vdmsound");
	DatosVDMSound = sql->getDefectVDMSound();

	cargarDatosVDMSound(DatosVDMSound);
}

void frmWizardVdmSound::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
// General
	ui->btnVdms_ExeJuego->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnVdms_ExeJuego_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnVdms_params_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnVdms_Icono->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnVdms_Icono_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnDescargarInfo->setIcon(QIcon(fGrl->theme() +"img16/go-down.png"));
}

void frmWizardVdmSound::cargarDatosJuego(stDatosJuego datos)
{
	DatosJuego = datos;
	ui->txtDat_titulo->setText(datos.titulo);

	Thumbs      = datos.thumbs;
	CoverFront  = datos.cover_front;
	CoverBack   = datos.cover_back;
	CoverLeft   = datos.cover_left;
	CoverRight  = datos.cover_right;
	CoverTop    = datos.cover_top;
	CoverBottom = datos.cover_bottom;

// Thumbs
	file_thumbs = grlDir.Temp +"imagenes/small/"+ Thumbs;
	if (!QFile::exists(file_thumbs))
	{
		Thumbs = "";
		file_thumbs = "";
	}
// CoverFront
	file_cover_front = grlDir.Temp +"imagenes/"+ CoverFront;
	if (!QFile::exists(file_cover_front))
	{
		CoverFront = "";
		file_cover_front = "";
	}
// CoverBack
	file_cover_back = grlDir.Temp +"imagenes/"+ CoverBack;
	if (!QFile::exists(file_cover_back))
	{
		CoverBack = "";
		file_cover_back = "";
	}
// CoverLeft
	file_cover_left = grlDir.Temp +"imagenes/"+ CoverLeft;
	if (!QFile::exists(file_cover_left))
	{
		CoverLeft = "";
		file_cover_left = "";
	}
// CoverRight
	file_cover_right = grlDir.Temp +"imagenes/"+ CoverRight;
	if (!QFile::exists(file_cover_right))
	{
		CoverRight = "";
		file_cover_right = "";
	}
// CoverTop
	file_cover_top = grlDir.Temp +"imagenes/"+ CoverTop;
	if (!QFile::exists(file_cover_top))
	{
		CoverTop = "";
		file_cover_top = "";
	}
// CoverBottom
	file_cover_bottom = grlDir.Temp +"imagenes/"+ CoverBottom;
	if (!QFile::exists(file_cover_bottom))
	{
		CoverBottom = "";
		file_cover_bottom = "";
	}
}

bool frmWizardVdmSound::setDatosJuegos()
{
	bool isOk = false;

	DatosJuego.icono          = "cat/vdmsound.png";
	DatosJuego.titulo         = ui->txtDat_titulo->text();
	DatosJuego.titulo_guiones = sql->removeAccents(DatosJuego.titulo);
	DatosJuego.tipo_emu       = "vdmsound";
	DatosJuego.fecha          = sql->getTime();
	DatosJuego.idgrl          = sql->insertaDatos(categoria.tabla, DatosJuego);
	isOk = DatosJuego.idgrl.isEmpty() ? false : true;

	DatosJuego.game_dir = "id-"+ DatosJuego.idgrl +"_"+ DatosJuego.titulo_guiones +"_"+ DatosJuego.tipo_emu;
	grlDir.DatosGame    = grlDir.DatosDbGrl + DatosJuego.game_dir +"/";

	if (isOk)
	{
		DatosJuego.thumbs       = "";
		DatosJuego.cover_front  = "";
		DatosJuego.cover_back   = "";
		DatosJuego.cover_left   = "";
		DatosJuego.cover_right  = "";
		DatosJuego.cover_top    = "";
		DatosJuego.cover_bottom = "";

		fGrl->comprobarDirectorio(grlDir.DatosGame);
		fGrl->comprobarDirectorio(grlDir.DatosGame +"archivos/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"caja/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"capturas/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"imagenes/small/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"sonidos/");
		fGrl->comprobarDirectorio(grlDir.DatosGame +"videos/");

		if (!Thumbs.isEmpty() && QFile::exists(file_thumbs))
		{
			DatosJuego.thumbs = "thumbs."+ grlCfg.thumb_format.toLower();
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "thumbs", DatosJuego.thumbs);
			fGrl->crearThumbs(file_thumbs, grlDir.DatosGame + DatosJuego.thumbs, grlCfg.thumb_width, grlCfg.thumb_height, grlCfg.thumb_quality, true, grlCfg.thumb_format);
		}

		if (!CoverFront.isEmpty() && QFile::exists(file_cover_front))
		{
			DatosJuego.cover_front = "cover_front"+ fGrl->getInfoFile(file_cover_front).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_front", DatosJuego.cover_front);
			fGrl->copiarArchivo(file_cover_front, grlDir.DatosGame +"caja/"+ DatosJuego.cover_front, false, true);
		}

		if (!CoverBack.isEmpty() && QFile::exists(file_cover_back))
		{
			DatosJuego.cover_back = "cover_back"+ fGrl->getInfoFile(file_cover_back).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_back", DatosJuego.cover_back);
			fGrl->copiarArchivo(file_cover_back, grlDir.DatosGame +"caja/"+ DatosJuego.cover_back, false, true);
		}

		if (!CoverLeft.isEmpty() && QFile::exists(file_cover_left))
		{
			DatosJuego.cover_left = "cover_left"+ fGrl->getInfoFile(file_cover_left).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_left", DatosJuego.cover_left);
			fGrl->copiarArchivo(file_cover_left, grlDir.DatosGame +"caja/"+ DatosJuego.cover_left, false, true);
		}

		if (!CoverRight.isEmpty() && QFile::exists(file_cover_right))
		{
			DatosJuego.cover_right = "cover_right"+ fGrl->getInfoFile(file_cover_right).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_right", DatosJuego.cover_right);
			fGrl->copiarArchivo(file_cover_right, grlDir.DatosGame +"caja/"+ DatosJuego.cover_right, false, true);
		}

		if (!CoverTop.isEmpty() && QFile::exists(file_cover_top))
		{
			DatosJuego.cover_top = "cover_top"+ fGrl->getInfoFile(file_cover_top).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_top", DatosJuego.cover_top);
			fGrl->copiarArchivo(file_cover_top, grlDir.DatosGame +"caja/"+ DatosJuego.cover_top, false, true);
		}

		if (!CoverBottom.isEmpty() && QFile::exists(file_cover_bottom))
		{
			DatosJuego.cover_bottom = "cover_bottom"+ fGrl->getInfoFile(file_cover_bottom).Ext;
			sql->actualizaDatosItem(categoria.tabla, DatosJuego.idgrl, "cover_bottom", DatosJuego.cover_bottom);
			fGrl->copiarArchivo(file_cover_bottom, grlDir.DatosGame +"caja/"+ DatosJuego.cover_bottom, false, true);
		}

		if (listImagenesImportadas.size() > 0)
		{
			stDatosImagenes imagen;
			QString dir_out = grlDir.DatosGame +"imagenes/";
			const int numImagenes = listImagenesImportadas.size();
			for (int i = 0; i < numImagenes; ++i)
			{
				imagen = listImagenesImportadas.at(i);

				if (imagen.isImport)
					fGrl->copiarArchivo(imagen.dir_in +"imagenes/"+ imagen.nombre, dir_out + imagen.nombre, false, true);
				else
					fGrl->copiarArchivo(imagen.dir_in + imagen.nombre, dir_out + imagen.nombre, false, true);

				fGrl->crearThumbs(dir_out + imagen.nombre, dir_out +"small/"+ imagen.nombre +".jpg", grlCfg.thumb_img_width, grlCfg.thumb_img_height, grlCfg.thumb_img_quality);
			}
		}
	}

	return isOk;
}

void frmWizardVdmSound::cargarDatosVDMSound(stConfigVDMSound cfgVdms)
{
	ui->txtVdms_path_exe->setText(cfgVdms.path_exe);

	ui->txtVdms_params->setText(cfgVdms.program_1);
	ui->txtVdms_icon->setText(cfgVdms.program_2);
}

void frmWizardVdmSound::setDatosVDMSound()
{
	DatosVDMSound.path_exe  = ui->txtVdms_path_exe->text();
	DatosVDMSound.program_1 = ui->txtVdms_params->text();
	DatosVDMSound.program_2 = ui->txtVdms_icon->text();
}

void frmWizardVdmSound::on_btnOk_clicked()
{
	if (ui->txtDat_titulo->text().isEmpty())
		QMessageBox::information(this, titulo_ventana(), tr("Debes poner por lo menos el título."));
	else {
		if (ui->txtVdms_path_exe->text().isEmpty())
			QMessageBox::information(this, titulo_ventana(), tr("Debes indicar el ejecutable del juego"));
		else {
			if (setDatosJuegos())
			{
				setDatosVDMSound();

				DatosVDMSound.idgrl = DatosJuego.idgrl;
				DatosVDMSound.idcat = categoria.id;
				DatosVDMSound.id    = sql->insertaVdms(DatosVDMSound);

			// Guardamos los cambios en la base de datos.
				sql->comit();
				QDialog::accept();

			}
		}
	}
}

void frmWizardVdmSound::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmWizardVdmSound::on_btnVdms_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Vdms_path_exe, "DosGames", tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtVdms_path_exe->setText(fGrl->setDirRelative(archivo, "DosGames"));
			grlCfg.Vdms_path_exe = fGrl->setDirRelative(f_info.Path, "DosGames");
		}
	}
}

void frmWizardVdmSound::on_btnVdms_ExeJuego_clear_clicked()
{
	ui->txtVdms_path_exe->clear();
}

void frmWizardVdmSound::on_btnVdms_params_clear_clicked()
{
	ui->txtVdms_params->clear();
}

void frmWizardVdmSound::on_btnVdms_Icono_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos(this, tr("Selecciona un archivo"), grlCfg.Vdms_icon, "", tr("Iconos") +" (*.ico);;"+ tr("Todos los archivos") +" (*)");

	if (!archivo.isEmpty())
	{
		stFileInfo f_info = fGrl->getInfoFile(archivo);
		if (f_info.Exists)
		{
			ui->txtVdms_icon->setText(fGrl->setDirRelative(archivo));
			grlCfg.Vdms_icon = fGrl->setDirRelative(f_info.Path);
		}
	}
}

void frmWizardVdmSound::on_btnVdms_Icono_clear_clicked()
{
	ui->txtVdms_icon->clear();
}

void frmWizardVdmSound::on_btnDescargarInfo_clicked()
{
	stConfigDOSBox  DatosDosBox   = sql->getDefectDOSBox();
	stConfigScummVM  DatosScummVM = sql->getDefectScummVM();

	DatosJuego.titulo = ui->txtDat_titulo->text();

	frmImportarJuego *importarJuego = new frmImportarJuego(sql, grlCfg, categoria, DatosJuego, DatosDosBox, DatosScummVM, DatosVDMSound, false, false, this);
	importarJuego->setWindowFlags(Qt::Window);

	if (importarJuego->exec() == QDialog::Accepted)
	{
		grlCfg = importarJuego->getGrlCfg();

		if (importarJuego->getDatosImport().size() > 0)
		{
			stImport import = importarJuego->getDatosImport().at(0);

			DatosJuego = import.datos;
			listImagenesImportadas = import.imagenes;

			cargarDatosJuego(DatosJuego);

			if (!import.solo_datos && DatosJuego.tipo_emu == "vdmsound")
			{
				cargarDatosVDMSound(import.vdmsound);
			}
		}
	} else {
		grlCfg = importarJuego->getGrlCfg();
	}

	delete importarJuego;
}
