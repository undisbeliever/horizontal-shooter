
scope Entities {
scope Particles {

buildFunctionTable(BaseEntityFunctionTable, Particles)
Entity.useDefaultRenderer()

// IN: A = parameter (address entity)
//    MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.Particle.Animations.Animation
    jmp     MetaSprite.SetAnimation
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
    // Delete entity is animation has stopped

    lda.b   BaseEntity.MetaSprite.Animation.pc
    bne     +
        ldy.w   #Entity.lists.toDelete
        sty.w   Entity.changeList
    +
    rts
}


// No Collisions
a16()
i16()
code()
BlockWithShield:
TakeDamageFromEntity:
CollideWithBody:
CollideWithShield:
scope CalculateAttackValue: {
    rts
}


macro _entity(name, frameSetId, palette) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  Entity.ListId.PARTICLE  // initial List
            dw  MSFS.{frameSetId}       // frameSetId
            db  {palette}               // defaultPalette
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entity.Data.List) / 2)
            dw  RomData
    }
}
_entity(Explosion, Explosion, 0)

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

