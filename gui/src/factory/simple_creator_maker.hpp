#include <memory>
#include <new>
#include "simple_creator_maker.h"
#include "simple_creator_maker_memory_error.h"

template<typename Base, typename ParticularCreator, typename... Arguments>
std::unique_ptr<Creator<Base, Arguments...>> SimpleCreatorMaker<Base, ParticularCreator, Arguments...>::make() const
{
    try
    {
        return std::make_unique<ParticularCreator>();
    }
    catch (const std::bad_alloc &)
    {
        throw SimpleCreatorMakerMemoryError{"Memory allocation failed.", typeid(SimpleCreatorMakerMemoryError)};
    }
}
