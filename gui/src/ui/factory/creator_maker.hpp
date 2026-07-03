#include <memory>
#include <new>
#include "creator_maker.h"
#include "creator_maker_memory_error.h"

template<typename Base, typename ParticularCreator, typename... Arguments>
requires ConstructibleDerivative<Creator<Base, Arguments...>, ParticularCreator>
std::unique_ptr<Creator<Base, Arguments...>> CreatorMaker::make()
{
    try
    {
        return std::make_unique<ParticularCreator>();
    }
    catch (const std::bad_alloc &)
    {
        throw new CreatorMakerMemoryError{"Memory allocation failed.", typeid(CreatorMakerMemoryError), nullptr};
    }
}
