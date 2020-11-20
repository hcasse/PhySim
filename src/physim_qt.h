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

#ifndef PHYSIM_QT_H_
#define PHYSIM_QT_H_

#include <physim.h>
using namespace physim;

#include <string>
using namespace std;

#include <QApplication>
#include <QLineSeries>
#include <QChartView>
#include <QMainWindow>
#include <QValueAxis>
#include <QGestureEvent>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QAbstractItemModel>

class Selector {
public:
	void select(AbstractPort *p, int i) { _ports.insert(make_pair(p, i)); }
	void unselect(AbstractPort *p, int i) { _ports.erase(make_pair(p, i)); }
	const set<pair<AbstractPort *, int>> ports() const { return _ports; }
private:
	set<pair<AbstractPort *, int> > _ports;
};

class ComposedModelNode;
class Stat;

class TreeModel: public QAbstractItemModel {
	Q_OBJECT
public:
	explicit TreeModel(Selector *selector, ComposedModel *model, QObject *parent = nullptr);
	~TreeModel();

	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(
		int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	QModelIndex index(
		int row,
		int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
	ComposedModelNode *root;
};

class Chart : public QtCharts::QChart {
public:
	bool sceneEvent(QEvent *event);
	bool gestureEvent(QGestureEvent *event);
};

class LineChartView: public QtCharts::QChartView {
public:
	LineChartView();
	LineChartView(QtCharts::QChart *chart);
protected:
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
private:
	QPointF last_pos;
};

class QtSimulateWindow: public QMainWindow {
	Q_OBJECT
public:
	QtSimulateWindow(string name, QApplication& application, ComposedModel *_model, Simulation *_sim);
	void run();
private slots:
	void do_run();
	void durationChanged(int i);
private:
	void createActions();
	LineChartView view;
	QApplication& app;
	QAction *run_action;
	ComposedModel *model;
	Selector selector;
	vector<Stat *> stats;
	Simulation *sim;
	duration_t duration;
};

#endif /* PHYSIM_QT_H_ */
