#include <queue>
#include <list>
#include <vector>
#include <set>
#include <iostream>
#include <cmath>
#include <string>
using namespace std;

#include "gridmap.h"
#include "objectmanager.h"
#include "globals.h"

GridMap::GridMap(){
}

GridMap::GridMap(vector<vector<ObjectRef> > omap){
	object_map = omap;
}

GridMap::GridMap(int w, int h, ObjectRef def){
	for(int y = 0; y < h; y++){
		vector<ObjectRef> temp;
		for(int x = 0; x < w; x++){
			temp.push_back(def);
		}
		object_map.push_back(temp);
	}
}

bool GridMap::PointIsValid(PointI a){
	return (a.x >= 0 && a.y >= 0 && a.y < (int)object_map.size() && a.x < (int)object_map[a.y].size());
}

bool GridMap::MoveObject(PointI a, PointI b){
	// Attempts to move object at point A to point B
	// If an object exists at point B, return false, else return true
	if(PointIsValid(a) && PointIsValid(b) && object_map[b.y][b.x] == 0){
		// Points are valid and no object is at B
		object_map[b.y][b.x] = object_map[a.y][a.x];
		object_map[a.y][a.x] = 0;
		return true;
	}
	return false;
}

bool GridMap::AddObject(ObjectRef ref, PointI pos){
	if(PointIsValid(pos) && object_map[pos.y][pos.x] == 0){
		object_map[pos.y][pos.x] = ref;
		return true;
	}
	return false;
}

set<ObjectRef> GridMap::GetDrawSet(PointF groundCorners[4]){
	set<ObjectRef> drawSet;
	int height = object_map.size();
	if(height <= 0){
		cout << "Map has zero height!" << endl;
		return drawSet;
	}
	int width = object_map[0].size();
	for(int i = 0; i < 4; i++){
		if(groundCorners[i].x < 0) groundCorners[i].x = 0;
		if(groundCorners[i].y < 0) groundCorners[i].y = 0;
		if(groundCorners[i].x > width) groundCorners[i].x = width;
		if(groundCorners[i].y > height) groundCorners[i].y = height;
	}
	return drawSet;
}

ObjectRef GridMap::GetObjectRefAt(PointI pos){
	if(PointIsValid(pos)){
		return object_map[pos.y][pos.x];
	}
	return 0;
}

list<PointI> GridMap::AStar(PointI a, PointI b){
	if(!PointIsValid(a) || !PointIsValid(b) || GetObjectRefAt(b) != 0){ // we're trying to move into a filled space
		return list<PointI>();
	}
	set<AStarPoint*,ASPComp> open;
	set<PointI> closed;
	set<AStarPoint*> toDelete;
	AStarPoint* start = new AStarPoint;
	toDelete.insert(start);
	start->point = a;
	start->parent = NULL;
	start->d = 0;
	start->rank = 0;
	open.insert(start);
	AStarPoint* cur = *(open.begin());
	while(!open.empty() && !(cur->point == b)){
		open.erase(open.begin());
		closed.insert(cur->point);
		for(int d = 0; d < 8; ++d){
			PointI temp;
			if(d&4){ // diags
				temp.x = cur->point.x + 2*(d&1)-1;
				temp.y = cur->point.y + (d&2)-1;
			} else { // verts and horis
				temp.x = cur->point.x + ((d&1)?((d&2)-1):0);
				temp.y = cur->point.y + ((d&1)?0:((d&2)-1));
			}
			if(PointIsValid(temp) && object_map[temp.y][temp.x] == 0 && closed.find(temp) == closed.end()){
				AStarPoint* n = new AStarPoint;
				toDelete.insert(n);
				n->point = temp;
				n->d = cur->d+1;
				int x = b.x-n->point.x;
				int y = b.y-n->point.y;
				x = x<0?-x:x;
				y = y<0?-y:y;
				// TODO some sort of cross product?
				n->rank = n->d + 2*(x>y?x:y) + ((d&4)>>2); // total distance traveled + manhattan distance w/ unit diagonals
				n->parent = cur;
				set<AStarPoint*,ASPComp>::iterator node;
				for(node = open.begin(); node != open.end(); ++node){
					if((*node)->point == n->point){ // they're the same point, we only want the better one
						if(n->rank < (*node)->rank){ // the new one is better than old
							open.erase(node);
							open.insert(n);
						}
						break;
					}
				}
				if(node == open.end()){
					open.insert(n);
				}
			}
		}
		if(!open.empty()){
			cur = *(open.begin());
		}
	}
	list<PointI> pathList;
	if(cur->point.x == b.x && cur->point.y == b.y){
		while(cur->parent != NULL){
			pathList.push_front(cur->point);
			cur = cur->parent;
		}
	}
	for(set<AStarPoint*>::iterator i = toDelete.begin(); i != toDelete.end(); ++i){
		delete *i;
	}
	return pathList;
}
