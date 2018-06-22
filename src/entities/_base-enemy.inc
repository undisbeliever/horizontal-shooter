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
inline useDefaultProcessCollision() {
    constant ProcessCollision = Entities.BaseEnemy.ProcessCollision
}

a16()
i16()
code()
function Destructor {
    rts
}


// A = MetaSprite.Collision.bits
// Y = MetaSprite.Collision.otherEntity
a16()
i16()
code()
function ProcessCollision {

    bit.w   #THIS_BODY
    beq     NoCollision

    bit.w   #OTHER_ATTACK | OTHER_BODY | OTHER_SHIELD
    beq     NoCollision
        // this body collided with other attack, body or shield

        lda.b   BaseEnemy.hp

        assert(THIS_WEAK == 0x80)
        bit.w   MetaSprite.Collision.bits - 1
        bmi     +
            // Enemy attack hit weak spot (remove 2 HP)
            dec
        +

        dec
        bpl     +
            lda.w   #0
        +
        sta.b   BaseEnemy.hp

NoCollision:
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

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

