package itmo.se.lab1

import org.junit.jupiter.api.Test
import org.junit.jupiter.api.Assertions

class TrigApproxTest {
    @Test
    fun `sine matches expected values at inflection points`() {
        Assertions.assertEquals(0.0, TrigApprox.sin(-Math.PI), 1E-6, "sin(-pi) = 0")
        Assertions.assertEquals(0.0, TrigApprox.sin(0.0), 1E-6, "sin(0) = 0")
        Assertions.assertEquals(0.0, TrigApprox.sin(Math.PI), 1E-6, "sin(pi) = 0")
    }

    @Test
    fun `sine matches expected values at extremum points`() {
        Assertions.assertEquals(1.0, TrigApprox.sin(Math.PI / 2), 1E-6, "sin(pi/2) = 1")
        Assertions.assertEquals(-1.0, TrigApprox.sin(-Math.PI / 2), 1E-6, "sin(-pi/2) = -1")
    }

    @Test
    fun `sine is periodic`() {
        for (i in 0..4) {
            Assertions.assertEquals(0.5, TrigApprox.sin(2 * i * Math.PI + Math.PI / 6), 1E-6, "sin(${i * 2}pi + pi/6) = 0.5")
            Assertions.assertEquals(-0.5, TrigApprox.sin(-2 * i * Math.PI - Math.PI / 6), 1E-6, "sin(-${i * 2}pi - pi/6) = -0.5")
        }
    }

    @Test
    fun `sine of infinity is nan`() {
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Double.POSITIVE_INFINITY),"sin(+inf) = NaN")
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Double.NEGATIVE_INFINITY),"sin(-inf) = NaN")
    }

    @Test
    fun `sine of nan is nan`() {
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Double.NaN, eps = 1E-6),"sin(NaN) = NaN")
    }

    @Test
    fun `sine approximation meets requested precision`() {
        Assertions.assertEquals(0.5, TrigApprox.sin(Math.PI / 6, eps = 1.0), 1.0, "sin(pi/6) = 0.5, eps = 1.0")
        Assertions.assertEquals(0.5, TrigApprox.sin(Math.PI / 6, eps = 1E-8), 1E-8, "sin(pi/6) = 0.5, eps = 1e-8")
        Assertions.assertEquals(0.5, TrigApprox.sin(Math.PI / 6, eps = 1E-16), 1E-16, "sin(pi/6) = 0.5, eps = 1e-16")
    }

    @Test
    fun `sine with invalid precision is nan`() {
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Math.PI / 6, eps = Double.POSITIVE_INFINITY), "sin(x) = NaN, eps = +inf")
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Math.PI / 6, eps = Double.NEGATIVE_INFINITY), "sin(x) = NaN, eps = -inf")
        Assertions.assertEquals(Double.NaN, TrigApprox.sin(Math.PI / 6, eps = Double.NaN), "sin(x) = NaN, eps = NaN")
    }
}