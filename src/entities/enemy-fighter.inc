
scope Entities {
scope EnemyFighter {

buildFunctionTable(BaseEntityFunctionTable, EnemyFighter)
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


// IN: A = parameter (currently undefined)
a16()
i16()
code()
scope Init: {
    ldx.b   EntityStruct.romPtr

    lda.l   RomData.initialVelocity,x
    sta.b   EntityStruct.xVecl
    lda.l   RomData.initialVelocity + 1,x
    sta.b   EntityStruct.xVecl + 1

    lda.w   #MSEO.Enemy.Animations.Flying_Left
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


constant BOSS_LIST(1 * 2)
constant ENEMY_LIST(2 * 2)

macro _entity(name, frameSetId, velocity) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  ENEMY_LIST          // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dl  {velocity}          // launch velocity
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entities.Data.List) / 2)
            dw  RomData
    }
}
_entity(Slow,   EnemyFighter,   -0x011234)
_entity(Medium, EnemyFighter,   -0x015678)
_entity(Fast,   EnemyFighter,   -0x019abc)

}
}

// vim: ft=asm ts=4 sw=4 et:

