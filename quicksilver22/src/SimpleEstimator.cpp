#include "../include/SimpleGraph.h"
#include "../include/SimpleEstimator.h"
#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <QueryParser.h>
#include "vector"
#include <math.h>
using namespace std;


std::map<int,float> map;
int **init;
SimpleEstimator::SimpleEstimator(std::shared_ptr<SimpleGraph> &g){
    // works only with SimpleGraph
    graph=g;
}

vector<int> nodes_middle(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /**
    *@return a vector of all the nodes in the middle of a path of l1/l2 e.g the end nodes of label l1 and the start nodes of label l2.
    */
    vector<int> res;
    vector<int> temp;
    std::map<int,bool> curr;
    for(int i=0; i<n_vertices; i++)
        curr[i] = false;
    if(!label1.reverse){
        for(int i=0; i<n_vertices; i++){
            for(int j=0; j<rev_adj[i].size(); j++){
                if(rev_adj[i][j].first==label1.label && !curr[rev_adj[i][j].second]){
                    temp.push_back(i);
                    curr[rev_adj[i][j].second] = true;
                    break;
                }
            }
        }
    }
    else if(label1.reverse){
        for(int i=0; i<n_vertices; i++){
            for(int j=0; j<rev_adj[i].size(); j++){
                if(adj[i][j].first==label1.label && !curr[adj[i][j].second]){
                    temp.push_back(i);
                    curr[adj[i][j].second] = true;
                    break;
                }
            }
        }
    }
    if(!label2.reverse){
        for(int i : temp){
            for(int j=0; j<adj[i].size(); j++){
                if(adj[i][j].first==label2.label)
                    res.push_back(i);
            }
        }
    }
    else if(label2.reverse){
        for(int i : temp){
            for(int j=0; j<rev_adj[i].size(); j++){
                if(rev_adj[i][j].first==label2.label)
                    res.push_back(i);
            }
        }
    }
    return res;
}

float syn1(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj, int** init){
    /**
    *@return the formula of synopsis 1 for a join path of labels l1/l2
    */
    int n_join = nodes_middle(label1, label2, n_vertices, adj, rev_adj).size();
    return n_join*(float(init[label1.label][2])/float(init[label1.label][4]))*(float(init[label2.label][2])/float(init[label2.label][3]));
}

int d_source(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /**
    *@return the number of start nodes of the path labeled l1/l2.
    */
    vector<int> nodes_in_middle = nodes_middle(label1, label2, n_vertices, adj, rev_adj);
    std::map<int,bool> ocurr;
    int res = 0;
    for(int i=0; i<n_vertices; i++){
        ocurr[i] = false;
    }
    for(int i: nodes_in_middle){
        for(int j=0; j<rev_adj[i].size(); j++){
            if(rev_adj[i][j].first==label1.label && !ocurr[rev_adj[i][j].second])
                res+=1;
            break;
        }
    }
    return res;

}

int out_path(LabelDir label, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, int source){
    /**
    *@return the number of path labeled l for a specific node.
    *If the source node is not specified e.g source = -1, then the function returns the total number of path labeled l1.
    */
    int res = 0;
    if(source == -1){
        for(int i=0; i<n_vertices; i++){
            for(int j=0; j<adj[i].size(); j++){
                if(adj[i][j].first==label.label)
                    res+=1;
            }
        }
    }
    else{
        for(int j=0; j<adj[source].size(); j++){
            if(adj[source][j].first==label.label)
                res+=1;
        }
    }
    return res;
}

int n_paths(int node, LabelDir label, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj){
    /**
     *@return the total number of path labeled label from a specific node.
     */
    int res = 0;
    for(int j=0;j<adj[node].size(); j++){
        if(adj[node][j].first==label.label){
            res+=1;
        }
    }
    return res;
}

int one(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /***
    *@return #one from synopsis 2 e.g the number of paths labeled l1 from nodes in "out" to nodes in "middle".
    */
    vector<int> nodes_in_middle = nodes_middle(label1, label2, n_vertices, adj, rev_adj);
    int res = 0;
    for(int i:nodes_in_middle){
        res+=n_paths(i, label1, rev_adj);
    }
    return res;
}

int two(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /***
    *@return #two in synopsis 2 e.g the number of paths labeled l2 from nodes in "middle" to nodes in "in".
    */
    vector<int> nodes_in_middle = nodes_middle(label1, label2, n_vertices, adj, rev_adj);
    int res = 0;
    for(int i: nodes_in_middle){
        res+=n_paths(i, label2, adj);
    }
    return res;
}

