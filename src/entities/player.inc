
scope Entities {
scope PlayerEntity {

constant SCREEN_MARGIN_TOP(8)
constant SCREEN_MARGIN_BOTTOM(8)
constant SCREEN_MARGIN_LEFT(12)
constant SCREEN_MARGIN_RIGHT(60)

// Friction
// (0:0:15 fixed point)
constant FRICTION(0x6666)

// Player Acceleration
// (0:0:15 fixed point)
constant X_ACCEL(FRICTION + 0x1234)
constant Y_ACCEL(FRICTION + 0x3456)

// Max Player Velocity
// (0:7:7 fixed point)
constant MAX_X_VECL(0x0140)
constant MAX_Y_VECL(0x0200)

// cooldown between missiles
// (int frames)
constant MISSILE_COOLDOWN(14)

// The action point type of a missile
constant MISSILE_ACTION_POINT(1)

buildFunctionTable(BaseEntityFunctionTable, PlayerEntity)
Entity.useDefaultRenderer()

scope EntityStruct: {
    childstruct(BaseEntity)
        field(missileCooldown, 2)
    endstruct()
}


// IN: A = parameter (NONE)
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.Player.Animations.Straight
    jsr     MetaSprite.SetAnimation

    stz.b   EntityStruct.missileCooldown

    rts
}

a16()
i16()
code()
scope Destructor: {
    rts
}


// Change velocity depending on state of dpad
// Velocity is capped to MAX_*_VECL
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro ProcessDpad() {
    assert16a()
    assert16i()

    macro _AddAcceleration(c, accel, maxVecl) {
        scope {#} {
            lda.b   EntityStruct.{c}Vecl.hw
            bmi     Minus
                cmp.w   #{maxVecl}
                bcs     End

        Minus:
            lda.b   EntityStruct.{c}Vecl
            clc
            adc.w   #{accel}
            sta.b   EntityStruct.{c}Vecl
            bcc     End
                sep     #$20
                inc.b   EntityStruct.{c}Vecl + 2
                rep     #$20
        End:
        }
    }

    macro _SubAcceleration(c, accel, maxVecl) {
        scope {#} {
            lda.b   EntityStruct.{c}Vecl.hw
            bpl     Positive
                cmp.w   #-{maxVecl}
                bcc     End

        Positive:
            lda.b   EntityStruct.{c}Vecl
            sec
            sbc.w   #{accel}
            sta.b   EntityStruct.{c}Vecl
            bcs     End
                sep     #$20
                dec.b   EntityStruct.{c}Vecl + 2
                rep     #$20
        End:
        }
    }

    scope UpdateYVecl {
        lda.w   Controller.Joy1.current
        bit.w   #JOY.up
        beq     ElseTestDown
            // Up pressed
            lda.w   #MSEO.Player.Animations.Up
            jsr     MetaSprite.SetAnimation

            _SubAcceleration(y, Y_ACCEL, MAX_Y_VECL)

            bra     EndIf

    ElseTestDown:
        bit.w   #JOY.down
        beq     Else
            // Down pressed

            lda.w   #MSEO.Player.Animations.Down
            jsr     MetaSprite.SetAnimation

            _AddAcceleration(y, Y_ACCEL, MAX_Y_VECL)

            bra     EndIf
    Else:
        // Up/Down not pressed

        lda.w   #MSEO.Player.Animations.Straight
        jsr     MetaSprite.SetAnimation

    EndIf:
    }

    scope UpdateXVecl {
        lda.w   Controller.Joy1.current
        bit.w   #JOY.left
        beq     ElseTestRight
            // Left pressed

            _SubAcceleration(x, X_ACCEL, MAX_X_VECL)

            bra     EndIf

    ElseTestRight:
        bit.w   #JOY.right
        beq     EndIf
            // Right pressed

            _AddAcceleration(x, X_ACCEL, MAX_X_VECL)

            bra     EndIf

    EndIf:
    }
}



// Apply friction to entity
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro ProcessFriction() {
    assert16a()
    assert16i()

    macro _handleFriction(c) {
        scope Friction_{c} {
            lda.b   EntityStruct.{c}Vecl.hw
            bmi     Minus
                // Positive Velocity
                // subtract friction

                sec
                lda.b   EntityStruct.{c}Vecl
                sbc.w   #FRICTION
                tay

                sep     #$20
            a8()
                lda.b   EntityStruct.{c}Vecl + 2
                sbc.b   #0
                bpl     +
                    lda.b   #0
                    ldy.w   #0
            +
                sty.b   EntityStruct.{c}Vecl
                sta.b   EntityStruct.{c}Vecl + 2

                bra     End

        Minus:
                // Negative velocity
                // add friction

                clc
                lda.b   EntityStruct.{c}Vecl
                adc.w   #FRICTION
                tay

                sep     #$20
            a8()
                lda.b   EntityStruct.{c}Vecl + 2
                adc.b   #0
                bmi     +
                    lda.b   #0
                    ldy.w   #0
            +
                sty.b   EntityStruct.{c}Vecl
                sta.b   EntityStruct.{c}Vecl + 2

        End:
            rep     #$20
        a16()
        }
    }
    _handleFriction(x)
    _handleFriction(y)
}


// Prevent the entity from leaving the screen
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro LimitToMargin() {
    assert16a()
    assert16i()

    macro _limitToMargin(c, evaluate min, evaluate max) {
        scope Limit_{c} {
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
macro scope SpawnMissilesOnButtonPress() {
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
            ldy.w   #Missile.PlayerPlasma.ENTITY_ID
            jsr     MetaSprite.ActionPoint.SpawnEntityOfType


            lda.w   #MISSILE_COOLDOWN
            sta.b   EntityStruct.missileCooldown
EndIf:
}


a16()
i16()
code()
scope Process: {
    ProcessDpad()
    ProcessFriction()

    jsr     Physics.ProcessEntity

    LimitToMargin()

    SpawnMissilesOnButtonPress()

    rts
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// OUTPUT: A = attack value
a16()
i16()
code()
scope CalculateAttackValue: {
    // ::TODO proper value::

    lda.w   #1
    rts
}


// ::TODO collision behaviour::
a16()
i16()
code()
BlockWithShield:
CollideWithBody:
CollideWithShield:
TakeDamageFromEntity:
    rts


rodata(EN_RomData)
scope RomData: {
    dw  FunctionTable
    db  Entity.ListId.PLAYER    // initial List
    dw  MSFS.Player             // frameSetId
    db  0                       // defaultPalette
}

rodata(EN_RomDataList)
constant ENTITY_ID((pc() - Entity.Data.List) / 2)
    dw  RomData
}
}

// vim: ft=asm ts=4 sw=4 et:

