package itmo.se.lab1

import java.lang.IllegalStateException
import kotlin.collections.HashMap
import kotlin.math.pow
import kotlin.math.sqrt

class DomainModel {
    companion object {
        val STORY = Spacetime().apply {
            // Зажужжал мотор. Тоненький свист перерос в рев воздуха, вырывающегося в черную пустоту, усеянную невероятно яркими светящимися точками.
            // Форд и Артур вылетели в открытый космос, как конфетти из хлопушки. Глава 8

            val p1 = Person("Ford", 1.0f)
            val p2 = Person("Arthur", 1.0f)
            val airlock = Airlock(10.0f, atmosphere = Atmosphere(1.034f,
                    mapOf(SoundSource.Air to SoundLevel.Hiss)), state = AirlockState.Closed)

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
    }
}

class Spacetime {
    private val contents: MutableMap<FictionalTime, Environment> = HashMap()
    private var currentTime: FictionalTime = FictionalTime(0, 0)

    operator fun get(t: FictionalTime): Environment? = contents[t]

    fun advance(newTime: FictionalTime, newEntities: List<Pair<Location, Entity>> = emptyList()) {
        val oldEnv = contents[currentTime]
        val timeDiff = currentTime.diff(newTime)
        val updatedEntities = oldEnv?.entities?.flatMap { (l, e) -> e.update(l, oldEnv, timeDiff).toList() }?.map { (l, e) ->
            Pair(Location(l.position + l.velocity, l.velocity), e)
        }?.distinctBy { (_, e) -> e } ?: emptyList()
        contents[newTime] = Environment((updatedEntities + newEntities).toMap())
        currentTime = newTime
    }
}

data class Environment(val entities: Map<Location, Entity>) {
    operator fun get(location: Location) = entities[location]

    fun getChildEntities(location: Location, entity: Entity): Iterable<Pair<Location, Entity>> {
        val children = mutableListOf<Pair<Location, Entity>>()
        for ((l, e) in entities)
            if (l != location && l.position.squaredDistance(location.position) < entity.boundingRadius.pow(2))
                children.add(Pair(l, e))
        return children
    }
}

data class FictionalTime(val chapter: Int, val word: Int) {
    fun diff(t2: FictionalTime): Int =
            if (t2.chapter > chapter)
                t2.chapter - chapter
            else
                t2.word - word
}

data class Vector3(val x: Float, val y: Float, val z: Float) {
    operator fun plus(o: Vector3) = Vector3(x + o.x, y + o.y, z + o.z)

    fun squaredDistance(o: Vector3) = (x - o.x).pow(2) + (y - o.y).pow(2) + (z - o.z).pow(2)
}

enum class VelocityMagnitude { Still, Moving, FlyingLikeCorkFromToyGun }

data class Location(val position: Vector3, val velocity: Vector3 = Vector3(0f, 0f, 0f)) {
    val velocityMagnitude: VelocityMagnitude
        get() {
            val magnitude = sqrt(velocity.x.pow(2) + velocity.y.pow(2) + velocity.z.pow(2))
            if (magnitude < 0.01f)
                return VelocityMagnitude.Still
            if (magnitude < 100f)
                return VelocityMagnitude.Moving
            return VelocityMagnitude.FlyingLikeCorkFromToyGun
        }
}

enum class SoundSource { Mechanical, Air }
enum class SoundLevel { Hiss, Whir, DeafeningRoar }

data class Atmosphere(var pressureBar: Float,
                      var sounds: Map<SoundSource, SoundLevel> = emptyMap())

abstract class Entity {
    abstract val boundingRadius: Float
    open val atmosphere: Atmosphere? = null

    open fun update(loc: Location, env: Environment, timeDiff: Int): Map<Location, Entity> = mapOf(loc to this)
}

data class CelestialObject(override val boundingRadius: Float,
                           override val atmosphere: Atmosphere?,
                           val brightness: Float) : Entity()

data class Person(val name: String, override val boundingRadius: Float) : Entity()

enum class AirlockState { Closed, Opening, Opened, Closing }

data class Airlock(override val boundingRadius: Float,
                   override val atmosphere: Atmosphere,
                   val state: AirlockState) : Entity() {
    companion object {
        const val CLOSED_PRESSURE = 1.034f
    }

    fun open(): Airlock = when (state) {
        AirlockState.Closed -> copy(state = AirlockState.Opening, atmosphere = atmosphere.copy(sounds =
        atmosphere.sounds + (SoundSource.Mechanical to SoundLevel.Whir) + (SoundSource.Air to SoundLevel.DeafeningRoar)))
        AirlockState.Closing -> throw IllegalStateException("Cannot open the airlock when it is closing")
        else -> this
    }

    fun close(): Airlock = when (state) {
        AirlockState.Opened -> copy(state = AirlockState.Closing)
        AirlockState.Opening -> throw IllegalStateException("Cannot close the airlock when it is opening")
        else -> this
    }

    override fun update(loc: Location, env: Environment, timeDiff: Int): Map<Location, Entity> = when (state) {
        AirlockState.Closing -> mapOf(loc to copy(state = AirlockState.Closed, atmosphere = Atmosphere(pressureBar = CLOSED_PRESSURE)))
        AirlockState.Opening -> {
            val childEntities = env.getChildEntities(loc, this)
            mapOf(loc to copy(state = AirlockState.Opened, atmosphere = Atmosphere(pressureBar = 0f))) +
                    childEntities.map { (l, e) -> Pair(Location(l.position, l.velocity + Vector3(100f, 100f, 0f)), e) }.toMap()
        }
        else -> mapOf(loc to this)
    }
}
