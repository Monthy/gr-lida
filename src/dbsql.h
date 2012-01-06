/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#ifndef DBSQL_H
#define DBSQL_H

#include <QtCore>
#include <QtGui>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlResult>

#include "funciones.h"

class dbSql
{
public:
	dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, QString db_port);
	~dbSql();

	bool dbisOpen();// Retorna si esta abierta o no la base de datos.
	void dbClose();	// Cierra la base de datos.

	int getCount(QString stTable, QString stWhere = "");
	QString ItemIDIndex(QString SQLtabla, QString SQLindex, QString column = "*");
// Elimina un juego de la base de datos.
	void ItemEliminar(const QString IDgrl);
// Crea las tablas si no las tiene.
	void CrearTablas();
// Datos Juego.
	QHash<QString, QString> show_Datos(QString IDgrl);
	QString ItemInsertaDatos(const QHash<QString, QString> datos);
	void ItemActualizaDatos(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaDatosItem(const QString m_key, const QString m_value, const QString IDgrl);
	void ItemActualizaDatosRating(const QString new_rating , const QString IDgrl );
	void ItemActualizaDatosFavorito(const QString EstadoFav , const QString IDgrl );
// DOSBox.
	QHash<QString, QString> showConfg_DOSBox(QString IDgrl);
	QString ItemInsertaDbx(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaDbx(const QHash<QString, QString> datos, const QString IDdbx);
	void ItemInsertaMontajesDbx(QTreeWidget *treeWidget, const QString IDdbx);
	QString ItemInsertaUnMontajeDbx(const QHash<QString, QString> datos, const QString IDdbx);
	void ItemActualizaMontajeDbx(QTreeWidget *treeWidget);
	void ItemEliminarMontaje( const QString IDmountdbx );
// ScummVM.
	QHash<QString, QString> showConfg_ScummVM(QString IDgrl);
	void ItemInsertaSvm(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaSvm(const QHash<QString, QString> datos, const QString IDsvm);
// VDMSound.
	QHash<QString, QString> showConfg_VDMSound(QString IDgrl);
	void ItemInsertaVdms(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaVdms(const QHash<QString, QString> datos, const QString IDvdms);
// Archivos.
	void ItemInsertaFiles(QTreeWidget *treeWidget, const QString IDgrl);
	QString ItemInsertaUnFiles(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaFiles(const QHash<QString, QString> datos, const QString IDFiles);
	void ItemEliminarFiles( const QString IDFiles );
// URL.
	void ItemInsertaURL(QTreeWidget *treeWidget, const QString IDgrl);
	QString ItemInsertaUnURL(const QHash<QString, QString> datos, const QString IDgrl);
	void ItemActualizaURL(const QHash<QString, QString> datos, const QString IDURL);
	void ItemEliminarURL( const QString IDURL );

private:
	Funciones fGrl;
	QSqlDatabase sqldb;
	bool ok_OpenDB;
	QString dbType;

	bool Chequear_Query(QSqlQuery q);
	void eliminararchivo(QString archivo);
};

#endif
