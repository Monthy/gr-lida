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

#include <QtGui>
#include <QtNetwork>

#include "httpdownload.h"
#include "ui_login_url.h"

HttpDownload::HttpDownload(QWidget *parent)
    : QWidget(parent)
{
	setStatusLabel( tr("Por favor, introduzca la dirección URL de un archivo que desea descargar.") );
	setStatusBtnDownload( true );
	setHttpWindowTitle();

	progressDialog = new QProgressDialog(this);
	http = new QHttp(this);


	connect(http, SIGNAL( requestFinished(int, bool) ), this, SLOT( httpRequestFinished(int, bool) ) );
	connect(http, SIGNAL( responseHeaderReceived(const QHttpResponseHeader &) ), this, SLOT( readResponseHeader(const QHttpResponseHeader &) ) );
	connect(http, SIGNAL( dataReadProgress(int, int) ), this, SLOT( updateDataReadProgress(int, int) ) );
	connect(http, SIGNAL( stateChanged(int) ), this, SLOT( httpstateChanged(int) ) );
	connect(http, SIGNAL( authenticationRequired(const QString &, quint16, QAuthenticator *) ), this, SLOT( slotAuthenticationRequired(const QString &, quint16, QAuthenticator *) ) );

#ifndef QT_NO_OPENSSL
	connect(http, SIGNAL( sslErrors(const QList<QSslError> &) ), this, SLOT( sslErrors(const QList<QSslError> &) ) );
#endif

	connect(progressDialog, SIGNAL( canceled() ), this, SLOT( cancelDownload() ) );
}

HttpDownload::~HttpDownload()
{

}

void HttpDownload::setHttpProxy(int typeProxy, const QString host, int port, const QString username, const QString password)
{
	QNetworkProxy proxy;

	switch ( typeProxy )
	{
		case 0:
			proxy.setType(QNetworkProxy::DefaultProxy);
		break;
		case 1:
			proxy.setType(QNetworkProxy::Socks5Proxy);
		break;
		case 2:
			proxy.setType(QNetworkProxy::NoProxy);
		break;
		case 3:
			proxy.setType(QNetworkProxy::HttpProxy);
		break;
		case 4:
			proxy.setType(QNetworkProxy::HttpCachingProxy);
		break;
		default:
			proxy.setType(QNetworkProxy::NoProxy);
		break;
	}
	proxy.setHostName( host );
	proxy.setPort( port );
	proxy.setUser( username );
	proxy.setPassword( password );
//	QNetworkProxy::setApplicationProxy(proxy);
	http->setProxy( proxy );

//	http->setProxy(host, port, username, password);
}

void HttpDownload::setHttpWindowTitle(QString titulo)
{
	m_httpwindowtitle = titulo;
}

void HttpDownload::downloadFile(QString urlfile, QString fileName, QString metodo, QString contentPost)
{
	urlLineEdit.clear();
	urlLineEdit = urlfile;

	QUrl url( urlLineEdit );

	if(fileName.isEmpty())
		fileName = "index.html";

	if(QFile::exists(fileName))
	{
	//	if(QMessageBox::question(this, m_httpwindowtitle,
	//		tr("Ya existe un archivo con el mismo nombre '%1' en el directorio actual.").arg( fileName )+" "+
	//		tr("¿Quieres sobreescribirlo?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
	//		return;
		QFile::remove(fileName);
	}

	file = new QFile(fileName);
	if( !file->open(QIODevice::WriteOnly) )
	{
		QMessageBox::information(this, m_httpwindowtitle, tr("No se ha podido guardar el archivo %1: %2.").arg( fileName ).arg( file->errorString() ));

		delete file;
		file = 0;

		return;
	}
	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
	http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

	if( !url.userName().isEmpty() )
		http->setUser(url.userName(), url.password());

	httpRequestAborted = false;

//	QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
//	if (path.isEmpty())
//		path = "/";

	if( metodo == "POST" )
	{
	// header
		QHttpRequestHeader header;
	// content
		QByteArray content;
		content.append( contentPost );

//		header.setRequest(metodo, path);
		header.setRequest(metodo, url.path());
		header.setValue("Host", url.host() );
		header.setContentType("application/x-www-form-urlencoded"); // important
		header.setContentLength( content.length() );

		httpGetId = http->request(header, content, file);
	} else
		httpGetId = http->get(url.toEncoded(), file);
//		httpGetId = http->get( path, file);

	progressDialog->setWindowTitle( m_httpwindowtitle );
	progressDialog->setLabelText( tr("Descargando %1.").arg( fileName ) );

	setStatusBtnDownload( false );
}