int in(LabelDir label1, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /***
    *@return the number of nodes in G which have incoming path labeled with l.
    */
    int res = 0;
    std::map<int,bool> ocurr;
    for(int i=0; i<n_vertices; i++){
        ocurr[i]=false;
    }
    for(int i=0; i<n_vertices; i++)
        for(int j=0; j<rev_adj[i].size(); j++){
            if(rev_adj[i][j].first==label1.label && !ocurr[rev_adj[i][j].second]){
                ocurr[rev_adj[i][j].second] = true;
                res+=1;
                break;
            }
        }
    return res;
}

int syn_in(LabelDir label1, LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /***
    *@return the number of nodes in G which have incoming path labeled with l1/l2.
    */
    int res = 0;
    std::map<int,bool> ocurr;
    for(int i=0; i<n_vertices; i++){
        ocurr[i]=false;
    }
    vector<int> nodes_in_middle = nodes_middle(label1, label2, n_vertices, adj, rev_adj);
    for (int i: nodes_in_middle){
        for(int j=0; j<adj[i].size(); j++){
            if(rev_adj[i][j].first==label2.label && ocurr[rev_adj[i][j].second]){
                res+=1;
                ocurr[rev_adj[i][j].second] = true;
                break;
            }
        }
    }
    return res;
}

int out(LabelDir label2, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj){
    /**
     *@return #out from synopsis 1 e.g the number of nodes in G which have outgoing edge labeled with label2.
     */
    int res = 0;
    std::map<int,bool> ocurr;
    for(int i=0; i<n_vertices; i++){
        ocurr[i]=false;
    }
    for(int i=0; i<n_vertices; i++)
        for(int j=0; j<rev_adj[i].size(); j++){
            if(rev_adj[i][j].first==label2.label && !ocurr[rev_adj[i][j].second]){
                ocurr[rev_adj[i][j].second] = true;
                res+=1;
                break;
            }
        }
    cout << "out reverse" << endl;
    return res;
}

int union_source(PathEntry path_entry, int** init){
    /**
     *Given a path, the union is computed by adding all the possible path by the current label in the path.
     *@return the number of path for an union path.
     */
    int res = 0;
    for(int i=0; i<path_entry.labels.size(); i++){
        res+=init[path_entry.labels[i].label][3];
    }
    return res;
}

int union_target(PathEntry path_entry, int** init){
    /**
     *Given a path, the union is computed by adding all the possible path by the current label in the path.
     *@return the number of path for an union path.
     */
    int res = 0;
    for(int i=0; i<path_entry.labels.size(); i++){
        res+=init[path_entry.labels[i].label][4];
    }
    return res;
}


float concatenation(vector<PathEntry> path, int source, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj, int** init){
    /**
     *
     *
     */
    cout << "hi1" << endl;
    int _out;
    if(path.size()==2){
        cout << "hi2" << endl;
        int n_join = nodes_middle(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj).size();
        if(source>n_vertices)
            _out = out_path(path[0].labels[0], n_vertices, adj, -1);
        else
            _out = out_path(path[0].labels[0], n_vertices, adj, source);
        cout << "out = " << out << endl;
        cout << "two = " << two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj) << endl;
        cout << "in = " << in(path[0].labels[0], n_vertices, adj) << endl;
        return _out*(float(two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj))/float(in(path[0].labels[0], n_vertices, adj)));
        // cout << "n_join = " << n_join << endl;
        // cout << "syn1 right value = " << float(init[path[1].labels[0].label][2])/float(init[path[1].labels[0].label][3])<< endl;
        // cout << "syn1 left value = " <<float(init[path[0].labels[0].label][2])/float(init[path[0].labels[0].label][4]) << endl;
        // return n_join*(float(init[path[0].labels[0].label][2])/float(init[path[0].labels[0].label][4]))*(float(init[path[1].labels[0].label][2])/float(init[path[1].labels[0].label][3]));
    }
    else{
        int size_path = path.size();
        LabelDir label1 = path[size_path-2].labels[0];
        LabelDir label2 = path[size_path-1].labels[0];
        path.pop_back();
        int middle = nodes_middle(label1, label2, n_vertices, adj, rev_adj).size();
        int distinct_source = d_source(label1, label2, n_vertices, adj, rev_adj);
        float syn = syn1(label1, label2, n_vertices, adj, rev_adj, init);
        //return concatenation(path, n_vertices, adj, rev_adj, init)*(float(middle)*syn)/(float(distinct_source)*float(incoming));
        //return concatenation(path, n_vertices, adj, rev_adj, init)*(float(two(label1, label2, n_vertices, adj))/float(in(label1, n_vertices, adj)));
        return 0;
    }
}

vector<float> concatenation_backward(vector<PathEntry> path, int source, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj, int** init){
    /**
     *Compute the concatenation using the synopsis 2 method. In this method the concatenation start by the end of the path, this is why it is "backward".
     *@return a vector where vector[0] is the number distinct source nodes of the path, vector[1] the number of possible path and vector[2] the distinct number of target nodes.
     */
    int count = 0;
    float avg = 0;
    float avg_source = 0;
    float avg_target = 0;
    float _out;
    PathEntry last_el = path[path.size()-1];
    bool verify = false;
    if(path.size() > 2)
        verify = true;
    while(path.size() > 2){
        int size_path = path.size();
        LabelDir label1 = path[size_path-2].labels[0];
        LabelDir label2 = path[size_path-1].labels[0];
        path.pop_back();
        int incoming = in(label1, n_vertices, rev_adj);
        int outgoing = out(label1, n_vertices, rev_adj);
        if(label1.reverse && label2.reverse)
            avg += float(one(label2, label1, n_vertices, adj, rev_adj))/float(outgoing);
        else if(label1.reverse){
            avg += 2*sqrt(outgoing*out(label2, n_vertices, rev_adj))*(init[label2.label][2]/pow(outgoing,2));
            cout << "hey" << endl;
        }
        else if(label2.reverse)
            avg += 2*sqrt(incoming*in(label2, n_vertices, rev_adj))*(init[label2.label][2]/pow(incoming,2));
        else{
            avg += float(two(label1, label2, n_vertices, adj, rev_adj))/float(in(label1, n_vertices, rev_adj));
            avg_source += float(nodes_middle(label1, label2, n_vertices, adj, rev_adj).size())/float(incoming);
            avg_target += float(syn_in(label1, label2, n_vertices, adj, rev_adj))/float(incoming);
        }
        count +=1;
    }
    cout << "after the while loop" << endl;
    avg = avg/count;
    avg_source = avg_source/count;
    avg_target = avg_target/count;
    if(source>n_vertices){
        _out = (float(out_path(path[0].labels[0], n_vertices, adj, -1))+float(out_path(last_el.labels[0], n_vertices, rev_adj, -1)))/2;
    }
    else
        _out = out_path(path[0].labels[0], n_vertices, adj, source);
    if(verify){
        if(path[0].labels[0].reverse && path[1].labels[0].reverse)
            return vector{float(0),_out*(float(one(path[0].labels[0], path[1].labels[0], n_vertices, adj,rev_adj))/float(out(path[0].labels[0], n_vertices, adj)))*avg,float(0)};
        else if(path[0].labels[0].reverse)
            return vector{float(0),float(_out*(2*sqrt(out(path[0].labels[0], n_vertices, rev_adj)*out(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(out(path[0].labels[0], n_vertices, rev_adj),2)))*avg), float(0)};
        else if(path[1].labels[0].reverse){
            cout << "reverse" << endl;
            return vector{float(0),float(_out*(2*sqrt(in(path[0].labels[0], n_vertices, rev_adj)*in(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(in(path[0].labels[0], n_vertices, rev_adj),2)))*avg),float(0)};
        }
        else
            return vector{avg_source*init[path[0].labels[0].label][3],_out*float(two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj))/float(in(path[0].labels[0], n_vertices, adj))*avg,avg_target*init[last_el.labels[0].label][4]};
    }
    else{
        if(path[0].labels[0].reverse && path[1].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]),_out*(float(one(path[0].labels[0], path[1].labels[0], n_vertices, adj,rev_adj))/float(out(path[0].labels[0], n_vertices, adj))),float(init[last_el.labels[0].label][4])};
        else if(path[0].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]),float(_out*(2*sqrt(out(path[0].labels[0], n_vertices, rev_adj)*out(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(out(path[0].labels[0], n_vertices, rev_adj),2)))),float(init[last_el.labels[0].label][4])};
        else if(path[1].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]), float(_out*(2*sqrt(in(path[0].labels[0], n_vertices, rev_adj)*in(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(in(path[0].labels[0], n_vertices, rev_adj),2)))),float(init[last_el.labels[0].label][4])};
        else{
            return vector{float(init[path[0].labels[0].label][3]), _out*float(two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj))/float(in(path[0].labels[0], n_vertices, adj)),float(init[last_el.labels[0].label][4])};
        }
    }
}

