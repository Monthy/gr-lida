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

#ifndef DBSQL_H
#define DBSQL_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>

#include <QList>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QHash>

#include "funciones.h"

class dbSql : public QObject
{
	Q_OBJECT

public:
	dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, QString db_port, QString lng, QString theme);
	~dbSql();

	void setTheme(QString theme);
	void setIdioma(QString lng);

	QSqlDatabase getSqlDB(){return sqldb;}
	bool close();

// Comprueba si el Query se ha ejecutado bien.
	bool chequearQuery(QSqlQuery &query);

// Retorna si esta abierta o no la base de datos.
	bool dbisOpen();
// Lista las Tablas de la Base de Datos
	QStringList listarTablas();
// Cuenta el número de registros que tiene.
	int getCount(QString tabla, QString sql_where = "", QString sql_col = "", QString col_value = "");
//	int getCount(QString tabla, QString sql_where = "", QString sql_col = "");
// Devuelve los datos correspondientes a la tabla, id y columnas indicadas.
	QHash<QString, QString> getDatos(QString tabla, QString sql_where, QString sql_col, QString sep = ", ");

// Renombrar una Tabla
	bool renombrarTabla(QString old_name, QString new_name);
// Crea una Tabla nueva
	bool crearTabla(QString tabla);
// Crea las tablas si no las tiene.
	void crearTablas();

// Datos Juego.
	stDatosJuego show_Datos(QString tabla, QString IDgrl);
	QString insertaDatos(QString tabla, stDatosJuego datos);
	bool actualizaDatos(QString tabla, stDatosJuego datos);
	bool actualizaDatosItem(QString tabla, QString IDgrl, QString m_key, QString m_value);
	bool actualizaDatosRating(QString tabla, QString IDgrl, QString new_rating);
	bool actualizaDatosFavorito(QString tabla, QString IDgrl, QString estadoFav);
	bool eliminarDatos(QString IDgrl, QString IDcat);
// DOSBox.
	stConfigDOSBox showConfg_DOSBox(QString IDgrl, QString IDcat);
	QString insertaDbx(stConfigDOSBox cfgDbx);
	bool actualizaDbx(stConfigDOSBox cfgDbx);
	void insertaMontajesDbx(QTreeWidget *twDbx, QString IDdbx);
	QString insertaUnMontajeDbx(stConfigDOSBoxMount cfgDbxMount);
	void actualizaMontajeDbx(QTreeWidget *twDbx);
	bool eliminarMontaje(QString IDmountdbx);
// ScummVM.
	stConfigScummVM showConfg_ScummVM(QString IDgrl, QString IDcat);
	QString insertaSvm(stConfigScummVM cfgSvm);
	bool actualizaSvm(stConfigScummVM cfgSvm);
// VDMSound.
	stConfigVDMSound showConfg_VDMSound(QString IDgrl, QString IDcat);
	QString insertaVdms(stConfigVDMSound cfgVdms);
	bool actualizaVdms(stConfigVDMSound cfgVdms);
// Archivos.
	void cargarDatosFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat, QString sql_where = "");
	void insertaFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat);
	QString insertaUnFiles(stDatosFiles datos);
	bool actualizaFiles(stDatosFiles datos);
	bool eliminarFiles(QString IDFiles);
// URL.
	QList<QString> getDatosUrls(QString IDgrl, QString IDcat, QString sql_where = "");
	void cargarDatosUrls(QTreeWidget *twUrls, QString IDgrl, QString IDcat, QString sql_where = "");
	void insertaURL(QTreeWidget *twUrls, QString IDgrl, QString IDcat);
	QString insertaUnURL(stDatosUrls datos);
	bool actualizaURL(stDatosUrls datos);
	bool eliminarURL(QString IDUrl);
// Categorías
	QList<stGrlCats> getCategorias(QString sql_where = "");
	QHash<int, stGrlCats> getCategoriasLista(QString sql_where = "");
	void cargarCategorias(QTreeWidget *twCat);
	QString insertaCategoria(QString tabla, QString titulo, QString img, int orden, QString emu_show = "all");
	bool actualizaCategoria(QString IDcat, QString tabla, QString titulo, QString img, QString emu_show);
	void actualizaOrdenCategoria(QTreeWidget *twCat);
	bool eliminarCategoria(QString IDcat);
// MenuNav
	void cargarMenuNav(QTreeWidget *twMnuNav, QString tabla, bool isOpt = false);
	QString insertaMenuNav(QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool show, bool expanded);
	void actualizaOrdenMenuNav(QTreeWidget *twMnuNav);
	bool actualizaMenuNav(QString IDmnu, QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool show, bool expanded);
	bool eliminarMenuNav(QString IDmnu);

private:
	QString lastInsertId(QSqlQuery &query, QString sql_col = "");

	Funciones *fGrl;

	QSqlDatabase sqldb;
	bool isOpenDb;
	QString dbType, dbTypeId;

};

#endif // DBSQL_H
