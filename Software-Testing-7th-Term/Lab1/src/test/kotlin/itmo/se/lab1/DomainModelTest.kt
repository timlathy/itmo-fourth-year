package itmo.se.lab1

import org.junit.jupiter.api.Test
import org.junit.jupiter.api.Assertions
import java.lang.IllegalStateException

class DomainModelTest {
    /* Unit */

    @Test
    fun `airlock opens with a sound then loses atmosphere`() {
        val airlock = Airlock(1.0f)

        Assertions.assertEquals(AirlockState.Closed, airlock.state)
        Assertions.assertEquals(Airlock.CLOSED_PRESSURE, airlock.atmosphere.pressureBar)
        Assertions.assertEquals(SoundLevel.Hiss, airlock.atmosphere.sounds[SoundSource.Air])

        val openingAirlock = airlock.open()

        Assertions.assertEquals(AirlockState.Opening, openingAirlock.state)
        Assertions.assertEquals(SoundLevel.Whir, openingAirlock.atmosphere.sounds[SoundSource.Mechanical])
        Assertions.assertEquals(SoundLevel.DeafeningRoar, openingAirlock.atmosphere.sounds[SoundSource.Air])

        val update = openingAirlock.update(Location(Vector3(0f,0f,0f)), Environment(emptyMap())).toList()
        Assertions.assertEquals(1, update.size)
        val (_, openedAirlock) = update[0]

        Assertions.assertEquals(AirlockState.Opened, (openedAirlock as Airlock).state)
        Assertions.assertTrue(openedAirlock.atmosphere.sounds.isEmpty())
        Assertions.assertEquals(0f, openedAirlock.atmosphere.pressureBar)
    }

    @Test
    fun `airlock closes restoring atmosphere`() {
        val airlock = Airlock(1.0f)

        val (_, openedAirlock) = airlock.open().update(Location(Vector3(0f,0f,0f)), Environment(emptyMap())).toList().first()
        Assertions.assertEquals(AirlockState.Opened, (openedAirlock as Airlock).state)

        val closingAirlock = openedAirlock.close()
        Assertions.assertEquals(AirlockState.Closing, closingAirlock.state)
        Assertions.assertTrue(openedAirlock.atmosphere.sounds.isEmpty()) // no sounds without atmosphere
        Assertions.assertEquals(0f, openedAirlock.atmosphere.pressureBar)

        val update = closingAirlock.update(Location(Vector3(0f,0f,0f)), Environment(emptyMap())).toList()
        Assertions.assertEquals(1, update.size)
        val (_, closedAirlock) = update[0]

        Assertions.assertEquals(AirlockState.Closed, (closedAirlock as Airlock).state)
        Assertions.assertEquals(SoundLevel.Hiss, closedAirlock.atmosphere.sounds[SoundSource.Air])
        Assertions.assertFalse(closedAirlock.atmosphere.sounds.containsKey(SoundSource.Mechanical))
        Assertions.assertEquals(Airlock.CLOSED_PRESSURE, closedAirlock.atmosphere.pressureBar)
    }

    @Test
    fun `airlock cannot be closed while opening or opened while closing`() {
        val openingAirlock = Airlock(1.0f, state = AirlockState.Opening)
        Assertions.assertThrows(IllegalStateException::class.java) { openingAirlock.close() }

        val closingAirlock = Airlock(1.0f, state = AirlockState.Closing)
        Assertions.assertThrows(IllegalStateException::class.java) { closingAirlock.open() }
    }

    /* Integration */

