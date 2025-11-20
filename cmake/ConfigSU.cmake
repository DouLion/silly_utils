# 将 prepare.h.in 配置为 prepare.h
configure_file(
    ${SU_FILE_ROOT}/su/su_config.h.in
    ${SU_FILE_ROOT}/su/su_config.h
    @ONLY
)