
file(GLOB _files *.png)

install(FILES ${_files}
        DESTINATION ${DATA_INSTALL_DIR}/digikam/data
)
