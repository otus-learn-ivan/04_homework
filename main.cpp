
#include <vector>
#include <sstream>
#include <array>
#include <charconv>
#include <iostream>
#include <string_view>
#include <system_error>
#include <cstdint>
#include <algorithm>
#include <list>
#include <type_traits>

using namespace std;

std::string show_to_chars(int format_args)
{
    std::array<char, 10> str;
    if (auto [ptr, ec]
            = std::to_chars(str.data(), str.data() + str.size(), format_args);
        ec == std::errc()){
            return string (std::string_view(str.data(), ptr-str.data()));;
        }
    else{
            std::cout << std::make_error_code(ec).message() << '\n';
            return std::string{};
        }
}

template <typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
vector<string> T_to_vector_int(T v){
    uint8_t* array_int = (uint8_t*)&v+sizeof(T)-1;
    vector<string> out_vector{};
    for(size_t i=0 ; i < sizeof(T) ;i++){
        out_vector.push_back(show_to_chars( size_t(*array_int)));
        array_int--;
    }
    return out_vector;
}

vector<string> T_to_vector_int(string s){
    vector<string> out_vector{};
    out_vector.push_back(s);
    return out_vector;
}

template<typename TContainer>
vector<string> parsing_container(TContainer& cont){
    vector<string> out_vector{};
    for(auto v:cont){
        out_vector.push_back(show_to_chars(v));
    }
    return out_vector;
}

template<typename T, typename = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<T, std::void_t<
    typename T::value_type,
    typename T::size_type,
    typename T::allocator_type,
    typename T::iterator,
    typename T::const_iterator,
    decltype(std::declval<T>().size()),
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    decltype(std::declval<T>().cbegin()),
    decltype(std::declval<T>().cend())
>> : std::true_type {};

template <typename T, typename std::enable_if_t<is_container<T>::value, int> = 0>
vector<string>  T_to_vector_int(T val) {
    return parsing_container(val);
}

template <typename TAction, typename TupleT, std::size_t... Is>
void enumeration_of_elements_tuple_(TAction& Action, const TupleT& tp, std::index_sequence<Is...>) {
    (Action(std::get<Is>(tp)), ...);
}

template <typename TAction, typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
void enumeration_of_elements_tuple(TAction& Action ,const TupleT& tp) {
    enumeration_of_elements_tuple_(Action,tp, std::make_index_sequence<TupSize>{});
}

template <typename... T>
std::enable_if_t<std::is_same<std::tuple<T...>, std::tuple<T...>>::value, vector<string>> T_to_vector_int(std::tuple<T...> val) {
    auto first_elem = std::get<0>(val);
    auto test_types = [&first_elem](auto elem){
        if(!is_same_v<decltype(first_elem),decltype(elem)>){
            cerr<<"tuple types do not match\n";
            throw "tuple types do not match";
        }
    };
    enumeration_of_elements_tuple(test_types,val);
    vector<string> out_vector{};
    auto action = [&out_vector](auto elem){
        out_vector.push_back(show_to_chars(elem));
    };
    enumeration_of_elements_tuple(action,val);
    return out_vector;
}

/**
    \brief Шаблонная функция печати ip адреса

    \tparam T - любой тип, для которого определена оператор суммирования 
    и оператор присваивания

    \param [in] a Первый объект, который нужно сложить
    \param [in] b Второй объект, который нужно сложить

    
    Функция  прербразует входные данные при помощи  функции T_to_vector_int в вектор <int>  и выводит его на печать
*/
template <typename T>
void print_ip(T addres){
    auto vector_ip = T_to_vector_int(addres);
    for_each(vector_ip.begin(),vector_ip.end()-1,[](auto s){
        cout << s <<".";
    });
    cout << *(vector_ip.end()-1) << endl;
}

int main(int , char**)
{

    print_ip( int8_t{-1} ); // 255
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::string{"Hello, World!"} ); // Hello, World!
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
//    print_ip( std::make_tuple(1.23, 456, 789, 0) ); // 123.456.789.0
    return 0;
}
