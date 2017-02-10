// InputSystem.cpp: implementation of the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "InputSystem.h"
#include "Messager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


float InputSystem::DOUBLE_CLICK_TIME; // максимальное время на второй клик - если время больше - событие "аннулируется"
float InputSystem::LONG_TAP_EPS; // погрешность на случайные движения мыши во время нажатия, если движение более сильное - событие "аннулируется"
float InputSystem::LONG_TAP_TIME; // минимальное время на вызов события - если время меньше - событие "аннулируется"
float InputSystem::_timeCounter; // счетчик времени(отсчитывает время либо с последнего события нажатия или отпускания кнопки мыши
bool InputSystem::_longTap; // true - если ловим событие "Длинный Тап"
bool InputSystem::_doubleClick; // true - если ловим это событие "Двойной клик"
FPoint InputSystem::_longTapPos;
InputSystem *InputSystem::_locked;
std::set<int> InputSystem::_pressedKeys;

InputSystem::InputSystem()
{
	_listeners.push_back(this);
}

InputSystem::~InputSystem()
{
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != this; i++);
	if (i != _listeners.end()) {// нет в списке (уже удален)
		_listeners.erase(i);
	}
}

void InputSystem::OnMouseDown(const FPoint &mousePos) {}

void InputSystem::OnMouseUp() {}

void InputSystem::OnMouseMove(const FPoint &mousePos) {}

void InputSystem::OnLongTap(const FPoint &mousePos) {}

void InputSystem::OnDoubleClick(const FPoint &mousePos) {}

bool InputSystem::OnMouseWheel(int direction) {return false;}

bool InputSystem::OnKey(int key, bool bDown) {return false;}

void InputSystem::MouseDown(const FPoint &mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnMouseDown(mousePos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::LongTap() {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(_longTapPos)) {
			(*i)->OnLongTap(_longTapPos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::DoubleClick(const FPoint &mousePos) {
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->IsMouseOver(mousePos)) {
			(*i)->OnDoubleClick(mousePos);
			_locked = *i;
			return;
		}
	}
}

void InputSystem::MouseUp() {
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseUp();
	}
	_locked = NULL;
}

void InputSystem::Reset() {
	_locked = NULL;
}

void InputSystem::MouseMove(const FPoint &mousePos) {
	if (_locked != NULL) {
		_locked->OnMouseMove(mousePos);
		return;
	}
	for (Listeners::iterator i = _listeners.begin(), e = _listeners.end(); i != e; i++) {
		(*i)->OnMouseMove(mousePos);
	}
}

InputSystem::Listeners InputSystem::_listeners;

void InputSystem::RemoveFromList(InputSystem *listener) {
	Listeners::iterator i;
	for (i = _listeners.begin(); i != _listeners.end() && *i != listener; i++);
	if (i == _listeners.end()) {// нет в списке (уже удален)
		LOG("удален уже Listener или удаляемый не унаследован от InputSystem");
		exit(-4);
	}
	_listeners.erase(i);
}

void OnLongTap(int direction) {
}

void OnDoubleClick(int direction) {
}

void InputSystem::MouseWheel(int direction) {
	if (_locked != NULL) {
		_locked->OnMouseWheel(direction);
		return;
	}
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->OnMouseWheel(direction)) {
			return;
		}
	}
}

bool InputSystem::CheckForEvent(float dt) {
	_timeCounter += dt;
	_doubleClick &= (_timeCounter < DOUBLE_CLICK_TIME);
	//hgeInputEvent event;
	//while (Render::GetDC()->Input_GetEvent(&event)) {
	//	if (event.type == INPUT_MBUTTONDOWN && event.key == HGEK_LBUTTON) {
	//		_longTapPos = FPoint(event.x, event.y);
	//		MouseDown(_longTapPos);
	//		if (_doubleClick) {
	//			DoubleClick(_longTapPos);
	//			_doubleClick = false;
	//		}
	//		_timeCounter = dt / 2.f;
	//		_longTap = true;
	//		_doubleClick = true;
	//	} else if (event.type == INPUT_MBUTTONUP && event.key == HGEK_LBUTTON) {
	//		MouseUp();
	//		_longTap = false;
	//	} else if (event.type == INPUT_MOUSEMOVE) {
	//		FPoint pos = FPoint(event.x, event.y);
	//		MouseMove(pos);
	//		_longTap &= (_longTapPos - pos).Length() > LONG_TAP_EPS;
	//	} else if (event.type == INPUT_MOUSEWHEEL) {
	//		MouseWheel(event.wheel);
	//	} else if (event.type == INPUT_KEYDOWN /*&& event.key == HGEK_ESCAPE*/) {
	//		for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
	//			if ((*i)->OnKey(event.key)) {
	//				break;
	//			}
	//		}
	//	//	return true;
	//	}
	//}
	if (_longTap) {
		LongTap();
		_longTap = false;
	}
	return false;
}

void InputSystem::Init() {
	LONG_TAP_EPS = 10.f;
	LONG_TAP_TIME = 0.2f;
	DOUBLE_CLICK_TIME = 0.2f;
	_timeCounter = 0.f;
	_longTap = false;
	_doubleClick = false; 
	_locked = NULL;
}

void InputSystem::OnKeyDown(int key)
{
	_pressedKeys.insert(key);
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		if ((*i)->OnKey(key, true)) {
			return;
		}
	}
}

void InputSystem::OnKeyUp(int key)
{
	if (_pressedKeys.find(key) != _pressedKeys.end())
	{
		_pressedKeys.erase(key);
	}
	for (Listeners::reverse_iterator i = _listeners.rbegin(), e = _listeners.rend(); i != e; i++) {
		(*i)->OnKey(key, false);
	}
}

bool InputSystem::IsPressed(int key) 
{
	return (_pressedKeys.find(key) != _pressedKeys.end());
}