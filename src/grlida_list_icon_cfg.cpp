/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_list_icon_cfg.h"
#include "ui_list_icon_cfg.h"

frmListIconCfg::frmListIconCfg(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat, QString name_theme, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmListIconCfg)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	if( !name_theme.isEmpty() )
		grlCfg.NameDirTheme = name_theme;

	fGrl->setTheme(grlCfg.NameDirTheme);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	grlCfg    = m_cfg;
	sql       = m_sql;
	id_cat    = m_id_cat;

	grlDir.Home   = fGrl->GRlidaHomePath();
	grlDir.Datos  = grlDir.Home +"datos/";
	grlDir.Themes = grlDir.Home +"themes/";

	cargarConfig();
	cargarListaThemes();
	cargarListaCategorias();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmListIconCfg::~frmListIconCfg()
{
	delete grl_lv_delegate;
	delete lv_model;
	delete fGrl;
	delete ui;
}

void frmListIconCfg::cargarConfig()
{
	ui->btnIconMode->setChecked(true);
	emit on_btnIconMode_clicked(true);

	grl_lv_delegate = new GrlListViewDelegate(sql, grlDir.Home, fGrl->Theme(), fGrl->ThemeApp(), grlCfg.FormatsImage.join(";"), ui->lvJuegos);
	grl_lv_delegate->setDirApp( grlDir.Home );
	grl_lv_delegate->setTheme( fGrl->Theme() );
	grl_lv_delegate->setThemeCat( fGrl->ThemeApp() );

// Creamos el data model para el QListView
	lv_model = new QStandardItemModel( ui->lvJuegos );
// Configuración del lvJuegos
	ui->lvJuegos->setItemDelegate( grl_lv_delegate );
	ui->lvJuegos->setStyleSheet( fGrl->myStyleSheet(true) );

	lv_model->clear();
	for(int i= 0; i < 2; ++i)
	{
		QStandardItem *lvItem = new QStandardItem();
		lvItem->setData("null" , grl_lv_delegate->IdGrlRole );
		lvItem->setData("Game "+ fGrl->IntToStr(i), grl_lv_delegate->TituloRole );
		lvItem->setData(QPixmap(fGrl->ThemeApp() +"images/juego_sin_imagen.png"), grl_lv_delegate->IconRole );
		lvItem->setData("datos", grl_lv_delegate->TipoEmuRole );
		lvItem->setData(5      , grl_lv_delegate->RatingRole  );
		lv_model->setItem(i, 0, lvItem);

		QTreeWidgetItem *twItem = new QTreeWidgetItem( ui->twJuegos );
		twItem->setText( 0, "Game "+ fGrl->IntToStr(i) );
		twItem->setIcon( 0, QIcon(fGrl->ThemeApp() +"images/juego_sin_imagen.png") );
	}

	ui->lvJuegos->setIconSize( QSize(lwConf.icon_width, lwConf.icon_height) );
	ui->lvJuegos->setModel(lv_model);
	ui->lvJuegos->clearSelection();
	ui->lvJuegos->setCurrentIndex(lv_model->index(0, 0));

	ui->twJuegos->setIconSize( QSize(lwConf.tw_icon_width, lwConf.tw_icon_height) );
	ui->twJuegos->clearSelection();
	ui->twJuegos->setCurrentItem(ui->twJuegos->topLevelItem(0), 0);
}

void frmListIconCfg::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/style.png") );

// [tw_list_icon]
	ui->btn_tw_icon_width_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_tw_icon_height_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));

// [picflow_img]
	ui->btn_pf_img_width_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_pf_img_height_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));

