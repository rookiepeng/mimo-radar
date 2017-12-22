#include "waveform.h"
#include "ui_waveform.h"

WaveForm::WaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveForm)
{
    ui->setupUi(this);

    //![1]
    series = new QSplineSeries();
    series->setName("spline");
    //![1]

    //![2]
    series->append(0, 0.1);
    series->append(2, 0.4);
    series->append(3, 1);
    series->append(7, 1.2);
    series->append(10, 0.5);
    //*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    //![2]

    //![3]
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Beat Signal");
    chart->createDefaultAxes();
    chart->axisY()->setRange(0, 1.5);
    chart->axisX()->setRange(0, 30);
    //![3]

    //![4]
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    //ui->mainLayout->addWidget(chartView,0,0);
    ui->mainLayout->insertWidget(1,chartView);
}

WaveForm::~WaveForm()
{
    delete ui;
}

void WaveForm::updateWavform()
{
    *series << QPointF(22, 8);
    //series->clear();
}
