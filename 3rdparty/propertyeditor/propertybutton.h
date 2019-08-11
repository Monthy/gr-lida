#ifndef PROPERTYBUTTON_H
#define PROPERTYBUTTON_H

#include <QToolButton>

class PropertyButton : public QWidget
{
	Q_OBJECT
public:
	explicit PropertyButton(QWidget *hostedEditor, QWidget *parent = 0);
	virtual ~PropertyButton();

	void setBtnText(const QString &text);
	void setBtnIcon(const QString &icon);
	void setBtnIconSize(const QSize &size);

	void setBtnDefText(const QString &text);
	void setBtnDefIcon(const QString &icon);
	void setBtnDefIconSize(const QSize &size);

public Q_SLOTS:
	void setBtnVisible(bool visible);
	void setBtnDefVisible(bool visible);

protected Q_SLOTS:
// reimp
	virtual void onButtonClicked() {}
	virtual void onButtonDefaultClicked() = 0;
	virtual void onEditorClicked() {}

protected:
	virtual bool event(QEvent *e);
	virtual void showEvent(QShowEvent *e);

	QWidget *m_hostedEditor;
	QToolButton *m_button;
	QToolButton *m_button_def;
};

template<class EditorClass>
class TPropertyButton : public PropertyButton
{
public:
	TPropertyButton(EditorClass *hostedEditor, QWidget *parent = 0):
		PropertyButton(hostedEditor, parent) {}

protected:
	EditorClass *getEditor() const
	{
		return dynamic_cast<EditorClass *>(m_hostedEditor);
	}
	EditorClass *getButton() { return m_button; }
	EditorClass *getButtonDef() { return m_button_def; }
};

#endif // PROPERTYBUTTON_H
