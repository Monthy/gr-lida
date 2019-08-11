/*
 *
 * Copyright (C) 2015-2017 Pedro A. Garcia Rosado Aka Monthy
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

#include "codeeditor.h"

#include <QSyntaxHighlighter>
#include <QAbstractItemView>
#include <QPainter>
#include <QStringListModel>
#include <QScrollBar>
#include <QFile>

class Highlighter : public QSyntaxHighlighter
{
public:
	Highlighter(CodeEditor::SyntaxType type, QTextDocument *parent = 0);
	void setSyntaxType(CodeEditor::SyntaxType type);
	void setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg = Qt::transparent, bool bold = false, bool italic = false, bool underline = false);
	void setColor(CodeEditor::HighlightType type, const QColor &color);
	void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

protected:
	void highlightBlock(const QString &text);
	void highlightBlockJavaScriptAndCss(const QString &text, int m_state);
	void highlightBlockHtml(const QString &text);

private:
	enum {
		Normal = -1,
		Number = 0,
		String = 1,
		Comment = 2,
		CommentDoc = 3,
		Regex = 4,
		Identifier = 5,
		isJs = 6,
		isCss = 7
	};
	enum {
		jsNormal = -1,
		jsNumber = 0,
		jsString = 1,
		jsComment = 2,
		jsCommentDoc = 3,
		jsRegex = 4,
		jsIdentifier = 5
	};

	CodeEditor::SyntaxType m_syntaxtype;
	QSet<QString> m_keywords;
	QSet<QString> m_keywordsCss;
	QSet<QString> m_builtin;
	QSet<QString> m_builtinCss;
	QSet<QString> m_classes;
	QSet<QString> m_other;
	bool isScript, isStyle;
	int tmp_state;

	QHash<CodeEditor::HighlightType, QTextCharFormat> m_formats;
	QString m_markString;
	Qt::CaseSensitivity m_markCaseSensitivity;
};

Highlighter::Highlighter(CodeEditor::SyntaxType type, QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	m_markCaseSensitivity = Qt::CaseInsensitive;

// Obsidian theme
	setFormatFor(CodeEditor::Normal     , QColor("#e0e2e4"));
	setFormatFor(CodeEditor::Number     , QColor("#ffcd22"));
	setFormatFor(CodeEditor::NumberCss  , QColor("#ffcd22"));
	setFormatFor(CodeEditor::String     , QColor("#ec7600"));
	setFormatFor(CodeEditor::StringHtml , QColor("#ec7600"));
	setFormatFor(CodeEditor::Comment    , QColor("#66747a"));
	setFormatFor(CodeEditor::CommentDoc , QColor("#66747a"));
	setFormatFor(CodeEditor::Regex      , QColor("#F74692"));
	setFormatFor(CodeEditor::RegexCss   , QColor("#F74692"));
	setFormatFor(CodeEditor::RegexHtml  , QColor("#F74692"));
	setFormatFor(CodeEditor::Operator   , QColor("#e0e2e4"));
	setFormatFor(CodeEditor::Keyword    , QColor("#93c763"), Qt::transparent, true);
	setFormatFor(CodeEditor::KeywordCss , QColor("#668cb0"));
	setFormatFor(CodeEditor::KeywordHtml, QColor("#668cb0"), Qt::transparent, true);
	setFormatFor(CodeEditor::BuiltIn    , QColor("#93c763"), Qt::transparent, true);
	setFormatFor(CodeEditor::BuiltInCss , QColor("#93c763"), Qt::transparent, true);
	setFormatFor(CodeEditor::BuiltInHtml, QColor("#b3b689"), Qt::transparent, true);
	setFormatFor(CodeEditor::Classes    , QColor("#668cb0"), Qt::transparent, true);
	setFormatFor(CodeEditor::ClassesCss , QColor("#93c763"), Qt::transparent, true);
	setFormatFor(CodeEditor::Other      , QColor("#ff8000"));
	setFormatFor(CodeEditor::Marker     , QColor("#e0e2e4"), QColor("#404e51"));
	setFormatFor(CodeEditor::Identifier , QColor("#d5ab55"));

// https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
	m_keywords << "abstract" << "boolean" << "break" << "byte" << "case" << "catch"
		<< "char" << "class" << "const" << "continue" << "debugger" << "default"
		<< "delete" << "do" << "double" << "else" << "enum" << "export" << "extends"
		<< "false" << "final" << "finally" << "float" << "for" << "function" << "goto"
		<< "if" << "implements" << "import" << "in" << "instanceof" << "int" << "interface"
		<< "long" << "native" << "new" << "null" << "package" << "private" << "protected"
		<< "public" << "return" << "short" << "static" << "super" << "switch"<< "synchronized"
		<< "this" << "throw" << "throws" << "transient" << "true" << "try"<< "typeof" << "var"
		<< "void" << "while" << "with";

	m_keywordsCss << "absolute" << "add" << "adjoins" << "align" << "alternate" << "alternating"
		<< "area" << "arrow" << "attachment" << "background" << "bar" << "base" << "border"
		<< "bottom" << "branch" << "bright" << "button" << "buttons" << "center" << "character"
		<< "children" << "chunk" << "clip" << "close" << "color" << "colors" << "content"
		<< "corner" << "dark" << "dash" << "decoration" << "dot" << "down" << "drop" << "empty"
		<< "face" << "family" << "flags" << "float" << "font" << "for" << "gridline" << "handle"
		<< "has" << "have" << "height" << "highlight" << "highlighted" << "icon" << "icons"
		<< "image" << "indicator" << "interaction" << "item" << "layout" << "left" << "light"
		<< "line" << "lineedit" << "link" << "margin" << "max" << "menu" << "messagebox" << "mid"
		<< "midlight" << "min" << "none" << "opacity" << "origin" << "padding" << "page" << "paint"
		<< "pane" << "password" << "position" << "radius" << "relative" << "right" << "row"
		<< "scroller" << "section" << "selection" << "separator" << "shadow" << "show" << "siblings"
		<< "size" << "spacing" << "spread" << "stop" << "style" << "sub" << "subcontrol" << "tab"
		<< "tear" << "tearoff" << "text" << "title" << "top" << "up" << "vertical" << "visited"
		<< "weight" << "width" << "window" << "overflow" << "display" << "cursor" << "white-space"
		<< "clear" << "list-style";

// built-in and other popular objects + properties
	m_builtin << "abs" << "acos" << "apply" << "arguments" << "arity" << "Array" << "asin" << "atan"
		<< "atan2" << "bind" << "call" << "caller" << "ceil" << "charAt" << "charCodeAt" << "concat"
		<< "constructor" << "cos" << "create" << "decodeURI" << "decodeURIComponent" << "defineProperties"
		<< "defineProperty" << "document" << "E" << "encodeURI" << "encodeURIComponent" << "eval" << "every"
		<< "exec" << "exp" << "filter" << "floor" << "forEach" << "fromCharCode" << "Function"
		<< "getOwnPropertyDescriptor" << "getOwnPropertyNames" << "global" << "hasOwnProperty"
		<< "ignoreCase" << "index" << "indexOf" << "Infinity" << "input" << "isArray" << "isFinite"
		<< "isNaN" << "isPrototypeOf" << "join" << "JSON" << "keys" << "lastIndex" << "lastIndexOf"
		<< "length" << "LN10" << "LN2" << "localCompare" << "log" << "LOG10E" << "LOG2E" << "map"
		<< "match" << "Math" << "max" << "min" << "multiline" << "name" << "NaN" << "navigator"
		<< "Object" << "parse" << "parseFloat" << "parseInt"<< "PI" << "pop" << "pow" << "propertyIsEnumerable"
		<< "prototype" << "push" << "quote" << "random" << "reduce" << "reduceRight" << "RegExp" << "replace"
		<< "reverse" << "round" << "search" << "shift" << "sin" << "slice" << "some" << "sort" << "source"
		<< "splice" << "split" << "sqrt" << "SQRT1_2" << "SQRT2" << "String" << "stringify"<< "substr"
		<< "substring" << "tan" << "test"<< "toLocaleLowerCase" << "toLocaleString" << "toLocaleUpperCase"
		<< "toLowerCase" << "toSource" << "toString" << "toUpperCase"<< "trim" << "trimLeft" << "trimRight"
		<< "undefined" << "unshift" << "unwatch" << "userAgent" << "valueOf" << "watch" << "window"
		<< "__defineGetter__" << "__defineSetter__" << "__lookupGetter__" << "__lookupSetter__"
		<< "__noSuchMethod__" << "__parent__" << "__proto__";

	m_builtinCss << "a" << "b" << "black" << "body" << "bold" << "br" << "button" << "checked" << "darkgray"
		<< "dashed" << "dd" << "del" << "div" << "dl" << "dotted" << "double" << "dt" << "editable"
		<< "em" << "fieldset" << "first" << "form" << "frame" << "frameset" << "gray" << "groove" << "x" << "y"
		<< "h1" << "h2" << "h3" << "h4" << "h5" << "h6" << "head" << "hr" << "hsv" << "hsva" << "html"
		<< "i" << "iframe" << "img" << "input" << "inset" << "italic" << "label" << "last" << "legend"
		<< "li" << "link" << "middle" << "no-repeat" << "ol" << "optgroup" << "option" << "outset" << "z"
		<< "p" << "pre" << "pt" << "px" << "qlineargradient" << "repeat" << "repeat-x" << "repeat-y"
		<< "rgb" << "rgba" << "ridge" << "s" << "select" << "solid" << "span" << "strong" << "sub"
		<< "summary" << "sup" << "table" << "tbody" << "td" << "textarea" << "tfoot" << "th" << "thead"
		<< "tr" << "transparent" << "tt" << "u" << "ul"<< "unchecked" << "url" << "white" << "no" << "hidden"
		<< "block" << "pointer" << "normal" << "inline" << "auto" << "center" << "nowrap" << "both" << "none";

	m_classes << "QAbstractScrollArea" << "QCheckBox" << "QColumnView" << "QComboBox" << "QDateEdit"
		<< "QTextBrowser" << "QDateTimeEdit" << "QDialog" << "QDialogButtonBox" << "QDockWidget"
		<< "QDoubleSpinBox" << "QFrame" << "QGraphicsView" << "QGroupBox" << "QHeaderView"
		<< "QLabel" << "QLineEdit"<< "QListView" << "QListWidget" << "QMainWindow" << "QMenu"
		<< "QMenuBar" << "QMessageBox" << "QProgressBar" << "QPushButton" << "QRadioButton"
		<< "QScrollBar" << "QSizeGrip" << "QSlider" << "QSpinBox" << "QSplitter" << "QStatusBar"
		<< "QTabBar" << "QTabWidget" << "QTableView" << "QTableWidget" << "QTextEdit" << "QTimeEdit"
		<< "QToolBar" << "QToolButton" << "QToolBox" << "QToolTip" << "QTreeView" << "QTreeWidget"
		<< "QWidget" << "QPlainTextEdit" << "Spacer" << "QCommandLinkButton" << "QStackedWidget"
		<< "QMdiArea" << "QDial" << "QCalendarWidget" << "QLCDNumber" << "Line" << "QDeclarativeView"
		<< "QWebView" << "QAbstractItemView";

	m_other << "closed" << "focus" << "hover" << "open" << "pad" << "pane" << "pressed" << "selected";

	setSyntaxType(type);
}

void Highlighter::setSyntaxType(CodeEditor::SyntaxType type)
{
	m_syntaxtype = type;
	rehighlight();
}

void Highlighter::setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg, bool bold, bool italic, bool underline)
{
	m_formats[type].setForeground(colorFg);
	m_formats[type].setBackground(colorBg);
	m_formats[type].setFontWeight(bold ? QFont::Bold : QFont::Normal);
	m_formats[type].setFontItalic(italic);
	m_formats[type].setFontUnderline(underline);
}

void Highlighter::setColor(CodeEditor::HighlightType type, const QColor &color)
{
	setFormatFor(type, color);
	rehighlight();
}

void Highlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
{
	m_markString = str;
	m_markCaseSensitivity = caseSensitivity;
	rehighlight();
}

void Highlighter::highlightBlock(const QString &text)
{
	if (m_syntaxtype == CodeEditor::Html)
		highlightBlockHtml(text);
	else
		highlightBlockJavaScriptAndCss(text, m_syntaxtype == CodeEditor::Css ? isCss : isJs);

	if (!m_markString.isEmpty())
	{
		int m_pos = 0;
		int m_len = m_markString.length();
		for (;;)
		{
			m_pos = text.indexOf(m_markString, m_pos, m_markCaseSensitivity);
			if (m_pos < 0)
				break;
			setFormat(m_pos, m_len, m_formats[CodeEditor::Marker]);
			++m_pos;
		}
	}
}

void Highlighter::highlightBlockJavaScriptAndCss(const QString &text, int m_state)
{
	int js_state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;
	while (pos <= len)
	{
		QChar ch = (pos < len) ? text.at(pos) : QChar();
		QChar next = (pos < len - 1) ? text.at(pos + 1) : QChar();
		switch (js_state)
		{
			case jsNormal:
			default:
				start = pos;
				setFormat(start, len, m_formats[CodeEditor::Normal]);
				if (ch.isSpace()) {
					++pos;
				} else if (ch.isDigit()) {
					++pos;
					js_state = jsNumber;
				} else if (ch == '-' && m_state == isCss) {
					setFormat(start, 1, m_formats[CodeEditor::BuiltInCss]);
					++pos;
					js_state = jsNormal;
				} else if (ch == '.' && m_state == isCss) {
					start = pos;
					while (pos < len)
					{
						if (QString("<>(){}[] ,;").contains(text.at(pos)) || text.at(pos) == '\n')
						{
							js_state = jsNormal;
							break;
						} else
							++pos;
					}
					setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
				} else if (ch == '#') {
					QRegExp expression("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})");
					int index = expression.indexIn(text, pos);
					if (index >= 0)
					{
						int length = expression.matchedLength();
						setFormat(start, length, m_formats[CodeEditor::Identifier]);
						pos += length;
						js_state = jsNormal;
					} else {
						start = pos;
						while (pos < len) {
							if (QString("<>(){}[] ,;").contains(text.at(pos)) || text.at(pos) == '\n') {
								js_state = jsNormal;
								break;
							}
							++pos;
						}
						setFormat(start, pos - start, m_formats[m_state == isCss ? CodeEditor::BuiltInCss : CodeEditor::Operator]);
					}
				} else if (ch.isLetter() || ch == '_') {
					++pos;
					js_state = jsIdentifier;
				} else if (ch == '\'' || ch == '\"') {
					++pos;
					js_state = jsString;
				} else if (ch == '/' && next == '/') {
					pos = len;
					setFormat(start, len, m_formats[CodeEditor::Comment]);
				} else if (text.mid(pos, 3) == "/**") {
					js_state = jsCommentDoc;
				} else if (ch == '/' && next == '*') {
					++pos;
					++pos;
					js_state = jsComment;
				} else if (ch == '(' && next != '*' && m_state == isCss) {
					++pos;
					js_state = jsRegex;
				} else if (ch == '/' && next != '*') {
					++pos;
					js_state = jsRegex;
				} else {
				//	if (!QString("(){}[]").contains(ch))
					setFormat(start, 1, m_formats[CodeEditor::Operator]);
					if (ch == '-' && m_state == isCss)
						setFormat(start, 1, m_formats[CodeEditor::KeywordCss]);
					++pos;
					js_state = jsNormal;
				}
			break;
			case jsNumber:
				if (ch.isSpace() || !ch.isDigit())
				{
					setFormat(start, pos - start, m_formats[m_state == isCss ? CodeEditor::NumberCss : CodeEditor::Number]);
					js_state = jsNormal;
				} else
					++pos;
			break;
			case jsIdentifier:
				if (ch.isSpace() || !(ch.isLetter() || ch == '_'))
				{
					QString token = text.mid(start, pos - start).trimmed();
					if (m_keywords.contains(token))
						setFormat(start, pos - start, m_formats[CodeEditor::Keyword]);
					if (m_keywordsCss.contains(token) && m_state == isCss)
						setFormat(start, pos - start, m_formats[CodeEditor::KeywordCss]);
					if (m_builtin.contains(token))
						setFormat(start, pos - start, m_formats[CodeEditor::BuiltIn]);
					if (m_builtinCss.contains(token) && m_state == isCss)
						setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
					if (m_classes.contains(token))
						setFormat(start, pos - start, m_formats[m_state == isCss ? CodeEditor::ClassesCss : CodeEditor::Classes]);
					if (m_other.contains(token))
						setFormat(start, pos - start, m_formats[CodeEditor::Other]);
					js_state = jsNormal;
				} else
					++pos;
			break;
			case jsString:
				if (ch == text.at(start))
				{
				//	QChar prev = (pos > 0) ? text.at(pos - 1) : QChar();
					++pos;
				//	if (prev != '\\') {
						setFormat(start, pos - start, m_formats[CodeEditor::String]);
						js_state = jsNormal;
				//	}
				} else
					++pos;
			break;
			case jsComment:
			case jsCommentDoc:
				++pos;
				if (ch == '*' && next == '/')
				{
					++pos;
					setFormat(start, pos - start, m_formats[(js_state == CommentDoc ? CodeEditor::CommentDoc : CodeEditor::Comment)]);
					js_state = jsNormal;
				}
			break;
			case jsRegex:
				if (m_state == isCss)
				{
					++pos;
					setFormat(start, pos - start, m_formats[CodeEditor::RegexCss]);
					setFormat(start, 1, m_formats[CodeEditor::Operator]);
					if (next == ')')
						js_state = jsNormal;
				} else {
					if (ch == '/')
					{
						QChar prev = (pos > 0) ? text.at(pos - 1) : QChar();
						++pos;
						if (prev != '\\') {
							setFormat(start, pos - start, m_formats[CodeEditor::Regex]);
							js_state = jsNormal;
						}
					} else
						++pos;
				}
			break;
		}
	}

	if (js_state == jsComment)
		setFormat(start, len, m_formats[CodeEditor::Comment]);
	else if (js_state == jsCommentDoc)
		setFormat(start, len, m_formats[CodeEditor::CommentDoc]);
	else
		js_state = jsNormal;

	setCurrentBlockState(js_state);
}

void Highlighter::highlightBlockHtml(const QString &text)
{
	int c_state = -1;
	int state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;
	isScript = false;
	isStyle  = false;

	while (pos < len)
	{
		switch (state)
		{
			case Normal:
			case isJs:
			case isCss:
			default:
				if (state == isJs || state == isCss)
				{
					int m_state = state;
					start = pos;
					while (pos < len)
					{
						if (text.mid(pos, 3) == "/**")
						{
							tmp_state = m_state;
							state = CommentDoc;
							break;
						} else if (text.mid(pos, 2) == "/*") {
							tmp_state = m_state;
							state = Comment;
							break;
						} else if (text.mid(pos, 8) == "</script" || text.mid(pos, 7) == "</style") {
							state    = Normal;
							isScript = false;
							isStyle  = false;
							break;
						} else
							++pos;
					}
					if (state == Normal)
						setFormat(start, pos - start, m_formats[CodeEditor::Normal]);
					else
						highlightBlockJavaScriptAndCss(text, m_state);
				} else {
					while (pos < len)
					{
						QChar ch = text.at(pos);
						if ((ch == '<' && state == Normal) || (text.mid(pos, 2) == "/*" && (state == isJs || state == isCss)))
						{
							if (text.mid(pos, 7) == "<script" || text.mid(pos, 6) == "<style")
							{
								if (text.mid(pos, 7) == "<script")
									isScript = true;
								if (text.mid(pos, 6) == "<style")
									isStyle = true;
							}

							if (text.mid(pos, 3) == "/**" && (state == isJs || state == isCss))
							{
								tmp_state = state;
								state = CommentDoc;
							} else if (text.mid(pos, 2) == "/*" && (state == isJs || state == isCss)) {
								tmp_state = state;
								state = Comment;
							} else if (text.mid(pos, 4) == "<!--") {
								state = Comment;
							} else {
								state = Identifier;
								start = pos;
								while (pos < len && text.at(pos) != ' ' && text.at(pos) != '>' && text.at(pos) != '\t' && text.mid(pos, 2) != "/>")
									++pos;
								if (text.mid(pos, 2) == "/>")
									++pos;
								setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
								break;
							}
							break;
						} else if (ch == '&' && state == Normal) {
							start = pos;
							while (pos < len && text.at(pos++) != ';')
								;
							setFormat(start, pos - start, m_formats[CodeEditor::RegexHtml]);
						} else {
							// No tag, comment or entity started, continue...
							++pos;
						}
					}
				}
			break;
			case CommentDoc:
			case Comment:
				c_state = state;
				start = pos;
				while (pos < len)
				{
					bool isCode = (tmp_state == isJs || tmp_state == isCss) ? true : false;
					if (text.mid(pos, 3) == "-->" || ((text.mid(pos, 2) == "*/" || text.mid(pos, 8) == "</script" || text.mid(pos, 7) == "</style") && isCode))
					{
						if (text.mid(pos, 2) == "*/" && isCode)
						{
							pos += 2;
						} else if (text.mid(pos, 8) == "</script" && isCode) {
							pos -= 8;
						} else if (text.mid(pos, 7) == "</style" && isCode) {
							pos -= 7;
						} else
							pos += 3;

						if (tmp_state == isJs)
							state = isJs;
						else if (tmp_state == isCss)
							state = isCss;
						else
							state = Normal;
						break;
					} else
						++pos;
				}
				setFormat(start, pos - start, m_formats[(c_state == CommentDoc ? CodeEditor::CommentDoc : CodeEditor::Comment)]);
			break;
			case Identifier:
				QChar quote = QChar::Null;
				while (pos < len)
				{
					QChar ch = text.at(pos);
					if (quote.isNull())
					{
						start = pos;
						if (ch == '\'' || ch == '"')
						{
							quote = ch;
						} else if (ch == '>') {
							++pos;
							setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
							if (isScript)
								state = isJs;
							else if (isStyle)
								state = isCss;
							else
								state = Normal;
							break;
						} else if (text.mid(pos, 2) == "/>") {
							pos += 2;
							setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
							state = Normal;
							break;
						} else if (ch != ' ' && text.at(pos) != '\t') {
							// Tag not ending, not a quote and no whitespace, so
							// we must be dealing with an attribute.
							++pos;
							while (pos < len && text.at(pos) != ' ' && text.at(pos) != '\t' && text.at(pos) != '=')
								++pos;
							setFormat(start, pos - start, m_formats[CodeEditor::BuiltInHtml]);
							start = pos;
						}
					} else if (ch == quote) {
						quote = QChar::Null;
						// Anything quoted is a value
						setFormat(start, pos - start + 1, m_formats[CodeEditor::StringHtml]);
					}
					++pos;
				}
			break;
		}
	}

	setCurrentBlockState(state);
}

