/*
 *
 * Copyright (C) 2014 Pedro A. Garcia Rosado Aka Monthy
 * Contact: http://www.gr-lida.org/
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Contact: http://www.qt-project.org/legal
 *
 * $QT_BEGIN_LICENSE:GPL$
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * $QT_END_LICENSE$
 *
 *
**/

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
class Highlighter;
QT_END_NAMESPACE

class CodeEditor: public QPlainTextEdit
{
	Q_OBJECT
public:
	typedef enum {
		Normal,
		Number,
		NumberCss,
		String,
		StringHtml,
		Comment,
		CommentDoc,
		Regex,
		RegexCss,
		RegexHtml,
		Operator,
		Keyword,
		KeywordCss,
		KeywordHtml,
		BuiltIn,
		BuiltInCss,
		BuiltInHtml,
		Classes,
		ClassesCss,
		Other,
		Marker,
		Identifier,
	//--
		Background,
		NumberArea,
		LineNumber,
		LineSelect
	} HighlightType;

	typedef enum {
		JavaScript,
		Html,
		Css,
	} SyntaxType;

	CodeEditor(QWidget *parent = 0, SyntaxType type = Html);
	~CodeEditor();

	void setSyntaxType(SyntaxType type);
	void setFormatFor(HighlightType type, const QColor &colorFg, const QColor &colorBg = Qt::transparent, bool bold = false, bool italic = false, bool underline = false);
	void setColor(HighlightType type, const QColor &color);

	bool isLineNumbersVisible() const;
	bool isTextWrapEnabled() const;
	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

public slots:
	void mark(const QString &str, Qt::CaseSensitivity sens = Qt::CaseInsensitive);
	void setLineNumbersVisible(bool visible);
	void setTextWrapEnabled(bool enable);

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void highlightCurrentLine();

private:
	SyntaxType syntaxtype;
	Highlighter *highlighter;
	QWidget *lineNumberArea;
	QFont font;
	QColor numberAreaColor;
	QColor lineNumberColor;
	QColor lineSelectColor;
	bool showLineNumbers;
	bool textWrap;

};

#endif // CODEEDITOR_H
