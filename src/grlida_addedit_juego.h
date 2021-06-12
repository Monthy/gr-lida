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

#ifndef GRLIDA_ADDEDIT_JUEGO_H
#define GRLIDA_ADDEDIT_JUEGO_H

#include <QDialog>
#include <QTextEdit>
#include <QCloseEvent>

#include "funciones.h"
#include "dbsql.h"
#include "grlida_addedit_dosbox.h"
#include "grlida_addedit_scummvm.h"
#include "grlida_addedit_vdmsound.h"
#include "grlida_importar_juego.h"
#include "grlida_multimedia.h"
#include "grlida_pdf_viewer.h"
#include "grlida_addedit_virtual_drive.h"
#include "grdap.h"
#include "qcheckcombobox.h"
#include "editorwidget/editorwidget.h"

namespace Ui {
	class frmAddEditJuego;
}

class frmAddEditJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmAddEditJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, QString tipo_emu, bool m_editando, QWidget *parent = 0);
	~frmAddEditJuego();

	stDatosJuego getDatosJuegos(){return DatosJuego;}
	stGrlCfg getGrlCfg(){return grlCfg;}

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmAddEditJuego *ui;

	enum tab_datos {
		tabDatos        = 0,
		tabDescripcion  = 1,
		tabMultimedia   = 2,
		tabOtrosDatos   = 3,
		tabEjecutabeRom = 4,
		tabDOSBox       = 5,
		tabScummVM      = 6,
		tabVDMSound     = 7
	};

	enum tipoImagen {
		t_thumb        = 0,
		t_cover_front  = 1,
		t_cover_back   = 2,
		t_cover_left   = 3,
		t_cover_right  = 4,
		t_cover_top    = 5,
		t_cover_bottom = 6,
		t_imagenes     = 7,
		t_capturas     = 8
	};

	Funciones *fGrl;
	dbSql *sql;
	EditorWidget *txtDat_comentario;
	GrlMultiMedia *mediaVideo, *mediaSound;
	GrDap *grdap;
	frmPdfViewer *pdfViewer;

	frmAddEditDosBox *wDbx;
	frmAddEditScummVM *wSvm;
	frmAddEditVDMSound *wVdms;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;
//	stConfigDOSBox DatosDosBox;
//	stConfigScummVM DatosScummVM;
//	stConfigVDMSound DatosVDMSound;
	QList<stDatosImagenes> listImagenesAdd;
	QList<stDatosImagenes> listImagenesImportadas;
	QStringList listVideosAdd, listSonidosAdd;

	QString IdGame, TipoEmu/*, TituloGuiones*/, oldTitulo, oldGameDir, oldThumbs;
//	QString oldPathCapturas;
	QString Thumbs, CoverFront, CoverBack, CoverLeft, CoverRight, CoverTop, CoverBottom;
	QString file_thumbs, file_cover_front, file_cover_back, file_cover_left, file_cover_right, file_cover_top, file_cover_bottom;
	QStringList dap_ext, comic_ext, z_ext;
	bool Editando, isOpenGrDap, isOpenPdf;
	int idx_virtual_drive;

	QCheckComboBox *cbxDat_genero;
	QCheckComboBox *cbxDat_compania;
	QCheckComboBox *cbxDat_desarrollador;
	QCheckComboBox *cbxDat_tema;
	QCheckComboBox *cbxDat_grupo;
	QCheckComboBox *cbxDat_idioma;
	QCheckComboBox *cbxDat_idioma_voces;
	QCheckComboBox *cbxDat_formato;
	QCheckComboBox *cbxDat_perspectiva;
	QCheckComboBox *cbxDat_sistemaop;

	void createWidgets();
	void cargarConfig();
	void setTheme();

	void cargarDatosJuego(stDatosJuego datos, bool isImport = false);
	bool setDatosJuegos(bool isSoloDatos = false);

	void asignarImagen(stFileInfo f_info, tipoImagen tipo);

	void enabledUrlUpdate(QString texto, int col);
	void addEditTwDatosURL(bool isNew);

	void enabledDatosUpdate(QString texto, int col);
	void addEditTwDatosFiles(bool isNew);

	void cargarParametrosTwList(QTreeWidget *twList, QString parametros);
	QString getParametrosTwList(QTreeWidget *twList);

	void cargarVirtualDriveList();

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnDescargarInfo_clicked();
	void on_cbxDat_tipo_emu_activated(int index);
	void on_btnDat_grupo_add_clicked();
// Thumbs
	void on_btnDat_thumbs_crear_clicked();
	void on_btnDat_thumbs_abrir_clicked();
	void on_btnDat_thumbs_ver_clicked();
	void on_btnDat_thumbs_eliminar_clicked();
