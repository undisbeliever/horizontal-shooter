
scope Entities {
scope PlayerEntity {

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

