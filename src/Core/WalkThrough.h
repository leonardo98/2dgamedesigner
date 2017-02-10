#ifndef WALKTHROUGH
#define WALKTHROUGH

#include "ogl/fpoint.h"
#include <QVector>

struct PointData
{
	char someData;
	FPoint pos;
};

class WalkThrough
{
public:
	WalkThrough();
	void Clear();
	bool Init();
	void Render();
private:
	bool m_failed;
	QVector<PointData> _recording;
};

#endif//WALKTHROUGH
