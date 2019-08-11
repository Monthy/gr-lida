#include "propertyeditor.h"

#include <QKeyEvent>
#include <QHeaderView>
#include <QApplication>
#include <QPainter>

PropertyEditor::PropertyEditor(QWidget *parent) :
	QTreeWidget(parent),
	m_addingItem(false)
{
	QStringList labels;
	labels << tr("Parameter") << tr("Value");
	setHeaderLabels(labels);

#if QT_VERSION >= 0x050000
	header()->setSectionsMovable(false);
//	header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//	header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
#else
	header()->setMovable(false);
//	header()->setResizeMode(0, QHeaderView::ResizeToContents);
//	header()->setResizeMode(1, QHeaderView::ResizeToContents);
#endif

//	setRootIsDecorated(false);
	setUniformRowHeights(true);
	setAlternatingRowColors(true);
	setAllColumnsShowFocus(true);

	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
//	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
	connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void PropertyEditor::adjustToContents()
{
	header()->resizeSections(QHeaderView::ResizeToContents);
}

bool PropertyEditor::addProperty(PropertyItem *prop, const QByteArray &key)
{
	if (prop == NULL)
		return false;

	m_addingItem = true;

	prop->setSizeHint(1, QSize(100, 24));

	if (!key.isEmpty())
		prop->setKey(key);

	addTopLevelItem(prop);

	prop->onAdded();

	expandItem(prop);

	m_addingItem = false;

	return true;
}

bool PropertyEditor::removeProperty(PropertyItem *prop)
{
	if (prop == NULL)
		return false;

	int idx = indexOfTopLevelItem(prop);
	takeTopLevelItem(idx);
	delete prop;

	return true;
}

// slots
void PropertyEditor::onItemClicked(QTreeWidgetItem *item, int column)
{
	PropertyItem *prop = dynamic_cast<PropertyItem *>(item);
	if (prop != NULL && !prop->isDisabled())
	{
		if (column == 1)
			prop->startEdit();
		else
			prop->finishEdit();
	}
}

void PropertyEditor::onItemChanged(QTreeWidgetItem *item, int column)
{
	if (m_addingItem)
		return;

	PropertyItem *prop = dynamic_cast<PropertyItem *>(item);
	if (prop != NULL)
		Q_EMIT itemChanged(prop, column);
}

void PropertyEditor::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == previous)
		return;

	PropertyItem *oldProp = dynamic_cast<PropertyItem *>(previous);
	if (oldProp != NULL)
		oldProp->finishEdit();
}

void PropertyEditor::onValueChanged(const QString &value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(int value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(bool value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(double value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(const QColor &value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(const QFont &value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(const QDate &value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(const QTime &value)
{
	emitValueChanged(value);
}

void PropertyEditor::onValueChanged(const QDateTime &value)
{
	emitValueChanged(value);
}

void PropertyEditor::emitValueChanged(const QVariant &value)
{
	PropertyItem *prop = dynamic_cast<PropertyItem *>(currentItem());
	if (prop != NULL)
		Q_EMIT valueChanged(prop, value);
}

// keys event
void PropertyEditor::keyPressEvent(QKeyEvent *event)
{
	PropertyItem *prop = dynamic_cast<PropertyItem *>(currentItem());
	if (prop != NULL)
	{
		QWidget *editWidget = prop->getActiveEditor();

		if (prop->onKeyPressed(event, editWidget))
			return;

		switch (event->key())
		{
			case Qt::Key_Return:
				if (editWidget == NULL)
					prop->startEdit();
				else {
					if (editWidget->isVisible())
						prop->finishEdit();
					else
						prop->startEdit();
				}
			break;
			case Qt::Key_Escape:
				if (editWidget != NULL && editWidget->isVisible())
					prop->finishEdit(true);
			break;
			case Qt::Key_Space:
				if (prop->isCheckable())
						prop->setChecked(!prop->isChecked());
				return;
			default:
			break;
		}
	}
	QTreeWidget::keyPressEvent(event);
}
