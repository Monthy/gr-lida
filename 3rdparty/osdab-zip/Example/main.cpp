/****************************************************************************
** Filename: main.cpp
** Last updated [dd/mm/yyyy]: 27/03/2011
**
** Test routine for the Zip and UnZip classed.
**
** Copyright (C) 2007-2012 Angius Fabrizio. All rights reserved.
**
** This file is part of the OSDaB project (http://osdab.42cows.org/).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See the file LICENSE.GPL that came with this software distribution or
** visit http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
**********************************************************************/

#include "zip.h"
#include "unzip.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <iostream>
#include <iomanip>
#include <stdlib.h>

void invalidCMD();
bool decompress(const QString& file, const QString& out, const QString& pwd);
bool compress(const QString& zip, const QString& dir, const QString& pwd);
bool listFiles(const QString& file, const QString& pwd);
bool verifyArchive(const QString& file, const QString& pwd);

using namespace std;

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Test routine for the OSDaB Project Zip/UnZip classes" << endl << endl;
		cout << "Compression: zip [-p PWD] ZIPFILE DIRECTORY" << endl;
        cout << "Decompression: zip -d [-p PWD] ZIPFILE OUTPUT_DIR" << endl << endl;
        cout << "Show Archive Contents: zip -l [-p PWD] ZIPFILE" << endl;
        cout << "Verify Archive: zip -v [-p PWD] ZIPFILE" << endl << endl;
        cout << "(C) 2007-2012 Angius Fabrizio\nLicensed under the terms of the GNU GPL Version 2 or later" << endl;
		return -1;
	}

	QString fname;
	QString dname;
	QString pwd;

	bool resOK = true;

	if (strlen(argv[1]) == 2 &&	argv[1][0] == '-') 	{
		switch (argv[1][1]) {
        case 'd': {
            if (argc >= 6) {
                if (strcmp(argv[2], "-p") == 0) {
                    pwd = QString(argv[3]);
                    fname = QString(argv[4]);
                    dname = QString(argv[5]);
                } else invalidCMD();
            } else if (argc >= 4) {
                fname = QString(argv[2]);
                dname = QString(argv[3]);
            } else invalidCMD();

            resOK = decompress(fname, dname, pwd);
        } break;

        case 'l': {
            if (argc >= 5) {
                if (strcmp(argv[2], "-p") == 0) {
                    pwd = QString(argv[3]);
                    fname = QString(argv[4]);
                } else invalidCMD();
            } else if (argc >= 3) {
                fname = QString(argv[2]);
            } else invalidCMD();

            resOK = listFiles(fname, pwd);
        } break;

        case 'p': {
            if (argc >= 5) {
                pwd = QString(argv[2]);
                fname = QString(argv[3]);
                dname = QString(argv[4]);
            } else invalidCMD();

            resOK = compress(fname, dname, pwd);
        } break;

        case 'v': {
            if (argc >= 5) {
                if (strcmp(argv[2], "-p") == 0) {
                    pwd = QString(argv[3]);
                    fname = QString(argv[4]);
                } else invalidCMD();
            } else if (argc >= 3) {
                fname = QString(argv[2]);
            } else invalidCMD();

            resOK = verifyArchive(fname, pwd);
        } break;

        default: invalidCMD();
		}

	} else {
		// no parameters -- compress directly
		resOK = compress(QString(argv[1]), QString(argv[2]), 0);
	}

    return resOK ? EXIT_SUCCESS : EXIT_FAILURE;
}

void invalidCMD()
{
    cout << "Invalid command line. Usage:" << endl;
    cout << "Compression: zip [-p PWD] DIRECTORY" << endl;
    cout << "List files: zip -l [-p PWD] ZIPFILE" << endl;
    cout << "Decompression: zip -d [-p PWD] ZIPFILE OUTPUT_DIR" << endl << endl;
    exit(-1);
}

bool decompress(const QString& file, const QString& out, const QString& pwd)
{

	if (!QFile::exists(file)) {
		cout << "File does not exist." << endl << endl;
		return false;
	}

	UnZip::ErrorCode ec;
	UnZip uz;

	if (!pwd.isEmpty())
		uz.setPassword(pwd);

	ec = uz.openArchive(file);
	if (ec != UnZip::Ok) {
		cout << "Failed to open archive: " << uz.formatError(ec).toAscii().data() << endl << endl;
		return false;
	}

	ec = uz.extractAll(out);
	if (ec != UnZip::Ok) {
		cout << "Extraction failed: " << uz.formatError(ec).toAscii().data() << endl << endl;
		uz.closeArchive();
		return false;
	}

	return true;
}

