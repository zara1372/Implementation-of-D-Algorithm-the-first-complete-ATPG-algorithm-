#include "benchmark.h"
#include "ui_benchmark.h"
#include "QDebug"
benchmark::benchmark(QStringList in,QStringList out,QStringList sig,QStringList left ,QStringList rigt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::benchmark),
  listin(in),
  listout(out),
  listsignal(sig),
  listleft(left),
  listright(rigt)
{
    ui->setupUi(this);
    for (int i = 0; i < listleft.length(); ++i) {
        ui->textBrowser->append(listleft.at(i));
         ui->textBrowser_2->append(listright.at(i));
}
    for (int i = 0; i < listin.length(); ++i) {
        ui->textBrowser_3->append(listin.at(i));

    }
    for (int i = 0; i < listout.length(); ++i) {
        ui->textBrowser_4->append(listout.at(i));

    }

}

benchmark::~benchmark()
{
    delete ui;
}
