
scope Entities {
scope EnemyFighter {

buildFunctionTable(BaseEntityFunctionTable, EnemyFighter)
Entity.useDefaultRenderer()
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultCalculateAttackValue()


scope EntityStruct: {
    childstruct(BaseEnemy)
    endstruct()
}

scope RomData: {
    childstruct(BaseEnemyData)
    endstruct()
}


// IN: A = parameter (currently undefined)
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.Enemy.Animations.Flying_Left
    jsr     MetaSprite.SetAnimation

    jmp     BaseEnemy.Init
}


a16()
i16()
code()
scope Process: {
    jmp     BaseEnemy.Process
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// INPUT: A = attack value
a16()
i16()
code()
scope TakeDamageFromEntity: {
    jmp     BaseEnemy.TakeDamageFromEntity
}


// ::TODO collision behaviour::
a16()
i16()
code()
BlockWithShield:
CollideWithBody:
CollideWithShield:
    rts


macro _entity(name, frameSetId, velocity) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  Entity.ListId.ENEMY // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dw  3                   // health
            dl  {velocity}          // launch velocity
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entity.Data.List) / 2)
            dw  RomData
    }
}
_entity(Slow,   EnemyFighter,   -0x011234)
_entity(Medium, EnemyFighter,   -0x015678)
_entity(Fast,   EnemyFighter,   -0x019abc)

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

