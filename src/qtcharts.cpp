/*
 * PhySim library -- DEVS for physics
 * Copyright (C) 2020  Hugues Cassé <hug.casse@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */
#include <physim/qtcharts.h>

#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>
#include <QSpinBox>
#include <QLabel>

#include "physim_qt.h"

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

string cropped_name(string name) {
	auto p = name.find_first_of('.');
	if(p == string::npos)
		return name;
	else
		return name.substr(p + 1);
}

class Stat {
public:
	Stat(AbstractPort *_port, int _i, QColor color)
		: series(new QLineSeries()), port(_port), i(_i)
	{
		series->setColor(color);
		series->setName(cropped_name(port->fullname()).c_str());
	}
	QLineSeries *series;
	AbstractPort *port;
	int i;
	void record(date_t date) {
		 series->append(date, port->asReal(i));
	}
};

class ModelNode {
public:
	ModelNode(Selector *selector, ModelNode *parent = nullptr, int index = 0)
		: _parent(parent), _index(index), _selector(selector) { }
	virtual ~ModelNode() { }
	virtual ModelNode *at(int i) = 0;
	virtual int countChildren() = 0;
	virtual string name() = 0;
	inline ModelNode *parent() const { return _parent; }
	inline int index() const { return _index; }
	virtual bool isSelectable() const { return false; }
	virtual bool selected() const { return false; }
	virtual void select(bool s) { }
	inline Selector *selector() const { return _selector; }
private:
	ModelNode *_parent;
	int _index;
	Selector *_selector;
};

class PortNode: public ModelNode {
public:
	PortNode(AbstractPort *port, int i, Selector *selector, ModelNode *parent, int index)
		: ModelNode(selector, parent, index), _i(i), _port(port), _selected(false) { }
	ModelNode *at(int i) override { return nullptr; }
	int countChildren() override { return 0; }
	string name() override {
		if(_name == "") {
			if(_port->size() == 1)
				_name = _port->name();
			else
				_name = _port->name() + "[" + to_string(_i) + "]";
			if(_port->mode() == OUT)
				_name = _name + " (output)";
			else
				_name = _name + " (input)";
		}
		return _name;
	}

	bool isSelectable() const { return _port->supportsReal(); }
	bool selected() const { return _selected; }
	void select(bool s) override {
		_selected = s;
		if(s)
			selector()->select(_port, _i);
		else
			selector()->unselect(_port, _i);
	}

private:
	AbstractPort *_port;
	int _i;
	string _name;
	bool _selected = false;
};

class SimpleModelNode: public ModelNode {
public:
	SimpleModelNode(Model *model, Selector *selector, ModelNode *parent, int index)
		: ModelNode(selector, parent, index), _model(model), _children_done(false) { }
	ModelNode *at(int i) { initChildren(); return _children[i]; }
	int countChildren() override { initChildren(); return _children.size(); }
	string name() override { return _model->name(); }

protected:

	virtual void initChildren() {
		if(_children_done)
			return;
		_children_done = true;
		int i = 0;
		for(auto p: _model->ports()) {
			if(p->size() == 1) {
				_children.push_back(new PortNode(p, 0, selector(), this, i));
				i++;
			}
			else
				for(int j = 0; j < p->size(); j++) {
					_children.push_back(new PortNode(p, j, selector(), this, i));
					i++;
				}
		}
	}
	vector<ModelNode *> _children;
	bool _children_done;

private:
	Model *_model;
};

class ComposedModelNode: public SimpleModelNode {
public:
	ComposedModelNode(ComposedModel *model, Selector *selector, ModelNode *parent = nullptr, int index = 0)
		: SimpleModelNode(model, selector, parent, index), _model(model) { }

protected:

	void initChildren() override {
		if(_children_done)
			return;
		SimpleModelNode::initChildren();
		int i = _children.size();
		for(auto m: _model->subModels()) {
			if(m->isComposed())
				_children.push_back(new ComposedModelNode(static_cast<ComposedModel *>(m), selector(), this, i));
			else
				_children.push_back(new SimpleModelNode(m, selector(), this, i));
			i++;
		}
	}

	ComposedModel *_model;
};


// TreeModel class

///
TreeModel::TreeModel(Selector *selector, ComposedModel *model, QObject *parent)
	: QAbstractItemModel(parent), root(new ComposedModelNode(model, selector))
{
}

///
TreeModel::~TreeModel() {
}

///
QVariant TreeModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();
	ModelNode *n = static_cast<ModelNode *>(index.internalPointer());
	if(role == Qt::DisplayRole) {
		if(index.column() == 0)
			return n->name().c_str();
	}
	else if(role == Qt::CheckStateRole) {
		if(index.column() == 1 and n->isSelectable()) {
			if(n->selected())
				return Qt::Checked;
			else
				return Qt::Unchecked;
		}
	}
	return QVariant();
}

///
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
	if(!index.isValid())
		return Qt::NoItemFlags;
	ModelNode *n = static_cast<ModelNode *>(index.internalPointer());
	if(index.column() == 1 and n->isSelectable())
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///
QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role ) const {
	if(role != Qt::DisplayRole)
		return QVariant();
	return "Model";
}

///
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
	if(column > 1)
		return QModelIndex();
	if(!parent.isValid())
		return createIndex(row, column, root->at(row));
	else
		return createIndex(row, column, static_cast<ModelNode *>(parent.internalPointer())->at(row));
}

///
QModelIndex TreeModel::parent(const QModelIndex &index) const {
	if(!index.isValid())
		return QModelIndex();
	else {
		auto n = static_cast<ModelNode *>(index.internalPointer());
		if(n == root)
			return QModelIndex();
		else
			return createIndex(n->parent()->index(), 0, n->parent());
	}
}


///
int TreeModel::rowCount(const QModelIndex &parent) const {
	if(parent.column() > 0)
		return 0;
	ModelNode *n;
	if(!parent.isValid())
		n = root;
	else
		n = static_cast<ModelNode *>(parent.internalPointer());
	return n->countChildren();
}

///
int TreeModel::columnCount(const QModelIndex &parent) const {
	return 2;
}


bool TreeModel::setData(
	const QModelIndex &index,
	const QVariant &value,
	int role)
{
	if(not index.isValid())
		return false;
	ModelNode *n = static_cast<ModelNode *>(index.internalPointer());
    if(role == Qt::CheckStateRole and n->isSelectable()) {
    	auto b = value.toBool();
    	if(b != n->selected()) {
    		n->select(b);
            emit dataChanged(index, index);
    	}
        return true;
    }
    return false;
}


// Chart class

///
bool Chart::sceneEvent(QEvent *event) {
	if (event->type() == QEvent::Gesture)
		return gestureEvent(static_cast<QGestureEvent *>(event));
	return QChart::event(event);
}

///
bool Chart::gestureEvent(QGestureEvent *event) {
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


// LineChartView class

///
LineChartView::LineChartView() { }

///
LineChartView::LineChartView(QtCharts::QChart *chart): QtCharts::QChartView(chart) {
}

///
void LineChartView::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	}
}

///
void LineChartView::mousePressEvent(QMouseEvent *event) {
	if(event->button() == Qt::LeftButton) {
		QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
		last_pos = event->pos();
		event->accept();
	}
	QChartView::mousePressEvent(event);
}

///
void LineChartView::mouseMoveEvent(QMouseEvent *event) {
	if(event->buttons() & Qt::LeftButton) {
		auto s = event->pos() - last_pos;
		chart()->scroll(-s.x(), s.y());
		last_pos = event->pos();
		event->accept();
	}
	QChartView::mouseMoveEvent(event);
}

///
void LineChartView::wheelEvent(QWheelEvent *event) {
	qreal factor = event->angleDelta().y() > 0? 0.75: 1.5;
	chart()->zoom(factor);
	event->accept();
	QChartView::wheelEvent(event);
}


// QTSimulateWindow class

///
QtSimulateWindow::QtSimulateWindow(
	string name,
	QApplication& application,
	ComposedModel *_model,
	Simulation *_sim)
:
	app(application),
	run_action(nullptr),
	model(_model),
	sim(_sim),
	duration(20)
{
	// set stat display
	auto chart = new Chart();
	chart->setTitle(name.c_str());
	view.setRenderHint(QPainter::Antialiasing);
	view.setChart(chart);

	// make the content
	auto split = new QSplitter(this);
	auto value_tree = new QTreeView(split);
	value_tree->setModel(new TreeModel(&selector, model));
	value_tree->show();
	split->addWidget(value_tree);
	split->addWidget(&view);

	// put all together
	setCentralWidget(split);
	resize(820, 600);
	setWindowTitle(QString(name.c_str()));
	createActions();
}

///
void QtSimulateWindow::createActions() {
	QMenu *file_menu = menuBar()->addMenu(tr("File"));

	auto quit_action = new QAction("Quit", this);
	quit_action->setStatusTip("Quit the application.");
	connect(quit_action, &QAction::triggered, &app, &QApplication::closeAllWindows);
	file_menu->addAction(quit_action);

	QMenu *sim_menu = menuBar()->addMenu(tr("&Simulation"));
	QToolBar *sim_bar = addToolBar(tr("Simulation"));

	run_action = new QAction("&Run", this);
	run_action->setStatusTip("Run the simulation");
	connect(run_action, &QAction::triggered, this, &QtSimulateWindow::do_run);
	sim_menu->addAction(run_action);
	sim_bar->addAction(run_action);

	auto duration_label = new QLabel("  Duration:");
	sim_bar->addWidget(duration_label);
	auto duration_box = new QSpinBox();
	duration_box->setMinimum(0);
	duration_box->setMaximum(INT_MAX);
	duration_box->setValue(duration);
	sim_bar->addWidget(duration_box);
	connect(duration_box, QOverload<int>::of(&QSpinBox::valueChanged), this, &QtSimulateWindow::durationChanged);

}

///
void QtSimulateWindow::run() {
	show();
	app.exec();
}

///
void QtSimulateWindow::durationChanged(int i) {
	duration = i;
}

///
void QtSimulateWindow::do_run() {

	// free previous data
	auto chart = view.chart();
	chart->removeAllSeries();
	for(auto s: stats)
		delete s;
	stats.clear();

	// prepare the data
	int color = 0;
	for(auto p: selector.ports()) {
		stats.push_back(new Stat(p.first, p.second, colors[color]));
		color = (color + 1) % color_count;
	}

	// generate the graph
	sim->stop();
	sim->start();
	for(auto s: stats)
		s->record(sim->date());
	while(sim->date() <= duration) {
		sim->step();
		for(auto s: stats)
			s->record(sim->date());
	}

	// display the graph
	for(auto s: stats)
		chart->addSeries(s->series);
	chart->createDefaultAxes();
}


namespace physim {

using namespace QtCharts;

static int argc = 1;
static char app_name[] = "qtcharts";
static char *argv[] = { app_name, nullptr };

class Application: public QApplication {
public:
	Application(): QApplication(argc, argv) {
	}
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
	chart->setTitle(name().c_str());
	chart->createDefaultAxes();

	// display it
	auto view = new LineChartView(chart);
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

/**
 * @fn QtSimulate;
 * Class defining a main program simulation running a Qt window
 * letting the user interact with the simulation.
 */

///
QtSimulate::QtSimulate(string name): ApplicationModel(name) {
}

///
int QtSimulate::perform() {
	Application app;
	QtSimulateWindow win(name(), app, this, &sim());
	win.run();
	return 0;
}

} // physim
