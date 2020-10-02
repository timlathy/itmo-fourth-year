package itmo.se.lab2.unit

import itmo.se.lab2.LogarithmEvaluator
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource

class LogarithmEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/ln-x-lt-1.csv"], numLinesToSkip = 1)
    fun `it approximates natural logarithm in the region x lt 1`(x: Double, ln: Double, eps: Double) {
        Assertions.assertEquals(ln, LogarithmEvaluator().ln(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/ln-x-gt-1.csv"], numLinesToSkip = 1)
    fun `it approximates natural logarithm in the region x gt 1`(x: Double, ln: Double, eps: Double) {
        Assertions.assertEquals(ln, LogarithmEvaluator().ln(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/ln-invalid-x.csv"], numLinesToSkip = 1)
    fun `it returns NaN for invalid x`(x: Double, ln: Double, eps: Double) {
        Assertions.assertEquals(ln, LogarithmEvaluator().ln(x, eps), eps)
    }
}