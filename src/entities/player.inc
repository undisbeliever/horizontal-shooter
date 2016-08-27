
scope Entities {
scope PlayerEntity {

// Player Velocity
// (0:7:15 fixed point)
constant X_VECL(0x010000)
constant Y_VECL(0x018000)


buildFunctionTable(BaseEntityFunctionTable, PlayerEntity)
Entities.useDefaultRenderer()

scope EntityStruct: {
    childstruct(BaseEntity)
    endstruct()
}

// IN: A = parameter (frameDelay)
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.Player.Animations.Straight
    jsr     MetaSprite.SetAnimation

    rts
}

a16()
i16()
code()
scope Destructor: {
    rts
}


a16()
i16()
code()
scope Process: {
    scope UpdateYVecl {
        lda.l   JOY1
        bit.w   #JOY.up
        beq     ElseTestDown
            // Up pressed

            lda.w   #MSEO.Player.Animations.Up
            jsr     MetaSprite.SetAnimation

            lda.w   #-Y_VECL
            sta.b   EntityStruct.yVecl
            lda.w   #(-Y_VECL) >> 8
            sta.b   EntityStruct.yVecl + 1

            bra     EndIf

    ElseTestDown:
        bit.w   #JOY.down
        beq     Else
            // Down pressed

            lda.w   #MSEO.Player.Animations.Down
            jsr     MetaSprite.SetAnimation

            lda.w   #Y_VECL
            sta.b   EntityStruct.yVecl
            lda.w   #(Y_VECL) >> 8
            sta.b   EntityStruct.yVecl + 1

            bra     EndIf
    Else:
        // Up/Down not pressed

        lda.w   #MSEO.Player.Animations.Straight
        jsr     MetaSprite.SetAnimation

        stz.b   EntityStruct.yVecl
        stz.b   EntityStruct.yVecl + 1

    EndIf:
    }

    scope UpdateXVecl {
        lda.l   JOY1
        bit.w   #JOY.left
        beq     ElseTestRight
            // Left pressed

            lda.w   #-X_VECL
            sta.b   EntityStruct.xVecl
            lda.w   #(-X_VECL) >> 8
            sta.b   EntityStruct.xVecl + 1

            bra     EndIf

    ElseTestRight:
        bit.w   #JOY.right
        beq     Else
            // Right pressed

            lda.w   #X_VECL
            sta.b   EntityStruct.xVecl
            lda.w   #X_VECL >> 8
            sta.b   EntityStruct.xVecl + 1

            bra     EndIf
    Else:
        // Left/Right not pressed

        stz.b   EntityStruct.xVecl
        stz.b   EntityStruct.xVecl + 1

    EndIf:
    }


    macro _addVelocity(c) {
        clc
        lda.b   EntityStruct.{c}Vecl
        adc.b   EntityStruct.{c}Pos
        sta.b   EntityStruct.{c}Pos

        lda.b   EntityStruct.{c}Vecl + 1
        xba
        bpl     +
            ora     #$ff00
            bra     ++
        +
            and     #$00ff
        +
        adc.b   EntityStruct.{c}Pos + 2
        sta.b   EntityStruct.{c}Pos + 2
    }
    _addVelocity(x)
    _addVelocity(y)

    rts
}


rodata(EN_RomData)
scope RomData: {
    dw  FunctionTable
    db  0               // initial List (player) ::TODO name these lists::
    dw  MSFS.Player     // frameSetId
    db  0               // defaultPalette
}

rodata(EN_RomDataList)
constant ENTITY_ID((pc() - Entities.Data.List) / 2)
    dw  RomData
}
}

// vim: ft=asm ts=4 sw=4 et:

