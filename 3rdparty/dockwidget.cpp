#include "dockwidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QMouseEvent>

DockWidget::DockWidget(QWidget *parent, const QString &title)
	: QDockWidget(parent)
{
	m_autoHide = true;
	m_titlebarVisible = true;

	QDockWidget::setWindowTitle(title);
	setMouseTracking(true);

	QStyleOptionDockWidget opt;
	initStyleOption(&opt);
	m_titleBar = new TitleBarWidget(this, opt);
	m_titleBar->setTitle(title);
	setTitleBarWidget(m_titleBar);

	QAbstractButton *origFloatButton = findChild<QAbstractButton *>(QLatin1String("qt_dockwidget_floatbutton"));
	connect(m_titleBar->m_floatButton, SIGNAL(clicked()), origFloatButton, SLOT(click()));

	QAbstractButton *origCloseButton = findChild<QAbstractButton *>(QLatin1String("qt_dockwidget_closebutton"));
	connect(m_titleBar->m_closeButton, SIGNAL(clicked()), origCloseButton, SLOT(click()));
}

bool DockWidget::eventFilter(QObject *, QEvent *event)
{
	if (!ignoreAutoHide() && event->type() == QEvent::MouseMove && autoHideTitleBars())
	{
		QMouseEvent *me = static_cast<QMouseEvent *>(event);
		int y = me->pos().y();
		int x = me->pos().x();
		int h = qMin(10, m_titleBar->m_floatButton->height());
		if (widget() && 0 <= x && x < widget()->width() && 0 <= y && y <= h)
		{
			m_titleBar->setActive(true);
			m_startPos = mapToGlobal(me->pos());
		}
	}

	return false;
}

void DockWidget::enterEvent(QEvent *event)
{
	if (!ignoreAutoHide())
		QApplication::instance()->installEventFilter(this);
	QDockWidget::enterEvent(event);
}

void DockWidget::leaveEvent(QEvent *event)
{
	if (!ignoreAutoHide())
	{
		if (autoHideTitleBars())
			m_titleBar->setActive(false);

		QApplication::instance()->removeEventFilter(this);
	}
	QDockWidget::leaveEvent(event);
}

void DockWidget::setWindowTitle(const QString &title)
{
	m_titleBar->setTitle(title);
	QDockWidget::setWindowTitle(title);
}

void DockWidget::setAutoHideTitleBars(bool autoHide)
{
	m_autoHide = autoHide;

	if (m_autoHide && !ignoreAutoHide())
		m_titleBar->setActive(false);

	if ((!m_autoHide && visibleTitleBars()) || ignoreAutoHide())
		m_titleBar->setActive(visibleTitleBars());
}

bool DockWidget::autoHideTitleBars()
{
	return m_autoHide;
}

void DockWidget::setIgnoreAutoHide(bool ignore)
{
	m_ignoreAutoHide = ignore;

	if (m_ignoreAutoHide)
		m_titleBar->setActive(visibleTitleBars());
}

bool DockWidget::ignoreAutoHide()
{
	return m_ignoreAutoHide;
}

void DockWidget::setVisibleTitleBars(bool visible)
{
	m_titleBar->setActive(visible);
	m_titlebarVisible = visible;
}

bool DockWidget::visibleTitleBars()
{
	return m_titlebarVisible;
}

//-----------------------------------------
DockWidgetTitleButton::DockWidgetTitleButton(QWidget *parent)
	: QAbstractButton(parent)
{
	setFocusPolicy(Qt::NoFocus);
}

QSize DockWidgetTitleButton::sizeHint() const
{
	ensurePolished();

	int size = 2*style()->pixelMetric(QStyle::PM_DockWidgetTitleBarButtonMargin, 0, this);
	if (!icon().isNull()) {
		int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
		QSize sz = icon().actualSize(QSize(iconSize, iconSize));
		size += qMax(sz.width(), sz.height());
	}

	return QSize(size, size);
}

