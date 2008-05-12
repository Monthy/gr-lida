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
#include <QDir>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QSqlQuery>
#include <QSqlRecord>
#include "funciones.h"
#include "dbsql.h"
#include "ui_addedit_juego.h"

class frmAddEditJuego : public QDialog {
    Q_OBJECT
public:
	frmAddEditJuego( bool EditJuego, QString TipoEmu, QString stIDIndex, QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmAddEditJuego();

	Ui::AddEditJuegoClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosScummvm;
	QHash<QString, QString> DatosDosBox;
	QHash<QString, QString> DatosVDMSound;

	QString stItemIDSvm;
	QString stItemIDDbx;
	QString stItemIDVdms;

private:
	Funciones fGrl;
	dbSql *sql;

	bool EditandoJuego;
	QString TipoEmulador;
	QString stItemIDGrl;
	QString stTituloAddEdit(){ return windowTitle(); }
	QString stHomeDir, stIconDir, stDatosDir;
	QStringList filters;
	QStringList svm_Lista, svm_ListaTemp, smiles_Lista, smiles_ListaTemp;
	QFile file_thumbs, file_cover_front, file_cover_back;
	QString stThumbs, stCoverFront, stCoverBack;
	QHash<QString, QString> listSmailes;
	//QHash<QString, QString>::const_iterator i_Hash;
	QUrl my_url;
	//QHash<QString, QString>  TempProfileDosBox;

private slots:
	void on_btnOk();
	void on_cbxDatos_TipoEmu_txtChanged(const QString texto);
// Referente al DatosJuego ---------------------------------------
	void CargarDatosJuego( QString stIDIndex );
	void setDatosJuegos();
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
	void on_btnTool_Buscar();
	void on_btnTool_BuscarAnterior();
	void on_btnTool_BuscarSiguiente();
	void on_btnTool_Reemplazar();
	void on_btnTool_Preview();
	void on_btnNuevaUrl();
	void on_btnEditarUrl();
	void on_btnEliminarUrl();
	void on_btnAbrirUrl();
	void on_btnDatosFiles_PathFile();
	void on_btnAddFile();
	void on_btnEditFile();
	void on_btnUpdateFile();
	void on_btnDeleteFile();

// ---------------------------------------------------------------
// Referente al ScummVM ------------------------------------------
	void CargarDatosScummVM( QString stIDsvm );
	void setDatosScummVM();
	void on_twScummVM_clicked( QTreeWidgetItem *item );
	void on_btnDefecto();
	void on_btnDirSvm_path();
	void on_btnDirSvm_save();
	void on_btnDirSvm_extra();
	void on_btnDirSvm_capturas();
	void on_btnDirSvm_sonido();
	void on_btnDirSvm_sound_font();

// ---------------------------------------------------------------
// Referente al DOSBox -------------------------------------------
	void CargarDatosDosBox(QString stIDdbx, const QString ProfileGame = "", bool isProfileGame = false);
	void CargarDatosDBxMontaje( QString stIDdbx );
	void setDatosDosBox();
	QString setOpcionesSerial();
	void on_setProfileGame(const QString ProfileGame);
	void on_btnDbx_FileConfg();
	void on_btnDbx_ExeJuego();
	void on_btnDbx_ExeSetup();
	void on_btnDirGravisUltraSound();
	void on_btnDbx_mapperfile();
	void on_btnDbx_language();
	void on_btnDbx_capturas();
	void on_btnDbx_musica();
	void on_btnDbx_AddSerial();
	void on_btnMount_Add();
	void on_btnMount_Edit();
	void on_btnMount_Delete();
	void on_btnMount_Clear();
	void on_btnMount_Subir();
	void on_btnMount_Bajar();
	void on_btnMount_AutoCrear();
	void on_btnMount_Primario();

// ---------------------------------------------------------------
// Referente al VDMSound -----------------------------------------
	void CargarDatosVDMSound( QString stIDvdms );
	void setDatosVDMSound();
	void on_btnVdms_FileConfg();
	void on_btnVdms_ExeJuego();
	void on_btnVdms_Icono();
// ---------------------------------------------------------------
};

#endif // GRLIDA_ADDEDIT_JUEGO_H
