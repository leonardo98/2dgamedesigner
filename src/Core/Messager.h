// Messager.h: interface for the Messager class.
#ifndef MYENGINE_MESSAGER_H
#define MYENGINE_MESSAGER_H

#include "types.h"
#include <QMutex>
//
//    ,    
//

class Messager
{
public:

    //
    virtual void OnMessage(const std::string &){};
//	// ...
//	virtual void SetValue(const std::string &variableName, const float &value){};
//	virtual void SetValue(const std::string &variableName, const bool &value){};
//	virtual void SetValue(const std::string &variableName, const std::string &value){};

//	virtual float GetNumberValue(const std::string &variableName){return -1;};
//	virtual bool GetBoolValue(const std::string &variableName){return false;};
//	virtual std::string GetValue(const std::string &variableName){return "-1";};

//	virtual std::string GetMyName(){return _name;};

	//       ...
	Messager(std::string receiverName);
	// ...       xml  <messager name="[   ]"/>
	Messager(rapidxml::xml_node<> *xe);

	~Messager();
    static void SendMsg(const std::string &receiverName, const std::string &message, float delay = 0.f);

//	static int SetValueF(const std::string &receiverName, const std::string &variableName, const float &value);
//	static int SetValueB(const std::string &receiverName, const std::string &variableName, const bool &value);
//	static int SetValueS(const std::string &receiverName, const std::string &variableName, const std::string &value);

//	static float GetNumberValue(const std::string &receiverName, const std::string &variableName);
//	static bool GetBoolValue(const std::string &receiverName, const std::string &variableName);
//	static std::string GetValue(const std::string &receiverName, const std::string &variableName);

	static bool CanCut(const std::string &message, const std::string &substr, std::string &result);

private:
    static QMutex m_mutex;
	typedef QList<Messager *> List;
	static List _receiver;
	std::string _name;
	struct Letter{
		std::string receiver;
		std::string message;
		float timer;
	};
	typedef QList<Letter *> AllMessages;
	static AllMessages _incoming;
	static AllMessages _messages;
    static void CoreSendMsgs(float dt); //
	friend class Core;
};

#endif//MYENGINE_MESSAGER_H
