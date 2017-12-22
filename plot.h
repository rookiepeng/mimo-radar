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
    void updatePlot(const QList<QPointF> &plotData);

  private:
    Ui::Plot *ui;
    QChartView *chartView;

    QLineSeries *series;
    QChart *chart;
};

#endif // PLOT_H