void HttpDownload::cancelDownload()
{
	setStatusLabel( tr("Descarga cancelada.") );

	httpRequestAborted = true;
	http->abort();

	setStatusBtnDownload( true );
}

void HttpDownload::httpRequestFinished(int requestId, bool error)
{
	if(requestId != httpGetId)
		return;
	if(httpRequestAborted)
	{
		if(file)
		{
			file->close();
			file->remove();

			delete file;
			file = 0;
		}

		progressDialog->hide();
		return;
	}

	if(requestId != httpGetId)
		return;

	progressDialog->hide();
	file->flush();
	file->close();

	if(error)
	{
		file->remove();
		QMessageBox::information(this, m_httpwindowtitle, tr("No se ha podido guardar el archivo: %1.").arg( http->errorString() ) );
	} else {
		QString fileName = QFileInfo( QUrl( urlLineEdit ).path() ).fileName();
		setStatusLabel( tr("Descargado en el directorio: %1.").arg( fileName ) );
		emit StatusRequestFinished();
	}

	setStatusBtnDownload( true );

	delete file;
	file = 0;
}

void HttpDownload::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
	switch (responseHeader.statusCode())
	{
	case 200:                   // Ok
	case 301:                   // Moved Permanently
	case 302:                   // Found
	case 303:                   // See Other
	case 307:                   // Temporary Redirect
		// these are not error conditions
		break;

	default:
		setStatusLabel("");
		QMessageBox::information(this, m_httpwindowtitle, tr("La descarga ha fallado: %1.").arg( responseHeader.reasonPhrase() ) );

		httpRequestAborted = true;
		progressDialog->hide();

		http->disconnect();
		http->abort();
		setStatusBtnDownload( true );
	}
}


void HttpDownload::updateDataReadProgress(int bytesRead, int totalBytes)
{
	if(httpRequestAborted)
		return;

	progressDialog->setMaximum(totalBytes);
	progressDialog->setValue(bytesRead);
}

void HttpDownload::httpstateChanged(int state)
{
	switch( state )
	{
		case QHttp::Unconnected:
//			setStatusLabel("There is no connection to the host");
		break;
		case QHttp::HostLookup:
//			setStatusLabel("A host name lookup is in progress.");
		break;
		case QHttp::Connecting:
//			setStatusLabel("An attempt to connect to the host is in progress.");
		break;
		case QHttp::Sending:
//			setStatusLabel("The client is sending its request to the server.");
		break;
		case QHttp::Reading:
//			setStatusLabel("The client's request has been sent and the client is reading the server's response.");
		break;
		case QHttp::Connected:
//			setStatusLabel("The connection to the host is open, but the client is neither sending a request, nor waiting for a response.");
		break;
		case QHttp::Closing:
//			setStatusLabel("The connection is closing down, but is not yet closed. (The state will be Unconnected when the connection is closed.");
		break;
	}
}

void HttpDownload::slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator)
{
	QDialog dlg;
	Ui::DialogLogin ui;
	ui.setupUi(&dlg);
	dlg.adjustSize();
	ui.siteDescription->setText(tr("%1 en %2").arg(authenticator->realm()).arg(hostName));

	if(dlg.exec() == QDialog::Accepted)
	{
		authenticator->setUser( ui.userEdit->text() );
		authenticator->setPassword( ui.passwordEdit->text() );
	}
}

void HttpDownload::setStatusBtnDownload(bool mbool)
{
	m_downloadButton = mbool;
	emit StatusBtnDownloadChanged( mbool );
}

void HttpDownload::setStatusLabel(QString str)
{
	m_statuslabel = str;
	emit statusLabelChanged( str );
}

#ifndef QT_NO_OPENSSL
void HttpDownload::sslErrors(const QList<QSslError> &errors)
{
	QString errorString;
	foreach (const QSslError &error, errors)
	{
		if(!errorString.isEmpty())
			errorString += ", ";
		errorString += error.errorString();
	}

	if(QMessageBox::warning(this, m_httpwindowtitle, tr("Uno o más errores de SSL se ha producido: %1").arg(errorString),
			QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
		http->ignoreSslErrors();
	}
}
#endif
