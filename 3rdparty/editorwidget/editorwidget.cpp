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

#include "editorwidget.h"
#include <QWidget>
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QStackedWidget>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextEdit>
#include <QTextList>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMimeData>

static QAction *createAction(const QIcon &icon, const QString &text, const bool checked, QObject *parent = 0)
{
	QAction *result = new QAction(parent);
	result->setIcon(icon);
	result->setText(text);
	result->setCheckable(checked);
	result->setChecked(false);
	return result;
}

static QToolButton *createToolButton(const QIcon &icon, const QString &text, QWidget *parent = 0)
{
	QToolButton *result = new QToolButton(parent);
	result->setIcon(icon);
	result->setText(text);
	result->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	result->setAutoRaise(true);
	return result;
}

const bool simplifyRichTextDefault = true;

// Richtext simplification filter helpers: Elements to be discarded
static inline bool filterElement(const QStringRef &name)
{
	return name != "meta" && name != "style";
}

// Richtext simplification filter helpers: Filter attributes of elements
static inline void filterAttributes(const QStringRef &name, QXmlStreamAttributes *atts,
	bool *paragraphAlignmentFound)
{
	typedef QXmlStreamAttributes::iterator AttributeIt;
	if (atts->isEmpty())
		return;
	 // No style attributes for <body>
	if (name == "body") {
		atts->clear();
		return;
	}
	// Clean out everything except 'align' for 'p'
	if (name == "p") {
		for (AttributeIt it = atts->begin(); it != atts->end(); ) {
			if (it->name() == "align") {
				++it;
				*paragraphAlignmentFound = true;
			} else
				it = atts->erase(it);
		}
		return;
	}
}

// Richtext simplification filter helpers: Check for blank QStringRef.
static inline bool isWhiteSpace(const QStringRef &in)
{
	const int count = in.size();
	for (int i = 0; i < count; i++)
		if (!in.at(i).isSpace())
			return false;
	return true;
}

// Richtext simplification filter: Remove hard-coded font settings,
// <style> elements, <p> attributes other than 'align' and
// and unnecessary meta-information.
QString simplifyRichTextFilter(const QString &in, bool *isPlainTextPtr = 0)
{
	unsigned elementCount = 0;
	bool paragraphAlignmentFound = false;
	QString out;
	QXmlStreamReader reader(in);
	QXmlStreamWriter writer(&out);
	writer.setAutoFormatting(false);
	writer.setAutoFormattingIndent(0);

	while (!reader.atEnd()) {
		switch (reader.readNext()) {
		case QXmlStreamReader::StartElement:
			elementCount++;
			if (filterElement(reader.name())) {
				const QStringRef name = reader.name();
				QXmlStreamAttributes attributes = reader.attributes();
				filterAttributes(name, &attributes, &paragraphAlignmentFound);
				writer.writeStartElement(name.toString());
				if (!attributes.isEmpty())
					writer.writeAttributes(attributes);
			} else
				reader.readElementText(); // Skip away all nested elements and characters.
			break;
		case QXmlStreamReader::Characters:
			if (!isWhiteSpace(reader.text()))
				writer.writeCharacters(reader.text().toString());
			break;
		case QXmlStreamReader::EndElement:
			writer.writeEndElement();
			break;
		default:
			break;
		}
	}
	// Check for plain text (no spans, just <html><head><body><p>)
	if (isPlainTextPtr)
		*isPlainTextPtr = !paragraphAlignmentFound && elementCount == 4u; //
	return out;
}

