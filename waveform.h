#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class WaveForm;
}

class WaveForm : public QWidget
{
    Q_OBJECT

  public:
    explicit WaveForm(QWidget *parent = 0);
    ~WaveForm();

  public slots:
    void updateWavform();

  private:
    Ui::WaveForm *ui;
    QChartView *chartView;

    QSplineSeries *series;
    QChart *chart;
};

#endif // WAVEFORM_H
