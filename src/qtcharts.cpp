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

namespace physim {

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
	//QtCharts::QChartView view;
	int argc = 1;
	char t[] = "";
	char *argv[] = { t };
	QApplication app(argc, argv);

	// build the chart
	auto chart = new QtCharts::QChart();
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
	auto view = new QtCharts::QChartView(chart);
	view->setRenderHint(QPainter::Antialiasing);
	QMainWindow window;
	window.setCentralWidget(view);
	window.resize(820, 600);
	window.show();
	app.exec();
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
