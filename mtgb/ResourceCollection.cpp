#include "ResourceCollection.h"
#include "WindowContextResource.h"

mtgb::ResourceCollection::ResourceCollection(ResourceCollection&& other) noexcept
    :resourceCollection_(std::move(other.resourceCollection_))
    , insertionOrder_(std::move(other.insertionOrder_))
{}

mtgb::ResourceCollection::~ResourceCollection()
{
    Release();
}

mtgb::ResourceCollection::ResourceCollection(const ResourceCollection& other)
{
    Copy(other);
}

void mtgb::ResourceCollection::Copy(const ResourceCollection& other)
{
    for (const auto& [key, ptr] : other.resourceCollection_)
    {
        resourceCollection_[key] = ptr ? ptr->Clone() : nullptr;
    }
    insertionOrder_ = other.insertionOrder_;
}

mtgb::ResourceCollection mtgb::ResourceCollection::Clone() const
{
    return ResourceCollection(*this);
}

void mtgb::ResourceCollection::Release()
{
    for (auto& [type, resource] : resourceCollection_)
    {
        SAFE_DELETE(resource);
    }
    resourceCollection_.clear();
}

mtgb::ResourceCollection mtgb::ResourceCollection::operator=(const ResourceCollection& other)
{
    if (this != &other)
    {
        // Šù‘¶ƒŠƒ\[ƒX‚Ì‰ð•ú
        Release();
        this->Copy(other);
    }
    return *this;
}

mtgb::WindowContextResource*& mtgb::ResourceCollection::operator[](const std::type_index& key)
{
    if (resourceCollection_.find(key) == resourceCollection_.end())
    {
        insertionOrder_.push_back(key);
    }
    return resourceCollection_[key];
}

const mtgb::WindowContextResource* mtgb::ResourceCollection::operator[](const std::type_index& key) const
{
    auto it = resourceCollection_.find(key);
    return it != resourceCollection_.end() ? it->second : nullptr;
}