class LineNumberArea : public QWidget
{
private:
	struct BlockInfo {
		int position;
		int number;
	};

public:
	LineNumberArea(QWidget *parent) : QWidget(parent)
	{
		backgroundColor = QColor("#2b3337");
		lineNumberColor = QColor("#81969a");
	}

	QVector<BlockInfo> lineNumbers;
	QFont font;
	QColor backgroundColor;
	QColor lineNumberColor;

protected:
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.fillRect(event->rect(), backgroundColor);
		painter.setPen(lineNumberColor);
		painter.setFont(font);

		int fontheight = QFontMetrics(font).height();
		const int listSize = lineNumbers.size();
		for (int i = 0; i < listSize; ++i)
			painter.drawText(0, lineNumbers.at(i).position, width() - 4, fontheight, Qt::AlignRight, QString::number(lineNumbers.at(i).number));
	}

};

CodeEditor::CodeEditor(QWidget *parent, SyntaxType type)
	: QPlainTextEdit(parent)
{
	lineNumberArea  = new LineNumberArea(this);
	highlighter     = new Highlighter(type, document());
	showLineNumbers = true;
	textWrap        = false;

	complete = new QCompleter(this);
	complete->setWidget(this);
	complete->setCompletionMode(QCompleter::PopupCompletion);
	complete->setCaseSensitivity(Qt::CaseInsensitive);
	complete->setWrapAround(false);
	connect(complete, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

	QFont textFont = font();
#if defined(Q_OS_MAC)
	textFont.setPointSize(12);
	textFont.setFamily("Monaco");
#elif defined(Q_OS_UNIX)
	textFont.setFamily("Monospace");
#else
	textFont.setPointSize(11);
	textFont.setFamily("Courier");
#endif
	setFont(textFont);
	lineNumberArea->font = textFont;

	setTabStopWidth(40);
	setTextWrapEnabled(textWrap);
	setSyntaxType(type);

	setFormatFor(Normal, QColor("#e0e2e4"));
	setColor(Background, QColor("#232a2d"));
	setColor(NumberArea, QColor("#2b3337"));
	setColor(LineNumber, QColor("#81969a"));
	setColor(LineSelect, QColor("#2f393c"));

	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberArea()));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
}

CodeEditor::~CodeEditor()
{
	delete highlighter;
	delete lineNumberArea;
	delete complete;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);
	updateLineNumberArea();
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);

	if (complete && complete->popup()->isVisible())
	{
		// The following keys are forwarded by the completer to the widget
		switch (event->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				event->ignore();
				return; // let the completer do default behavior
			default:
				break;
		}
	}

	bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space); // CTRL+Space
	if (!complete || !isShortcut) // do not process the shortcut when we have a completer
		QPlainTextEdit::keyPressEvent(event);

	const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!complete || (ctrlOrShift && event->text().isEmpty()))
		return;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = tc.selectedText();

	if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(event->text().right(1))))
	{
		complete->popup()->hide();
		return;
	}

	if (completionPrefix != complete->completionPrefix())
	{
		complete->setCompletionPrefix(completionPrefix);
		complete->popup()->setCurrentIndex(complete->completionModel()->index(0, 0));
	}

	QRect cr = cursorRect();
	cr.setWidth(complete->popup()->sizeHintForColumn(0) + complete->popup()->verticalScrollBar()->sizeHint().width());
	complete->complete(cr); // popup it up!
}

void CodeEditor::setSyntaxType(SyntaxType type)
{
	syntaxtype = type;
	highlighter->setSyntaxType(type);
	highlightCurrentLine();
}

void CodeEditor::setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg, bool bold, bool italic, bool underline)
{
	if (type == Normal)
	{
		QPalette pal = palette();
		pal.setColor(QPalette::Text, colorFg);
		setPalette(pal);
	}

	highlighter->setFormatFor(type, colorFg, colorBg, bold, italic, underline);
	highlighter->rehighlight();
	highlightCurrentLine();
}

void CodeEditor::setColor(HighlightType type, const QColor &color)
{
	if (type == Background)
	{
		QPalette pal = palette();
		pal.setColor(QPalette::Base, color);
		setPalette(pal);
	} else if (type == Normal) {
		QPalette pal = palette();
		pal.setColor(QPalette::Text, color);
		setPalette(pal);
		highlighter->setColor(Normal, color);
	} else if (type == NumberArea) {
		lineNumberArea->backgroundColor = color;
		updateLineNumberArea();
	} else if (type == LineNumber) {
		lineNumberArea->lineNumberColor = color;
		updateLineNumberArea();
	} else if (type == LineSelect) {
		lineSelectColor = color;
		highlightCurrentLine();
	} else {
		highlighter->setColor(type, color);
		highlightCurrentLine();
	}
}

void CodeEditor::setListWordsCompletion(const QString& fileName)
{
	if (!fileName.isEmpty())
	{
		QStringList words;
		QFile file(fileName);
		if (file.open(QFile::ReadOnly) && !fileName.isEmpty())
		{
			while (!file.atEnd())
			{
				QByteArray line = file.readLine();
				if (!line.isEmpty())
					words << line.trimmed();
			}
			QStringListModel *model_list_words = new QStringListModel(words, complete);
			complete->setModel(model_list_words);
		}
	}
}

