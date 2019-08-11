#include <QFileDialog>

#include "filedialogproperty.h"

FileDialogProperty::FileDialogProperty(const QString &name, const QString &value, const QString &defaultValue):
	PropertyItem(name),
	m_value(value),
	m_defaultValue(defaultValue)
{
	m_caption   = "";
	m_directory = "";
	m_filter    = "";
	m_isDir     = false;
	m_isSave    = false;
	setValue(value);
}

FileDialogProperty::FileDialogProperty(PropertyItem *top, const QString &name, const QString &value, const QString &defaultValue):
	PropertyItem(top, name),
	m_value(value),
	m_defaultValue(defaultValue)
{
	m_caption   = "";
	m_directory = "";
	m_filter    = "";
	m_isDir     = false;
	m_isSave    = false;
	setValue(value);
}

void FileDialogProperty::setFileDialog(const QString &caption, const QString &directory, const QString &filter, bool isSave)
{
	m_caption   = caption;
	m_directory = directory;
	m_filter    = filter;
	m_isDir     = false;
	m_isSave    = isSave;
	QString icon = (m_isSave ? "save" : "open");
	setBtnIcon(":/images/"+ icon +"_file.png");
}

void FileDialogProperty::setFolderDialog(const QString &caption, const QString &directory)
{
	m_caption   = caption;
	m_directory = directory;
	m_filter    = "";
	m_isDir     = true;
	m_isSave    = false;
	setBtnIcon(":/images/open_folder.png");
}

void FileDialogProperty::setValue(const QString &value)
{
	m_value = value;
	PropertyItem::setValue();
}

const QString &FileDialogProperty::getValue() const
{
	return m_value;
}

void FileDialogProperty::setBtnText(const QString &text)
{
	m_btn_text = text;
}

void FileDialogProperty::setBtnIcon(const QString &icon)
{
	m_btn_icon = icon;
}

void FileDialogProperty::setCaption(const QString &caption)
{
	m_caption = caption;
}

const QString &FileDialogProperty::getCaption() const
{
	return m_caption;
}

void FileDialogProperty::setDirectory(const QString &directory)
{
	m_directory = directory;
}

const QString &FileDialogProperty::getDirectory() const
{
	return m_directory;
}

void FileDialogProperty::setFilter(const QString &filter)
{
	m_filter = filter;
}

const QString &FileDialogProperty::getFilter() const
{
	return m_filter;
}

void FileDialogProperty::setIsDir(bool isDir)
{
	m_isDir = isDir;
}

bool FileDialogProperty::getIsDir() const
{
	return m_isDir;
}

void FileDialogProperty::setIsSave(bool isSave)
{
	m_isSave = isSave;
}

bool FileDialogProperty::getIsSave() const
{
	return m_isSave;
}

QVariant FileDialogProperty::getVariantValue() const
{
	return m_value;
}

void FileDialogProperty::displayValue()
{
	setText(1, m_value);
}

QWidget *FileDialogProperty::createEditor() const
{
	m_editor = new QLineEdit();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<FileDialogProperty *>(this));
	if (!m_btn_text.isEmpty())
		hostEditor->setBtnText(m_btn_text);
	if (!m_btn_icon.isEmpty())
		hostEditor->setBtnIcon(m_btn_icon);
	return hostEditor;
}

void FileDialogProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setText(m_value);
		m_editor->selectAll();
		QObject::connect(m_editor, SIGNAL(textChanged(QString)), treeWidget(), SLOT(onValueChanged(QString)));
	}
}

void FileDialogProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		QString value = m_editor->text();
		setValue(value);
	}
}

// ButtonsEditor --
FileDialogProperty::ButtonsEditor::ButtonsEditor(QLineEdit *fileEditor, FileDialogProperty *property):
	TPropertyButton<QLineEdit>(fileEditor)
{
	m_property = property;
}

void FileDialogProperty::ButtonsEditor::onButtonClicked()
{
	QString archivo = "";
	if (m_property->getIsDir())
		archivo = QFileDialog::getExistingDirectory(this, m_property->getCaption(), m_property->getDirectory(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	else {
		if (m_property->getIsSave())
			archivo = QFileDialog::getSaveFileName(this, m_property->getCaption(), m_property->getDirectory(), m_property->getFilter());
		else
			archivo = QFileDialog::getOpenFileName(this, m_property->getCaption(), m_property->getDirectory(), m_property->getFilter());
	}

	if (!archivo.isEmpty())
	{
		getEditor()->setText(archivo);
		m_property->finishEdit();
	}
}

void FileDialogProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setText(m_property->getDefaultValue());
	m_property->finishEdit();
}
