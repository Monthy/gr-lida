/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2021 Pedro A. Garcia Rosado Aka Monthy
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

/*
 * Modos de Compatibilidad
 *   WIN95          Windows 95
 *   WIN98          Windows 98 / Windows Me
 *   WIN4SP5        Windows NT 4.0 SP5
 *   WIN2000        Windows 2000
 *   WINXPSP2       Windows XP SP2
 *   WINXPSP3       Windows XP SP3
 *   VISTARTM       Vista
 *   VISTASP1       Vista SP1
 *   VISTASP2       Vista SP2
 *   WIN7RTM        Windows 7
 *   WIN8RTM        Windows 8
 *   WINSRV03SP1    Windows Server 2003 SP1
 *   WINSRV08SP1    Windows Server 2008 SP1
 *
 * Configuraciones
 *   DISABLETHEMES                     Deshabilitar los temas visuales
 *   640X480                           Ejecutar con una resolución de pantalla de 640 x 480
 *   HIGHDPIAWARE                      Disable display scaling on high DPI settings
 *   256COLOR y 16BITCOLOR             Modo de color reducido
 *     256COLOR                        Color de 8 bits (256)
 *     16BITCOLOR                      Color de 16 bits (65536)
 *   DISABLEDWM                        Disable Desktop Composition
 *   DISABLEDXMAXIMIZEDWINDOWEDMODE    Deshabilitar optimizacion de pantalla completa
 *
 * Nivel de privilegio
 *   RUNASADMIN    Ejecutar este programa como administrador
*/

#include <QDesktopWidget>
#include <QStyle>
#include <QMessageBox>
#include <QDir>
#include <windows.h>

#include "grlida_compatibilidad_exe.h"
#include "ui_compatibilidad_exe.h"

frmCompatibilidadExe::frmCompatibilidadExe(QString m_path_exe, QString m_cfg_compatibilidad, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmCompatibilidadExe)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	path_exe           = m_path_exe;
	cfg_compatibilidad = m_cfg_compatibilidad;

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmCompatibilidadExe::~frmCompatibilidadExe()
{
	delete fGrl;
	delete ui;
}

void frmCompatibilidadExe::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/applications.png"));

// General
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnCancel->setIcon(QIcon(fGrl->theme() +"img16/cancelar.png"));
}

void frmCompatibilidadExe::cargarConfig()
{
	QStringList lista_modo_compatibilidad;
	lista_modo_compatibilidad << "WIN95"  << "WIN98"
		<< "WIN4SP5" << "WIN2000" << "WINXPSP2" << "WINXPSP3"
		<< "VISTARTM" << "VISTASP1" << "VISTASP2"
		<< "WIN7RTM" << "WIN8RTM"
		<< "WINSRV03SP1" << "WINSRV08SP1";

	ui->cbxDat_compatibilidad_modo->clear();
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win9x.png") , "Windows 95"             , "WIN95"      );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win9x.png") , "Windows 98 / Windows Me", "WIN98"      );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/winnt.png") , "Windows NT 4.0 SP5"     , "WIN4SP5"    );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/winnt.png") , "Windows 2000"           , "WIN2000"    );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/winxp.png") , "Windows XP SP2"         , "WINXPSP2"   );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/winxp.png") , "Windows XP SP3"         , "WINXPSP3"   );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win7.png")  , "Vista"                  , "VISTARTM"   );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win7.png")  , "Vista SP1"              , "VISTASP1"   );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win7.png")  , "Vista SP2"              , "VISTASP2"   );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win7.png")  , "Windows 7"              , "WIN7RTM"    );
	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/win8.png")  , "Windows 8"              , "WIN8RTM"    );
//	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "Windows Server 2003 SP1", "WINSRV03SP1");
//	ui->cbxDat_compatibilidad_modo->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "Windows Server 2008 SP1", "WINSRV08SP1");

	ui->chkDat_compatibilidad_color_reducido->setChecked(false);
	ui->cbxDat_compatibilidad_color_reducido->clear();
	ui->cbxDat_compatibilidad_color_reducido->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "Color de 8 bits (256)"   , "256COLOR"  );
	ui->cbxDat_compatibilidad_color_reducido->addItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "Color de 16 bits (65536)", "16BITCOLOR");
	ui->cbxDat_compatibilidad_color_reducido->setEnabled(false);

	ui->chkDat_compatibilidad_resolucion_640x480->setChecked(false);
	ui->chkDat_compatibilidad_deshabilitar_temas->setChecked(false);
	ui->chkDat_compatibilidad_desactivar_servicios_avanzados_texto->setChecked(false);
	ui->chkDat_compatibilidad_ejecutar_admin->setChecked(false);

	QStringList cfg_tmp   = cfg_compatibilidad.split("|");
	QStringList cfg_param = cfg_tmp.at(1).split(" ");

	ui->gBox_compatibilidad_modo->setChecked(fGrl->strToBool(cfg_tmp.at(0)));

	bool color_seleccionado = false;
	int row_compatibilidad_modo = -1;
	foreach (QString cfg, cfg_param)
	{
		if (row_compatibilidad_modo == -1)
			row_compatibilidad_modo = lista_modo_compatibilidad.indexOf(cfg, 0);
		if (!color_seleccionado && (cfg == "256COLOR" || cfg == "16BITCOLOR"))
		{
			color_seleccionado = true;
			int idx_color = (cfg == "16BITCOLOR") ? 1 : 0;
			ui->chkDat_compatibilidad_color_reducido->setChecked(true);
			ui->cbxDat_compatibilidad_color_reducido->setEnabled(true);
			ui->cbxDat_compatibilidad_color_reducido->setCurrentIndex(idx_color);
		}
		if (cfg == "640X480")
			ui->chkDat_compatibilidad_resolucion_640x480->setChecked(true);
		if (cfg == "DISABLETHEMES")
			ui->chkDat_compatibilidad_deshabilitar_temas->setChecked(true);
		if (cfg == "DISABLEDWM")
			ui->chkDat_compatibilidad_desactivar_servicios_avanzados_texto->setChecked(true);
		if (cfg == "RUNASADMIN")
			ui->chkDat_compatibilidad_ejecutar_admin->setChecked(true);
	}

	if (row_compatibilidad_modo < 0) row_compatibilidad_modo = 0;
	ui->cbxDat_compatibilidad_modo->setCurrentIndex(row_compatibilidad_modo);
}

