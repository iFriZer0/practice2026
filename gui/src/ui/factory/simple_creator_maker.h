#ifndef SIMPLE_CREATOR_MAKER_H__

#define SIMPLE_CREATOR_MAKER_H__

#include <memory>
#include "creator_maker.h"

template<typename Base, typename ParticularCreator, typename... Arguments>
class SimpleCreatorMaker final : public CreatorMaker<Base, Arguments...>
{
public:
    std::unique_ptr<Creator<Base, Arguments...>> make() const override;
};

#include "simple_creator_maker.hpp"

#endif
