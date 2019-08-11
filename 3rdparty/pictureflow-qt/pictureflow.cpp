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

#include "pictureflow.h"

// detect Qt version
#if QT_VERSION < 0x040300
#error PictureFlow widgets need Qt 4.3 or later
#endif

#include <QApplication>
#include <QCache>
#include <QHash>
#include <QImage>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QVector>
#include <QWidget>

// for fixed-point arithmetic, we need minimum 32-bit long
// long long (64-bit) might be useful for multiplication and division
typedef long PFreal;
#define PFREAL_SHIFT 10
#define PFREAL_ONE (1 << PFREAL_SHIFT)

#define IANGLE_MAX 1024
#define IANGLE_MASK 1023

inline PFreal fmul(PFreal a, PFreal b)
{
	return ((long long)(a)) * ((long long)(b)) >> PFREAL_SHIFT;
}

inline PFreal fdiv(PFreal num, PFreal den)
{
	long long p = (long long)(num) << (PFREAL_SHIFT * 2);
	long long q = p / (long long)den;
	long long r = q >> PFREAL_SHIFT;

	return r;
}

inline PFreal fsin(int iangle)
{
	// warning: regenerate the table if IANGLE_MAX and PFREAL_SHIFT are changed!
	static const PFreal tab[] = {
            3,    103,    202,    300,    394,    485,    571,    652,
          726,    793,    853,    904,    947,    980,   1004,   1019,
         1023,   1018,   1003,    978,    944,    901,    849,    789,
          721,    647,    566,    479,    388,    294,    196,     97,
           -4,   -104,   -203,   -301,   -395,   -486,   -572,   -653,
         -727,   -794,   -854,   -905,   -948,   -981,  -1005,  -1020,
        -1024,  -1019,  -1004,   -979,   -945,   -902,   -850,   -790,
         -722,   -648,   -567,   -480,   -389,   -295,   -197,    -98,
         3
	};

	while (iangle < 0)
		iangle += IANGLE_MAX;
	iangle &= IANGLE_MASK;

	int i = (iangle >> 4);
	PFreal p = tab[i];
	PFreal q = tab[(i + 1)];
	PFreal g = (q - p);
	return p + g * (iangle - i * 16) / 16;
}

inline PFreal fcos(int iangle)
{
	return fsin(iangle + (IANGLE_MAX >> 2));
}

/* ----------------------------------------------------------

PictureFlowState stores the state of all slides, i.e. all the necessary
information to be able to render them.

PictureFlowAnimator is responsible to move the slides during the
transition between slides, to achieve the effect similar to Cover Flow,
by changing the state.

PictureFlowSoftwareRenderer (or PictureFlowOpenGLRenderer) is
the actual 3-d renderer. It should render all slides given the state
(an instance of PictureFlowState).

Instances of all the above three classes are stored in
PictureFlowPrivate.

------------------------------------------------------- */

struct SlideInfo {
	int slideIndex;
	int angle;
	PFreal cx;
	PFreal cy;
	int blend;
};

struct stSlideImage {
	QImage* imagen;
	QString caption;
	PictureFlow::FlowStatus mark;
};

class PictureFlowState
{
public:
	PictureFlowState(int angle = 50, float spacingRatio = 0);
	~PictureFlowState();

	void reposition();
	void reset();

	QRgb backgroundColor;
	QRgb fontColor;
	QFont slideFont;

	QString numberFormat;
	QString slideTitle;
	QImage slideTitleIcon;
	QImage slideIconExtra;
	QImage markUnread;
	QImage markRead;
	QImage markOpened;

	int slideWidth;
	int slideHeight;
	bool slideFixSize;
	PictureFlow::ReflectionEffect reflectionEffect;
	QVector<stSlideImage> slideImages;

	int posTitle;
	int posTitleIcon;
	int posCaption;
	int posNumber;
	int posIconExtra;

	QPoint marginTitle;
	QPoint marginTitleIcon;
	QPoint marginCaption;
	QPoint marginNumber;
	QPoint marginIconExtra;
	QPoint margin;

	bool showLinesPos;
	bool showTitle;
	bool showTitleIcon;
	bool showCaption;
	bool showNumber;
	bool showIconExtra;
	bool showTriangle;
	bool showMarks;

	int angle;
	int rawAngle;
	int spacing;
	float spacingRatio;
	PFreal offsetX;
	PFreal offsetY;

	SlideInfo centerSlide;
	QVector<SlideInfo> leftSlides;
	QVector<SlideInfo> rightSlides;
	int centerIndex;
};

class PictureFlowAnimator
{
public:
	PictureFlowAnimator();
	PictureFlowState *state;

	void start(int slide);
	void stop(int slide);
	void update();

	int target;
	int step;
	int frame;
	QTimer animateTimer;
	bool animating;
};

class PictureFlowAbstractRenderer
{
public:
	PictureFlowAbstractRenderer() : state(0), dirty(false), widget(0) {}
	virtual ~PictureFlowAbstractRenderer() {}

	PictureFlowState *state;
	bool dirty;
	QWidget *widget;

	virtual void init() = 0;
	virtual void paint() = 0;
};

class PictureFlowSoftwareRenderer : public PictureFlowAbstractRenderer
{
public:
	PictureFlowSoftwareRenderer();
	~PictureFlowSoftwareRenderer();

	virtual void init();
	virtual void paint();

private:
	QSize size;
	QRgb bgcolor;
	int effect;
	QImage buffer;
	QVector<PFreal> rays;
	QImage *blankSurface;

	QCache<int, QImage> surfaceCache;
	QHash<int, QImage *> imageHash;

	void render();
	void renderSlides();
	QRect renderSlide(const SlideInfo &slide, int col1 = -1, int col2 = -1);
	QImage *surface(int slideIndex);
	QPoint getPosImage(QImage image, int posImage, int m_x, int m_y);
};

// ------------- PictureFlowState ---------------------------------------

PictureFlowState::PictureFlowState(int a, float sr) :
	backgroundColor(0), slideWidth(150), slideHeight(200),
	reflectionEffect(PictureFlow::BlurredReflection), rawAngle(a), spacingRatio(sr), centerIndex(0)
{
	fontColor = QColor(255, 255, 255).rgb();
	numberFormat = "%1/%2";
	centerIndex = 0;
	slideFont.setFamily("Arial");
	slideFont.setPointSize(8);
	slideFixSize = false;
}

PictureFlowState::~PictureFlowState()
{
	for (int i = 0; i < (int)slideImages.count(); i++)
		delete slideImages[i].imagen;
}

// readjust the settings, call this when slide dimension is changed
void PictureFlowState::reposition()
{
  //angle = 70 * IANGLE_MAX / 360; // approx. 70 degrees tilted
	angle = rawAngle * IANGLE_MAX / 360;
	offsetX = slideWidth / 2 * (PFREAL_ONE - fcos(angle));
	offsetY = slideWidth / 2 * fsin(angle);
	offsetX += slideWidth * PFREAL_ONE;
	offsetY += slideWidth * PFREAL_ONE / 3;// 4
	if (rawAngle < 45)
		offsetX += offsetX / 4;
	if (angle > 0)
		spacing = slideWidth * 0.35;
	else
		spacing = slideWidth * spacingRatio + slideWidth * (spacingRatio ? 0.10 : 0.2);
}

// adjust slides so that they are in "steady state" position
void PictureFlowState::reset()
{
	centerSlide.angle = 0;
	centerSlide.cx = 0;
	centerSlide.cy = 0;
	centerSlide.slideIndex = centerIndex;
	centerSlide.blend = 256;

	if (angle == 0 && spacingRatio)
		leftSlides.resize(4);
	else
		leftSlides.resize(6);

	for (int i = 0; i < (int)leftSlides.count(); i++)
	{
		SlideInfo &si = leftSlides[i];
		si.angle = angle;
		si.cx = -(offsetX + spacing * i * PFREAL_ONE);
		si.cy = offsetY;
		si.slideIndex = centerIndex - 1 - i;
		si.blend = 200;
		if (i == (int)leftSlides.count() - 2)
			si.blend = 128;
		if (i == (int)leftSlides.count() - 1)
			si.blend = 0;
	}

	if (angle == 0 && spacingRatio)
		rightSlides.resize(4);
	else
		rightSlides.resize(6);

	for (int i = 0; i < (int)rightSlides.count(); i++)
	{
		SlideInfo &si = rightSlides[i];
		si.angle = -angle;
		si.cx = offsetX + spacing * i * PFREAL_ONE;
		si.cy = offsetY;
		si.slideIndex = centerIndex + 1 + i;
		si.blend = 200;
		if (i == (int)rightSlides.count() - 2)
			si.blend = 128;
		if (i == (int)rightSlides.count() - 1)
			si.blend = 0;
	}
}

// ------------- PictureFlowAnimator  ---------------------------------------

PictureFlowAnimator::PictureFlowAnimator() :
	state(0), target(0), step(0), frame(0), animating(false)
{
}

void PictureFlowAnimator::start(int slide)
{
	target = slide;
	if (!animateTimer.isActive() && state)
	{
		step = (target < state->centerSlide.slideIndex) ? -1 : 1;
		animateTimer.setSingleShot(true);
		animateTimer.start(30); //TODO comprobar rendimiento, originalmente era 30
		animating = true;
	}
}

void PictureFlowAnimator::stop(int slide)
{
	step = 0;
	target = slide;
	frame = slide << 16;
	animateTimer.stop();
	animating = false;
}

void PictureFlowAnimator::update()
{
  /*if (!animateTimer.isActive())
		return;*/
	if (step == 0)
		return;
	if (!state)
		return;

	int speed = 16384 / 4; //TODO comprobar rendimiento, originalmente era / 4

#if 1
	// deaccelerate when approaching the target
	const int max = 2 * 65536; //TODO cambiado de 2 * a 4 * comprobar rendimiento

	int fi = frame;
	fi -= (target << 16);
	if (fi < 0)
		fi = -fi;
	fi = qMin(fi, max);

	int ia = IANGLE_MAX * (fi - max / 2) / (max * 2);
	speed = 512 + 16384 * (PFREAL_ONE + fsin(ia)) / PFREAL_ONE;
#endif

	frame += speed * step;

	int index = frame >> 16;
	int pos = frame & 0xffff;
	int neg = 65536 - pos;
	int tick = (step < 0) ? neg : pos;
	PFreal ftick = (tick * PFREAL_ONE) >> 16;

	if (step < 0)
		index++;

	if (state->centerIndex != index)
	{
		state->centerIndex = index;
		frame = index << 16;
		state->centerSlide.slideIndex = state->centerIndex;
		for (int i = 0; i < (int)state->leftSlides.count(); i++)
			state->leftSlides[i].slideIndex = state->centerIndex - 1 - i;
		for (int i = 0; i < (int)state->rightSlides.count(); i++)
			state->rightSlides[i].slideIndex = state->centerIndex + 1 + i;
	}

	state->centerSlide.angle = (step * tick * state->angle) >> 16;
	state->centerSlide.cx = -step * fmul(state->offsetX, ftick);
	state->centerSlide.cy = fmul(state->offsetY, ftick);

	if (state->centerIndex == target)
	{
		stop(target);
		state->reset();
		return;
	}

	const int countLeftSlides = state->leftSlides.count();
	for (int i = 0; i < countLeftSlides; i++)
	{
		SlideInfo &si = state->leftSlides[i];
		si.angle = state->angle;
		si.cx = -(state->offsetX + state->spacing * i * PFREAL_ONE + step * state->spacing * ftick);
		si.cy = state->offsetY;
	}

	const int countRightSlides = state->rightSlides.count();
	for (int i = 0; i < countRightSlides; i++)
	{
		SlideInfo &si = state->rightSlides[i];
		si.angle = -state->angle;
		si.cx = state->offsetX + state->spacing * i * PFREAL_ONE - step * state->spacing * ftick;
		si.cy = state->offsetY;
	}

	if (step > 0)
	{
		PFreal ftick = (neg * PFREAL_ONE) >> 16;
		state->rightSlides[0].angle = -(neg * state->angle) >> 16;
		state->rightSlides[0].cx = fmul(state->offsetX, ftick);
		state->rightSlides[0].cy = fmul(state->offsetY, ftick);
	} else {
		PFreal ftick = (pos * PFREAL_ONE) >> 16;
		state->leftSlides[0].angle = (pos * state->angle) >> 16;
		state->leftSlides[0].cx = -fmul(state->offsetX, ftick);
		state->leftSlides[0].cy = fmul(state->offsetY, ftick);
	}

	// must change direction ?
	if (target < index)
	{
		if (step > 0)
			step = -1;
	}

	if (target > index)
	{
		if (step < 0)
			step = 1;
	}

	// the first and last slide must fade in/fade out
	int nleft  = state->leftSlides.count();
	int nright = state->rightSlides.count();
	int fade   = pos / 256;

	for (int index = 0; index < nleft; index++)
	{
		int blend = 200;
		if (index == nleft - 1)
			blend = (step > 0) ? 0 : 128 - fade / 2;
		if (index == nleft - 2)
			blend = (step > 0) ? 128 - fade / 2 : 200 - (0.5625 * fade / 2);
		if (index == nleft - 3)
			blend = (step > 0) ? 200 - (0.5625 * fade / 2) : 200;
		if (index == 0)
			blend = (step > 0) ? 200 : 200 + 56 - (0.4375 * fade / 2);
		state->leftSlides[index].blend = blend;
	}

	for (int index = 0; index < nright; index++)
	{
		int blend = (index < nright - 2) ? 200 : 128;
		if (index == nright - 1)
			blend = (step > 0) ? fade / 2 : 0;
		if (index == nright - 2)
			blend = (step > 0) ? 128 + (0.5625 * fade / 2) : (0.5625 * fade / 2);
		if (index == nright - 3)
			blend = (step > 0) ? 200 : 128 + (0.5625 * fade / 2);
		if (index == 0)
			blend = (step > 0) ? 200 + (0.4375 * fade / 2) : 200;
		state->rightSlides[index].blend = blend;
	}

	state->centerSlide.blend = (step > 0) ? 256 - (0.4375 * fade / 2) : 200 + (0.4375 * fade / 2);
}

