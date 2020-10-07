package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.Nested
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource
import org.mockito.Mockito.*

class EquationSystemTest {
    fun mockTrigEvaluator(
        x: Double,
        sin: Double,
        tan: Double,
        cot: Double,
        sec: Double,
        eps: Double
    ): IGenericTrigonometryEvaluator =
        mock(IGenericTrigonometryEvaluator::class.java).apply {
            `when`(sin(eq(x), eq(eps))).thenReturn(sin)
            `when`(tan(eq(x), eq(eps))).thenReturn(tan)
            `when`(cot(eq(x), eq(eps))).thenReturn(cot)
            `when`(sec(eq(x), eq(eps))).thenReturn(sec)
        }

    fun mockLogEvaluator(
        x: Double,
        log2: Double,
        log3: Double,
        log5: Double,
        log10: Double,
        eps: Double
    ): IGenericLogarithmEvaluator =
        mock(IGenericLogarithmEvaluator::class.java).apply {
            `when`(log(eq(x), eq(2.0), eq(eps))).thenReturn(log2)
            `when`(log(eq(x), eq(3.0), eq(eps))).thenReturn(log3)
            `when`(log(eq(x), eq(5.0), eq(eps))).thenReturn(log5)
            `when`(log(eq(x), eq(10.0), eq(eps))).thenReturn(log10)
        }

    @Nested
    inner class EquationSystemUnitTest {
        @ParameterizedTest
        @CsvFileSource(resources = ["/equation-system.csv"], numLinesToSkip = 1)
        fun `it computes the equation for valid inputs`(
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
            val mockTrig = mockTrigEvaluator(x, sin, tan, cot, sec, eps)
            val mockLog = mockLogEvaluator(x, log2, log3, log5, log10, eps)
            val system = EquationSystem(mockTrig, mockLog)
            Assertions.assertEquals(y, system.compute(x, eps), eps)
        }
    }

    @Nested
    inner class EquationSystemIntegrationTest {
        @ParameterizedTest
        @CsvFileSource(resources = ["/equation-system.csv"], numLinesToSkip = 1)
        fun `it integrates logarithm evaluator`(
            x: Double,
            y: Double,
            sin: Double,
            tan: Double,
            cot: Double,
            sec: Double,
            _log2: Double,
            _log3: Double,
            _log5: Double,
            _log10: Double,
            eps: Double
        ) {
            val mockTrig = mockTrigEvaluator(x, sin, tan, cot, sec, eps)
            val system = EquationSystem(mockTrig, GenericLogarithmEvaluator(LogarithmEvaluator()))
            Assertions.assertEquals(y, system.compute(x, eps), eps)
        }

        @ParameterizedTest
        @CsvFileSource(resources = ["/equation-system.csv"], numLinesToSkip = 1)
        fun `it integrates trigonometry evaluator`(
            x: Double,
            y: Double,
            _sin: Double,
            _tan: Double,
            _cot: Double,
            _sec: Double,
            log2: Double,
            log3: Double,
            log5: Double,
            log10: Double,
            eps: Double
        ) {
            val mockLog = mockLogEvaluator(x, log2, log3, log5, log10, eps)
            val system = EquationSystem(GenericTrigonometryEvaluator(TrigonometryEvaluator()), mockLog)
            Assertions.assertEquals(y, system.compute(x, eps), eps)
        }

        @ParameterizedTest
        @CsvFileSource(resources = ["/equation-system.csv"], numLinesToSkip = 1)
        fun `it integrates all evaluators`(
            x: Double,
            y: Double,
            _sin: Double,
            _tan: Double,
            _cot: Double,
            _sec: Double,
            _log2: Double,
            _log3: Double,
            _log5: Double,
            _log10: Double,
            eps: Double
        ) {
            val system = EquationSystem(
                GenericTrigonometryEvaluator(TrigonometryEvaluator()),
                GenericLogarithmEvaluator(LogarithmEvaluator())
            )
            Assertions.assertEquals(y, system.compute(x, eps), eps)
        }
    }
}