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

#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>

#include "grlida.h"
#include "grlida_config_inicial.h"

int main(int argc, char *argv[])
{
	Funciones fGrl;
	QTranslator translator;

	QApplication app(argc, argv);
	app.setApplicationName("GR-lida");

	stGrlDir grlDir;
	grlDir.Home = fGrl.GRlidaHomePath();

	QSplashScreen splash( QPixmap(":/images/splash.png") );
	splash.show();

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Cargando configuración..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

	stGrlCfg grlCfg = fGrl.cargarGRLConfig(grlDir.Home +"GR-lida.conf");

	if( grlCfg.Primeravez )
	{
		frmConfigInicial *ConfigInicial = new frmConfigInicial(grlCfg);
		if( ConfigInicial->exec() == QDialog::Accepted )
		{
			grlCfg = ConfigInicial->getGrlCfg();
			fGrl.guardarGRLConfig(grlDir.Home +"GR-lida.conf", grlCfg);
		}
		delete ConfigInicial;
	}

	fGrl.setIdioma(grlCfg.IdiomaSelect);
	if( !translator.load(grlDir.Home +"idiomas/gr-lida_"+ grlCfg.IdiomaSelect +".qm") )
		translator.load(":/idiomas/gr-lida_"+ fGrl.Idioma() +".qm");
	app.installTranslator(&translator);

// Crear directorio de la aplicacion si no existe
	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta principal"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	fGrl.comprobarDirectorio( grlDir.Home );

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta idiomas"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio idiomas si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"idiomas");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta iconos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Themes si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"iconos");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta themes"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio iconos si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"themes");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta smiles"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio smiles si no existe
	fGrl.comprobarDirectorio( grlDir.Home +"smiles" );

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta datos"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio datos si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"datos");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta scripts"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio scripts si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"scripts");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta templates"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio templates si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"templates");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta configuración del DOSBox"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confDbx si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"confdbx");

//	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta roms"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
//	app.processEvents();
//	fGrl.comprobarDirectorio(  );

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta DosGames"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
	fGrl.comprobarDirectorio(grlDir.Home +"DosGames");

#ifdef Q_OS_WIN32
	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta configuración del VDMSound"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio confVdms si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"confvdms");
#endif

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta thumbs"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Thumbs si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"thumbs");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta covers"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Covers si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"covers");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta capturas"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Capturas si no existe
	fGrl.comprobarDirectorio(grlDir.Home +"capturas");

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando carpeta temp"), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();
// Crear directorio Temp para las descargas temporales de imagenes, archivos
	fGrl.comprobarDirectorio(grlDir.Home +"temp");

// Crea una copia de seguridad de la Base de Datos
	fGrl.crearCopiaSeguridad_DB(grlDir.Home, grlCfg.Version);

	GrLida w;

	splash.showMessage(QObject::tr("Iniciando") +": "+ QObject::tr("Comprobando archivos de datos..."), Qt::AlignLeft | Qt::AlignBottom,  Qt::white);
	app.processEvents();

	w.show();
	splash.finish(&w);

	return app.exec();
}
