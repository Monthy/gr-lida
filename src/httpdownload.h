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

#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

#include <QtDebug>

#include <QWidget>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
class QFile;
class QLabel;
class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

struct stDownItem {
	QString urlfile;
	QString fileName;
	QString tipo;
	QString contentPost;
};

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

	void setHttpProxy(int typeProxy, const QString host, quint16 port, const QString username = "", const QString password = "");
	void setHttpWindowTitle(QString titulo = "GR-lida - HttpDownload"){ m_httpwindowtitle = titulo; }
	void setConfirmRedirect(bool confirmRedirect) { isConfirmRedirect = confirmRedirect; }

	void downloadMultiFiles(QList<stDownItem> listDown);
	void downloadFile(QString urlfile, QString fileName, QString tipo = "GET", QString contentPost = "", bool multiFiles = false);

	bool getStatusBtnDownload(){ return m_downloadButton; }
	void defaultInfo();
	void setStatusLabel(QString text);

	void cfgProgressBar(int min, int max);
	void setProgressBarValue(int value, QString text);

signals:
	void statusBtnEnabled(bool mbool);
	void statusFinished();

private:
	QUrl url;
	QNetworkAccessManager qnam;
	QNetworkReply *reply;
	QNetworkProxy proxy;
	QSslConfiguration sslConfiguration;
	QList<stDownItem> listDownload;
	QProgressBar *progressBar;
	QFile *file;
	QLabel *statusLabel;
	QLabel *progressBarLabel;
	int currentDownload;
	bool isMultiFiles, isConfirmRedirect;

	QString m_httpwindowtitle;
	bool m_downloadButton, httpRequestAborted, isProxyEnable;

	void startRequest(const QUrl &m_url, TipoDown tipo = d_GET, QString contentPost = "");
	QFile *openFileForWrite(const QString &fileName);
	QString covertBytes(float size);

public slots:
	void cancelDownload();

private slots:
	void setStatusBtnDownload(bool mbool);
	void nextDownloadFile();

	void httpFinished();
	void httpReadyRead();
	void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
	void slotAuthenticationRequired(QNetworkReply*, QAuthenticator*);
#ifndef QT_NO_OPENSSL
	void sslErrors(QNetworkReply*, const QList<QSslError> &errors);
#endif

};

#endif // HTTPDOWNLOAD_H
