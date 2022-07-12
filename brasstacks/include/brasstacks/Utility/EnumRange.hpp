#ifndef BRASSTACKS_UTILITY_ENUMRANGE_HPP
#define BRASSTACKS_UTILITY_ENUMRANGE_HPP

//
// This helper template permits the iteration of an enum class, making the
// typesafe enum more useful in more situations where you'd like to use a
// classic enum.
// 
// Credit to eric: https://stackoverflow.com/a/63365764/1464937
//

namespace btx {

template <class ENUM, typename TYPE = int>
struct EnumRange {
    struct Iterator {
        explicit Iterator(TYPE v) : value(v) { }
        void operator++() { ++value; }
        bool operator!=(Iterator rhs) { return value != rhs.value; }
        ENUM operator*() const { return static_cast<ENUM>(value); }

        TYPE value = 0;
    };

    Iterator begin() const { return Iterator(0); }
    Iterator end()   const { return Iterator(static_cast<TYPE>(ENUM::END)); }

    // NOTE ------------------------------------------------------------------
    // Your enum class definition must have END as its final entry. This is
    // case sensitive.
    // -----------------------------------------------------------------------
};

} // namespace btx
#endif // BRASSTACKS_UTILITY_ENUMRANGE_HPP