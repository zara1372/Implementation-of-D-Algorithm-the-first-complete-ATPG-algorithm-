#ifndef BUILDGRAPH_H
#define BUILDGRAPH_H


#include <iostream>
#include <new>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include "verilogLineParser.h"
#include "verilogNode.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <boost/config.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>
#include "faultLine.h"
#include <map>
#include <list>
#include "global.h"

using namespace std;
using namespace boost;




typedef adjacency_list<vecS, vecS, directedS,
               property<vertex_distance_t, int>,
               property<edge_weight_t, int> > graph_t;

class BuildGraph {
public:
   void ParseInputFile ();
   void InitializeNodes ();
   //void ConnectNodes ();
   void ConnectNodes (const VerilogNode *);
   void ConnectNodes (VerilogNode *, int);
   void ConnectNodes (VerilogNode *, list <LineType> &,
              map <LineType, int> &);

   void CreateInputLineMap (const VerilogNode *, list <LineType> &);

   void CreateInputLineMap(const VerilogNode *,
               map<LineType,string>,
               const list <LineType>,
               map<LineType,int>,
               map<LineType,LineList>&);

   void CreateInputLineMap(const VerilogNode *,
               const list <LineType>,
               map<LineType,int>,
               map<LineType,LineList>&);

   void CreateFanoutLineMap(map<LineType,string>,
                const list <LineType>,
                map<LineType,int>,
                map<LineType,LineList>&);
   void CreateFaninLineMap(const list<LineType>,
               map<LineType,string>,
               map<LineType,LineType> &);

   ifstream mfile;
   int mnum_nodes;
   int mnum_edges;

   // There is NO explicit graph structure as of nw.
   // Purpose acheived through maps and lists.
   //adjacency_list<> mgraph; //default directedS
   //graph_t mgraph;

   BuildGraph (VerilogNode*, char*);
   BuildGraph (VerilogNode *, char* ,
           list<LineType> &, map<LineType,int> &);
   BuildGraph (VerilogNode *, char* ,
           list<LineType>&, map<LineType,int>&, map<LineType,string>&);

