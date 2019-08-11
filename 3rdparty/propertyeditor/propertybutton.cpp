#include "propertybutton.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QEvent>

PropertyButton::PropertyButton(QWidget *hostedEditor, QWidget *parent):
	QWidget(parent),
	m_hostedEditor(hostedEditor)
{
	setContentsMargins(0, 0, 0, 0);

	QHBoxLayout *hbl = new QHBoxLayout(this);
	hbl->setContentsMargins(0, 0, 0, 0);
	hbl->setSpacing(0);
	setLayout(hbl);

	m_hostedEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	hbl->addWidget(m_hostedEditor);

	QSizePolicy buttonSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	m_button = new QToolButton(this);
	m_button->setSizePolicy(buttonSizePolicy);
	m_button->setText("...");
	hbl->addWidget(m_button);

	m_button_def = new QToolButton(this);
	m_button_def->setSizePolicy(buttonSizePolicy);
	m_button_def->setText("");
	m_button_def->setIcon(QIcon(":/images/resetproperty.png"));
	m_button_def->setIconSize(QSize(8, 6));
	hbl->addWidget(m_button_def);

	connect(m_button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
	connect(m_button_def, SIGNAL(clicked()), this, SLOT(onButtonDefaultClicked()));

	if (qobject_cast<QToolButton *>(m_hostedEditor) || qobject_cast<QPushButton *>(m_hostedEditor))
		connect(m_hostedEditor, SIGNAL(clicked()), this, SLOT(onEditorClicked()));
}

PropertyButton::~PropertyButton()
{
	layout()->removeWidget(m_hostedEditor);
	m_hostedEditor->setParent(NULL);
	m_hostedEditor->hide();
}

void PropertyButton::setBtnText(const QString &text)
{
	m_button->setText(text);
}

void PropertyButton::setBtnIcon(const QString &icon)
{
	m_button->setIcon(QIcon(icon));
}

void PropertyButton::setBtnIconSize(const QSize &size)
{
	m_button->setIconSize(size);
}

void PropertyButton::setBtnVisible(bool visible)
{
	m_button->setVisible(visible);
}

void PropertyButton::setBtnDefText(const QString &text)
{
	m_button_def->setText(text);
}

void PropertyButton::setBtnDefIcon(const QString &icon)
{
	m_button_def->setIcon(QIcon(icon));
}

void PropertyButton::setBtnDefIconSize(const QSize &size)
{
	m_button_def->setIconSize(size);
}

void PropertyButton::setBtnDefVisible(bool visible)
{
	m_button_def->setVisible(visible);
}

bool PropertyButton::event(QEvent *e)
{
	if (e->type() == QEvent::FocusIn)
	{
		m_hostedEditor->setFocus();
		e->accept();
		return true;
	}
	return QWidget::event(e);
}

void PropertyButton::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	m_hostedEditor->show();
}
