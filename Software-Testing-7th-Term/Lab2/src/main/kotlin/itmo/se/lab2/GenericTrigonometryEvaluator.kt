package itmo.se.lab2

import kotlin.math.abs

interface IGenericTrigonometryEvaluator {
    fun sin(x: Double, eps: Double): Double
    fun tan(x: Double, eps: Double): Double
    fun cot(x: Double, eps: Double): Double
    fun sec(x: Double, eps: Double): Double
}

class GenericTrigonometryEvaluator(private val trigEval: ITrigonometryEvaluator) : IGenericTrigonometryEvaluator {
    override fun sin(x: Double, eps: Double): Double {
        val sin = trigEval.sin(x, eps)
        return if (!sin.isNaN() && abs(sin) < eps) 0.0 else sin
    }

    override fun tan(x: Double, eps: Double): Double {
        val cos = cos(x, eps)
        return if (cos == 0.0) Double.NaN else (sin(x, eps) / cos)
    }

    override fun cot(x: Double, eps: Double): Double {
        val tan = tan(x, eps)
        return if (tan == 0.0) Double.NaN else (1.0 / tan)
    }

    override fun sec(x: Double, eps: Double): Double {
        val cos = cos(x, eps)
        return if (cos == 0.0) Double.NaN else (1.0 / cos)
    }

    private fun cos(x: Double, eps: Double) =
        sin(x + Math.PI / 2, eps)
}
