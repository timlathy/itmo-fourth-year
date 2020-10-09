package itmo.se.lab2

import java.io.File
import kotlin.math.pow

const val precision = 8
val epsilon = 10.0.pow(-precision)

val availableFunctions = mapOf<String, (Double) -> Double>(
    "sin" to ({ x -> GenericTrigonometryEvaluator(TrigonometryEvaluator()).sin(x, epsilon) }),
    "tan" to ({ x -> GenericTrigonometryEvaluator(TrigonometryEvaluator()).tan(x, epsilon) }),
    "cot" to ({ x -> GenericTrigonometryEvaluator(TrigonometryEvaluator()).cot(x, epsilon) }),
    "sec" to ({ x -> GenericTrigonometryEvaluator(TrigonometryEvaluator()).sec(x, epsilon) }),
    "ln" to ({ x -> LogarithmEvaluator().ln(x, epsilon) }),
    "log2" to ({ x -> GenericLogarithmEvaluator(LogarithmEvaluator()).log(x, 2.0, epsilon) }),
    "log3" to ({ x -> GenericLogarithmEvaluator(LogarithmEvaluator()).log(x, 3.0, epsilon) }),
    "log5" to ({ x -> GenericLogarithmEvaluator(LogarithmEvaluator()).log(x, 5.0, epsilon) }),
    "log10" to ({ x -> GenericLogarithmEvaluator(LogarithmEvaluator()).log(x, 10.0, epsilon) }),
    "f" to ({ x ->
        EquationSystem(
            GenericTrigonometryEvaluator(TrigonometryEvaluator()),
            GenericLogarithmEvaluator(LogarithmEvaluator())
        ).compute(x, epsilon)
    }),
)

fun main(args: Array<String>) {
    val location = args.getOrNull(0)
    val functionName = args.getOrNull(1)
    val function = functionName?.let(availableFunctions::get)
    val from = args.getOrNull(2)?.toDoubleOrNull()?.takeIf { it.isFinite() }
    val step = args.getOrNull(3)?.toDoubleOrNull()?.takeIf { it.isFinite() }
    val count = args.getOrNull(4)?.toIntOrNull()?.takeIf { it > 0 }

    if (location == null || function == null || from == null || step == null || count == null) {
        println(
            "Usage: lab2 outfile function from step count\n" +
                    "where outfile is the location of the output csv file\n" +
                    "      function is one of: " + availableFunctions.keys.joinToString() + "\n" +
                    "      from is the starting value of x\n" +
                    "      step is the increment of each x value\n" +
                    "      count is the number of input values to use"
        )
        return
    }

    val xs = generateSequence(from) { (it + step).roundToPlaces(precision) }.take(count)
    val ys = xs.map(function)

    val csv = xs.zip(ys).joinToString("\n", prefix = "x,${functionName}(x)\n") { (x, y) -> "%.8f,%.8f".format(x, y) }

    File(location).writeText(csv)
}

fun Double.roundToPlaces(places: Int) = "%.${places}f".format(this).toDouble()