void frmCompatibilidadExe::on_btnOk_clicked()
{
#ifdef Q_OS_WIN
	bool borrar_reg_compatibilidad = false;
	QStringList cfg_param;
	//cfg_param << "~";// En Window 10

	if (ui->gBox_compatibilidad_modo->isChecked())
		cfg_param << ui->cbxDat_compatibilidad_modo->itemData(ui->cbxDat_compatibilidad_modo->currentIndex()).toString();
	if (ui->chkDat_compatibilidad_color_reducido->isChecked())
		cfg_param << ui->cbxDat_compatibilidad_color_reducido->itemData(ui->cbxDat_compatibilidad_color_reducido->currentIndex()).toString();;
	if (ui->chkDat_compatibilidad_resolucion_640x480->isChecked())
		cfg_param << "640X480";
	if (ui->chkDat_compatibilidad_deshabilitar_temas->isChecked())
		cfg_param << "DISABLETHEMES";
	if (ui->chkDat_compatibilidad_desactivar_servicios_avanzados_texto->isChecked())
		cfg_param << "DISABLEDWM";
	if (ui->chkDat_compatibilidad_ejecutar_admin->isChecked())
		cfg_param << "RUNASADMIN";

	path_exe = path_exe.replace("/", "\\");

	cfg_compatibilidad = "";
	cfg_compatibilidad.append(cfg_param.join(" "));
	if (cfg_compatibilidad.isEmpty())
		borrar_reg_compatibilidad = true;

	// Abrimos la llave del registro.
	LPCWSTR lpSubKey = TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");

	HKEY hKey;
	LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_SET_VALUE, &hKey); // KEY_ALL_ACCESS
	if (openRes == ERROR_SUCCESS)
	{
		qDebug() << "Correcto al Abrir el Registro.";

		LPCWSTR value = path_exe.toStdWString().c_str();

		if (borrar_reg_compatibilidad)
		{
		// Eliminamos el registro.
			LONG delRes = RegDeleteValue(hKey, value);
			if (delRes != ERROR_SUCCESS)
				qDebug() << "Error al Eliminar el Registro.";
			else
				qDebug() << "Correcto al Eliminar el Registro.";
		} else {
		// Guardamos el registro.
			QByteArray regValueBuff = QByteArray((const char *)cfg_compatibilidad.utf16(), (cfg_compatibilidad.length() + 1) * 2);

			LONG setRes = RegSetValueEx(hKey, value, 0, REG_SZ, reinterpret_cast<const unsigned char *>(regValueBuff.constData()), regValueBuff.size());
			if (setRes == ERROR_SUCCESS)
				qDebug() << "Correcto al Escribir el Registro.";
			else
				qDebug() << "Error al Escribir el Registro.";
		}
	} else
		qDebug() << "Error al Abrir el Registro.";

	// Cerramos la llave del registro.
	LONG closeOut = RegCloseKey(hKey);
	if (closeOut == ERROR_SUCCESS)
		qDebug() << "Correcto al Cerrar la Llave.";
	else
		qDebug() << "Error al Cerrar la Llave.";

	cfg_compatibilidad = fGrl->boolToStr(ui->gBox_compatibilidad_modo->isChecked()) +"|"+ cfg_compatibilidad;
#else
	QMessageBox::information(this, tr("Información"), tr("La ejecucion de compatibilidad no esta disponible para sistemas operativos que no sean Windows."));
#endif

	QDialog::accept();
}

void frmCompatibilidadExe::on_btnCancel_clicked()
{
	QDialog::reject();
}

void frmCompatibilidadExe::on_chkDat_compatibilidad_color_reducido_clicked(bool checked)
{
	ui->cbxDat_compatibilidad_color_reducido->setEnabled(checked);
}
