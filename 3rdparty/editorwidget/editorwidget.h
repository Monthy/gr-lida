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

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include "codeeditor.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QFontComboBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QToolButton;
QT_END_NAMESPACE

class EditorRichText : public QTextEdit
{
	Q_OBJECT
public:
	explicit EditorRichText(QWidget *parent = 0);
	void setDefaultFont(QFont font);
	bool simplifyRichText() const {return m_simplifyRichText;}

public slots:
	void setText(const QString &text);
	void setSimplifyRichText(bool v);
	QString text(Qt::TextFormat format) const;

signals:
	void stateChanged();
	void simplifyRichTextChanged(bool);

private:
	bool m_simplifyRichText;
};

class EditorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit EditorWidget(QString theme = ":/", QWidget *parent = 0);
	~EditorWidget();

	void showSource(bool checked);
	void showFindReplace(bool checked);
	void showSmiles(bool checked);
	void setSmileList(QString filename, QString sep, QString path_smile);
	void setDefaultFont(const QFont &font);
	void setSourceFont(const QFont &font);
	void setTabStopWidth(int width = 80);
	void setText(const QString &text);
	void setSyntaxType(CodeEditor::SyntaxType type);
	void setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg = Qt::transparent, bool bold = false, bool italic = false, bool underline = false);
	void setColor(CodeEditor::HighlightType type, const QColor &color);

	bool isShowSource();
	bool isShowFindReplace();
	bool isShowSmiles();
	QString text(Qt::TextFormat format = Qt::AutoText) const;
	QString toPlainText() const;
	bool isLineNumbersVisible() const;
	bool isTextWrapEnabled() const;

public slots:
	void mark(const QString &str, Qt::CaseSensitivity sens = Qt::CaseInsensitive);
	void setLineNumbersVisible(bool checked);
	void setTextWrapEnabled(bool enable);

protected:
	void focusInEvent(QFocusEvent *event);

private:
	enum PagIndex {RichTextIndex, PlainTextIndex};
	enum State {Clean, RichTextChanged, PlainTextChanged};

	EditorRichText *editor_rich_text;
	CodeEditor     *editor_plain_text;
	QStackedWidget *pag_stacked;
	QListWidget    *list_smile;
	QWidget        *toolbar_find_replace;
	State m_state;
	int m_initialPag;
	QColor m_color;
	QString m_theme, m_align_checked;
	QLineEdit   *txt_find;
	QLineEdit   *txt_replace;
	QToolButton *btnReplace;
	QToolButton *btnReplaceAndNext;
	QToolButton *btnReplaceAll;
	QCheckBox   *chkCaseSensitive;
	QCheckBox   *chkWholeWords;
	QFontComboBox *m_font_input;
	QComboBox *m_font_size_input;
//	QAction *m_pdf_action;
	QAction *m_cut_action;
	QAction *m_copy_action;
	QAction *m_paste_action;
	QAction *m_undo_action;
	QAction *m_redo_action;
	QAction *m_bold_action;
	QAction *m_italic_action;
	QAction *m_underline_action;
	QAction *m_valign_sup_action;
	QAction *m_valign_sub_action;
	QAction *m_align_left_action;
	QAction *m_align_center_action;
	QAction *m_align_right_action;
	QAction *m_align_justify_action;
	QAction *m_link_action;
	QAction *m_image_action;
	QAction *m_color_action;
	QAction *m_simplify_richtext_action;
	QAction *m_find_replace_text_action;
	QAction *m_rich_plain_action;
	QAction *m_smiles_action;

	QString smilesToString(QString str) const;
	QString smilesToImage(QString str) const;
	bool findFirst(QString str_find, bool forward);
	void findReplace(QString str_find, QString str_replace, bool r_next = false, bool r_all = false);
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
	void fontChanged(const QFont &f);
	void colorChanged(const QColor &c);
	void alignmentChanged(Qt::Alignment a);

private slots:
	void pagIndexChanged(int newIndex);
	void richTextChanged();
	void plainTextChanged();
	void list_smile_itemDoubleClicked(QListWidgetItem *item);

	void on_show_source(bool checked);
	void on_show_find_replace(bool checked);
//	void on_edit_export_pdf();
	void on_edit_font(const QString &font);
	void on_edit_font_size(const QString &size);
	void on_edit_cut();
	void on_edit_copy();
	void on_edit_paste();
	void on_edit_undo();
	void on_edit_redo();
	void on_edit_bold();
	void on_edit_italic();
	void on_edit_underline();
	void on_edit_text_align(QAction *action);
	void on_edit_valign_sup();
	void on_edit_valign_sub();
	void on_edit_image();
	void on_edit_link(bool checked);
	void on_edit_color();

	void txtFindTextChanged(QString text);
	void chkCaseSensitive_toggled(bool checked);
	void chkReplace_toggled(bool checked);
	void btnFindBack_clicked();
	void btnFindNext_clicked();
	void btnReplace_clicked();
	void btnReplaceAndNext_clicked();
	void btnReplaceAll_clicked();
	void currentCharFormatChanged(const QTextCharFormat &format);
	void cursorPositionChanged();
	void clipboardDataChanged();

};

#endif // EDITORWIDGET_H
