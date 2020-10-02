package itmo.se.lab2

import kotlin.math.abs
import kotlin.math.pow

interface ILogarithmEvaluator {
    fun ln(x: Double, eps: Double): Double
}

class LogarithmEvaluator : ILogarithmEvaluator {
    override fun ln(x: Double, eps: Double): Double = when {
        x <= 0 || x.isNaN() || eps.isNaN() || eps.isInfinite() -> {
            Double.NaN
        }
        x == Double.POSITIVE_INFINITY -> {
            Double.POSITIVE_INFINITY
        }
        x < 1 -> {
            // ln (1+z) = (-1)^(i-1)*z^i/i
            val z = x - 1
            var term = z
            var sum = z
            var i = 2
            while (abs(term) >= eps) {
                term *= -z
                sum += term / i
                i++
            }
            sum
        }
        else -> {
            // ln (z/z-1) = 1/(i*z^i)
            val z = x / (x - 1.0)
            var term = z
            var sum = 0.0
            var i = 1
            while (abs(term) > eps) {
                term = 1.0 / (i * z.pow(i))
                sum += term
                i++
            }
            sum
        }
    }
}