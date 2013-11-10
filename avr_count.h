#define MARGIN 2
#define OFFSET 10
template<int N> struct Ln {enum {value = Ln<N >> 1>::value + 1};};
template<> struct Ln<1> {enum {value = 0};};
/* The indirection is needed for proper macro expansion. */
#define LIST_GENERATOR(name, x) name(x)
/* We define our "enum". */
#define commands(item)\
    item(rauf)\
    item(runter)\
    item(halt)\
    item(init)
