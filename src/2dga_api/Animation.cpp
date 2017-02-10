#include <algorithm>
#include "Animation.h"
#include "../Core/ogl/render.h"
#include "../Core/Core.h"

void Animation::AddBone(MovingPart *bone) {
    _renderList.push_back(bone);
}

//
//  -    
// (    , 
// ..        )
//
Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

//
//  -     XML 
//    ,
//       ,
//   TinyXml
//
Animation::Animation(rapidxml::xml_node<> *xe, GLTexture2D * hTexture)
{
    _time = fatof(xe->first_attribute("time")->value());
    _pivotPos.x = fatof(xe->first_attribute("pivotX")->value());
    _pivotPos.y = fatof(xe->first_attribute("pivotY")->value());

    rapidxml::xml_node<> *element = xe->first_node("movingPart");
	while (element) {
		_bones.push_back(new MovingPart(this, element, hTexture));
        element = element->next_sibling("movingPart");
	}
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);

	std::sort(_renderList.begin(), _renderList.end(), CmpBoneOrder);
}

//
//     , 
// "pos" -    , 
//    -  "mirror"  "true".
//        Draw()
//   ,       
//
//void Animation::SetPos(const FPoint &pos, bool mirror) {
//	_subPosition.Unit();
//	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
//	if (mirror) {
//		_subPosition.Scale(-1.f, 1.f);
//	}
//	_subPosition.Move(pos.x, pos.y);
//}

//
//     
// "position" -       [0, 1]
//
void Animation::Draw(float position) {
	PreDraw(position);

	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->Draw();
	}
}

void Animation::PreDraw(float position) {
	assert(0 <= position && position <= 1.f);
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->_visible = false;
	}

	_matrixsStack.clear(); 
	_matrixsStack.push_back(_subPosition);
	//         "" 
	for(unsigned int i = 0; i < _bones.size();i++) {
		_bones[i]->PreDraw(position, _matrixsStack);
	}    
}

//
//     
//      Update   
//  "position"  Draw
// ,      :
// progress += deltaTime / anim->Time();
// if(progress >= 1.f) progress = 0.f;
//
float Animation::Time() const {
	return _time;
}

bool Animation::PixelCheck(const FPoint &pos) {
    for (List::iterator i = _renderList.begin(), e = _renderList.end(); i != e; ++i) {
		if ((*i)->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

//void Animation::Attach(const std::string &boneName, const char *fileName) {
//	for(unsigned int i = 0; i < _renderList.size();i++) {
//		if (_renderList[i]->boneName == boneName) {
//			_renderList[i]->Attach(fileName);
//		}
//	}
//}

void Animation::EncapsulateAllDots(float p, Rect &rect) {
	assert(0 <= p && p <= 1.f);
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->_visible = false;
	}

	_matrixsStack.clear(); 
	_matrixsStack.push_back(_subPosition);
	//         "" 
	for(unsigned int i = 0; i < _bones.size();i++) {
		_bones[i]->PreDraw(p, _matrixsStack);
	}    
    for (List::iterator i = _renderList.begin(), e = _renderList.end(); i != e; ++i) {
		(*i)->EncapsulateAllDots(rect);
	}
}

//  ""     
void AnimationState::InitWithAnimation(Animation *anim, float position) {
	_partPositions.resize(anim->_renderList.size());
	for (unsigned int i = 0; i < _partPositions.size(); ++i) {
		anim->_renderList[i]->PreCalcLocalPosition(position);
		_partPositions[i] = anim->_renderList[i]->precalculatedLocalPosition;
	}
}

//  ""     
void AnimationState::InitWithAnimations(Animation *animOne, float positionOne, Animation *animTwo, float positionTwo, float mix) {
	_partPositions.resize(animOne->_renderList.size());

	float minus = 1 - mix;
	for(unsigned int i = 0; i < animOne->_renderList.size();i++) {
		MovingPart *m = animOne->_renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(positionOne);

		MovingPart *mMixWith = animTwo->_renderList[i];
		mMixWith->_visible = false;
		mMixWith->PreCalcLocalPosition(positionTwo);

		_partPositions[i].angle = mMixWith->precalculatedLocalPosition.angle * minus 
									+ m->precalculatedLocalPosition.angle * mix;
		_partPositions[i].sx = mMixWith->precalculatedLocalPosition.sx * minus 
									+ m->precalculatedLocalPosition.sx * mix;
		_partPositions[i].sy = mMixWith->precalculatedLocalPosition.sy * minus
									+ m->precalculatedLocalPosition.sy * mix;
		_partPositions[i].x = mMixWith->precalculatedLocalPosition.x * minus 
									+ m->precalculatedLocalPosition.x * mix;
		_partPositions[i].y = mMixWith->precalculatedLocalPosition.y * minus 
									+ m->precalculatedLocalPosition.y * mix;
	}

}

//       , 
//      
void AnimationState::DrawMixed(Animation *anim, float position, float mix) {
	float minus = 1 - mix;
	for(unsigned int i = 0; i < anim->_renderList.size();i++) {
		MovingPart *m = anim->_renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);
		m->localPosition.angle = _partPositions[i].angle * minus + m->precalculatedLocalPosition.angle * mix;
		m->localPosition.sx = _partPositions[i].sx * minus + m->precalculatedLocalPosition.sx * mix;
		m->localPosition.sy = _partPositions[i].sy * minus + m->precalculatedLocalPosition.sy * mix;
		m->localPosition.x = _partPositions[i].x * minus + m->precalculatedLocalPosition.x * mix;
		m->localPosition.y = _partPositions[i].y * minus + m->precalculatedLocalPosition.y * mix;
	}

	anim->_matrixsStack.clear(); 
	anim->_matrixsStack.push_back(Render::GetCurrentMatrix());
	//         "" 
	anim->_matrixsStack.back().Mul(anim->_subPosition);
	for (unsigned int i = 0; i < anim->_bones.size(); ++i) {
		anim->_bones[i]->PreDrawInLocalPosition(anim->_matrixsStack);
	}

	for(unsigned int i = 0; i < anim->_renderList.size();i++) {
		anim->_renderList[i]->Draw();
	}
}

//       ,   
//    
void AnimationState::MixWith(Animation *anim, float position, float mix) {
	float minus = 1 - mix;
	for(unsigned int i = 0; i < anim->_renderList.size();i++) {
		MovingPart *m = anim->_renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);
		_partPositions[i].angle = _partPositions[i].angle * minus + m->precalculatedLocalPosition.angle * mix;
		_partPositions[i].sx = _partPositions[i].sx * minus + m->precalculatedLocalPosition.sx * mix;
		_partPositions[i].sy = _partPositions[i].sy * minus + m->precalculatedLocalPosition.sy * mix;
		_partPositions[i].x = _partPositions[i].x * minus + m->precalculatedLocalPosition.x * mix;
		_partPositions[i].y = _partPositions[i].y * minus + m->precalculatedLocalPosition.y * mix;
	}
}

