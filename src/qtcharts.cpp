/*
 * qtcharts.h
 *
 *  Created on: 15 nov. 2020
 *      Author: casse
 */

#include <physim/qtcharts.h>
#include <QApplication>
#include <QLineSeries>
#include <QChartView>
#include <QMainWindow>
#include <QValueAxis>
#include <QGestureEvent>

namespace physim {

using namespace QtCharts;

static QColor colors[] = {
	QColor(Qt::black),
	QColor(Qt::blue),
	QColor(Qt::green),
	QColor(Qt::red),
	QColor(Qt::darkBlue),
	QColor(Qt::darkGreen),
	QColor(Qt::darkRed),
	QColor(Qt::darkCyan),
	QColor(Qt::darkYellow),
	QColor(Qt::darkMagenta)
};
static const int color_count = sizeof(colors) / sizeof(QColor);

static int argc = 1;
static char app_name[] = "qtcharts";
static char *argv[] = { app_name, nullptr };

class Application: public QApplication {
public:
	Application(): QApplication(argc, argv) {
	}
};

class Chart : public QtCharts::QChart {
public:
	bool sceneEvent(QEvent *event) {
		if (event->type() == QEvent::Gesture)
			return gestureEvent(static_cast<QGestureEvent *>(event));
		return QChart::event(event);
	}

	bool gestureEvent(QGestureEvent *event) {
		if(QGesture *gesture = event->gesture(Qt::PanGesture)) {
			QPanGesture *pan = static_cast<QPanGesture *>(gesture);
			QChart::scroll(-(pan->delta().x()), pan->delta().y());
		}
		if (QGesture *gesture = event->gesture(Qt::PinchGesture)) {
			QPinchGesture *pinch = static_cast<QPinchGesture *>(gesture);
			if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
				QChart::zoom(pinch->scaleFactor());
		}
		return true;
	}
};

class LineChartView: public QtCharts::QChartView {
public:
	LineChartView(QtCharts::QChart *chart): QtCharts::QChartView(chart) { }
protected:

	void keyPressEvent(QKeyEvent *event) {
		switch (event->key()) {
		case Qt::Key_Plus:
			chart()->zoomIn();
			break;
		case Qt::Key_Minus:
			chart()->zoomOut();
			break;
		}
	}


	void mousePressEvent(QMouseEvent *event) {
		if(event->button() == Qt::LeftButton) {
			QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
			last_pos = event->pos();
			event->accept();
		}
		QChartView::mousePressEvent(event);
	}

	void mouseMoveEvent(QMouseEvent *event) {
		if(event->buttons() & Qt::LeftButton) {
			auto s = event->pos() - last_pos;
			chart()->scroll(-s.x(), s.y());
			last_pos = event->pos();
			event->accept();
		}
		QChartView::mouseMoveEvent(event);
	}

	void wheelEvent(QWheelEvent *event) {
		qreal factor = event->angleDelta().y() > 0? 0.75: 1.5;
		chart()->zoom(factor);
		event->accept();
		QChartView::wheelEvent(event);
	}

private:
	QPointF last_pos;
};

class LineWindow: public QMainWindow {
public:
	LineWindow(Application& application, Chart *chart):
		app(application), view(chart)
	{
		view.setRenderHint(QPainter::Antialiasing);
		setCentralWidget(&view);
		resize(820, 600);
	}

	void run() {
		show();
		app.exec();
	}

private:
	LineChartView view;
	Application& app;
};


/**
 * @class QtLineDisplay
 * Displayer model able to show several output ports values
 * as a chart of lines.
 */

///
QtLineDisplay::AbstractReporter::AbstractReporter(QtLineDisplay *d) {
	color = colors[d->_color];
	d->_color = (d->_color + 1) % color_count;
}

/**
 * Build a model displaying stats using Qt line charts.
 * @param name		Name of the model.
 * @param parent	Parent model.
 */
QtLineDisplay::QtLineDisplay(string name, ComposedModel *parent)
	: Model(name, parent), _color(0)
	{ }

///
QtLineDisplay::~QtLineDisplay() {
	for(auto r: _reps)
		delete r;
}

/**
 * @fn void QtLineDisplay::add(OutputPort<T, N>& port, int i);
 * Add the given port or value of the output port to be displayed.
 * @param port	Output port to display.
 * @param i		For array port, the index of the value to display.
 */

///
void QtLineDisplay::start() {

}

///
void QtLineDisplay::stop() {
	Application app;
	/*int argc = 1;
	char t[] = "";
	char *argv[] = { t };
	QApplication app(argc, argv);*/

	// build the chart
	auto chart = new Chart();
	for(auto r: _reps) {
		auto ls = new QtCharts::QLineSeries();
		for(int i = 0; i < _dates.size(); i++)
			ls->append(_dates[i], r->vals[i]);
		ls->setColor(r->color);
		ls->setName(r->name().c_str());
		chart->addSeries(ls);
	}
	//chart->legend()->hide();
	chart->setTitle(name().c_str());
	chart->createDefaultAxes();

	// display it
	auto view = new LineChartView(chart);
	/*view->setRenderHint(QPainter::Antialiasing);
	QMainWindow window;
	window.setCentralWidget(view);
	window.resize(820, 600);
	window.show();
	app.exec();*/
	LineWindow win(app, chart);
	win.run();
}

///
void QtLineDisplay::update() {
	_dates.push_back(date());
	for(auto r: _reps)
		r->record();
}

void QtLineDisplay::propagate(const AbstractPort& port) {
	sim().triggerLast(*this);
}

} // physim
