#include "comboboxproperty.h"

ComboBoxProperty::ComboBoxProperty(const QString &name, const ListData &list, int index, int defaultIndex, QSize iconSize):
	PropertyItem(name),
	m_index(index),
	m_listData(list),
	m_defaultIndex(defaultIndex),
	m_iconSize(iconSize)
{
	m_minHeight = 24;
	m_editable = false;
	setIndex(m_index);
}

ComboBoxProperty::ComboBoxProperty(PropertyItem *top, const QString &name, const ListData &list, int index, int defaultIndex, QSize iconSize):
	PropertyItem(top, name),
	m_index(index),
	m_listData(list),
	m_defaultIndex(defaultIndex),
	m_iconSize(iconSize)
{
	m_minHeight = 24;
	m_editable = false;
	setIndex(m_index);
}

void ComboBoxProperty::setIndex(int index)
{
	m_index = index;
	PropertyItem::setValue();
}

int ComboBoxProperty::getIndex() const
{
	return m_index;
}

void ComboBoxProperty::setIconSize(const QSize &size)
{
	m_iconSize = size;
	m_editor->setIconSize(size);
}

void ComboBoxProperty::setList(const ListData &list)
{
// handle editor...
	m_listData = list;
	PropertyItem::setValue();
}

int ComboBoxProperty::findIndexOf(const QString &value, int from, bool isData) const
{
	int idx = -1;
	if (from < m_listData.size())
	{
		int i = from;
		bool isFinding = false;
		while (i < m_listData.size() && !isFinding)
		{
			if (isData)
			{
				if (m_listData.at(i).m_userData.toString() == value)
				{
					idx = i;
					isFinding = true;
				}
			} else {
				if (m_listData.at(i).m_text == value)
				{
					idx = i;
					isFinding = true;
				}
			}

			if (!isFinding)
				++i;
		}
	}
	return idx;
}

int ComboBoxProperty::indexOfData(const QString &value, int from) const
{
	return findIndexOf(value, from, true);
}

int ComboBoxProperty::indexOf(const QString &value, int from) const
{
	return findIndexOf(value, from);
}

void ComboBoxProperty::onShowEditor(QWidget *editWidget)
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(editWidget);
	if (hostEditor != NULL)
	{
		if (m_editor->count() == 0)
		{
			for (int i = 0; i < m_listData.size(); i++)
			{
				const ListDataItem &data = m_listData.at(i);
				m_editor->addItem(data.m_icon, data.m_text, data.m_userData);
			}
			QObject::connect(m_editor, SIGNAL(currentIndexChanged(int)), treeWidget(), SLOT(onValueChanged(int)));
		}
	}
}

QVariant ComboBoxProperty::getVariantValue() const
{
	return m_index;
}

void ComboBoxProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	if (m_listData.isEmpty())
	{
		setText(1, QObject::tr("<empty>"));
		setIcon(1, QIcon());
	} else {
		if (m_index < 0)
		{
			setText(1, QObject::tr("<unknown>"));
			setIcon(1, QIcon());
		} else {
			const ListDataItem &data = m_listData.at(m_index);
			setText(1, data.m_text);
			setIcon(1, data.m_icon);
		}
	}

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

void ComboBoxProperty::validateValue()
{
	if (m_index < 0)
		m_index = 0;
	if (m_index >= m_listData.size())
		m_index = m_listData.size() - 1;
}

QWidget *ComboBoxProperty::createEditor() const
{
	m_editor = new QComboBox();
	m_editor->setEditable(m_editable);
	m_editor->setIconSize(m_iconSize);
	m_editor->setMinimumHeight(m_minHeight);
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<ComboBoxProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void ComboBoxProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		m_editor->setCurrentIndex(m_index);
	}
}

void ComboBoxProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		int index = -1;
		if (m_editable)
		{
			QString text = m_editor->currentText();
			index = m_editor->findData(text);
			if (index == -1)
			{
				m_listData.append(ListDataItem(QIcon(), text, text));
				m_editor->addItem(QIcon(), text, text);
				index = m_editor->findData(text);
				m_editor->setCurrentIndex(index);
			}
		}
		index = m_editor->currentIndex();
		if (index != m_index)
			setIndex(index);
	}
}

// ButtonsEditor --
ComboBoxProperty::ButtonsEditor::ButtonsEditor(QComboBox *editor, ComboBoxProperty *property):
	TPropertyButton<QComboBox>(editor)
{
	m_property = property;
}

void ComboBoxProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->setCurrentIndex(m_property->getDefaultIndex());
	m_property->finishEdit();
}