// [list_icon]
	ui->btn_icon_width_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_icon_height_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_cover_top_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_cover_top_select_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_cover_top_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_cover_top_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_scaled_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_scale_w_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_scale_h_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_scale_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_img_scale_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_tipo_emu_show_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_tipo_emu_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_tipo_emu_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_rating_show_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_rating_vertical_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_rating_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_rating_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_bg_show_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_bg_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_bg_select_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_bg_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_bg_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_show_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_pos_x_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_pos_y_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_width_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_height_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_max_caracteres_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_size_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_color_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_color_select_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_bold_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_italic_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));
	ui->btn_title_font_underline_def->setIcon(QIcon(fGrl->Theme() +"img16/mp_aleatorio.png"));

	ui->btnIconMode->setIcon(QIcon(fGrl->Theme() +"img16/cover_mode.png"));
	ui->btnSave->setIcon(QIcon(fGrl->Theme() +"img16/floppy_2.png"));
	ui->btnSaveText->setIcon(QIcon(fGrl->Theme() +"img16/floppy_2.png"));
	ui->btnOk->setIcon(QIcon(fGrl->Theme() +"img16/aplicar.png"));

	ui->cbxListFiles->clear();
	ui->cbxListFiles->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "StyleSheet.qss"        , "StyleSheet.qss"        );
	ui->cbxListFiles->addItem(QIcon(fGrl->Theme() +"img16/style.png"), "StyleSheetList.qss"    , "StyleSheetList.qss"    );
	ui->cbxListFiles->addItem(QIcon(fGrl->Theme() +"img16/html.png") , "tpl_info.html"         , "tpl_info.html"         );
	ui->cbxListFiles->addItem(QIcon(fGrl->Theme() +"img16/html.png" ), "tpl_juego_info.html"   , "tpl_juego_info.html"   );
	ui->cbxListFiles->addItem(QIcon(fGrl->Theme() +"img16/html.png") , "tpl_juego_no_info.html", "tpl_juego_no_info.html");
}

void frmListIconCfg::cargarListaThemes()
{
	stGrlDatos dat;
	int id_theme  = 0;
	int num_theme = 1;
	QString autor, version;
	QDir dir( grlDir.Themes );
	QStringList list_themes = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);

	dat.key    = "defecto";
	dat.titulo = "Theme: defecto - Autor: Monthy - v0.11.0";
	dat.extra  = "";
	dat.icono  = "";

	themes.clear();
	themes.insert(id_theme, dat);

	ui->cbxThemes->clear();
	ui->cbxThemes->addItem(QIcon(fGrl->Theme() +"img16/style.png"), dat.titulo, dat.key);

	const int list_themesSize = list_themes.size();
	for (int i = 0; i < list_themesSize; ++i)
	{
		if( dir.exists(grlDir.Themes + list_themes[i]) && list_themes[i] != "defecto" )
		{
			QSettings settings(grlDir.Themes + list_themes[i] +"/info.ini", QSettings::IniFormat);
			settings.beginGroup("info");
				autor   = settings.value("autor"  , "-").toString();
				version = settings.value("version", "-").toString();
			settings.endGroup();

			dat.key    = list_themes[i];
			dat.titulo = "Theme: "+ dat.key +" - Autor: "+ autor +" - v"+ version;
			dat.extra  = "";
			dat.icono  = "";

			if( grlCfg.NameDirTheme == dat.key )
				id_theme = num_theme;

			themes.insert(num_theme, dat);
			ui->cbxThemes->addItem(QIcon(fGrl->Theme() +"img16/style.png"), dat.titulo, num_theme);
			++num_theme;
		}
	}

	ui->cbxThemes->setCurrentIndex(id_theme);
	emit on_cbxThemes_activated(id_theme);
}

// Carga las categorias de la base de datos.
void frmListIconCfg::cargarListaCategorias()
{
//setUpdatesEnabled( false );
	QSqlQuery query(sql->getSqlDB());
	stGrlCats cat;

	categoria.clear();
	ui->cbxCategorias->clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias ORDER BY orden ASC;");
	if( sql->chequearQuery(query) )
	{
		if( query.first() )
		{
			do {
				cat.id       = query.record().value("id").toString();
				cat.tabla    = query.record().value("tabla").toString();
				cat.titulo   = query.record().value("titulo").toString();
				cat.img      = query.record().value("img").toString();
				cat.orden    = query.record().value("orden").toString();
				cat.emu_show = query.record().value("emu_show").toString();
				categoria.insert(cat.id.toInt(), cat);

				QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
				if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
					ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
				else
					ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
			} while ( query.next() );
		} else {
			cat.tabla    = "dbgrl";
			cat.titulo   = "Db GR-lida";
			cat.img      = "pc.png";
			cat.orden    = "0";
			cat.emu_show = "all";
			cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
			categoria.insert(cat.id.toInt(), cat);

			QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
			if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
				ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
			else
				ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
		}
	} else {
		cat.tabla    = "dbgrl";
		cat.titulo   = "Db GR-lida";
		cat.img      = "pc.png";
		cat.orden    = "0";
		cat.emu_show = "all";
		cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
		categoria.insert(cat.id.toInt(), cat);

		QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
		if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
			ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
	}
	query.clear();
//setUpdatesEnabled( true );

	int id_index = ui->cbxCategorias->findData(id_cat);
	if( id_index < 0 )
		id_index = 0;

	ui->cbxCategorias->setCurrentIndex(id_index);
	emit on_cbxCategorias_activated(id_index);
}

