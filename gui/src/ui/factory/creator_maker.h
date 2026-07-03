#ifndef CREATOR_MAKER_H__

#define CREATOR_MAKER_H__

#include <memory>
#include <type_traits>
#include "creator.h"

template<typename Base, typename Derived, typename... Arguments>
concept ConstructibleDerivative = std::is_base_of_v<Base, Derived> && std::is_constructible_v<Derived, Arguments...>;

class CreatorMaker final
{
public:
    template<typename Base, typename ParticularCreator, typename... Arguments>
    requires ConstructibleDerivative<Creator<Base, Arguments...>, ParticularCreator>
    static std::unique_ptr<Creator<Base, Arguments...>> make();
};

#include "creator_maker.hpp"

#endif
