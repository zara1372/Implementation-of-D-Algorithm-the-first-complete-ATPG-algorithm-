#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <QDialog>

namespace Ui {
class benchmark;
}

class benchmark : public QDialog
{
    Q_OBJECT

public:
    explicit benchmark(QStringList in={},QStringList out={} ,QStringList sig={},QStringList left={} ,QStringList rigt={},QWidget *parent = nullptr);
    ~benchmark();

private:
    Ui::benchmark *ui;
    QStringList listin,listout,listsignal,listleft,listright;

};

#endif // BENCHMARK_H
