// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides the view::repeat_view.
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

#pragma once

#include <range/v3/view/single.hpp>

#include <seqan3/core/metafunction/iterator.hpp>
#include <seqan3/core/metafunction/range.hpp>
#include <seqan3/range/detail/random_access_iterator.hpp>
#include <seqan3/std/ranges>

namespace seqan3::detail
{

// ---------------------------------------------------------------------------------------------------------------------
// repeat_view class
// ---------------------------------------------------------------------------------------------------------------------

/*!\brief The type returned by seqan3::view::repeat.
 * \tparam value_t The type of value to repeat which is always wrapped in a std::view::single.
 * \implements std::ranges::View
 * \implements std::ranges::RandomAccessRange
 * \implements std::ranges::OutputRange
 * \ingroup view
 *
 * \details
 *
 * This class models an infinite range over a given value, although "infinity" is limited by the maximum value of
 * `difference_type` of which the iterator stores a member variable to represent distances.
 *
 * Note that most members of this class are generated by ranges::view_interface which is not yet documented here.
 */
template <std::CopyConstructible value_t>
class repeat_view : public std::ranges::view_interface<repeat_view<value_t>>
{
private:
    //!/brief the base type.
    using base_t = std::ranges::view_interface<repeat_view<value_t>>;

    //!\brief The sentinel type is set to std::ranges::default_sentinel_t.
    using sentinel_type = std::ranges::default_sentinel_t;

    //!\brief The iterator type for view::repeat (a random access iterator).
    template <typename parent_type>
    class repeat_view_iterator : public detail::random_access_iterator_base<parent_type, repeat_view_iterator>
    {
        //!\brief The CRTP base type.
        using base_t = detail::random_access_iterator_base<parent_type, repeat_view_iterator>;

        //!\brief The base position type.
        using typename base_t::position_type;

    public:
        //!\brief Type for distances between iterators.
        using typename base_t::difference_type;
        //!\brief Value type of container elements.
        using typename base_t::value_type;
        //!\brief Use reference type defined by container.
        using typename base_t::reference;
        //!\brief Pointer type is pointer of container element type.
        using typename base_t::pointer;
        //!\brief Tag this class as a random access iterator.
        using typename base_t::iterator_category;

        /*!\name Constructors, destructor and assignment
         * \{
         */
        constexpr repeat_view_iterator() = default;                                         //!< Defaulted.
        constexpr repeat_view_iterator(repeat_view_iterator const &) = default;             //!< Defaulted.
        constexpr repeat_view_iterator & operator=(repeat_view_iterator const &) = default; //!< Defaulted.
        constexpr repeat_view_iterator (repeat_view_iterator &&) = default;                 //!< Defaulted.
        constexpr repeat_view_iterator & operator=(repeat_view_iterator &&) = default;      //!< Defaulted.
        ~repeat_view_iterator() = default;                                                  //!< Defaulted.

        /*!\brief Construct by host range.
         * \param host The host range.
         */
        explicit constexpr repeat_view_iterator(parent_type & host) noexcept : base_t{host} {}

        /*!\brief Constructor for const version from non-const version.
         * \param rhs a non-const version of repeat_view_iterator to construct from.
         */
        template <typename parent_type2>
        //!\cond
            requires std::is_const_v<parent_type> && !std::is_const_v<parent_type2> &&
                     std::is_same_v<std::remove_const_t<parent_type>, parent_type2>
        //!\endcond
        constexpr repeat_view_iterator(repeat_view_iterator<parent_type2> const & rhs) noexcept :
            base_t{rhs}
        {}
        //!\}

        /*!\name Comparison operators
         * \{
         */
        //!\brief Inherit the equality comparison (same type) from base type.
        using base_t::operator==;
        //!\brief Inherit the inequality comparison (same type) from base type.
        using base_t::operator!=;

        //!\brief Equality comparison to the sentinel always returns false on an infinite view.
        constexpr bool operator==(std::ranges::default_sentinel_t const &) const noexcept
        {
            return false;
        }

        //!\brief Inequality comparison to the sentinel always returns true on an infinite view.
        constexpr bool operator!=(std::ranges::default_sentinel_t const &) const noexcept
        {
            return true;
        }

        //!\brief Equality comparison to the sentinel always returns false on an infinite view.
        friend constexpr bool operator==(std::ranges::default_sentinel_t const &,
                                         repeat_view_iterator const &) noexcept
        {
            return false;
        }

        //!\brief Inequality comparison to the sentinel always returns true on an infinite view.
        friend constexpr bool operator!=(std::ranges::default_sentinel_t const &,
                                         repeat_view_iterator const &) noexcept
        {
            return true;
        }
        //!\}
    };

public:
    /*!\name Associated types
     * \{
     */
    //!\brief The value type (equals the value_t with any references removed).
    using value_type = std::remove_reference_t<value_t>;
    //!\brief The reference_type.
    using reference  = value_type &;
    //!\brief The const reference type.
    using const_reference = value_type const &;
    //!\brief The size_type is void, because this range is never sized.
    using size_type = void;
    //!\brief The type to store the difference of two iterators.
    using difference_type = ptrdiff_t;
    //!\brief The iterator type of this view (a random access iterator).
    using iterator = repeat_view_iterator<repeat_view>;
    //!\brief The const_iterator type is equal to the iterator type but over the const qualified type.
    using const_iterator = repeat_view_iterator<repeat_view const>;
    //!\}

    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr repeat_view() = default;                                //!< Defaulted.
    constexpr repeat_view(repeat_view const &) = default;             //!< Defaulted.
    constexpr repeat_view & operator=(repeat_view const &) = default; //!< Defaulted.
    constexpr repeat_view(repeat_view &&) = default;                  //!< Defaulted.
    constexpr repeat_view & operator=(repeat_view &&) = default;      //!< Defaulted.
    ~repeat_view() = default;                                         //!< Defaulted.

