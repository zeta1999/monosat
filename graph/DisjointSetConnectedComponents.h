/*
 * DisjointSetConnectedComponents.h
 *
 *  Created on: 2014-01-18
 *      Author: sam
 */

#ifndef DISJOINTSETSCONNECTEDCOMPONENTS_H_
#define DISJOINTSETSCONNECTEDCOMPONENTS_H_



#include "mtl/Vec.h"
#include "mtl/Heap.h"
#include "DynamicGraph.h"
#include "core/Config.h"
#include "ConnectedComponents.h"
#include "DisjointSets.h"
#include <limits>
using namespace Minisat;



template<class Status,class EdgeStatus=DefaultEdgeStatus>
class DisjointSetsConnectedComponents:public ConnectedComponents{
public:

	DynamicGraph<EdgeStatus> & g;
	Status &  status;
	int last_modification;

	int last_addition;
	int last_deletion;
	int history_qhead;

	int last_history_clear;
	bool hasParents;
	int INF;
	DisjointSets sets;

	vec<int> q;
	vec<int> check;
	const int reportPolarity;

	//vec<char> old_seen;


	struct DefaultReachStatus{
			vec<bool> stat;
				void setReachable(int u, bool reachable){
					stat.growTo(u+1);
					stat[u]=reachable;
				}
				bool isReachable(int u) const{
					return stat[u];
				}
				DefaultReachStatus(){}
			};

public:


	DisjointSetsConnectedComponents(DynamicGraph<EdgeStatus> & graph, Status & _status, int _reportPolarity=0 ):g(graph), status(_status), last_modification(-1),last_addition(-1),last_deletion(-1),history_qhead(0),last_history_clear(0),INF(0),reportPolarity(_reportPolarity){
		marked=false;
		mod_percentage=0.2;
		stats_full_updates=0;
		stats_fast_updates=0;
		stats_skip_deletes=0;
		stats_skipped_updates=0;
		stats_full_update_time=0;
		stats_fast_update_time=0;
		stats_num_skipable_deletions=0;
		stats_fast_failed_updates=0;
		hasParents=false;
	}

	void setNodes(int n){
		q.capacity(n);
		check.capacity(n);

		INF=std::numeric_limits<int>::max();
		sets.AddElements(n);

	}

	void update( ){
		static int iteration = 0;
		int local_it = ++iteration ;

		if(last_modification>0 && g.modifications==last_modification){
			stats_skipped_updates++;
			return;
		}
		stats_full_updates++;
		double startdupdatetime = cpuTime();
		if(last_deletion==g.deletions){
			stats_num_skipable_deletions++;
		}
		hasParents=false;
		sets.Reset();
		setNodes(g.nodes);




		for(int i = 0;i<g.edges;i++){
			if(g.edgeEnabled(i)){
				int u = g.all_edges[i].from;
				int v = g.all_edges[i].to;
				sets.UnionElements(u,v);
			}
		}



		status.setComponents(sets.NumSets());



		last_modification=g.modifications;
		last_deletion = g.deletions;
		last_addition=g.additions;

		history_qhead=g.history.size();
		last_history_clear=g.historyclears;



		stats_full_update_time+=cpuTime()-startdupdatetime;;
	}

	bool connected(int from, int to){
		update();
		return sets.FindSet(from)==sets.FindSet(to);
	}


	 int numComponents(){
		 update();
		 return sets.NumSets();
	 }
	int getComponent(int node){
		update();
		return sets.FindSet(node);
	}
	int getElement(int set){
		update();
		return sets.GetElement(set);
	}

	bool dbg_uptodate(){
		return true;
	};


};


#endif /* DISJOINTSETCONNECTEDCOMPONENTS_H_ */