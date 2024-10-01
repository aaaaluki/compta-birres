set(SOURCE_FILES
    "main.c"
    "compta_birres.c"
    "sample_tasks.c"
    "cbutils.c"
    )

idf_component_register(SRCS "${SOURCE_FILES}"
                    INCLUDE_DIRS ".")
