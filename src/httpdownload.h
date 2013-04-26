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

#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

#include <QtGui>
#include <QWidget>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QFile;
class QProgressDialog;
class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class HttpDownload : public QWidget
{
	Q_OBJECT

public:
	HttpDownload(QWidget *parent = 0);
	~HttpDownload();

	enum TipoDown
	{
		d_GET  = 0,
		d_POST = 1
	};

	void setHttpProxy(int typeProxy, const QString host, int port, const QString username = "", const QString password = "");
	void setHttpWindowTitle(QString titulo = "GR-lida - HttpDownload"){ m_httpwindowtitle = titulo; }

	void downloadFile(QString urlfile, QString fileName, QString tipo = "GET", QString contentPost = "");
	void startRequest(QUrl m_url, TipoDown tipo = d_GET, QString contentPost = "");

	QString getStatusLabel(){ return m_statuslabel; }
	bool getStatusBtnDownload(){ return m_downloadButton; }

signals:
	void statusLabel(QString str);
	void statusBtnEnabled(bool mbool);
	void statusFinished();

private:
	QUrl url;
	QNetworkAccessManager qnam;
	QNetworkReply *reply;
	QNetworkProxy proxy;
	QProgressDialog *progressDialog;
	QFile *file;

	QString m_httpwindowtitle;
	QString m_statuslabel;

	int httpGetId;
	bool m_downloadButton, httpRequestAborted, isProxyEnable;

	void setStatusLabel(QString str);
	void setStatusBtnDownload(bool mbool);

public slots:
	void cancelDownload();

private slots:
	void httpFinished();
	void httpReadyRead();
	void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
	void slotAuthenticationRequired(QNetworkReply*, QAuthenticator*);
#ifndef QT_NO_OPENSSL
	void sslErrors(QNetworkReply*, const QList<QSslError> &errors);
#endif

};

#endif // HTTPDOWNLOAD_H
