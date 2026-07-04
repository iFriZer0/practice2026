#include <memory>
#include <new>
#include <utility>
#include "simple_creator.h"
#include "simple_creator_memory_error.h"

template<typename Base, typename Derived, typename... Arguments>
std::unique_ptr<Base> SimpleCreator<Base, Derived, Arguments...>::create(Arguments&& ...arguments) const
{
    try
    {
        return std::make_unique<Derived>(std::forward<Arguments>(arguments)...);
    }
    catch (const std::bad_alloc &)
    {
        throw SimpleCreatorMemoryError{"Memory allocation failed.", typeid(SimpleCreatorMemoryError)};
    }
}
