#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTextStream"
#include "QMessageBox"
#include "QFile"
#include "QFileDialog"
#include "QDebug"
#include "benchmark.h"
#include "new"
#include "verilogNode.h"
#include "global.h"
#include "buildGraph.h"
#include "atpg.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <new>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cassert>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <cstring>

using namespace std;
using namespace  boost;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_read_clicked()
{
    QString  filters("vhdl File (*.vhd);; Text File (*.txt)");
        QString defult("vhdl File (*.vhd)");
       QString  path= QFileDialog::getOpenFileName(this, "Save file", QDir::currentPath(),
                                                       filters, &defult);
       string pt=path.toLocal8Bit().constData();
       string faultfile_tmp = pt+"_"+"fault.lst";
          char *faultfile = (char *)faultfile_tmp.c_str();
    ifstream  file;
    file.open(pt);
    if(!file.is_open()){
        QMessageBox::information(nullptr,"info","not file");
    }
    string textline;
    int num_nodes =0;
    while (file.good()) {
        getline(file,textline);
        string txtline_tmp = textline;

        if(txtline_tmp.empty()){
            continue;
        }
        num_nodes++;
    }
    file.close();
    node = new(nothrow) VerilogNode[num_nodes];
    BuildGraph graph1(node,(char *) pt.c_str() , linelist,
                line_to_node_map,
                line_to_net_map);
    graph1.CreateFanoutLineMap(line_to_net_map, linelist,
                      line_to_node_map,
                      line_to_fanoutline_map);
       graph1.CreateFaninLineMap(linelist, line_to_net_map,
                     line_to_faninline_map);
       graph1.CreateInputLineMap(node, linelist, line_to_node_map,
                     line_to_inputline_map);
       cout <<"#########################################################"<<endl;
       list<LineType> linelist_golden = linelist;
          list<LineType>::iterator i = linelist_golden.begin();
          int count = 1;
          int faultno = 1;
          for (; i != linelist_golden.end(); i++) {
             LineType curr_line = (*i);
             cout <<"---------------------------------------------------------"<<endl;
             cout <<"---------------------------------------------------------"<<endl;
             linelist = linelist_golden;
             if (PI == curr_line.type || PO == curr_line.type) continue;
                  cout <<"TG for line #"<<count<<" SA-1. Fault #"<<faultno++<<endl;
                  //---cout <<"Output line and corresponding input lines of the component"<<endl;
                  //---putLine(linelist, line_to_inputline_map, true);
                  Atpg atpg_sa1(linelist);
                  atpg_sa1.UpdateChanges (curr_line, linelist);
                  atpg_sa1.UpdateMaps (node, linelist,
                           line_to_node_map,
                           line_to_net_map,
                           line_to_fanoutline_map,
                           line_to_faninline_map,
                           line_to_inputline_map,
                           line_to_outputline_map);
                  //putLine (curr_line);
                  PutPiPatterns (linelist);
                  bool gen_status;
                  gen_status = atpg_sa1.GenerateTP (node, curr_line, linelist,
                                line_to_node_map,
                                line_to_net_map,
                                line_to_inputline_map,
                                line_to_outputline_map,
                                line_to_fanoutline_map,
                                ZERO);
                  if (gen_status) {
                       PutPiPatterns (linelist);
                       PutPiPatterns (linelist, (*i), "SA-1", faultfile);
                        }
                  else cout <<"ERROR: Cannot generate the test patterns for this fault."<<endl;
                       cout<<"Here !!!"<<endl;
                       cout <<"---------------------------------------------------------"<<endl;
                       cout <<"---------------------------------------------------------"<<endl;

                       linelist = linelist_golden;
                       curr_line = (*i);
                       cout <<"TG for line #"<<count<<" SA-0. Fault #"<<faultno++<<endl;
                       Atpg atpg_sa0(linelist);
                       atpg_sa0.UpdateChanges (curr_line, linelist);
                       atpg_sa0.UpdateMaps (node, linelist,
                                line_to_node_map,
                                line_to_net_map,
                                line_to_fanoutline_map,
                                line_to_faninline_map,
                                line_to_inputline_map,
                                line_to_outputline_map);
                       //putLine (curr_line);
                       PutPiPatterns (linelist);
                       gen_status = atpg_sa0.GenerateTP (node, curr_line, linelist,
                                     line_to_node_map,
                                     line_to_net_map,
                                     line_to_inputline_map,
                                     line_to_outputline_map,
                                     line_to_fanoutline_map,
                                     ONE);
                       if (gen_status) {
                          PutPiPatterns (linelist);
                          PutPiPatterns (linelist, (*i), "SA-0", faultfile);
                           }
                           else cout <<"ERROR: Cannot generate the test patterns for this fault."<<endl;
                           count++;
                        }
                        cout <<"#########################################################"<<endl;

                        //file1.close();


    QFile  fileall(path);

    if(!fileall.open(QIODevice::ReadOnly)){
        QMessageBox::information(nullptr,"info",fileall.errorString());
    }
//    QTextStream in(&file);
   QTextStream all(&fileall);
   ui->textBrowser->setText(all.readAll());


//    QString text,v;

//    while (!in.atEnd()) {
//    text = in.readLine();

//     bool a=text.contains(": in");
//     bool b=text.contains(": out");
//     bool c=text.contains("signal");
//     bool asi=text.contains("<=");
//     if(a){
//     listin<<text.split(":").first().simplified().remove(' ');
//     }
//     if(b){

//     listout<<text.split(":").first().simplified().remove(' ');
//     }
//     if(c){
//         text = text.simplified().remove(' ');

//     listsignal<<text.split(":").first().split("signal").last();
//     }
//     if(asi){
//     listleft<<text.split("<=").first().simplified().remove(' ');
//     listright<<text.split("<=").last();
//     }
//    }
// file.close();
    //qDebug()<<listleft<<listright<<listin<<listout<<listsignal;

}

void MainWindow::on_pushButton_clicked()
{
    if(listin.isEmpty()){
        QMessageBox::critical(nullptr,"error","not file");
    }
    else{
        benchmark *dia=new benchmark(listin,listout,listsignal,listleft ,listright);
        dia->setModal(1);
        dia->exec();
    }
}