EditorRichText::EditorRichText(QWidget *parent) :
	QTextEdit(parent), m_simplifyRichText(simplifyRichTextDefault)
{
	connect(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SIGNAL(stateChanged()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SIGNAL(stateChanged()));
}

void EditorRichText::setText(const QString &text)
{
	if (Qt::mightBeRichText(text))
		setHtml(text);
	else
		setPlainText(text);
}

void EditorRichText::setSimplifyRichText(bool v)
{
	if (v != m_simplifyRichText) {
		m_simplifyRichText = v;
		emit simplifyRichTextChanged(v);
	}
}

void EditorRichText::setDefaultFont(QFont font)
{
	// Some default fonts on Windows have a default size of 7.8,
	// which results in complicated rich text generated by toHtml().
	// Use an integer value.
	const int pointSize = qRound(font.pointSizeF());
	if (pointSize > 0 && !qFuzzyCompare(qreal(pointSize), font.pointSizeF()))
		font.setPointSize(pointSize);

	document()->setDefaultFont(font);
	if (font.pointSize() > 0)
		setFontPointSize(font.pointSize());
	else
		setFontPointSize(QFontInfo(font).pointSize());
	emit textChanged();
}

QString EditorRichText::text(Qt::TextFormat format) const
{
	switch (format) {
#if QT_VERSION < 0x050000
		case Qt::LogText:
#endif
	case Qt::PlainText:
		return toPlainText();
	case Qt::RichText:
		return m_simplifyRichText ? simplifyRichTextFilter(toHtml()) : toHtml();
	case Qt::AutoText:
		break;
	}
	const QString html = toHtml();
	bool isPlainText;
	const QString simplifiedHtml = simplifyRichTextFilter(html, &isPlainText);
	if (isPlainText)
		return toPlainText();
	return m_simplifyRichText ? simplifiedHtml : html;
}

EditorWidget::EditorWidget(QString theme, QWidget *parent) :
	QWidget(parent),
	editor_rich_text(new EditorRichText(this)),
	editor_plain_text(new CodeEditor(this, CodeEditor::Html)),
	pag_stacked(new QStackedWidget(this)),
	m_state(Clean),
	m_initialPag(RichTextIndex)
{
	m_theme = theme;
	m_color = editor_rich_text->textColor();

	connect(editor_plain_text, SIGNAL(textChanged()), this, SLOT(plainTextChanged()));
	connect(editor_rich_text, SIGNAL(textChanged()), this, SLOT(richTextChanged()));
	connect(editor_rich_text, SIGNAL(simplifyRichTextChanged(bool)), this, SLOT(richTextChanged()));
	connect(editor_rich_text, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
	connect(editor_rich_text, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	list_smile = new QListWidget(this);
	list_smile->setMaximumSize(QSize(155, 16777215));
	list_smile->setMovement(QListView::Static);
	list_smile->setResizeMode(QListView::Adjust);
	list_smile->setViewMode(QListView::IconMode);
	connect(list_smile, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(list_smile_itemDoubleClicked(QListWidgetItem*)));

	toolbar_find_replace = new QWidget(this);
	toolbar_find_replace->setMinimumSize(QSize(0, 30));

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(4);

		QHBoxLayout *toolbar_layout = new QHBoxLayout();
		toolbar_layout->setContentsMargins(0, 0, 0, 0);
		toolbar_layout->setSpacing(10);

			QToolBar *toolbar_edit = new QToolBar(this);
			toolbar_edit->setMinimumSize(QSize(0, 30));
			toolbar_edit->setIconSize(QSize(20, 20));
			toolbar_edit->setStyleSheet("QToolBar{border:0px;}");
		// Save Pdf
		/*		m_pdf_action = createAction(QIcon(m_theme +"img16/pdf.png"), tr("Exportar a PDF") +"...", false, toolbar_edit);
				m_pdf_action->setPriority(QAction::LowPriority);
				m_pdf_action->setShortcut(Qt::CTRL + Qt::Key_D);
				connect(m_pdf_action, SIGNAL(triggered()), this, SLOT(on_edit_export_pdf()));
			toolbar_edit->addAction(m_pdf_action);
			toolbar_edit->addSeparator();*/
		// combos font and size
				QWidget *toolbar_font_input = new QWidget(this);
				QHBoxLayout *combofont_layout = new QHBoxLayout(toolbar_font_input);
				combofont_layout->setContentsMargins(0, 0, 2, 0);
					m_font_input = new QFontComboBox(toolbar_edit);
					connect(m_font_input, SIGNAL(activated(QString)), this, SLOT(on_edit_font(QString)));
				combofont_layout->addWidget(m_font_input);
					m_font_size_input = new QComboBox(toolbar_edit);
					QFontDatabase font_db;
					foreach(int size, font_db.standardSizes())
						m_font_size_input->addItem(QString::number(size));
					connect(m_font_size_input, SIGNAL(activated(QString)), this, SLOT(on_edit_font_size(QString)));
				combofont_layout->addWidget(m_font_size_input);
			//	combofont_layout->setStretch(0, 1);

			toolbar_edit->addWidget(toolbar_font_input);
			toolbar_edit->addSeparator();
		// cut, copy, paste
				m_cut_action = createAction(QIcon(m_theme +"img16/edit_cut.png"), tr("Cortar"), false, toolbar_edit);
				m_cut_action->setPriority(QAction::LowPriority);
				m_cut_action->setShortcut(QKeySequence::Cut);
				connect(m_cut_action, SIGNAL(triggered()), this, SLOT(on_edit_cut()));
			toolbar_edit->addAction(m_cut_action);
				m_copy_action = createAction(QIcon(m_theme +"img16/edit_copy.png"), tr("Copiar"), false, toolbar_edit);
				m_copy_action->setPriority(QAction::LowPriority);
				m_copy_action->setShortcut(QKeySequence::Copy);
				connect(m_copy_action, SIGNAL(triggered()), this, SLOT(on_edit_copy()));
			toolbar_edit->addAction(m_copy_action);
				m_paste_action = createAction(QIcon(m_theme +"img16/edit_paste.png"), tr("Pegar"), false, toolbar_edit);
				m_paste_action->setPriority(QAction::LowPriority);
				m_paste_action->setShortcut(QKeySequence::Paste);
				connect(m_paste_action, SIGNAL(triggered()), this, SLOT(on_edit_paste()));
			toolbar_edit->addAction(m_paste_action);
			toolbar_edit->addSeparator();
		// undo, redo
				m_undo_action = createAction(QIcon(m_theme +"img16/edit_deshacer.png"), tr("Deshacer"), false, toolbar_edit);
				m_undo_action->setShortcut(QKeySequence::Undo);
				connect(m_undo_action, SIGNAL(triggered()), this, SLOT(on_edit_undo()));
			toolbar_edit->addAction(m_undo_action);
				m_redo_action = createAction(QIcon(m_theme +"img16/edit_rehacer.png"), tr("Rehacer"), false, toolbar_edit);
				m_redo_action->setPriority(QAction::LowPriority);
				m_redo_action->setShortcut(QKeySequence::Redo);
				connect(m_redo_action, SIGNAL(triggered()), this, SLOT(on_edit_redo()));
			toolbar_edit->addAction(m_redo_action);
			toolbar_edit->addSeparator();
		// bold, italic, underline, ,
				m_bold_action = createAction(QIcon(m_theme +"img16/edit_negrita.png"), tr("Negrita"), true, toolbar_edit);
				m_bold_action->setPriority(QAction::LowPriority);
				m_bold_action->setShortcut(Qt::CTRL + Qt::Key_B);
				connect(m_bold_action, SIGNAL(triggered()), this, SLOT(on_edit_bold()));
			toolbar_edit->addAction(m_bold_action);
				m_italic_action = createAction(QIcon(m_theme +"img16/edit_cursiva.png"), tr("Cursiva"), true, toolbar_edit);
				m_italic_action->setPriority(QAction::LowPriority);
				m_italic_action->setShortcut(Qt::CTRL + Qt::Key_I);
				connect(m_italic_action, SIGNAL(triggered()), this, SLOT(on_edit_italic()));
			toolbar_edit->addAction(m_italic_action);
				m_underline_action = createAction(QIcon(m_theme +"img16/edit_subrayada.png"), tr("Subrayado"), true, toolbar_edit);
				m_underline_action->setPriority(QAction::LowPriority);
				m_underline_action->setShortcut(Qt::CTRL + Qt::Key_U);
				connect(m_underline_action, SIGNAL(triggered()), this, SLOT(on_edit_underline()));
			toolbar_edit->addAction(m_underline_action);
			toolbar_edit->addSeparator();
		// align: left, center, right, justify
				QActionGroup *grp = new QActionGroup(toolbar_edit);
				connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(on_edit_text_align(QAction*)));
				if (QApplication::isLeftToRight()) {
					m_align_left_action   = createAction(QIcon(m_theme +"img16/edit_text_left.png"), tr("Izquierdo"), true, grp);
					m_align_center_action = createAction(QIcon(m_theme +"img16/edit_text_center.png"), tr("Centro"), true, grp);
					m_align_right_action  = createAction(QIcon(m_theme +"img16/edit_text_right.png"), tr("Derecho"), true, grp);
				} else {
					m_align_right_action  = createAction(QIcon(m_theme +"img16/edit_text_right.png"), tr("Derecho"), true, grp);
					m_align_center_action = createAction(QIcon(m_theme +"img16/edit_text_center.png"), tr("Centro"), true, grp);
					m_align_left_action   = createAction(QIcon(m_theme +"img16/edit_text_left.png"), tr("Izquierdo"), true, grp);
				}
				m_align_justify_action = createAction(QIcon(m_theme +"img16/edit_text_justify.png"), tr("Justificado"), true, grp);
				m_align_left_action->setPriority(QAction::LowPriority);
				m_align_left_action->setShortcut(Qt::CTRL + Qt::Key_L);
				m_align_center_action->setPriority(QAction::LowPriority);
				m_align_center_action->setShortcut(Qt::CTRL + Qt::Key_E);
				m_align_right_action->setPriority(QAction::LowPriority);
				m_align_right_action->setShortcut(Qt::CTRL + Qt::Key_R);
				m_align_justify_action->setPriority(QAction::LowPriority);
				m_align_justify_action->setShortcut(Qt::CTRL + Qt::Key_J);
			toolbar_edit->addActions(grp->actions());
			toolbar_edit->addSeparator();
		// superscript, subscript
				m_valign_sup_action = createAction(QIcon(m_theme +"img16/edit_text_super.png"), tr("Superíndice"), true, toolbar_edit);
				connect(m_valign_sup_action, SIGNAL(triggered(bool)), this, SLOT(on_edit_valign_sup()));
			toolbar_edit->addAction(m_valign_sup_action);
				m_valign_sub_action = createAction(QIcon(m_theme +"img16/edit_text_subs.png"), tr("Subíndice"), true, toolbar_edit);
				connect(m_valign_sub_action, SIGNAL(triggered(bool)), this, SLOT(on_edit_valign_sub()));
			toolbar_edit->addAction(m_valign_sub_action);
			toolbar_edit->addSeparator();
		// image, link, color, simplify
				m_image_action = createAction(QIcon(m_theme +"img16/edit_imagen.png"), tr("Imagen"), false, toolbar_edit);
				connect(m_image_action, SIGNAL(triggered()), this, SLOT(on_edit_image()));
			toolbar_edit->addAction(m_image_action);
				m_link_action = createAction(QIcon(m_theme +"img16/edit_enlace.png"), tr("Enlace"), true, toolbar_edit);
				connect(m_link_action, SIGNAL(triggered(bool)), this, SLOT(on_edit_link(bool)));
			toolbar_edit->addAction(m_link_action);
				QPixmap pix(16, 16);
				pix.fill(Qt::black);
				m_color_action = createAction(QIcon(pix), tr("Color") +"...", false, toolbar_edit);
				connect(m_color_action, SIGNAL(triggered()), this, SLOT(on_edit_color()));
			toolbar_edit->addAction(m_color_action);
			toolbar_edit->addSeparator();
				m_simplify_richtext_action = createAction(QIcon(m_theme +"img16/edit_simplify_richtext.png"), tr("Simplificar") +" Html", true, toolbar_edit);
				m_simplify_richtext_action->setChecked(editor_rich_text->simplifyRichText());
				connect(m_simplify_richtext_action, SIGNAL(triggered(bool)), editor_rich_text, SLOT(setSimplifyRichText(bool)));
				connect(editor_rich_text, SIGNAL(simplifyRichTextChanged(bool)), m_simplify_richtext_action, SLOT(setChecked(bool)));
			toolbar_edit->addAction(m_simplify_richtext_action);

		toolbar_layout->addWidget(toolbar_edit);

			QToolBar *toolbar_opts = new QToolBar(this);
			toolbar_opts->setIconSize(QSize(20, 20));
			toolbar_opts->setMinimumSize(QSize(30, 30));
			toolbar_opts->setStyleSheet("QToolBar{border:0px;}");
				m_find_replace_text_action = createAction(QIcon(m_theme +"img16/edit_buscar.png"), tr("Buscar") +"/"+ tr("Reemplazar"), true, toolbar_opts);
				m_find_replace_text_action->setPriority(QAction::LowPriority);
				m_find_replace_text_action->setShortcut(QKeySequence::Find);
				connect(m_find_replace_text_action, SIGNAL(triggered(bool)), this, SLOT(on_show_find_replace(bool)));
			toolbar_opts->addAction(m_find_replace_text_action);
				m_rich_plain_action = createAction(QIcon(m_theme +"img16/script.png"), tr("Editor") +"/"+ tr("Código"), true, toolbar_opts);
				connect(m_rich_plain_action, SIGNAL(triggered(bool)), this, SLOT(on_show_source(bool)));
			toolbar_opts->addAction(m_rich_plain_action);
				m_smiles_action = createAction(QIcon(m_theme +"img16/smile.png"), tr("Smiles"), true, toolbar_opts);
				connect(m_smiles_action, SIGNAL(triggered(bool)), list_smile, SLOT(setVisible(bool)));
			toolbar_opts->addAction(m_smiles_action);

		toolbar_layout->addWidget(toolbar_opts);
		toolbar_layout->setStretch(0, 1);

	main_layout->addLayout(toolbar_layout);

		QHBoxLayout *edit_smiles_layout = new QHBoxLayout();
		edit_smiles_layout->setContentsMargins(0, 0, 0, 0);
		edit_smiles_layout->setSpacing(4);

			QWidget *rich_edit = new QWidget();
				QVBoxLayout *rich_edit_layout = new QVBoxLayout(rich_edit);
				rich_edit_layout->setContentsMargins(0, 0, 0, 0);
				rich_edit_layout->addWidget(editor_rich_text);
			pag_stacked->addWidget(rich_edit);

			QWidget *plain_edit = new QWidget();
				QVBoxLayout *plain_edit_layout = new QVBoxLayout(plain_edit);
				plain_edit_layout->setContentsMargins(0, 0, 0, 0);
				plain_edit_layout->addWidget(editor_plain_text);
			pag_stacked->addWidget(plain_edit);
			connect(pag_stacked, SIGNAL(currentChanged(int)), this, SLOT(pagIndexChanged(int)));

		edit_smiles_layout->addWidget(pag_stacked);
		edit_smiles_layout->addWidget(list_smile);

	main_layout->addLayout(edit_smiles_layout);

		QGridLayout *gridLayout = new QGridLayout(toolbar_find_replace);
		gridLayout->setSpacing(4);
		gridLayout->setContentsMargins(0, 0, 0, 0);
			QLabel *lb_find = new QLabel(tr("Buscar")+":", toolbar_find_replace);
		gridLayout->addWidget(lb_find, 0, 0, 1, 1);
			txt_find = new QLineEdit(toolbar_find_replace);
			txt_find->setMinimumSize(QSize(0, 24));
			connect(txt_find, SIGNAL(textChanged(QString)), this, SLOT(txtFindTextChanged(QString)));
		gridLayout->addWidget(txt_find, 0, 1, 1, 1);
			QToolButton *btnFindBack = createToolButton(QIcon(m_theme +"img16/edit_buscar_anterior.png"), tr("Buscar anterior"), toolbar_find_replace);
			btnFindBack->setShortcut(QKeySequence::FindPrevious);
			connect(btnFindBack, SIGNAL(clicked()), this, SLOT(btnFindBack_clicked()));
		gridLayout->addWidget(btnFindBack, 0, 2, 1, 1);
			QToolButton *btnFindNext = createToolButton(QIcon(m_theme +"img16/edit_buscar_siguiente.png"), tr("Buscar siguiente"), toolbar_find_replace);
			btnFindBack->setShortcut(QKeySequence::FindNext);
			connect(btnFindNext, SIGNAL(clicked()), this, SLOT(btnFindNext_clicked()));
		gridLayout->addWidget(btnFindNext, 0, 3, 1, 1);
			chkCaseSensitive = new QCheckBox(tr("Coincidir mayúsculas/minúsculas"), toolbar_find_replace);
			chkCaseSensitive->setChecked(false);
			connect(chkCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(chkCaseSensitive_toggled(bool)));
		gridLayout->addWidget(chkCaseSensitive, 0, 5, 1, 1);
			QCheckBox *chkReplace = new QCheckBox(tr("Reemplazar por") +":", toolbar_find_replace);
			chkReplace->setChecked(false);
			connect(chkReplace, SIGNAL(toggled(bool)), this, SLOT(chkReplace_toggled(bool)));
		gridLayout->addWidget(chkReplace, 1, 0, 1, 1);
			txt_replace = new QLineEdit(toolbar_find_replace);
			txt_replace->setEnabled(false);
			txt_replace->setMinimumSize(QSize(0, 24));
		gridLayout->addWidget(txt_replace, 1, 1, 1, 1);
			btnReplace = createToolButton(QIcon(m_theme +"img16/edit_reemplazar.png"), tr("Reemplazar"), toolbar_find_replace);
			btnReplace->setEnabled(false);
			connect(btnReplace, SIGNAL(clicked()), this, SLOT(btnReplace_clicked()));
		gridLayout->addWidget(btnReplace, 1, 2, 1, 1);
			btnReplaceAndNext = createToolButton(QIcon(m_theme +"img16/edit_reemplazar.png"), tr("Reemplazar siguiente"), toolbar_find_replace);
			btnReplaceAndNext->setEnabled(false);
			connect(btnReplaceAndNext, SIGNAL(clicked()), this, SLOT(btnReplaceAndNext_clicked()));
		gridLayout->addWidget(btnReplaceAndNext, 1, 3, 1, 1);
			btnReplaceAll = createToolButton(QIcon(m_theme +"img16/edit_reemplazar.png"), tr("Reemplazar todo"), toolbar_find_replace);
			btnReplaceAll->setEnabled(false);
			connect(btnReplaceAll, SIGNAL(clicked()), this, SLOT(btnReplaceAll_clicked()));
		gridLayout->addWidget(btnReplaceAll, 1, 4, 1, 1);
			chkWholeWords = new QCheckBox(tr("Solo palabras completas"), toolbar_find_replace);
		gridLayout->addWidget(chkWholeWords, 1, 5, 1, 1);

	main_layout->addWidget(toolbar_find_replace);

#ifndef QT_NO_CLIPBOARD
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
#endif

	showSource(m_initialPag == RichTextIndex ? false : true);
	showFindReplace(false);
	showSmiles(false);
	setTabStopWidth(40);

	fontChanged(editor_rich_text->font());
	colorChanged(editor_rich_text->textColor());
	alignmentChanged(editor_rich_text->alignment());
}

EditorWidget::~EditorWidget()
{
//	delete m_pdf_action;
	delete m_cut_action;
	delete m_copy_action;
	delete m_paste_action;
	delete m_undo_action;
	delete m_redo_action;
	delete m_bold_action;
	delete m_italic_action;
	delete m_underline_action;
	delete m_valign_sup_action;
	delete m_valign_sub_action;
	delete m_align_left_action;
	delete m_align_center_action;
	delete m_align_right_action;
	delete m_align_justify_action;
	delete m_link_action;
	delete m_image_action;
	delete m_color_action;
	delete m_simplify_richtext_action;
	delete m_find_replace_text_action;
	delete m_rich_plain_action;
	delete m_smiles_action;
	delete m_font_input;
	delete m_font_size_input;
	delete txt_find;
	delete txt_replace;
	delete btnReplace;
	delete btnReplaceAndNext;
	delete btnReplaceAll;
	delete chkCaseSensitive;
	delete chkWholeWords;
	delete toolbar_find_replace;
	delete list_smile;
	delete editor_rich_text;
	delete editor_plain_text;
	delete pag_stacked;
}

void EditorWidget::focusInEvent(QFocusEvent *)
{
	if (m_initialPag == PlainTextIndex)
		editor_rich_text->setFocus(Qt::TabFocusReason);
	else
		editor_plain_text->setFocus(Qt::TabFocusReason);
}

void EditorWidget::showSource(bool checked)
{
	m_rich_plain_action->setChecked(checked);
	emit on_show_source(checked);
}

bool EditorWidget::isShowSource()
{
	return m_rich_plain_action->isChecked();
}

void EditorWidget::showFindReplace(bool checked)
{
	m_find_replace_text_action->setChecked(checked);
	emit on_show_find_replace(checked);
}

bool EditorWidget::isShowFindReplace()
{
	return m_find_replace_text_action->isChecked();
}

void EditorWidget::showSmiles(bool checked)
{
	m_smiles_action->setChecked(checked);
	list_smile->setVisible(checked);
}

bool EditorWidget::isShowSmiles()
{
	return m_smiles_action->isChecked();
}

void EditorWidget::setSmileList(QString filename, QString sep, QString path_smile)
{
	QFile file_in( filename );

	if (file_in.exists()) {
		if (file_in.open(QIODevice::ReadOnly) != 0) {
			QString line;
			QStringList list;
			QTextStream in(&file_in);
			in.setCodec("UTF-8");
			list_smile->clear();
		//	editor_rich_text->document()->clear();
			while ( !in.atEnd() ) {
				line = in.readLine();
				if (!line.isEmpty()) {
					list = line.split( sep );
					if (QFile::exists(path_smile + list.at(1))) {
						QListWidgetItem *item = new QListWidgetItem(list_smile);
						item->setData(Qt::UserRole, list.at(1));
						item->setText( list.at(0) );
						item->setIcon(QIcon(path_smile + list.at(1)));
						editor_rich_text->document()->addResource(QTextDocument::ImageResource, QUrl("smile_rs_"+ list.at(0) +"_"+ list.at(1)), QImage(path_smile + list.at(1)));
					}
				}
			}
		}
	}
}

void EditorWidget::setDefaultFont(const QFont &font)
{
	editor_rich_text->setDefaultFont(font);
	fontChanged(editor_rich_text->font());
}

void EditorWidget::setSourceFont(const QFont &font)
{
	editor_plain_text->setFont(font);
}

void EditorWidget::setTabStopWidth(int width)
{
	editor_rich_text->setTabStopWidth(width);
	editor_plain_text->setTabStopWidth(width);
}

void EditorWidget::setText(const QString &text)
{
// Generally simplify rich text unless verbose text is found.
	const bool isSimplifiedRichText = !text.startsWith("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
	editor_rich_text->setSimplifyRichText(isSimplifiedRichText);
	editor_rich_text->setText(smilesToImage(text));
	editor_plain_text->setPlainText(smilesToString(text));
	m_state = Clean;
}

QString EditorWidget::text(Qt::TextFormat format) const
{
	// In autotext mode, if the user has changed the source, use that
	if (format == Qt::AutoText && (m_state == Clean || m_state == PlainTextChanged))
		return editor_plain_text->toPlainText();
	// If the plain text HTML editor is selected, first copy its contents over
	// to the rich text editor so that it is converted to Qt-HTML or actual
	// plain text.
	if (m_initialPag == PlainTextIndex && m_state == PlainTextChanged)
		editor_rich_text->setHtml(editor_plain_text->toPlainText());
	return editor_rich_text->text(format);
}

QString EditorWidget::toPlainText() const
{
	if (m_initialPag == RichTextIndex)
		editor_plain_text->setPlainText(smilesToString(editor_rich_text->text(Qt::RichText)));
	return editor_plain_text->toPlainText();
}

void EditorWidget::setSyntaxType(CodeEditor::SyntaxType type)
{
	editor_plain_text->setSyntaxType(type);
}

void EditorWidget::setFormatFor(CodeEditor::HighlightType type, const QColor &colorFg, const QColor &colorBg, bool bold, bool italic, bool underline)
{
	editor_plain_text->setFormatFor(type, colorFg, colorBg, bold, italic, underline);
}

void EditorWidget::setColor(CodeEditor::HighlightType type, const QColor &color)
{
	editor_plain_text->setColor(type, color);
}

void EditorWidget::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
{
	editor_plain_text->mark(str, caseSensitivity);
}

void EditorWidget::setLineNumbersVisible(bool checked)
{
	editor_plain_text->setLineNumbersVisible(checked);
}

bool EditorWidget::isLineNumbersVisible() const
{
	return editor_plain_text->isLineNumbersVisible();
}

void EditorWidget::setTextWrapEnabled(bool enable)
{
	editor_plain_text->setTextWrapEnabled(enable);
}

bool EditorWidget::isTextWrapEnabled() const
{
	return editor_plain_text->isTextWrapEnabled();
}

QString EditorWidget::smilesToString(QString str) const
{
	str.replace("<p", "\n<p");
	str.replace("</body", "\n</body");
	int count_smiles = list_smile->count();
	if (count_smiles > -1) {
		for (int i = 0; i < count_smiles; ++i) {
			QString smile_text = list_smile->item(i)->text();
			QString smile_icon = list_smile->item(i)->data(Qt::UserRole).toString();
			if (str.indexOf("<img src=\":smile_rs_"+ smile_text +"_"+ smile_icon +"\"/>", 0) != -1)
				str.replace("<img src=\":smile_rs_"+ smile_text +"_"+ smile_icon +"\"/>", smile_text +" ");
			else
				str.replace("<img src=\":smile_rs_"+ smile_text +"_"+ smile_icon +"\" />", smile_text +" ");
		}
	}
	return str;
}

QString EditorWidget::smilesToImage(QString str) const
{
	int count_smiles = list_smile->count();
	if (count_smiles > -1) {
		for (int i = 0; i < count_smiles; ++i) {
			QString smile_text = list_smile->item(i)->text();
			QString smile_icon = list_smile->item(i)->data(Qt::UserRole).toString();
			QRegExp rx_smile(QRegExp::escape(smile_text) +"\\s");
			str.replace(rx_smile, "<img src=\":smile_rs_"+ smile_text +"_"+ smile_icon +"\"/> ");
		}
	}
	return str;
}

bool EditorWidget::findFirst(QString str_find, bool forward)
{
	QTextDocument::FindFlags flags;
	if (forward)
		flags |= QTextDocument::FindBackward;
	if (chkWholeWords->isChecked())
		flags |= QTextDocument::FindWholeWords;
	if (chkCaseSensitive->isChecked())
		flags |= QTextDocument::FindCaseSensitively;

	if (m_initialPag == RichTextIndex) {
		editor_rich_text->setFocus(Qt::ShortcutFocusReason);
		return editor_rich_text->find(str_find, flags);
	} else {
		editor_plain_text->setFocus(Qt::ShortcutFocusReason);
		return editor_plain_text->find(str_find, flags);
	}
}

void EditorWidget::findReplace(QString str_find, QString str_replace, bool r_next, bool r_all)
{
	bool isFind = false;
	QString str_select = "";
	if (r_next)
		isFind = findFirst(str_find, false);

	if (m_initialPag == RichTextIndex) {
		str_select = editor_rich_text->textCursor().selectedText();
		if (!str_select.isEmpty() && str_select == str_find)
			editor_rich_text->insertHtml( str_replace );
	} else {
		str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty() && str_select == str_find)
			editor_plain_text->insertPlainText( str_replace );
	}

	if (r_all && isFind)
		findReplace(str_find, str_replace, true, true);
}

void EditorWidget::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	if (m_initialPag == RichTextIndex) {
		QTextCursor cursor = editor_rich_text->textCursor();
		if (!cursor.hasSelection())
			cursor.select(QTextCursor::WordUnderCursor);
		cursor.mergeCharFormat(format);
		editor_rich_text->mergeCurrentCharFormat(format);
	}
}

void EditorWidget::fontChanged(const QFont &f)
{
	if (m_initialPag == RichTextIndex) {
		m_font_input->setCurrentIndex(m_font_input->findText(QFontInfo(f).family()));
		m_font_size_input->setCurrentIndex(m_font_size_input->findText(QString::number(f.pointSize())));
		m_bold_action->setChecked(f.bold());
		m_italic_action->setChecked(f.italic());
		m_underline_action->setChecked(f.underline());
	}
}

void EditorWidget::colorChanged(const QColor &c)
{
	QPixmap pix(16, 16);
	pix.fill(c);
	m_color_action->setIcon(QIcon(pix));
}

void EditorWidget::alignmentChanged(Qt::Alignment a)
{
	if (m_initialPag == RichTextIndex) {
		if (a & Qt::AlignHCenter) {
			m_align_center_action->setChecked(true);
			m_align_checked = "center";
		} else if (a & Qt::AlignRight) {
			m_align_right_action->setChecked(true);
			m_align_checked = "right";
		} else if (a & Qt::AlignJustify) {
			m_align_justify_action->setChecked(true);
			m_align_checked = "justify";
		} else {
			m_align_left_action->setChecked(true);
			m_align_checked = "left";
		}
	}
}

void EditorWidget::pagIndexChanged(int newIndex)
{
	m_initialPag = (newIndex == 0 ? RichTextIndex : PlainTextIndex);
	// Anything changed, is there a need for a conversion?
	if (newIndex == PlainTextIndex && m_state != RichTextChanged)
		return;
	if (newIndex == RichTextIndex && m_state != PlainTextChanged)
		return;
	const State oldState = m_state;
	// Remember the cursor position, since it is invalidated by setPlainText
	int position = 0;
	QTextCursor cursor;
	if (newIndex == PlainTextIndex) {
		position =  editor_plain_text->textCursor().position();
		cursor = editor_plain_text->textCursor();
		editor_plain_text->setPlainText(smilesToString(editor_rich_text->text(Qt::RichText)));
	} else {
		position =  editor_rich_text->textCursor().position();
		cursor = editor_rich_text->textCursor();
		editor_rich_text->setHtml(smilesToImage(editor_plain_text->toPlainText()));
	}
	cursor.movePosition(QTextCursor::End);
	if (cursor.position() > position)
		cursor.setPosition(position);
	if ( newIndex == PlainTextIndex )
		editor_plain_text->setTextCursor(cursor);
	else
		editor_rich_text->setTextCursor(cursor);
	m_state = oldState; // Changed is triggered by setting the text
}

void EditorWidget::richTextChanged()
{
	m_state = RichTextChanged;
}

void EditorWidget::plainTextChanged()
{
	m_state = PlainTextChanged;
}

void EditorWidget::list_smile_itemDoubleClicked(QListWidgetItem *item)
{
	if (item) {
		if (m_initialPag == RichTextIndex) {
			editor_rich_text->insertHtml("<img src=\":smile_rs_"+ item->text() +"_"+ item->data(Qt::UserRole).toString() +"\"/> ");
			editor_rich_text->setFocus();
		} else {
			editor_plain_text->insertPlainText(item->text() +" ");
			editor_plain_text->setFocus();
		}
	}
}

void EditorWidget::on_show_source(bool checked)
{
	m_initialPag = checked ? PlainTextIndex : RichTextIndex;
	m_bold_action->setCheckable(!checked);
	m_italic_action->setCheckable(!checked);
	m_underline_action->setCheckable(!checked);
	m_align_left_action->setCheckable(!checked);
	m_align_center_action->setCheckable(!checked);
	m_align_right_action->setCheckable(!checked);
	m_align_justify_action->setCheckable(!checked);
	m_valign_sup_action->setCheckable(!checked);
	m_valign_sub_action->setCheckable(!checked);
	m_link_action->setCheckable(!checked);
	m_simplify_richtext_action->setVisible(!checked);

	if (m_initialPag == PlainTextIndex)
		editor_plain_text->setFocus();
	else {
		editor_rich_text->setFocus();
		if (m_align_checked == "center")
			m_align_center_action->setChecked(checked);
		else if (m_align_checked == "right")
			m_align_right_action->setChecked(checked);
		else if (m_align_checked == "justify")
			m_align_justify_action->setChecked(checked);
		else
			m_align_left_action->setChecked(checked);
		alignmentChanged(editor_rich_text->alignment());
	}

	pag_stacked->setCurrentIndex(checked ? 1 : 0);
}

void EditorWidget::on_show_find_replace(bool checked)
{
	QString str_select = "";
	if (m_initialPag == PlainTextIndex)
		str_select = editor_plain_text->textCursor().selectedText();
	else
		str_select = editor_rich_text->textCursor().selectedText();
	if (!str_select.isEmpty())
		txt_find->setText(str_select);
	toolbar_find_replace->setVisible(checked);
	txt_find->setFocus();
}

/*void EditorWidget::on_edit_export_pdf()
{
	if (m_initialPag == RichTextIndex) {
	#ifndef QT_NO_PRINTER
		QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
		if (!fileName.isEmpty()) {
			if (QFileInfo(fileName).suffix().isEmpty())
				fileName.append(".pdf");
			QPrinter printer(QPrinter::HighResolution);
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOutputFileName(fileName);
			editor_rich_text->document()->print(&printer);
		}
	#endif
	}
}*/

void EditorWidget::on_edit_font(const QString &font)
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<span style=\"font-family:'"+ font +"';\">"+ str_select +"</span>");
	} else {
		QTextCharFormat fmt;
		fmt.setFontFamily(font);
		mergeFormatOnWordOrSelection(fmt);
	}
}

