#include "WalkThrough.h"
#include <stdio.h>
#include <string>
#include "Math.h"
#include "ogl/render.h"
#include "Core.h"
#include "../Gamee/TileEditor.h"

WalkThrough::WalkThrough()
{
	Clear();
}

void WalkThrough::Clear()
{
	m_failed = true;
}

bool WalkThrough::Init()
{
	unsigned int record(0);
	FILE *f = NULL;
	do
	{
		std::string fileName("walkthrough" + Math::IntToStr(record) + "-");
		fileName += (TileEditor::Instance()->GetCurrentLevel() + ".bin");
		f = fopen((Core::workDir + fileName).c_str(), "rb");
		++record;
	}
	while (f == NULL && record <= 9);
	
	if (!f)
	{
		m_failed = true;
		return false;
	}

	fseek(f, 0L, SEEK_END);
	unsigned int size = ftell(f) / sizeof(PointData);
	fseek(f, 0L, SEEK_SET);

	_recording.resize(size);
	fread(&(_recording[0]), sizeof(PointData), size, f);
	fclose(f);

	m_failed = false;
	return true;
}

void WalkThrough::Render()
{
	if (m_failed)
	{
		return;
	}
	Render::PushMatrix();
    Render::MatrixScale(60.f, 60.f);
	for (int i = _recording.size() - 2; i >= 0 ; --i)
	{
		Render::Line(_recording[i].pos.x, _recording[i].pos.y
							, _recording[i + 1].pos.x, _recording[i + 1].pos.y
							, 0xFFFC00F9);
	}
	Render::PopMatrix();
}