void CodeEditor::mark(const QString &str, Qt::CaseSensitivity sens)
{
	highlighter->mark(str, sens);
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
	showLineNumbers = visible;
	updateLineNumberArea();
}

bool CodeEditor::isLineNumbersVisible() const
{
	return showLineNumbers;
}

void CodeEditor::setTextWrapEnabled(bool enable)
{
	textWrap = enable;
	setLineWrapMode(enable ? WidgetWidth : NoWrap);
}

bool CodeEditor::isTextWrapEnabled() const
{
	return textWrap;
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	Q_UNUSED(rect)
	if (dy != 0)
		updateLineNumberArea();
}

void CodeEditor::updateLineNumberArea()
{
	QRect cr = contentsRect();
	if (!showLineNumbers)
	{
		lineNumberArea->hide();
		setViewportMargins(0, 0, 0, 0);
		lineNumberArea->setGeometry(cr.left(), cr.top(), 0, cr.height());
		return;
	}

	lineNumberArea->font = this->font();
	lineNumberArea->show();

	int space = 0;
	if (showLineNumbers)
	{
		int digits = 3;
		int maxLines = qMax(1, blockCount());
		for (int number = 10; number <= maxLines; number *= 10)
			if (number > 999) ++digits;
		space += 8 + fontMetrics().width('9') * digits;
	}

	setViewportMargins(space, 0, 0, 0);
	lineNumberArea->setGeometry(cr.left(), cr.top(), space, cr.height());
	QRectF lineNumberAreaRect(cr.left(), cr.top(), space, cr.height());

	QTextBlock block = firstVisibleBlock();
	int index = 0;
	while (block.isValid())
	{
		if (block.isVisible())
		{
			QRectF rect = blockBoundingGeometry(block).translated(contentOffset());
			if (lineNumberAreaRect.intersects(rect))
			{
				if (lineNumberArea->lineNumbers.count() >= index)
					lineNumberArea->lineNumbers.resize(index + 1);
				lineNumberArea->lineNumbers[index].position = rect.top();
				lineNumberArea->lineNumbers[index].number   = block.blockNumber() + 1;
				++index;
			}
			if (rect.top() > lineNumberAreaRect.bottom())
				break;
		}
		block = block.next();
	}
	lineNumberArea->lineNumbers.resize(index);
	lineNumberArea->update();
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		selection.format.setBackground(lineSelectColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::insertCompletion(const QString& completion)
{
	QTextCursor tc = textCursor();
	int extra = completion.length() - complete->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}
