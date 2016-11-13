
scope Physics {

// Processes the player's physics
// Updates player position depending on velocity
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
scope ProcessEntity: {
    macro _addVelocity(c) {
        clc
        lda.b   BaseEntity.{c}Vecl
        adc.b   BaseEntity.{c}Pos
        sta.b   BaseEntity.{c}Pos

        lda.b   BaseEntity.{c}Vecl + 2
        adc.b   BaseEntity.{c}Pos + 2

        if BaseEntity.{c}Pos == BaseEntity.xPos {
            clc
            adc.w   #Camera.StarField.VELOCITY_INT
        }
        sta.b   BaseEntity.{c}Pos + 2
    }
    _addVelocity(x)
    _addVelocity(y)

    rts
}

}

// vim: ft=bass-65816 ts=4 sw=4 et:

