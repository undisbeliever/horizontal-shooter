
scope Entities {
scope Missile {

// Missile X Velocity
// (0:7:16 fixed point)
constant X_VELOCITY(0x028000)


buildFunctionTable(BaseEntityFunctionTable, Missile)
Entities.useDefaultRenderer()

scope EntityStruct: {
    childstruct(BaseEntity)
    endstruct()
}

scope RomData: {
    childstruct(BaseEntityData)
        field(initialVelocity, 3)
    endstruct()
}


// IN: A = parameter (address entity)
//    MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
a16()
i16()
code()
scope Init: {
    ldx.w   MetaSprite.ActionPoint.address
    lda.l   MetaSprite.Format.ActionPoints.xPos,x
    and.w   #0x00ff
    cmp.w   #INT_MS8_OFFSET
    bcs     Right

        // Missile travels left
        // load negative velocity
        ldx.b   EntityStruct.romPtr

        sec
        lda.w   #0
        sbc.l   RomData.initialVelocity,x
        sta.b   EntityStruct.xVecl

        sep     #$20
    a8()
        lda.b   #0
        sbc.l   RomData.initialVelocity + 2,x
        sta.b   EntityStruct.xVecl + 2
        rep     #$30
    a16()

        lda.w   #MSEO.Missile.Animations.Move_Left

        bra     EndIf

    Right:
        // missile travels right
        ldx.b   EntityStruct.romPtr

        lda.l   RomData.initialVelocity,x
        sta.b   EntityStruct.xVecl
        lda.l   RomData.initialVelocity + 1,x
        sta.b   EntityStruct.xVecl + 1

        lda.w   #MSEO.Missile.Animations.Move_Right

EndIf:
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

        if EntityStruct.{c}Pos == EntityStruct.xPos {
            clc
            adc.w   #Camera.StarField.VELOCITY_INT
        }

        sta.b   EntityStruct.{c}Pos + 2
    }
    _addVelocity(x)
    _addVelocity(y)

    rts
}


constant PLAYER_PROJECTILE(3 * 2)
constant ENEMY_PROJECTILE(4 * 2)

macro _entity(name, frameSetId, list, velocity) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  {list}              // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dl  {velocity}          // launch velocity
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entities.Data.List) / 2)
            dw  RomData
    }
}
_entity(PlayerPlasma, Plasma, PLAYER_PROJECTILE, 0x028000)

}
}

// vim: ft=asm ts=4 sw=4 et:
