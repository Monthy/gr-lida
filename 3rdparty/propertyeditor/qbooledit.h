#ifndef QBOOLEDIT_H
#define QBOOLEDIT_H

#include <QCheckBox>

class QBoolEdit : public QWidget {
	Q_OBJECT
public:
	QBoolEdit(QWidget *parent = 0);

	void setText(const QString &text);
	void setText(const QString &textTrue, const QString &textFalse);

	QString text() const { return m_textTrue;}
	QString textTrue() const { return m_textTrue;}
	QString textFalse() const { return m_textFalse;}

	void setIcon(const QIcon &icon);
	void setBgColor(const QColor &color);
	bool isChecked() const;

	Qt::CheckState checkState() const;
	void setCheckState(Qt::CheckState state);

	bool blockCheckBoxSignals(bool block);

public Q_SLOTS:
	void setIconSize(const QSize &size);
	void setChecked(bool checked);

Q_SIGNALS:
	void toggled(bool checked);

protected:
	void mouseReleaseEvent(QMouseEvent *event);

private Q_SLOTS:
	void onToggled(bool checked);

private:
	QCheckBox *m_checkBox;
	QString m_textTrue, m_textFalse;
};

#endif // QBOOLEDIT_H