// ------------- PictureFlowSoftwareRenderer ---------------------------------------

PictureFlowSoftwareRenderer::PictureFlowSoftwareRenderer() :
	PictureFlowAbstractRenderer(), size(0, 0), bgcolor(0), effect(-1), blankSurface(0)
{
}

PictureFlowSoftwareRenderer::~PictureFlowSoftwareRenderer()
{
	surfaceCache.clear();
	buffer = QImage();
	delete blankSurface;
}

void PictureFlowSoftwareRenderer::paint()
{
	if (!widget)
		return;

	if (widget->size() != size)
		init();

	if (state->backgroundColor != bgcolor)
	{
		bgcolor = state->backgroundColor;
		surfaceCache.clear();
	}

	if ((int)(state->reflectionEffect) != effect)
	{
		effect = (int)state->reflectionEffect;
		surfaceCache.clear();
	}

	if (dirty)
		render();

	QPainter painter(widget);
	painter.drawImage(QPoint(0, 0), buffer);
}

void PictureFlowSoftwareRenderer::init()
{
	if (!widget)
		return;

	surfaceCache.clear();
	blankSurface = 0;

	size = widget->size();
	int ww = size.width();
	int wh = size.height();
	int w = (ww + 1) / 2;
	int h = (wh + 1) / 2;
	if (h < 10) // TODO a partir de qué h es seguro??
		return;

	buffer = QImage(ww, wh, QImage::Format_RGB32);
	buffer.fill(bgcolor);

	rays.resize(w * 2);
	for (int i = 0; i < w; i++)
	{
		PFreal gg = ((PFREAL_ONE >> 1) + i * PFREAL_ONE) / (2 * h);
		rays[w - i - 1] = -gg;
		rays[w + i] = gg;
	}

	dirty = true;
}

// TODO: optimize this with lookup tables
static QRgb blendColor(QRgb c1, QRgb c2, int blend)
{
	int r = qRed(c1) * blend / 256 + qRed(c2) * (256 - blend) / 256;
	int g = qGreen(c1) * blend / 256 + qGreen(c2) * (256 - blend) / 256;
	int b = qBlue(c1) * blend / 256 + qBlue(c2) * (256 - blend) / 256;
	return qRgb(r, g, b);
}

static QImage *prepareSurface(const QImage *slideImage, int w, int h, QRgb bgcolor, PictureFlow::ReflectionEffect reflectionEffect, bool fix_size)
{
	int psw, psh;
	if (fix_size)
	{
		psw = w;
		psh = h;
	} else {
		int iw, ih;
		iw = slideImage->width();
		ih = slideImage->height();
		if (iw > ih)
		{
			psw = w;
			psh = w * (1.0 * ih / iw);
		} else {
			int h1 = h;
			psw = h1 * (1.0 * iw / ih);
			psh = h1;

			while (psw > w)
			{
				h1 -= 2;
				psw = h1 * (1.0 * iw / ih);
				psh = h1;
			}
		}
	}
	w = psw;

	Qt::TransformationMode mode = Qt::SmoothTransformation;
	QImage img = slideImage->scaled(psw, psh, Qt::IgnoreAspectRatio, mode);

	// slightly larger, to accomodate for the reflection
	int hs = h * 2;
	int hofs = h / 3;

	// offscreen buffer: black is sweet
	QImage *result = new QImage(hs, w, QImage::Format_RGB32);
	result->fill(bgcolor);

	// transpose the image, this is to speed-up the rendering
	// because we process one column at a time
	// (and much better and faster to work row-wise, i.e in one scanline)
	int lhof = (h - psh);
  //int lwof = (w - psw) / 2;
	for (int x = 0; x < psw; x++)
	{
		for (int y = 0; y < psh; y++)
			result->setPixel(hofs + y + lhof, x, img.pixel(x, y));
	}

	if (reflectionEffect != PictureFlow::NoReflection)
	{
		// create the reflection
		int ht = fix_size ? hs - h - hofs : hs - (h + hofs); // ver hs - (h - hofs)
		int hte = ht;
		for (int x = 0; x < psw; x++)
		{
			for (int y = 0; y < ht; y++)
			{
				QRgb color;
				if (y < psh)
					color = img.pixel(x, psh - y - 1);
				else
					color = bgcolor;
				result->setPixel(h + hofs + y, x, blendColor(color, bgcolor, 80 * (hte - y) / hte));
			}
		}

		if (reflectionEffect == PictureFlow::BlurredReflection)
		{
			// blur the reflection everything first
			// Based on exponential blur algorithm by Jani Huhtanen
			QRect rect;
			if (fix_size)
				rect.setRect(hs / 2, 0, hs / 2, w);
			else
				rect.setRect(h + hofs, 0, hs / 2, psw);
			rect &= result->rect();

			int r1 = rect.top();
			int r2 = rect.bottom();
			int c1 = rect.left();
			int c2 = rect.right();

			int bpl = result->bytesPerLine();
			int rgba[4];
			unsigned char* p;

			// how many times blur is applied?
			// for low-end system, limit this to only 1 loop
			for (int loop = 0; loop < 2; loop++)
			{
				for (int col = c1; col <= c2; col++)
				{
					p = result->scanLine(r1) + col * 4;
					for (int i = 0; i < 3; i++)
						rgba[i] = p[i] << 4;

					p += bpl;
					for (int j = r1; j < r2; j++, p += bpl)
						for (int i = 0; i < 3; i++)
							p[i] = (rgba[i] += (((p[i] << 4) - rgba[i])) >> 1) >> 4;
				}

				for (int row = r1; row <= r2; row++)
				{
					p = result->scanLine(row) + c1 * 4;
					for (int i = 0; i < 3; i++)
						rgba[i] = p[i] << 4;

					p += 4;
					for (int j = c1; j < c2; j++, p += 4)
						for (int i = 0; i < 3; i++)
							p[i] = (rgba[i] += (((p[i] << 4) - rgba[i])) >> 1) >> 4;
				}

				for (int col = c1; col <= c2; col++)
				{
					p = result->scanLine(r2) + col * 4;
					for (int i = 0; i < 3; i++)
						rgba[i] = p[i] << 4;

					p -= bpl;
					for (int j = r1; j < r2; j++, p -= bpl)
						for (int i = 0; i < 3; i++)
							p[i] = (rgba[i] += (((p[i] << 4) - rgba[i])) >> 1) >> 4;
				}

				for (int row = r1; row <= r2; row++)
				{
					p = result->scanLine(row) + c2 * 4;
					for (int i = 0; i < 3; i++)
						rgba[i] = p[i] << 4;

					p -= 4;
					for (int j = c1; j < c2; j++, p -= 4)
						for (int i = 0; i < 3; i++)
							p[i] = (rgba[i] += (((p[i] << 4) - rgba[i])) >> 1) >> 4;
				}
			}

			// overdraw to leave only the reflection blurred (but not the actual image)
			/*for (int x = 0; x < psw; x++) // w --> psw
			{
				for (int y = 0; y < ht; y++) // h --> ht
					result->setPixel(hofs + y, x, img.pixel(x, y));
			}*/
		}
	}

	return result;
}

