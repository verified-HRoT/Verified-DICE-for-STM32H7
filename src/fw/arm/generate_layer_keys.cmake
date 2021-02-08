#
# Copyright (c) 2020 Microsoft. All rights reserved.
#

macro(generate_layer_keys)

    set(PARSE_KEY head -8 | tail -3 | sed 's/://g' | tr -d '\\n' | sed 's/ *//g')

    set(L0_IMAGE_KEY ${FW_DIR}/keys/L0_ED25519KEY.txt)
    set(L1_IMAGE_KEY ${FW_DIR}/keys/L1_ED25519KEY.txt)
    set(KEY_MANIFEST_KEY ${FW_DIR}/keys/KEY_MANIFEST_ED25519KEY.txt)

    add_custom_command(
        OUTPUT ${KEY_MANIFEST_KEY}
        COMMAND openssl genpkey -algorithm ed25519 -text | ${PARSE_KEY} 
            > ${KEY_MANIFEST_KEY}
    )

    add_custom_target(key_manifest_key ALL
        DEPENDS ${KEY_MANIFEST_KEY}
    )

    add_custom_command(
        OUTPUT ${L0_IMAGE_KEY}
        COMMAND openssl genpkey -algorithm ed25519 -text | ${PARSE_KEY}
            > ${L0_IMAGE_KEY}
    )
    
    add_custom_target(L0_IMAGE_KEY ALL
        DEPENDS ${L0_IMAGE_KEY}
    )
    
    add_custom_command(
        OUTPUT ${L1_IMAGE_KEY}
        COMMAND openssl genpkey -algorithm ed25519 -text | ${PARSE_KEY}
            > ${L1_IMAGE_KEY}
    )

    add_custom_target(L1_IMAGE_KEY ALL
        DEPENDS ${L1_IMAGE_KEY}
    )

endmacro()