void Animation::DrawMixed(float position, const Animation &anim, float animPosition, float mix) {
	float minus = 1 - mix;
	for(unsigned int i = 0; i < _renderList.size();i++) {
		MovingPart *m = _renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);

		MovingPart *mMixWith = anim._renderList[i];
		mMixWith->_visible = false;
		mMixWith->PreCalcLocalPosition(animPosition);

		m->localPosition.angle = mMixWith->precalculatedLocalPosition.angle * minus 
									+ m->precalculatedLocalPosition.angle * mix;
		m->localPosition.sx = mMixWith->precalculatedLocalPosition.sx * minus 
									+ m->precalculatedLocalPosition.sx * mix;
		m->localPosition.sy = mMixWith->precalculatedLocalPosition.sy * minus
									+ m->precalculatedLocalPosition.sy * mix;
		m->localPosition.x = mMixWith->precalculatedLocalPosition.x * minus 
									+ m->precalculatedLocalPosition.x * mix;
		m->localPosition.y = mMixWith->precalculatedLocalPosition.y * minus 
									+ m->precalculatedLocalPosition.y * mix;
	}

	_matrixsStack.clear(); 
	_matrixsStack.push_back(Render::GetCurrentMatrix());
	//         "" 
	_matrixsStack.back().Mul(_subPosition);
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->PreDrawInLocalPosition(_matrixsStack);
	}

	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->Draw();
	}
}



/*     */

//   
//    -  "true"
//     -  "" 
//
//     HGE     
//           
//
bool AnimationManager::Load(const std::string &fileName) {
	//   -      "tex",   
	//
	//   -      
	
	std::string fullFileName = Core::Resource_MakePath(fileName.c_str());
	for (unsigned int i = 0; i < _files.size(); ++i) {
		if (_files[i] == fullFileName) { //      -   
			return true;
		}
	}

	GLTexture2D * tex = new GLTexture2D((fullFileName.substr(0, fullFileName.length() - 3) + "png").c_str());
    if (tex->failed())
    {
        LOG("assigned png file not found " + (fullFileName.substr(0, fullFileName.length() - 3) + "png").c_str());
		delete tex;
		return false;
	}
	//  

    std::ifstream myfile(fullFileName.c_str());
    std::vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>( ));

    if (buffer.size())
    { //    -  XML
        buffer.push_back('\0');
        rapidxml::xml_document<> doc;
        if (!Parse(doc, buffer, fileName.c_str()))
        {
            delete tex;
            return false;
        }
        rapidxml::xml_node<> *root = doc.first_node();//.RootElement();
        rapidxml::xml_node<> *animation = root->first_node("Animation");
		while (animation) {
            const char *id = animation->first_attribute("id")->value();
			//  -        id -    
			if (_animations.find(id) == _animations.end()) { 
                Animation *a = _animations[id] = new Animation(animation, tex);
                a->SetTexturePath(fileName + "/" + id);
                if (Animation::errMessage.size())
                {
                    LOG("animation loading failed: " + id + " : " + Animation::errMessage);
                    Animation::errMessage.clear();
                }
			} else {
                LOG("animation already loaded: " + id);
			}
            animation = animation->next_sibling("Animation");
		}
		_textures.push_back(tex);
		_files.push_back(fullFileName);//     

		return true;
	} else { 
		//         
		//  ,  "false"
		//
		//   -      
		delete tex;
		//  

		//char buffer[1024];
		//GetCurrentDirectoryA(1024, buffer);
		LOG("file not found: %s" + fileName);
		return false;
	}
}

//
//       
//
void AnimationManager::UnloadAll() {
	for (AnimationMap::iterator it = _animations.begin(); it != _animations.end(); it++) {
		delete (*it).second;
	}
	for (unsigned int i = 0; i < _textures.size();i++) {
		delete (_textures[i]);
	}
	_animations.clear();
	_textures.clear();
	_files.clear();
}

//
//      animationId -      
// (   delete  !)
//
Animation *AnimationManager::getAnimation(const std::string &animationId) {
	AnimationMap::iterator it_find = _animations.find(animationId);
	if (it_find != _animations.end()) {
		return (*it_find).second;
	}
	LOG("animation " + animationId + " not found.");
	return NULL;
}

AnimationManager::AnimationMap AnimationManager::_animations;
QVector<std::string> AnimationManager::_files;
QVector<GLTexture2D *> AnimationManager::_textures;

std::string Animation::errMessage;