bool compress(const QString& zip, const QString& dir, const QString& pwd)
{
	QFileInfo fi(dir);
	if (!fi.isDir()) {
		cout << "Directory does not exist." << endl << endl;
		return false;
	}

	Zip::ErrorCode ec;
	Zip uz;

	ec = uz.createArchive(zip);
	if (ec != Zip::Ok) {
		cout << "Unable to create archive: " << uz.formatError(ec).toAscii().data() << endl << endl;
		return false;
	}

    uz.setPassword(pwd);
    ec = uz.addDirectory(dir);
	if (ec != Zip::Ok) {
		cout << "Unable to add directory: " << uz.formatError(ec).toAscii().data() << endl << endl;
	}

	uz.setArchiveComment("This archive has been created using OSDaB Zip (http://osdab.42cows.org/).");

	if (uz.closeArchive() != Zip::Ok) {
		cout << "Unable to close the archive: " << uz.formatError(ec).toAscii().data() << endl << endl;
	}

	return ec == Zip::Ok;
}

bool listFiles(const QString& file, const QString& pwd)
{
	if (!QFile::exists(file)) {
		cout << "File does not exist." << endl << endl;
		return false;
	}

	UnZip::ErrorCode ec;
	UnZip uz;

	if (!pwd.isEmpty())
		uz.setPassword(pwd);

	ec = uz.openArchive(file);
	if (ec != UnZip::Ok) {
		cout << "Unable to open archive: " << uz.formatError(ec).toAscii().data() << endl << endl;
		return false;
	}

	QString comment = uz.archiveComment();
	if (!comment.isEmpty())
		cout << "Archive comment: " << comment.toAscii().data() << endl << endl;

	QList<UnZip::ZipEntry> list = uz.entryList();
	if (list.isEmpty()) {
		cout << "Empty archive.";
	} else {
		cout.setf(ios::left);
		cout << setw(40) << "Filename";
		cout.unsetf(ios::left);
		cout << setw(10) << "Size" << setw(10) << "Ratio" << setw(10) << "CRC32" << endl;
		cout.setf(ios::left);
		cout << setw(40) << "--------";
		cout.unsetf(ios::left);
		cout << setw(10) << "----" << setw(10) << "-----" << setw(10) << "-----" << endl;

		for (int i = 0; i < list.size(); ++i) {
			const UnZip::ZipEntry& entry = list.at(i);

			double ratio = entry.uncompressedSize == 0 ? 0 : 100 - (double) entry.compressedSize * 100 / (double) entry.uncompressedSize;

			QString ratioS = QString::number(ratio, 'f', 2).append("%");
			QString crc;
			crc = crc.sprintf("%X", entry.crc32).rightJustified(8, '0');
			QString file = entry.filename;
			int idx = file.lastIndexOf("/");
			if (idx >= 0 && idx != file.length()-1)
				file = file.right(file.length() - idx - 1);
			file = file.leftJustified(40, ' ', true);

			if (entry.encrypted)
				file.append("*");

			cout << setw(40) << file.toAscii().data() << setw(10) << entry.uncompressedSize << setw(10) << ratioS.toAscii().data() << setw(10) << crc.toAscii().data() << endl;
		}
	}

	uz.closeArchive();
	return true;
}

bool verifyArchive(const QString& file, const QString& pwd)
{
    if (!QFile::exists(file)) {
        cout << "File does not exist." << endl << endl;
        return false;
    }

    UnZip uz;
    UnZip::ErrorCode ec =UnZip::Ok;

    if (!pwd.isEmpty())
        uz.setPassword(pwd);

    ec = uz.openArchive(file);
    if (ec != UnZip::Ok) {
        cout << "Unable to open archive: " << uz.formatError(ec).toAscii().data() << endl << endl;
        return false;
    }

    ec = uz.verifyArchive();
    switch (ec) {
    case UnZip::WrongPassword:
        cout << "Wrong password." << endl << endl;
        break;
    case UnZip::PartiallyCorrupted:
        cout << "Corrupted entries found." << endl << endl;
        break;
    case UnZip::Corrupted:
        cout << "Corrupted archive." << endl << endl;
        break;
    case UnZip::Ok:
        cout << "No problem found." << endl << endl;
        break;
    default:
        cout << "An error occurred: " << (int)ec << endl << endl;
    }

    uz.closeArchive();
    return ec == UnZip::Ok;
}
