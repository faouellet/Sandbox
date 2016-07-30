template <class U, class... Ts>
struct is_all_same;

template <class U, class T, class... Ts>
struct is_all_same<U, T, Ts...>
{
    static constexpr bool value = std::is_same<U, T>::value && is_all_same<U, Ts...>::value;
};

template <class U, class T>
struct is_all_same<U, T>
{
    static constexpr bool value = std::is_same<U, T>::value;
};

template <class U>
struct is_all_same<U>
{
    static constexpr bool value = true;
};