/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#include "httpdownload.h"
#include "ui_login_url.h"

HttpDownload::HttpDownload(QWidget *parent)
	: QWidget(parent)
{
	setStatusLabel( tr("Por favor, introduzca la dirección URL de un archivo que desea descargar.") );
	setStatusBtnDownload( true );
	setHttpWindowTitle();
	isProxyEnable = false;

	progressDialog = new QProgressDialog(this);

	connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)));

	sslConfiguration = QSslConfiguration::defaultConfiguration();
#if QT_VERSION >= 0x050000
	sslConfiguration.setProtocol(QSsl::TlsV1_2);
#else
	sslConfiguration.setProtocol(QSsl::TlsV1);
#endif

#ifndef QT_NO_OPENSSL
	connect(&qnam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif

	connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
}

HttpDownload::~HttpDownload()
{

}

void HttpDownload::setHttpProxy(int typeProxy, const QString host, int port, const QString username, const QString password)
{
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

	isProxyEnable = true;
}

void HttpDownload::setStatusLabel(QString str)
{
	m_statuslabel = str;
	emit statusLabel( m_statuslabel );
}

void HttpDownload::setStatusBtnDownload(bool mbool)
{
	m_downloadButton = mbool;
	emit statusBtnEnabled( m_downloadButton );
}

void HttpDownload::startRequest(QUrl m_url, TipoDown tipo, QString contentPost)
{
	if( isProxyEnable )
		qnam.setProxy(proxy);

	QNetworkRequest request;
	request.setSslConfiguration(sslConfiguration);
	request.setUrl(m_url);

	switch (tipo)
	{
		case d_GET:
			reply = qnam.get(request);
		break;
		case d_POST:
		// content
			QByteArray content;
			content.append(contentPost);
			reply = qnam.post(request, content);
		break;
	}

	connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDataReadProgress(qint64, qint64)));
}

void HttpDownload::downloadFile(QString urlfile, QString fileName, QString tipo, QString contentPost)
{
	url = urlfile;

	if( fileName.isEmpty() )
		fileName = "index.html";

	if( QFile::exists(fileName) )
	{
//		if( QMessageBox::question(this, m_httpwindowtitle,
//			tr("Ya existe un archivo con el mismo nombre '%1' en el directorio actual.").arg( fileName ) +"\n"+
//			tr("¿Quieres sobreescribirlo?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
//			return;
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

	progressDialog->setWindowTitle(m_httpwindowtitle);
	progressDialog->setLabelText(tr("Descargando %1.").arg(fileName));
	setStatusBtnDownload( false );

	// schedule the request
	httpRequestAborted = false;
	startRequest(url, (tipo == "POST")? d_POST : d_GET, contentPost);
}

void HttpDownload::cancelDownload()
{
	setStatusLabel( tr("Descarga cancelada.") );

	httpRequestAborted = true;
	reply->abort();

	setStatusBtnDownload( true );
}

void HttpDownload::httpFinished()
{
	if( httpRequestAborted )
	{
		if( file )
		{
			file->close();
			file->remove();
			delete file;
			file = 0;
		}
		reply->deleteLater();
		progressDialog->hide();
		return;
	}

	progressDialog->hide();
	file->flush();
	file->close();

	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if( reply->error() )
	{
		file->remove();
		QMessageBox::information(this, m_httpwindowtitle, tr("No se ha podido guardar el archivo: %1.").arg( reply->errorString() ) );
		setStatusBtnDownload( true );
	}
	else if( !redirectionTarget.isNull() )
	{
		QUrl newUrl = url.resolved( redirectionTarget.toUrl() );
		if( QMessageBox::question(this, m_httpwindowtitle, tr("¿Redireccionar a '%1'?").arg( newUrl.toString() ), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			url = newUrl;
			reply->deleteLater();
			file->open(QIODevice::WriteOnly);
			file->resize(0);
			startRequest(url);
			return;
		}
	} else {
		QString fileName = QFileInfo( QUrl( url ).path() ).fileName();
		setStatusLabel( tr("Descargado '%1' en el directorio actual.").arg( fileName ) );
		setStatusBtnDownload( true );
	}

	reply->deleteLater();
	reply = 0;
	delete file;
	file = 0;
	emit statusFinished();
}

void HttpDownload::httpReadyRead()
{
// this slot gets called every time the QNetworkReply has new data.
// We read all of its new data and write it into the file.
// That way we use less RAM than when reading it at the finished()
// signal of the QNetworkReply
	if( file )
		file->write(reply->readAll());
}

void HttpDownload::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	if( httpRequestAborted )
		return;

	progressDialog->setMaximum( totalBytes );
	progressDialog->setValue( bytesRead );
}

void HttpDownload::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
{
	QDialog dlg;
	Ui::DialogLogin ui;
	ui.setupUi(&dlg);
	dlg.adjustSize();
	ui.siteDescription->setText(tr("%1 en %2").arg(authenticator->realm()).arg(url.host()));

// Did the URL have information? Fill the UI
// This is only relevant if the URL-supplied credentials were wrong
	ui.userEdit->setText(url.userName());
	ui.passwordEdit->setText(url.password());

	if( dlg.exec() == QDialog::Accepted )
	{
		authenticator->setUser(ui.userEdit->text());
		authenticator->setPassword(ui.passwordEdit->text());
	}
}

#ifndef QT_NO_OPENSSL
void HttpDownload::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
	QString errorString;
	foreach (const QSslError &error, errors)
	{
		if( !errorString.isEmpty() )
			errorString += ", ";
		errorString += error.errorString();
	}

	if( QMessageBox::warning(this, m_httpwindowtitle, tr("Uno o más errores de SSL se ha producido: %1").arg(errorString),
			QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore )
	{
		reply->ignoreSslErrors();
	}
}
#endif
