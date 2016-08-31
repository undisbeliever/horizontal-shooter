
scope Spawner {

    constant MIN_SPAWN_DELAY(75)        // 1 second
    constant MAX_SPAWN_DELAY(75 * 10)   // 10 seconds

    // delay to next enemy spawn
    // (word)
    allocate(spawnDelay, wram7e, 2)

    // id of previous spawn table
    // (word)
    allocate(previousTableId, wram7e, 2)


// REQUIRES: 16 bit A, DB = 0x7e
macro scope Init() {
    assert16a()

    lda.w   #1
    sta.w   Spawner.spawnDelay

    lda.w   #0xffff
    sta.w   Spawner.previousTableId
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
macro scope ProcessGameLoop() {
    assert16a()
    assert16i()

    // Spawn if no enemies on screen or --spawnDelay < 0

    lda.w   Entities.lists.enemies
    bne     DecrementCounter
    lda.w   Entities.lists.bosses
    bne     DecrementCounter
    bra     Spawn

    DecrementCounter:
        dec.w   Spawner.spawnDelay
        bpl     EndIf

    Spawn:
        jsr     Spawner.Spawn
    EndIf:
}


// Spawns a group of enemies according the data table
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e, Entities setup
a16()
i16()
scope Spawn: {
allocateTmpWord(tmp_address)
allocateTmpWord(tmp_count)

    lda.w   #Spawner.MAX_SPAWN_DELAY / 2
    sta.w   Spawner.spawnDelay


    // ::TODO select random table not of previous

    lda.w   previousTableId
    inc
    cmp.w   #SpawnTableList.N_ELEMENTS
    bcc     +
        lda.w   #0
    +
    sta.w   previousTableId

    asl
    tax
    lda.l   SpawnTableList,x
    tax


    lda.l   SpawnTableOffset,x
    and.w   #0x0f
    beq     Return
    sta.w   tmp_count


    Loop:
        phx
            lda.l   SpawnTableRow.xPos,x
            clc
            adc.w   Camera.xPos
            sta.w   Entities.Spawn.xPos

            lda.l   SpawnTableRow.yPos,x
            clc
            adc.w   Camera.yPos
            sta.w   Entities.Spawn.yPos

            lda.l   SpawnTableRow.parameter,x
            tay
            lda.l   SpawnTableRow.entityId,x

            jsr     Entities.Spawn

        pla
        clc
        adc.w   #SpawnTableRow.size
        tax

        dec.w   tmp_count
        bne     Loop

Return:
    rts
}



rodata(rom0)
constant SpawnTableOffset(pc() & 0xff0000)

scope SpawnTableRow {
    struct(SpawnTableOffset + 1)
        field(entityId, 2)
        field(parameter,2)
        field(xPos, 2)
        field(yPos, 2)
    endstruct()
}

scope SpawnTableData: {
    constant XSTART(Camera.DISPLAY_WIDTH + Camera.ACTIVE_WINDOW_EXTEND_HORIZONTAL / 2)
    evaluate count(0)

    macro table() {
        _{count}:
        global evaluate count({count} + 1)
    }

    macro row(entity, evaluate param, evaluate xPos, evaluate yPos) {
        dw  Entities.{entity}.ENTITY_ID, {param}, XSTART + {xPos}, {yPos}
    }

    table()
        db 12
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 0)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 1)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 2)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 3)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 4)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 5)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 6)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 7)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 8)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 9)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 10)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 11)

    table()
        db 12
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 0)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 1)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 2)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 3)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 4)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 5)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 6)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 7)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 8)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 9)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 10)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 11)

    table()
        db 12
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 0)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 1)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 2)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 3)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 4)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 5)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 6)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 7)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 8)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 9)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 10)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 11)

    table()
        db 12
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 0)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 1)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 2)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 3)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 4)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 5)
        row(EnemyFighter.Fast,      0,      0,      18 + 17 * 6)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 7)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 8)
        row(EnemyFighter.Medium,    0,      0,      18 + 17 * 9)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 10)
        row(EnemyFighter.Slow,      0,      0,      18 + 17 * 11)
}

scope SpawnTableList: {
    macro _repeat(evaluate n) {
        if {n} < {SpawnTableData.count} {
            dw  SpawnTableData._{n}

            _repeat({n} + 1)
        }
    }
    _repeat(0)

    constant N_ELEMENTS((pc() - SpawnTableList) / 2)
}


code()
}

// vim: ft=asm ts=4 sw=4 et:

