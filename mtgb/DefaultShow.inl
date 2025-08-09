// DefaultShow.inl - テンプレート実装ファイル
namespace mtgb {

template<typename T>
void DefaultShow(T* value, const char* name)
{
    using Type = std::remove_cvref_t<T>;
    if constexpr (std::is_array_v<Type>)
    {
        if (ImGui::CollapsingHeader(name))
        {
            ImGui::Indent();

            //要素型取得
            using ElemType = std::remove_extent_t<Type>;
            //0次元目の要素数取得
            constexpr size_t N = std::extent_v<Type>;

            //配列要素ごとに再帰する
            for (size_t i = 0; i < N; ++i)
            {
                std::string elemName = std::string(name) + "[" + std::to_string(i) + "]";
                DefaultShow(static_cast<ElemType*>(&(*value)[i]), elemName.c_str());
            }

            ImGui::Unindent();
        }
    }
    else if constexpr (std::is_same_v<Type, bool>)
    {
        ImGui::Checkbox(name, reinterpret_cast<bool*>(value));
    }
    else if constexpr (std::is_same_v<Type, float>)
    {
        ImGui::InputFloat(name, reinterpret_cast<float*>(value));
    }
    else if constexpr (std::is_same_v<Type, float*>)
    {
        ImGui::InputFloat(name, *reinterpret_cast<float**>(value));
    }
    else if constexpr (std::is_same_v<Type,int>)
    {
        ImGui::InputInt(name, reinterpret_cast<int*>(value));
    }
    else if constexpr (std::is_same_v<Type, int*>)
    {
        ImGui::InputInt(name, *reinterpret_cast<int**>(value));
    }
    else if constexpr (std::is_same_v<Type, long>)
    {
        ImGui::Text("%s : %4.2ld", name, *value);
    }
    else if constexpr (std::is_same_v<Type, long*>)
    {
        ImGui::Text("%s : %4.2ld", name, **reinterpret_cast<long**>(value));
    }
    else if constexpr (std::is_same_v<Type, unsigned long>)
    {
        ImGui::Text("%s : %4.2lo", name, *value);
    }
    else if constexpr (std::is_same_v<Type, unsigned long*>)
    {
        ImGui::Text("%s : %4.2lo", name, **reinterpret_cast<unsigned long**>(value));
    }
    else if constexpr (std::is_same_v<Type, unsigned char>)
    {
        ImGui::Text("%s : %4.2hhu", name, *value);
    }
    else if constexpr (std::is_enum_v<Type>)
    {
        // TODO: enum handling
    }
    else if constexpr (std::is_same_v<Type, std::string>)
    {
        std::string str = mtgb::MultiToUTF8(*value).c_str();
        std::vector<char> buffer(str.begin(), str.end());
        buffer.resize(256);
        if (ImGui::InputText(name,buffer.data(), buffer.size()))
        {
            *value = UTF8ToMulti(std::string(buffer.data()));
        }
    }
    else
    {
        ImGui::Text("%s:Unknown,%s",name,typeid(Type).name() );
    }
}

} // namespace mtgb