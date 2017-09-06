// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {

namespace BaseEnemy {
    basestruct(BaseEntity)
        field(hp, 2)
    endstruct()
}

namespace BaseEnemyData {
    basestruct(BaseEntityData)
        field(health, 2)
        field(initialVelocity, 4)
    endstruct()
}


namespace BaseEnemy {

// IN: A = parameter (currently undefined)
a16()
i16()
code()
function Init {
    ldx.b   BaseEnemy.romPtr

    lda.l   BaseEnemyData.initialVelocity,x
    sta.b   BaseEnemy.xVecl
    lda.l   BaseEnemyData.initialVelocity + 2,x
    sta.b   BaseEnemy.xVecl + 2

    lda.l   BaseEnemyData.health,x
    sta.b   BaseEnemy.hp

    rts
}


inline useDefaultDestructor() {
    constant Destructor = Entities.BaseEnemy.Destructor
}

a16()
i16()
code()
function Destructor {
    rts
}


a16()
i16()
code()
function Process {
    lda.b   BaseEnemy.hp
    bne     +
        // Entity is out of HP
        // Spawn explosion
        lda.b   BaseEnemy.xPos.px
        sta.w   Entity.Spawn.xPos

        lda.b   BaseEnemy.yPos.px
        sta.w   Entity.Spawn.yPos

        ldy.w   #0
        lda.w   #Entities.Particles.Explosion.ENTITY_ID
        jsr     Entity.Spawn

        ldy.w   #Entity.lists.toDelete
        sty.w   Entity.changeList
        rts
    +

    jmp     Physics.ProcessEntity
}


inline useDefaultCalculateAttackValue() {
    constant CalculateAttackValue = Entities.BaseEnemy.CalculateAttackValue
}

// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// OUTPUT: A = attack value
a16()
i16()
code()
function CalculateAttackValue {
    lda.w   #1
    rts
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// INPUT: A = attack value
a16()
i16()
code()
function TakeDamageFromEntity {
    // A = clamp(hp - A)

    eor.w   #0xffff
    sec
    adc.b   BaseEnemy.hp
    bcs     +
        lda.w   #0
    +
    sta.b   BaseEnemy.hp

    rts
}


// ::TODO collision behaviour::
a16()
i16()
code()
BlockWithShield:
CollideWithBody:
CollideWithShield:
    rts

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