void EditorWidget::on_edit_font_size(const QString &size)
{
	qreal pointSize = size.toFloat();
	if (size.toFloat() > 0) {
		if (m_initialPag == PlainTextIndex) {
			const QString str_select = editor_plain_text->textCursor().selectedText();
			if (!str_select.isEmpty())
				editor_plain_text->textCursor().insertText("<span style=\"font-size:"+ size +"pt;\">"+ str_select +"</span>");
		} else {
				QTextCharFormat fmt;
				fmt.setFontPointSize(pointSize);
				mergeFormatOnWordOrSelection(fmt);
		}
	}
}

void EditorWidget::on_edit_cut()
{
	if (m_initialPag == PlainTextIndex)
		editor_plain_text->cut();
	else
		editor_rich_text->cut();
}

void EditorWidget::on_edit_copy()
{
	if (m_initialPag == PlainTextIndex)
		editor_plain_text->copy();
	else
		editor_rich_text->copy();
}

void EditorWidget::on_edit_paste()
{
	if (m_initialPag == PlainTextIndex)
		editor_plain_text->paste();
	else
		editor_rich_text->paste();
}

void EditorWidget::on_edit_undo()
{
	if (m_initialPag == PlainTextIndex)
		editor_plain_text->undo();
	else
		editor_rich_text->undo();
}

void EditorWidget::on_edit_redo()
{
	if (m_initialPag == PlainTextIndex)
		editor_plain_text->redo();
	else
		editor_rich_text->redo();
}

void EditorWidget::on_edit_bold()
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<strong>"+ str_select +"</strong>");
	} else {
		QTextCharFormat fmt;
		fmt.setFontWeight(m_bold_action->isChecked() ? QFont::Bold : QFont::Normal);
		mergeFormatOnWordOrSelection(fmt);
	}
}

void EditorWidget::on_edit_italic()
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<em>"+ str_select +"</em>");
	} else {
		QTextCharFormat fmt;
		fmt.setFontItalic(m_italic_action->isChecked());
		mergeFormatOnWordOrSelection(fmt);
	}
}

void EditorWidget::on_edit_underline()
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<u>"+ str_select +"</u>");
	} else {
		QTextCharFormat fmt;
		fmt.setFontUnderline(m_underline_action->isChecked());
		mergeFormatOnWordOrSelection(fmt);
	}
}

void EditorWidget::on_edit_text_align(QAction *action)
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		QString txt_align = "";
		if (action == m_align_center_action)
			txt_align = " align=\"center\"";
		else if (action == m_align_right_action)
			txt_align = " align=\"right\"";
		else if (action == m_align_justify_action)
			txt_align = " align=\"justify\"";
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<p"+ txt_align +">"+ str_select +"</p>");
	} else {
		if (action == m_align_center_action)
			editor_rich_text->setAlignment(Qt::AlignHCenter);
		else if (action == m_align_right_action)
			editor_rich_text->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
		else if (action == m_align_justify_action)
			editor_rich_text->setAlignment(Qt::AlignJustify);
		else
			editor_rich_text->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
		alignmentChanged(editor_rich_text->alignment());
	}
}

