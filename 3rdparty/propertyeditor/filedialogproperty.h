#ifndef FILEDIALOGPROPERTY_H
#define FILEDIALOGPROPERTY_H

#include <QLineEdit>

#include "propertyitem.h"
#include "propertybutton.h"

class FileDialogProperty : public PropertyItem
{
public:
	FileDialogProperty(const QString &name, const QString &value = "", const QString &defaultValue = "");
	FileDialogProperty(PropertyItem *top, const QString &name, const QString &value = "", const QString &defaultValue = "");

	void setFileDialog(const QString &caption = "", const QString &directory = "", const QString &filter = "", bool isSave = false);
	void setFolderDialog(const QString &caption = "", const QString &directory = "");

	void setValue(const QString &value);
	const QString &getValue() const;

	void setDefaultValue(const QString &value) { m_defaultValue = value; }
	const QString &getDefaultValue() const { return m_defaultValue; }

	void setBtnText(const QString &text);
	void setBtnIcon(const QString &icon);

	void setCaption(const QString &caption);
	const QString &getCaption() const;

	void setDirectory(const QString &directory);
	const QString &getDirectory() const;

	void setFilter(const QString &filter);
	const QString &getFilter() const;

	void setIsDir(bool isDir);
	bool getIsDir() const;

	void setIsSave(bool isSave);
	bool getIsSave() const;

// reimp
	virtual QVariant getVariantValue() const;
	virtual void displayValue();

	virtual QWidget *createEditor() const;
	virtual void valueToEditor();
	virtual void valueFromEditor();

protected:
	mutable QString m_value;
	mutable QLineEdit *m_editor;
	QString m_defaultValue;
	QString m_caption, m_directory, m_filter;
	bool m_isDir, m_isSave;
	QString m_btn_text, m_btn_icon;

	class ButtonsEditor : public TPropertyButton<QLineEdit>
	{
	public:
		ButtonsEditor(QLineEdit *editor, FileDialogProperty *property);
	protected:
		virtual void onButtonClicked();
		virtual void onButtonDefaultClicked();
		FileDialogProperty *m_property;
	};
};

#endif // FILEDIALOGPROPERTY_H