void frmListIconCfg::setColorBtn(QPushButton *btn, QColor color)
{
	int w = 40;
	int h = 16;
	QPixmap img(w, h);
	QPainter painter(&img);
	painter.fillRect(img.rect(), color);
	painter.setPen(Qt::black);
	painter.drawRect(0, 0, w-1, h-1);
	painter.end();
	btn->setIconSize(QSize(w, h));
	btn->setIcon(QIcon(img));
}

void frmListIconCfg::cargarIconConfig()
{
	lwConf = fGrl->cargarListWidgetIconConf( categoria[id_cat].tabla );
	lwConftmp = lwConf;
	grl_lv_delegate->setTheme( fGrl->Theme() );
	grl_lv_delegate->setThemeCat( fGrl->ThemeApp() );
	grl_lv_delegate->setTable( categoria[id_cat].tabla );

// [tw_list_icon]
	ui->tw_icon_width->setValue( lwConf.tw_icon_width );
	ui->tw_icon_height->setValue( lwConf.tw_icon_height );

// [picflow_img]
	ui->pf_img_width->setValue( lwConf.pf_img_width );
	ui->pf_img_height->setValue( lwConf.pf_img_height );

// [list_icon]
	ui->icon_width->setValue( lwConf.icon_width );
	ui->icon_height->setValue( lwConf.icon_height );
	ui->img_cover_top->setText( lwConf.img_cover_top );
	ui->img_cover_top_select->setText( lwConf.img_cover_top_select );
	ui->img_cover_top_pos_x->setValue( lwConf.img_cover_top_pos_x );
	ui->img_cover_top_pos_y->setValue( lwConf.img_cover_top_pos_y );
	ui->img_scaled->setChecked( lwConf.img_scaled );
	ui->img_scale_w->setValue( lwConf.img_scale_w );
	ui->img_scale_h->setValue( lwConf.img_scale_h );
	ui->img_scale_pos_x->setValue( lwConf.img_scale_pos_x );
	ui->img_scale_pos_y->setValue( lwConf.img_scale_pos_y );
	ui->tipo_emu_show->setChecked( lwConf.tipo_emu_show );
	ui->tipo_emu_pos_x->setValue( lwConf.tipo_emu_pos_x );
	ui->tipo_emu_pos_y->setValue( lwConf.tipo_emu_pos_y );
	ui->rating_show->setChecked( lwConf.rating_show );
	ui->rating_vertical->setChecked( lwConf.rating_vertical );
	ui->rating_pos_x->setValue( lwConf.rating_pos_x );
	ui->rating_pos_y->setValue( lwConf.rating_pos_y );
	ui->title_bg_show->setChecked( lwConf.title_bg_show );
	ui->title_bg->setText( lwConf.title_bg );
	ui->title_bg_select->setText( lwConf.title_bg_select );
	ui->title_bg_pos_x->setValue( lwConf.title_bg_pos_x );
	ui->title_bg_pos_y->setValue( lwConf.title_bg_pos_y );
	ui->title_show->setChecked( lwConf.title_show );
	ui->title_pos_x->setValue( lwConf.title_pos_x );
	ui->title_pos_y->setValue( lwConf.title_pos_y );
	ui->title_width->setValue( lwConf.title_width );
	ui->title_height->setValue( lwConf.title_height );
	ui->title_max_caracteres->setValue( lwConf.title_max_caracteres );
	ui->title_font->setCurrentIndex( ui->title_font->findText( lwConf.title_font ) );
	ui->title_font_size->setValue( lwConf.title_font_size );
	title_font_color        = fGrl->getColor(lwConf.title_font_color);
	title_font_color_select = fGrl->getColor(lwConf.title_font_color_select);
	setColorBtn(ui->btn_title_font_color       , title_font_color);
	setColorBtn(ui->btn_title_font_color_select, title_font_color_select);
	ui->title_font_bold->setChecked( lwConf.title_font_bold );
	ui->title_font_italic->setChecked( lwConf.title_font_italic );
	ui->title_font_underline->setChecked( lwConf.title_font_underline );

	previewIconConfig();
}

