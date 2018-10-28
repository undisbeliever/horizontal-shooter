// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace PlayerEntity {

constant SCREEN_MARGIN_TOP = 8
constant SCREEN_MARGIN_BOTTOM = 8
constant SCREEN_MARGIN_LEFT = 12
constant SCREEN_MARGIN_RIGHT = 60

// Friction
// (0:0:15 fixed point)
constant FRICTION = 0x6666

// Repeat dead explosion frame count
// (int frames)
constant DEAD_EXPLOSION_FRAME_DELAY = 25

// Player Acceleration
// (0:0:15 fixed point)
constant X_ACCEL = FRICTION + 0x1234
constant Y_ACCEL = FRICTION + 0x3456

// Max Player Velocity
// (0:15:16 fixed point)
constant MAX_X_VECL = 0x014000
constant MAX_Y_VECL = 0x020000

// cooldown between missiles
// (int frames)
constant MISSILE_COOLDOWN = 20

// The action point type of a missile
constant MISSILE_ACTION_POINT = 1

buildFunctionTable(BaseEntityFunctionTable, PlayerEntity)

namespace EntityStruct {
    childstruct(BaseEntity)
        field(hp, 2)

        // reused for explosion delay
        field(missileCooldown, 2)
    endstruct()
}

namespace RomData {
    childstruct(BaseEntityData)
        field(health, 2)
    endstruct()
}


// IN: A = parameter (NONE)
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    lda.w   #MSEO.Player.Animations.Straight
    jsr     MetaSprite.SetAnimation

    stz.b   EntityStruct.missileCooldown

    ldx.b   EntityStruct.romPtr
    lda.l   RomData.health,x
    sta.b   EntityStruct.hp

    // always activate
    sec
    rts
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

        lda.b   EntityStruct.hp

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
        sta.b   EntityStruct.hp

NoCollision:

    rts
}


// Change velocity depending on state of dpad
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro ProcessDpad() {
    assert16a()
    assert16i()

    macro _AddAccel(c, evaluate a) {
        clc
        lda.b   EntityStruct.{c}Vecl
        adc.w   #{a}
        sta.b   EntityStruct.{c}Vecl

        lda.b   EntityStruct.{c}Vecl + 2
        adc.w   #{a} >> 16
        sta.b   EntityStruct.{c}Vecl + 2
    }

    namespace UpdateYVecl {
        lda.w   Controller.Joy1.current
        bit.w   #JOY.up
        beq     ElseTestDown
            // Up pressed
            lda.w   #MSEO.Player.Animations.Up
            jsr     MetaSprite.SetAnimation

            _AddAccel(y, -Y_ACCEL)

            bra     EndIf

    ElseTestDown:
        bit.w   #JOY.down
        beq     Else
            // Down pressed

            lda.w   #MSEO.Player.Animations.Down
            jsr     MetaSprite.SetAnimation

            _AddAccel(y, Y_ACCEL)

            bra     EndIf
    Else:
        // Up/Down not pressed

        lda.w   #MSEO.Player.Animations.Straight
        jsr     MetaSprite.SetAnimation

    EndIf:
    }

    namespace UpdateXVecl {
        lda.w   Controller.Joy1.current
        bit.w   #JOY.left
        beq     ElseTestRight
            // Left pressed

            _AddAccel(x, -X_ACCEL)

            bra     EndIf

    ElseTestRight:
        bit.w   #JOY.right
        beq     EndIf
            // Right pressed

            _AddAccel(x, X_ACCEL)

            bra     EndIf

    EndIf:
    }
}



// Apply friction and velocity cap to entity.
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro ProcessFriction() {
    assert16a()
    assert16i()

    macro _handleFriction(c, evaluate friction, evaluate maxVecl) {
        namespace Friction_{c} {
            // if velocity > 0:
            //   velocity -= FRICTION
            //   if velocity < 0:
            //      velocity = 0
            //   else if velocity > MAX:
            //      velocity = MAX
            // else:
            //   velocity += FRICTION
            //   if velocity >= 0:
            //      velocity = 0
            //   else if velocity < -MAX:
            //      velocity = -MAX

            lda.b   EntityStruct.{c}Vecl
            ldx.b   EntityStruct.{c}Vecl + 2
            bmi     Minus

            namespace Positive {
                // A = lw, X = hw
                sec
                sbc.w   #{friction}
                tay

                txa
                sbc.w   #{friction} >> 16

                bmi     Zero

                // Y = lw, A = hw

                cmp.w   #{maxVecl} >> 16
                bcc     Store
                bne     Clamp
                cpy.w   #{maxVecl}
                bcc     Store

            Clamp:
                ldy.w   #{maxVecl}
                lda.w   #{maxVecl} >> 16

            Store:
                sty.b   EntityStruct.{c}Vecl
                sta.b   EntityStruct.{c}Vecl + 2
                bra     End

            Zero:
                stz.b   EntityStruct.{c}Vecl
                stz.b   EntityStruct.{c}Vecl + 2
                bra     End
            }

            Minus:
            namespace Minus {
                // A = lw, X = hw

                clc
                adc.w   #{friction}
                tay

                txa
                adc.w   #{friction} >> 16

                bpl     Zero

                // Y = lw, A = hw

                cmp.w   #(-{maxVecl}) >> 16
                bcc     Clamp
                bne     Store
                cpy.w   #-{maxVecl}
                bcs     Store

            Clamp:
                ldy.w   #-{maxVecl}
                lda.w   #(-{maxVecl}) >> 16

            Store:
                sty.b   EntityStruct.{c}Vecl
                sta.b   EntityStruct.{c}Vecl + 2
                bra     End

            Zero:
                stz.b   EntityStruct.{c}Vecl
                stz.b   EntityStruct.{c}Vecl + 2
                bra     End
            }
        End:
        }
    }
    _handleFriction(x, FRICTION, MAX_X_VECL)
    _handleFriction(y, FRICTION, MAX_Y_VECL)
}