QSize DockWidgetTitleButton::minimumSizeHint() const
{
	return sizeHint();
}

void DockWidgetTitleButton::enterEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::enterEvent(event);
}

void DockWidgetTitleButton::leaveEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::leaveEvent(event);
}

void DockWidgetTitleButton::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	QStyleOptionToolButton opt;
	opt.init(this);
	opt.state |= QStyle::State_AutoRaise;
	opt.icon = icon();
	opt.subControls = 0;
	opt.activeSubControls = 0;
	opt.features = QStyleOptionToolButton::None;
	opt.arrowType = Qt::NoArrow;
	int size = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this);
	opt.iconSize = QSize(size, size);

#ifdef Q_WS_MAC
	if (!qobject_cast<QMacStyle *>(style()))
#endif
	{
		if (isEnabled() && underMouse() && !isChecked() && !isDown())
			opt.state |= QStyle::State_Raised;
		if (isChecked())
			opt.state |= QStyle::State_On;
		if (isDown())
			opt.state |= QStyle::State_Sunken;
		style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
	}

	style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

//-----------------------------------------
TitleBarWidget::TitleBarWidget(DockWidget *parent, const QStyleOptionDockWidget &opt)
	: QWidget(parent), q(parent), m_active(true)
{
	m_titleLabel = new QLabel(this);

	m_floatButton = new DockWidgetTitleButton(this);
	m_floatButton->setIcon(q->style()->standardIcon(QStyle::SP_TitleBarNormalButton, &opt, q));

	m_closeButton = new DockWidgetTitleButton(this);
	m_closeButton->setIcon(q->style()->standardIcon(QStyle::SP_TitleBarCloseButton, &opt, q));

#ifndef QT_NO_ACCESSIBILITY
	m_floatButton->setAccessibleName(QDockWidget::tr("Float"));
	m_floatButton->setAccessibleDescription(QDockWidget::tr("Undocks and re-attaches the dock widget"));
	m_closeButton->setAccessibleName(QDockWidget::tr("Close"));
	m_closeButton->setAccessibleDescription(QDockWidget::tr("Closes the dock widget"));
#endif

	setActive(false);

	const int minWidth = 10;
	const int maxWidth = 10000;
	const int inactiveHeight = 0;
	const int activeHeight = m_closeButton->sizeHint().height() + 2;

	m_minimumInactiveSize = QSize(minWidth, inactiveHeight);
	m_maximumInactiveSize = QSize(maxWidth, inactiveHeight);
	m_minimumActiveSize   = QSize(minWidth, activeHeight);
	m_maximumActiveSize   = QSize(maxWidth, activeHeight);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 0, 0, 0);
	layout->addWidget(m_titleLabel);
	layout->addStretch();
	layout->addWidget(m_floatButton);
	layout->addWidget(m_closeButton);
	setLayout(layout);

	setProperty("managed_titlebar", 1);
}

void TitleBarWidget::enterEvent(QEvent *event)
{
	setActive(true);
	QWidget::enterEvent(event);
}

void TitleBarWidget::setTitle(const QString &title)
{
	m_titleLabel->setText(title);
}

void TitleBarWidget::setActive(bool on)
{
	m_active = on;
	updateChildren();
}

void TitleBarWidget::updateChildren()
{
	bool clickable = isClickable();
	m_titleLabel->setVisible(clickable);
	m_floatButton->setVisible(clickable);
	m_closeButton->setVisible(clickable);
}

bool TitleBarWidget::isClickable() const
{
	return m_active;
}

QSize TitleBarWidget::sizeHint() const
{
	ensurePolished();
	return isClickable() ? m_maximumActiveSize : m_maximumInactiveSize;
}

QSize TitleBarWidget::minimumSizeHint() const
{
	ensurePolished();
	return isClickable() ? m_minimumActiveSize : m_minimumInactiveSize;
}
