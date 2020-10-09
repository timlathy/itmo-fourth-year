package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvFileSource

class TrigonometryEvaluatorTest {
    @ParameterizedTest
    @CsvFileSource(resources = ["/trig.csv"], numLinesToSkip = 1)
    fun `it approximates sine`(
        x: Double,
        sin: Double,
        _cos: Double,
        _tan: Double,
        _cot: Double,
        _sec: Double,
        eps: Double
    ) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), eps)
    }

    @ParameterizedTest
    @CsvFileSource(resources = ["/trig-invalid.csv"], numLinesToSkip = 1)
    fun `it returns NaN when called with invalid parameters`(
        x: Double,
        sin: Double,
        _cos: Double,
        _tan: Double,
        _cot: Double,
        _sec: Double,
        eps: Double
    ) {
        Assertions.assertEquals(sin, TrigonometryEvaluator().sin(x, eps), 1E-6)
    }
}