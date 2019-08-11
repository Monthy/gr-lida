#ifndef GRLIDA_IMPORTAR_DATOS_H
#define GRLIDA_IMPORTAR_DATOS_H

#include <QWidget>

#include "funciones.h"

#include "propertyeditor/propertyeditor.h"
#include "propertyeditor/propertyheader.h"
#include "propertyeditor/boolproperty.h"
#include "propertyeditor/comboboxcheckproperty.h"
#include "propertyeditor/comboboxproperty.h"
#include "propertyeditor/filedialogproperty.h"
#include "propertyeditor/integerproperty.h"
#include "propertyeditor/stringproperty.h"

namespace Ui {
class frmImportarDatos;
}

class frmImportarDatos : public QWidget
{
	Q_OBJECT

public:
	explicit frmImportarDatos(stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, bool m_editando, QWidget *parent = 0);
	~frmImportarDatos();

	void setEditorDatos(stDatosJuego cfgDat, stDatosJuego dat_def);
	stDatosJuego getEditorDatos() { return DatosJuego; }

signals:
	void cfgDatChanged(stDatosJuego cfgDat);

private:
	Ui::frmImportarDatos *ui;

	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;

//	QStringList icon_title_list;
	QFileInfoList icon_list;
	bool Editando, isLoad;

// Property Editor Datos
	StringProperty        *pdat_titulo, *pdat_subtitulo, *pdat_tamano, *pdat_comentario;
	StringProperty        *pdat_usuario, *pdat_parametros_exe, *pdat_parametros_setup;
	StringProperty        *pdat_estado;
	BoolProperty          *pdat_solo_datos, *pdat_original, *pdat_favorito, *pdat_gamepad;
	ComboBoxProperty      *pdat_icono, *pdat_anno, *pdat_numdisc, *pdat_graficos;
	ComboBoxProperty      *pdat_sonido, *pdat_jugabilidad;
	ComboBoxProperty      *pdat_rating, *pdat_edad_recomendada, *pdat_tipo_emu;
	ComboBoxCheckProperty *pdat_genero, *pdat_compania, *pdat_desarrollador, *pdat_tema;
	ComboBoxCheckProperty *pdat_grupo, *pdat_perspectiva, *pdat_idioma, *pdat_idioma_voces;
	ComboBoxCheckProperty *pdat_formato, *pdat_sistemaop;
	FileDialogProperty    *pdat_thumbs, *pdat_cover_front, *pdat_cover_back, *pdat_path_exe;
	FileDialogProperty    *pdat_cover_left, *pdat_cover_right, *pdat_cover_top, *pdat_cover_bottom;
	FileDialogProperty    *pdat_path_setup;

	void cargarConfig();
	void cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col = 2, QString sep = "|", bool isClear = true, bool keyVisible = false);
	void cargarEditorDatos();

private slots:
	void onValueDatosChanged(PropertyItem *prop, const QVariant &value);
	void on_twDatos_itemChanged(QTreeWidgetItem *item, int column);

};

#endif // GRLIDA_IMPORTAR_DATOS_H
