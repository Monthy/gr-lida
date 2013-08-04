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

#include "grlida_delegate.h"

// INICIO GrlListViewDelegate -----------------------------------------------------------------------------------
GrlListViewDelegate::GrlListViewDelegate(dbSql *m_sql, QString dir_app, QString dir_theme, QString dir_theme_app, QString filter, QWidget *parent)
	:QItemDelegate(parent)
{
	sql        = m_sql;
	stDirApp   = dir_app;
	stTheme    = dir_theme;
	stThemeApp = dir_theme_app;
	stFilter   = filter;
}

GrlListViewDelegate::~GrlListViewDelegate()
{

}

void GrlListViewDelegate::setLwIconCfg(stLwIconCfg m_lwConf)
{
	lwConf = m_lwConf;

	if( !cover_top.load(stThemeApp + lwConf.img_cover_top) )
		cover_top.load(":/images/list_cover_top.png");

	if( !cover_top_select.load(stThemeApp + lwConf.img_cover_top_select) )
		cover_top_select.load(":/images/list_cover_top_select.png");

	if( !title_bg.load(stThemeApp + lwConf.title_bg) )
		title_bg.load(":/images/list_cover_title_bg.png");

	if( !title_bg_select.load(stThemeApp + lwConf.title_bg_select) )
		title_bg_select.load(":/images/list_cover_title_bg.png");

	if( !star_on.load(stTheme +"images/star_on.png") )
		star_on.load(":/images/star_on.png");

	if( !star_off.load(stTheme +"images/star_off.png") )
		star_off.load(":/images/star_off.png");

	ico_emu.clear();
	fGrl.insertaIconos(ico_emu, stThemeApp +"img16_cat/", "", stFilter, false);

	QHash<QString, stGrlDatos> emu_list = fGrl.cargaDatosQHash(stDirApp +"datos/emu_list.txt", 4, "|");
	foreach (const stGrlDatos &dat, emu_list)
		ico_emu.insert(dat.key, QPixmap(stThemeApp +"img16_cat/"+ dat.icono));

	pen = QApplication::palette().text().color();
	pen_def.setColor(QColor(lwConf.title_font_color.at(0).toInt(), lwConf.title_font_color.at(1).toInt(), lwConf.title_font_color.at(2).toInt()));
	pen_select.setColor(QColor(lwConf.title_font_color_select.at(0).toInt(), lwConf.title_font_color_select.at(1).toInt(), lwConf.title_font_color_select.at(2).toInt()));
}

void GrlListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	int rect_x = option.rect.x();
	int rect_y = option.rect.y();

// Dibujamos la caratula.
	QPixmap icon = qvariant_cast<QPixmap>(index.data(IconRole));
	if( lwConf.img_scaled )
		painter->drawPixmap(QRect(rect_x + lwConf.img_scale_pos_x, rect_y + lwConf.img_scale_pos_y, lwConf.img_scale_w, lwConf.img_scale_h), icon);
	else
		painter->drawPixmap(QPointF(rect_x + lwConf.img_scale_pos_x, rect_y + lwConf.img_scale_pos_y), icon);

// Dibujamos caratula top
	if( option.state & QStyle::State_Selected )
		painter->drawPixmap(QPointF(rect_x + lwConf.img_cover_top_pos_x, rect_y + lwConf.img_cover_top_pos_y), cover_top_select);
	else
		painter->drawPixmap(QPointF(rect_x + lwConf.img_cover_top_pos_x, rect_y + lwConf.img_cover_top_pos_y), cover_top);

// Dibujamos tipo de emulador
	if( lwConf.tipo_emu_show )
	{
		QString str_emu = qvariant_cast<QString>(index.data(TipoEmuRole));
		painter->drawPixmap(QPointF(rect_x + lwConf.tipo_emu_pos_x, rect_y + lwConf.tipo_emu_pos_y), ico_emu[str_emu]);
	}

// Dibujamos el rating
	if( lwConf.rating_show )
	{
		int rating = qvariant_cast<int>(index.data(RatingRole));
		int rating_x = rect_x + lwConf.rating_pos_x;
		int rating_y = rect_y + lwConf.rating_pos_y;
		for (int i = 0; i < 5; ++i)
		{
			if( i < rating )
				painter->drawPixmap(QPointF(rating_x, rating_y), star_on);
			else
				painter->drawPixmap(QPointF(rating_x, rating_y), star_off);

			if( lwConf.rating_vertical )
				rating_y -= star_on.height();
			else
				rating_x += star_on.width();
		}
	}

// Dibujamos el fondo del titulo
	if( lwConf.title_bg_show )
	{
		if( option.state & QStyle::State_Selected )
			painter->drawPixmap(QPointF(rect_x + lwConf.title_bg_pos_x, rect_y + lwConf.title_bg_pos_y), title_bg_select);
		else
			painter->drawPixmap(QPointF(rect_x + lwConf.title_bg_pos_x, rect_y + lwConf.title_bg_pos_y), title_bg);
	}

// Dibujamos el titulo
	if( lwConf.title_show )
	{
		QString titulo = qvariant_cast<QString>(index.data(TituloRole));
		QRectF rect_font(rect_x + lwConf.title_pos_x, rect_y + lwConf.title_pos_y, lwConf.title_width, lwConf.title_height);
		QFont font;
		font.setFamily( lwConf.title_font );
		font.setPointSize( lwConf.title_font_size );
		font.setBold( lwConf.title_font_bold );
		font.setItalic( lwConf.title_font_italic );
		font.setUnderline( lwConf.title_font_underline );
		font.setStyleStrategy(QFont::PreferAntialias);
		painter->setFont( font );

		if( option.state & QStyle::State_Selected )
			painter->setPen(pen_select);
		else
			painter->setPen(pen_def);

		if( titulo.size() > lwConf.title_max_caracteres )
			painter->drawText(rect_font, Qt::AlignCenter, titulo.left(lwConf.title_max_caracteres) +"..");
		else
			painter->drawText(rect_font, Qt::AlignCenter, titulo);

		painter->setPen(pen);
	}
	QItemDelegate::paint(painter, option, index);
}

QSize GrlListViewDelegate::sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	return QSize(lwConf.icon_width, lwConf.icon_height);
	return QItemDelegate::sizeHint(option, index);
}

bool GrlListViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if( index.column() == 0 )
	{
		if( event->type() == QEvent::MouseButtonPress )
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

			if( index.data(RatingRole).toString() != "" )
			{
				QString id_grl = index.data(IdGrlRole).toString();
				int stars, pos_x, pos_y, rating_x, rating_y, limit_x, limit_y;

				pos_x    = mouseEvent->pos().x();
				pos_y    = mouseEvent->pos().y();
				rating_x = option.rect.x() + lwConf.rating_pos_x;
				if( lwConf.rating_vertical )
				{
					rating_y = option.rect.y() + lwConf.rating_pos_y + star_on.height() - (5 * star_on.height());
					limit_x  = star_on.width();
					limit_y  = 5 * star_on.height();
				} else {
					rating_y = option.rect.y() + lwConf.rating_pos_y;
					limit_x  = 5 * star_on.width();
					limit_y  = star_on.height();
				}
				if( pos_x > rating_x && pos_y > rating_y && pos_x <= (rating_x + limit_x) && pos_y <= (rating_y + limit_y) )
				{
					if( lwConf.rating_vertical )
						stars = qAbs(qBound(0, int(0.5 + qreal(pos_y - rating_y) / star_on.height()), 5)-5);
					else
						stars = qBound(0, int(0.5 + qreal(pos_x - rating_x) / star_on.width()), 5);

					model->setData(index, QVariant(stars), RatingRole);
					sql->actualizaDatosRating(stTabla, id_grl, fGrl.IntToStr(stars));
				}
			}
			return false;
		}
		return true;
	} else
		return QItemDelegate::editorEvent(event, model, option, index);
}
// FIN GrlListViewDelegate --------------------------------------------------------------------------------------

// INICIO GrlTreeViewModel --------------------------------------------------------------------------------------
GrlTreeViewModel::GrlTreeViewModel(QObject *parent, QSqlDatabase db, QString formato_fecha)
	:QSqlTableModel(parent, db)
{
	if( formato_fecha.isEmpty() )
		formatofecha = "dd/MM/yyyy HH:mm:ss";
	else
		formatofecha = formato_fecha;
}

QVariant GrlTreeViewModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::TextAlignmentRole)
	{
		if( this->fieldIndex("anno") == index.column() || this->fieldIndex("tamano") == index.column() ||
			this->fieldIndex("graficos") == index.column() || this->fieldIndex("sonido") == index.column() ||
			this->fieldIndex("jugabilidad") == index.column() || this->fieldIndex("fecha") == index.column() ||
			this->fieldIndex("tipo_emu") == index.column() )
		{
			return Qt::AlignCenter;
		}
	}
	else if (role == Qt::DisplayRole)
	{
		if( this->fieldIndex("fecha") == index.column() )
		{
			QDateTime dt;
			dt.setTime_t( QSqlTableModel::data(index, Qt::DisplayRole).toInt() );
			return ""+ dt.toString(formatofecha);
		}
		if( this->fieldIndex("genero") == index.column() || this->fieldIndex("compania") == index.column() ||
			this->fieldIndex("desarrollador") == index.column() || this->fieldIndex("tema") == index.column() ||
			this->fieldIndex("grupo") == index.column() || this->fieldIndex("perspectiva") == index.column() ||
			this->fieldIndex("idioma") == index.column() || this->fieldIndex("idioma_voces") == index.column() ||
			this->fieldIndex("formato") == index.column() || this->fieldIndex("sistemaop") == index.column() )
		{
			QString texto = QSqlTableModel::data(index, Qt::DisplayRole).toString().replace(";", "; ");
			return texto;
		}
	}

	return QSqlTableModel::data(index, role);
}
// FIN GrlTreeViewModel -----------------------------------------------------------------------------------------

// INICIO GrlTreeViewDelegate -----------------------------------------------------------------------------------
GrlTreeViewDelegate::GrlTreeViewDelegate(dbSql *m_sql, QString dir_app, QString dir_theme, QString dir_theme_app, QString filter, QTreeWidget *m_twNav, QWidget *parent)
	:QItemDelegate(parent)
{
	sql        = m_sql;
	stDirApp   = dir_app;
	stTheme    = dir_theme;
	stThemeApp = dir_theme_app;
	twNav      = m_twNav;

	if( !star_on.load(stTheme +"images/star_on.png") )
		star_on.load(":/images/star_on.png");

	if( !star_off.load(stTheme +"images/star_off.png") )
		star_off.load(":/images/star_off.png");

	if( !sin_img.load(stTheme +"img16/sinimg.png") )
		sin_img.load(":/img16/sinimg.png");

	if( !img_original.load(stTheme +"img16/original.png") )
		img_original.load(":/img16/original.png");

	edades.clear();
	fGrl.insertaIconos(edades, stTheme +"img16/", "edad_", filter);

	icono.clear();
	fGrl.insertaIconos(icono, stDirApp +"iconos/", "", filter);
	fGrl.insertaIconos(icono, stThemeApp +"img24_cat/", "", filter);
}

GrlTreeViewDelegate::~GrlTreeViewDelegate()
{

}

void GrlTreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	if( index.column() == col[col_Icono] || index.column() == col[col_Original] ||
		index.column() == col[col_Favorito] || index.column() == col[col_Rating] ||
		index.column() == col[col_Edad] )
	{
		const QAbstractItemModel *model = index.model();
		QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ?
			(option.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive : QPalette::Disabled;

		if( option.state & QStyle::State_Selected )
			painter->fillRect(option.rect, option.palette.color(cg, QPalette::Highlight));

		if( !model->data(index, Qt::DisplayRole).toString().isEmpty() )
		{
			int width, height, x, y;
			if ( index.column() == col[col_Icono] )
			{
				QString ico = model->data(index, Qt::DisplayRole).toString();
				width  = icono[ico].width();
				height = icono[ico].height();
				QImage img_painter(option.rect.width(), option.rect.height(), QImage::Format_ARGB32);
				x = (img_painter.width() / 2) - (width / 2);
				y = (img_painter.height() / 2) - (height / 2);
				QPainter p(&img_painter);
				p.setCompositionMode(QPainter::CompositionMode_Source);
				p.fillRect(img_painter.rect(), Qt::transparent);
				p.drawPixmap(x, y, icono[ico]);
				p.end();
				x = option.rect.x();
				y = option.rect.y() + (option.rect.height() / 2) - (img_painter.height() / 2);

				painter->drawImage(x, y, img_painter);
			}
			if ( index.column() == col[col_Original] )
			{
				QString original = model->data(index, Qt::DisplayRole).toString().toLower();
				width  = img_original.width();
				height = img_original.height();
				x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
				y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

				if( original == "true" || original == "yes" || original == "1" )
					painter->drawPixmap(x, y, img_original);
				else
					painter->drawPixmap(x, y, sin_img);
			}
			if ( index.column() == col[col_Favorito] )
			{
				bool favorito = model->data(index, Qt::DisplayRole).toBool();
				width  = img_favorito.width();
				height = img_favorito.height();
				x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
				y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

				if( favorito )
					painter->drawPixmap(x, y, img_favorito);
				else
					painter->drawPixmap(x, y, sin_img);
			}
			if ( index.column() == col[col_Rating] )
			{
				int rating = model->data(index, Qt::DisplayRole).toInt();
				width  = star_on.width();
				height = star_on.height();
				QImage img_painter(option.rect.width(), option.rect.height(), QImage::Format_ARGB32);
				x = 0;
				y = (img_painter.height() / 2) - (height / 2);
				QPainter p(&img_painter);
				p.setCompositionMode(QPainter::CompositionMode_Source);
				p.fillRect(img_painter.rect(), Qt::transparent);
				for (int i = 0; i < 5; ++i)
				{
					if(i < rating)
						p.drawPixmap(x, y, star_on);
					else
						p.drawPixmap(x, y, star_off);
					x += width;
				}
				p.end();
				x = option.rect.x();
				y = option.rect.y() + (option.rect.height() / 2) - (img_painter.height() / 2);

				painter->drawImage(x, y, img_painter);
			}
			if ( index.column() == col[col_Edad] )
			{
				QString edad = "edad_"+ model->data(index, Qt::DisplayRole).toString() +".png";
				width  = edades[edad].width();
				height = edades[edad].height();
				x      = option.rect.x() + (option.rect.width() / 2) - (width / 2);
				y      = option.rect.y() + (option.rect.height() / 2) - (height / 2);

				painter->drawPixmap(x, y, edades[edad]);
			}
		}
		drawFocus(painter, option, option.rect.adjusted(0, 0, 0, 0));
	} else {
		QStyleOptionViewItemV3 opt = option;
		opt.rect.adjust(0, 0, 0, 0); // since we draw the grid ourselves
		QItemDelegate::paint(painter, opt, index);
	}
}