void EditorWidget::on_edit_valign_sup()
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<span style=\"vertical-align:super;\">"+ str_select +"</span>");
	} else {
		const QTextCharFormat::VerticalAlignment align = m_valign_sup_action->isChecked() ?
				QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal;
		QTextCharFormat fmt;
		fmt.setVerticalAlignment(align);
		mergeFormatOnWordOrSelection(fmt);
		m_valign_sub_action->setChecked(false);
	}
}

void EditorWidget::on_edit_valign_sub()
{
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<span style=\"vertical-align:sub;\">"+ str_select +"</span>");
	} else {
		const QTextCharFormat::VerticalAlignment align = m_valign_sub_action->isChecked() ?
				QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal;
		QTextCharFormat fmt;
		fmt.setVerticalAlignment(align);
		mergeFormatOnWordOrSelection(fmt);
		m_valign_sup_action->setChecked(false);
	}
}

void EditorWidget::on_edit_image()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Seleccionar una imagen"), "",
							tr("Imagenes") +" (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tif *.tiff *.xbm *.xpm *.svg);;"+
							tr("Imagen") +" JPEG (*.jpg *.jpeg);;"+
							tr("Imagen") +" PNG (*.png);;"+
							tr("Imagen") +" BMP (*.bmp);;"+
							tr("Imagen") +" GIF (*.gif);;"+
							tr("Imagen") +" TIFF (*.tif *.tiff);;"+
							tr("Imagen") +" SVG (*.svg);;"+
							tr("Imagen") +" PNM (*.pbm *.pgm *.ppm);;"+
							tr("Imagen") +" X BitMap (*.xbm);;"+
							tr("Imagen") +" X PixMap (*.xpm);;"+
							tr("Imágenes soportadas") +" (*)", 0);

	if (!fileName.isEmpty() && QFile::exists(fileName)) {
		if (m_initialPag == PlainTextIndex)
			editor_plain_text->textCursor().insertText("<img src=\""+ fileName +"\"/>");
		else
			editor_rich_text->textCursor().insertHtml("<img src=\""+ fileName +"\"/>");
	}
}

