package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource

class LogarithmEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/ln-x.csv"], numLinesToSkip = 1)
    fun `it approximates natural logarithm`(x: Double, ln: Double, eps: Double) {
        Assertions.assertEquals(ln, LogarithmEvaluator().ln(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/ln-invalid-x.csv"], numLinesToSkip = 1)
    fun `it returns NaN for invalid x`(x: Double, ln: Double, eps: Double) {
        Assertions.assertEquals(ln, LogarithmEvaluator().ln(x, eps), eps)
    }
}