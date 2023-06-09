set(srcs
    "main.c"
)

set(reqs
    "OwnN2K"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ""
    REQUIRES ${reqs}
)