// Prevent the entity from leaving the screen
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro LimitToMargin() {
    assert16a()
    assert16i()

    macro _limitToMargin(c, evaluate min, evaluate max) {
        namespace Limit_{c} {
            lda.w   Camera.{c}Pos
            clc
            adc.w   #{min}
            cmp.b   EntityStruct.{c}Pos.px
            bcs     OutsideMargin

            // carry clear
            adc.w   #{max} - {min}
            cmp.b   EntityStruct.{c}Pos.px
            bcs     InsideMargin

        OutsideMargin:
            sta.b   EntityStruct.{c}Pos.px

        InsideMargin:
        }
    }
    _limitToMargin(x, SCREEN_MARGIN_LEFT, Camera.DISPLAY_WIDTH - SCREEN_MARGIN_RIGHT)
    _limitToMargin(y, SCREEN_MARGIN_TOP, Camera.DISPLAY_HEIGHT - SCREEN_MARGIN_BOTTOM)
}


// Spawn missile on button press (with cooldown timer)
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro SpawnMissilesOnButtonPress() {
    lda.b   EntityStruct.missileCooldown
    beq     TestButton
        dec
        sta.b   EntityStruct.missileCooldown
        bra     EndIf

    TestButton:
        lda.w   Controller.Joy1.current
        bit.w   #JOY.b | JOY.y
        beq     EndIf
            lda.w   #MISSILE_ACTION_POINT
            ldy.w   #Missile.PlayerPlasma.PROJECTILE_ID
            jsr     Entity.SpawnPlayerProjectileFromActionPoint

            lda.w   #MISSILE_COOLDOWN
            sta.b   EntityStruct.missileCooldown
EndIf:
}


// Spawns multiple explosions throughout the ship
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// DP = entity
a16()
i16()
code()
function Process.Dead {
allocateTmpWord(explosionLocation)
constant explosionLoc.x = explosionLocation
constant explosionLoc.y = explosionLocation + 1

    lda.b   EntityStruct.missileCooldown
    dec
    bpl     SkipExplosion

        // EXTREMELY BASIC explosion "randomizer"
        //   explosionLoc.x += 23
        //   explosionLoc.y += 37
        //   spawn.xPos = entity.xPos - 8 + (explosionLoc.x & 15)
        //   spawn.yPos = entity.yPos - 4 + (explosionLoc.y & 7)

        assert(explosionLoc.y == explosionLoc.x + 1)

        lda.w   explosionLoc.x
        // ignore carry
        adc.w   #37 << 8 | 23
        sta.w   explosionLoc.x

        and.w   #15
        sec
        sbc.w   #8
        clc
        adc.b   EntityStruct.xPos.px
        sta.w   Entity.Spawn.xPos

        lda.w   explosionLoc.y
        and.w   #7
        sec
        sbc.w   #4
        clc
        adc.b   EntityStruct.yPos.px
        sta.w   Entity.Spawn.yPos

        ldy.w   #0
        lda.w   #Entities.Particles.Explosion.ENTITY_ID
        jsr     Entity.Spawn

        // reset explosion counter
        lda.w   #DEAD_EXPLOSION_FRAME_DELAY - 1

SkipExplosion:
    sta.b   EntityStruct.missileCooldown

    jmp     Entity.DrawAndGotoNextEntity
}

// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// DP = entity
a16()
i16()
code()
function Process {
    lda.b   EntityStruct.hp
    beq     Dead

    ProcessDpad()
    ProcessFriction()

    jsr     Physics.ProcessEntity

    LimitToMargin()

    SpawnMissilesOnButtonPress()

    jmp     Entity.DrawAndGotoNextEntity
}


rodata(EN_RomData)
PlayerRomData:
    dw  FunctionTable
    db  Entity.ListId.PLAYER    // initial List
    db  0                       // defaultPalette
    dw  MSFS.Player             // frameSetId
    dw  6                       // health


rodata(EN_EntityList)
constant ENTITY_ID = (pc() - Entity.Data.EntityList) / 2
    dw  PlayerRomData
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