void EditorWidget::on_edit_link(bool checked)
{
	bool ok = false;
	QString newUrl, old_url = "";
	if (checked && m_initialPag == RichTextIndex)
		old_url = editor_rich_text->currentCharFormat().anchorHref();
	newUrl = QInputDialog::getText(this, tr("Crear un enlace"), tr("URL del enlace") +":", QLineEdit::Normal, old_url, &ok);

	if( !newUrl.startsWith("://") )
		newUrl.prepend("http://");

	if (m_initialPag == PlainTextIndex) {
		QString str_select = editor_plain_text->textCursor().selectedText();
		if (str_select.isEmpty())
			str_select = newUrl;
		if (ok)
			editor_plain_text->textCursor().insertText("<a href=\""+ newUrl +"\">"+ str_select +"</a>");
	} else {
		QTextCharFormat fmt;
		if (checked && ok) {
			if (old_url.isEmpty())
				editor_rich_text->textCursor().insertHtml("<a href=\""+ newUrl +"\">"+ newUrl +"</a>");
			else {
				fmt.setAnchor(true);
				fmt.setAnchorHref(newUrl);
				fmt.setForeground(QApplication::palette().color(QPalette::Link));
				fmt.setFontUnderline(true);
			}
		} else {
			fmt.setAnchor(false);
			fmt.setForeground(QApplication::palette().color(QPalette::Text));
			fmt.setFontUnderline(false);
		}
		mergeFormatOnWordOrSelection(fmt);
	}
}

