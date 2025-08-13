#include "Inspector.h"

#include "WindowContextUtil.h"


Inspector& Inspector::Instance()
{
	static Inspector instance;
	return instance;
}

void Inspector::Show(std::type_index typeIdx, std::any instance, const char* name)
{
	if (mtgb::WinCtxRes::CurrContext() != mainWindow_)
	{
		return;
	}
	TypeRegistry::Instance().CallFunc(typeIdx, instance, name);
}