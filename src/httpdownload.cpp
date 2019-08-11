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

#include <QMessageBox>

#include "httpdownload.h"
#include "ui_login_url.h"

HttpDownload::HttpDownload(QWidget *parent) : QWidget(parent)
{
	setStatusBtnDownload(true);
	setHttpWindowTitle();
	isProxyEnable = false;
	httpRequestAborted = false;
	isConfirmRedirect = true;
	reply = 0;
	file = 0;

	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	gridLayout->setHorizontalSpacing(6);
	gridLayout->setVerticalSpacing(2);
		progressBar = new QProgressBar(this);
		progressBar->setObjectName(QString::fromUtf8("progressBar"));
		progressBar->setMinimumSize(QSize(200, 10));
		progressBar->setMaximumSize(QSize(200, 10));
		progressBar->setStyleSheet(QString::fromUtf8("padding:0;"));
		progressBar->setTextVisible(false);
		progressBar->setMinimum(0);
		progressBar->setValue(0);
	gridLayout->addWidget(progressBar, 0, 0, 1, 1);
		progressBarLabel = new QLabel(this);
		progressBarLabel->setObjectName(QString::fromUtf8("progressBarLabel"));
		progressBarLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		progressBarLabel->setMinimumSize(QSize(144, 15));
		progressBarLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
		progressBarLabel->setText("");
	gridLayout->addWidget(progressBarLabel, 0, 1, 1, 1);
		statusLabel = new QLabel(this);
		statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
		statusLabel->setAlignment(Qt::AlignCenter);
		statusLabel->setWordWrap(true);
	gridLayout->addWidget(statusLabel, 1, 0, 1, 2);

	defaultInfo();

	connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)));

	sslConfiguration = QSslConfiguration::defaultConfiguration();
#if QT_VERSION >= 0x050000
	sslConfiguration.setProtocol(QSsl::TlsV1_2);
#else
	sslConfiguration.setProtocol(QSsl::TlsV1);
#endif

#ifndef QT_NO_SSL // QT_NO_OPENSSL
	connect(&qnam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
#endif
}

HttpDownload::~HttpDownload()
{

}

void HttpDownload::setHttpProxy(int typeProxy, const QString host, quint16 port, const QString username, const QString password)
{
	switch (typeProxy)
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
	proxy.setHostName(host);
	proxy.setPort(port);
	proxy.setUser(username);
	proxy.setPassword(password);

	isProxyEnable = true;
}

void HttpDownload::setStatusBtnDownload(bool mbool)
{
	m_downloadButton = mbool;
	emit statusBtnEnabled(m_downloadButton);
}

void HttpDownload::defaultInfo()
{
	setStatusLabel("");
	progressBarLabel->setText("");
	progressBar->setValue(0);
}

void HttpDownload::setStatusLabel(QString text)
{
	statusLabel->setText(text);
}

void HttpDownload::cfgProgressBar(int min, int max)
{
	progressBar->setRange(min, max);
	progressBar->setValue(0);
}

void HttpDownload::setProgressBarValue(int value, QString text)
{
	progressBar->setValue(value);
	progressBarLabel->setText(text);
}

void HttpDownload::startRequest(const QUrl &m_url, TipoDown tipo, QString contentPost)
{
	url = m_url;
	httpRequestAborted = false;

	if (isProxyEnable)
		qnam.setProxy(proxy);

	QNetworkRequest request;
	request.setSslConfiguration(sslConfiguration);
	request.setUrl(url);
//	request.setRawHeader("User-Agent", "Mozilla/5.0");

	switch (tipo)
	{
		case d_GET:
			reply = qnam.get(request);
		break;
		case d_POST:
			request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
		// content
			QByteArray content;
			content.append(contentPost);
			reply = qnam.post(request, content);
		break;
	}

	connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateDataReadProgress(qint64, qint64)));

	setStatusLabel(tr("Descargando %1").arg(url.toString()));
}

void HttpDownload::downloadMultiFiles(QList<stDownItem> listDown)
{
	listDownload = listDown;
	currentDownload = 0;
	isMultiFiles = true;
	emit nextDownloadFile();
}

void HttpDownload::nextDownloadFile()
{
	if (currentDownload < listDownload.count())
	{
		stDownItem curremtDown = listDownload.at(currentDownload);
		++currentDownload;

		if (!curremtDown.urlfile.isEmpty())
			downloadFile(curremtDown.urlfile, curremtDown.fileName, curremtDown.tipo, curremtDown.contentPost, true);
		else
			emit nextDownloadFile();
	} else {
		emit setStatusBtnDownload(true);
		emit statusFinished();
	}
}