QImage *PictureFlowSoftwareRenderer::surface(int slideIndex)
{
	if (!state)
		return 0;
	if (slideIndex < 0)
		return 0;
	if (slideIndex >= (int)state->slideImages.count())
		return 0;

	int key = slideIndex;

	QImage *img = state->slideImages.at(slideIndex).imagen;

	bool empty = img ? img->isNull() : true;
	if (empty)
	{
		surfaceCache.remove(key);
		imageHash.remove(slideIndex);
		if (!blankSurface)
		{
			int sw = state->slideWidth;
			int sh = state->slideHeight;

			QImage img = QImage(sw, sh, QImage::Format_RGB32);

			QPainter painter(&img);
			QPoint p1(sw * 4 / 10, 0);
			QPoint p2(sw * 6 / 10, sh);
			QLinearGradient linearGrad(p1, p2);
			linearGrad.setColorAt(0, Qt::black);
			linearGrad.setColorAt(1, Qt::white);
			painter.setBrush(linearGrad);
			painter.fillRect(0, 0, sw, sh, QBrush(linearGrad));

		/*  painter.setPen(QPen(QColor(64, 64, 64), 4));
			painter.setBrush(QBrush());
			painter.drawRect(2, 2, sw - 3, sh - 3);*/

			painter.end();

			blankSurface = prepareSurface(&img, sw, sh, bgcolor, state->reflectionEffect, state->slideFixSize);
		}
		return blankSurface;
	}

	bool exist = imageHash.contains(slideIndex);
	if (exist)
	{
		if (img == imageHash.find(slideIndex).value())
		{
			if (surfaceCache.contains(key))
				return surfaceCache[key];
		}
	}

	QImage *sr = prepareSurface(img, state->slideWidth, state->slideHeight, bgcolor, state->reflectionEffect, state->slideFixSize);

	// check if this slide must be marked
	if (state->showMarks)
	{
		if (state->slideImages[slideIndex].mark)
		{
			QPainter painter(sr);
			painter.setPen(QColor(255, 0, 0).rgb());
			int sh = sr->height();
			int jInit = sh * 4 / 5;
			int iInit = state->slideHeight + state->slideHeight / 3;
		  /*for (int j = jInit; j < sh; j ++)
			{
				for (int i = iInit - (j - jInit); i < iInit; i++)
				{
					painter.drawPoint(i, j);
				}
			}*/
			//painter.drawImage(QRect(iInit - (sh - jInit), jInit, sh - jInit, sh - jInit), state->mark);
			switch (state->slideImages[slideIndex].mark)
			{
				case PictureFlow::Unread:
					painter.drawImage(QRect(iInit - (sh - jInit), jInit, sh - jInit, sh - jInit), state->markUnread);
				break;
				case PictureFlow::Read:
					painter.drawImage(QRect(iInit - (sh - jInit), jInit, sh - jInit, sh - jInit), state->markRead);
				break;
				case PictureFlow::Opened:
					painter.drawImage(QRect(iInit - (sh - jInit), jInit, sh - jInit, sh - jInit), state->markOpened);
				break;
			}
		}
	}

	surfaceCache.insert(key, sr);
	imageHash.insert(slideIndex, img);

	return sr;
}

// Renders a slide to offscreen buffer. Returns a rect of the rendered area.
// col1 and col2 limit the column for rendering.
QRect PictureFlowSoftwareRenderer::renderSlide(const SlideInfo &slide, int col1, int col2)
{
	int blend = slide.blend;
	if (!blend)
		return QRect();

	QImage *src = surface(slide.slideIndex);
	if (!src)
		return QRect();

	QRect rect(0, 0, 0, 0);

	int sw = src->height();
	int sh = src->width();
	int h = buffer.height();
	int w = buffer.width();

	if (col1 > col2)
	{
		int c = col2;
		col2 = col1;
		col1 = c;
	}

	col1 = (col1 >= 0) ? col1 : 0;
	col2 = (col2 >= 0) ? col2 : w - 1;
	col1 = qMin(col1, w - 1);
	col2 = qMin(col2, w - 1);

	int zoom = 100;
	int distance = h * 100 / zoom;
	PFreal sdx = fcos(slide.angle);
	PFreal sdy = fsin(slide.angle);
	PFreal xs = slide.cx - state->slideWidth * sdx / 2;
	PFreal ys = slide.cy - state->slideWidth * sdy / 2;
	PFreal dist = distance * PFREAL_ONE;

	int xi = qMax((PFreal)0, ((w * PFREAL_ONE / 2) + fdiv(xs * h, dist + ys)) >> PFREAL_SHIFT);
	if (xi >= w)
		return rect;

	bool flag = false;
	rect.setLeft(xi);
	for (int x = qMax(xi, col1); x <= col2; x++)
	{
		PFreal hity = 0;
		PFreal fk = rays[x];
		if (sdy)
		{
			fk = fk - fdiv(sdx, sdy);
			hity = -fdiv((rays[x] * distance - slide.cx + slide.cy * sdx / sdy), fk);
		}

		dist = distance * PFREAL_ONE + hity;
		if (dist < 0)
			continue;

		PFreal hitx = fmul(dist, rays[x]);
		PFreal hitdist = fdiv(hitx - slide.cx, sdx);

		int column = sw / 2 + (hitdist >> PFREAL_SHIFT);
		if (column >= sw)
			break;
		if (column < 0)
			continue;

		rect.setRight(x);
		if (!flag)
			rect.setLeft(x);
		flag = true;

		int y1 = h / 2;
		int y2 = y1 + 1;
		QRgb *pixel1 = (QRgb *)(buffer.scanLine(y1)) + x;
		QRgb *pixel2 = (QRgb *)(buffer.scanLine(y2)) + x;
		QRgb pixelstep = pixel2 - pixel1;

		int center = (sh / 2);
		int dy = dist / h;
		int p1 = center * PFREAL_ONE - dy / 2;
		int p2 = center * PFREAL_ONE + dy / 2;

		const QRgb *ptr = (const QRgb *)(src->scanLine(column));
		if (blend == 256)
		{
			while ((y1 >= 0) && (y2 < h) && (p1 >= 0))
			{
				*pixel1 = ptr[p1 >> PFREAL_SHIFT];
				*pixel2 = ptr[p2 >> PFREAL_SHIFT];
				p1 -= dy;
				p2 += dy;
				y1--;
				y2++;
				pixel1 -= pixelstep;
				pixel2 += pixelstep;
			}
		} else {
			while ((y1 >= 0) && (y2 < h) && (p1 >= 0))
			{
				QRgb c1 = ptr[p1 >> PFREAL_SHIFT];
				QRgb c2 = ptr[p2 >> PFREAL_SHIFT];
				*pixel1 = blendColor(c1, bgcolor, blend);
				*pixel2 = blendColor(c2, bgcolor, blend);
				p1 -= dy;
				p2 += dy;
				y1--;
				y2++;
				pixel1 -= pixelstep;
				pixel2 += pixelstep;
			}
		}
	}

	rect.setTop(0);
	rect.setBottom(h - 1);
	return rect;
}

void PictureFlowSoftwareRenderer::renderSlides()
{
	int nleft = state->leftSlides.count();
	int nright = state->rightSlides.count();

	QRect r = renderSlide(state->centerSlide);
	int c1 = r.left();
	int c2 = r.right();

	for (int index = 0; index < nleft; index++)
	{
		QRect rs = renderSlide(state->leftSlides[index], 0, c1 - 1);
		if (!rs.isEmpty())
			c1 = rs.left();
	}

	for (int index = 0; index < nright; index++)
	{
		QRect rs = renderSlide(state->rightSlides[index], c2 + 1, buffer.width());
		if (!rs.isEmpty())
			c2 = rs.right();
	}
}

QPoint PictureFlowSoftwareRenderer::getPosImage(QImage image, int posImage, int m_x, int m_y)
{
	QPoint pos(m_x, m_y);
	switch (posImage)
	{
		case 0: // topLeft
			pos.setX(m_x);
			pos.setY(m_y);
		break;
		case 1: // topCenter
			pos.setX((buffer.width() / 2) - (image.width() / 2));
			pos.setY(m_y);
		break;
		case 2: // topRight
			pos.setX(buffer.width() - image.width() - m_x);
			pos.setY(m_y);
		break;
		case 3: // centerLeft
			pos.setX(m_x);
			pos.setY((buffer.height() / 2) - (image.height() / 2));
		break;
		case 4: // center
			pos.setX((buffer.width() / 2) - (image.width() / 2));
			pos.setY((buffer.height() / 2) - (image.height() / 2));
		break;
		case 5: // centerRight
			pos.setX(buffer.width() - image.width() - m_x);
			pos.setY((buffer.height() / 2) - (image.height() / 2));
		break;
		case 6: // bottomLeft
			pos.setX(m_x);
			pos.setY(buffer.height() - image.height() - m_y);
		break;
		case 7: // bottomCenter
			pos.setX((buffer.width() / 2) - (image.width() / 2));
			pos.setY(buffer.height() - image.height() - m_y);
		break;
		case 8: // bottomRight
			pos.setX(buffer.width() - image.width() - m_x);
			pos.setY(buffer.height() - image.height() - m_y);
		break;
	}
	return pos;
}

