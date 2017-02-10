// InputSystem.h: interface for the InputSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_INPUTSYSTEM_H
#define MYENGINE_INPUTSYSTEM_H

#include "types.h"
#include "Object.h"
#include <set>

//
// надстройка над системой ввода,
// если нужно обрабатывать событи€ мыши - наследуемс€ от этого класса
// если нет - от Object.h
//

class InputSystem 
	: public Object  
{
private:
	static float DOUBLE_CLICK_TIME; // максимальное врем€ на второй клик - если врем€ больше - событие "аннулируетс€"
	static float LONG_TAP_EPS; // погрешность на случайные движени€ мыши во врем€ нажати€, если движение более сильное - событие "аннулируетс€"
	static float LONG_TAP_TIME; // минимальное врем€ на вызов событи€ - если врем€ меньше - событие "аннулируетс€"
	static float _timeCounter; // счетчик времени(отсчитывает врем€ либо с последнего событи€ нажати€ или отпускани€ кнопки мыши
	static bool _longTap; // true - если ловим событие "ƒлинный “ап"
	static bool _doubleClick; // true - если ловим это событие "ƒвойной клик"
	static FPoint _longTapPos;
	static InputSystem *_locked;// пишу того кто вз€лс€ обрабатывать клик(нажатие/отпускание) - остальным не делаю MouseMove
	static std::set<int> _pressedKeys;
public:
	InputSystem();
	virtual ~InputSystem();
	// эти методы класса переопредел€ем 
	// если хотим обрабатывать соотвествующее событие

	// клик по элементу(нажатие левой кнопки мыши)
	virtual void OnMouseDown(const FPoint &mousePos);
	// отпустили кнопку мыши(неважно где)
	virtual void OnMouseUp();
	// движение мыши
	virtual void OnMouseMove(const FPoint &mousePos);
	// колесико мыши
	virtual bool OnMouseWheel(int direction);
	
	virtual bool OnKey(int key, bool bDown);

	// длительное нажатие и удержание
	virtual void OnLongTap(const FPoint &mousePos);
	// двойной клик
	virtual void OnDoubleClick(const FPoint &mousePos);

	// ќЅя«ј≈Ћ№Ќќ нужно определить дл€ корректной работы OnMouseDown() и прочих функций
	virtual bool IsMouseOver(const FPoint &mousePos) = 0;
	
protected:
	// дл€ сложных(вложенных) элементов ввода - может понадобитьс€, переписать общий способ
	void RemoveFromList(InputSystem *listener); 

private:
	typedef std::list<InputSystem *> Listeners;
	static Listeners _listeners;

	static void MouseWheel(int direction);
	static void LongTap();
	static void DoubleClick(const FPoint &mousePos);

public:
	static void MouseDown(const FPoint &mousePos);
	static void MouseUp();
	static void MouseMove(const FPoint &mousePos);

	static void Reset();
	static void Init();
	static bool CheckForEvent(float dt);

	static void OnKeyDown(int key);
	static void OnKeyUp(int key);
	static bool IsPressed(int key);

};

#endif//MYENGINE_INPUTSYSTEM_H