package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.Nested
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource
import org.mockito.ArgumentMatchers.*
import org.mockito.Mockito

class GenericLogarithmEvaluatorTest {
    @Nested
    inner class GenericLogarithmEvaluatorUnitTest {
        @ParameterizedTest
        @CsvFileSource(resources = ["/generic-log.csv"], numLinesToSkip = 1)
        fun `it computes logarithm of any base`(
            y: Double,
            x: Double,
            base: Double,
            logX: Double,
            logBase: Double,
            eps: Double
        ) {
            val mockLnEval = Mockito.mock(ILogarithmEvaluator::class.java)
            Mockito.`when`(mockLnEval.ln(eq(x), anyDouble())).thenReturn(logX)
            Mockito.`when`(mockLnEval.ln(eq(base), anyDouble())).thenReturn(logBase)

            val logEval = GenericLogarithmEvaluator(mockLnEval)
            Assertions.assertEquals(y, logEval.log(x, base, eps), eps)
        }

        @ParameterizedTest
        @CsvFileSource(resources = ["/generic-log-invalid.csv"], numLinesToSkip = 1)
        fun `it returns NaN when called with invalid parameters`(
            y: Double,
            x: Double,
            base: Double,
            logX: Double,
            logBase: Double,
            eps: Double
        ) {
            val mockLnEval = Mockito.mock(ILogarithmEvaluator::class.java)
            Mockito.`when`(mockLnEval.ln(eq(x), doubleThat { !(it.isNaN() || it.isInfinite()) })).thenReturn(logX)
            Mockito.`when`(mockLnEval.ln(eq(base), doubleThat { !(it.isNaN() || it.isInfinite()) })).thenReturn(logBase)
            Mockito.`when`(mockLnEval.ln(eq(x), doubleThat { it.isNaN() || it.isInfinite() })).thenReturn(Double.NaN)
            Mockito.`when`(mockLnEval.ln(eq(base), doubleThat { it.isNaN() || it.isInfinite() })).thenReturn(Double.NaN)

            val logEval = GenericLogarithmEvaluator(mockLnEval)
            Assertions.assertEquals(y, logEval.log(x, base, eps), 10E-6)
        }
    }

    @Nested
    inner class GenericLogarithmEvaluatorIntegrationTest {
        @ParameterizedTest
        @CsvFileSource(resources = ["/generic-log.csv"], numLinesToSkip = 1)
        fun `it computes logarithm of any base`(
            y: Double,
            x: Double,
            base: Double,
            _logX: Double,
            _logBase: Double,
            eps: Double
        ) {
            val lnEval = LogarithmEvaluator()
            val logEval = GenericLogarithmEvaluator(lnEval)
            Assertions.assertEquals(y, logEval.log(x, base, eps), eps)
        }

        @ParameterizedTest
        @CsvFileSource(resources = ["/generic-log-invalid.csv"], numLinesToSkip = 1)
        fun `it returns NaN when called with invalid parameters`(
            y: Double,
            x: Double,
            base: Double,
            _logX: Double,
            _logBase: Double,
            eps: Double
        ) {
            val lnEval = LogarithmEvaluator()
            val logEval = GenericLogarithmEvaluator(lnEval)
            Assertions.assertEquals(y, logEval.log(x, base, eps), 10E-6)
        }
    }
}