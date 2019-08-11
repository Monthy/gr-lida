/*
  PictureFlow - animated image show widget
  http://pictureflow.googlecode.com

  Copyright (C) 2009 Ariya Hidayat (ariya@kde.org)
  Copyright (C) 2008 Ariya Hidayat (ariya@kde.org)
  Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef PICTUREFLOW_H
#define PICTUREFLOW_H

#include <QWidget>

class PictureFlowPrivate;

/*!
  Class PictureFlow implements an image show widget with animation effect
  like Apple's CoverFlow (in iTunes and iPod). Images are arranged in form
  of slides, one main slide is shown at the center with few slides on
  the left and right sides of the center slide. When the next or previous
  slide is brought to the front, the whole slides flow to the right or
  the right with smooth animation effect; until the new slide is finally
  placed at the center.

 */
class PictureFlow : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
	Q_PROPERTY(QSize slideSize READ slideSize WRITE setSlideSize)
	Q_PROPERTY(int slideCount READ slideCount)
	Q_PROPERTY(int centerIndex READ centerIndex WRITE setCenterIndex)

public:
	enum FlowType {
		CoverFlowLike   = 0,
		Strip		   = 1,
		StripOverlapped = 2
	};

	enum FlowStatus {
		Unread = 0,
		Read   = 1,
		Opened = 2
	};

	enum ReflectionEffect {
		NoReflection,
		PlainReflection,
		BlurredReflection
	};

	/*!
	  Creates a new PictureFlow widget.
	*/
	PictureFlow(QWidget *parent = 0, FlowType flowType = CoverFlowLike);

	/*!
	  Destroys the widget.
	*/
	~PictureFlow();

	/*!
	  Returns the background color.
	*/
	QColor backgroundColor() const;

	/*!
	  Sets the background color. By default it is black.
	*/
	void setBackgroundColor(const QColor &c);

	/*!
	  Returns the font.
	*/
	QFont getPicFlowFont() const;

	/*!
	  Sets the font. By default it is Arial 14.
	*/
	void setPicFlowFont(const QFont& font);
	void setPicFlowFont(QString f_family, int f_size, bool f_bold = false, bool f_italic = false);

	/*!
	  Returns the font color.
	*/
	QColor getFontColor() const;

	/*!
	  Sets the font color. By default it is white.
	*/
	void setFontColor(const QColor& c);

	/*!
	  Returns the dimension of each slide (in pixels).
	*/
	QSize slideSize() const;

	/*!
	  Sets the dimension of each slide (in pixels).
	*/
	void setSlideSize(QSize size);

	void setSlideFixSize(bool fix_size);

	/*!
	  Returns the total number of slides.
	*/
	int slideCount() const;

	/*!
	  Returns QImage of specified slide.
	*/
	QImage slide(int index) const;

	/*!
	  Returns the index of slide currently shown in the middle of the viewport.
	*/
	int centerIndex() const;

	/*!
	  Returns the effect applied to the reflection.
	*/
	ReflectionEffect reflectionEffect() const;

	/*!
	  Sets the effect applied to the reflection. The default is PlainReflection.
	*/
	void setReflectionEffect(ReflectionEffect effect);

public slots:

	/*!
	  Adds a new slide.
	*/
	void addSlide(const QImage &image, QString caption = "");

	/*!
	  Adds a new slide.
	*/
	void addSlide(const QPixmap &pixmap, QString caption = "");

	/*!
	  Removes an existing slide.
	*/
	void removeSlide(int index);

	/*!
	  Sets an image for specified slide. If the slide already exists,
	  it will be replaced.
	*/
	void setSlide(int index, const QImage &image, QString caption = "");

	/*!
	  Sets a pixmap for specified slide. If the slide already exists,
	  it will be replaced.
	*/
	void setSlide(int index, const QPixmap &pixmap, QString caption = "");

	/*!
	  Sets slide to be shown in the middle of the viewport. No animation
	  effect will be produced, unlike using showSlide.
	*/
	void setCenterIndex(int index);

	/*!
	  Clears all slides.
	*/
	void clear();

	/*!
	  Shows previous slide using animation effect.
	*/
	void showPrevious();

	/*!
	  Shows next slide using animation effect.
	*/
	void showNext();

	/*!
	  Go to specified slide using animation effect.
	*/
	void showSlide(int index);

	/*!
	  Rerender the widget. Normally this function will be automatically invoked
	  whenever necessary, e.g. during the transition animation.
	*/
	void render();

	/*!
	  Schedules a rendering update. Unlike render(), this function does not cause
	  immediate rendering.
	*/
	void triggerRender();

	void setFlowType(FlowType flowType);

	void setShowLinesPos(bool enable = false);

	void setShowTitle(bool enable);
	void setSlideTitle(QString title);
	void setSlideTitle(QString title, int position);
	void setSlideTitlePos(int position = Qt::AlignLeft | Qt::AlignTop);
	void setSlideTitleMargin(QPoint margin);

	void setShowTitleIcon(bool enable);
	void setSlideTitleIcon(QString titleIcon);
	void setSlideTitleIcon(QPixmap titleIcon);
	void setSlideTitleIcon(QImage titleIcon);
	void setSlideTitleIconPos(int position);
	void setSlideTitleIconMargin(QPoint margin);

	void setShowCaption(bool enable);
	void setSlideCaption(int index, QString caption);
	void setSlideCaptionPos(int position = Qt::AlignHCenter | Qt::AlignBottom);
	void setSlideCaptionMargin(QPoint margin);

	void setShowNumber(bool enable);
	void setSlideNumberPos(int position = Qt::AlignRight | Qt::AlignTop);
	void setSlideNumberMargin(QPoint margin);
	void setSlideNumberFormat(QString format = "%1/%2");

	void setShowIconExtra(bool enable);
	void setSlideIconExtra(QString iconExtra);
	void setSlideIconExtra(QPixmap iconExtra);
	void setSlideIconExtra(QImage iconExtra);
	void setSlideIconExtraPos(int position = Qt::AlignRight | Qt::AlignTop);
	void setSlideIconExtraMargin(QPoint margin);

	void setShowTriangle(bool enable);

	void setSlideMargin(QPoint margin);

	void setMarkImage(FlowStatus flowStatus, const QImage &image);

	void markSlide(int index, FlowStatus flowStatus = Read);

	void updateMarks();

	void unmarkSlide(int index);

	void setShowMarks(bool enable);

	void resortCovers(QList<int> newOrder);

signals:
	void centerIndexChanged(int index);
	void centerIndexChangedSilent(int index);
	void finishedAnimation();

protected:
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event, int slideWidth);
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateAnimation();

private:
	PictureFlowPrivate *d;
	int framesSkip;

};

#endif // PICTUREFLOW_H
