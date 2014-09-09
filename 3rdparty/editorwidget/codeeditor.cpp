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

#include "codeeditor.h"

#include <QSyntaxHighlighter>
#include <QPainter>

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
	void highlightBlockJavaScriptAndCss(const QString &text);
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
		jsRegex = 2,
		jsIdentifier = 3
	};

	CodeEditor::SyntaxType m_syntaxtype;
	QSet<QString> m_keywords;
	QSet<QString> m_keywordsCss;
	QSet<QString> m_builtin;
	QSet<QString> m_builtinCss;
	QSet<QString> m_classes;
	QSet<QString> m_other;
	bool isScript, isStyle;

	QHash<CodeEditor::HighlightType, QTextCharFormat> m_formats;
	QString m_markString;
	Qt::CaseSensitivity m_markCaseSensitivity;
};

Highlighter::Highlighter(CodeEditor::SyntaxType type, QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	m_markCaseSensitivity = Qt::CaseInsensitive;

	setFormatFor(CodeEditor::Normal     , QColor("#000000"));                               // Qt::black
	setFormatFor(CodeEditor::Number     , QColor("#ff0000"));                               // Qt::red
	setFormatFor(CodeEditor::NumberCss  , QColor("#ff8000"));                               // orange
	setFormatFor(CodeEditor::String     , QColor("#808080"));                               // Qt::darkGray
	setFormatFor(CodeEditor::StringHtml , QColor("#0000ff"));                               // Qt::blue
	setFormatFor(CodeEditor::Comment    , QColor("#008000"));                               // Qt::darkGreen
	setFormatFor(CodeEditor::CommentDoc , QColor("#008080"));                               // Qt::darkCyan
	setFormatFor(CodeEditor::Regex      , QColor("#8000ff"));                               // lila
	setFormatFor(CodeEditor::RegexCss   , QColor("#ff8000"));                               // orange
	setFormatFor(CodeEditor::RegexHtml  , QColor("#ff0000"));                               // Qt::red
	setFormatFor(CodeEditor::Operator   , QColor("#000000"), Qt::transparent, true);        // Qt::black
	setFormatFor(CodeEditor::Keyword    , QColor("#000080"), Qt::transparent, true, true);  // Qt::darkBlue
	setFormatFor(CodeEditor::KeywordCss , QColor("#0000ff"));                               // Qt::blue
	setFormatFor(CodeEditor::KeywordHtml, QColor("#800080"), Qt::transparent, true);        // Qt::darkMagenta
	setFormatFor(CodeEditor::BuiltIn    , QColor("#008080"), Qt::transparent, false, true); // Qt::darkCyan
	setFormatFor(CodeEditor::BuiltInCss , QColor("#008080"), Qt::transparent, false, true); // Qt::darkCyan
	setFormatFor(CodeEditor::BuiltInHtml, QColor("#000000"), Qt::transparent, true);        // Qt::black
	setFormatFor(CodeEditor::Classes    , QColor("#800080"));                               // Qt::darkMagenta
	setFormatFor(CodeEditor::ClassesCss , QColor("#800080"), Qt::transparent, true);        // Qt::darkMagenta
	setFormatFor(CodeEditor::Other      , QColor("#ff8000"));                               // orange
	setFormatFor(CodeEditor::Marker     , QColor("#000000"), QColor("#ffff00"));            // Qt::black, Qt::yellow
	setFormatFor(CodeEditor::Identifier , QColor("#800000"));                               // Qt::darkRed

	// https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
	m_keywords.clear();
	m_keywords << "abstract" << "boolean" << "break" << "byte" << "case" << "catch"
		<< "char" << "class" << "const" << "continue" << "debugger" << "default"
		<< "delete" << "do" << "double" << "else" << "enum" << "export" << "extends"
		<< "false" << "final" << "finally" << "float" << "for" << "function" << "goto"
		<< "if" << "implements" << "import" << "in" << "instanceof" << "int" << "interface"
		<< "long" << "native" << "new" << "null" << "package" << "private" << "protected"
		<< "public" << "return" << "short" << "static" << "super" << "switch"<< "synchronized"
		<< "this" << "throw" << "throws" << "transient" << "true" << "try"<< "typeof" << "var"
		<< "void" << "while" << "with";

	m_keywordsCss.clear();
	m_keywordsCss
		<< "absolute"  << "add"  << "adjoins"  << "align"  << "alternate"  << "alternating"
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
		<< "weight" << "width" << "window" << "x" << "y";

	// built-in and other popular objects + properties
	m_builtin.clear();
	m_builtin
		<< "abs" << "acos" << "apply" << "arguments" << "arity" << "Array" << "asin" << "atan"
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

	m_builtinCss.clear();
	m_builtinCss
		<< "a" << "b" << "black" << "body" << "bold" << "br" << "button" << "checked" << "darkgray"
		<< "dashed" << "dd" << "del" << "div" << "dl" << "dotted" << "double" << "dt" << "editable"
		<< "em" << "fieldset" << "first" << "form" << "frame" << "frameset" << "gray" << "groove"
		<< "h1" << "h2" << "h3" << "h4" << "h5" << "h6" << "head" << "hr" << "hsv" << "hsva" << "html"
		<< "i" << "iframe" << "img" << "input" << "inset" << "italic" << "label" << "last" << "legend"
		<< "li" << "link" << "middle" << "no-repeat" << "ol" << "optgroup" << "option" << "outset"
		<< "p" << "pre" << "pt" << "px" << "qlineargradient" << "repeat" << "repeat-x" << "repeat-y"
		<< "rgb" << "rgba" << "ridge" << "s" << "select" << "solid" << "span" << "strong" << "sub"
		<< "summary" << "sup" << "table" << "tbody" << "td" << "textarea" << "tfoot" << "th" << "thead"
		<< "tr" << "transparent" << "tt" << "u" << "ul"<< "unchecked" << "url" << "white";

	m_classes.clear();
	m_classes
		<< "QAbstractScrollArea" << "QCheckBox" << "QColumnView" << "QComboBox" << "QDateEdit"
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

	m_other.clear();
	m_other
		<< "closed" << "focus" << "hover" << "open" << "pad" << "pane" << "pressed" << "selected";

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
	rehighlight();
}

void Highlighter::setColor(CodeEditor::HighlightType type, const QColor &color)
{
	m_formats[type].setForeground(color);
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
		highlightBlockJavaScriptAndCss(text);

	if (!m_markString.isEmpty()) {
		int m_pos = 0;
		int m_len = m_markString.length();
		for (;;) {
			m_pos = text.indexOf(m_markString, m_pos, m_markCaseSensitivity);
			if (m_pos < 0)
				break;
			setFormat(m_pos, m_len, m_formats[CodeEditor::Marker]);
			++m_pos;
		}
	}
}

void Highlighter::highlightBlockJavaScriptAndCss(const QString &text)
{
	int state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;
	while (pos <= len) {
		QChar ch = (pos < len) ? text.at(pos) : QChar();
		QChar next = (pos < len - 1) ? text.at(pos + 1) : QChar();
		switch (state) {
		case Normal:
		default:
			start = pos;
			setFormat(start, len, m_formats[CodeEditor::Normal]);
			if (ch.isSpace()) {
				++pos;
			} else if (ch.isDigit()) {
				++pos;
				state = Number;
			} else if (ch == '#') {
				QRegExp expression("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})");
				int index = expression.indexIn(text, pos);
				if (index >= 0) {
					int length = expression.matchedLength();
					setFormat(start, length, m_formats[CodeEditor::Identifier]);
					pos += length;
					state = Normal;
				} else {
					start = pos;
					while (pos < len) {
						if (QString("<>(){}[] ,;").contains(text.at(pos)) || text.at(pos) == '\n') {
							state = Normal;
							break;
						}
						++pos;
					}
					setFormat(start, pos - start, m_formats[CodeEditor::Operator]);
				}
			} else if (ch.isLetter() || ch == '_') {
				++pos;
				state = Identifier;
			} else if (ch == '\'' || ch == '\"') {
				++pos;
				state = String;
			} else if (ch == '/' && next == '/') {
				pos = len;
				setFormat(start, len, m_formats[CodeEditor::Comment]);
			} else if (text.mid(pos, 3) == "/**") {
				state = CommentDoc;
			} else if (ch == '/' && next == '*') {
				++pos;
				++pos;
				state = Comment;
			} else if (ch == '/' && next != '*') {
				++pos;
				state = Regex;
			} else {
			//	if (!QString("(){}[]").contains(ch))
				setFormat(start, 1, m_formats[CodeEditor::Operator]);
				if (ch == '-' && m_syntaxtype == CodeEditor::Css)
					setFormat(start, 1, m_formats[CodeEditor::KeywordCss]);
				++pos;
				state = Normal;
			}
			break;
		case Number:
			if (ch.isSpace() || !ch.isDigit()) {
				setFormat(start, pos - start, m_formats[(m_syntaxtype == CodeEditor::Css ? CodeEditor::NumberCss : CodeEditor::Number)]);
				state = Normal;
			} else
				++pos;
			break;
		case Identifier:
			if (ch.isSpace() || !(ch.isLetter() || ch == '_')) {
				QString token = text.mid(start, pos - start).trimmed();
				if (m_keywords.contains(token))
					setFormat(start, pos - start, m_formats[CodeEditor::Keyword]);
				else if (m_keywordsCss.contains(token) && m_syntaxtype == CodeEditor::Css)
					setFormat(start, pos - start, m_formats[CodeEditor::KeywordCss]);
				else if (m_builtin.contains(token))
					setFormat(start, pos - start, m_formats[CodeEditor::BuiltIn]);
				else if (m_builtinCss.contains(token) && m_syntaxtype == CodeEditor::Css)
					setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
				else if (m_classes.contains(token))
					setFormat(start, pos - start, m_formats[(m_syntaxtype == CodeEditor::Css ? CodeEditor::ClassesCss : CodeEditor::Classes)]);
				else if (m_other.contains(token))
					setFormat(start, pos - start, m_formats[CodeEditor::Other]);
				state = Normal;
			} else
				++pos;
			break;
		case String:
			if (ch == text.at(start)) {
				QChar prev = (pos > 0) ? text.at(pos - 1) : QChar();
				++pos;
				if (prev != '\\') {
					setFormat(start, pos - start, m_formats[CodeEditor::String]);
					state = Normal;
				}
			} else
				++pos;
			break;
		case Comment:
		case CommentDoc:
			++pos;
			if (ch == '*' && next == '/') {
				++pos;
				setFormat(start, pos - start, m_formats[(state == CommentDoc ? CodeEditor::CommentDoc : CodeEditor::Comment)]);
				state = Normal;
			}
			break;
		case Regex:
			if (m_syntaxtype == CodeEditor::Css) {
				++pos;
				setFormat(start, pos - start, m_formats[CodeEditor::RegexCss]);
				if (next == ')')
					state = Normal;
			} else {
				if (ch == '/') {
					QChar prev = (pos > 0) ? text.at(pos - 1) : QChar();
					++pos;
					if (prev != '\\') {
						setFormat(start, pos - start, m_formats[CodeEditor::Regex]);
						state = Normal;
					}
				} else
					++pos;
			}
			break;
		}
	}

	if (state == Comment)
		setFormat(start, len, m_formats[CodeEditor::Comment]);
	else if (state == CommentDoc)
		setFormat(start, len, m_formats[CodeEditor::CommentDoc]);
	else
		state = Normal;

	setCurrentBlockState(state);
}

void Highlighter::highlightBlockHtml(const QString &text)
{
	int js_state = previousBlockState();
	int state = previousBlockState();
	int len = text.length();
	int start = 0;
	int pos = 0;
	while (pos < len) {
		switch (state) {
		case Normal:
		case isJs:
		case isCss:
		default:
			while (pos < len) {
				QChar ch = text.at(pos);
				QChar next = text.at(pos+1);
				if ((text.mid(pos, 8) == "</script" || text.mid(pos, 7) == "</style") && js_state != jsString)
					state = Normal;

				if ((ch == '<' && state == Normal) || (text.mid(pos, 2) == "/*" && (state == isJs || state == isCss) && js_state != jsString)) {
					isScript = false;
					isStyle  = false;
					if (text.mid(pos, 7) == "<script") {
						state = isJs;
						isScript = true;
					}
					if (text.mid(pos, 6) == "<style") {
						state = isCss;
						isStyle = true;
					}

					if (text.mid(pos, 3) == "/**" && (state == isJs || state == isCss)) {
						state = CommentDoc;
					} else if (text.mid(pos, 2) == "/*" && (state == isJs || state == isCss)) {
						state = Comment;
					} else if (text.mid(pos, 4) == "<!--" && state == Normal) {
						state = Comment;
					} else {
						state = Identifier;
						start = pos;
						while (pos < len && text.at(pos) != ' ' && text.at(pos) != '>'
								&& text.at(pos) != '\t' && text.mid(pos, 2) != "/>")
							++pos;
						if (text.mid(pos, 2) == "/>")
							++pos;
						setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
						break;
					}
					break;
				} else if (ch == '&' && state == Normal) {
					state = Regex;
					start = pos;
					while (pos < len && text.at(pos++) != ';');
					setFormat(start, pos - start, m_formats[CodeEditor::RegexHtml]);
					state = Normal;
				} else if (ch == '.' && state == isCss && js_state != jsNormal && js_state != jsRegex) {
					start = pos;
					while (pos < len) {
						if (QString("<>(){}[] ,;").contains(text.at(pos)) || text.at(pos) == '\n') {
							state = isCss;
							break;
						} else
							++pos;
					}
					setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
				} else if (ch == '#' && state == isCss && js_state != jsNormal && js_state != jsRegex) {
					start = pos;
					while (pos < len) {
						if (QString("<>(){}[] ,;").contains(text.at(pos)) || text.at(pos) == '\n') {
							state = isCss;
							break;
						} else
							++pos;
					}
					setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
				} else {
				//-- ini js and css
					if(state == isJs || state == isCss ) {
						next = text.at(pos+1);
						switch (js_state) {
						case jsNormal:
						default:
							start = pos;
							setFormat(start, len, m_formats[CodeEditor::Normal]);
							if (ch.isSpace()) {
								++pos;
							} else if (ch.isDigit()) {
								++pos;
								js_state = jsNumber;
							} else if (ch == '#' && state == isCss) {
								QRegExp expression("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})");
								int index = expression.indexIn(text, pos);
								if (index >= 0) {
									pos = index;
									int length = expression.matchedLength();
									setFormat(pos, length, m_formats[CodeEditor::Identifier]);
									pos += length;
								} else
									++pos;
								js_state = jsNormal;
							} else if (ch.isLetter() || ch == '_') {
								++pos;
								js_state = jsIdentifier;
							} else if (ch == '\'' || ch == '\"') {
								++pos;
								js_state = jsString;
							} else if (ch == '/' && next == '/') {
								pos = len;
								setFormat(start, len, m_formats[CodeEditor::Comment]);
							} else if (ch == '/' && next != '*') {
								++pos;
								js_state = jsRegex;
							} else {
							//	if (!QString("(){}[]").contains(ch))
								setFormat(start, 1, m_formats[CodeEditor::Operator]);
								if (ch == '-' && state == isCss)
									setFormat(start, 1, m_formats[CodeEditor::KeywordCss]);
								++pos;
								js_state = jsNormal;
							}
							break;
						case jsNumber:
							if (ch.isSpace() || !ch.isDigit()) {
								setFormat(start, pos - start, m_formats[(state == isCss ? CodeEditor::NumberCss : CodeEditor::Number)]);
								js_state = jsNormal;
							} else
								++pos;
							break;
						case jsIdentifier:
							if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
								QString token = text.mid(start, pos - start).trimmed();
								if (m_keywords.contains(token))
									setFormat(start, pos - start, m_formats[CodeEditor::Keyword]);
								else if (m_keywordsCss.contains(token) && state == isCss)
									setFormat(start, pos - start, m_formats[CodeEditor::KeywordCss]);
								else if (m_builtin.contains(token))
									setFormat(start, pos - start, m_formats[CodeEditor::BuiltIn]);
								else if (m_builtinCss.contains(token) && state == isCss)
									setFormat(start, pos - start, m_formats[CodeEditor::BuiltInCss]);
								else if (m_classes.contains(token))
									setFormat(start, pos - start, m_formats[(state == isCss ? CodeEditor::ClassesCss : CodeEditor::Classes)]);
								else if (m_other.contains(token))
									setFormat(start, pos - start, m_formats[CodeEditor::Other]);
								js_state = jsNormal;
							} else
								++pos;
							break;
						case jsString:
							if (ch == text.at(start)) {
								QChar prev = (pos > 0) ? text.at(pos - 1) : QChar();
								++pos;
								if (prev != '\\') {
									setFormat(start, pos - start, m_formats[CodeEditor::String]);
									js_state = jsNormal;
								}
							} else
								++pos;
							break;
						case jsRegex:
							if (state == isCss) {
								++pos;
								setFormat(start, pos - start, m_formats[CodeEditor::RegexCss]);
								if (next == ')')
									js_state = jsNormal;
							} else {
								if (ch == '/') {
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
					} else {
						// No tag, comment or entity started, continue...
						++pos;
					}
				//-- fin js and css
				}
			}
			break;
		case CommentDoc:
		case Comment:
			start = pos;
			while (pos < len) {
				if (text.mid(pos, 2) == "*/" || text.mid(pos, 3) == "-->") {
					if (text.mid(pos, 2) == "*/")
						pos += 2;
					else
						pos += 3;
					setFormat(start, pos - start, m_formats[(state == CommentDoc ? CodeEditor::CommentDoc : CodeEditor::Comment)]);
					state = Normal;
					break;
				} else
					++pos;
				setFormat(start, pos - start, m_formats[(state == CommentDoc ? CodeEditor::CommentDoc : CodeEditor::Comment)]);
			}
			break;
		case Identifier:
			QChar quote = QChar::Null;
			while (pos < len) {
				QChar ch = text.at(pos);
				if (quote.isNull()) {
					start = pos;
					if (ch == '\'' || ch == '"') {
						quote = ch;
					} else if (ch == '>') {
						++pos;
						setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
						if(isScript)
							state = isJs;
						else if(isStyle)
							state = isCss;
						else
							state = Normal;
						break;
					} else if (text.mid(pos, 2) == "/>") {
						pos += 2;
						setFormat(start, pos - start, m_formats[CodeEditor::KeywordHtml]);
						if(isScript)
							state = isJs;
						else if(isStyle)
							state = isCss;
						else
							state = Normal;
						break;
					} else if (ch != ' ' && text.at(pos) != '\t') {
						// Tag not ending, not a quote and no whitespace, so
						// we must be dealing with an attribute.
						++pos;
						while (pos < len && text.at(pos) != ' '
								&& text.at(pos) != '\t' && text.at(pos) != '=')
							++pos;
						setFormat(start, pos - start, m_formats[CodeEditor::BuiltInHtml]);
						start = pos;
					}
				} else if (ch == quote) {
					quote = QChar::Null;
					// Anything quoted is a value
					setFormat(start, pos - start+1, m_formats[CodeEditor::StringHtml]);
				}
				++pos;
			}
			break;
		}
	}

	setCurrentBlockState(js_state);
	setCurrentBlockState(state);
}

const int sepLine = 8;
class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *editor) : QWidget(editor) {
		m_editor = editor;
	}

	QSize sizeHint() const {
		return QSize(m_editor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		m_editor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor *m_editor;
};

CodeEditor::CodeEditor(QWidget *parent, SyntaxType type)
	: QPlainTextEdit(parent)
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(11);
	setFont(font);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	lineNumberArea  = new LineNumberArea(this);
	highlighter     = new Highlighter(type, document());
	showLineNumbers = true;
	textWrap        = false;

	setTabStopWidth(40);
	setTextWrapEnabled(false);
	setSyntaxType(type);
}

CodeEditor::~CodeEditor()
{
	delete highlighter;
	delete lineNumberArea;
}

void CodeEditor::setSyntaxType(SyntaxType type)
{
	syntaxtype = type;
	if (syntaxtype == Html || syntaxtype == Css)
		setColor(Background, QColor("#ffffff"));
	else
		setColor(Background, QColor("#f2f4ff"));
	setColor(NumberArea, QColor("#e4e4e4"));
	setColor(LineNumber, QColor("#808080"));
	setColor(LineSelect, QColor("#e0f0ff"));

	highlighter->setSyntaxType(type);
	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

void CodeEditor::setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg, bool bold, bool italic, bool underline)
{
	if (type == Normal) {
		QPalette pal = palette();
		pal.setColor(QPalette::Text, colorFg);
		setPalette(pal);
	}
	highlighter->setFormatFor(type, colorFg, colorBg, bold, italic, underline);
	highlighter->rehighlight();
	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

void CodeEditor::setColor(HighlightType type, const QColor &color)
{
	if (type == Background) {
		QPalette pal = palette();
		pal.setColor(QPalette::Base, color);
		setPalette(pal);
	} else if (type == Normal) {
		QPalette pal = palette();
		pal.setColor(QPalette::Text, color);
		setPalette(pal);
		highlighter->setColor(Normal, color);
	} else if (type == NumberArea)
		numberAreaColor = color;
	else if (type == LineNumber)
		lineNumberColor = color;
	else if (type == LineSelect)
		lineSelectColor = color;
	else
		highlighter->setColor(type, color);
}

void CodeEditor::mark(const QString &str, Qt::CaseSensitivity sens)
{
	highlighter->mark(str, sens);
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
	showLineNumbers = visible;
	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
	lineNumberArea->setVisible(showLineNumbers);
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

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), numberAreaColor);
	painter.setPen(lineNumberColor);
	painter.setFont(font);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();
	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.drawText(0, top, lineNumberArea->width()-4, fontMetrics().height(),
							 Qt::AlignRight, number);
		}
		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

int CodeEditor::lineNumberAreaWidth()
{
	int space = 0;
	if (showLineNumbers)
	{
		int digits = 1;
		int max = qMax(1, blockCount());
		while (max >= 10) {
			max /= 10;
			++digits;
		}
		space = sepLine + fontMetrics().width(QLatin1Char('9')) * digits;
	}
	return space;
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::highlightCurrentLine()
{
	if (isReadOnly()) {
		setExtraSelections(QList<QTextEdit::ExtraSelection>());
	} else {
		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(lineSelectColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		QList<QTextEdit::ExtraSelection> extraSelections;
		extraSelections.append(selection);
		setExtraSelections(extraSelections);
	}
}