vector<float> concatenation_foward(vector<PathEntry> path, int target, int n_vertices, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> adj, std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rev_adj, int** init){
    /**
     *Compute the concatenation using the synopsis 2 method. In this method the concatenation start by the beginning of the path, this is why it is "forward". It is useful when the target node is known.
     *@return a vector where vector[0] is the number distinct source nodes of the path, vector[1] the number of possible path and vector[2] the distinct number of target nodes.
     */
    int count = 0;
    float avg = 0;
    float avg_source;
    float avg_target;
    int _out;
    PathEntry last_el = path[path.size()-1];
    bool verify = false;
    if(path.size() > 2)
        verify = true;
    while(path.size() > 2){
        int size_path = path.size();
        LabelDir label1 = path[0].labels[0];
        LabelDir label2 = path[1].labels[0];
        path.erase(path.begin());
        int incoming = in(label1, n_vertices, rev_adj);
        int outgoing;
        if(label1.reverse && label2.reverse)
            avg += float(one(label2, label1, n_vertices, adj, rev_adj))/float(out(label1, n_vertices, rev_adj));
        else if(label1.reverse){
            outgoing = out(label1, n_vertices, rev_adj);
            avg += 2*sqrt(outgoing*out(label2, n_vertices, rev_adj))*(init[label2.label][2]/pow(outgoing,2));
        }
        else if(label2.reverse)
            avg += 2*sqrt(incoming*in(label2, n_vertices, rev_adj))*(init[label2.label][2]/pow(incoming,2));
        else{
            avg += float(two(label1, label2, n_vertices, adj, rev_adj))/float(in(label1, n_vertices, rev_adj));
            avg_source += float(nodes_middle(label1, label2, n_vertices, adj, rev_adj).size())/float(incoming);
            avg_target += float(syn_in(label1, label2, n_vertices, adj, rev_adj))/float(incoming);
        }
        count +=1;
    }
    avg = avg/count;
    avg_source = avg_source/count;
    avg_target = avg_target/count;

    if(target>n_vertices){
        _out = out_path(path[1].labels[0], n_vertices, adj, -1);
    }
    else
        _out = out_path(path[1].labels[0], n_vertices, rev_adj, target);
    if(verify){
        if(path[0].labels[0].reverse && path[1].labels[0].reverse)
            return vector{float(0),_out*(float(one(path[0].labels[0], path[1].labels[0], n_vertices, adj,rev_adj))/float(out(path[0].labels[0], n_vertices, rev_adj)))*avg,float(0)};
        else if(path[0].labels[0].reverse)
            return vector{float(0),float(_out*(2*sqrt(out(path[0].labels[0], n_vertices, rev_adj)*out(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(out(path[0].labels[0], n_vertices, rev_adj),2)))*avg), float(0)};
        else if(path[1].labels[0].reverse)
            return vector{float(0),float(_out*(2*sqrt(in(path[0].labels[0], n_vertices, rev_adj)*in(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(in(path[0].labels[0], n_vertices, rev_adj),2)))*avg),float(0)};
        else
            return vector{avg_source*init[path[0].labels[0].label][3],_out*float(two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj))/float(in(path[0].labels[0], n_vertices, adj))*avg,avg_target*init[last_el.labels[0].label][4]};
    }
    else{
        cout << "hi" << endl;
        if(path[0].labels[0].reverse && path[1].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]),_out*(float(one(path[0].labels[0], path[1].labels[0], n_vertices, adj,rev_adj))/float(out(path[0].labels[0], n_vertices, adj))),float(init[last_el.labels[0].label][4])};
        else if(path[0].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]),float(_out*(2*sqrt(out(path[0].labels[0], n_vertices, rev_adj)*out(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(out(path[0].labels[0], n_vertices, rev_adj),2)))),float(init[last_el.labels[0].label][4])};
        else if(path[1].labels[0].reverse)
            return vector{float(init[path[0].labels[0].label][3]), float(_out*(2*sqrt(in(path[0].labels[0], n_vertices, rev_adj)*in(path[1].labels[0], n_vertices, rev_adj))*(init[path[1].labels[0].label][2]/pow(in(path[0].labels[0], n_vertices, rev_adj),2)))),float(init[last_el.labels[0].label][4])};
        else
            return vector{float(init[path[0].labels[0].label][3]), _out*float(two(path[0].labels[0], path[1].labels[0], n_vertices, adj, rev_adj))/float(in(path[0].labels[0], n_vertices, adj)),float(init[last_el.labels[0].label][4])};
    }
}

