#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>
#include <QAbstractButton>
#include <QMenu>
#include <QLabel>

class TitleBarWidget;

class DockWidget : public QDockWidget
{
public:
	DockWidget(QWidget *parent, const QString &title = "");

	void setWindowTitle(const QString &title);

	void setAutoHideTitleBars(bool autoHide);
	bool autoHideTitleBars();

	void setIgnoreAutoHide(bool ignore);
	bool ignoreAutoHide();

	void setVisibleTitleBars(bool visible);
	bool visibleTitleBars();

protected:
	bool eventFilter(QObject *, QEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	QPoint m_startPos;
	TitleBarWidget *m_titleBar;
	bool m_autoHide, m_titlebarVisible, m_ignoreAutoHide;

};

class DockWidgetTitleButton : public QAbstractButton
{
public:
	DockWidgetTitleButton(QWidget *parent);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void paintEvent(QPaintEvent *event);

};

class TitleBarWidget : public QWidget
{

public:
	TitleBarWidget(DockWidget *parent, const QStyleOptionDockWidget &opt);

	void setTitle(const QString &title);
	void setActive(bool on);
	void updateChildren();
	bool isClickable() const;
	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	void enterEvent(QEvent *event);

private:
	DockWidget *q;
	bool m_active;
	QSize m_minimumActiveSize;
	QSize m_maximumActiveSize;
	QSize m_minimumInactiveSize;
	QSize m_maximumInactiveSize;

public:
	QLabel *m_titleLabel;
	DockWidgetTitleButton *m_floatButton;
	DockWidgetTitleButton *m_closeButton;

};

#endif // DOCKWIDGET_H