void frmListIconCfg::guardarIconConfig()
{
// [tw_list_icon]
	lwConf.tw_icon_width  = ui->tw_icon_width->value();
	lwConf.tw_icon_height = ui->tw_icon_height->value();

// [picflow_img]
	lwConf.pf_img_width  = ui->pf_img_width->value();
	lwConf.pf_img_height = ui->pf_img_height->value();

// [list_icon]
	lwConf.icon_width              = ui->icon_width->value();
	lwConf.icon_height             = ui->icon_height->value();
	lwConf.img_cover_top           = ui->img_cover_top->text();
	lwConf.img_cover_top_select    = ui->img_cover_top_select->text();
	lwConf.img_cover_top_pos_x     = ui->img_cover_top_pos_x->value();
	lwConf.img_cover_top_pos_y     = ui->img_cover_top_pos_y->value();
	lwConf.img_scaled              = ui->img_scaled->isChecked();
	lwConf.img_scale_w             = ui->img_scale_w->value();
	lwConf.img_scale_h             = ui->img_scale_h->value();
	lwConf.img_scale_pos_x         = ui->img_scale_pos_x->value();
	lwConf.img_scale_pos_y         = ui->img_scale_pos_y->value();
	lwConf.tipo_emu_show           = ui->tipo_emu_show->isChecked();
	lwConf.tipo_emu_pos_x          = ui->tipo_emu_pos_x->value();
	lwConf.tipo_emu_pos_y          = ui->tipo_emu_pos_y->value();
	lwConf.rating_show             = ui->rating_show->isChecked();
	lwConf.rating_vertical         = ui->rating_vertical->isChecked();
	lwConf.rating_pos_x            = ui->rating_pos_x->value();
	lwConf.rating_pos_y            = ui->rating_pos_y->value();
	lwConf.title_bg_show           = ui->title_bg_show->isChecked();
	lwConf.title_bg                = ui->title_bg->text();
	lwConf.title_bg_select         = ui->title_bg_select->text();
	lwConf.title_bg_pos_x          = ui->title_bg_pos_x->value();
	lwConf.title_bg_pos_y          = ui->title_bg_pos_y->value();
	lwConf.title_show              = ui->title_show->isChecked();
	lwConf.title_pos_x             = ui->title_pos_x->value();
	lwConf.title_pos_y             = ui->title_pos_y->value();
	lwConf.title_width             = ui->title_width->value();
	lwConf.title_height            = ui->title_height->value();
	lwConf.title_max_caracteres    = ui->title_max_caracteres->value();
	lwConf.title_font              = ui->title_font->currentText().isEmpty() ? "Tahoma" : ui->title_font->currentText();
	lwConf.title_font_size         = ui->title_font_size->value();
	lwConf.title_font_color        = fGrl->setColor(title_font_color);
	lwConf.title_font_color_select = fGrl->setColor(title_font_color_select);
	lwConf.title_font_bold         = ui->title_font_bold->isChecked();
	lwConf.title_font_italic       = ui->title_font_italic->isChecked();
	lwConf.title_font_underline    = ui->title_font_underline->isChecked();

	fGrl->guardarListWidgetIconConf(lwConf, categoria[id_cat].tabla );
}

void frmListIconCfg::previewIconConfig()
{
	ui->twJuegos->setIconSize( QSize(lwConf.tw_icon_width, lwConf.tw_icon_height) );
	ui->lvJuegos->setIconSize( QSize(lwConf.icon_width, lwConf.icon_height) );
	grl_lv_delegate->setLwIconCfg(lwConf);

	ui->lvJuegos->update();
}

