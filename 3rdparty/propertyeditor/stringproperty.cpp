#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPlainTextEdit>
#include <QPushButton>

#include "stringproperty.h"

StringProperty::StringProperty(const QString &name, const QString &value, const QString &defaultValue, bool multiLine):
	PropertyItem(name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_multiLine(multiLine)
{
	m_readonly = false;
	setValue(value);
}

StringProperty::StringProperty(PropertyItem *top, const QString &name, const QString &value, const QString &defaultValue, bool multiLine):
	PropertyItem(top, name),
	m_value(value),
	m_defaultValue(defaultValue),
	m_multiLine(multiLine)
{
	m_readonly = false;
	setValue(value);
}

void StringProperty::setValue(const QString &value)
{
	m_value = value;
	PropertyItem::setValue();
}

const QString &StringProperty::getValue() const
{
	return m_value;
}

QVariant StringProperty::getVariantValue() const
{
	return m_value;
}

void StringProperty::displayValue()
{
	setText(1, m_value);
}

QWidget *StringProperty::createEditor() const
{
	m_editor = new QLineEdit();
	m_editor->setReadOnly(m_readonly);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<StringProperty *>(this));
	hostEditor->setBtnVisible(m_multiLine);
	hostEditor->setBtnIcon(":/images/edit.png");
	return hostEditor;
}

void StringProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setText(m_value);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(textChanged(QString)), treeWidget(), SLOT(onValueChanged(QString)));
	}
}

void StringProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
		setValue(m_editor->text());
}

// ButtonsEditor --
StringProperty::ButtonsEditor::ButtonsEditor(QLineEdit *editor, StringProperty *property):
	TPropertyButton<QLineEdit>(editor)
{
	m_property = property;
}

void StringProperty::ButtonsEditor::onButtonClicked()
{
	QVBoxLayout *mainLayout;
	QHBoxLayout *hLayout;
	QSpacerItem *hSpacer;
	QPlainTextEdit *textEdit;
	QPushButton *btnAcept, *btnCancel;

	QDialog *Dialog = new QDialog(this);
	Dialog->resize(320, 250);
	Dialog->setWindowTitle("Edit text");

	mainLayout = new QVBoxLayout(Dialog);
	mainLayout->setContentsMargins(4, 4, 4, 4);
		textEdit = new QPlainTextEdit(Dialog);
		textEdit->setPlainText(getEditor()->text());
		textEdit->setReadOnly(getEditor()->isReadOnly());
	mainLayout->addWidget(textEdit);
		hLayout = new QHBoxLayout();
			hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		hLayout->addItem(hSpacer);
			btnAcept = new QPushButton(Dialog);
			btnAcept->setText(QObject::tr("Acept"));
			btnAcept->setMinimumSize(QSize(80, 24));
			connect(btnAcept, SIGNAL(clicked()), Dialog, SLOT(accept()));
		hLayout->addWidget(btnAcept);
			btnCancel = new QPushButton(Dialog);
			btnCancel->setText(QObject::tr("Cancel"));
			btnCancel->setMinimumSize(QSize(80, 24));
			connect(btnCancel, SIGNAL(clicked()), Dialog, SLOT(reject()));
		hLayout->addWidget(btnCancel);
	mainLayout->addLayout(hLayout);

	if (Dialog->exec() == QDialog::Accepted)
	{
		getEditor()->setText(textEdit->toPlainText());
		m_property->finishEdit();
	}

	delete Dialog;
}

void StringProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setText(m_property->getDefaultValue());
	m_property->finishEdit();
}
