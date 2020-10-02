package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource
import org.mockito.ArgumentMatchers.*
import org.mockito.Mockito

class GenericLogarithmEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/log.csv"], numLinesToSkip = 1)
    fun `it computes logarithm of any base with mock ln evaluator`(y: Double, x: Double, base: Double, logX: Double, logBase: Double, eps: Double) {
        val mockLnEval = Mockito.mock(ILogarithmEvaluator::class.java)
        Mockito.`when`(mockLnEval.ln(eq(x), anyDouble())).thenReturn(logX)
        Mockito.`when`(mockLnEval.ln(eq(base), anyDouble())).thenReturn(logBase)

        val logEval = GenericLogarithmEvaluator(mockLnEval)
        Assertions.assertEquals(y, logEval.log(x, base, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/log.csv"], numLinesToSkip = 1)
    fun `it computes logarithm of any base with real ln evaluator`(y: Double, x: Double, base: Double, _logX: Double, _logBase: Double, eps: Double) {
        val lnEval = LogarithmEvaluator()
        val logEval = GenericLogarithmEvaluator(lnEval)
        Assertions.assertEquals(y, logEval.log(x, base, eps), eps)
    }
}