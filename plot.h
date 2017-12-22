/****************************************************************************************
//    plot.h
//
//    Copyright (C) 2017  Zach (Zhengyu) Peng, https://zpeng.me
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************************/

#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class Plot;
}

class Plot : public QWidget
{
  Q_OBJECT

public:
  explicit Plot(QWidget *parent = 0);
  ~Plot();

public slots:
  void updatePlot(const QVector<float> &time, const QVector<float> &data);
  void updatePlot(const QVector<QPointF> &plotData);
  void clearPlot();

private:
  Ui::Plot *ui;
  QChartView *chartView;

  QLineSeries *series;
  QChart *chart;
};

#endif // PLOT_H