// Render the slides. Updates only the offscreen buffer.
void PictureFlowSoftwareRenderer::render()
{
	buffer.fill(state->backgroundColor);
	renderSlides();

	//-- Original -------------------------------------
	/*if (state->slideImages.size() > 0)
	{
		int size  = buffer.width() * 0.015;
		int start = buffer.width() * 0.010;

		QPainter painter(&buffer);
		painter.setPen(QColor(255, 255, 255).rgb() - state->backgroundColor);
		painter.setFont(QFont("Arial", start + size * 0.5));
		painter.drawText(start, start + size, QString().setNum(state->centerIndex + 1) + "/" + QString().setNum(state->slideImages.size()));
	}*/
	//-------------------------------------------------

//  int x = buffer.width() / 2;
//  int size  = buffer.width() * 0.021; //0.015
//  int start = buffer.width() * 0.010;
//  state->slideFont.setPointSize(start + size * 0.5);

	int m_x = state->margin.x();
	int m_y = state->margin.y();
	QRect rect(m_x, m_y, buffer.width() - (m_x * 2), buffer.height() - (m_y * 2));
	QRect rectTmp;
	QRect boundingRect = rect;
	QPainter painter(&buffer);
	painter.setFont(state->slideFont);
	QPen pen = painter.pen();

	if (state->showLinesPos)
	{
		pen.setStyle(Qt::DashLine);
		pen.setBrush(Qt::cyan);
		painter.setPen(pen);
		painter.drawRect(rect);
	}

	if (state->showTitleIcon)
	{
		QPoint posTitleIcon = getPosImage(state->slideTitleIcon, state->posTitleIcon, m_x, m_y);
		painter.drawImage(posTitleIcon.x() + state->marginTitleIcon.x(), posTitleIcon.y() + state->marginTitleIcon.y(), state->slideTitleIcon);

		if (state->showLinesPos)
		{
			pen.setBrush(Qt::magenta);
			painter.setPen(pen);
			painter.drawRect(posTitleIcon.x() + state->marginTitleIcon.x(), posTitleIcon.y() + state->marginTitleIcon.y(), state->slideTitleIcon.width(), state->slideTitleIcon.height());
		}
	}

	if (state->showIconExtra)
	{
		QPoint posIconExtra = getPosImage(state->slideIconExtra, state->posIconExtra, m_x, m_y);
		painter.drawImage(posIconExtra.x() + state->marginIconExtra.x(), posIconExtra.y() + state->marginIconExtra.y(), state->slideIconExtra);

		if (state->showLinesPos)
		{
			pen.setBrush(Qt::magenta);
			painter.setPen(pen);
			painter.drawRect(posIconExtra.x() + state->marginIconExtra.x(), posIconExtra.y() + state->marginIconExtra.y(), state->slideIconExtra.width(), state->slideIconExtra.height());
		}
	}

	if (state->showTitle)
	{
		rectTmp = rect;
		painter.setPen(state->fontColor);

		if (state->posTitle == 34 || state->posTitle == 130 || state->posTitle == 66)
		{
			rectTmp.setX(rect.x() - state->marginTitle.x());
			rectTmp.setWidth(rectTmp.width() - state->marginTitle.x());
		} else {
			rectTmp.setX(rect.x() + state->marginTitle.x());
			rectTmp.setWidth(rectTmp.width() + state->marginTitle.x());
		}

		painter.drawText(rectTmp.x(), rectTmp.y() + state->marginTitle.y(), rectTmp.width(), rectTmp.height(),
						 state->posTitle, state->slideTitle, &boundingRect);

		if (state->showLinesPos)
		{
			pen.setBrush(Qt::red);
			painter.setPen(pen);
			painter.drawRect(boundingRect);
		}
	}

	if (state->slideImages.size() > 0)
	{
		if (state->showCaption)
		{
			rectTmp = rect;
			painter.setPen(state->fontColor);

			if (state->posCaption == 34 || state->posCaption == 130 || state->posCaption == 66)
			{
				rectTmp.setX(rect.x() - state->marginCaption.x());
				rectTmp.setWidth(rectTmp.width() - state->marginCaption.x());
			} else {
				rectTmp.setX(rect.x() + state->marginCaption.x());
				rectTmp.setWidth(rectTmp.width() + state->marginCaption.x());
			}

			painter.drawText(rectTmp.x(), rectTmp.y() + state->marginCaption.y(), rectTmp.width(), rectTmp.height(),
							 state->posCaption, state->slideImages[state->centerIndex].caption, &boundingRect);

			if (state->showLinesPos)
			{
				pen.setBrush(Qt::green);
				painter.setPen(pen);
				painter.drawRect(boundingRect);
			}
		}
	}

	if (state->showNumber)
	{
		rectTmp = rect;
		painter.setPen(state->fontColor);

		if (state->posNumber == 34 || state->posNumber == 130 || state->posNumber == 66)
		{
			rectTmp.setX(rect.x() - state->marginNumber.x());
			rectTmp.setWidth(rectTmp.width() - state->marginNumber.x());
		} else {
			rectTmp.setX(rect.x() + state->marginNumber.x());
			rectTmp.setWidth(rectTmp.width() + state->marginNumber.x());
		}

		int c_idx = 0;
		if (state->slideImages.size() > 0)
			c_idx = state->centerIndex + 1;

		painter.drawText(rectTmp.x(), rectTmp.y() + state->marginNumber.y(), rectTmp.width(), rectTmp.height(),
						 state->posNumber, QString(state->numberFormat).arg(c_idx).arg(state->slideImages.size()), &boundingRect);

		if (state->showLinesPos)
		{
			pen.setBrush(Qt::yellow);
			painter.setPen(pen);
			painter.drawRect(boundingRect);
		}
	}

	if (state->showTriangle)
	{
		int px = buffer.width() / 2;
		int size = buffer.width() * 0.016; // 0.015
		int start = buffer.width() * 0.010;// * 0.010
		for (int j = start; j < size; j++)
		{
			buffer.setPixel(QPoint(px, j), state->fontColor);
			for (int i = 0; i < size - j; i++)
			{
				buffer.setPixel(QPoint(px - i, j), state->fontColor);
				buffer.setPixel(QPoint(px + i, j), state->fontColor);
			}
		}
	}

	painter.end();
	dirty = false;
}

// -----------------------------------------

class PictureFlowPrivate
{
public:
	PictureFlowState *state;
	PictureFlowAnimator *animator;
	PictureFlowAbstractRenderer *renderer;
	QTimer triggerTimer;
};

PictureFlow::PictureFlow(QWidget *parent, FlowType flowType) : QWidget(parent)
{
	d = new PictureFlowPrivate;

	switch (flowType)
	{
		case CoverFlowLike:
			d->state = new PictureFlowState(50, 0);
			break;
		case Strip:
			d->state = new PictureFlowState(0, 1);
			break;
		case StripOverlapped:
			d->state = new PictureFlowState(0, 0);
			break;
		default:
			d->state = new PictureFlowState(50, 0);
			break;
	}

	framesSkip = 0;

	d->state->reset();
	d->state->reposition();

	d->renderer = new PictureFlowSoftwareRenderer;
	d->renderer->state = d->state;
	d->renderer->widget = this;
	d->renderer->init();

	d->animator = new PictureFlowAnimator;
	d->animator->state = d->state;

	QObject::connect(&d->animator->animateTimer, SIGNAL(timeout()), this, SLOT(updateAnimation()));
	QObject::connect(&d->triggerTimer, SIGNAL(timeout()), this, SLOT(render()));

	setAttribute(Qt::WA_StaticContents, true);
	setAttribute(Qt::WA_OpaquePaintEvent, true); // false
	setAttribute(Qt::WA_NoSystemBackground, true); // false

	setPicFlowFont("Arial", 14);
	setShowLinesPos(false);
}

PictureFlow::~PictureFlow()
{
	delete d->renderer;
	delete d->animator;
	delete d->state;
	delete d;
}

int PictureFlow::slideCount() const
{
	return d->state->slideImages.count();
}

QColor PictureFlow::backgroundColor() const
{
	return QColor(d->state->backgroundColor);
}

void PictureFlow::setBackgroundColor(const QColor &c)
{
	d->state->backgroundColor = c.rgb();
	triggerRender();
}

QFont PictureFlow::getPicFlowFont() const
{
	return d->state->slideFont;
}

void PictureFlow::setPicFlowFont(const QFont& font)
{
	this->setFont(font);
	d->state->slideFont = font;
	triggerRender();
}

void PictureFlow::setPicFlowFont(QString f_family, int f_size, bool f_bold, bool f_italic)
{
	QFont font(f_family, f_size);
	font.setBold(f_bold);
	font.setItalic(f_italic);
	setPicFlowFont(font);
}

QColor PictureFlow::getFontColor() const
{
	return QColor(d->state->fontColor);
}

void PictureFlow::setFontColor(const QColor& c)
{
	d->state->fontColor = c.rgb();
	triggerRender();
}

