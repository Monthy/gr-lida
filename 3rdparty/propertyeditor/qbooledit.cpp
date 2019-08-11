#include <QApplication>
#include <QMouseEvent>
#include <QHBoxLayout>

#include "qbooledit.h"

QBoolEdit::QBoolEdit(QWidget *parent):
	QWidget(parent),
	m_checkBox(new QCheckBox(this)),
	m_textTrue(QObject::tr("True")), m_textFalse(QObject::tr("False"))
{
	setContentsMargins(0, 0, 0, 0);
	QHBoxLayout *h_layout = new QHBoxLayout(this);
	if (QApplication::layoutDirection() == Qt::LeftToRight)
		h_layout->setContentsMargins(3, 1, 0, 1);
	else
		h_layout->setContentsMargins(0, 1, 3, 1);
	h_layout->setSpacing(0);
	m_checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	h_layout->addWidget(m_checkBox);
	connect(m_checkBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));

	setText(m_textTrue, m_textFalse);
	setLayout(h_layout);
	setFocusProxy(m_checkBox);
}

void QBoolEdit::setText(const QString &text)
{
	m_textTrue = text;
	m_textFalse = m_textTrue;
	m_checkBox->setText(isChecked() ? m_textTrue : m_textFalse);
}

void QBoolEdit::setText(const QString &textTrue, const QString &textFalse)
{
	m_textTrue = textTrue;
	m_textFalse = textFalse;
	m_checkBox->setText(isChecked() ? m_textTrue : m_textFalse);
}

void QBoolEdit::setIcon(const QIcon &icon)
{
	m_checkBox->setIcon(icon);
}

void QBoolEdit::setIconSize(const QSize &size)
{
	m_checkBox->setIconSize(size);
}

void QBoolEdit::setBgColor(const QColor &color)
{
	m_checkBox->setStyleSheet("background-color: "+ color.name() +";");
}

bool QBoolEdit::isChecked() const
{
	return m_checkBox->isChecked();
}

void QBoolEdit::setChecked(bool checked)
{
	m_checkBox->setChecked(checked);
}

Qt::CheckState QBoolEdit::checkState() const
{
	return m_checkBox->checkState();
}

void QBoolEdit::setCheckState(Qt::CheckState state)
{
	m_checkBox->setCheckState(state);
}

bool QBoolEdit::blockCheckBoxSignals(bool block)
{
	return m_checkBox->blockSignals(block);
}

void QBoolEdit::mouseReleaseEvent(QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
	if (event->button() != Qt::LeftButton)
		return;

	m_checkBox->toggle();
	event->accept();
}

void QBoolEdit::onToggled(bool checked)
{
	m_checkBox->setText(checked ? m_textTrue : m_textFalse);
	Q_EMIT toggled(checked);
}
