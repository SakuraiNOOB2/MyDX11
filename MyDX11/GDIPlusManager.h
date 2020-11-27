#pragma once

#include "myWin.h"


class GDIPlusManager {

public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;

};