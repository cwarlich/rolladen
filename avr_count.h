/**********************/
/* Change as desired. */
/**********************/
#define MARGIN 4 // Allows half of that much pulses error, may start at zero.
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
/* Sometimes, one pulses are missing or counted too */
/* much, so we always send half of the margin more. */
#define M (MARGIN + 1 + MARGIN / 2)
/* The indirection is needed for proper macro expansion. */
#define LIST_GENERATOR(name, x) name(x)
/* Make an anonymous enum. */
#define MAKE_ENUM(name) enum {LIST_GENERATOR(name, ENUM_ITEM) oneMoreThanLastEnum}
/* Make an enum item. */
#define ENUM_ITEM(name) name,
/* Now, we actually create the enum. */
MAKE_ENUM(commands);
