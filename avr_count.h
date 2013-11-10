/**********************/
/* Change as desired. */
/**********************/
#define MARGIN 2 // Allows that much pulses counted too much, may start at zero.
#define OFFSET 3 // Minimal address, may start at zero.
/* Will be expanded to an enum comprising the commands. */
#define commands(item)\
    item(rauf)\
    item(runter)\
    item(halt)\
    item(init)
/*****************************************************************************/
/* Don't change anything below if you don't exactly know what you are doing! */
/*****************************************************************************/
/* Sometimes, one pulse s missing, so we always sent one more. */
#define M (MARGIN + 2)
/* The indirection is needed for proper macro expansion. */
#define LIST_GENERATOR(name, x) name(x)
/* Make an anonymous enum. */
#define MAKE_ENUM(name) enum {LIST_GENERATOR(name, ENUM_ITEM) oneMoreThanLastEnum}
/* Make an enum item. */
#define ENUM_ITEM(name) name,
/* Now, we actually create the enum. */
MAKE_ENUM(commands);