QSize PictureFlow::slideSize() const
{
	return QSize(d->state->slideWidth, d->state->slideHeight);
}

void PictureFlow::setSlideSize(QSize size)
{
	d->state->slideWidth = size.width();
	d->state->slideHeight = size.height();
	d->state->reposition();
	triggerRender();
}

void PictureFlow::setSlideFixSize(bool fix_size)
{
	d->state->slideFixSize = fix_size;
	d->state->reposition();
	triggerRender();
}

PictureFlow::ReflectionEffect PictureFlow::reflectionEffect() const
{
	return d->state->reflectionEffect;
}

void PictureFlow::setReflectionEffect(ReflectionEffect effect)
{
	d->state->reflectionEffect = effect;
	triggerRender();
}

QImage PictureFlow::slide(int index) const
{
	QImage *i = 0;
	if ((index >= 0) && (index < slideCount()))
		i = d->state->slideImages[index].imagen;
	return i ? QImage(*i) : QImage();
}

void PictureFlow::addSlide(const QImage &image, QString caption)
{
	int c = d->state->slideImages.count();
	d->state->slideImages.resize(c + 1);
	d->state->slideImages[c].imagen  = new QImage(image);
	d->state->slideImages[c].caption = caption;
	d->state->slideImages[c].mark	= Unread;
	triggerRender();
}

void PictureFlow::addSlide(const QPixmap &pixmap, QString caption)
{
	addSlide(pixmap.toImage(), caption);
}

void PictureFlow::removeSlide(int index)
{
	int c = d->state->slideImages.count();
	if (index >= 0 && index < c)
	{
		d->state->slideImages.remove(index);
		setCenterIndex(index);
	}
}

void PictureFlow::setSlide(int index, const QImage &image, QString caption)
{
	if ((index >= 0) && (index < slideCount()))
	{
		QImage *i = image.isNull() ? 0 : new QImage(image);
		delete d->state->slideImages[index].imagen;
		d->state->slideImages[index].imagen  = i;
		d->state->slideImages[index].caption = caption;
		triggerRender();
	}
}

void PictureFlow::setSlide(int index, const QPixmap &pixmap, QString caption)
{
	setSlide(index, pixmap.toImage(), caption);
}

int PictureFlow::centerIndex() const
{
	return d->state->centerIndex;
}

void PictureFlow::setCenterIndex(int index)
{
	index = qMin(index, slideCount() - 1);
	index = qMax(index, 0);
	d->state->centerIndex = index;
	d->state->reset();
	d->animator->stop(index);
	triggerRender();
}

void PictureFlow::clear()
{
	const int c = d->state->slideImages.count();
	for (int i = 0; i < c; i++)
		delete d->state->slideImages[i].imagen;

	d->state->slideImages.resize(0);
	d->state->reset();
	triggerRender();
}

void PictureFlow::render()
{
	d->renderer->dirty = true;
	update();
}

void PictureFlow::triggerRender()
{
	d->triggerTimer.setSingleShot(true);
	d->triggerTimer.start(0);
}

void PictureFlow::showPrevious()
{
	int step = d->animator->step;
	int center = d->state->centerIndex;

	if (step > 0)
	{
		d->animator->start(center);
		emit centerIndexChanged(center);
	}

	if (step == 0)
	{
		if (center > 0)
		{
			d->animator->start(center - 1);
			emit centerIndexChanged(center - 1);
		}
	}

	if (step < 0)
	{
		d->animator->target = qMax(0, center - 2);
		emit centerIndexChanged(qMax(0, center - 2));
	}
}

void PictureFlow::showNext()
{
	int step = d->animator->step;
	int center = d->state->centerIndex;

	if (step < 0)
	{
		d->animator->start(center);
		emit centerIndexChanged(center);
	}

	if (step == 0)
	{
		if (center < slideCount() - 1)
		{
			d->animator->start(center + 1);
			emit centerIndexChanged(center + 1);
		}
	}

	if (step > 0)
	{
		d->animator->target = qMin(center + 2, slideCount() - 1);
		emit centerIndexChanged(qMin(center + 2, slideCount() - 1));
	}
}

void PictureFlow::showSlide(int index)
{
	index = qMax<int>(index, 0);
	index = qMin<int>(slideCount() - 1, index);
	if ((int)index == d->state->centerSlide.slideIndex)
	{
		return;
	}

	int distance = centerIndex() - index;

	if (abs(distance) > 10)
	{
		if (distance < 0)
			setCenterIndex(centerIndex() + (-distance) - 10);
		else
			setCenterIndex(centerIndex() - distance + 10);
	}

	d->state->centerIndex = index;
	d->animator->start(index);
}

void PictureFlow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Left)
	{
/*	  if (event->modifiers() == Qt::ControlModifier)
			showSlide(centerIndex() - 10);
		else*/
			showPrevious();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Right)
	{
/*	  if (event->modifiers() == Qt::ControlModifier)
			showSlide(centerIndex() + 10);
		else*/
			showNext();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Up)
	{
		// TODO emit selected signal
		return;
	}

	event->ignore();
}

void PictureFlow::mousePressEvent(QMouseEvent *event)
{
	mousePressEvent(event, 0);
}

void PictureFlow::mousePressEvent(QMouseEvent *event, int slideWidth)
{
	if (event->x() > (width() + slideWidth) / 2)
		showNext();
	else if (event->x() < (width() - slideWidth) / 2)
		showPrevious();
	//else (centered slide space)
}

void PictureFlow::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	d->renderer->paint();
}

void PictureFlow::resizeEvent(QResizeEvent *event)
{
	if (d->state->slideFixSize)
	{
		triggerRender();
	} else {
	  //triggerRender();
		int heightWidget = event->size().height();
		int height, width;
		height = heightWidget * 0.55;
		width = height * 0.65;
		setSlideSize(QSize(width, height));
		render();
		d->animator->start(centerIndex());
	}

	QWidget::resizeEvent(event);
}

#include <QTime>
void PictureFlow::updateAnimation() //bucle principal
{
	QTime now;
	now.start();
	bool frameSkiped = false;

	int old_center = d->state->centerIndex;
	d->animator->update();
  //triggerRender();
	if (framesSkip == 0)
		render(); //triggerRender();
	else {
		framesSkip--;
		frameSkiped = true;
	}

	if (d->state->centerIndex != old_center)
		emit centerIndexChangedSilent(d->state->centerIndex);
	if (d->animator->animating == true)
	{
		int difference = 10 - now.elapsed();
		if (difference >= 0 && !frameSkiped)
		{
			QTimer::singleShot(difference, this, SLOT(updateAnimation()));
		} else {
			QTimer::singleShot(0, this, SLOT(updateAnimation()));
			if (!frameSkiped)
				framesSkip = -((difference - 10) / 10);
		}
	} else
		emit finishedAnimation();
}

void PictureFlow::setFlowType(FlowType flowType)
{
	switch (flowType)
	{
		case CoverFlowLike:
			d->state->rawAngle = 50;
			d->state->spacingRatio = 0;
			d->state->reposition();
			break;
		case Strip:
			d->state->rawAngle = 0;
			d->state->spacingRatio = 1;
			d->state->reposition();
			break;
		case StripOverlapped:
			d->state->rawAngle = 0;
			d->state->spacingRatio = 0;
			d->state->reposition();
			break;
		default:
			d->state->rawAngle = 50;
			d->state->spacingRatio = 0;
			d->state->reposition();
			break;
	}

	d->state->reset();
	d->renderer->init();
	repaint();
}

void PictureFlow::setShowLinesPos(bool enable)
{
	d->state->showLinesPos = enable;
	triggerRender();
}

void PictureFlow::setShowTitle(bool enable)
{
	d->state->showTitle = enable;
	triggerRender();
}

void PictureFlow::setSlideTitle(QString title)
{
	d->state->slideTitle = title;
	triggerRender();
}

void PictureFlow::setSlideTitle(QString title, int position)
{
	d->state->slideTitle = title;
	setSlideTitlePos(position);
}

void PictureFlow::setSlideTitlePos(int position)
{
	d->state->posTitle = position;
	triggerRender();
}

void PictureFlow::setSlideTitleMargin(QPoint margin)
{
	d->state->marginTitle = margin;
	triggerRender();
}

void PictureFlow::setShowTitleIcon(bool enable)
{
	d->state->showTitleIcon = enable;
	triggerRender();
}

void PictureFlow::setSlideTitleIcon(QString titleIcon)
{
	setSlideTitleIcon(QImage(titleIcon));
}

void PictureFlow::setSlideTitleIcon(QPixmap titleIcon)
{
	setSlideTitleIcon(titleIcon.toImage());
}

void PictureFlow::setSlideTitleIcon(QImage titleIcon)
{
	d->state->slideTitleIcon = titleIcon;
	triggerRender();
}

void PictureFlow::setSlideTitleIconPos(int position)
{
	d->state->posTitleIcon = position;
	triggerRender();
}

void PictureFlow::setSlideTitleIconMargin(QPoint margin)
{
	d->state->marginTitleIcon = margin;
	triggerRender();
}

void PictureFlow::setShowCaption(bool enable)
{
	d->state->showCaption = enable;
	triggerRender();
}

void PictureFlow::setSlideCaption(int index, QString caption)
{
	d->state->slideImages[index].caption = caption;
}

void PictureFlow::setSlideCaptionPos(int position)
{
	d->state->posCaption = position;
	triggerRender();
}

void PictureFlow::setSlideCaptionMargin(QPoint margin)
{
	d->state->marginCaption = margin;
	triggerRender();
}

void PictureFlow::setShowNumber(bool enable)
{
	d->state->showNumber = enable;
	triggerRender();
}

void PictureFlow::setSlideNumberPos(int position)
{
	d->state->posNumber = position;
	triggerRender();
}

void PictureFlow::setSlideNumberMargin(QPoint margin)
{
	d->state->marginNumber = margin;
	triggerRender();
}

void PictureFlow::setSlideNumberFormat(QString format)
{
	d->state->numberFormat = format;
	triggerRender();
}

void PictureFlow::setShowIconExtra(bool enable)
{
	d->state->showIconExtra = enable;
	triggerRender();
}

void PictureFlow::setSlideIconExtra(QString iconExtra)
{
	setSlideIconExtra(QImage(iconExtra));
}

void PictureFlow::setSlideIconExtra(QPixmap iconExtra)
{
	setSlideIconExtra(iconExtra.toImage());
}

void PictureFlow::setSlideIconExtra(QImage iconExtra)
{
	d->state->slideIconExtra = iconExtra;
	triggerRender();
}

void PictureFlow::setSlideIconExtraPos(int position)
{
	d->state->posIconExtra = position;
	triggerRender();
}

void PictureFlow::setSlideIconExtraMargin(QPoint margin)
{
	d->state->marginIconExtra = margin;
	triggerRender();
}

void PictureFlow::setSlideMargin(QPoint margin)
{
	d->state->margin = margin;
	triggerRender();
}

void PictureFlow::setShowTriangle(bool enable)
{
	d->state->showTriangle = enable;
	triggerRender();
}

void PictureFlow::setMarkImage(FlowStatus flowStatus, const QImage &image)
{
	QMatrix m;
	m.rotate(-90);
	m.scale(-1,1);
	QImage imageTransformed = image.transformed(m, Qt::SmoothTransformation);
	switch (flowStatus)
	{
		case Unread:
			d->state->markUnread = imageTransformed;
		break;
		case Read:
			d->state->markRead = imageTransformed;
		break;
		case Opened:
			d->state->markOpened = imageTransformed;
		break;
	}
}

void PictureFlow::markSlide(int index, FlowStatus flowStatus)
{
	if (index < d->state->slideImages.size())
		d->state->slideImages[index].mark = flowStatus;
}

void PictureFlow::updateMarks()
{
	d->renderer->init();
	d->renderer->paint();
	repaint();
}

void PictureFlow::unmarkSlide(int index)
{
	if (index < d->state->slideImages.size())
		d->state->slideImages[index].mark = Unread;
}

void PictureFlow::setShowMarks(bool enable)
{
	d->state->showMarks = enable;
	updateMarks();
}

void PictureFlow::resortCovers(QList<int> newOrder)
{
	QVector<stSlideImage> slideImagesNew;

	QVector<SlideInfo> slidesInfo;
	slidesInfo << d->state->leftSlides << d->state->centerSlide << d->state->rightSlides;
	QVector<SlideInfo> slidesInfoNew;

	int order = 0;
	const int listOrderSize = newOrder.size();
	for (int i = 0; i < listOrderSize; ++i)
	{
		slideImagesNew << d->state->slideImages.at(newOrder.at(i));
		slidesInfoNew << slidesInfo.at(newOrder.at(i));
		slidesInfoNew.last().slideIndex = order++;
	}

	d->state->slideImages = slideImagesNew;
	d->state->leftSlides  = slidesInfoNew.mid(0, d->state->leftSlides.size());
	d->state->centerSlide = slidesInfoNew.at(d->state->centerIndex);
	d->state->leftSlides  = slidesInfoNew.mid(d->state->centerIndex + 1, d->state->leftSlides.size());

	setCenterIndex(d->state->centerIndex);
}