// CoverFront
	void on_btnDat_thumbs_crear__clicked();
	void on_btnDat_cover_front_abrir_clicked();
	void on_btnDat_cover_front_ver_clicked();
	void on_btnDat_cover_front_eliminar_clicked();
// CoverBack
	void on_btnDat_cover_back_abrir_clicked();
	void on_btnDat_cover_back_ver_clicked();
	void on_btnDat_cover_back_eliminar_clicked();
// CoverLeft
	void on_btnDat_cover_left_abrir_clicked();
	void on_btnDat_cover_left_ver_clicked();
	void on_btnDat_cover_left_eliminar_clicked();
// CoverRight
	void on_btnDat_cover_right_abrir_clicked();
	void on_btnDat_cover_right_ver_clicked();
	void on_btnDat_cover_right_eliminar_clicked();
// CoverTop
	void on_btnDat_cover_top_abrir_clicked();
	void on_btnDat_cover_top_ver_clicked();
	void on_btnDat_cover_top_eliminar_clicked();
// CoverBottom
	void on_btnDat_cover_bottom_abrir_clicked();
	void on_btnDat_cover_bottom_ver_clicked();
	void on_btnDat_cover_bottom_eliminar_clicked();
// Multimedia
	void on_lwImagenes_itemDoubleClicked(QListWidgetItem *item);
	void on_btnDat_imagenes_add_clicked();
	void on_btnDat_imagenes_eliminar_clicked();
	void on_btnDat_imagenes_asignar_clicked();

	void on_lwCapturas_itemDoubleClicked(QListWidgetItem *item);
	void on_btnDat_capturas_add_clicked();
	void on_btnDat_capturas_eliminar_clicked();

	void on_btnDat_videos_add_clicked();
	void on_btnDat_videos_eliminar_clicked();

	void on_btnDat_sonidos_add_clicked();
	void on_btnDat_sonidos_eliminar_clicked();

	void changeMediaConfig();
// Otros datos
	void on_btnDat_path_exe_clicked();
	void on_btnDat_path_exe_clear_clicked();
	void on_btnDat_path_exe_compatibilidad_clicked();

	void on_txtDat_parametros_exe_textEdited(const QString &arg1);
	void on_txtDat_parametros_exe_valor_textEdited(const QString &arg1);
	void on_btnDat_parametros_exe_add_clicked();
	void on_btnDat_parametros_exe_update_clicked();
	void on_btnDat_parametros_exe_eliminar_clicked();
	void on_btnDat_parametros_exe_clear_clicked();
	void on_twDatosParametrosExe_itemClicked(QTreeWidgetItem *item, int column);

	void on_btnDat_path_setup_clicked();
	void on_btnDat_path_setup_clear_clicked();
	void on_btnDat_path_setup_compatibilidad_clicked();

	void on_txtDat_parametros_setup_textEdited(const QString &arg1);
	void on_txtDat_parametros_setup_valor_textEdited(const QString &arg1);
	void on_btnDat_parametros_setup_add_clicked();
	void on_btnDat_parametros_setup_update_clicked();
	void on_btnDat_parametros_setup_eliminar_clicked();
	void on_btnDat_parametros_setup_clear_clicked();
	void on_twDatosParametrosSetup_itemClicked(QTreeWidgetItem *item, int column);
// Montar Imagen en unidad virtual.
	void on_btnDat_path_image_clicked();
	void on_btnDat_path_image_clear_clicked();
	void on_btn_virtual_drive_edit_clicked();
// Direcciones url
	void on_cbxUrl_url_editTextChanged(const QString &arg1);
	void on_txtUrl_descripcion_textChanged();
	void on_cbxUrl_url_activated(int index);
	void on_btnUrl_add_clicked();
	void on_btnUrl_update_clicked();
	void on_btnUrl_abrir_clicked();
	void on_btnUrl_delete_clicked();
	void on_twDatosURL_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column);
// Datos usuario
	void on_btnDat_usuario_clear_clicked();
// Datos archivos
	void on_txtFile_path_textEdited(const QString &arg1);
	void on_txtFile_nombre_textEdited(const QString &arg1);
	void on_txtFile_descripcion_textChanged();
	void on_btnFile_path_clicked();
	void on_btnFile_path_clear_clicked();
	void on_btnFile_add_clicked();
	void on_btnFile_update_clicked();
	void on_btnFile_delete_clicked();
	void on_twDatosFiles_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatosFiles_itemDoubleClicked(QTreeWidgetItem *item, int column);

	void on_txtDat_titulo_editingFinished();

};

#endif // GRLIDA_ADDEDIT_JUEGO_H
