#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

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
    void updatePlot();

  private:
    Ui::Plot *ui;
    QChartView *chartView;

    QSplineSeries *series;
    QChart *chart;
};

#endif // PLOT_H
