/****************************************************************************************
//    Copyright (C) 2017  Zhengyu Peng, zhengyu.peng@ttu.edu
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
