//#include "Inspector.h"
//#include "TypeRegistryImpl.h"  // Reflection.h‚Ì‘ã‚í‚è‚ÉTypeRegistryImpl‚ðŠÜ‚Þ
//
////std::unordered_map<std::type_index, ShowFuncType> TypeRegistry::showFunctions_;
//
//TypeRegistry&  TypeRegistry::Instance()
//{
//	static TypeRegistry instance;
//	return instance;
//}
//
//void TypeRegistry::Initialize()
//{
//	for (auto& itr : provisionalRegisterFunc_)
//	{
//		itr.second();
//	}
//}
//
//void TypeRegistry::CallFunc(std::type_index typeIdx, std::any instance, const char* name)
//{
//	const auto& itr = showFunctions_.find(typeIdx);
//	if (itr != showFunctions_.end())
//	{
//		itr->second(instance, name);
//	}
//}
//
//void TypeRegistry::ProvisionalRegister(std::type_index typeIdx, std::function<void(void)> registerFunc)
//{
//	provisionalRegisterFunc_.emplace(typeIdx,registerFunc);
//}
//
//TypeRegistry::TypeRegistry()
//{
//}
