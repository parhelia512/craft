#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include <map>
#include <set>
using namespace std;

#include "object.h"
#include "unit.h"
#include "gridmap.h"

typedef unsigned int ObjectRef;

class ObjectManager{
public:
	ObjectManager();
	~ObjectManager();

	ObjectRef Add(Object* obj);
	ObjectRef Add(Unit *unit, GridMap *gridMap);
	void AddRef(ObjectRef ref);
	void RemoveRef(ObjectRef ref);
	void UpdateAll(int ticks);
	void DrawObjects(ModelManager *modelManager, TextureManager *textureManager, set<ObjectRef> refs);
	vector<vector<ObjectRef> > LoadObjectMap(string mapFileName);
private:
	map<ObjectRef,Object*> objects;
	map<ObjectRef,int> refcounts;
	int next_unused_ref;
};

#endif
