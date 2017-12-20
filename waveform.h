#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class WaveForm;
}

class WaveForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaveForm(QWidget *parent = 0);
    ~WaveForm();

private:
    Ui::WaveForm *ui;
};

#endif // WAVEFORM_H