void HttpDownload::downloadFile(QString urlfile, QString fileName, QString tipo, QString contentPost, bool multiFiles)
{
	isMultiFiles = multiFiles;
	if (!isMultiFiles)
		currentDownload = 0;

	const QString urlSpec = urlfile;
	if (urlSpec.isEmpty())
		return;

	if (fileName.isEmpty())
		fileName = "index.html";

	const QUrl newUrl = QUrl::fromUserInput(urlSpec);
	if (!newUrl.isValid()) {
		QMessageBox::information(this, tr("Error"), tr("URL Invalida: %1: %2").arg(urlSpec, newUrl.errorString()));
		return;
	}

	if (QFile::exists(fileName))
	{
//		if (QMessageBox::question(this, m_httpwindowtitle,
//			tr("Ya existe un archivo con el mismo nombre '%1' en el directorio actual.").arg(fileName) +"\n"+
//			tr("¿Quieres sobreescribirlo?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
//			return;
//		QFile::remove(fileName);
	}

	file = openFileForWrite(fileName);
	if (!file)
		return;

	emit setStatusBtnDownload(false);

// schedule the request
	startRequest(newUrl, (tipo == "POST") ? d_POST : d_GET, contentPost);
}

QFile *HttpDownload::openFileForWrite(const QString &fileName)
{
	QScopedPointer<QFile> file(new QFile(fileName));
	if (!file->open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Error"), tr("No se ha podido guardar el archivo %1: %2.").arg(fileName, file->errorString()));
		return 0;
	}
	return file.take();
}

QString HttpDownload::covertBytes(float size)
{
	QStringList list;
	list << "KB" << "MB" << "GB" << "TB";
	QStringListIterator i(list);
	QString unidad("bytes");

	while (size >= 1024.0 && i.hasNext())
	{
		unidad = i.next();
		size /= 1024.0;
	}

	return QString().setNum(size, 'f', 2) +" "+ unidad;
}

void HttpDownload::cancelDownload()
{
	setStatusLabel(tr("Descarga cancelada."));
	httpRequestAborted = true;
	reply->abort();
	emit setStatusBtnDownload(true);
}

void HttpDownload::httpFinished()
{
	QFileInfo fi;
	if (file)
	{
		fi.setFile(file->fileName());
		file->close();
		delete file;
		file = 0;
	}

	if (httpRequestAborted)
	{
		reply->deleteLater();
		reply = 0;
		return;
	}

	if (reply->error())
	{
//		QFile::remove(fi.absoluteFilePath());
		setStatusLabel(tr("Descarga fallida: %1.").arg(reply->errorString()));
		emit setStatusBtnDownload(true);
		reply->deleteLater();
		reply = 0;
		return;
	}

	const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	reply->deleteLater();
	reply = 0;

	if (!redirectionTarget.isNull())
	{
		const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
		if (isConfirmRedirect)
		{
			int respuesta = QMessageBox::question(this, tr("Redireccionar"), tr("¿Redireccionar a '%1'?").arg(redirectedUrl.toString()), QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::No);
			if (respuesta == QMessageBox::No)
			{
			//	QFile::remove(fi.absoluteFilePath());
				emit setStatusBtnDownload(true);
				setStatusLabel(tr("Descarga fallida: Redirigir rechazado."));
				return;
			}
			if (respuesta == QMessageBox::YesAll)
				isConfirmRedirect = false;
		}

		file = openFileForWrite(fi.absoluteFilePath());
		if (!file)
		{
			emit setStatusBtnDownload(true);
			return;
		}
		startRequest(redirectedUrl);
		return;
	}

	setStatusLabel(tr("Descargado %1 de %2 en %3").arg(covertBytes(fi.size())).arg(fi.fileName(), fi.absolutePath()));

	if (isMultiFiles)
		nextDownloadFile();
	else {
		emit setStatusBtnDownload(true);
		emit statusFinished();
	}
}

void HttpDownload::httpReadyRead()
{
// this slot gets called every time the QNetworkReply has new data.
// We read all of its new data and write it into the file.
// That way we use less RAM than when reading it at the finished()
// signal of the QNetworkReply
	if (file)
		file->write(reply->readAll());
}

void HttpDownload::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	if (httpRequestAborted)
		return;

	progressBar->setMaximum(totalBytes);
	setProgressBarValue(bytesRead, tr("Descargando")+ ": "+ covertBytes(bytesRead));
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

	if (dlg.exec() == QDialog::Accepted)
	{
		authenticator->setUser(ui.userEdit->text());
		authenticator->setPassword(ui.passwordEdit->text());
	}
}

#ifndef QT_NO_SSL //QT_NO_OPENSSL
void HttpDownload::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
	QString errorString;
	const int listErrorsSize = errors.size();
	for (int i = 0; i < listErrorsSize; ++i)
	{
		if (!errorString.isEmpty())
			errorString += ", ";
		errorString += errors.at(i).errorString();
	}

	if (QMessageBox::warning(this, m_httpwindowtitle, tr("Uno o más errores de SSL se ha producido: %1").arg(errorString),
	        QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore)
	{
		reply->ignoreSslErrors();
	}
}
#endif
