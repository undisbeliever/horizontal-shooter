// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {

namespace BaseEnemy {
    basestruct(BaseEntity)
        field(hp, 2)
    endstruct()
}

namespace BaseEnemy {

define RomStruct = Project.EntityRomStructs.BaseEnemyData

// IN: A = parameter (currently undefined)
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    ldx.b   BaseEnemy.romPtr

    lda.l   {RomStruct}.initialVelocity,x
    sta.b   BaseEnemy.xVecl
    lda.l   {RomStruct}.initialVelocity + 2,x
    sta.b   BaseEnemy.xVecl + 2

    lda.l   {RomStruct}.health,x
    sta.b   BaseEnemy.hp

    // always activate
    sec
    rts
}


inline useDefaultDestructor() {
    constant Destructor = Entities.BaseEnemy.Destructor
}
inline useDefaultDeactivated() {
    constant Deactivated = Entities.BaseEnemy.Deactivated
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

a16()
i16()
code()
function Deactivated {
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
        sta.w   SpawnEntity.xPos

        lda.b   BaseEnemy.yPos.px
        sta.w   SpawnEntity.yPos

        SpawnEntity(Explosion)

        jmp     Entity.DeleteAndGotoNextEntity
    +

    jsr     Physics.ProcessEntity

    jmp     Entity.DeleteIfOutsideOrDrawAndGotoNextEntity
}
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

