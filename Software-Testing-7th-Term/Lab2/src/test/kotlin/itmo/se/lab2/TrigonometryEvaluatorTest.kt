package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource

class TrigonometryEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/sin-x.csv"], numLinesToSkip = 1)
    fun `it approximates sine`(x: Double, sin: Double, eps: Double) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/sin-invalid-x.csv"], numLinesToSkip = 1)
    fun `it returns NaN for invalid x`(x: Double, sin: Double, eps: Double) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), eps)
    }
}