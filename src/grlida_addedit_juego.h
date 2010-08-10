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

#ifndef GRLIDA_ADDEDIT_JUEGO_H
#define GRLIDA_ADDEDIT_JUEGO_H

#include <QtCore>
#include <QtGui>
#include <QTextEdit>

#include "funciones.h"
#include "dbsql.h"
#include "ui_addedit_juego.h"

class frmAddEditJuego : public QDialog
{
    Q_OBJECT

public:
	frmAddEditJuego(bool EditJuego, QString TipoEmu, QString stIDIndex, QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmAddEditJuego();

	Ui::AddEditJuegoClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosScummVM;
	QHash<QString, QString> DatosDosBox;
	QHash<QString, QString> DatosVDMSound;

protected:
	void closeEvent( QCloseEvent *e );

private:
	QHash<QString, QVariant> GRLConfig;

	Funciones fGrl;
	dbSql *sql;

	bool EditandoJuego;

	QString stTituloAddEdit(){ return windowTitle(); }
	QString stItemIDGrl, TipoEmulador;
	QString stHomeDir, stIconDir, stDatosDir, stTheme, stIdioma, stTempDir, stUsuario;

	QString stCoversDir, stThumbsDir, stListThumbsDir, stCapturesDir;
	QString stThumbs, stCoverFront, stCoverBack;
	QString file_thumbs, file_cover_front, file_cover_back;

	QStringList filters;
	QHash<QString, QString> listSmailes;

	QUrl my_url;

	void createConnections();
	void setTheme();
	void CargarConfig();

// Referente al DatosJuego ---------------------------------------
	void CargarDatosJuego(QString stIDIndex);
	void setDatosJuegos();

private slots:
	void on_btnOk();
	void on_cbxDatos_TipoEmu_Changed(int row);

// Referente al DatosJuego ---------------------------------------
	void on_twDatoSmile_Dblclicked( QTreeWidgetItem *item);
	void on_btnImgAbrir_Thumbs();
	void on_btnImgAbrir_CoverFront();
	void on_btnImgAbrir_CoverBack();
	void on_btnImgVer_Thumbs();
	void on_btnImgVer_CoverFront();
	void on_btnImgVer_CoverBack();
	void on_btnImgEliminar_Thumbs();
	void on_btnImgEliminar_CoverFront();
	void on_btnImgEliminar_CoverBack();
	void on_btnDescargarInfo();
	void on_btnTool_Cortar();
	void on_btnTool_Copiar();
	void on_btnTool_Pegar();
	void on_btnTool_SelectAll();
	void on_btnTool_Deshacer();
	void on_btnTool_Rehacer();
	void on_btnTool_TextoNegrita();
	void on_btnTool_TextoCursiva();
	void on_btnTool_TextoSubrayado();
	void on_btnTool_InsertarImg();
	void on_btnTool_InsertaUrl();
	void on_btnTool_Buscar(bool estado);
	void on_btnTool_Reemplazar(bool estado);
	void on_btnTool_ReemplazarTexto();
	void on_btnTool_BuscarAnterior();
	void on_btnTool_BuscarSiguiente();
	void on_btnTool_Preview();
	void on_btnNuevaUrl();
	void on_btnEditarUrl();
	void on_btnEliminarUrl();
	void on_btnAbrirUrl();
	void on_btnDatos_ExeJuego();
	void on_btnDatosFiles_PathFile();
	void on_btnAddFile();
	void on_btnEditFile();
	void on_btnUpdateFile();
	void on_btnDeleteFile();
	void on_twDatosFiles_Dblclicked(QTreeWidgetItem *item);

	void findText(QString text, bool m_anterior = false);
};

#endif // GRLIDA_ADDEDIT_JUEGO_H
