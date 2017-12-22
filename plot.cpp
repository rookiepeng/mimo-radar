#include "plot.h"
#include "ui_plot.h"

Plot::Plot(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Plot)
{
    ui->setupUi(this);

    //![1]
    series = new QLineSeries();
    series->setName("beatSignal");
    //![1]

    //![2]
    //series->append(0, 0.1);
    //*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
    //![2]

    //![3]
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Beat Signal");
    chart->createDefaultAxes();
    chart->axisY()->setRange(0, 1.5);
    chart->axisY()->setTitleText("Voltage (V)");
    chart->axisX()->setRange(0, 40);
    chart->axisX()->setTitleText("Time (ms)");
    //![3]

    //![4]
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    //![4]

    //![5]
    //ui->mainLayout->addWidget(chartView,0,0);
    ui->mainLayout->insertWidget(0, chartView); // (position, widget)
    //![5]
}

Plot::~Plot()
{
    delete ui;
}

void Plot::updatePlot(const QVector<float> &time, const QVector<float> &data)
{
    //*series << QPointF(5, 0.5);
    //*series << QPointF(6, 0.5);
    series->clear();
    //series->append(time,data);
    for(quint16 i=0; i<data.size();i++)
    {
        series->append(time.at(i),data.at(i));
    }
}

void Plot::updatePlot(const QList<QPointF> &plotData)
{
    //*series << QPointF(5, 0.5);
    //*series << QPointF(6, 0.5);
    series->clear();
    series->append(plotData);
}
