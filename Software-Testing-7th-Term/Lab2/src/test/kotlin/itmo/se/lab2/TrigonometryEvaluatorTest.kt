package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource

class TrigonometryEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/sin.csv"], numLinesToSkip = 1)
    fun `it approximates sine`(x: Double, sin: Double, eps: Double) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/sin-invalid.csv"], numLinesToSkip = 1)
    fun `it returns NaN when called with invalid parameters`(x: Double, sin: Double, eps: Double) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), 10E-6)
    }
}