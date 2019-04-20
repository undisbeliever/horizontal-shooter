// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace EnemyFighter {

buildFunctionTable(BaseEntityFunctionTable, EnemyFighter)
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultDeactivated()
BaseEnemy.useDefaultProcessCollision()


namespace EntityStruct {
    childstruct(BaseEnemy)
    endstruct()
}


// IN: A = parameter (currently undefined)
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    SetAnimation(Flying_Left)

    jmp     BaseEnemy.Init
}


a16()
i16()
code()
function Process {
    jmp     BaseEnemy.Process
}
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

