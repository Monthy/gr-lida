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

#ifndef DBSQL_H
#define DBSQL_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>
//#include <QSqlTableModel>

//#include <QList>
//#include <QString>
//#include <QStringList>
#include <QTreeWidget>
#include <QHash>

#include "g_structs.h"

class dbSql : public QObject
{
	Q_OBJECT

public:
	dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, int db_port, QString dirApp, QString lng, QString theme);
	~dbSql();

	QSqlDatabase getSqlDB(){return sqldb;}

	void comit();

	void setTheme(QString theme);
	void setIdioma(QString lng);
// Devuelve si el archivos admiten traducción
	QString getArchivoIsLng(QString archivo);
// Devuelve si el archivos es de 3 columnas
	bool getArchivoIs3Col(QString archivo);
// Convierte bytes
	QString covertir_bytes(double size);
	QString removeAccents(QString str, QString sep = "_", bool to_lower = true);
// Elimina etiquetas de html
	QString removeHtmlTag(QString str);
// Devuelve el tiempo actual, formato unix
	QString getTime();
// Convierte fecha 'dd/MM/yyyy HH:mm:ss' a formato unix
	QString setTime(QString fecha, QString formato = "dd/MM/yyyy HH:mm:ss");

// Cierra la base de datos
	bool close();
// Retorna si esta abierta o no la base de datos
	bool dbisOpen();
// Muestra una ventana de información
	void ventanaInfo(QString titulo, QString icono, int w, int h, QString contenido, bool isTextoPlano = true);
// Comprueba si el Query se ha ejecutado bien
	bool chequearQuery(QSqlQuery &query);
	bool eliminarItemTable(QString tabla, QString Id);

// Lista las tablas de la base de datos
	QStringList listarTablas();
// Cuenta el número de registros que tiene
	int getCount(QString tabla, QString sql_where = "", QString sql_col = "", QString col_value = "");
// Devuelve los datos correspondientes a la tabla, id y columnas indicadas
	QHash<QString, QString> getDatos(QString tabla, QString sql_where, QString sql_col, QString sep = ", ");
// Actualiza el orden de un QTreeWidget y en la base de datos
	void actualizaOrdenTreeWidget(QTreeWidget *twOrden, QString tabla, QString col_id, QString col_orden, int coltw_id, int coltw_order);


// Comprueba si existe la columna de una tabla
	bool comprobarColumnaTabla(QString tabla, QString columna);
// Renombrar una tabla
	bool renombrarTabla(QString old_name, QString new_name);
// Eliminamos una tabla
	bool eliminarTabla(QString tabla);
// Crea una tabla nueva para Datos
	bool crearTablaDatos(QString tabla);
// Crea la tabla dbgrl_mnu_nav
	bool crearTablaMnuNav(bool isTemp = false);
// Crea las tablas de la base de datos si es necesario
	void crearTablas();

// Datos Juego
	QString setDefDatValue(bool isQHash, QString value, QString def = "");
	stDatosJuego getDefectDatos(QString icono = "datos", QHash<QString, QString> dato = (QHash<QString, QString>()));
	stDatosJuego show_Datos(QString tabla, QString IDgrl);
	QString insertaDatos(QString tabla, stDatosJuego datos);
	bool actualizaDatos(QString tabla, stDatosJuego datos);
	bool actualizaDatosItem(QString tabla, QString IDgrl, QString m_key, QString m_value, QString m_where = "idgrl");
// DOSBox
	stConfigDOSBox getDefectDOSBox(QHash<QString, QString> dato = (QHash<QString, QString>()));
	stConfigDOSBox showConfg_DOSBox(QString IDgrl, QString IDcat);
	stConfigDOSBox showConfg_DOSBox(QString IDdosbox);

	QString insertaDbx(stConfigDOSBox cfgDbx);
	bool actualizaDbx(stConfigDOSBox cfgDbx);
	QList<stConfigDOSBoxMount> showConfg_DOSBoxMount(QString IDdosbox);
	QString insertaUnMontajeDbx(stConfigDOSBoxMount cfgDbxMount);
	void insertaMontajesDbx(QList<stConfigDOSBoxMount> listMount, QString IDdbx);
	bool actualizaMontajeDbx(stConfigDOSBoxMount cfgDbxMount);
	bool eliminarMontajeDbx(QString IDmountdbx);
// ScummVM
	stConfigScummVM getDefectScummVM(QHash<QString, QString> dato = (QHash<QString, QString>()));
	stConfigScummVM showConfg_ScummVM(QString IDgrl, QString IDcat);
	stConfigScummVM showConfg_ScummVM(QString IDscummvm);
	QString insertaSvm(stConfigScummVM cfgSvm);
	bool actualizaSvm(stConfigScummVM cfgSvm);
// VDMSound
	stConfigVDMSound getDefectVDMSound(QHash<QString, QString> dato = (QHash<QString, QString>()));
	stConfigVDMSound showConfg_VDMSound(QString IDgrl, QString IDcat);
	QString insertaVdms(stConfigVDMSound cfgVdms);
	bool actualizaVdms(stConfigVDMSound cfgVdms);
// Archivos
	void cargarDatosFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat, QString sql_where = "");
	QString insertaUnFiles(stDatosFiles datos);
	void insertaFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat);
	bool actualizaFiles(stDatosFiles datos);
	bool eliminarFiles(QString IDFiles);
// URL
//	QList<QString> getDatosUrls(QString IDgrl, QString IDcat, QString sql_where = "");
	void cargarDatosUrls(QTreeWidget *twUrls, QString IDgrl, QString IDcat, QString sql_where = "");
	QString insertaUnURL(stDatosUrls datos);
	void insertaURL(QTreeWidget *twUrls, QString IDgrl, QString IDcat);
	bool actualizaURL(stDatosUrls datos);
	bool eliminarURL(QString IDUrl);
// Categorías
	QList<stGrlCats> getCategorias(QString sql_where = "");
	void cargarCategorias(QTreeWidget *twCat);
	QString insertaCategoria(stGrlCats cat);
	bool actualizaCategoria(stGrlCats cat);
	bool eliminarCategoria(QString IDcat);
// MenuNav
	void cargarMenuNav(QTreeWidget *twMnuNav, QString tabla, bool isOpt = false);
	QString insertaMenuNav(QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool mostrar, bool expanded);
	bool actualizaMenuNav(QString IDmnu, QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool mostrar, bool expanded);
	bool eliminarMenuNav(QString IDmnu);
// MnuShortcut
	QString insertaMnuShortcut(QString titulo, QString key_sequence, QString sql_query, QString img, int orden, bool mostrar, bool separador);
	bool actualizaMnuShortcut(QString IDmnu, QString titulo, QString key_sequence, QString sql_query, QString img, int orden, bool mostrar, bool separador);
	bool eliminarMnuShortcut(QString IDmnu);

private:
// Retorna el último ID insertado
	QString lastInsertId(QSqlQuery &query, QString sql_col = "");
	QString qpsql_pkey(QString tabla);
	QString qpsql_return_id(QString sql_col);
// Convierte texto "true", "yes", "1" a tipo bool
	bool strToBool(QString str);
// Convierte bool a texto "true", "yes" indicando el tipo
	QString boolToStr(bool estado, bool type_yes = false);

	stConfigDOSBox showConfg_DOSBox_SqlQuery(QString sql_query);
	stConfigScummVM showConfg_ScummVM_SqlQuery(QString sql_query);

	QSqlDatabase sqldb;
	bool isOpenDb;
	QString dbType, dbTypeId;
	QString qpsql_oids, qpsql_text;
	QString stDirApp, stLngSelect, stTheme;

};

#endif // DBSQL_H
