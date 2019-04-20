// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Physics {

// Processes the player's physics
// Updates player position depending on velocity
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function ProcessEntity {
    macro _addVelocity(c) {
        clc
        lda.b   BaseEntity.{c}Vecl
        adc.b   BaseEntity.{c}Pos.sx
        sta.b   BaseEntity.{c}Pos.sx

        lda.b   BaseEntity.{c}Vecl + 2
        adc.b   BaseEntity.{c}Pos.px

        if BaseEntity.{c}Pos.px == BaseEntity.xPos.px {
            clc
            adc.w   #Camera.StarField.VELOCITY_INT
        }
        sta.b   BaseEntity.{c}Pos.px
    }
    _addVelocity(x)
    _addVelocity(y)

    rts
}

}

// vim: ft=bass-65816 ts=4 sw=4 et:

