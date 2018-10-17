// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace EnemyFighter {

buildFunctionTable(BaseEntityFunctionTable, EnemyFighter)
Entity.useDefaultRenderer()
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultProcessCollision()


namespace EntityStruct {
    childstruct(BaseEnemy)
    endstruct()
}

namespace RomData {
    childstruct(BaseEnemyData)
    endstruct()
}


// IN: A = parameter (currently undefined)
a16()
i16()
code()
function Init {
    lda.w   #MSEO.Enemy.Animations.Flying_Left
    jsr     MetaSprite.SetAnimation

    jmp     BaseEnemy.Init
}


a16()
i16()
code()
function Process {
    jmp     BaseEnemy.Process
}


inline _entity(name, frameSetId, velocity) {
    namespace {name} {
        rodata(EN_RomData)
        RomData:
            dw  FunctionTable
            db  Entity.ListId.ENEMY // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dw  3                   // health
            dd  {velocity}          // launch velocity


        rodata(EN_EntityList)
        constant ENTITY_ID = (pc() - Entity.Data.EntityList) / 2
            dw  RomData
    }
}
_entity(Slow,   EnemyFighter,   -0x011234)
_entity(Medium, EnemyFighter,   -0x015678)
_entity(Fast,   EnemyFighter,   -0x019abc)

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

