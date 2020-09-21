package itmo.se.lab1

import org.junit.jupiter.api.Test
import org.junit.jupiter.api.Assertions

class DomainModelTest {
    @Test
    fun `characters are located in the airlock at t=0`() {
        val env = DomainModel.STORY[FictionalTime(chapter = 7, word = 0)]!!
        val airlockEntity = env.entities.entries.find { (_, e) -> e is Airlock }

        Assertions.assertNotNull(airlockEntity)
        val (airlockLocation, airlock) = airlockEntity!!
        Assertions.assertEquals(AirlockState.Closed, (airlock as Airlock).state)

        val insideAirlock = env.getChildEntities(airlockLocation, airlock).toList()
        Assertions.assertEquals(2, insideAirlock.size)

        val arthur = insideAirlock.find { (_, e) -> e is Person && e.name == "Arthur" }
        Assertions.assertNotNull(arthur)

        val ford = insideAirlock.find { (_, e) -> e is Person && e.name == "Ford" }
        Assertions.assertNotNull(ford)
    }

    @Test
    fun `airlock is closed at t=0 and opening at t=2`() {
        val env0 = DomainModel.STORY[FictionalTime(chapter = 7, word = 0)]!!
        val (_, airlock0) = env0.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Closed, (airlock0 as Airlock).state)
        Assertions.assertTrue(airlock0.atmosphere.sounds.containsKey(SoundSource.Air))
        Assertions.assertEquals(SoundLevel.Hiss, airlock0.atmosphere.sounds[SoundSource.Air])

        val env2 = DomainModel.STORY[FictionalTime(chapter = 7, word = 2)]!!
        val (_, airlock2) = env2.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Opening, (airlock2 as Airlock).state)
        Assertions.assertTrue(airlock2.atmosphere.sounds.containsKey(SoundSource.Air))
        Assertions.assertEquals(SoundLevel.DeafeningRoar, airlock2.atmosphere.sounds[SoundSource.Air])
        Assertions.assertTrue(airlock2.atmosphere.sounds.containsKey(SoundSource.Mechanical))
        Assertions.assertEquals(SoundLevel.Whir, airlock2.atmosphere.sounds[SoundSource.Mechanical])
    }

    @Test
    fun `airlock is open and empty at t=17`() {
        val env = DomainModel.STORY[FictionalTime(chapter = 7, word = 17)]!!
        val (airlockLocation, airlock) = env.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Opened, (airlock as Airlock).state)
        Assertions.assertTrue(airlock.atmosphere.sounds.isEmpty())

        val insideAirlock = env.getChildEntities(airlockLocation, airlock).toList()
        Assertions.assertEquals(0, insideAirlock.size)
    }

    @Test
    fun `characters fly into space at t=17`() {
        val env = DomainModel.STORY[FictionalTime(chapter = 7, word = 17)]!!
        val people = env.entities.entries.filter { (_, e) -> e is Person }

        Assertions.assertEquals(2, people.size)
        val (arthurLocation, _) = people.find { (_, e) -> e is Person && e.name == "Arthur" }!!
        val (fordLocation, _) = people.find { (_, e) -> e is Person && e.name == "Ford" }!!

        Assertions.assertEquals(VelocityMagnitude.FlyingLikeCorkFromToyGun, arthurLocation.velocityMagnitude)
        Assertions.assertEquals(VelocityMagnitude.FlyingLikeCorkFromToyGun, fordLocation.velocityMagnitude)
    }

    @Test
    fun `characters are surrounded by many bright celestial objects at t=17`() {
        val env = DomainModel.STORY[FictionalTime(chapter = 7, word = 17)]!!
        val stars = env.entities.entries.filter { (_, e) -> e is CelestialObject }

        Assertions.assertTrue(stars.size > 100) // many objects
        Assertions.assertEquals(Float.MAX_VALUE,
                (stars.minByOrNull { (_, e) -> (e as CelestialObject).brightness }!!.value as CelestialObject).brightness)
    }
}