#include <queue>
#include <stack>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;

#include "gridmap.h"
#include "objectmanager.h"
#include "globals.h"

GridMap::GridMap(int w, int h, ObjectRef def){
	width = w;
	height = h;
	object_map = new ObjectRef[w*h];
	for(int i = 0; i < w*h; i++){
		if(i%6 && (i%11 || i > 790)){
			object_map[i] = def;
		} else {
			object_map[i] = 1;
		}
	}
}

GridMap::~GridMap(){
	if(object_map != NULL){
		delete[] object_map;
	}
}

void GridMap::WriteOut(){
	ofstream of("test.ppm");
	of << "P3" << endl;
	of << width << " " << height << endl;
	of << "255" << endl;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			switch(object_map[y*width+x]){
				case 0:
					of << "0 0 0" << endl;
					break;
				case 1:
					of << "255 255 255" << endl;
					break;
				case 2:
					of << "0 255 0" << endl;
					break;
				case 3:
					of << "0 255 255" << endl;
					break;
			}
		}
	}
	of.close();
}

GridMap::LoadMap(string mapname)
{
	fstream mapfile;
	mapfile.open(mapname , fstream::in);

	int width, height;

	mapfile >> width;
	mapfile >> height;
	
	object_map = new int* [height];

	for (int i = 0; i < height; i++)
	{
		object_map[i] = new int [width];

		for (int j = 0; j < width; j++)
		{
			string s;
			mapfile >> s;

			switch(s[0])
			{
				case '.': //walkable
					break;
				case '#': //non-walkable
					break;
				case '^': //trees
					break;
				case '*': //gold
					break;
				case '1': //player1 start
					break;
				case '2': //player2 start
					break;
				default:
					break;
			}
		}
	}
	mapfile.close();
}

stack<PointI> GridMap::AStar(PointI a, PointI b){
	priority_queue<AStarPoint*,vector<AStarPoint*>,ASPComp> open;
	set<PointI> closed;
	vector<AStarPoint*> toDelete;
	AStarPoint* start = new AStarPoint;
	toDelete.push_back(start);
	start->point = a;
	start->parent = NULL;
	start->d = 0;
	start->rank = 0;
	open.push(start);
	AStarPoint* cur = open.top();
	while(!open.empty() && (cur->point.x != b.x || cur->point.y != b.y)){
		open.pop();
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
			if(temp.x >= 0 && temp.x < width && temp.y >= 0 && temp.y < height && object_map[temp.y*width+temp.x] == 0 && closed.find(temp) == closed.end()){
				AStarPoint* n = new AStarPoint;
				toDelete.push_back(n);
				n->point = temp;
				n->d = cur->d+1;
				int x = b.x-n->point.x;
				int y = b.y-n->point.y;
				x = x<0?-x:x;
				y = y<0?-y:y;
				n->rank = n->d + ((x>y)?x:y); // total distance traveled + manhattan distance w/ unit diagonals (nice heuristic)
				n->parent = cur;
				//cout << temp.x << " " << temp.y << " " << n->rank << endl;
				open.push(n);
			}
		}
		if(!open.empty()){
			cur = open.top();
		}
	}
	stack<PointI> pathStack;
	if(cur->point.x == b.x && cur->point.y == b.y){
		while(cur->parent != NULL){
			pathStack.push(cur->point);
			//object_map[cur->point.y*width+cur->point.x] = 2; // draw on our map, remove me!
			cur = cur->parent;
		}
	}
	for(vector<AStarPoint*>::iterator i = toDelete.begin(); i != toDelete.end(); ++i){
		/*if(object_map[(*i)->point.y*width+(*i)->point.x] != 2){
			object_map[(*i)->point.y*width+(*i)->point.x] = 3;
		}*/
		delete *i;
	}
	return pathStack;
}