// Categorias
void frmListIconCfg::on_cbxCategorias_activated(int index)
{
	if( index > -1 )
		id_cat = ui->cbxCategorias->itemData(index, Qt::UserRole).toInt();
	else
		id_cat = 0;
	cargarIconConfig();
}

// Themes
void frmListIconCfg::on_cbxThemes_activated(int index)
{
	if( index > -1 )
	{
		int id_theme = ui->cbxThemes->itemData(index, Qt::UserRole).toInt();
		fGrl->setTheme( themes[id_theme].key );
		setTheme();
		ui->cbxListFiles->setCurrentIndex(0);
		emit on_cbxListFiles_activated(0);
		cargarIconConfig();
	}
}

// Iconos del treewidget
void frmListIconCfg::on_tw_icon_width_valueChanged(int arg1)
{
	lwConf.tw_icon_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tw_icon_width_def_clicked()
{
	lwConf.tw_icon_width = lwConftmp.tw_icon_width;
	ui->tw_icon_width->setValue( lwConf.tw_icon_width );
	previewIconConfig();
}

void frmListIconCfg::on_tw_icon_height_valueChanged(int arg1)
{
	lwConf.tw_icon_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tw_icon_height_def_clicked()
{
	lwConf.tw_icon_height = lwConftmp.tw_icon_height;
	ui->tw_icon_height->setValue( lwConf.tw_icon_height );
	previewIconConfig();
}

// Imagenes del picturefow
void frmListIconCfg::on_pf_img_width_valueChanged(int arg1)
{
	lwConf.pf_img_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_pf_img_width_def_clicked()
{
	lwConf.pf_img_width = lwConftmp.pf_img_width;
	ui->pf_img_width->setValue( lwConf.pf_img_width );
	previewIconConfig();
}

void frmListIconCfg::on_pf_img_height_valueChanged(int arg1)
{
	lwConf.pf_img_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_pf_img_height_def_clicked()
{
	lwConf.pf_img_height = lwConftmp.pf_img_height;
	ui->pf_img_height->setValue( lwConf.pf_img_height );
	previewIconConfig();
}

// Iconos de la lista en modo caratula
void frmListIconCfg::on_icon_width_valueChanged(int arg1)
{
	lwConf.icon_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_icon_width_def_clicked()
{
	lwConf.icon_width = lwConftmp.icon_width;
	ui->icon_width->setValue( lwConf.icon_width );
	previewIconConfig();
}

void frmListIconCfg::on_icon_height_valueChanged(int arg1)
{
	lwConf.icon_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_icon_height_def_clicked()
{
	lwConf.icon_height = lwConftmp.icon_height;
	ui->icon_height->setValue( lwConf.icon_height );
	previewIconConfig();
}

//--
void frmListIconCfg::on_btn_img_cover_top_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), lwConf.img_cover_top, "", tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			lwConf.img_cover_top = archivo;
			ui->img_cover_top->setText( archivo );
			previewIconConfig();
		}
	}
}

void frmListIconCfg::on_btn_img_cover_top_def_clicked()
{
	lwConf.img_cover_top = lwConftmp.img_cover_top;
	ui->img_cover_top->setText(lwConf.img_cover_top);
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_editingFinished()
{
	lwConf.img_cover_top = ui->img_cover_top->text();
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_select_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), lwConf.img_cover_top_select, "", tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			lwConf.img_cover_top_select = archivo;
			ui->img_cover_top_select->setText( archivo );
			previewIconConfig();
		}
	}
}

