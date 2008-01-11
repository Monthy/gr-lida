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

#include "grlida_acercad.h"

frmAcercaD::frmAcercaD(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

    connect( ui.btnAcercaQT, SIGNAL( clicked() ), qApp, SLOT( aboutQt() ) );

	QString AcercaD_Html;
	AcercaD_Html.clear();
	AcercaD_Html.append( "<strong>GR-lida</strong> es un lanzador común para los emuladores:" );
	AcercaD_Html.append( "<center><strong>DOSBox</strong>, <strong><font color=\"#009900\">ScummVM</font></strong> y <strong>VDMSound</strong><br></center>" );
	AcercaD_Html.append( "<strong>GR-lida</strong> es GPL. Para mejorar el programa puedes dejar tu opinión en  <a href=\"http://www.laisladelabandoware.com\">http://www.laisladelabandoware.com</a><br />" );
	AcercaD_Html.append( "Web Oficiales del lanzador y los distintos emuladores:<br />" );
	AcercaD_Html.append( "Lanzador <strong>GR-lida</strong>  <a href=\"http://www.laisladelabandoware.com/grlida.php\">http://www.laisladelabandoware.com</a><br />" );
	AcercaD_Html.append( "Emulador <strong>DOSBox</strong>: <a href=\"http://dosbox.sourceforge.net\">http://dosbox.sourceforge.net</a><br />" );
	AcercaD_Html.append( "Emulador <strong><font color=\"#009900\">ScummVM</font></strong>: <a href=\"http://www.scummvm.org\">http://www.scummvm.org</a><br />" );
	AcercaD_Html.append( "Emulador <strong>VDMSound</strong>: <a href=\"http://sourceforge.net/projects/vdmsound/\">http://sourceforge.net/projects/vdmsound/</a><br>" );
	ui.txtAcercaD->setHtml( AcercaD_Html );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAcercaD::~frmAcercaD(){}
