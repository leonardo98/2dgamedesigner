#include "RedoUndoManager.h"
#include "TileEditorInterface.h"

RedoUndoManager::~RedoUndoManager() {
    Clear();
}

RedoUndoManager::RedoUndoManager() 
: _currentVersion(0)
{
}

void RedoUndoManager::PushCopy(LevelSet *lset, const QVector<BeautyBase *> &currentBeauty) {
    if (_history.size() != 0) {
        TileEditorInterface::Instance()->Changes();
    }
    ClearRedo();
    OneState one;
    one.levelSet = new LevelSet(*lset);
    for (unsigned int i = 0; i < currentBeauty.size(); ++i) {
        one.selectedBeauty.push_back(SearchBeautyOrder(lset, currentBeauty[i]));
    }
    _history.push_back(one);
    if (_history.size() > MAX_UNDO_STEPS) {
        delete _history.front().levelSet;
        _history.erase(_history.begin());
    }
    _currentVersion = _history.size() - 1;
}

LevelSet * RedoUndoManager::Undo(QVector<int> &beauty) {
    TileEditorInterface::Instance()->Changes();
    assert(_history.size());
    if (_currentVersion > 0) {
        _currentVersion--;
    }

    beauty = _history[_currentVersion].selectedBeauty;

    return _history[_currentVersion].levelSet;
}

LevelSet * RedoUndoManager::Redo(QVector<int> &beauty) {
    TileEditorInterface::Instance()->Changes();
    assert(_history.size());
    if (_currentVersion < (int)_history.size() - 1) {
        _currentVersion++;
    }

    beauty = _history[_currentVersion].selectedBeauty;

    return _history[_currentVersion].levelSet;
}

void RedoUndoManager::Clear() {
    while (_history.size()) {
        delete _history.back().levelSet;
        _history.pop_back();
    }
    _currentVersion = 0;
}

void RedoUndoManager::ClearRedo() {
    while ((int)_history.size() - 1> _currentVersion) {
        delete _history.back().levelSet;
        _history.pop_back();
    }
}

int RedoUndoManager::SearchBeautyOrder(LevelSet *levelSet, BeautyBase *beauty) {
    for (unsigned int i = 0; i < levelSet->beauties.size(); ++i) {
        if (levelSet->beauties[i] == beauty) {
            return i;
        }
    }
    return -1;
}
