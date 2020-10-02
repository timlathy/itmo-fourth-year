package itmo.se.lab2.unit

import itmo.se.lab2.EquationSystem
import itmo.se.lab2.IGenericLogarithmEvaluator
import itmo.se.lab2.IGenericTrigonometryEvaluator
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource
import org.mockito.ArgumentMatchers.eq
import org.mockito.Mockito.`when`
import org.mockito.Mockito.mock

class EquationSystemTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/equation-system-stub.csv"], numLinesToSkip = 1)
    fun `it handles division by zero`(
        x: Double,
        y: Double,
        sin: Double,
        tan: Double,
        cot: Double,
        sec: Double,
        log2: Double,
        log3: Double,
        log5: Double,
        log10: Double,
        eps: Double
    ) {
        val mockTrig = mock(IGenericTrigonometryEvaluator::class.java)
        `when`(mockTrig.sin(eq(x), eq(eps))).thenReturn(sin)
        `when`(mockTrig.tan(eq(x), eq(eps))).thenReturn(tan)
        `when`(mockTrig.cot(eq(x), eq(eps))).thenReturn(cot)
        `when`(mockTrig.sec(eq(x), eq(eps))).thenReturn(sec)

        val mockLog = mock(IGenericLogarithmEvaluator::class.java)
        `when`(mockLog.log(eq(x), eq(2.0), eq(eps))).thenReturn(log2)
        `when`(mockLog.log(eq(x), eq(3.0), eq(eps))).thenReturn(log3)
        `when`(mockLog.log(eq(x), eq(5.0), eq(eps))).thenReturn(log5)
        `when`(mockLog.log(eq(x), eq(10.0), eq(eps))).thenReturn(log10)

        val system = EquationSystem(mockTrig, mockLog)
        Assertions.assertEquals(y, system.compute(x, eps), eps)
    }
}