#lib files
aux_source_directory(. DIR_LIB_FDWS_SRCS)

#build fdws
add_library(fdws ${DIR_LIB_FDWS_SRCS})

#build shared lib
add_library(fdws_shared SHARED ${DIR_LIB_FDWS_SRCS})
