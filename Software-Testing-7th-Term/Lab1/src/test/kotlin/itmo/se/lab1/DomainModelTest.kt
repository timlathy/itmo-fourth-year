package itmo.se.lab1

import org.junit.jupiter.api.*
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvSource
import java.lang.IllegalArgumentException
import java.lang.IllegalStateException

class DomainModelTest {
    @Nested
    inner class DomainModelAirlockTest {
        @Test
        fun `it opens with a sound then loses atmosphere`() {
            val airlock = Airlock(1.0f)

            Assertions.assertEquals(AirlockState.Closed, airlock.state)
            Assertions.assertEquals(Airlock.CLOSED_PRESSURE, airlock.atmosphere.pressureBar)
            Assertions.assertEquals(SoundLevel.Hiss, airlock.atmosphere.sounds[SoundSource.Air])

            val openingAirlock = airlock.open()

            Assertions.assertEquals(AirlockState.Opening, openingAirlock.state)
            Assertions.assertEquals(SoundLevel.Whir, openingAirlock.atmosphere.sounds[SoundSource.Mechanical])
            Assertions.assertEquals(SoundLevel.DeafeningRoar, openingAirlock.atmosphere.sounds[SoundSource.Air])

            val update = openingAirlock.update(Location(Vector3.ZERO), Environment(emptyMap())).toList()
            Assertions.assertEquals(1, update.size)
            val (_, openedAirlock) = update[0]

            Assertions.assertEquals(AirlockState.Opened, (openedAirlock as Airlock).state)
            Assertions.assertTrue(openedAirlock.atmosphere.sounds.isEmpty())
            Assertions.assertEquals(0f, openedAirlock.atmosphere.pressureBar)
        }

        @Test
        fun `it restores atmosphere after closing`() {
            val airlock = Airlock(1.0f)

            val (_, openedAirlock) = airlock.open().update(Location(Vector3.ZERO), Environment(emptyMap())).toList().first()
            Assertions.assertEquals(AirlockState.Opened, (openedAirlock as Airlock).state)

            val closingAirlock = openedAirlock.close()
            Assertions.assertEquals(AirlockState.Closing, closingAirlock.state)
            Assertions.assertTrue(openedAirlock.atmosphere.sounds.isEmpty()) // no sounds without atmosphere
            Assertions.assertEquals(0f, openedAirlock.atmosphere.pressureBar)

            val update = closingAirlock.update(Location(Vector3.ZERO), Environment(emptyMap())).toList()
            Assertions.assertEquals(1, update.size)
            val (_, closedAirlock) = update[0]

            Assertions.assertEquals(AirlockState.Closed, (closedAirlock as Airlock).state)
            Assertions.assertEquals(SoundLevel.Hiss, closedAirlock.atmosphere.sounds[SoundSource.Air])
            Assertions.assertFalse(closedAirlock.atmosphere.sounds.containsKey(SoundSource.Mechanical))
            Assertions.assertEquals(Airlock.CLOSED_PRESSURE, closedAirlock.atmosphere.pressureBar)
        }

        @Test
        fun `it cannot be closed while opening or opened while closing`() {
            val openingAirlock = Airlock(1.0f, state = AirlockState.Opening)
            val e = Assertions.assertThrows(IllegalStateException::class.java) { openingAirlock.close() }
            Assertions.assertEquals("Cannot close the airlock when it is opening", e.message)

            val closingAirlock = Airlock(1.0f, state = AirlockState.Closing)
            val e2 = Assertions.assertThrows(IllegalStateException::class.java) { closingAirlock.open() }
            Assertions.assertEquals("Cannot open the airlock when it is closing", e2.message)
        }

        @Test
        fun `opening or closing it twice is a noop`() {
            val openedAirlock = Airlock(1.0f, state = AirlockState.Opened)
            Assertions.assertEquals(openedAirlock, openedAirlock.open())

            val closedAirlock = Airlock(1.0f, state = AirlockState.Closed)
            Assertions.assertEquals(closedAirlock, closedAirlock.close())
        }
    }

    @Nested
    inner class DomainModelEnvironmentTest {
        @Test
        fun `it looks up entities inside another entity`() {
            val container = object : Entity() {
                override val boundingRadius: Float = 10.0f
            }
            val objectInside = object : Entity() {
                override val boundingRadius: Float = 1.0f
            }
            val objectOnEdge = object : Entity() {
                override val boundingRadius: Float = 1.0f
            }
            val objectOutside = object : Entity() {
                override val boundingRadius: Float = 1.0f
            }

            val locContainer = Location(Vector3.ZERO) to container
            val locObjectInside = Location(Vector3(4f, 4f, 4f)) to objectInside
            val locObjectOnEdge = Location(Vector3(10f, 0f, 0f)) to objectOnEdge
            val locObjectOutside = Location(Vector3(10f, 10f, 0f)) to objectOutside

            val env = Environment(mapOf(locContainer, locObjectInside, locObjectOnEdge, locObjectOutside))
            val objectsInside = env.getChildEntities(locContainer.first, locContainer.second).toList()
            Assertions.assertEquals(1, objectsInside.size)
            Assertions.assertEquals(locObjectInside, objectsInside.first())
        }
    }

    @Nested
    inner class DomainModelSpacetimeTest {
        @Test
        fun `it invokes update on entities and applies their velocity to location`() {
            val entity = object : Entity() {
                override val boundingRadius: Float = 1.0f

                override fun update(loc: Location, env: Environment): Map<Location, Entity> =
                    mapOf(loc.copy(velocity = Vector3(1f, 2f, 3f)) to this)
            }
            val locEntity = Location(Vector3.ZERO, velocity = Vector3.ZERO) to entity

            val sp = Spacetime().apply {
                advance(FictionalTime(chapter = 0, word = 0), listOf(locEntity))
                advance(FictionalTime(chapter = 0, word = 1))
                advance(FictionalTime(chapter = 0, word = 2))
            }

            Assertions.assertEquals(Vector3.ZERO, sp[FictionalTime(chapter = 0, word = 0)]!!.entities.entries.first().key.position)
            Assertions.assertEquals(Vector3(1f, 2f, 3f), sp[FictionalTime(chapter = 0, word = 1)]!!.entities.entries.first().key.position)
            Assertions.assertEquals(Vector3(2f, 4f, 6f), sp[FictionalTime(chapter = 0, word = 2)]!!.entities.entries.first().key.position)
        }

        @Test
        fun `it rejects invalid time advances`() {
            val sp = Spacetime()
            sp.advance(FictionalTime(chapter = 1, word = 0))

            val e = Assertions.assertThrows(IllegalArgumentException::class.java) {
                sp.advance(FictionalTime(chapter = 1, word = 0))
            }
            Assertions.assertEquals("Cannot advance to the same time twice, specify a different time that has not been advanced to yet", e.message)
        }
    }

    @Nested
    inner class DomainModelFictionalTimeTest {
        @Test
        fun `it rejects negative chapter and word indexes`() {
            val e = Assertions.assertThrows(IllegalArgumentException::class.java) {
                FictionalTime(word = -1, chapter = 0)
            }
            Assertions.assertEquals("Word index must be a non-negative number", e.message)

            val e1 = Assertions.assertThrows(IllegalArgumentException::class.java) {
                FictionalTime(word = 0, chapter = -1)
            }
            Assertions.assertEquals("Chapter index must be a non-negative number", e1.message)
        }
    }

    @Nested
    inner class DomainModelLocationTest {
        @ParameterizedTest
        @CsvSource("0,0,0", "0.0001,0.0001,0", "0.009,0.0009,0")
        fun `it marks objects as still when velocity is negligible`(x: Float, y: Float, z: Float) {
            Assertions.assertEquals(VelocityMagnitude.Still, Location(Vector3.ZERO, velocity = Vector3(x, y, z)).velocityMagnitude)
        }

        @ParameterizedTest
        @CsvSource("0.01,0,0", "0,0,10", "19,40,23")
        fun `it marks objects as moving when velocity is reasonable`(x: Float, y: Float, z: Float) {
            Assertions.assertEquals(VelocityMagnitude.Moving, Location(Vector3.ZERO, velocity = Vector3(x, y, z)).velocityMagnitude)
        }

        @ParameterizedTest
        @CsvSource("100,0,0", "60,60,60", "200,10,500")
        fun `it marks objects flying like a cork from a toy gun when appropriate`(x: Float, y: Float, z: Float) {
            Assertions.assertEquals(VelocityMagnitude.FlyingLikeCorkFromToyGun, Location(Vector3.ZERO, velocity = Vector3(x, y, z)).velocityMagnitude)
        }
    }
}