    private val integrationStory = Spacetime().apply {
        // Зажужжал мотор. Тоненький свист перерос в рев воздуха, вырывающегося в черную пустоту, усеянную невероятно яркими светящимися точками.
        // Форд и Артур вылетели в открытый космос, как конфетти из хлопушки. Глава 8

        val p1 = Person("Ford", 1.0f)
        val p2 = Person("Arthur", 1.0f)
        val airlock = Airlock(10.0f)

        advance(FictionalTime(chapter = 7, word = 0), listOf(
                Location(Vector3(0.0f, 0.0f, 0.0f)) to airlock,
                Location(Vector3(1.0f, 1.0f, 0.0f)) to p1,
                Location(Vector3(-1.0f, -1.0f, 0.0f)) to p2))

        advance(FictionalTime(chapter = 7, word = 2), listOf(
                Location(Vector3(0.0f, 0.0f, 0.0f)) to airlock.open()))

        val stars = List(1000) { i ->
            val sign = if (i % 2 == 0) 1 else -1
            val position = Vector3(1000.0f * (i + 10) * sign, 1000.0f * (i + 10) * sign, 1000.0f * i * sign)
            val entity = CelestialObject(boundingRadius = 1000.0f * i, atmosphere = null, brightness = Float.MAX_VALUE)
            Location(position) to entity
        }

        advance(FictionalTime(chapter = 7, word = 17), stars)

        advance(FictionalTime(chapter = 8, word = 0))
    }

    @Test
    fun `characters are located in the airlock at t=0`() {
        val env = integrationStory[FictionalTime(chapter = 7, word = 0)]!!
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
        val env0 = integrationStory[FictionalTime(chapter = 7, word = 0)]!!
        val (_, airlock0) = env0.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Closed, (airlock0 as Airlock).state)
        Assertions.assertTrue(airlock0.atmosphere.sounds.containsKey(SoundSource.Air))
        Assertions.assertEquals(SoundLevel.Hiss, airlock0.atmosphere.sounds[SoundSource.Air])

        val env2 = integrationStory[FictionalTime(chapter = 7, word = 2)]!!
        val (_, airlock2) = env2.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Opening, (airlock2 as Airlock).state)
        Assertions.assertTrue(airlock2.atmosphere.sounds.containsKey(SoundSource.Air))
        Assertions.assertEquals(SoundLevel.DeafeningRoar, airlock2.atmosphere.sounds[SoundSource.Air])
        Assertions.assertTrue(airlock2.atmosphere.sounds.containsKey(SoundSource.Mechanical))
        Assertions.assertEquals(SoundLevel.Whir, airlock2.atmosphere.sounds[SoundSource.Mechanical])
    }

    @Test
    fun `airlock is open and empty at t=17`() {
        val env = integrationStory[FictionalTime(chapter = 7, word = 17)]!!
        val (airlockLocation, airlock) = env.entities.entries.find { (_, e) -> e is Airlock }!!

        Assertions.assertEquals(AirlockState.Opened, (airlock as Airlock).state)
        Assertions.assertTrue(airlock.atmosphere.sounds.isEmpty())

        val insideAirlock = env.getChildEntities(airlockLocation, airlock).toList()
        Assertions.assertEquals(0, insideAirlock.size)
    }

    @Test
    fun `characters fly into space at t=17`() {
        val env = integrationStory[FictionalTime(chapter = 7, word = 17)]!!
        val people = env.entities.entries.filter { (_, e) -> e is Person }

        Assertions.assertEquals(2, people.size)
        val (arthurLocation, _) = people.find { (_, e) -> e is Person && e.name == "Arthur" }!!
        val (fordLocation, _) = people.find { (_, e) -> e is Person && e.name == "Ford" }!!

        Assertions.assertEquals(VelocityMagnitude.FlyingLikeCorkFromToyGun, arthurLocation.velocityMagnitude)
        Assertions.assertEquals(VelocityMagnitude.FlyingLikeCorkFromToyGun, fordLocation.velocityMagnitude)
    }

    @Test
    fun `characters are surrounded by many bright celestial objects at t=17`() {
        val env = integrationStory[FictionalTime(chapter = 7, word = 17)]!!
        val stars = env.entities.entries.filter { (_, e) -> e is CelestialObject }

        Assertions.assertTrue(stars.size > 100) // many objects
        Assertions.assertEquals(Float.MAX_VALUE,
                (stars.minByOrNull { (_, e) -> (e as CelestialObject).brightness }!!.value as CelestialObject).brightness)
    }
}