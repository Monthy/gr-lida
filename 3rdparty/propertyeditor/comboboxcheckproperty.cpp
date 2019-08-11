#include "comboboxcheckproperty.h"

ComboBoxCheckProperty::ComboBoxCheckProperty(const QString &name, const ListData &list, const QString &value, const QString &defaultValue, const QString &sep, bool useData):
	PropertyItem(name),
	m_listData(list),
	m_value(value),
	m_defaultValue(defaultValue),
	m_sep(sep),
	m_useData(useData),
	m_index(0)
{
	setValue(m_value);
}

ComboBoxCheckProperty::ComboBoxCheckProperty(PropertyItem *top, const QString &name, const ListData &list, const QString &value, const QString &defaultValue, const QString &sep, bool useData):
	PropertyItem(top, name),
	m_listData(list),
	m_value(value),
	m_defaultValue(defaultValue),
	m_sep(sep),
	m_useData(useData),
	m_index(0)
{
	setValue(m_value);
}

void ComboBoxCheckProperty::setList(const ListData &list)
{
// handle editor...
	m_listData = list;
	PropertyItem::setValue();
}

void ComboBoxCheckProperty::setValue(const QStringList &value)
{
	m_value = value.join(m_sep);
	PropertyItem::setValue();
}

void ComboBoxCheckProperty::setValue(const QString &value)
{
	m_value = value;
	PropertyItem::setValue();
}

QString ComboBoxCheckProperty::getValue() const
{
	return m_value;
}

void ComboBoxCheckProperty::onShowEditor(QWidget *editWidget)
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(editWidget);
	if (hostEditor != NULL)
	{
		if (m_editor->count() == 0)
		{
			m_editor->blockSignals(true);
			for (int i = 0; i < m_listData.size(); i++)
			{
				const ListDataItem &data = m_listData.at(i);
				m_editor->addItem(data.m_icon, data.m_text, data.m_userData);
			}
			m_editor->setSeparator(m_sep);
			if (m_useData)
				m_editor->setCheckedItemsUserData(m_value);
			else
				m_editor->setCheckedItems(m_value);
			m_editor->blockSignals(false);
		}
	}
}

QVariant ComboBoxCheckProperty::getVariantValue() const
{
	return m_value;
}

void ComboBoxCheckProperty::displayValue()
{
	if (treeWidget())
		treeWidget()->blockSignals(true);

	if (m_listData.isEmpty())
	{
		setText(1, QObject::tr("<empty>"));
		setIcon(1, QIcon());
	} else {
		setText(1, m_value);
		if (m_index < 0)
			setIcon(1, QIcon());
		else
			setIcon(1, m_listData.at(m_index).m_icon);
	}

	if (treeWidget())
		treeWidget()->blockSignals(false);
}

QWidget *ComboBoxCheckProperty::createEditor() const
{
	m_editor = new QCheckComboBox();
	ButtonsEditor *hostEditor = new ButtonsEditor(m_editor, const_cast<ComboBoxCheckProperty *>(this));
	hostEditor->setBtnVisible(false);
	return hostEditor;
}

void ComboBoxCheckProperty::valueToEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		if (m_useData)
			m_editor->setCheckedItemsUserData(m_value);
		else
			m_editor->setCheckedItems(m_value);
		QObject::connect(m_editor, SIGNAL(textChanged(QString)), treeWidget(), SLOT(onValueChanged(QString)));
	}
}

void ComboBoxCheckProperty::valueFromEditor()
{
	ButtonsEditor *hostEditor = dynamic_cast<ButtonsEditor *>(getActiveEditor());
	if (hostEditor != NULL)
	{
		int index = m_editor->currentIndex();
		if (index != m_index)
			m_index = index;

		if (m_useData)
			m_value = m_editor->getCheckedItemsUserData();
		else
			m_value = m_editor->getCheckedItems();

		setValue(m_value);
	}
}

// ButtonsEditor --
ComboBoxCheckProperty::ButtonsEditor::ButtonsEditor(QCheckComboBox *editor, ComboBoxCheckProperty *property):
	TPropertyButton<QCheckComboBox>(editor)
{
	m_property = property;
}

void ComboBoxCheckProperty::ButtonsEditor::onButtonDefaultClicked()
{
	getEditor()->emitTextChanged(m_property->getDefaultValue());
	m_property->setValue(m_property->getDefaultValue());
	m_property->finishEdit();
}
