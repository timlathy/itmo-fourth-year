package itmo.se.lab2

import kotlin.math.pow

interface IGenericTrigonometryEvaluator {
    fun sin(x: Double, eps: Double): Double
    fun tan(x: Double, eps: Double): Double
    fun cot(x: Double, eps: Double): Double
    fun sec(x: Double, eps: Double): Double
}

class EquationSystem(
    private val trigEval: IGenericTrigonometryEvaluator,
    private val logEval: IGenericLogarithmEvaluator
) {
    fun compute(x: Double, eps: Double): Double =
        if (x <= 0) {
            val sinx = trigEval.sin(x, eps)
            val tanx = trigEval.tan(x, eps)
            val cotx = trigEval.cot(x, eps)
            val secx = trigEval.sec(x, eps)

            val numerator = (sinx * sinx).pow(2) / cotx
            val denominator = secx / (sinx / tanx) + sinx - secx
            (numerator / denominator).pow(2)
        } else {
            val log5x = logEval.log(x, base = 5.0, eps)
            val log2x = logEval.log(x, base = 2.0, eps)
            val log3x = logEval.log(x, base = 3.0, eps)
            val log10x = logEval.log(x, base = 10.0, eps)

            log5x / log2x - log2x - log3x - log10x.pow(3) + log10x * log10x
        }
}