   //BuildGraph (VerilogNode*, char*);
   ~BuildGraph (){}
   void FindPath(VerilogNode *, int, int);
private:
};
BuildGraph::BuildGraph (VerilogNode *rnode, char* filename,
            list<LineType> &linelist,
            map<LineType,int> &line_to_node_map,
            map<LineType,string> &line_to_net_map){

   ifstream file1;
   string txtline;
   file1.open(filename);
   if (!file1.is_open()) {
      cout<<"Cannot open input file"<<endl; assert(0);
   }

   mnum_nodes = 0;
   while (file1.good()){ // get the number of txtlines (nodes in the graph)
      getline (file1, txtline);
      string txtline_tmp = txtline;
      //txtline_tmp.erase(
      //remove_if(txtline_tmp.begin(), txtline_tmp.end(), isspace),
      //txtline_tmp.end()); //Check for all spaces.
      if (txtline_tmp.empty()) continue; //Check for blank txtline.
      mnum_nodes ++;
   }
   file1.close();
   TOTAL_NODES = mnum_nodes;

   int node_num = 0;
   file1.open(filename);
   while (node_num<mnum_nodes) {
      getline (file1, txtline);
      string txtline_tmp = txtline;
      //txtline_tmp.erase();
      //remove_if(txtline_tmp.begin(), txtline_tmp.end(), isspace),
      //txtline_tmp.end()); //Check for all spaces.
      if (txtline_tmp.empty()) continue; //Check for blank txtline.

      //Parse the txtline and save the contents in node.
      VerilogLineParser txtline1(txtline, rnode[node_num],
              linelist, line_to_node_map, line_to_net_map, node_num);
      node_num++;
   }
   file1.close();
}
void BuildGraph::CreateFanoutLineMap(map<LineType,string> line_to_net_map,
                     const list <LineType> linelist,
                     map<LineType,int> line_to_node_map,
                     map<LineType,LineList>& line_to_fanoutline_map){

   list<LineType> linelist_i = linelist; // for the first loop.
   list<LineType> linelist_j = linelist; // for the second loop.
   const list<LineType>::size_type linelist_size = linelist.size();
   list<LineType>::size_type i, j;
   LineList linelist_tmp;

   for (i=0; i<linelist_size; i++) {
      LineType curr_line_i = linelist_i.front();
      linelist_i.pop_front();
      //putLine(curr_line_i);
      if (curr_line_i.direction == IN) continue;
      //if (curr_line_i.type == PI) continue; // PI is also a driver.
      //if (curr_line_i.type == PI||IN) continue; //continue if its OUT/PO, only.
      linelist_tmp.mlines.clear();
      //putLine(curr_line_i);
      //cout <<"Here-1 "<<curr_line_i.name<<endl;
      linelist_j = linelist; // Initialize again.
      for (j=0; j<linelist_size; j++) {
     //cout <<"loop j"<<endl;
     LineType curr_line_j = linelist_j.front();
     linelist_j.pop_front();
     //cout <<"Pop OK"<<endl;
     //if (curr_line_j.type == PO||OUT) continue; //skip PO/OUT
     if (curr_line_j.direction == OUT) continue;
     //if (curr_line_j.type == PO) continue; // PO is also a fanout.
     //cout <<"   Here-2 "<<curr_line_j.name<<endl;
     map<LineType,string>::iterator p1,p2;
     p1 = line_to_net_map.find(curr_line_i);
     p2 = line_to_net_map.find(curr_line_j);
     //putLine(curr_line_j);
     if (p1->second == p2->second) {
        //cout <<curr_line_i.name<<" connected to "<<curr_line_j.name<<endl;
        linelist_tmp.mlines.push_front(curr_line_j);
     }
      }
      line_to_fanoutline_map.insert (make_pair(curr_line_i, linelist_tmp));
   }

}
void BuildGraph::CreateFaninLineMap(const list<LineType>linelist,
                    map<LineType,string> line_to_net_map,
                    map<LineType,LineType> &line_to_faninline_map){

   list<LineType> linelist_i = linelist, linelist_tmp = linelist;
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();


   for (i=0; i<linelist_size; i++) {
      LineType curr_line = linelist_i.front();
      linelist_i.pop_front();

      if (curr_line.direction == OUT) continue; //No driver line for OUT
      if (curr_line.type == PI) continue; //No driver line for PI
      LineType driver_line =
     curr_line.DriverLine (line_to_net_map, linelist_tmp);

      line_to_faninline_map.insert (make_pair(curr_line, driver_line));
   }
}
void BuildGraph::CreateInputLineMap(const VerilogNode *rnode,
                    const list <LineType> linelist,
                    map<LineType,int> line_to_node_map,
                    map<LineType,LineList>& line_to_inputline_map){

   list<LineType> linelist_i=linelist, linelist_j=linelist;
   LineList linelist_tmp;
   list<LineType>::size_type i,j;
   const list<LineType>::size_type linelist_size=linelist.size();
   LineType curr_line_i, curr_line_j;

   //putLine (linelist_i);
   for (i=0; i<linelist_size; i++) {
      curr_line_i = linelist_i.front();
      linelist_i.pop_front();
      if (curr_line_i.type == PI) continue;
      if (curr_line_i.direction == IN) continue; // Only OUT lines have inputlines.

      //cout <<"here"<<endl;
      //putLine(curr_line_i);
      map<LineType,int>::iterator p = line_to_node_map.find(curr_line_i);
      if (p == line_to_node_map.end()) cout<<" [ERROR: NO MATCHING NODE !]"<<endl;
      else {
     int nodeID = p->second;
     list<string> input_terms_list = rnode[nodeID].input_list;
     list<string>::size_type i_terms=0, no_of_terms = input_terms_list.size();
     linelist_tmp.mlines.clear();
     //cout <<"i_terms="<<i_terms<<" no_of_terms="<<no_of_terms<<endl;
     for (i_terms=0; i_terms<no_of_terms; i_terms++) {
        //cout <<"here"<<endl;
        string input_line_name = rnode[nodeID].component_name+"_"+
           input_terms_list.front();
        //cout <<input_line_name<<endl;
        input_terms_list.pop_front();
        //cout <<input_line_name<<endl;

        // find input_line_name in linelist and take tht line.
        linelist_j = linelist; // Initialize linelist_j again.
        for (j=0; j<linelist_size; j++) {
           LineType curr_line_j = linelist_j.front();
           linelist_j.pop_front();
           if (curr_line_j.name == input_line_name) {
          //cout <<curr_line_j.name<<" "<<curr_line_i.name<<endl;
          linelist_tmp.mlines.push_front(curr_line_j);
           }
        }
     }
      }
      //cout <<"input_line_list size ="<<(int)linelist_tmp.mlines.size()
      //<<" for outline "<<curr_line_i.name<<endl;
      line_to_inputline_map.insert(make_pair(curr_line_i, linelist_tmp));
   }
}
#endif // BUILDGRAPH_H
