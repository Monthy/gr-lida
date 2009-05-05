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

#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

#include <QtGui/QWidget>

class QFile;
class QHttp;
class QHttpResponseHeader;
class QProgressDialog;
class QSslError;
class QAuthenticator;

class HttpDownload : public QWidget
{
    Q_OBJECT

public:
	HttpDownload(QWidget *parent = 0);
	~HttpDownload();

	void downloadFile(QString urlfile, QString fileName, QString metodo = "", QString contentPost = "");

	QString getStatusLabel(){ return m_statuslabel; };
	bool getStatusBtnDownload(){ return m_downloadButton; };
	void setHttpProxy(int typeProxy, const QString host, int port, const QString username = "", const QString password = "");
	void setHttpWindowTitle(QString titulo = "HttpDownload");

private:
	QProgressDialog *progressDialog;

	QString m_httpwindowtitle;
	QString m_statuslabel;
	QString urlLineEdit;
	bool m_downloadButton;

	QHttp *http;
	QFile *file;
	int httpGetId;
	bool httpRequestAborted;

	void setStatusLabel(QString str);
	void setStatusBtnDownload(bool mbool);

private slots:
	void cancelDownload();

	void httpRequestFinished(int requestId, bool error);
	void readResponseHeader(const QHttpResponseHeader &responseHeader);
	void updateDataReadProgress(int bytesRead, int totalBytes);
	void httpstateChanged(int state);
	void slotAuthenticationRequired(const QString &, quint16, QAuthenticator *);
#ifndef QT_NO_OPENSSL
	void sslErrors(const QList<QSslError> &errors);
#endif

signals:
	void statusLabelChanged(QString str);
	void StatusBtnDownloadChanged(bool mbool);
	void StatusRequestFinished();

};

#endif // HTTPDOWNLOAD_H
