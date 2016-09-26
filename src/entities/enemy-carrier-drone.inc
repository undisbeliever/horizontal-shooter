
scope Entities {
scope EnemyCarrierDrone {

constant SCREEN_MARGIN_TOP(24)
constant SCREEN_MARGIN_BOTTOM(24)

constant DELAY_BETWEEN_SHOTS(90)

constant MISSILE_ACTION_POINT(1)

buildFunctionTable(BaseEntityFunctionTable, EnemyCarrierDrone)
Entity.useDefaultRenderer()
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultCalculateAttackValue()

scope State {
    createEnum(0, 2)
        enum(MOVE_TO_POSITION)
        enum(PATROL)

    constant MASK(0x1 << 1)

    code()
    FunctionTable:
        dw  Process.MoveToPosition
        dw  Process.Patrol

        assert(pc() - FunctionTable == MASK + 2)
}

scope EntityStruct {
    childstruct(BaseEnemy)
        field(state, 2)

        // if non-zero then entity is moving down
        field(movingUpOnZero, 1)

        // frame decrement counter
        field(counter, 2)

        // index offset for the path
        field(pathIndex, 1)
    endstruct()
}

scope RomData {
    childstruct(BaseEnemyData)
        // repeated until path.frames = 0
        scope Path {
            // (0:7:15 fixed point)
            constant xVecl(initialVelocity)
            // (0:7:15 fixed point)
            field(yVecl, 3)
            // number of movement frames
            // (byte - frames)
            field(frames, 1)

            assert(xVecl + 3 == yVecl)

            constant size(7)
        }
    endstruct()
}


// IN: A = parameter (boss address
//    MetaSprite.ActionPoint.address - address of actionPoint that spawned this entity
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.EnemyCarrierDrone.Animations.Flying
    jsr     MetaSprite.SetAnimation

    assert(State.MOVE_TO_POSITION == 0)
    stz.b   EntityStruct.state

    sep     #$20
a8()
    stz.b   EntityStruct.pathIndex

    // set up-down direction
    ldx.w   MetaSprite.ActionPoint.address
    lda.l   MetaSprite.Format.ActionPoints.yPos,x
    cmp.b   #INT_MS8_OFFSET
    bcs     +
        // moving up
        lda.b   #0
    +
    sta.b   EntityStruct.movingUpOnZero

    rep     #$30
a16()
    ldx.b   EntityStruct.romPtr

    lda.l   RomData.health,x
    sta.b   EntityStruct.hp

End:
}

// Loads the next path into the entity velocity/counter
// INPUT: dp = entity
// OUTPUT: n set if this is the last path
a16()
i16()
code()
scope _LoadNextPath: {
    assert(pc() == Init.End)

    // X = romPtr + pathIndex
    // entity.pathIndex += sizeof(RomData.Path)
    //
    // entity.movementFrames = X->path.frames
    //
    // if rs.movingUpOnZero != 0
    //      entity.yVecl = - X->path.yVecl
    // else:
    //      entity.yVecl = X->path.yVecl
    // entity.xVecl = X->path.xVecl

    sep     #$21
a8()
    lda.b   EntityStruct.pathIndex
    tax
    // c set
    adc.b   #RomData.Path.size - 1
    sta.b   EntityStruct.pathIndex

    txa
    rep     #$21
a16()
    and.w   #0xff
    // carry clear
    adc.b   EntityStruct.romPtr
    tax

    sep     #$21
a8()
    lda.b   EntityStruct.movingUpOnZero
    bne     MoveDown
        // moving down
        lda.l   RomData.Path.yVecl,x
        eor.b   #0xff
        // carry set
        adc.b   #0
        sta.b   EntityStruct.yVecl

        rep     #$20
    a16()
        lda.l   RomData.Path.yVecl + 1,x
        eor.w   #0xffff
        adc.w   #0

        bra     EndIf

    MoveDown:
    a8()
        // moving up
        lda.l   RomData.Path.yVecl,x
        sta.b   EntityStruct.yVecl

        rep     #$20
    a16()
        lda.l   RomData.Path.yVecl + 1,x
EndIf:
a16()
    sta.b   EntityStruct.yVecl + 1

    lda.l   RomData.Path.xVecl,x
    sta.b   EntityStruct.xVecl
    lda.l   RomData.Path.xVecl + 1,x
    sta.b   EntityStruct.xVecl + 1

Spot:
    lda.l   RomData.Path.frames,x
    and.w   #0xff
    sta.b   EntityStruct.counter

    rts
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
scope Process: {
    lda.b   EntityStruct.state
    and.w   #State.MASK
    tax
    jmp     (State.FunctionTable,x)
}


// Move drone to desired position on screen and holds it there
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
scope Process.MoveToPosition: {
    // entity.counter--
    // if entity.counter == 0:
    //      _LoadNextPath
    //      if no more paths:
    //          entity.state = PATROL

    dec.b   EntityStruct.counter
    bne     +
        jsr     _LoadNextPath
        bne     +
            // no more paths
            ldx.w   #State.PATROL
            stx.b   EntityStruct.state
    +

    jmp     BaseEnemy.Process
}


// Default Carrier behaviour
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
scope Process.Patrol: {
    // Invert Y velocity when hit margins

    lda.b   EntityStruct.yVecl.hw
    bpl     MovingDown
        // Moving Up
        lda.w   Camera.yPos
        clc
        adc.w   #SCREEN_MARGIN_TOP
        cmp.b   EntityStruct.yPos.px
        bcc     EndIf
            bra     InvertYVelocity

    MovingDown:
        // Moving Down
        lda.w   Camera.yPos
        clc
        adc.w   #Camera.DISPLAY_HEIGHT - SCREEN_MARGIN_BOTTOM
        cmp.b   EntityStruct.yPos.px
        bcs     EndIf

        InvertYVelocity:
            sep     #$21
        a8()
            // carry set
            lda.b   #0
            sbc.b   EntityStruct.yVecl
            sta.b   EntityStruct.yVecl

            rep     #$20
        a16()
            lda.w   #0
            sbc.b   EntityStruct.yVecl + 1
            sta.b   EntityStruct.yVecl + 1

    EndIf:


    // Shoot every DELAY_BETWEEN_SHOTS frames
    lda.b   EntityStruct.counter
    bne     DontShoot

        lda.w   #MISSILE_ACTION_POINT
        ldy.w   #Missile.EnemyPlasma.ENTITY_ID
        jsr     Entity.SpawnEnemyProjectileFromActionPoint

        lda.w   #DELAY_BETWEEN_SHOTS + 1

DontShoot:
    dec
    sta.b   EntityStruct.counter

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


scope FrontBay {
    rodata(EN_RomData)
    scope CarrierDroneRomData: {
        dw  FunctionTable
        db  Entity.ListId.ENEMY     // initial List
        dw  MSFS.EnemyCarrierDrone  // frameSetId
        db  0                       // defaultPalette
        dw  2                       // health

        dl  0x000000                // xVecl
        dl  0x007777                // yVecl
        db  45                      // frame count

        dl  -0x07777                // xVecl
        dl  0x003bbb                // yVecl
        db  55                      // frame count

        dl  0                       // xVecl
        dl  0x011000                // yVecl
        db  0
    }

    rodata(EN_RomDataList)
    constant ENTITY_ID((pc() - Entity.Data.List) / 2)
        dw  CarrierDroneRomData
}

scope RearBay {
    rodata(EN_RomData)
    scope CarrierDroneRomData: {
        dw  FunctionTable
        db  Entity.ListId.ENEMY     // initial List
        dw  MSFS.EnemyCarrierDrone  // frameSetId
        db  0                       // defaultPalette
        dw  2                       // health

        dl  0x006666                // xVecl
        dl  0x000000                // yVecl
        db  50                      // frame count

        dl  0x005555                // xVecl
        dl  0x003bbb                // yVecl
        db  50                      // frame count

        dl  0                       // xVecl
        dl  0x011000                // yVecl
        db  0
    }

    rodata(EN_RomDataList)
    constant ENTITY_ID((pc() - Entity.Data.List) / 2)
        dw  CarrierDroneRomData
}

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:
