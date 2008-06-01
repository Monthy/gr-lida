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

	stTheme = fGrl.ThemeGrl();
	setTheme();

    connect( ui.btnAcercaQT, SIGNAL( clicked() ), qApp, SLOT( aboutQt() ) );

	QString AcercaD_Html;
	AcercaD_Html.clear();
	AcercaD_Html.append( "<strong>GR-lida</strong> "+ tr("es un lanzador común para los emuladores:") );
	AcercaD_Html.append( "<center><strong>DOSBox</strong>, <strong><font color=\"#009900\">ScummVM</font></strong>, <strong>VDMSound</strong><br></center>" );
	AcercaD_Html.append( "<strong>GR-lida</strong> "+ tr("es GPL. Para mejorar el programa puedes dejar tu opinión en")+ " <a href=\"http://www.laisladelabandoware.es\">http://www.laisladelabandoware.es</a><br />" );
	AcercaD_Html.append( tr("Web Oficiales del lanzador y los distintos emuladores:")+"<br />" );
	AcercaD_Html.append( tr("Lanzador")+" <strong>GR-lida</strong>: <a href=\"http://www.laisladelabandoware.es/grlida.php\">http://www.laisladelabandoware.es</a><br />" );
	AcercaD_Html.append( tr("Emulador")+" <strong>DOSBox</strong>: <a href=\"http://dosbox.com\">http://dosbox.com</a><br />" );
	AcercaD_Html.append( tr("Emulador")+" <strong><font color=\"#009900\">ScummVM</font></strong>: <a href=\"http://www.scummvm.org\">http://www.scummvm.org</a><br />" );
	AcercaD_Html.append( tr("Emulador")+" <strong>VDMSound</strong>: <a href=\"http://sourceforge.net/projects/vdmsound/\">http://sourceforge.net/projects/vdmsound/</a><br>" );
	ui.txtAcercaD->setHtml( AcercaD_Html );
    ui.label_7->setText(tr("Para") + " <b>Windows, Linux, Mac</b>");

// Abrimos la lista de Traductores
	QFile file( ":/list_translators.txt" );
	QStringList str_Lista, str_ListaTemp;
    	
	ui.treeWidget->clear();
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
    	QTextStream in(&file);
		in.setCodec("UTF-8");

    	while ( !in.atEnd() )
    		str_ListaTemp << in.readLine();

    	for ( int i = 0; i < str_ListaTemp.size(); i++ )
   		{
    		str_Lista = str_ListaTemp[i].split( "|" );

   			QTreeWidgetItem *item = new QTreeWidgetItem( ui.treeWidget );

			item->setFont( 0, QFont("Times", 10, QFont::Bold));
   			item->setText( 0, str_Lista.value(0) );
   			if( str_Lista.value(2)=="")
   				item->setIcon( 1, QIcon(stTheme+"img16/sinimg.png") );
   			else
   				item->setIcon( 1, QIcon(stTheme+"img_lng/"+str_Lista.value(2)+".png") );	   				
   			item->setText( 1, str_Lista.value(1));
   		}
   	}
   	file.close();

// licencia GPL
	QFile fileL( ":/license.gpl" );
	fileL.open( QFile::ReadOnly );
	ui.txtLicense->setPlainText( QString::fromUtf8( fileL.readAll() ) );
	fileL.close();
	ui.txtLicense->moveCursor( QTextCursor::Start );

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmAcercaD::~frmAcercaD(){}

void frmAcercaD::setTheme()
{
	ui.btnAcercaQT->setIcon( QIcon(stTheme+"img16/qt.png") );
	ui.btnClose->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.lb_Dbximg->setPixmap( QPixmap(stTheme+"img24/emu_dbx.png") );
	ui.lb_Svmimg->setPixmap( QPixmap(stTheme+"img24/emu_svm.png") );
	ui.lb_Vdmsimg->setPixmap( QPixmap(stTheme+"img24/emu_vdms.png") );
}
