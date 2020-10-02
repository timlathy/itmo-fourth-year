package itmo.se.lab2

interface IGenericLogarithmEvaluator {
    fun log(x: Double, base: Double, eps: Double): Double
}

class GenericLogarithmEvaluator(private val lnEval: ILogarithmEvaluator) : IGenericLogarithmEvaluator {
    override fun log(x: Double, base: Double, eps: Double): Double =
        // `ln` needs to be approximated with a higher precision because the total error accumulates and we want to keep it under `eps`
        lnEval.ln(x, eps / 100) / lnEval.ln(base, eps / 100)
}