#ifndef ATLAS_H
#define ATLAS_H

#include <vector>
#include "assert.h"

class Atlas {

public:	

	//! Прямоугольник
	struct Rect
	{
		int x; 
		int y;
		int w;
		int h;

		Rect(int x, int y, int w, int h);
		void Set(int x, int y, int w, int h);
		Rect();
	};

	//! Массив областей
	typedef std::vector<Rect *> List;


	Atlas(int SurfaceMaxWidth, int SurfaceMaxHeight);
	~Atlas();

	//! Добавить область
	// Возвращает указатель на выделенную область
	Rect * Alloc(int w,int h);

private:

	//! Выделить область в контейнере, разбив область
	void Split(Rect *parent, int w, int h);

	//! Список областей верхнего уровняs
	List m_AreaRoots;
	List m_Busy;

};


#endif