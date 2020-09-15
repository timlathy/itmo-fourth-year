package itmo.se.lab1

import kotlin.math.abs

class TrigApprox {
    companion object {
        fun sin(x: Double, eps: Double = 1E-6): Double {
            if (x.isInfinite() || x.isNaN() || eps.isInfinite() || eps.isNaN())
                return Double.NaN

            var seriesSum = 0.0
            var term = x
            var n = 1

            while (abs(term) > eps) {
                seriesSum += term
                term *= -x * x / (2 * n * (2 * n + 1))
                n++
            }

            return seriesSum
        }
    }
}