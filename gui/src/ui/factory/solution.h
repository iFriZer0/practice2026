#ifndef SOLUTION_H__

#define SOLUTION_H__

#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <utility>
#include "creator.h"
#include "creator_maker.h"

template<typename Base, typename Identifier, typename... Arguments>
class Solution final
{
public:
    explicit Solution() = default;
    explicit Solution(const Solution<Base, Identifier, Arguments...> &other);
    explicit Solution(Solution<Base, Identifier, Arguments...> &&other) noexcept;
    explicit Solution(std::initializer_list<std::pair<Identifier, std::shared_ptr<CreatorMaker<Base, Arguments...>>>> makers);

    ~Solution() = default;

    Solution<Base, Identifier, Arguments...> &operator=(const Solution<Base, Identifier, Arguments...> &other);
    Solution<Base, Identifier, Arguments...> &operator=(Solution<Base, Identifier, Arguments...> &&other) noexcept;

    bool add(const Identifier &identifier, const CreatorMaker<Base, Arguments...> &maker);

    std::unique_ptr<Creator<Base, Arguments...>> make(const Identifier &identifier) const;
private:
    std::unordered_map<Identifier, std::shared_ptr<CreatorMaker<Base, Arguments...>>> makers;
};

#include "solution.hpp"

#endif
