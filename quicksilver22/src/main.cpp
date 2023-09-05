#include "../include/Bench.h"
#include <iostream>
#include "../include/rss.h"
#include "../include/SimpleGraph.h"
#include "../include/Graph.h"
#include "vector"
#include "QueryParser.h"
#include <iostream>
#include <string.h>
#include <Query.h>
#include <map>
using namespace std;

struct Syn{
    int out=0;
    int in=0;
    int path=0;
    int pairs=0;
};



int main(int argc, char *argv[]) {
    /**
     * Creation of Synopsis Array for easy lookup of No.of.Source,
     * No.of.In, No.of.Path and No.of Unique Source and No.of.Unique Target.
     * 
     * @param array stores theses values as integer
     * @param array[0] stores No.of.Source
     * @param array[1] stores No.of.In
     * @param array[2] stores No.of.Path
     * @param array[3] stores No.of Unique Source
     * @param array[4] stores No.of.Unique Target
     * 
     * @param g is the graph object
     * 
     * use the max value of array[3] and array[4] for generating the number of
     *  unique (source,target) vertex pairs
     * 
    */
 /*   SimpleGraph g;
    vector<vector<int>> syn1;
    int array[36][5]={0};
    g.readFromContiguousFile("../workload/real/1/graph.nt");
    for(int i=0; i<g.getNoVertices();i++)
    {
        std::map<int,int> freq_fwd;
        std::map<int,int> freq_rev;
        vector<pair<uint32_t,uint32_t>> temp= g.adj[i];
        for(int j=0; j<g.adj[i].size();j++)
        {

            array[g.adj[i][j].first][0]+=1;
            array[g.adj[i][j].first][2]+=1;
            if (freq_fwd.find(g.adj[i][j].first)==freq_fwd.end())
            {
                array[g.adj[i][j].first][3]+=1;
                freq_fwd[g.adj[i][j].first]=1;
            }
    
        }
        //vector<pair<uint32_t,uint32_t>> temprev= g.reverse_adj[i];
        for(int k=0;k<g.reverse_adj[i].size();k++)
        {
            
            array[g.reverse_adj[i][k].first][1]+=1;
            array[g.reverse_adj[i][k].first][2]+=1;
           if (freq_rev.find(g.reverse_adj[i][k].first)==freq_rev.end())
            {
                array[g.reverse_adj[i][k].first][4]+=1;
                freq_rev[g.reverse_adj[i][k].first]=1;
            }
        }
    }

}*/

    

   
     if(argc < 3) {
        std::cout << "Usage: quicksilver <graphFile> <queriesFile>" << std::endl;
        return 0;
    }

    // args
    std::string graphFile {argv[1]};
    std::string queriesFile {argv[2]};

    estimatorBench(graphFile, queriesFile);

    return 0; }
