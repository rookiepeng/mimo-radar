#include "waveform.h"
#include "ui_waveform.h"

WaveForm::WaveForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaveForm)
{
    ui->setupUi(this);
}

WaveForm::~WaveForm()
{
    delete ui;
}