float sum_of_pairs(vector<float> n_paths){
    vector<float> res;
    float res_final;
    if (n_paths.size() == 1)
        return n_paths[0];
    for(size_t i=0; i<n_paths.size()-1; i+2){
        res.push_back((n_paths[i]+n_paths[i+1]+max(n_paths[i],n_paths[i+1]))/2);
    }
    if(n_paths.size()%2==0)
        return sum_of_pairs(res);
    else
        res_final = sum_of_pairs(res);
    return (res_final+n_paths[-1]+max(res_final,n_paths[-1]))/2;
}

float union_estimation(int **init, PathEntry path_entry){
    vector<float> res;
    for(int i=0; i<path_entry.labels.size(); i++)
        res.push_back(init[path_entry.labels[i].label][2]);
    return sum_of_pairs(res);
}

void SimpleEstimator::prepare() {

    // do your prep here

    vector<vector<int>> syn1;
    int n_vertices = graph->getNoVertices();
    int n_edges = graph->getNoDistinctEdges();
    init = new int*[n_edges];
    for(int i=0; i<n_edges; i++){
        init[i] = new int[5];
    }
    for(int i=0; i<n_vertices;i++)
    {
        std::map<int,int> freq_fwd;
        std::map<int,int> freq_rev;
        vector<pair<uint32_t,uint32_t>> temp= graph->adj[i];
        for(int j=0; j<graph->adj[i].size();j++)
        {
            init[graph->adj[i][j].first][0]+=1;
            init[graph->adj[i][j].first][2]+=1;
            if (freq_fwd.find(graph->adj[i][j].first)==freq_fwd.end())
            {
                init[graph->adj[i][j].first][3]+=1;
                freq_fwd[graph->adj[i][j].first]=1;
            }

        }
        //vector<pair<uint32_t,uint32_t>> temprev= graph->reverse_adj[i];
        for(int k=0;k<graph->reverse_adj[i].size();k++)
        {

            init[graph->reverse_adj[i][k].first][1]+=1;
            init[graph->reverse_adj[i][k].first][2]+=1;
            if (freq_rev.find(graph->reverse_adj[i][k].first)==freq_rev.end())
            {
                init[graph->reverse_adj[i][k].first][4]+=1;
                freq_rev[graph->reverse_adj[i][k].first]=1;
            }
        }
    }


}

cardStat SimpleEstimator::estimate(Triple &q) {

    // perform your estimation here
    vector<PathEntry> path = q.path;
    vector<float> est;
    float nodes_source;
    float nodes_target;
    int source = q.src;
    int target = q.trg;
    int _out = 0;
    if(path.size()>1){
        if(target<=graph->getNoVertices()){
            est = concatenation_foward(path, target, graph->getNoVertices(), graph->adj, graph->reverse_adj, init);
            nodes_source = est[0];
            nodes_target = 1;
        }
        else if(source<=graph->getNoVertices()){
            est = concatenation_backward(path, source, graph->getNoVertices(), graph->adj, graph->reverse_adj, init);
            nodes_source = 1;
            nodes_target = est[2];
        }
        else{
            est = concatenation_backward(path, source, graph->getNoVertices(), graph->adj, graph->reverse_adj, init);
            nodes_source = est[0];
            nodes_target = est[2];
        }
        return cardStat{uint32_t(nodes_source),uint32_t(est[1]),uint32_t(nodes_target)};
    }
    else
    if(source>graph->getNoVertices() && target<graph->getNoVertices()){
        for(int i=0; i<path[0].labels.size();i++)
            _out += out_path(path[0].labels[i], graph->getNoVertices(), graph->reverse_adj, target);
        return cardStat{uint32_t(union_source(path[0], init)),uint32_t(_out), 1};
    }
    else if(source < graph->getNoVertices() && target>graph->getNoVertices()){
        for(int i=0; i<path[0].labels.size();i++)
            _out += out_path(path[0].labels[i], graph->getNoVertices(), graph->adj, source);
        return cardStat{1,uint32_t(_out), uint32_t(union_target(path[0], init))};
    }
    else{
        for(int i=0; i<path[0].labels.size();i++)
            _out += out_path(path[0].labels[i], graph->getNoVertices(), graph->adj, -1);
        return cardStat{uint32_t(union_source(path[0], init)),uint32_t(_out),uint32_t(union_target(path[0], init))};
    }
// }
}
