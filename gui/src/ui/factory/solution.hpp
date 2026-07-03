#include <algorithm>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>
#include "solution.h"
#include "solution_memory_error.h"
#include "solution_no_maker_error.h"

template<typename Base, typename Identifier, typename... Arguments>
Solution<Base, Identifier, Arguments...>::Solution(const Solution<Base, Identifier, Arguments...> &other) try
    : makers{other.makers} {}
catch (const std::bad_alloc &)
{
    throw SolutionMemoryError{"Memory allocation failed.", typeid(SolutionMemoryError)};
}

template<typename Base, typename Identifier, typename... Arguments>
Solution<Base, Identifier, Arguments...>::Solution(Solution<Base, Identifier, Arguments...> &&other) noexcept
    : makers{std::move(other.makers)} {}


template<typename Base, typename Identifier, typename... Arguments>
Solution<Base, Identifier, Arguments...>::Solution(
        std::initializer_list<std::pair<Identifier, std::shared_ptr<CreatorMaker<Base, Arguments...>>>> makers
)
{
    try
    {
        std::ranges::copy(makers, std::inserter(this->makers, this->makers.end()));
    }
    catch (const std::bad_alloc &)
    {
        throw SolutionMemoryError{"Memory allocation failed.", typeid(SolutionMemoryError)};
    }
}

template<typename Base, typename Identifier, typename... Arguments>
Solution<Base, Identifier, Arguments...> &Solution<Base, Identifier, Arguments...>::operator=(const Solution<Base, Identifier, Arguments...> &other)
{
    try
    {
        makers = other.makers;
    }
    catch (const std::bad_alloc &)
    {
        throw SolutionMemoryError{"Memory allocation failed.", typeid(SolutionMemoryError)};
    }
}

template<typename Base, typename Identifier, typename... Arguments>
Solution<Base, Identifier, Arguments...> &Solution<Base, Identifier, Arguments...>::operator=(Solution<Base, Identifier, Arguments...> &&other) noexcept
{
    makers = std::move(other.makers);
}

template<typename Base, typename Identifier, typename... Arguments>
bool Solution<Base, Identifier, Arguments...>::add(const Identifier &identifier, const CreatorMaker<Base, Arguments...> &maker)
{
    try
    {
        return makers.insert(std::make_pair(identifier, maker)).second;
    }
    catch (const std::bad_alloc &)
    {
        throw SolutionMemoryError{"Memory allocation failed.", typeid(SolutionMemoryError)};
    }

}

template<typename Base, typename Identifier, typename... Arguments>
std::unique_ptr<Creator<Base, Arguments...>> Solution<Base, Identifier, Arguments...>::make(const Identifier &identifier) const
{
    try
    {
        return makers.at(identifier)->make();
    }
    catch (const std::out_of_range &)
    {
        throw SolutionNoMakerError{"Maker was not found.", typeid(SolutionNoMakerError<Identifier>), identifier};
    }
}