    //!\brief Construct from any type (Note: the value will be copied into view::single).
    constexpr explicit repeat_view(value_t const & value) : single_value{ranges::single_view{value}}
    {}

    //!\overload
    constexpr explicit repeat_view(value_t && value) : single_value{ranges::single_view{std::move(value)}}
    {}
    //!\}

    /*!\name Iterators
     * \{
     */
    /*!\brief Returns an iterator to the first element of the range.
     * \returns Iterator to the first element.
     *
     * \details
     *
     * This range is never empty so the returned iterator will never be equal to end().
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    constexpr iterator begin() noexcept
    {
        return iterator{*this};
    }

    //!\copydoc begin()
    constexpr const_iterator begin() const noexcept
    {
        return const_iterator{*this};
    }

    //!\copydoc begin()
    constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator{*this};
    }

    /*!\brief Returns an iterator to the element following the last element of the range.
     * \returns Iterator to the end.
     *
     * \details
     *
     * This element acts as a placeholder; attempting to dereference it results in undefined behaviour.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    constexpr sentinel_type end() noexcept
    {
        return {};
    }

    //!\copydoc end()
    constexpr sentinel_type end() const noexcept
    {
        return {};
    }

    //!\copydoc end()
    constexpr sentinel_type cend() const noexcept
    {
        return {};
    }
    //!\}

    /*!\name Element access
     * \{
     */
    /*!\brief Returns the n-th element.
     * \param n The position of the element to return.
     * \returns A reference to the cached value.
     *
     * \details
     *
     * Since this range is an infinite range, access is never out-of-bounds.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    constexpr const_reference operator[](difference_type const SEQAN3_DOXYGEN_ONLY(n)) const noexcept
    {
        return *single_value.begin();
    }

    constexpr reference operator[](difference_type const SEQAN3_DOXYGEN_ONLY(n)) noexcept
    {
        return *single_value.begin();
    }
    //!}

private:
    //!\brief A std::view::single over the input.
    decltype(std::view::single(std::declval<value_t &>())) single_value;
};

// ---------------------------------------------------------------------------------------------------------------------
// repeat (factory)
// ---------------------------------------------------------------------------------------------------------------------

//!\brief View factory definition for view::repeat.
struct repeat_fn
{
    //!\brief Returns an instance of seqan3::detail::repeat_view constructed with \p value.
    template <std::CopyConstructible value_type>
    constexpr auto operator()(value_type && value) const
    {
        return detail::repeat_view{std::forward<value_type>(value)};
    }
};

} // namespace seqan3::detail

namespace seqan3::view
{

// ---------------------------------------------------------------------------------------------------------------------
// view::repeat (factory instance)
// ---------------------------------------------------------------------------------------------------------------------

/*!\name General purpose views
 * \{
 */
/*!\brief A view factory that repeats a given value infinitely.
 * \tparam    value_t  The type of value to repeat wrapped in a std::view::single; must model std::CopyConstructible.
 * \param[in] value    The value to repeat.
 * \returns An infinite range over value.
 * \ingroup view
 *
 * \details
 *
 * ### View properties
 *
 * This view is **source-only**, it can only be at the beginning of a pipe of range transformations.
 *
 * | range concepts and reference_t  | `rrng_t` (returned range type)                     |
 * |---------------------------------|:--------------------------------------------------:|
 * | std::ranges::InputRange         | *guaranteed*                                       |
 * | std::ranges::ForwardRange       | *guaranteed*                                       |
 * | std::ranges::BidirectionalRange | *guaranteed*                                       |
 * | std::ranges::RandomAccessRange  | *guaranteed*                                       |
 * | std::ranges::ContiguousRange    |                                                    |
 * |                                 |                                                    |
 * | std::ranges::ViewableRange      | *guaranteed*                                       |
 * | std::ranges::View               | *guaranteed*                                       |
 * | std::ranges::SizedRange         |                                                    |
 * | std::ranges::CommonRange        |                                                    |
 * | std::ranges::OutputRange        | *guaranteed*                                       |
 * | seqan3::const_iterable_concept  | *guaranteed*                                       |
 * |                                 |                                                    |
 * | seqan3::reference_t             | std::remove_reference_t<value_t> &                 |
 *
 * See the \link view view submodule documentation \endlink for detailed descriptions of the view properties.
 *
 * \attention The given value to repeat is always **copied** into the range.
 *
 * ### Example
 *
 * \include test/snippet/range/view/repeat.cpp
 *
 * \hideinitializer
 */
constexpr inline detail::repeat_fn repeat{};
//!\}

} // namespace seqan3::view