void EditorWidget::on_edit_color()
{
	QColor color = QColorDialog::getColor(m_color, this);
	if (!color.isValid() || color == m_color)
		return;
	m_color = color;
	if (m_initialPag == PlainTextIndex) {
		const QString str_select = editor_plain_text->textCursor().selectedText();
		if (!str_select.isEmpty())
			editor_plain_text->textCursor().insertText("<span style=\"color:"+ color.name() +";\">"+ str_select +"</span>");
	} else {
		QTextCharFormat fmt;
		fmt.setForeground(color);
		mergeFormatOnWordOrSelection(fmt);
	}
	colorChanged(color);
}

void EditorWidget::txtFindTextChanged(QString text)
{
	emit mark(text, chkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
}

void EditorWidget::chkCaseSensitive_toggled(bool checked)
{
	emit mark(txt_find->text(), checked ? Qt::CaseSensitive : Qt::CaseInsensitive);
}

void EditorWidget::chkReplace_toggled(bool checked)
{
	txt_replace->setEnabled(checked);
	btnReplace->setEnabled(checked);
	btnReplaceAndNext->setEnabled(checked);
	btnReplaceAll->setEnabled(checked);
}

void EditorWidget::btnFindBack_clicked()
{
	findFirst(txt_find->text(), true);
}

void EditorWidget::btnFindNext_clicked()
{
	findFirst(txt_find->text(), false);
}

void EditorWidget::btnReplace_clicked()
{
	findReplace(txt_find->text(), txt_replace->text());
}

void EditorWidget::btnReplaceAndNext_clicked()
{
	findReplace(txt_find->text(), txt_replace->text(), true);
}

void EditorWidget::btnReplaceAll_clicked()
{
	findReplace(txt_find->text(), txt_replace->text(), true, true);
}

void EditorWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
	if (m_initialPag == RichTextIndex) {
		fontChanged(format.font());
		colorChanged(format.foreground().color());

		const QTextCharFormat::VerticalAlignment valign = format.verticalAlignment();
		const bool superScript = valign == QTextCharFormat::AlignSuperScript;
		const bool subScript = valign == QTextCharFormat::AlignSubScript;
		m_valign_sup_action->setChecked(superScript);
		m_valign_sub_action->setChecked(subScript);
		m_link_action->setChecked(format.isAnchor());
	}
}

void EditorWidget::cursorPositionChanged()
{
	if (m_initialPag == RichTextIndex)
		alignmentChanged(editor_rich_text->alignment());
}

void EditorWidget::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
	if (const QMimeData *md = QApplication::clipboard()->mimeData())
		m_paste_action->setEnabled(md->hasText());
#endif
}
