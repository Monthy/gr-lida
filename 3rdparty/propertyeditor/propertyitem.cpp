#include "propertyitem.h"

PropertyItem::PropertyItem(const QString &name):
	QTreeWidgetItem(),
	m_name(name),
	m_isCheckable(false),
	m_editorIsPrivate(false)
{
	setText(0, m_name);
	m_key = "";
}

PropertyItem::PropertyItem(PropertyItem *top, const QString &name):
	QTreeWidgetItem(top),
	m_name(name),
	m_isCheckable(false),
	m_editorIsPrivate(false)
{
	setText(0, m_name);
	m_key = "";
}

void PropertyItem::setChecked(Qt::CheckState state)
{
	m_isCheckable = true;
	setCheckState(0, state);
}

void PropertyItem::setChecked(bool checked)
{
	setChecked(checked ? Qt::Checked : Qt::Unchecked);
}

bool PropertyItem::isChecked() const
{
	return (checkState(0) == Qt::Checked);
}

bool PropertyItem::isCheckable() const
{
	return m_isCheckable;
}

void PropertyItem::setBackground(const QBrush &bg)
{
	for (int col = 0; col < columnCount(); ++col)
		QTreeWidgetItem::setBackground(col, bg);
}

void PropertyItem::setTextColor(const QColor &color)
{
	for (int col = 0; col < columnCount(); ++col)
		QTreeWidgetItem::setTextColor(col, color);
}

void PropertyItem::setValue()
{
	validateValue();

	displayValue();

	valueToEditor();
}

void PropertyItem::startEdit()
{
	QWidget *editWidget = getActiveEditor();

// create if needed
	if (editWidget == NULL)
		editWidget = createEditor();

	if (editWidget == NULL)
		return;

// show it
	if (!m_editorIsPrivate || (getActiveEditor() == NULL))
		treeWidget()->setItemWidget(this, 1, editWidget);

	if (m_editorIsPrivate)
		editWidget->show();

	onShowEditor(editWidget);
// set value and constraints to the editor
	valueToEditor();
// focus
	editWidget->setFocus();
}

void PropertyItem::finishEdit(bool cancel)
{
	QWidget *editWidget = getActiveEditor();
	if (editWidget != NULL)
	{
		if (!cancel)
			valueFromEditor();

		if (m_editorIsPrivate)
			editWidget->hide();
		else
			treeWidget()->removeItemWidget(this, 1);
	}
	treeWidget()->setFocus();
}

// internal
QWidget *PropertyItem::getActiveEditor()
{
	if (treeWidget() != NULL)
		return treeWidget()->itemWidget(this, 1);
	return NULL;
}

void PropertyItem::setEditorPrivate()
{
	m_editorIsPrivate = true;
}