QSize GrlTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	if ( index.column() == col[col_Rating] )
		return QSize(5 * star_on.width(), size_icon.height()) + QSize(1, 1);

	if( index.column() == col[col_Titulo] )
		return size_icon + QSize(1, 1);

	return QItemDelegate::sizeHint(option, index) + QSize(1, 1);
}

bool GrlTreeViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if( index.column() == col[col_Original] || index.column() == col[col_Favorito] || index.column() == col[col_Rating] )
	{
		if( event->type() == QEvent::MouseButtonPress )
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

			if( index.data(Qt::DisplayRole).toString() != "" )
			{
				QString id_grl = model->data(index.sibling(index.row(), col[col_IdGrl]), Qt::DisplayRole).toString();
				if( index.column() == col[col_Original] )
				{
					QString original = index.data(Qt::DisplayRole).toString();
					original = fGrl.StrToBool(original) ? "false" : "true";
					model->setData(index, original);
					sql->actualizaDatosItem(stTabla, id_grl, "original", original);

					QFont m_font;
					int total_original = sql->getCount(stTabla, "WHERE original='true'");
					if( total_original > 0 )
						m_font.setBold(true);
					else
						m_font.setBold(false);
					twNav->topLevelItem(2)->setFont(1, m_font );
					twNav->topLevelItem(2)->setText(1, fGrl.IntToStr(total_original));
				}
				if( index.column() == col[col_Favorito] )
				{
					QString fav = index.data(Qt::DisplayRole).toString();
					fav = fGrl.StrToBool(fav) ? "false" : "true";
					model->setData(index, fav);
					sql->actualizaDatosFavorito(stTabla, id_grl, fav);

					QFont m_font;
					int total_fav = sql->getCount(stTabla, "WHERE favorito='true'");
					if( total_fav > 0 )
						m_font.setBold(true);
					else
						m_font.setBold(false);
					twNav->topLevelItem(1)->setFont(1, m_font );
					twNav->topLevelItem(1)->setText(1, fGrl.IntToStr(total_fav));
				}
				if( index.column() == col[col_Rating] )
				{
					int stars = qBound(0, int(0.5 + qreal(mouseEvent->pos().x() - option.rect.x()) / star_on.width()), 5);
					model->setData(index, QVariant(stars));
					sql->actualizaDatosRating(stTabla, id_grl, fGrl.IntToStr(stars));
				}
			}
			return false; //so that the selection can change
		}
		return true;
	} else
		return QItemDelegate::editorEvent(event, model, option, index);
}
// FIN GrlTreeViewDelegate --------------------------------------------------------------------------------------