void frmListIconCfg::on_btn_img_cover_top_select_def_clicked()
{
	lwConf.img_cover_top_select = lwConftmp.img_cover_top_select;
	ui->img_cover_top->setText(lwConf.img_cover_top_select);
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_select_editingFinished()
{
	lwConf.img_cover_top_select = ui->img_cover_top_select->text();
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_pos_x_valueChanged(int arg1)
{
	lwConf.img_cover_top_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_pos_x_def_clicked()
{
	lwConf.img_cover_top_pos_x = lwConftmp.img_cover_top_pos_x;
	ui->img_cover_top_pos_x->setValue( lwConf.img_cover_top_pos_x );
	previewIconConfig();
}

void frmListIconCfg::on_img_cover_top_pos_y_valueChanged(int arg1)
{
	lwConf.img_cover_top_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_cover_top_pos_y_def_clicked()
{
	lwConf.img_cover_top_pos_y = lwConftmp.img_cover_top_pos_y;
	ui->img_cover_top_pos_y->setValue( lwConf.img_cover_top_pos_y );
	previewIconConfig();
}

//--
void frmListIconCfg::on_img_scaled_clicked(bool checked)
{
	lwConf.img_scaled = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scaled_def_clicked()
{
	lwConf.img_scaled = lwConftmp.img_scaled;
	ui->img_scaled->setChecked( lwConf.img_scaled );
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_w_valueChanged(int arg1)
{
	lwConf.img_scale_w = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_w_def_clicked()
{
	lwConf.img_scale_w = lwConftmp.img_scale_w;
	ui->img_scale_w->setValue( lwConf.img_scale_w );
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_h_valueChanged(int arg1)
{
	lwConf.img_scale_h = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_h_def_clicked()
{
	lwConf.img_scale_h = lwConftmp.img_scale_h;
	ui->img_scale_h->setValue( lwConf.img_scale_h );
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_pos_x_valueChanged(int arg1)
{
	lwConf.img_scale_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_pos_x_def_clicked()
{
	lwConf.img_scale_pos_x = lwConftmp.img_scale_pos_x;
	ui->img_scale_pos_x->setValue( lwConf.img_scale_pos_x );
	previewIconConfig();
}

void frmListIconCfg::on_img_scale_pos_y_valueChanged(int arg1)
{
	lwConf.img_scale_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_img_scale_pos_y_def_clicked()
{
	lwConf.img_scale_pos_y = lwConftmp.img_scale_pos_y;
	ui->img_scale_pos_y->setValue( lwConf.img_scale_pos_y );
	previewIconConfig();
}

//--
void frmListIconCfg::on_tipo_emu_show_clicked(bool checked)
{
	lwConf.tipo_emu_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_show_def_clicked()
{
	lwConf.tipo_emu_show = lwConftmp.tipo_emu_show;
	ui->tipo_emu_show->setChecked( lwConf.tipo_emu_show );
	previewIconConfig();
}

void frmListIconCfg::on_tipo_emu_pos_x_valueChanged(int arg1)
{
	lwConf.tipo_emu_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_pos_x_def_clicked()
{
	lwConf.tipo_emu_pos_x = lwConftmp.tipo_emu_pos_x;
	ui->tipo_emu_pos_x->setValue(lwConf.tipo_emu_pos_x);
	previewIconConfig();
}

void frmListIconCfg::on_tipo_emu_pos_y_valueChanged(int arg1)
{
	lwConf.tipo_emu_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_tipo_emu_pos_y_def_clicked()
{
	lwConf.tipo_emu_pos_y = lwConftmp.tipo_emu_pos_y;
	ui->tipo_emu_pos_y->setValue( lwConf.tipo_emu_pos_y );
	previewIconConfig();
}

//--
void frmListIconCfg::on_rating_show_clicked(bool checked)
{
	lwConf.rating_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_show_def_clicked()
{
	lwConf.rating_show = lwConftmp.rating_show;
	ui->rating_show->setChecked( lwConf.rating_show );
	previewIconConfig();
}

void frmListIconCfg::on_rating_vertical_clicked(bool checked)
{
	lwConf.rating_vertical = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_vertical_def_clicked()
{
	lwConf.rating_vertical = lwConftmp.rating_vertical;
	ui->rating_vertical->setChecked( lwConf.rating_vertical );
	previewIconConfig();
}

void frmListIconCfg::on_rating_pos_x_valueChanged(int arg1)
{
	lwConf.rating_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_pos_x_def_clicked()
{
	lwConf.rating_pos_x = lwConftmp.rating_pos_x;
	ui->rating_pos_x->setValue( lwConf.rating_pos_x );
	previewIconConfig();
}

void frmListIconCfg::on_rating_pos_y_valueChanged(int arg1)
{
	lwConf.rating_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_rating_pos_y_def_clicked()
{
	lwConf.rating_pos_y = lwConftmp.rating_pos_y;
	ui->rating_pos_y->setValue( lwConf.rating_pos_y );
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_bg_show_clicked(bool checked)
{
	lwConf.title_bg_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_show_def_clicked()
{
	lwConf.title_bg_show = lwConftmp.title_bg_show;
	ui->title_bg_show->setChecked( lwConf.title_bg_show );
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), lwConf.title_bg, "", tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			lwConf.title_bg = archivo;
			ui->title_bg->setText( archivo );
			previewIconConfig();
		}
	}
}

void frmListIconCfg::on_btn_title_bg_def_clicked()
{
	lwConf.title_bg = lwConftmp.title_bg;
	ui->title_bg->setText(lwConf.title_bg);
	previewIconConfig();
}

void frmListIconCfg::on_title_bg_editingFinished()
{
	lwConf.title_bg = ui->title_bg->text();
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_select_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), lwConf.title_bg_select, "", tr("Imagenes soportadas") +" ("+ grlCfg.FormatsImage.join(" ") +");;"+ tr("Todos los archivo") +" (*)");

	if( !archivo.isEmpty() )
	{
		stFileInfo f_info = fGrl->getInfoFile( archivo );
		if( f_info.Exists )
		{
			lwConf.title_bg_select = archivo;
			ui->title_bg_select->setText( archivo );
			previewIconConfig();
		}
	}
}

void frmListIconCfg::on_btn_title_bg_select_def_clicked()
{
	lwConf.title_bg_select = lwConftmp.title_bg_select;
	ui->title_bg_select->setText(lwConf.title_bg_select);
	previewIconConfig();
}

void frmListIconCfg::on_title_bg_select_editingFinished()
{
	lwConf.title_bg_select = ui->title_bg_select->text();
	previewIconConfig();
}

void frmListIconCfg::on_title_bg_pos_x_valueChanged(int arg1)
{
	lwConf.title_bg_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_pos_x_def_clicked()
{
	lwConf.title_bg_pos_x = lwConftmp.title_bg_pos_x;
	ui->title_bg_pos_x->setValue( lwConf.title_bg_pos_x );
	previewIconConfig();
}

void frmListIconCfg::on_title_bg_pos_y_valueChanged(int arg1)
{
	lwConf.title_bg_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_bg_pos_y_def_clicked()
{
	lwConf.title_bg_pos_y = lwConftmp.title_bg_pos_y;
	ui->title_bg_pos_y->setValue( lwConf.title_bg_pos_y );
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_show_clicked(bool checked)
{
	lwConf.title_show = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_show_def_clicked()
{
	lwConf.title_show = lwConftmp.title_show;
	ui->title_show->setChecked( lwConf.title_show );
	previewIconConfig();
}

void frmListIconCfg::on_title_pos_x_valueChanged(int arg1)
{
	lwConf.title_pos_x = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_pos_x_def_clicked()
{
	lwConf.title_pos_x = lwConftmp.title_pos_x;
	ui->title_pos_x->setValue( lwConf.title_pos_x );
	previewIconConfig();
}

void frmListIconCfg::on_title_pos_y_valueChanged(int arg1)
{
	lwConf.title_pos_y = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_pos_y_def_clicked()
{
	lwConf.title_pos_y = lwConftmp.title_pos_y;
	ui->title_pos_y->setValue( lwConf.title_pos_y );
	previewIconConfig();
}

void frmListIconCfg::on_title_width_valueChanged(int arg1)
{
	lwConf.title_width = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_width_def_clicked()
{
	lwConf.title_width = lwConftmp.title_width;
	ui->title_width->setValue( lwConf.title_width );
	previewIconConfig();
}

void frmListIconCfg::on_title_height_valueChanged(int arg1)
{
	lwConf.title_height = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_height_def_clicked()
{
	lwConf.title_height = lwConftmp.title_height;
	ui->title_height->setValue( lwConf.title_height );
	previewIconConfig();
}

void frmListIconCfg::on_title_max_caracteres_valueChanged(int arg1)
{
	lwConf.title_max_caracteres = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_max_caracteres_def_clicked()
{
	lwConf.title_max_caracteres = lwConftmp.title_max_caracteres;
	ui->title_max_caracteres->setValue( lwConf.title_max_caracteres );
	previewIconConfig();
}

//--
void frmListIconCfg::on_title_font_activated(const QString &arg1)
{
	lwConf.title_font = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_def_clicked()
{
	lwConf.title_font = lwConftmp.title_font;
	ui->title_font->setCurrentIndex( ui->title_font->findText( lwConf.title_font ) );
	previewIconConfig();
}

void frmListIconCfg::on_title_font_size_valueChanged(int arg1)
{
	lwConf.title_font_size = arg1;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_size_def_clicked()
{
	lwConf.title_font_size = lwConftmp.title_font_size;
	ui->title_font_size->setValue( lwConf.title_font_size );
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_color_clicked()
{
	QColor color = QColorDialog::getColor( fGrl->getColor(lwConf.title_font_color), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if( color.isValid() )
	{
		title_font_color = color;
		setColorBtn(ui->btn_title_font_color, color);
		lwConf.title_font_color = fGrl->setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_font_color_def_clicked()
{
	lwConf.title_font_color = lwConftmp.title_font_color;
	title_font_color = fGrl->getColor(lwConf.title_font_color);
	setColorBtn(ui->btn_title_font_color, title_font_color);
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_color_select_clicked()
{
	QColor color = QColorDialog::getColor( fGrl->getColor(lwConf.title_font_color_select), this, tr("Color de fondo"),  QColorDialog::DontUseNativeDialog);

	if( color.isValid() )
	{
		title_font_color_select = color;
		setColorBtn(ui->btn_title_font_color_select, color);
		lwConf.title_font_color_select = fGrl->setColor(color);
		previewIconConfig();
	}
}

void frmListIconCfg::on_btn_title_font_color_select_def_clicked()
{
	lwConf.title_font_color_select = lwConftmp.title_font_color_select;
	title_font_color_select = fGrl->getColor(lwConf.title_font_color_select);
	setColorBtn(ui->btn_title_font_color_select, title_font_color_select);
	previewIconConfig();
}

void frmListIconCfg::on_title_font_bold_clicked(bool checked)
{
	lwConf.title_font_bold = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_bold_def_clicked()
{
	lwConf.title_font_bold = lwConftmp.title_font_bold;
	ui->title_font_bold->setChecked( lwConf.title_font_bold );
	previewIconConfig();
}

void frmListIconCfg::on_title_font_italic_clicked(bool checked)
{
	lwConf.title_font_italic = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_italic_def_clicked()
{
	lwConf.title_font_italic = lwConftmp.title_font_italic;
	ui->title_font_italic->setChecked( lwConf.title_font_italic );
	previewIconConfig();
}

void frmListIconCfg::on_title_font_underline_clicked(bool checked)
{
	lwConf.title_font_underline = checked;
	previewIconConfig();
}

void frmListIconCfg::on_btn_title_font_underline_def_clicked()
{
	lwConf.title_font_underline = lwConftmp.title_font_underline;
	ui->title_font_underline->setChecked( lwConf.title_font_underline );
	previewIconConfig();
}

//--
void frmListIconCfg::on_btnIconMode_clicked(bool checked)
{
	ui->twJuegos->setVisible( !checked );
	ui->lvJuegos->setVisible( checked );
}

void frmListIconCfg::on_btnSave_clicked()
{
	guardarIconConfig();
}

//--
void frmListIconCfg::on_cbxListFiles_activated(int index)
{
	if( index > -1 )
	{
		textEditDef = fGrl->leerArchivo( fGrl->ThemeApp() + ui->cbxListFiles->itemData(index, Qt::UserRole).toString() );
		ui->textEdit->setPlainText( textEditDef );
	}
}

void frmListIconCfg::on_btnTextDef_clicked()
{
	ui->textEdit->setPlainText( textEditDef );
}

void frmListIconCfg::on_btnSaveText_clicked()
{
	QString archivo = fGrl->ThemeApp() + ui->cbxListFiles->itemData( ui->cbxListFiles->currentIndex() ).toString();
	QFile file_out( archivo );
	if( file_out.exists() )
	{
		if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
		{
			QTextStream out(&file_out);
			out.setCodec("UTF-8");
			out << ui->textEdit->toPlainText() << endl;
			out.flush();
			file_out.close();
		}
	}
}

//--
void frmListIconCfg::on_btnOk_clicked()
{
	QDialog::accept();
}
