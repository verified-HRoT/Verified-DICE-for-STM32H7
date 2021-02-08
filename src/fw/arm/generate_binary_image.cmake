#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

macro(generate_binary_image
        has_signed_header
        next_image_offset # 0 If it doesn't matter
)

    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} \
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_NAME}.map \
        -L ${FW_DIR}/common/arm \
        -T ${CMAKE_CURRENT_SOURCE_DIR}/${LINKERSCRIPT}"
    )

    # Generate a BIN file from the ELF file

    add_custom_command(
        OUTPUT ${IMAGE_BIN}
        DEPENDS ${IMAGE_ELF}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${IMAGE_ELF} ${IMAGE_BIN}
    )

    add_custom_target(${IMAGE_NAME}_bin ALL
        DEPENDS ${IMAGE_BIN}
    )

    # Generate a header if requested

    if(${has_signed_header})

        set(IMAGE_HEADER ${IMAGE_NAME}_header.bin)
        set(IMAGE_SIGNATURE ${IMAGE_NAME}_signature.bin)
        set(IMAGE_HEADER_SIGNED ${IMAGE_NAME}_header_signed.bin)

        # Generate an unsigned header from the BIN file

        add_custom_command(
            OUTPUT ${IMAGE_HEADER}
            DEPENDS ${IMAGE_BIN} headertool
            COMMAND ${CMAKE_BINARY_DIR}/src/fw/headertool/headertool
                            ${IMAGE_VERSION} ${IMAGE_BIN} ${IMAGE_HEADER}
                            ${next_image_offset}
        )

        add_custom_target(${IMAGE_NAME}_header ALL
            DEPENDS ${IMAGE_HEADER}
        )

        # Generate header signature

        add_custom_command(
            OUTPUT ${IMAGE_SIGNATURE}
            DEPENDS ${IMAGE_HEADER} signtool
            COMMAND ${CMAKE_BINARY_DIR}/src/fw/signtool/signtool
                            ${IMAGE_KEY} ${IMAGE_HEADER} ${IMAGE_SIGNATURE}
        )

        add_custom_target(${IMAGE_NAME}_signature ALL
            DEPENDS ${IMAGE_SIGNATURE}
        )

        # Generate signed header

        add_custom_command(
            OUTPUT ${IMAGE_HEADER_SIGNED}
            DEPENDS ${IMAGE_HEADER} ${IMAGE_SIGNATURE}
            COMMAND cat ${IMAGE_HEADER} ${IMAGE_SIGNATURE} > ${IMAGE_HEADER_SIGNED}
        )

        add_custom_target(${IMAGE_NAME}_header_signed ALL
            DEPENDS ${IMAGE_HEADER_SIGNED}
        )

    endif()

endmacro()
