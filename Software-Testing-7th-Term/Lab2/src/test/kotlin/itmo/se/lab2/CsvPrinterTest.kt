package itmo.se.lab2

import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.io.TempDir
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.PrintStream
import java.nio.file.Path

class CsvPrinterTest {
    lateinit var outputPath: String
    lateinit var trigEvaluator: IGenericTrigonometryEvaluator
    lateinit var logEvaluator: IGenericLogarithmEvaluator
    lateinit var funEvaluator: EquationSystem

    @BeforeEach
    fun initTestDependencies(@TempDir tempDir: Path) {
        outputPath = tempDir.resolve("test").toString()
        trigEvaluator = GenericTrigonometryEvaluator(TrigonometryEvaluator())
        logEvaluator = GenericLogarithmEvaluator(LogarithmEvaluator())
        funEvaluator = EquationSystem(trigEvaluator, logEvaluator)
    }

    @Test
    fun `it computes sin(x)`() {
        main(arrayOf(outputPath, "sin", "-3.14159265", "0.5235987", "13"))
        val (sinHeader, sinValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,sin(x)", sinHeader)
        Assertions.assertEquals(13, sinValues.size)
        sinValues.forEach { Assertions.assertEquals(trigEvaluator.sin(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes tan(x)`() {
        main(arrayOf(outputPath, "tan", "-3.14159265", "0.5235987", "13"))
        val (tanHeader, tanValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,tan(x)", tanHeader)
        Assertions.assertEquals(13, tanValues.size)
        tanValues.forEach { Assertions.assertEquals(trigEvaluator.tan(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes cot(x)`() {
        main(arrayOf(outputPath, "cot", "-3.14159265", "0.5235987", "13"))
        val (cotHeader, cotValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,cot(x)", cotHeader)
        Assertions.assertEquals(13, cotValues.size)
        cotValues.forEach { Assertions.assertEquals(trigEvaluator.cot(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes sec(x)`() {
        main(arrayOf(outputPath, "sec", "-3.14159265", "0.5235987", "13"))
        val (secHeader, secValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,sec(x)", secHeader)
        Assertions.assertEquals(13, secValues.size)
        secValues.forEach { Assertions.assertEquals(trigEvaluator.sec(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes ln(x)`() {
        main(arrayOf(outputPath, "ln", "0.001", "0.5", "10"))
        val (logHeader, logValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,ln(x)", logHeader)
        Assertions.assertEquals(10, logValues.size)
        logValues.forEach { Assertions.assertEquals(LogarithmEvaluator().ln(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes log2(x)`() {
        main(arrayOf(outputPath, "log2", "0.001", "0.5", "10"))
        val (logHeader, logValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,log2(x)", logHeader)
        Assertions.assertEquals(10, logValues.size)
        logValues.forEach { Assertions.assertEquals(logEvaluator.log(it.first, 2.0,1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes log3(x)`() {
        main(arrayOf(outputPath, "log3", "0.001", "0.5", "10"))
        val (logHeader, logValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,log3(x)", logHeader)
        Assertions.assertEquals(10, logValues.size)
        logValues.forEach { Assertions.assertEquals(logEvaluator.log(it.first, 3.0,1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes log5(x)`() {
        main(arrayOf(outputPath, "log5", "0.001", "0.5", "10"))
        val (logHeader, logValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,log5(x)", logHeader)
        Assertions.assertEquals(10, logValues.size)
        logValues.forEach { Assertions.assertEquals(logEvaluator.log(it.first, 5.0,1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes log10(x)`() {
        main(arrayOf(outputPath, "log10", "0.001", "0.5", "10"))
        val (logHeader, logValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,log10(x)", logHeader)
        Assertions.assertEquals(10, logValues.size)
        logValues.forEach { Assertions.assertEquals(logEvaluator.log(it.first, 10.0,1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it computes f(x)`() {
        main(arrayOf(outputPath, "f", "-6.28318531", "1.57079633", "10"))
        val (fHeader, fValues) = collectCsvPrinterResults(outputPath)
        Assertions.assertEquals("x,f(x)", fHeader)
        Assertions.assertEquals(10, fValues.size)
        fValues.forEach { Assertions.assertEquals(funEvaluator.compute(it.first, 1E-8), it.second, 1E-8) }
    }

    @Test
    fun `it displays usage when invalid arguments are received`() {
        val usage = """
Usage: lab2 outfile function from step count
where outfile is the location of the output csv file
      function is one of: sin, tan, cot, sec, ln, log2, log3, log5, log10, f
      from is the starting value of x
      step is the increment of each x value
      count is the number of input values to use

""".trimIndent()

        // Invalid number of arguments
        Assertions.assertEquals(usage, captureCsvPrinterOutput(emptyArray()))
        Assertions.assertEquals(usage, captureCsvPrinterOutput(arrayOf(outputPath)))
        // Invalid function name
        Assertions.assertEquals(usage, captureCsvPrinterOutput(arrayOf(outputPath, "sinn", "0", "1", "1")))
        // Invalid count
        Assertions.assertEquals(usage, captureCsvPrinterOutput(arrayOf(outputPath, "sin", "0", "1", "0")))
    }

    private fun collectCsvPrinterResults(outputPath: String): Pair<String, List<Pair<Double, Double>>> {
        val lines = File(outputPath).readText().splitToSequence("\n").iterator()
        val header = lines.next()
        val values = lines.asSequence().map { it.split(",") }.map { Pair(it[0].toDouble(), it[1].toDouble()) }
        return Pair(header, values.toList())
    }

    private fun captureCsvPrinterOutput(args: Array<String>): String {
        val oldStdout = System.out

        val stdoutSink = ByteArrayOutputStream()
        System.setOut(PrintStream(stdoutSink))

        main(args)

        System.setOut(oldStdout)
        return stdoutSink.toString()
    }
}