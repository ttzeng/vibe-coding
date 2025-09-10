# components/st7735/CMakeLists.txt
idf_component_register(SRCS "st7735.c" "st7735_font.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver)
