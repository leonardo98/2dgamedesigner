#ifndef RedoUndoManager_h
#define RedoUndoManager_h

#include "LevelSet.h"

class RedoUndoManager {

public:

    ~RedoUndoManager();
    RedoUndoManager();

    static const int MAX_UNDO_STEPS = 32;

    // создает копию и заталкивает ее в конец стека,
    // если стек переполниться - удаляет первый
    // если до этого выполняли undo - то удалаем все более поздние версии
    void PushCopy(LevelSet *lset, const QVector<BeautyBase *> &currentBeauty);

    // уменьшаем счетчик версии, выдаем версию из списка истории
    LevelSet * Undo(QVector<int> &beauty);

    // обратная Undo - если можно - увеличиваем счетчик версии, выдаем версию из списка истории
    LevelSet * Redo(QVector<int> &beauty);

    // удалить накопленое
    void Clear();

    // удалить все что отменили
    void ClearRedo();

private:

    struct OneState {
        LevelSet *levelSet;
        QVector<int> selectedBeauty;
    };

    QVector<OneState> _history;

    int _currentVersion;

    int SearchBeautyOrder(LevelSet *levelSet, BeautyBase *beauty);

};

#endif//RedoUndoManager_h
